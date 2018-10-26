
extern "C" int printf(const char*, ...);

struct foo{
        int val;
};

int main(){
        foo f = { 23 };
        printf("val = %d\n", f.val);
}
