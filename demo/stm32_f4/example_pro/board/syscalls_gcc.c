#if defined(__GNUC__) && !defined(__CC_ARM)

#include <errno.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>

extern char end;
extern char _estack;

static char *heap_end;

int _close(int file) {
    (void)file;
    errno = ENOSYS;
    return -1;
}

int _fstat(int file, struct stat *st) {
    (void)file;
    if (st != 0) {
        st->st_mode = S_IFCHR;
    }
    return 0;
}

int _isatty(int file) {
    (void)file;
    return 1;
}

int _lseek(int file, int ptr, int dir) {
    (void)file;
    (void)ptr;
    (void)dir;
    return 0;
}

int _read(int file, char *ptr, int len) {
    (void)file;
    (void)ptr;
    (void)len;
    return 0;
}

int _write(int file, char *ptr, int len) {
    (void)file;
    (void)ptr;
    return len;
}

void *_sbrk(ptrdiff_t incr) {
    char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = &end;
    }

    prev_heap_end = heap_end;
    if ((heap_end + incr) > &_estack) {
        errno = ENOMEM;
        return (void *)-1;
    }

    heap_end += incr;
    return prev_heap_end;
}

void _exit(int status) {
    (void)status;
    while (1) {
    }
}

int _kill(int pid, int sig) {
    (void)pid;
    (void)sig;
    errno = ENOSYS;
    return -1;
}

int _getpid(void) {
    return 1;
}

#endif
