# ddrescue for Mere Mortals

A hard drive that is starting to fail presents you with a cruel
choice. Copy the files off it and the copying itself may kill
it — every minute of work is a minute of wear on a mechanism
that is already dying. Read it in the usual way and the drive
will sit for five minutes retrying one bad spot while
everything behind it goes unrescued.

GNU ddrescue is the tool built for exactly this moment. It
copies a failing drive (or partition, or file) into an image
file on a healthy drive, in an order designed for a patient
that may not survive the afternoon: the easy data first, fast,
with no retries; then progressively more patient passes over
the parts that failed, splitting bad areas down to single
sectors. And it keeps a *mapfile* — a small text file that
records, block by block, what has been rescued, what has not
been tried, and what has failed. Stop the rescue because the
drive is clicking, the room is needed, or you are: run the
same command tomorrow and it picks up where it left off,
reading only what is still missing. Have a second, differently
damaged copy of the same disk? The same mapfile lets ddrescue
fill the first copy's holes from the second one.

This guide teaches the whole shape of it: what the three
files in every ddrescue command are, how to read a mapfile the
way you would read a chart, what each phase (copying,
trimming, scraping, retrying) is for and when to invite it,
how stopping and resuming really behaves, how to account —
byte for byte — for what you got back, and how a lab rescue
on files translates to a real failing drive.

> **Honest lab notes.** The verification lab for this guide
> could not use a real failing drive, or even a real block
> device — the build sandbox forbids loop devices outright
> (recorded in `ddrescue-lab/transcripts/t90-sandbox-limits.txt`).
> So the lab rescued *files*, which ddrescue handles exactly
> like devices: a 64 MiB "patient" image filled with a known
> pattern, one region per megabyte, every region checksummable,
> and its damage faked by a small program (`fakedead`, in
> `ddrescue-lab/fixtures/`) that makes chosen byte ranges
> answer reads with real `EIO` errors, the same error a dying
> sector reports. Every number in this guide — bytes rescued,
> read errors, what the mapfile said at each stage, the final
> recovered-vs-lost accounting — was measured against that
> known original and is reproduced in
> `ddrescue-lab/transcripts/`. What a file cannot imitate is
> a physical drive: its retries, its heat, its sounds, the
> way it may vanish from the bus mid-rescue. Chapter 10 covers
> real drives, and says plainly which of its advice is
> reasoned from the manual and field practice rather than
> measured in this lab. Where this guide claims a behaviour,
> it was run. Where it could not run, it says so.

## What this guide covers

- Installing GNU ddrescue, including the package-name trap
  that gets beginners a completely different program
  (chapter 3).
- The anatomy of the one command: input, output, mapfile —
  and why the mapfile is the most important of the three
  (chapter 2).
- A complete worked rescue with real output: first pass,
  the mapfile between passes, a scraping-and-retrying second
  pass (chapters 4–6).
- Reading mapfiles with your eyes and with ddrescue's own
  reporting tool, ddrescuelog (chapter 5).
- Stopping mid-rescue and resuming, with the proof that
  nothing already rescued is read twice (chapter 7).
- Merging two damaged copies into one whole file
  (chapter 8).
- The final accounting: how recovered and lost bytes are
  counted against a known original, and what to do with the
  image you end up with (chapter 9).
- Rescuing real devices: the rules that keep a dying drive
  alive long enough, and what differs from the lab
  (chapter 10).

## What it doesn't

- It is not a filesystem-repair manual. ddrescue delivers an
  image of the drive; tools that repair filesystems or carve
  files out of the image are neighbours (chapter 12), not
  this guide.
- It does not cover ddrescue's fill and generate modes, or
  rescuing from tape and optical media beyond a mention.
- It cannot promise your data. A rescue is salvage under
  time pressure, and chapter 13 is blunt about the odds.

## How to use this guide

Chapters 1–7 are the spine: read them in order and you can
run a rescue and explain every line of what it printed.
Chapter 8 is the chapter to read before you need it, if a
second copy might exist. Chapters 9 and 10 are the
follow-through. The recipes (chapter 11) assume the spine,
and the troubleshooting chapter (14) is organised by the
symptom you are staring at, because during a real rescue you
will arrive there in a hurry.

---

## Part of the series

This guide is one of the [For Mere Mortals](https://paulsullivan33-dev.github.io/mere-mortals/) guides: plain-language guides to the tools programmers and makers use.

Related guides:

- [Clonezilla for Mere Mortals](https://paulsullivan33-dev.github.io/clonezilla-for-mere-mortals/)
- [Backups for Mere Mortals](https://paulsullivan33-dev.github.io/backups-for-mere-mortals/)
- [Ncdu for Mere Mortals](https://paulsullivan33-dev.github.io/ncdu-for-mere-mortals/)

## License

The writing in this guide is licensed under [CC BY 4.0](https://github.com/paulsullivan33-dev/ddrescue-for-mere-mortals/blob/main/LICENSE). Code examples and lab scripts are licensed under the [MIT License](https://github.com/paulsullivan33-dev/ddrescue-for-mere-mortals/blob/main/LICENSE-CODE).
