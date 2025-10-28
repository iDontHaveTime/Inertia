#include "inr/Support/Alloc.hpp"
#include "inr/Support/Stream.hpp"

/**
 * Expected output:
 * 20
 *
 */

int main(){
    /* Create the allocator. */
    inr::basic_allocator alloc;

    /* Allocate an int. */
    int* x = alloc.alloc<int>();

    /* Set the int. confirm no segfaults. */
    *x = 20;

    /* Output the value of the integer. */
    inr::out<<*x<<inr::endl;

    /* Free the integer. */
    alloc.free(x);

    return 0;
}