# Your First Rescue

This chapter runs the lab's rescue end to end. Everything
uses files, so you can reproduce it without endangering any
hardware: a "patient" whose contents are known to the last
byte, damage whose size and position are declared in advance,
and an accounting at the end that either balances or doesn't.

## The patient

`make-pattern.py` (in `ddrescue-lab/fixtures/`) builds a
64 MiB image as 64 one-megabyte regions. Every region is
filled with a repeating line that names the region, so the
contents are self-describing — and, more usefully, each
region has a known SHA-256 checksum, tabulated in the
transcript `t01-pattern.txt`. If a byte comes back wrong,
the checksums say where.

## The damage

A real failing drive cannot be summoned in a sandbox (nor
would the sandbox let us near a block device:
`t90-sandbox-limits.txt`). The lab's stand-in is
`fakedead.c`, a small preload shim: it watches reads of the
patient file and answers any read touching a "dead" range
with a genuine input/output error — the same failure a bad
sector reports. The ranges, declared before the rescue ran:

| Dead range | Size |
| --- | --- |
| MiB 10 | 1 MiB |
| 8 KiB at the start of MiB 31 | 8 KiB |
| MiB 50–52 | 3 MiB |
| **Total dead** | **4,202,496 bytes** |

The shim proved out before any rescue depended on it
(`t02-fakedead.txt`): a plain reader sails through the file,
the same reader under the shim stops at exactly byte
10,485,760 with `Input/output error`.

## The first pass

The classic first move, and the command this guide
recommends learning first:

```
ddrescue -n patient.img rescued.img rescue.map
```

`-n` (`--no-scrape`) keeps this pass fast and shallow: copy
everything readable, trim the failures to size, and leave
the digging for a later visit. The status display ddrescue
updates as it works:

```
     ipos:   55574 kB, non-trimmed:        0 B,  current rate:   5250 kB/s
     opos:   55574 kB, non-scraped:    4199 kB,  average rate:  31453 kB/s
non-tried:        0 B,  bad-sector:     3072 B,    error rate:   3508 kB/s
  rescued:   62906 kB,   bad areas:        6,        run time:          1s
pct rescued:   93.73%, read errors:       73
```

Read it as a story: 62,906 kB (93.73%) rescued; 73 read
errors along the way; what remains is 4,199 kB marked
*non-scraped* — failed areas awaiting the digging phase —
plus 3,072 bytes in six tiny *bad-sector* splinters. And
notice the run already **trimmed**: `-n` skips scraping
only, so each dead range has been cut to its true edges plus
a 512-byte bad splinter at each end (chapter 6). The
mapfile it left behind is the subject of the next chapter,
and the summary tool agrees with the display:

```
$ ddrescuelog -t rescue.map
     non-tried:        0 B,  in      0 area(s)
       rescued:   62906 kB,  in      4 area(s)  ( 93.74%)
   non-trimmed:        0 B,  in      0 area(s)
   non-scraped:    4199 kB,  in      3 area(s)  (  6.26%)
    bad-sector:     3072 B,  in      6 area(s)
```

One honesty note about scale: the lab's patient is 64 MiB
and the pass took about a second. None of that transfers to
a real drive, where this same moment — everything easy
banked, damage charted — can be hours in, and is still the
moment the rescue turns from emergency to bookkeeping.

## The second visit

Days later in a real rescue, when the drive has rested and
you can spare it the attention, the same command without
`-n` and with retries invited:

```
ddrescue -r2 patient.img rescued.img rescue.map
```

ddrescue reads the mapfile, sees that everything is already
tried, and goes straight to the unfinished business: it
scrapes the three non-scraped areas sector by sector
splitting them down, then retries every surviving bad
sector twice over. In the lab the damage is absolute — the
shim never relents — so this is the pessimistic case: the
run ground through 24,618 read errors and recovered *no*
additional bytes, ending at the same 93.74%. On a real drive
the same pass is where marginal sectors give up their data
one read in fifty; the lab's zero here is the shim's
character, not the phase's worth. What the pass did change
is the bookkeeping: the three damaged areas consolidated
into exactly three bad-sector blocks, whose positions and
sizes are precisely the dead ranges declared before the
rescue began (chapter 9 does the full accounting).

## Check

- Known patient, declared damage, two visits:
  `ddrescue -n` to bank the easy data, then plain `ddrescue`
  with `-r` to dig.
- The status block and `ddrescuelog -t` tell the same
  story: how much is rescued, and in what state the rest
  waits.
- A scraping pass that recovers nothing is a legitimate
  outcome against truly dead media; against real drives it
  is the pass that pays.
