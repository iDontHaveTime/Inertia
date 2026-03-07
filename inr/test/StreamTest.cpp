#include <inr/Support/Stream.h>

int main() {
    inr::outs() << "Hello, World!" << '\n';

    inr::outs().flush();

    inr::errs() << "Errs test, floating point: " << 3.14f << " Integers: " << 42
                << '\n';

    inr::errs() << "Simulating error message:\n";

    (inr::errs().changeColor(inr::Colors::RED, true, false) << "error: ")
            .resetColor()
        << "just a test\n";

    return 0;
}