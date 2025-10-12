#include "inr/Support/inrint.hpp"
#include "inr/Support/inrstream.hpp"

int main(){
    inr::inrint ap_stack(1, 32);
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
