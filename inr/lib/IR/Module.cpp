#include <inr/IR/Function.h>
#include <inr/IR/Module.h>

#include "inr/IR/Instruction.h"

namespace inr {

void Module::print(raw_stream& os) const {
    os << "; Module = " << name_ << '\n';

    for(const Function& f : functions_) {
        os << "define " << *f.getType()->getReturn() << ' ' << f << '(';
        for(size_t i = 0; i < f.getArgs().size(); i++) {
            if(i) os << ", ";
            os << *f.getArgs()[i].getType();
            if(!f.getArgs()[i].getName().empty()) {
                os << ' ' << *f.getArg(i);
            }
        }
        os << ") {\n";

        for(const Block& b : f.blocks_) {
            os << b.getName() << ":\n";

            for(const Instruction& ins : b.instructions_) {
                os << '\t' << ins << ";\n";
            }
        }

        os << "}\n";
    }
}

} // namespace inr