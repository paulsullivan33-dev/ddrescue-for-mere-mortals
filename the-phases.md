# The Phases in Depth

Chapter 4 ran the phases as one workflow. This chapter takes
them one at a time, with the lab's mapfiles as the evidence
for what each one does to a block.

## Copying: the sweep

Copying reads the non-tried areas in large blocks. When a
read fails, ddrescue does not stop, retry, or dwell: it
skips ahead a cautious distance and keeps sweeping, noting
the failure for later phases, and after the forwards pass it
sweeps backwards too — damage sometimes reads better from
the other side. This is the phase that banks the overwhelming
majority of every rescue (93.74% of the lab's patient), and
its governing virtue is *haste without pressure*: the drive
is asked for easy data only.

Two knobs matter here. `--max-read-rate` throttles the sweep
(for a drive in obvious distress, chapter 10; the lab used
it to stage chapter 7's interruption). And the cluster size
(`-c`) sets the sweep's block size — larger is faster on
healthy stretches, smaller wastes less around damage; the
defaults are sane.

## Trimming: finding the edges

A failed copy block says "somewhere in this span, reading
broke." Trimming reads inwards from both ends of the span,
sector by sector, to find the last readable byte at each
side. Whatever survives becomes `+`; the shrunken failed
core becomes non-scraped (`/`), with a bad-sector splinter
(`-`) where the probing itself bottomed out — in the lab,
exactly 512 bytes at each edge of every dead range, visible
in the post-trim mapfile of chapter 5.

Trimming runs even on a `-n` first pass: `-n` is
`--no-scrape`, not `--no-trim`. That is why a single quick
command left the lab's mapfile with no `*` (non-trimmed)
blocks at all — the edges were already exact, and only the
interiors waited.

## Scraping: the digging

Scraping takes each non-scraped block and divides it:
read halves, then quarters, smaller and smaller, rescuing
every readable sector it can isolate and shrinking the
unreadable residue. On a real drive this is where most of
the *remaining* wins live — bad areas are usually freckled
with good sectors, and scraping is the systematic freckle
hunt. Against the lab's shim, whose dead ranges are dead to
the last byte, scraping could only subdivide and confirm:
it converted 4,199,424 bytes of `/` into part of three
final `-` blocks of exactly the declared sizes. Zero bytes
recovered, zero bytes wrongly kept — the pessimistic case,
run honestly.

## Retrying: the asking-again

Last, and only if asked with `-r N`, ddrescue re-reads each
bad sector, one at a time, up to N further passes,
alternating direction. On real media a sector that failed
an hour ago sometimes reads today — marginal magnetism,
temperature, luck — and retries are your claim on that luck.
They are also the most wearing phase, sector-by-sector
persistence against the sickest part of the drive, which is
why they come last, after everything else is safe, and why
the count defaults to zero. The lab's second visit used
`-r2`: 24,618 cumulative read errors across scraping and
both retry passes, and the final `-` blocks did not move.
Chapter 8 shows the other face of `-r`: pointed at a
*second copy*, the retry pass is the mechanism that merges
two damaged sources into one whole file.

## Putting the phases in their order

The workflow writes itself once the phases are understood:

1. `ddrescue -n` — copy and trim; bank everything easy
   while the drive is at its best.
2. `ddrescue` — copy anything left, then scrape; the long,
   careful dig.
3. `ddrescue -rN` — retry passes, as many as the drive's
   condition and your patience justify, safe in the
   knowledge that stopping loses nothing.

Each step is the same command modulo options, because the
mapfile — not your memory — tracks what remains to do.

## Check

- Copying banks the bulk, fast and shallow; trimming finds
  the exact edges of damage; scraping subdivides to rescue
  freckles of good data; retrying asks the worst sectors
  again, last and only on request.
- `-n` skips scraping only — a first pass still trims,
  which the lab's mapfile shows line by line.
- The phases are a queue stored in the mapfile. Order your
  commands from gentlest to most persistent and nothing is
  ever repeated or lost.
