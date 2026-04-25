#include "mlibc/tcb.hpp"
#include <abi-bits/errno.h>
#include <bits/ensure.h>
#include <bits/syscall.h>
#include <mlibc/all-sysdeps.hpp>
#include <string.h>

#define SYS_EXIT 0
#define SYS_READ 1
#define SYS_WRITE 2
#define SYS_MMAP 3
#define SYS_ARCH_PRCTL 4

#define STUB()                                                                                     \
	({                                                                                             \
		__ensure(!"STUB function was called");                                                     \
		__builtin_unreachable();                                                                   \
	})

namespace mlibc {

void Sysdeps<LibcPanic>::operator()() {
    sysdep<LibcLog>("!!! mlibc panic !!!");
    sysdep<Exit>(-1);
    __builtin_trap();
}

void Sysdeps<LibcLog>::operator()(const char *msg) {
    ssize_t unused;
    sysdep<Write>(2, msg, strlen(msg), &unused);
    char crlf[] = "\r\n";
    sysdep<Write>(2, crlf, 2, &unused);
}

int Sysdeps<Isatty>::operator()(int fd) {
    (void)fd;
    // this returns ENOTTY when it is not a tty, but we do not have a proper implementation
    // so always return that a file is a tty
    return 0;
}

int Sysdeps<Write>::operator()(int fd, void const *buf, size_t size, ssize_t *ret) {
	*ret = syscall(SYS_WRITE, fd, buf, size);

	// this can never fail in the demo os
	return 0;
}

int Sysdeps<TcbSet>::operator()(void *pointer) {
    auto ret = syscall(SYS_ARCH_PRCTL, 0x1002 /* ARCH_SET_FS */, pointer);
    if(ret < 0)
        return ret;
    return 0;
}

int Sysdeps<AnonAllocate>::operator()(size_t size, void **pointer) {
    return sysdep<VmMap>(
        nullptr,
        size,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0,
        pointer
    );
}

int Sysdeps<AnonFree>::operator()(void *, unsigned long) {
    return 0; // no-op, for now
}

int Sysdeps<Seek>::operator()(int, off_t, int, off_t *) { return ESPIPE; }

void Sysdeps<Exit>::operator()(int status) {
	syscall(SYS_EXIT);
	__builtin_unreachable();
}

int Sysdeps<Close>::operator()(int) { STUB(); }

int Sysdeps<FutexWake>::operator()(int *, bool) { STUB(); }
int Sysdeps<FutexWait>::operator()(int *, int, timespec const *) { STUB(); }

int Sysdeps<Read>::operator()(int fd, void *buffer, unsigned long buffer_size, long *bytes_read) {
    auto out = syscall(SYS_READ, fd, buffer, buffer_size);
    if (out < 0)
        return out;

    *bytes_read = (long) out;
    return 0;
}

int Sysdeps<Open>::operator()(const char *, int, unsigned int, int *) { STUB(); }

int Sysdeps<VmMap>::operator()(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window) {
    auto out = syscall(
        SYS_MMAP, hint, size, prot, flags, fd, offset
    );

    if (out < 0)
        return out;

    *window = (void *)out;
    return 0;
}

int Sysdeps<VmUnmap>::operator()(void *, size_t) { STUB(); }
int Sysdeps<ClockGet>::operator()(int, time_t *, long *) { STUB(); }

} // namespace mlibc
