#include <inr/Support/Terminal.h>
#include <sys/ioctl.h>

namespace inr::terminal {

/// The fd MUST be checked that its displayed.
static inline struct winsize getTerminalInfo(const stream& s, bool& success) {
    struct winsize w;
    int r = ioctl(s.getHandle(), TIOCGWINSZ, &w);

    if(r == -1) {
        success = false;
        return {};
    }

    success = true;
    return w;
}

int getWidth(stream& s) {
    if(!s.isDisplayed()) return 0;

    bool success;
    auto w = getTerminalInfo(s, success);

    if(!success) return -1;

    return w.ws_col;
}

} // namespace inr::terminal