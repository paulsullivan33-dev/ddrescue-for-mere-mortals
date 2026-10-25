# Two Copies Are Better Than One

Damage is rarely in the same place twice. Two backups of the
same disk, two burns of the same disc, a dying original and
a partial copy made years ago — each may be unreadable
somewhere different. ddrescue's mapfile turns that luck into
a complete file: rescue each copy into the *same* outfile
with the *same* mapfile, and the holes of one are filled
from the substance of the other. The lab verified the whole
dance, including the step that is not obvious.

## The lab's two patients

Copy A: the 64 MiB patient, dead at MiB 10 only.
Copy B: the same patient, dead at MiB 50–52 only.
Neither can produce a whole file alone.

Copy A's rescue, into a fresh `merged.img` and
`merged.map`:

```
$ ddrescue patientA.img merged.img merged.map
pct rescued:   98.43%
0x00000000  0x00A00000  +
0x00A00000  0x00100000  -
0x00B00000  0x03500000  +
```

One hole: exactly MiB 10, marked bad-sector.

## The non-obvious step

Now copy B's rescue, same outfile, same mapfile. A plain
run changes **nothing** — the lab measured zero read errors
and the same 98.43%. The reason is principled: a block
marked `-` is a *settled verdict*, and the copying phase
only visits blocks still in play. Pointing a fresh source
at the mapfile does not reopen settled blocks on its own.

What reopens them is the retry pass:

```
$ ddrescue -r1 patientB.img merged.img merged.map
pct rescued:  100.00%
0x00000000  0x04000000  +
```

The retry phase re-read the bad megabyte — from copy B,
where MiB 10 is perfectly healthy — and the mapfile
collapsed to a single finished block the size of the whole
patient. The independent accounting
(`t08-merging.txt`):

```
blocks compared:   16384 (67108864 bytes)
blocks identical:  16384 (67108864 bytes recovered)
blocks differing:  0 (0 bytes lost)
```

Two incomplete sources, one byte-perfect file, and the
checksums confirm it is not bravado: every one of the
16,384 compared blocks matches the known original.

## Rules for merging

- The copies must be copies **of the same source**, same
  size, sector-aligned the same way. Merging strangers
  produces a chimera that checksums will convict.
- Same outfile, same mapfile, every run. The mapfile is the
  merge's memory.
- Expect to need `-r` when a later copy must fill blocks an
  earlier copy already condemned; blocks merely untried or
  unscraped are picked up by an ordinary run.
- After each source, re-check the tally
  (`ddrescuelog -t`). When `-D` finally exits 0, the merge
  is not just finished — it is *complete*, every byte
  accounted for.

## Check

- Same destination image + same mapfile + several damaged
  copies = one merged rescue.
- Bad-sector blocks are settled; a retry pass (`-r`) is
  what asks the next copy about them.
- The lab's merge ended at 100.00% with a block-by-block
  comparison against the original showing zero differences.
