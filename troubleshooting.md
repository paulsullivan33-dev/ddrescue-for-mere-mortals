# Troubleshooting

Organised by symptom. The first two entries are errors the
lab produced on purpose so their text could be quoted
exactly; the rest follow from the guide's rescues and the
program's design.

**`ddrescue: patient.img: Infile and outfile are the same.`**
Input and output are the same file or device; ddrescue
refuses, exit 1. Point the outfile somewhere healthy. If you
meant to rescue a partition of a disk onto another partition
of the *same* disk — don't; that is the same-spindle trap of
chapter 10.

**The version banner doesn't say "GNU ddrescue".**
You installed the other package from chapter 3 (Debian's
`ddrescue`, a different tool). Install `gddrescue`, and check
`ddrescue --version` and `ddrescuelog --version` both report
GNU ddrescue and the same number.

**A rescue reports 100% rescued, instantly, with zero read
errors — on a source you *know* is failing.**
Be suspicious, not grateful, and check what ddrescue is
really reading. The lab met this exact ghost: its first
error-shim was silently bypassed (ddrescue's reads never
passed through it, thanks to the fortified `__open64_2`
entry point the shim hadn't wrapped), and every rescue
"passed" perfectly until a plain reader proved the errors
were never delivered. Real-world equivalents: a cache or
snapshot layer serving stale reads, the wrong device node,
a mounted image instead of the device. Verify the patient's
damage independently — a targeted read of a known-bad
region should fail — before trusting a clean report.

**`ddrescuelog -D` returns 1 although the rescue says
"Finished."**
Both are right: *finished* means no phase has anything left
to try; `-D`'s "done" means every byte was recovered. A
finished rescue with bad-sector blocks returns 1 forever.
Read `-t` for the exact residue.

**The merged second copy changed nothing.**
Bad-sector blocks are settled verdicts. Re-run the later
copy with a retry pass (`-r1`) to reopen them against the
new source — chapter 8's measured gotcha.

**The rescue is crawling and the error count climbs.**
Normal for the scraping and retrying phases, which work a
sector at a time in the worst neighbourhoods. Check the
phase in the status line: if you still see `*` or `/` states
hours into a big drive, patience is the treatment. If the
*copying* phase crawls, suspect the destination (slow
media, nearly full filesystem) as well as the patient.

**The drive vanished mid-rescue.**
Stop, let it cool, power-cycle it, and re-run the identical
command when it reappears; the mapfile picks up the story.
If it vanishes faster each time, the remaining life is
measured in minutes: restrict the next run with `-i`/`-s`
to the regions you value most, and read chapter 10's line
about mechanical noises twice.

**Output file smaller than the input, or the image won't
mount.**
The outfile reaches full size only as the rescue reaches
the end of the input; an interrupted rescue can leave it
short unless the tail was visited (the backwards pass
normally handles this). For mounting, the usual culprit is
mounting a whole-disk image as if it were a partition:
mount the partition image, or attach the disk image with
partition scanning (`losetup -P`, `kpartx`), on a copy.

**Kernel log flooding with read errors.**
Expected beside a failing drive — the package documentation
warns of it. The mapfile remains the authoritative tally;
silence the chatter only if the log itself is on precious
media.

**You lost the mapfile.**
The image keeps everything rescued so far, but nothing
knows which parts are good, so a new rescue starts the
record from zero. ddrescuelog has generate-mode options
described in its manual for reconstructing partial
knowledge; treat any of that as salvage, and take the
chapter 7 backup habit as the real fix.
