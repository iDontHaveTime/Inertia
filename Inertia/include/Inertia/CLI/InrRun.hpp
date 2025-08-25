#ifndef INERTIA_INRRUN_HPP
#define INERTIA_INRRUN_HPP

namespace Inertia{
    class InrRun{
    public:
        static int execute_command(int argc, const char* const argv[], const char* const envp[] = nullptr) noexcept;
    };
}

#endif // INERTIA_INRRUN_HPP
