#pragma once

#include "mlibc/sysdep-tags.hpp"
#include <mlibc/sysdep-signatures.hpp>

namespace mlibc {

struct DemoSysdepTags :
	LibcPanic,
	LibcLog,
	Isatty,
	Write,
	TcbSet,
	AnonAllocate,
	AnonFree,
	Seek,
	Exit,
	Close,
	FutexWake,
	FutexWait,
	Read,
	Open,
	VmMap,
	VmUnmap,
	ClockGet,
    Sleep,
    Mkdir
{};

template<typename Tag>
using Sysdeps = SysdepOf<DemoSysdepTags, Tag>;

} // namespace mlibc
