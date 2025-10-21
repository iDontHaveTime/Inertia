#include "inr/Defines/inrfiledef.hpp"
#include "inr/Support/inrfile.hpp"
#include <cstdio>
#include <fcntl.h>

int standard(){
    FILE* f = fopen("inr/test/resources/filetest.txt", "rb");
    if(!f){
        perror("fopen");
        return 1;
    }

    inr::inrfile file(f, inr::fs::OpeningType::Read, true);

    char buff[0x100];

    size_t bytes_read = file.read(buff, 1, 0x100);

    if(bytes_read == 0){
        printf("Error reading file.\n");
        return 1;
    }
    else{
        printf("%zu\n", bytes_read);
    }

    fwrite(buff, 1, bytes_read, stdout);
    putchar('\n');

    return 0;
}

int posix(){
    int fd = open("inr/test/resources/filetest.txt", O_RDONLY);
    if(fd < 0){
        perror("open");
        return 1;
    }

    inr::inrfile file(fd, 0x2000, inr::fs::OpeningType::Read, true);

    char buff[0x100];
    size_t bytes_read = file.read(buff, 1, 0x100);

    if(bytes_read == 0){
        printf("Error reading file.\n");
        return 1;
    }
    else{
        printf("%zu\n", bytes_read);
    }

    fwrite(buff, 1, bytes_read, stdout);
    putchar('\n');

    return 0;
}

int main(){
    if(standard()){
        printf("Standard failed.\n");
        return 1;
    }
    printf("Standard succeeded.\n");

    if(posix()){
        printf("POSIX failed.\n");
        return 1;
    }
    printf("POSIX succeeded.\n");

    return 0;
}