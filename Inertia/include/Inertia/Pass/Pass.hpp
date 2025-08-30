#ifndef INERTIA_PASS_HPP
#define INERTIA_PASS_HPP

#include "Inertia/IR/Frame.hpp"

namespace Inertia{
    class Pass{
    protected:
        Frame* frame;
    public:
        Pass(Frame* _frame) noexcept : frame(_frame){};

        virtual bool run() = 0;

        virtual ~Pass() noexcept = default;
    };
}

#endif // INERTIA_PASS_HPP
