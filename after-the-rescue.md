# After the Rescue

The rescue ends with three things: an image, a mapfile, and
a number. Attend to the number first, while the story is
fresh.

## The accounting

ddrescue's own summary of the lab's finished rescue:

```
$ ddrescuelog -t rescue.map
     non-tried:        0 B,  in      0 area(s)
       rescued:   62906 kB,  in      4 area(s)  ( 93.74%)
   non-trimmed:        0 B,  in      0 area(s)
   non-scraped:        0 B,  in      0 area(s)
    bad-sector:    4202 kB,  in      3 area(s)  (  6.26%)
```

Because the lab knows its original, it could audit that
summary with an independent block-by-block comparison
(`compare-blocks.py`, `t06-accounting.txt`):

```
blocks compared:   16384 (67108864 bytes)
blocks identical:  15358 (62906368 bytes recovered)
blocks differing:  1026 (4202496 bytes lost)
  lost blocks 2560-2815  (bytes 10485760-11534335)
  lost blocks 7936-7937  (bytes 32505856-32514047)
  lost blocks 12800-13567 (bytes 52428800-55574527)
```

The audit balances to the byte: 62,906,368 bytes recovered,
4,202,496 lost, the lost ranges identical to the damage
declared before the rescue ran, and a checksum pass over all
64 regions names only regions 10, 31 and 50–52 as differing.
That is the standard to hold any rescue to, adapted to what
you know: you will rarely have the original to compare
against (having it would rather defeat the purpose), but you
always have the mapfile's account of *where* the holes are —
and the holes, not the percentages, are what determine what
can be salvaged next.

## Where the holes fall decides everything

A lost megabyte inside a video file is a glitch; the same
megabyte across a filesystem's metadata is a catastrophe;
spread as 512-byte splinters it may be neither. Before
touching the image, locate the damage:

```
$ ddrescuelog -l - -b 1048576 rescue.map
10
31
50
51
52
```

…and the `-i`/`-o` domain options and `--block-size` let you
ask the same question at whatever granularity your next
tool needs. The mapfile's geometry is the triage: image
first, *then* decide how much repair the patient is worth.

## Working with the image

Rules that apply however the rescue went:

- **Keep the rescued image pristine.** Copy it, and do all
  repair work on the copy. If a repair goes wrong, the
  rescue is an expensive thing to redo — the source may no
  longer exist in readable form at all.
- **Never mount the image read-write first.** Mount a copy,
  read-only, and see what the filesystem says; tools like
  `fsck` belong on copies too, with their own backups.
- **Know what a hole looks like.** Unrescued areas in the
  image are zeros (or a fill pattern, if you chose one with
  the fill options). A file that opens but contains a run of
  zeros mid-stream is showing you a hole — correlate it
  with the mapfile before blaming the file.
- **Finish the paperwork.** `ddrescuelog -d` deletes the
  mapfile when — and only when — the rescue is complete in
  the strict sense (every byte recovered; verified in the
  lab on a perfect small rescue, and verified *refusing* on
  the lab's lossy one). Until then the mapfile stays with
  the image: together they are the provenance of every byte.
- If the losses matter, file-level recovery tools that read
  the mapfile can concentrate on the files that intersect
  the holes instead of the whole disk — the neighbours of
  chapter 12.

## Check

- Do the accounting while the rescue's story is fresh:
  totals from `ddrescuelog -t`, hole locations from
  `ddrescuelog -l`.
- In the lab the books balanced exactly — recovered plus
  lost equals the patient, and the lost ranges are the
  declared damage.
- The image is evidence: repair and mount copies of it,
  and keep its mapfile beside it until the recovery is
  truly done.
