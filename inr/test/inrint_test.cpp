#include "inr/Support/inrint.hpp"
#include "inr/Support/inrstream.hpp"

/**
 * Expected output:
 * ap_stack:
 *     on stack: true
 *     capacity: 8
 *     binary: 00000000000000000000000000000001
 *     decimal: 1
 * ap_heap:
 *     on stack: false
 *     capacity: 12
 *     binary: 000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000110010
 *     decimal: 50
 */

int main(){
    /* Create a stack based apint. */
    inr::inrint ap_stack(1, 32);
    /* Create a heap based apint. */
    inr::inrint ap_heap(50, 96);

    inr::out<<"ap_stack:\n";
    (inr::out<<"\ton stack: ").write_bool(ap_stack.on_stack())<<'\n';
    inr::out<<"\tcapacity: "<<ap_stack.capacity()<<'\n';
    inr::out<<"\tbinary: ";
    ap_stack.print_binary(inr::out)<<'\n';
    inr::out<<"\tdecimal: ";
    ap_stack.print_decimal(inr::out)<<'\n';


    inr::out<<"ap_heap:\n";
    (inr::out<<"\ton stack: ").write_bool(ap_heap.on_stack())<<'\n';
    inr::out<<"\tcapacity: "<<ap_heap.capacity()<<'\n';
    inr::out<<"\tbinary: ";
    ap_heap.print_binary(inr::out)<<'\n';
    inr::out<<"\tdecimal: ";
    ap_heap.print_decimal(inr::out)<<'\n';

    return 0;
}
