#include <inr/Platform/Platform.h>
#include <inr/Platform/Unified.h>
#include <inr/Platform/Standard.h>

#ifdef INERTIA_POSIX
#include <inr/Platform/POSIX.h>
#endif

namespace inr{

write_return uwrite(const void* buf, size_t size, size_t n, const fs::ufile& stream) noexcept{
    switch(stream.get_type()){
        case fs::ufile::handle_type::POSIX:
            #ifdef INERTIA_POSIX
            {
                ssize_t bytes_written = posix::write(stream.asPOSIX(), buf, size * n);
                return bytes_written < 0 ? 0 : bytes_written / size;
            }
            #else
            return 0;
            #endif
        case fs::ufile::handle_type::WINDOWS:
            return 0; // unsupported.
        case fs::ufile::handle_type::STANDARD:
            return standard::fwrite(buf, size, n, stream.asFILE());
        default:
            return 0;
    }
}

}