#include <inr/ISel/LinearISel.h>
#include <inr/ISel/x86/LinearISelPass.h>

namespace inr {

std::unique_ptr<ISelPass> TripleLinearISel::get(inr::Triple T) {
    switch(T.getArch()) {
        case Triple::Arch::Unknown:
            return std::unique_ptr<ISelPass>(nullptr);
        case Triple::Arch::x86_64:
            return std::unique_ptr<ISelPass>(new x86::LinearISelPass());
    }
}

} // namespace inr