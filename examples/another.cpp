
int x = 20;
extern void foo();

int hhh(char a, short b, int c, long d, long long e, char f, short g, int h, long i, long long j){
    return hhh(a,b,c,d,e,f,g,h,i,j);
}

int main(){
    auto ptr = foo;

    __asm__ volatile(
        "movq $16, %%rax"
        :
        :
        : "rax"
    );

    ptr();

    int x = 0;
    {
        int (is) = 2;
        x = is = 2;
    }

    x ^= x;

    x = hhh(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

    foo();
    x = 40;
    return x;
}