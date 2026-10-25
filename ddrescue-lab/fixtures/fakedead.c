// fakedead.c -- a stand-in "failing drive" for the ddrescue lab.
//
// The lab machine is a sandbox: it has no spare disk, no loop
// devices, and refuses to create device nodes, so there is no real
// block device to be ill with. This shim is loaded with LD_PRELOAD
// and answers genuine read(2)/pread(2) calls on one named file
// with -1/EIO whenever the requested range touches a byte range
// declared dead in the environment variable FAKEDEAD:
//
//   FAKEDEAD_FILE=patient.img
//   FAKEDEAD=10485760-11534336,32505856-32514048
//
// From ddrescue's side of the glass the errors are real: its reads
// really fail, with the same errno a dying drive produces. What is
// fake is only the drive. Ranges are [start, end) byte offsets.
//
// Only reads on the target file are touched. Everything else,
// including all writes, passes straight through.

#define _GNU_SOURCE
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXRANGES 32
#define MAXFDS    64

static long long dead_lo[MAXRANGES], dead_hi[MAXRANGES];
static int ndead = -1;              // -1 = not parsed yet
static char target[256] = "";
static int  target_fd[MAXFDS];      // fds currently open on target
static off_t fd_pos[MAXFDS];        // tracked offset for read()

static void parse_config(void)
{
    if (ndead >= 0) return;
    ndead = 0;
    const char *f = getenv("FAKEDEAD_FILE");
    if (f) { strncpy(target, f, sizeof(target) - 1); }
    const char *s = getenv("FAKEDEAD");
    if (!s) return;
    char buf[1024];
    strncpy(buf, s, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = 0;
    for (char *tok = strtok(buf, ","); tok && ndead < MAXRANGES;
         tok = strtok(NULL, ",")) {
        char *dash = strchr(tok, '-');
        if (!dash) continue;
        *dash = 0;
        dead_lo[ndead] = atoll(tok);
        dead_hi[ndead] = atoll(dash + 1);
        if (dead_hi[ndead] > dead_lo[ndead]) ndead++;
    }
    for (int i = 0; i < MAXFDS; i++) { target_fd[i] = -1; fd_pos[i] = 0; }
}

static int is_target(const char *path)
{
    if (!target[0] || !path) return 0;
    size_t lt = strlen(target), lp = strlen(path);
    if (lp < lt) return 0;
    return strcmp(path + lp - lt, target) == 0;   // suffix match
}

static void note_open(int fd, const char *path)
{
    if (fd < 0 || fd >= MAXFDS) return;
    if (is_target(path)) { target_fd[fd] = 1; fd_pos[fd] = 0; }
    else target_fd[fd] = -1;
}

static int fd_is_target(int fd)
{
    return fd >= 0 && fd < MAXFDS && target_fd[fd] == 1;
}

static int range_dead(off_t pos, size_t count)
{
    for (int i = 0; i < ndead; i++)
        if ((long long)pos < dead_hi[i] &&
            (long long)pos + (long long)count > dead_lo[i])
            return 1;
    return 0;
}

typedef int (*open_fn)(const char *, int, ...);
typedef int (*openat_fn)(int, const char *, int, ...);
typedef FILE *(*fopen_fn)(const char *, const char *);
typedef ssize_t (*pread_fn)(int, void *, size_t, off_t);
typedef ssize_t (*read_fn)(int, void *, size_t);
typedef off_t (*lseek_fn)(int, off_t, int);
typedef int (*close_fn)(int);

static int open_common(const char *path, int flags, mode_t mode, int use_mode)
{
    static open_fn real_open = NULL;
    if (!real_open) real_open = (open_fn)dlsym(RTLD_NEXT, "open");
    int fd = use_mode ? real_open(path, flags, mode)
                      : real_open(path, flags);
    parse_config();
    note_open(fd, path);
    return fd;
}

int open(const char *path, int flags, ...)
{
    mode_t mode = 0;
    int use_mode = (flags & (O_CREAT | O_TMPFILE)) != 0;
    if (use_mode) {
        va_list ap; va_start(ap, flags);
        mode = va_arg(ap, mode_t); va_end(ap);
    }
    return open_common(path, flags, mode, use_mode);
}

int open64(const char *path, int flags, ...)
{
    mode_t mode = 0;
    int use_mode = (flags & (O_CREAT | O_TMPFILE)) != 0;
    if (use_mode) {
        va_list ap; va_start(ap, flags);
        mode = va_arg(ap, mode_t); va_end(ap);
    }
    static open_fn real_open64 = NULL;
    if (!real_open64) real_open64 = (open_fn)dlsym(RTLD_NEXT, "open64");
    int fd = use_mode ? real_open64(path, flags, mode)
                      : real_open64(path, flags);
    parse_config();
    note_open(fd, path);
    return fd;
}

int openat(int dirfd, const char *path, int flags, ...)
{
    mode_t mode = 0;
    int use_mode = (flags & (O_CREAT | O_TMPFILE)) != 0;
    if (use_mode) {
        va_list ap; va_start(ap, flags);
        mode = va_arg(ap, mode_t); va_end(ap);
    }
    static openat_fn real_openat = NULL;
    if (!real_openat) real_openat = (openat_fn)dlsym(RTLD_NEXT, "openat");
    int fd = use_mode ? real_openat(dirfd, path, flags, mode)
                      : real_openat(dirfd, path, flags);
    parse_config();
    note_open(fd, path);
    return fd;
}

int openat64(int dirfd, const char *path, int flags, ...)
{
    mode_t mode = 0;
    int use_mode = (flags & (O_CREAT | O_TMPFILE)) != 0;
    if (use_mode) {
        va_list ap; va_start(ap, flags);
        mode = va_arg(ap, mode_t); va_end(ap);
    }
    static openat_fn real_openat64 = NULL;
    if (!real_openat64) real_openat64 = (openat_fn)dlsym(RTLD_NEXT, "openat64");
    int fd = use_mode ? real_openat64(dirfd, path, flags, mode)
                      : real_openat64(dirfd, path, flags);
    parse_config();
    note_open(fd, path);
    return fd;
}

typedef int (*open2_fn)(const char *, int);
typedef int (*openat2_fn)(int, const char *, int);

// Fortified entry points. Debian/Ubuntu build with
// _FORTIFY_SOURCE, which redirects open() calls whose flags are
// known at compile time to __open_2 / __open64_2 (and the openat
// pair). ddrescue's own imports are __open64_2 -- the first
// version of this shim wrapped only the classic names and
// ddrescue sailed straight past it, rescuing 100% of a "dead"
// file. Wrapping these is what makes the lab honest.
int __open_2(const char *path, int flags)
{
    static open2_fn real = NULL;
    if (!real) real = (open2_fn)dlsym(RTLD_NEXT, "__open_2");
    int fd = real(path, flags);
    parse_config();
    note_open(fd, path);
    return fd;
}

int __open64_2(const char *path, int flags)
{
    static open2_fn real = NULL;
    if (!real) real = (open2_fn)dlsym(RTLD_NEXT, "__open64_2");
    int fd = real(path, flags);
    parse_config();
    note_open(fd, path);
    return fd;
}

int __openat_2(int dirfd, const char *path, int flags)
{
    static openat2_fn real = NULL;
    if (!real) real = (openat2_fn)dlsym(RTLD_NEXT, "__openat_2");
    int fd = real(dirfd, path, flags);
    parse_config();
    note_open(fd, path);
    return fd;
}

int __openat64_2(int dirfd, const char *path, int flags)
{
    static openat2_fn real = NULL;
    if (!real) real = (openat2_fn)dlsym(RTLD_NEXT, "__openat64_2");
    int fd = real(dirfd, path, flags);
    parse_config();
    note_open(fd, path);
    return fd;
}

FILE *fopen(const char *path, const char *mode)
{
    static fopen_fn real_fopen = NULL;
    if (!real_fopen) real_fopen = (fopen_fn)dlsym(RTLD_NEXT, "fopen");
    FILE *fp = real_fopen(path, mode);
    parse_config();
    if (fp) note_open(fileno(fp), path);
    return fp;
}

FILE *fopen64(const char *path, const char *mode)
{
    static fopen_fn real_fopen64 = NULL;
    if (!real_fopen64) real_fopen64 = (fopen_fn)dlsym(RTLD_NEXT, "fopen64");
    FILE *fp = real_fopen64(path, mode);
    parse_config();
    if (fp) note_open(fileno(fp), path);
    return fp;
}

static ssize_t pread_common(int fd, void *buf, size_t count, off_t off,
                            const char *sym)
{
    static pread_fn real_p = NULL, real_p64 = NULL;
    parse_config();
    if (fd_is_target(fd) && range_dead(off, count)) {
        errno = EIO;
        return -1;
    }
    if (strcmp(sym, "pread64") == 0) {
        if (!real_p64) real_p64 = (pread_fn)dlsym(RTLD_NEXT, "pread64");
        return real_p64(fd, buf, count, off);
    }
    if (!real_p) real_p = (pread_fn)dlsym(RTLD_NEXT, "pread");
    return real_p(fd, buf, count, off);
}

ssize_t pread(int fd, void *buf, size_t count, off_t off)
{ return pread_common(fd, buf, count, off, "pread"); }

ssize_t pread64(int fd, void *buf, size_t count, off_t off)
{ return pread_common(fd, buf, count, off, "pread64"); }

ssize_t read(int fd, void *buf, size_t count)
{
    static read_fn real_read = NULL;
    if (!real_read) real_read = (read_fn)dlsym(RTLD_NEXT, "read");
    parse_config();
    if (fd_is_target(fd) && range_dead(fd_pos[fd], count)) {
        errno = EIO;
        return -1;
    }
    ssize_t r = real_read(fd, buf, count);
    if (fd_is_target(fd) && r > 0) fd_pos[fd] += r;
    return r;
}

static off_t lseek_common(int fd, off_t off, int whence, const char *sym)
{
    static lseek_fn real_l = NULL, real_l64 = NULL;
    off_t r;
    if (strcmp(sym, "lseek64") == 0) {
        if (!real_l64) real_l64 = (lseek_fn)dlsym(RTLD_NEXT, "lseek64");
        r = real_l64(fd, off, whence);
    } else {
        if (!real_l) real_l = (lseek_fn)dlsym(RTLD_NEXT, "lseek");
        r = real_l(fd, off, whence);
    }
    if (fd_is_target(fd) && r >= 0) fd_pos[fd] = r;
    return r;
}

off_t lseek(int fd, off_t off, int whence)
{ return lseek_common(fd, off, whence, "lseek"); }

off_t lseek64(int fd, off_t off, int whence)
{ return lseek_common(fd, off, whence, "lseek64"); }

int close(int fd)
{
    static close_fn real_close = NULL;
    if (!real_close) real_close = (close_fn)dlsym(RTLD_NEXT, "close");
    if (fd >= 0 && fd < MAXFDS) target_fd[fd] = -1;
    return real_close(fd);
}
