# What ddrescue Is

GNU ddrescue copies data out of a source that may not survive
being read. The source can be a whole drive, a partition, or
an ordinary file; the destination is almost always an image
file on a healthy drive, the same size as the source. What
makes it a rescue tool rather than a copier is the *order* it
reads in, and the *record* it keeps.

An ordinary copy reads front to back and treats every byte as
equally urgent. On a failing drive that is exactly wrong. The
first bad spot can stall a naive copy for minutes per retry
while gigabytes of perfectly readable data sit behind it —
data that might have been saved in those minutes, had anyone
asked for it first. ddrescue asks for it first. Its strategy:

1. **Sweep up everything easy.** Read in large blocks, no
   retries, skipping past failures so one bad area cannot
   hold the rest hostage.
2. **Work the edges.** Trim around the failures to find
   exactly where the damage starts and stops.
3. **Dig in small bites.** Split failed areas into smaller
   and smaller pieces, rescuing the good sectors hiding
   between the bad ones.
4. **Only then, retry.** Re-read the stubborn sectors, as
   many times as you allow — last, when everything else is
   already safe.

Every stage writes what it learned into the mapfile, so no
stage is ever repeated by accident and the rescue can stop
and resume anywhere. That discipline is the whole difference
between "we got whatever the drive felt like giving" and "we
got everything except these 8,208 sectors, and we can name
them."

## Where it fits

Reach for ddrescue when the *source* is the problem: a drive
reporting read errors, making new noises, getting slower by
the day, or a disk image or memory card with regions no tool
can read. If the source is healthy and you just need a copy,
simpler tools do it faster and friendlier (chapter 12). If
the drive is already dead — not recognised at all, clicking
steadily, smelling of hot electronics — ddrescue cannot
help; it talks to drives through the operating system, and a
drive the system cannot see is beyond software. That is a
job for a recovery lab with clean-room tools, and chapter 13
is honest about the boundary.

## What a rescue produces

The output of a finished rescue is an image: a byte-for-byte
picture of the source, full size, in which every byte
ddrescue managed to read is in its proper place, and every
byte it could not read is a hole (zeros, or whatever you told
it to fill with). The image is not the end of the story —
chapter 9 covers what you do with it — but it is the moment
the emergency ends. From the image onwards, you work on a
file that is in no hurry at all.

Our lab's finished rescue, counted byte for byte against the
original it was made from, recovered 62,906,368 of 67,108,864
bytes and lost exactly the 4,202,496 bytes (6.26%) that the
lab had pronounced dead — not one readable byte misplaced,
not one hole papered over. That exactness is the point of
the tool, and the standard the rest of this guide holds it
to.

## Check

- ddrescue exists for one situation: the source is failing.
- Its method is order plus record: easy data first, failures
  last, everything written down in the mapfile.
- The product is an image file with named, countable holes
  — the emergency reduced to arithmetic.
