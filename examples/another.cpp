
int x = 20;
extern void foo();

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

    foo();
    x = 40;
    return x;
}