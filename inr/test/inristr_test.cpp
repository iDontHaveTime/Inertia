#include "inr/Defines/FileDef.hpp"
#include "inr/Support/File.hpp"
#include <cstdio>

/**
 * Expected output:
 * 34
 * memory file contents. 1 2 3. ASCII
 * Standard succeeded.
 *
 * if posix:
 *
 * 34
 * memory file contents. 1 2 3. ASCII
 * POSIX succeeded.
 *
 */

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

#ifdef INERTIA_POSIX
#include <fcntl.h>
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