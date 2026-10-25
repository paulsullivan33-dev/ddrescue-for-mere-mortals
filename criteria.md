# Writing Criteria

The standing quality bar for this guide. Every claim was
verified live where it says so; anything not run is labelled
documented-only, with the reason.

## 1. Verified live, with output

- GNU ddrescue and ddrescuelog 1.27 ran throughout (package
  `gddrescue 1.27-1`; in this sandbox the .deb's genuine
  binaries were unpacked and run, because the shared apt
  state could not complete a system install — recorded in
  transcript `t00-version.txt` and disclosed in chapter 3).
- A 64 MiB pattern patient with per-region SHA-256 checksums
  was built and tabulated (`t01-pattern.txt`).
- Damage was produced by an LD_PRELOAD shim returning real
  `EIO` errors for declared byte ranges, after the shim
  itself was proved against a plain reader
  (`t02-fakedead.txt`); the dead ranges totalled 4,202,496
  bytes at MiB 10 (1 MiB), MiB 31 (8 KiB) and MiB 50–52
  (3 MiB).
- First pass (`ddrescue -n`): 62,906,368 bytes rescued
  (93.74%), 73 read errors, mapfile states captured and
  quoted (`t03-first-pass.txt`); trimming observed to run
  under `-n`.
- Interruption and resume: a 128 MiB rescue stopped by
  SIGINT at 19.04% resumed from `ipos` 25,559 kB, re-reading
  nothing, and finished with the dead megabyte as one
  1,048,576-byte bad-sector block (`t04-resume.txt`).
- Second pass with scraping and `-r2`: phases consolidated
  the damage into exactly three bad-sector blocks of the
  declared sizes; 24,618 cumulative read errors against
  permanently dead ranges; no bytes recovered — the
  pessimistic case, reported as such (`t05-second-pass.txt`).
- Final accounting by an independent block-comparison
  script: 62,906,368 bytes recovered, 4,202,496 lost, the
  lost ranges byte-identical to the declared damage; region
  checksums differ only in regions 10, 31 and 50–52
  (`t06-accounting.txt`).
- ddrescuelog on the real mapfiles: `-t` summaries, `-l`
  bad-block lists at two block sizes (MiB-granular answer
  10, 31, 50, 51, 52), `-A` annotation, `-D` exit status 1
  on a finished-but-lossy rescue and 0 on a perfect one,
  `-d` deleting only a perfect rescue's mapfile
  (`t07-ddrescuelog.txt`).
- Merging two damaged copies (damage at different offsets)
  into one byte-perfect file, including the discovery that a
  plain second run does not reopen bad-sector blocks and a
  `-r1` retry does (`t08-merging.txt`).
- The same-file refusal error, produced on purpose
  (`t09-errors.txt`).

## 2. Documented with a named constraint

- Behaviour of physically failing hardware (genuine bad
  sectors, drive-level retries and timeouts, heat, sounds,
  devices dropping off the bus) is documented-only: no
  failing drive existed in the lab, and the file-based
  stand-in is described as a stand-in wherever it is used.
- The sandbox forbids block-device work entirely —
  `mknod` of loop and device-mapper nodes returns
  "Operation not permitted", `losetup` finds no loop device,
  `/dev/fuse` is absent (`t90-sandbox-limits.txt`) — so
  `-d` direct I/O, mounting images, and device-as-outfile
  (`-f`) are documented-only, chapter 10.
- The Debian/Ubuntu package-name distinction (`gddrescue`
  vs the unrelated `ddrescue` package) is stated from the
  distributions' package descriptions; the gddrescue side
  of it was exercised directly, the `ddrescue`-package side
  is the documented trap, guarded by the `--version` banner
  check the chapter teaches.
- ddrescue's fill and generate modes are named but not
  worked; the guide's spine is the rescue workflow.

## 3. Output reproduced

- Versions, the pattern table, every rescue run, every
  mapfile state quoted, the accounting, the ddrescuelog
  operations and the induced error are captured in
  `ddrescue-lab/transcripts/` exactly as they ran, and the
  fixtures used (`make-pattern.py`, `fakedead.c`,
  `compare-blocks.py`) are kept in `ddrescue-lab/fixtures/`
  so the lab can be re-run.
- One lab false start is part of the record: the first
  version of the damage shim was bypassed by ddrescue's
  fortified `__open64_2` entry point, producing perfect
  "rescues" of a dead file until the shim's proof-test
  caught it. It is written up in chapter 14 as the general
  warning it deserves to be.

## 4. Accuracy

- Product facts (phases, mapfile characters, option
  meanings) agree with the ddrescue 1.27 manual shipped in
  the package; every quoted behaviour was also observed in
  the lab where the lab could reach it.
- Byte arithmetic in the guide is the lab's own measured
  output: 62,906,368 + 4,202,496 = 67,108,864, and
  ddrescuelog's totals matched the independent comparison
  exactly.

## 5. Completeness

- The whole arc is covered: installing (including the name
  trap), the three-file command, a worked rescue, the
  mapfile, each phase, stopping and resuming, merging
  copies, the final accounting, real-device rules, recipes,
  neighbours, limits, troubleshooting, glossary.

## 6. Accessibility

- Every command was run as shown; no GUI is involved.
- Status output is quoted in small, readable blocks, and
  the mapfile is taught character by character before its
  excerpts are used.

## 7. Structure

- Chapters follow the series layout: what it is, moving
  parts, installing, the worked core, deep-dives, recipes,
  neighbours, limits, troubleshooting, glossary, criteria.

## 8. Voice

- Second person, plain, unhurried — written for a reader
  whose drive may be dying as they read, which argues for
  calm and for short paths to the commands.

## 9. Honesty

- Simulated damage is called simulated; the one environment
  where behaviour is reasoned rather than run (real failing
  hardware) is named in the chapters that discuss it.
- The pessimistic scraping result (zero extra bytes) is
  printed, not hidden; so is the lab's own shim false
  start.
