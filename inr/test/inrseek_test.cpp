#include "inr/Defines/FileDef.hpp"
#include "inr/Support/Cexpr.hpp"
#include "inr/Support/File.hpp"
#include <cstdio>

int standard(){
    FILE* f = fopen("inr/test/resources/filetest.txt", "rb");
    if(!f){
        perror("fopen");
        return 1;
    }

    inr::inrfile file(f, inr::fs::OpeningType::Read, true);

    char buff[0x100];

    size_t bytes_read = file.read(buff, 1, 10);

    printf("Bytes read: %zu\nPosition: %lu\n", bytes_read, file.tell());

    FILE* f2 = fopen("inr/test/resources/writetest.txt", "wb");
    if(!f2){
        perror("fopen");
        return 1;
    }
    inr::inrfile file2(f2, inr::fs::OpeningType::Write, true);
    size_t bytes_wrote = file2.write("Hello", 1, inr::cexpr_strlen("Hello"));

    printf("Bytes Wrote: %zu\nPosition: %lu\n", bytes_wrote, file2.tell());
    return 0;
}

#ifdef INERTIA_POSIX
#include <unistd.h>
#include <fcntl.h>
int posix(){
    int fd = open("inr/test/resources/filetest.txt", O_RDONLY);
    if(fd < 0){
        perror("open");
        return 1;
    }

    inr::inrfile file(fd, 0x2000, inr::fs::OpeningType::Read, true);

    char buff[0x100];
    size_t bytes_read = file.read(buff, 1, 10);

    printf("Bytes read: %zu\nPosition: %lu\n", bytes_read, file.tell());

    int fd2 = open("inr/test/resources/writetest.txt", O_WRONLY | O_TRUNC);
    if(fd2 < 0){
        perror("open");
        return 1;
    }

    inr::inrfile file2(fd2, 0x2000, inr::fs::OpeningType::Write, true);

    size_t bytes_wrote = file2.write("Hello", 1, inr::cexpr_strlen("Hello"));

    printf("Bytes Wrote: %zu\nPosition: %lu\nReal Position: %lu\n", bytes_wrote, file2.tell(), lseek(fd2, 0, SEEK_CUR));
    return 0;
}
#endif

int main(){
    if(standard()){
        printf("Standard failed.\n");
        return 1;
    }
    printf("Standard succeeded.\n");

    #ifdef INERTIA_POSIX
    if(posix()){
        printf("POSIX failed.\n");
        return 1;
    }
    printf("POSIX succeeded.\n");
    #endif


    return 0;
}