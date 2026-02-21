#include <inr/Support/Stream.h>
#include <inr/Platform/Platform.h>

namespace inr{

#ifndef INERTIA_POSIX
ostream out{stdout};
ostream err{stderr};
#else
ostream out{1, true};
ostream err{2};
#endif

static ostream& inr_flush_impl(ostream& os) noexcept{
    os.flush();
    return os;
}

static ostream& inr_endl_impl(ostream& os) noexcept{
    (os<<'\n').flush();
    return os;
}

ostream::manipulator flush = inr_flush_impl;
ostream::manipulator endl =  inr_endl_impl;

}