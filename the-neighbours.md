# The Neighbours

ddrescue's job ends at a faithful image with named holes.
The tools around it divide into three families: copy *before*
things go wrong, cope when the source is healthy, and work
*on the image* after the rescue.

## Copying tools that are not ddrescue

- **`dd`** — the ancestor. Copies blocks faithfully and dumbly:
  no mapfile, no phases, no resume, and a tendency to stop or
  stall at the first error unless coaxed with
  `conv=noerror,sync` — the coaxing that beginners used to
  use, and that loses alignment and data quietly. ddrescue
  exists because this was not good enough.
- **`dd_rescue`** — the other Debian package from chapter 3.
  A smarter block copier with some error tolerance, but no
  mapfile-based workflow; fine tool, different book.
- **Drive cloners (Clonezilla and friends)** — excellent for
  imaging *healthy* drives, with filesystems understood and
  space saved. A cloner meeting a failing source meets the
  same stalls as a naive copy. Image the sick with ddrescue;
  clone the well with a cloner.
- **Backup tools (rsync, restic, tar)** — the right answer
  *before* the emergency: they copy files from a living
  filesystem, with history and verification. During the
  emergency they thrash the patient and abandon ship at the
  first unreadable file. Prevention here, rescue there.

## Working on the image afterwards

- **Filesystem tools (`fsck`, `ntfsfix`, and kin)** — run on
  a *copy* of the image, never the rescued original; their
  repairs are another pass of judgment on damaged goods.
- **Partition reconstruction (TestDisk)** — rebuilds lost
  partition tables from an image whose beginning was too
  damaged to read directly.
- **File carving (PhotoRec and friends)** — extracts files
  by content signature from an image whose filesystem
  cannot be revived; the last resort, producing files
  without names or folders, but files.
- **Mapfile-aware recovery (`ddrutility`, `hdparam`-era
  helpers)** — tools in ddrescue's orbit that use the
  mapfile to copy only the files that intersect the holes,
  from a second source, once you know which files matter.

## Check

- Before the emergency: backups. During it: ddrescue, for
  the image. After it: filesystem and carving tools, on
  copies.
- `dd` and `dd_rescue` are relatives, not substitutes; the
  mapfile is the difference.
