#include "inr/Support/inrarena.hpp"
#include "inr/Support/inrstream.hpp"
#include "inr/Support/inrvector.hpp"

/**
 * Expected output, assuming int is 4 bytes:
 * 42. 16 times
 * Arena allocator has freed everything.
 * Expecting 24 after allocation. Current: 4
 * Allocated. Current: 24
 * 16
 */

int main(){
    /* Make an arena. */
    inr::arena_allocator<0x2000> arena;

    for(int i = 0; i < 16; i++){
        /* Make an integer with value of 42. */
        int* x = arena.alloc<int>(42);

        /* Print the integer. */
        inr::out<<*x<<inr::endl;
        /* Free the integer. */
        arena.free(x);
    }

    if(arena.current() == 0){
        inr::out<<"Arena allocator has freed everything."<<inr::endl;
    }

    int* i = arena.alloc<int>(0);
    inr::out<<"Expecting 24 after allocation. Current: "<<arena.current()<<inr::endl;

    void* p = arena.alloc_raw(8, 16);

    inr::out<<"Allocated. Current: "<<arena.current()<<inr::endl;

    arena.free_raw(p, 8);
    arena.free(i);

    inr::out<<arena.current()<<inr::endl;
    arena.reset();

    inr::out<<arena.current()<<inr::endl;

    inr::inr_vec<int> vec(arena);

    for(int i = 0; i < 100; i++){
        vec.emplace_back(1);
    }

    inr::out<<arena.current()<<inr::endl;

    vec.clear(true);

    inr::out<<arena.current()<<inr::endl;

    return 0;
}