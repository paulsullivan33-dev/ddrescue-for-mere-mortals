# ddrescue lab

The verification lab for this guide. The sandbox forbids
block devices (see `transcripts/t90-sandbox-limits.txt`),
so the patient is a file and its "bad sectors" are produced
by an LD_PRELOAD shim that returns real EIO errors for
declared byte ranges — the same error a dying sector
reports. All results are in `transcripts/`, produced by the
tools in `fixtures/`:

- `make-pattern.py OUTFILE REGIONS` — a pattern image of
  REGIONS 1 MiB regions; prints per-region SHA-256.
- `fakedead.c` — build with
  `gcc -O2 -shared -fPIC -o fakedead.so fakedead.c -ldl`,
  then run the reader under it with
  `LD_PRELOAD=./fakedead.so FAKEDEAD_FILE=patient.img
  FAKEDEAD=start-end[,start-end...]`.
  Note the `__open_2`/`__open64_2` wrappers: fortified
  binaries (including ddrescue) open through them, and a
  shim without them is silently bypassed (chapter 14).
- `compare-blocks.py ORIG RESCUED [BLOCK]` — independent
  recovered-vs-lost accounting against a known original.

Re-running the lab needs a GNU ddrescue (`gddrescue`
package) and a patient of your own making; the transcripts
here record the run this guide quotes.
