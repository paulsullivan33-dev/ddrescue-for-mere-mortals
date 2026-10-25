# The Mapfile

The mapfile is a plain-text file you are expected to look
at. This is the lab's, after the first pass (chapter 4),
with the header comments removed:

```
0x03500000     +               1
0x00000000  0x00A00000  +
0x00A00000  0x00000200  -
0x00A00200  0x000FFC00  /
0x00AFFE00  0x00000200  -
0x00B00000  0x01400000  +
...
```

## The status line

The first line is the rescue's present tense: the current
position in the input (hexadecimal), what the running pass
is doing there, and the pass number. Here: position
0x03500000, status `+` meaning *finished*, pass 1. The
possible statuses for this line:

| Char | Meaning |
| --- | --- |
| `?` | copying non-tried areas |
| `*` | trimming failed blocks |
| `/` | scraping failed blocks |
| `-` | retrying bad sectors |
| `F` | filling blocks (fill mode) |
| `G` | generating a mapfile (generate mode) |
| `+` | finished |

Chapter 7's interrupted rescue left a status line reading
`0x01860000     ?               1` — position mid-file,
status copying, pass 1 — which tells you at a glance what
was happening when the run stopped and where it will pick
up.

## The block list

Each following line is one run of the input: starting
position, size, and a status character:

| Char | Meaning | State |
| --- | --- | --- |
| `?` | non-tried | not yet read |
| `*` | failed, non-trimmed | an error stopped a copy here; edges unknown |
| `/` | failed, non-scraped | trimmed, awaiting the digging phase |
| `-` | failed, bad-sector | tried hard; currently counted as lost |
| `+` | finished | rescued |

Now the excerpt reads plainly. MiB 0–10 (0xA00000 bytes from
0) is finished. Then 512 bytes of bad-sector (`-`), then a
long non-scraped stretch (`/`), then 512 more bad bytes —
that trio is the lab's dead MiB 10, trimmed to its true
edges during the first pass: the copy swept in, failed,
backed off to find where readability ends (512 B before the
failure resumes), and left the interior for scraping. The
finished block that follows runs to the next damaged area,
and so on. A mapfile is a chart of exactly this: not a log
of the past, but the current state of the whole disk,
stated tightly enough to resume from.

After the second pass (chapter 4) the same regions
consolidated: scraping subdivided the non-scraped interiors
to single sectors, retries confirmed them dead, and each
damaged area merged into one `-` block whose size is the
damage exactly:

```
0x00A00000  0x00100000  -
0x01F00000  0x00002000  -
0x03200000  0x00300000  -
```

1 MiB at MiB 10, 8 KiB at MiB 31, 3 MiB at MiB 50 — the
declared damage, to the byte.

## Asking instead of reading: ddrescuelog

You rarely need to parse the raw file by hand. `ddrescuelog`
answers the usual questions:

- `ddrescuelog -t rescue.map` — the status summary used in
  chapter 4: totals and percentages per state.
- `ddrescuelog -l - rescue.map` — list the bad-sector blocks
  by number. With `-b 1048576` the lab's mapfile answers
  `10, 31, 50, 51, 52` — the damaged megabytes, in a form a
  script (or a tired human) can use directly.
- `ddrescuelog -A rescue.map` — the mapfile annotated with
  human sizes next to each block.
- `ddrescuelog -D rescue.map` — exit status 0 only if the
  rescue is *done* in the strictest sense: every block
  successfully recovered. The lab's finished-but-lossy
  mapfile returns 1, and chapter 9 shows
  `ddrescuelog -d` deleting a mapfile only when a perfect
  rescue makes that true.

## Handling rules

- The mapfile must live on a **healthy** filesystem, not on
  the patient.
- Copy it somewhere safe between sessions
  (`cp rescue.map rescue.map.bak`). If it is ever lost, the
  image keeps the rescued data but forgets which parts it
  came from; rebuilding the knowledge means starting the
  reading over.
- Edit it only if you know why. ddrescue trusts it
  completely — that trust is what makes resume exact, and
  what makes a hand-mangled mapfile dangerous.

## Check

- Status line: where the rescue is and what it is doing.
  Block list: every region of the disk, one line each, with
  its state in a single character.
- The characters are a progress scale: `?` untried, `*`
  untrimmed, `/` unscraped, `-` bad, `+` safe.
- `ddrescuelog` exists so scripts and humans can ask the
  mapfile questions without parsing it raw.
