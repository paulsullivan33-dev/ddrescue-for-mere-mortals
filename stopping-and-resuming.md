# Stopping and Resuming

A rescue you cannot safely stop is a trap: drives overheat,
make new noises, need the machine they are attached to back,
or simply exhaust the human watching them. ddrescue is
built to be interrupted — Ctrl-C, a closed laptop, a power
cut — because the mapfile is written as it goes. This
chapter proves it with a staged interruption, because "of
course it resumes" is exactly the kind of claim this series
checks.

## The staged interruption

A fresh 128 MiB patient, one dead megabyte at MiB 100, and
the read rate capped with `--max-read-rate=4MiB` so the
rescue is still young when the signal lands. Six seconds
in, Ctrl-C (delivered by `kill -INT`, the same signal):

```
pct rescued:   19.04%, read errors:        0
Interrupted by user
```

The mapfile left behind (`t04-resume.txt`):

```
0x01860000     ?               1
0x00000000  0x01860000  +
0x01860000  0x067A0000  ?
```

Status: copying, parked at 0x01860000. Below it, the honest
ledger — 25,559 kB finished (`+`), the remaining 108,658 kB
non-tried (`?`), nothing claimed that wasn't done.
`ddrescuelog -t` agrees: 19.04% rescued, 80.96% non-tried.

## The resume

The resumption command is the same command, uncapped:

```
ddrescue patient2.img rescued2.img rescue2.map
```

Its opening line is the proof this chapter exists for:

```
rescued: 25559 kB, tried: 0 B, bad-sector: 0 B, bad areas: 0
     ipos:   25559 kB, ...
```

`ipos` — the input position — starts at 25,559 kB, not at
zero. The 19% already banked is not re-read, not skimmed,
not even asked about; ddrescue read the mapfile and went to
the frontier. Within seconds the resumed run finished the
copying, trimmed and scraped the dead megabyte it met at
MiB 100, and left it as one bad-sector block of exactly
1,048,576 bytes. Total elapsed across both runs, a handful
of seconds; work repeated, none.

## The rules of safe stopping

- **Any stop is safe.** Ctrl-C is the polite version, but a
  crash or power cut costs only the seconds since the
  mapfile's last update — it is flushed regularly as the
  rescue runs.
- **Resume is the same command.** Same infile, same outfile,
  same mapfile. Changing the outfile or dropping the
  mapfile starts a new story.
- **Back the mapfile up between sessions.** It is small and
  it is the rescue's memory:
  `cp rescue.map rescue.map.bak`. ddrescue can work with a
  copied-back mapfile exactly as with the original.
- **A rescued-so-far image is usable at any point.** Even
  mid-rescue, the outfile holds every finished block in its
  correct place — relevant if a drive sounds like it is
  dying *now* and you must choose between imaging further
  and inspecting what you have (chapter 10).
- **To abandon a region, edit the story, not the file.**
  If a drive is being destroyed by one area, mark it done
  with ddrescuelog's mapfile operations or restrict the run
  with `-i`/`--input-position` and `-s` so the area is
  never visited, rather than hand-editing states you have
  not fully understood.

## Check

- Interruption is a normal event in ddrescue's design; the
  mapfile's regular updates bound the loss to seconds.
- Resume is proved, not promised: the resumed run opens at
  the exact position the interrupted one left, and re-reads
  nothing.
- Keep mapfile backups. The image is the treasure; the
  mapfile is the treasure map.
