# Glossary

**Bad area** — A region of the input containing one or more
failed blocks; ddrescue's status display counts them
separately from bad sectors.

**Bad sector** — The smallest unit the accounting gives up
on individually: a block that failed copying, trimming,
scraping, and whatever retries were requested. Marked `-`
in the mapfile.

**Cluster** — The block size of the copying phase: how much
ddrescue asks for per read while sweeping.

**Copying** — The first phase: large-block reads of the
non-tried areas, forwards then backwards, skipping failures.

**ddrescuelog** — The companion tool that reports on,
converts, and manages mapfiles without touching data.

**Domain** — The part of the input a run concerns itself
with, set by size and position options or by the mapfile's
extent.

**EIO** — "Input/output error": the error a read returns
when the device (or, in the lab, the fakedead shim) cannot
deliver a block.

**Finished block (`+`)** — A block whose contents are
safely in the outfile.

**Image** — The outfile in its usual form: a full-size,
byte-positional picture of the input, holes and all.

**Mapfile** — The small text file recording the state of
every block of the input and the position of the current
pass; the rescue's memory and the price of admission for
resume and merging.

**Non-scraped (`/`)** — A failed block whose edges are known
(trimmed) but whose interior has not yet been subdivided by
scraping.

**Non-trimmed (`*`)** — A failed block whose true edges have
not yet been found by trimming.

**Non-tried (`?`)** — A block no phase has read yet.

**Patient** — This guide's word for the infile: the failing
drive, partition, or file being rescued from.

**Retrying** — The optional last phase: re-reading bad
sectors individually, up to the `-r` count of extra passes.

**Scraping** — The phase that subdivides failed blocks into
ever-smaller pieces to isolate and rescue the good sectors
among the bad.

**Sector** — The device's own block unit, 512 bytes in the
lab's accounting and on classic drives, 4096 on many modern
ones; ddrescue's smallest stage of work.

**Trimming** — The phase that reads inwards from a failed
block's edges to find exactly where readability ends.
