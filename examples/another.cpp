
int x = 20;
extern void hello();

int main(){
    auto ptr = hello;

    ptr();

    hello();
    x = 40;
    return 0;
}