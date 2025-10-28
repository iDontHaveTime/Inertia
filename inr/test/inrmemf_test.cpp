#include "inr/Defines/FileDef.hpp"
#include "inr/Support/File.hpp"
#include "inr/Support/Stream.hpp"
#include <cstdio>

static_assert(inr::fs::opening_type_read(inr::fs::OpeningType::Read), "read is wrong");

int test_standard(){
    FILE* f = fopen("inr/test/resources/filetest.txt", "r");
    if(!f){
        return 1;
    }

    inr::inrfile to_map(f, inr::fs::OpeningType::Read, true);
    if(to_map){
        inr::out<<"File opened."<<inr::endl;
    }
    else{
        return 1;
    }

    inr::inr_mem_file mf = to_map.fmem_open();

    if(!mf){
        inr::out<<mf.get_errc()<<inr::endl;
        inr::out<<"Memory file failed."<<inr::endl;
        return 1;
    }

    inr::out.write((const char*)mf.get_file(), mf.size())<<inr::endl;

    return 0;
}

#ifdef INERTIA_POSIX
#include <fcntl.h>
#include <unistd.h>
int test_posix(){
    int fd = open("inr/test/resources/filetest.txt", O_RDONLY);
    if(fd < 0){
        return 1;
    }

    inr::inrfile to_map(fd, 4096, inr::fs::OpeningType::Read, false);
    if(to_map){
        inr::out<<"File opened."<<inr::endl;
    }
    else{
        return 1;
    }

    inr::inr_mem_file mf = to_map.fmem_open();

    if(!mf){
        inr::out<<mf.get_errc()<<inr::endl;
        inr::out<<"Memory file failed."<<inr::endl;
        return 1;
    }

    inr::out.write((const char*)mf.get_file(), mf.size())<<inr::endl;

    close(fd);

    return 0;
}
#endif

int main(){

    #ifdef INERTIA_POSIX
    if(test_posix()){
        printf("POSIX failed.\n");
        return 1;
    }
    printf("POSIX succeeded.\n");
    #endif

    if(test_standard()){
        printf("Standard failed.\n");
        return 1;
    }
    printf("Standard succeeded.\n");
    
    return 0;
}