# Real Drives

Everything before this chapter was measured on files. This
chapter is about the drives the tool exists for. It is
labeled honestly: the *rules* come from ddrescue's design,
its manual, and long field practice; the lab could not
rehearse them against physical failing hardware, and the
guide says so wherever it matters rather than laundering
practice into "verification."

## The rules that outrank technique

1. **Never work on the patient's filesystem.** Do not boot
   from the failing drive, do not mount it read-write, do
   not let a helpful operating system "repair" it. Every
   write is wear, and some writes rewrite exactly the
   structures you will need. If the drive must be mounted
   at all before imaging, read-only — but imaging first is
   the better order.
2. **Image the whole device, not the files.** File-by-file
   copying makes the drive thrash across its surface and
   dies at the first bad file. ddrescue's sequential sweep
   is gentle precisely because it is indiscriminate. Get the
   image; pick through it later, on the copy, at leisure.
3. **The destination is a different, healthy device** with
   room for the full size of the patient. Rescuing onto the
   same spindle is a way to lose both.
4. **Heat and time are the budget.** A drive that is failing
   may be noticeably worse after hours of continuous work.
   This is why the workflow leads with the fast `-n` pass:
   the easy 90-plus percent is banked while the drive is
   coolest and freshest, and every later phase can be
   scheduled, stopped, and resumed around the drive's
   condition (chapter 7).

## Running against a device

Typical shape, as root, with the patient as `/dev/sdX`
(letters illustrative — identify your device by serial and
size, twice, before pressing enter; `ddrescue` to the wrong
output device is a classic way to have a second emergency):

```
ddrescue -n /dev/sdX /mnt/healthy/rescue.img rescue.map
ddrescue -r3 /dev/sdX /mnt/healthy/rescue.img rescue.map
```

Differences from the file lab:

- **Permissions and `-f`.** Reading a device needs root,
  and ddrescue requires `-f` (`--force`) before it will
  write *to a device* as outfile (rescuing a partition onto
  another partition, say). Writing the outfile as a file, as
  above, needs neither.
- **`-d` (`--direct`)** asks the kernel for direct I/O on
  the input, bypassing caches that can serve stale data or
  mask the drive's true answers. Field practice favours it
  for real devices; it is meaningless on the lab's regular
  files, and the guide marks it documented-only here.
- **Error chatter in the system log.** A drive reporting
  errors can flood the kernel log; the Debian package notes
  say so outright. It is noise, not news — the mapfile is
  the authoritative record.
- **The drive may disappear.** USB bridges drop, controllers
  reset, drives spin down and refuse to return. ddrescue
  stops cleanly, the mapfile stands, and when the device
  reappears — perhaps after a cool-down and a power cycle —
  the same command resumes. Build that expectation into your
  schedule rather than treating it as failure.
- **A drive that sounds wrong** — repetitive clicking,
  scraping, beeping — is telling you the failure is
  mechanical. Software time is over; every further minute
  of power can convert a lab-recoverable drive into scrap.
  Chapter 13 draws this line plainly.

## Partitions and pieces

Sometimes the whole device is more than you need or more
than the destination holds:

- `ddrescue -n /dev/sdX2 part.img part.map` images one
  partition; the `.img` then mounts like any partition
  image.
- `-s` limits the size rescued and `-i` sets the starting
  position, so a rescue can take the partition table's
  region first (it is small and precious), then decide.
- If the partition table itself is unreadable, image from
  zero anyway; chapter 12's neighbours include tools that
  reconstruct partitions from an image.

## Patience, scheduled

A large, sick drive can take days across many sessions.
That is not the tool failing; it is the tool refusing to
trade the patient's remaining life for your convenience.
Keep the mapfile backed up (chapter 7), note the tally after
each session (`ddrescuelog -t`), and let `-D` tell scripts
and tired humans whether another session is owed.

## Check

- Image first, files later; never repair, mount read-write,
  or boot from the patient.
- Devices bring root, `-f` for device outputs, `-d` direct
  I/O, kernel-log noise, and the possibility of vanishing —
  all survivable, because the mapfile holds.
- Clicking or scraping sounds mean stop: that boundary is
  mechanical, and no phase of ddrescue crosses it.
