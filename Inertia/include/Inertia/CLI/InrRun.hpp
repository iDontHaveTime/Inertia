#ifndef INERTIA_INRRUN_HPP
#define INERTIA_INRRUN_HPP

/* Very minimal class to run commands more efficiently than std::system(). */

namespace Inertia{
    /* Acts more as a namespace than a class. */
    class InrRun{
    public:
        /* Executes the command provided and returns the result. */
        static int execute_command(int argc, const char* const argv[], const char* const envp[] = nullptr) noexcept;
    };
}

#endif // INERTIA_INRRUN_HPP
