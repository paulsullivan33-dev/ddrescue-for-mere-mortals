#!/usr/bin/env python3
"""compare-blocks.py -- account for a rescue, block by block.

Usage: compare-blocks.py <original> <rescued> [blocksize]

Walks both files in blocks (default 4096 bytes) and reports how
many blocks are byte-identical (recovered) and how many differ
or are missing (lost), plus the list of differing block ranges.
This is the lab's final accounting: no trust in the rescuer's
own report, just the bytes.
"""
import hashlib
import sys


def blocks(path, bs):
    with open(path, "rb") as f:
        while True:
            b = f.read(bs)
            if not b:
                return
            yield b


def main():
    orig, resc = sys.argv[1], sys.argv[2]
    bs = int(sys.argv[3]) if len(sys.argv) > 3 else 4096
    same = diff = 0
    diff_ranges = []
    start = None
    last = -1
    for i, (a, b) in enumerate(zip(blocks(orig, bs), blocks(resc, bs))):
        last = i
        if hashlib.sha256(a).digest() == hashlib.sha256(b).digest():
            same += 1
            if start is not None:
                diff_ranges.append((start, i - 1))
                start = None
        else:
            diff += 1
            if start is None:
                start = i
    if start is not None:
        diff_ranges.append((start, last))
    total = same + diff
    print("block size:        %d bytes" % bs)
    print("blocks compared:   %d (%d bytes)" % (total, total * bs))
    print("blocks identical:  %d (%d bytes recovered)" % (same, same * bs))
    print("blocks differing:  %d (%d bytes lost)" % (diff, diff * bs))
    for lo, hi in diff_ranges:
        print("  lost blocks %d-%d  (bytes %d-%d)"
              % (lo, hi, lo * bs, (hi + 1) * bs - 1))


if __name__ == "__main__":
    main()
