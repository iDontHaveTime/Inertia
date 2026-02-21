#ifndef INERTIA_PLATFORM_UNIFIED_H
#define INERTIA_PLATFORM_UNIFIED_H

#include <inr/ADT/FileHandle.h>
#include <inr/Platform/Standard.h>

namespace inr{

    using read_return = standard::read_return;
    using write_return = standard::write_return;

    write_return uwrite(const void* buf, size_t size, size_t n, const fs::ufile& stream) noexcept;

}

#endif // INERTIA_PLATFORM_UNIFIED_H
