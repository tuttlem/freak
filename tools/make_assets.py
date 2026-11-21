#!/usr/bin/env python3
"""Create deterministic source art and convert it into the demo asset pack."""
import math
import pathlib
import subprocess
import sys
import struct
import wave

from PIL import Image, ImageDraw

ROOT = pathlib.Path(__file__).resolve().parent.parent
ASSETS = ROOT / "test" / "assets"
TOOL = ROOT / "tools" / "freak_asset.py"

def convert(*args):
    subprocess.check_call([sys.executable, str(TOOL), *map(str, args)])

def main():
    ASSETS.mkdir(exist_ok=True)
    image = Image.new("RGBA", (96, 56))
    draw = ImageDraw.Draw(image)
    for y in range(image.height):
        for x in range(image.width): image.putpixel((x,y),(x*2,20+y*2,80+(x+y)//4,255))
    draw.ellipse((28,8,68,48), fill=(255,210,30,255)); draw.ellipse((38,18,58,38), fill=(0,0,0,0))
    image.save(ASSETS / "showcase.png")
    sprites = Image.new("RGBA", (32, 16), (0,0,0,0)); d=ImageDraw.Draw(sprites)
    d.rectangle((2,3,12,14), fill=(40,230,255,255)); d.rectangle((18,1,29,14), fill=(255,100,35,255))
    sprites.save(ASSETS / "sprites.png")
    (ASSETS / "ui.bdf").write_text("""STARTFONT 2.1\nFONTBOUNDINGBOX 8 8 0 0\nCHARS 1\nSTARTCHAR A\nENCODING 65\nBITMAP\n18\n24\n42\n7E\n42\n42\n42\n00\nENDCHAR\nENDFONT\n""")
    with wave.open(str(ASSETS / "ping.wav"), "wb") as out:
        out.setnchannels(1); out.setsampwidth(1); out.setframerate(11025)
        out.writeframes(bytes(int(128+70*math.sin(i*2*math.pi*660/11025)) for i in range(2205)))
    (ASSETS / "cube.obj").write_text("""v -1 -1 -1\nv 1 -1 -1\nv 1 1 -1\nv -1 1 -1\nv -1 -1 1\nv 1 -1 1\nv 1 1 1\nv -1 1 1\nf 1 2 3 4\nf 5 8 7 6\nf 1 5 6 2\nf 2 6 7 3\nf 3 7 8 4\nf 5 1 4 8\n""")
    (ASSETS / "ping.imf").write_bytes(bytes((0x20,1,1,0)))
    (ASSETS / "beat.ftr").write_bytes(b"FTR1" + bytes((1,0,1,6,1,60,1,48,0)))
    midi_track = b"\x00\x90\x3c\x40\x60\x80\x3c\x00\x00\xff\x2f\x00"
    (ASSETS / "beat.mid").write_bytes(b"MThd" + struct.pack(">IHHH",6,0,1,96) + b"MTrk" + struct.pack(">I",len(midi_track)) + midi_track)
    (ASSETS / "level.csv").write_text("0,1,2,3\n3,2,1,0\n")
    convert("image", ASSETS/"showcase.png", ASSETS/"showcase.fim")
    convert("sprite", ASSETS/"sprites.png", ASSETS/"sprites.fsp", "--frame", "16x16")
    convert("palette", ASSETS/"showcase.png", ASSETS/"showcase.fpl")
    convert("font", ASSETS/"ui.bdf", ASSETS/"ui.ffn")
    convert("sound", ASSETS/"ping.wav", ASSETS/"ping.fsb", "--name", "PING")
    convert("model", ASSETS/"cube.obj", ASSETS/"cube.f3m")
    convert("music", ASSETS/"ping.imf", ASSETS/"ping.imf", "--kind", "imf")
    convert("music", ASSETS/"beat.ftr", ASSETS/"beat.ftr", "--kind", "tracker")
    convert("music", ASSETS/"beat.mid", ASSETS/"midi.ftr", "--kind", "midi")
    convert("map", ASSETS/"level.csv", ASSETS/"level.fmp", "--tile-width", "16", "--tile-height", "16")

if __name__ == "__main__": main()
