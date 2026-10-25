# Installing ddrescue

## The trap first

On Debian and Ubuntu there are **two different programs**
with nearly the same name, and the package called simply
`ddrescue` is not this one. That package is an older, simpler
tool (Kurt Garloff's `dd_rescue`): a decent block copier,
but with no mapfile, no phases, no resume — none of what
this guide is about.

The GNU program's package is **`gddrescue`**:

```
sudo apt install gddrescue
```

Whichever way you installed, verify what you actually got:

```
$ ddrescue --version
ddrescue (GNU ddrescue) 1.27
...
$ ddrescuelog --version
ddrescuelog (GNU ddrescue) 1.27
```

The words "GNU ddrescue" in the banner are the check. Both
commands arrive in the one package. On other distributions
the name differs slightly (Fedora also uses `gddrescue`);
building from the GNU source is a normal configure-and-make
affair if your distribution has neither.

## The version this guide used

The lab ran **GNU ddrescue 1.27**, from the `gddrescue 1.27-1`
package for Ubuntu 24.04. One disclosure in the spirit of
this series: the build sandbox shared its package state with
other builders and could not complete a system install, so
the lab downloaded that exact `.deb` and ran the genuine,
unmodified binaries unpacked from it — the versions recorded
in `ddrescue-lab/transcripts/t00-version.txt` are the
package's own. Nothing in the lab's results depends on how
the binaries arrived; on your machine, install the package
properly.

## Set up a rescue station

A rescue is not something to improvise at the sick machine
with its own failing disk as the destination. Before the
patient arrives:

- A healthy destination drive with free space at least equal
  to the patient's *whole size* — the image is full-size
  even when most of the disk is empty.
- Room in your plan for the image to be copied again later:
  repairs and file recovery (chapter 9) should work on a
  copy of the image, keeping the rescued original untouched.
- A place to keep the small things: the mapfile is tiny
  (kilobytes to a few megabytes) and more precious than the
  image — losing it mid-rescue costs you the record, not the
  data already banked. Chapter 7 has the backup habit.

## Check

- `gddrescue` is the package; `ddrescue` (the package) is a
  different, older tool with no mapfile.
- Both `ddrescue` and `ddrescuelog` should banner
  "GNU ddrescue" and the same version.
- Prepare the destination first: healthy drive, full-size
  space, and a safe home for the mapfile.
