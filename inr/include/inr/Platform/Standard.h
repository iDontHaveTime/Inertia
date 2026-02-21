#ifndef INERTIA_PLATFORM_STANDARD_H
#define INERTIA_PLATFORM_STANDARD_H

#include <cstdio>

namespace inr::standard{

    using ::fwrite;
    using ::fread;

    using write_return = size_t;
    using read_return = size_t;
}

#endif // INERTIA_PLATFORM_STANDARD_H
