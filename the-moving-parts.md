# The Moving Parts

Every ddrescue rescue is one command naming three files:

```
ddrescue [options] infile outfile mapfile
```

The order matters and so does never leaving the third one
off.

**The infile** is the patient: the failing drive
(`/dev/sdX`), a partition (`/dev/sdX2`), or — as in this
guide's lab — an ordinary file. ddrescue never writes to it.
(A safety worth knowing: give ddrescue the same file as
input and output and it refuses outright — the lab produced
the error on purpose: `ddrescue: patient.img: Infile and
outfile are the same.`)

**The outfile** is where the rescued data goes, normally an
image file on a healthy drive with at least as much free
space as the patient is big. It grows to the full size of
the input even though it begins life mostly empty: ddrescue
writes each rescued block at its true position, so the image
is always a faithful map of the source, holes and all.

**The mapfile** is the small text file that makes ddrescue
ddrescue. It records the state of every part of the input:
which blocks are rescued, which have not been tried, which
failed and in what way — plus where the current pass is and
what it is doing. It is updated as the rescue runs, so
killing the power mid-rescue loses at most the last few
seconds of work. Chapter 5 teaches you to read one; for now,
hold onto the rule:

> **Never run a rescue without a mapfile, and never delete
> the mapfile until the rescue is finished and verified.**
> Without it, ddrescue cannot resume, cannot skip what is
> already safe, and cannot merge a second copy — the three
> abilities that are the whole reason it exists. Running
> without one is permitted, and it reduces ddrescue to a
> slightly stubborn `dd`.

## The four phases

A rescue proceeds through phases, and the mapfile's
vocabulary (chapter 5) is really a description of which
phase has reached which block:

- **Copying** — the sweep. Reads the non-tried areas in big
  blocks, passes forwards and then backwards, skipping
  failures.
- **Trimming** — reads the edges of each failed block to
  shrink it to the truly unreadable core.
- **Scraping** — divides failed blocks into ever-smaller
  pieces to recover the good sectors among the bad.
- **Retrying** — re-reads the surviving bad sectors, one
  sector at a time, up to the number of extra passes you
  requested with `-r`.

You can invite or dismiss phases with options: `-n`
(`--no-scrape`) skips scraping for a quick first pass, `-r3`
asks for three retry passes at the end, and so on. Chapter 6
goes through each phase with the lab's mapfiles. The classic
workflow they add up to is two visits: a fast `-n` pass that
banks everything easy while the drive is freshest, then a
second command — same outfile, same mapfile — that scrapes
and retries as long as you care to let it.

## The second tool: ddrescuelog

The package also installs `ddrescuelog`, which never touches
data at all: it reads and reports on mapfiles. Ask it for a
status summary (`-t`), whether a rescue is completely done
(`-D`, designed to be tested in scripts), which blocks are
bad (`-l`), an annotated, human-sized copy of the mapfile
(`-A`), and more. Chapter 5 and chapter 9 use it on the
lab's real mapfiles; it is how you interrogate a rescue
without disturbing one.

## Options you will actually use

| Option | Meaning |
| --- | --- |
| `-n`, `--no-scrape` | First-pass mode: copy and trim, skip scraping |
| `-r N`, `--retry-passes=N` | Retry bad sectors N more times at the end |
| `-s`, `--size` | Rescue only the first bytes of the input (partition-sized pieces; the lab also uses it for a quick proof) |
| `-i`, `-o` | Start positions in input/output, for rescuing a known range |
| `--max-read-rate` | Throttle the copy — a kindness to a struggling drive, and how the lab staged its interruption demo |
| `-f`, `--force` | Required before ddrescue will write to a *device* as the outfile |
| `-d`, `--direct` | Direct disk access for the input (real drives; chapter 10) |
| `-v` | More status detail |

Everything else in the manual can wait until a recipe
(chapter 11) calls for it.

## Check

- Three files: patient in, image out, mapfile keeping the
  score. Omit the mapfile and you have thrown away the
  point.
- Four phases — copying, trimming, scraping, retrying —
  selectable with options, recorded in the mapfile.
- `ddrescuelog` reports on mapfiles; it is how you ask a
  rescue how it is doing without touching the data.
