#include <inr/Support/Stream.h>

int main(){
    inr::out<<"Hello, World!\n";
    inr::out<<"stdout buffer size: "<<inr::out.get_buffer_size()<<inr::endl;
    inr::out<<"buffer index: "<<inr::out.get_buffer_index()<<'\n';
    return 0;
}