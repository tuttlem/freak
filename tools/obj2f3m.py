#!/usr/bin/env python3
"""Convert a triangulated OBJ (v/vt/vn faces) into Freak's compact F3M1 mesh.

The runtime-facing layout is intentionally small: indexed vertices carry fixed
position/normal values and byte UVs.  Run: obj2f3m.py input.obj output.f3m
"""
import struct
import sys


def main(source, target):
    positions, texcoords, normals, vertices, indices, lookup = [], [], [], [], [], {}
    for line in open(source, encoding="utf8"):
        fields = line.split()
        if not fields:
            continue
        if fields[0] == "v": positions.append(tuple(map(float, fields[1:4])))
        elif fields[0] == "vt": texcoords.append(tuple(map(float, fields[1:3])))
        elif fields[0] == "vn": normals.append(tuple(map(float, fields[1:4])))
        elif fields[0] == "f":
            face = fields[1:]
            for tri in ((face[0], face[n], face[n + 1]) for n in range(1, len(face) - 1)):
                for ref in tri:
                    parts = (ref.split("/") + ["", ""])[:3]
                    key = tuple(int(value or 0) for value in parts)
                    if key not in lookup:
                        p = positions[key[0] - 1]; uv = texcoords[key[1] - 1] if key[1] else (0, 0)
                        normal = normals[key[2] - 1] if key[2] else (0, 0, 1)
                        lookup[key] = len(vertices)
                        vertices.append((*(round(value * 65536) for value in p), *(round(value * 65536) for value in normal), round(uv[0] * 255) & 255, round((1 - uv[1]) * 255) & 255))
                    indices.append(lookup[key])
    with open(target, "wb") as out:
        out.write(struct.pack("<IHHB", 0x314D3346, len(vertices), len(indices), 0))
        for vertex in vertices: out.write(struct.pack("<llllllBB", *vertex))
        out.write(struct.pack("<%dH" % len(indices), *indices))


if __name__ == "__main__":
    if len(sys.argv) != 3: raise SystemExit("usage: obj2f3m.py input.obj output.f3m")
    main(*sys.argv[1:])
