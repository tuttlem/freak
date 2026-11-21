#!/usr/bin/env python3
"""Host-side importers for Freak's compact DOS assets.

Requires Pillow for image/sprite/palette imports; all other commands use the
Python standard library.  Run ``python3 tools/freak_asset.py --help``.
"""
import argparse
import csv
import pathlib
import struct
import sys
import wave


def pillow():
    try:
        from PIL import Image
        return Image
    except ImportError as error:
        raise SystemExit("image import needs Pillow: python3 -m pip install Pillow") from error


def u16(value):
    return struct.pack("<H", value)


def palette_bytes(palette):
    values = palette[:768] + [0] * max(0, 768 - len(palette[:768]))
    return bytes(value * 63 // 255 for value in values)


def indexed(source, transparent):
    Image = pillow()
    image = Image.open(source).convert("RGBA")
    alpha = image.getchannel("A")
    rgb = image.convert("RGB")
    colours = 255 if transparent is not None else 256
    result = rgb.quantize(colors=colours, method=Image.Quantize.MEDIANCUT)
    pixels = bytearray(result.tobytes())
    pal = result.getpalette()[:colours * 3]
    if transparent is not None:
        # Shift the quantised colours so the requested key is guaranteed free.
        key = transparent
        for i, value in enumerate(pixels):
            pixels[i] = value + 1 if value >= key else value
        expanded = [0] * 768
        for index in range(colours):
            target = index + 1 if index >= key else index
            expanded[target * 3:target * 3 + 3] = pal[index * 3:index * 3 + 3]
        for i, value in enumerate(alpha.tobytes()):
            if value < 128:
                pixels[i] = key
        pal = expanded
    return image.width, image.height, bytes(pixels), palette_bytes(pal)


def image_command(args):
    w, h, pixels, pal = indexed(args.source, args.transparent)
    with open(args.target, "wb") as out:
        out.write(b"FIM1" + u16(w) + u16(h) + bytes([args.transparent, 0]))
        out.write(pal + pixels)


def sprite_command(args):
    w, h, pixels, pal = indexed(args.source, args.transparent)
    fw, fh = map(int, args.frame.lower().split("x"))
    if not fw or not fh or w % fw or h % fh:
        raise SystemExit("sheet dimensions must be an exact multiple of --frame")
    frames = []
    for row in range(h // fh):
        for col in range(w // fw):
            frame = bytearray()
            for y in range(fh):
                start = (row * fh + y) * w + col * fw
                frame.extend(pixels[start:start + fw])
            frames.append(frame)
    with open(args.target, "wb") as out:
        out.write(b"FSP1" + u16(fw) + u16(fh) + u16(len(frames)) +
                  bytes([args.transparent, 0]))
        out.write(pal + b"".join(frames))


def palette_command(args):
    _, _, _, pal = indexed(args.source, None)
    with open(args.target, "wb") as out:
        out.write(b"FPL1" + pal)


def font_command(args):
    glyphs, current, width, height = {}, None, None, None
    for raw in open(args.source, encoding="ascii", errors="replace"):
        words = raw.split()
        if not words: continue
        if words[0] == "FONTBOUNDINGBOX": width, height = map(int, words[1:3])
        elif words[0] == "STARTCHAR": current = {"rows": []}
        elif words[0] == "ENCODING" and current is not None: current["code"] = int(words[1])
        elif words[0] == "BITMAP" and current is not None: current["bitmap"] = True
        elif words[0] == "ENDCHAR" and current is not None:
            if 0 <= current.get("code", -1) < 256: glyphs[current["code"]] = current["rows"]
            current = None
        elif current is not None and current.get("bitmap"):
            try: current["rows"].append(int(words[0], 16))
            except ValueError: pass
    if not width or not height or width > 8 or height > 255 or not glyphs:
        raise SystemExit("BDF must contain 1..8 pixel wide glyphs")
    first, last = min(glyphs), max(glyphs)
    data = bytearray()
    for code in range(first, last + 1):
        rows = glyphs.get(code, [])
        data.extend((row << max(0, 8 - width)) & 255 for row in rows[:height])
        data.extend(b"\0" * max(0, height - len(rows)))
    with open(args.target, "wb") as out:
        out.write(b"FFN1" + bytes([width, height, first, last - first + 1]) + data)


def sound_command(args):
    with wave.open(args.source, "rb") as source:
        channels, width, rate, count = source.getnchannels(), source.getsampwidth(), source.getframerate(), source.getnframes()
        if source.getcomptype() != "NONE" or width not in (1, 2): raise SystemExit("only PCM 8/16-bit WAV is supported")
        raw = source.readframes(count)
    samples = bytearray()
    for frame in range(count):
        values = []
        for channel in range(channels):
            offset = (frame * channels + channel) * width
            value = raw[offset] - 128 if width == 1 else struct.unpack_from("<h", raw, offset)[0] >> 8
            values.append(value)
        samples.append(sum(values) // len(values) & 255)
    name = args.name.upper().encode("ascii", "replace")[:12].ljust(13, b"\0")
    with open(args.target, "wb") as out:
        out.write(b"FSBK" + u16(1) + name + u16(rate) + struct.pack("<III", len(samples), 0, len(samples)) + b"\0" + samples)


def model_command(args):
    from obj2f3m import main
    main(args.source, args.target)


def map_command(args):
    rows = [[int(value, 0) for value in row] for row in csv.reader(open(args.source)) if row]
    if not rows or not rows[0] or any(len(row) != len(rows[0]) for row in rows):
        raise SystemExit("CSV map must be a non-empty rectangle")
    if len(rows) > 65535 or len(rows[0]) > 65535:
        raise SystemExit("map is too large")
    with open(args.target, "wb") as out:
        out.write(b"FMP1" + u16(len(rows[0])) + u16(len(rows)) +
                  u16(args.tile_width) + u16(args.tile_height) + bytes((1, 0)))
        for row in rows:
            for tile in row:
                if tile < 0 or tile > 65535: raise SystemExit("tile IDs must be 0..65535")
                out.write(u16(tile))


def music_command(args):
    # IMF and FTR1 are already Freak-native streams.  Copying through this
    # command makes them first-class entries in an asset manifest/build.
    if args.kind == "midi":
        return midi_to_tracker(args.source, args.target, args.channels)
    data = pathlib.Path(args.source).read_bytes()
    expected = b"FTR1" if args.kind == "tracker" else None
    if expected and not data.startswith(expected): raise SystemExit("tracker input must be FTR1")
    pathlib.Path(args.target).write_bytes(data)


def vlq(data, position):
    value = 0
    while True:
        value = (value << 7) | (data[position] & 127); more = data[position] & 128; position += 1
        if not more: return value, position


def midi_to_tracker(source, target, channels):
    """Turn note-on events in a type-0/1 SMF into a compact FTR1 pattern.
    Tempo is intentionally normalised: four tracker rows per MIDI quarter note.
    """
    data = pathlib.Path(source).read_bytes()
    if data[:4] != b"MThd" or len(data) < 14: raise SystemExit("expected a Standard MIDI file")
    _, tracks, division = struct.unpack_from(">HHH", data, 8)
    if not division or division & 0x8000: raise SystemExit("SMPTE-timed MIDI is unsupported")
    position, notes = 14, []
    for _ in range(tracks):
        if data[position:position+4] != b"MTrk": raise SystemExit("invalid MIDI track")
        length = struct.unpack_from(">I", data, position+4)[0]; end = position + 8 + length
        position += 8; tick = 0; running = 0
        while position < end:
            delta, position = vlq(data, position); tick += delta; status = data[position]
            if status < 128: status = running
            else: position += 1; running = status
            if status == 0xFF:
                kind = data[position]; size, position = vlq(data, position+1); position += size
            elif status in (0xF0, 0xF7):
                size, position = vlq(data, position); position += size
            else:
                command = status & 0xF0; channel = status & 15
                first = data[position]; position += 1
                second = None if command in (0xC0,0xD0) else data[position]
                if second is not None: position += 1
                if command == 0x90 and second:
                    notes.append((tick * 4 // division, channel % channels, first))
        position = end
    rows = max([row for row, _, _ in notes] + [0]) + 1
    if rows > 65535: raise SystemExit("MIDI is too long for one FTR1 pattern")
    cells = bytearray(rows * channels * 4)
    for row, channel, note in notes:
        offset = (row * channels + channel) * 4
        cells[offset:offset+4] = bytes((note, 1, 48, 0))
    pathlib.Path(target).write_bytes(b"FTR1" + struct.pack("<HBBB", rows, channels, 6, 1) + cells)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="command", required=True)
    for name, func in (("image", image_command), ("sprite", sprite_command), ("palette", palette_command)):
        p = sub.add_parser(name); p.add_argument("source"); p.add_argument("target"); p.set_defaults(func=func)
        if name != "palette": p.add_argument("--transparent", type=int, default=0, choices=range(256))
    p = sub.choices["sprite"]; p.add_argument("--frame", required=True, metavar="WxH")
    p = sub.add_parser("font"); p.add_argument("source"); p.add_argument("target"); p.set_defaults(func=font_command)
    p = sub.add_parser("sound"); p.add_argument("source"); p.add_argument("target"); p.add_argument("--name", required=True); p.set_defaults(func=sound_command)
    p = sub.add_parser("model"); p.add_argument("source"); p.add_argument("target"); p.set_defaults(func=model_command)
    p = sub.add_parser("map"); p.add_argument("source"); p.add_argument("target"); p.add_argument("--tile-width", type=int, required=True); p.add_argument("--tile-height", type=int, required=True); p.set_defaults(func=map_command)
    p = sub.add_parser("music"); p.add_argument("source"); p.add_argument("target"); p.add_argument("--kind", choices=("imf", "tracker", "midi"), default="imf"); p.add_argument("--channels", type=int, default=4, choices=range(1,9)); p.set_defaults(func=music_command)
    args = parser.parse_args(); args.func(args)


if __name__ == "__main__": main()
