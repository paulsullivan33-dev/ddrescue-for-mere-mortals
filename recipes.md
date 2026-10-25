# Recipes

Each recipe assumes the spine (chapters 1–7). Commands use
files so they run anywhere; for a device, substitute the
device path as chapter 10 describes.

## The standard two-visit rescue

```
ddrescue -n patient.img rescued.img rescue.map   # bank the easy data
# ... hours or days later, drive rested ...
ddrescue -r3 patient.img rescued.img rescue.map  # scrape and retry
ddrescuelog -t rescue.map                        # the tally
```

## How done are we, exactly?

```
ddrescuelog -t rescue.map        # human summary
ddrescuelog -D rescue.map        # exit 0 only if every byte is in
echo $?
```

Use `-D` in scripts that decide whether to keep a rescue
session scheduled; the lab verified it returns 1 on a
finished-but-lossy mapfile and 0 on a perfect one.

## Which megabytes are damaged?

```
ddrescuelog -l - -b 1048576 rescue.map
```

Answers in block numbers of the size you choose — the
lab's final mapfile answered 10, 31, 50, 51, 52. Feed them
to the tool doing file-level triage on the image.

## Rescue one region only

The small-but-critical beginning of a disk (partition table
and filesystem metadata), or any known range:

```
ddrescue -i 0 -s 64MiB patient.img head.img head.map
```

The lab used `-s` the same way to keep its early proofs
quick: `ddrescue -n -s 16MiB`.

## A drive that must be babied

Cap the rate, take the easy pass, and let long pauses
happen between visits:

```
ddrescue -n --max-read-rate=20MiB patient.img rescued.img rescue.map
```

The rate cap proved itself in the lab as the interruption
stager; on hardware it is the documented way to keep a
limping drive from being run flat-out for hours.

## Merge a second copy

```
ddrescue copy-a.img merged.img merged.map
ddrescue -r1 copy-b.img merged.img merged.map   # -r reopens bad blocks
ddrescuelog -D merged.map && echo complete
```

Chapter 8 is the full story, including why the `-r` is not
optional once a block has been condemned.

## Rescue a partition to a partition

Destination is a device this time, so `-f` and root are
part of the recipe:

```
sudo ddrescue -f -n /dev/sdX2 /dev/sdY2 rescue.map
```

Double-check both device letters against sizes and serials
before enter; this recipe overwrites `/dev/sdY2` wholesale.

## Keep a diary with the patient

Between sessions:

```
ddrescuelog -t rescue.map >> rescue-diary.txt
cp rescue.map rescue.map.$(date +%F-%H%M)
```

Cheap insurance and, months later, the answer to "what did
we actually lose?" — asked exactly when the mapfile would
otherwise have been deleted.
