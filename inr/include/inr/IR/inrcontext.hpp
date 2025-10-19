#ifndef INERTIA_INRCONTEXT_HPP
#define INERTIA_INRCONTEXT_HPP


/**
 * @file inr/IR/inrcontext.hpp
 * @brief Inertia's context class.
 *
 * This header contains Inertia's IR context class.
 *
 **/

namespace inr{

    /**
     * @brief An essential class for storing context in one class.
     *
     * Keeps one IR context, is thread safe.
     */
    class inrContext{
    public:

        /* Constructors / Operators. */
        inrContext() noexcept = default;

        inrContext(const inrContext&) = delete;
        inrContext& operator=(const inrContext&) = delete;

        inrContext(inrContext&&) = default;
        inrContext& operator=(inrContext&&) = default;

        /* Destructor. */
        ~inrContext() noexcept = default;
        /* End of Constructors | Operators | Destructor. */

        /* Start of fields. */
    private:


    public:
        /* End of fields. */
    };

}

#endif // INERTIA_INRCONTEXT_HPP
