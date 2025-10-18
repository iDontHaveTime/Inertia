#include "inr/Defines/inrfiledef.hpp"
#include "inr/Support/inrfile.hpp"
#include "inr/Support/inrstream.hpp"
#include <fcntl.h>
#include <unistd.h>

int main(){
    int fd = open("inr/test/resources/filetest.txt", O_RDONLY);
    if(fd < 0){
        return 1;
    }

    inr::inrfile to_map(fd, 4096, inr::fs::OpeningType::Read, false);
    if(to_map){
        inr::out<<"File opened."<<inr::endl;
    }

    inr::inr_mem_file mf = to_map.fmem_open();

    if(!mf){
        inr::out<<mf.get_errc()<<inr::endl;
        inr::out<<"Memory file failed."<<inr::endl;
    }

    inr::out.write(mf.get_file(), mf.size())<<inr::endl;

    close(fd);

    return 0;
}