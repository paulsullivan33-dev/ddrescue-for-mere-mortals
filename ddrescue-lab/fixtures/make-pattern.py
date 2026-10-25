#!/usr/bin/env python3
"""make-pattern.py -- build the lab's checkable "disk" image.

Usage: make-pattern.py <output-file> <regions>

Writes an image of <regions> blocks of 1 MiB each. Every block
(region) is filled with one repeating 96-byte text line naming
its region number, so the content of every region is known and
checkable, and so is every copy of it: sha256 of a region of a
perfect copy must equal sha256 of the same region here.

The lab uses this file twice over: once as the pristine
original the damage is measured against, and once (through the
fakedead shim) as the "patient" ddrescue rescues.
"""
import hashlib
import struct
import sys

REGION = 1024 * 1024  # 1 MiB


def region_bytes(i, n):
    line = ("ddrescue lab pattern -- region %04d of %04d -- "
            "the rain in spain falls mainly on the plain --\n"
            % (i, n)).encode()
    reps = REGION // len(line) + 1
    return (line * reps)[:REGION]


def main():
    out, n = sys.argv[1], int(sys.argv[2])
    with open(out, "wb") as f:
        for i in range(n):
            f.write(region_bytes(i, n))
    print("wrote %s: %d regions, %d bytes" % (out, n, n * REGION))


if __name__ == "__main__":
    main()
