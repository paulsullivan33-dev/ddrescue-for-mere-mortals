# Limits

- **ddrescue reads through the operating system.** A drive
  the system cannot enumerate — dead electronics, seized
  motor, firmware that never comes ready — gives it nothing
  to read. That is a hardware-recovery problem, and the
  clicking-drive line in chapter 10 is the practical sign
  you have reached it.
- **Truly dead sectors stay dead.** The lab's shim was
  absolute, and the rescue's loss exactly equalled the
  declared damage: ddrescue's phases recover *marginal*
  data, not absent data. On real drives the scrapings are
  often better; they are never magic.
- **It knows nothing about files.** The image is blocks.
  ddrescue cannot tell a hole in a dissertation from a hole
  in free space — interpretation belongs to the mapfile's
  geometry plus the filesystem tools of chapters 9 and 12.
- **Speed belongs to the patient.** The lab's 64 MiB passed
  in a second; a multi-terabyte drive, healthy stretches
  included, takes as long as it takes, and a sick one takes
  longer. There is no mode that makes a dying mechanism
  fast, only modes that waste less of its remaining life.
- **Encrypted and self-encrypting storage narrows the
  win.** If the decryption happens in the drive or in a
  layer that needs the filesystem alive (full-disk
  encryption unlocked at boot, self-encrypting drives), a
  block image may preserve ciphertext you cannot open
  without the original stack — rescue the image anyway, but
  plan the decryption path before the hardware is gone.
- **The lab's boundary.** This guide's measured claims rest
  on file-based rescues with simulated read errors. The
  phases, mapfile semantics, resume and merging behaviours
  are properties of the program and were observed directly;
  claims about physical drives — retry warmth, head
  behaviour, disappearance and return — are the manual's and
  the field's, labeled as such in chapter 10, not this
  lab's measurements.
- **A mapfile is a liability if mishandled.** It authorises
  ddrescue to skip whatever it calls finished. Corrupt it,
  pair it with the wrong outfile, or lose it, and the
  rescue's memory is gone or lying. Small file, large
  trust: back it up, keep it beside its image.

## Check

- Software cannot see a drive the system cannot see.
- Dead is dead: the accounting at the end is exact about
  which bytes those were.
- The guide's measurements are file-true; its hardware
  advice is labeled practice, not lab results.
