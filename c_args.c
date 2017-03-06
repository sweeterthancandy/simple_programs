
extern int printf(const char*, ...);

int main(int argc, char** argv){
        int i;
        for(i = 0; i != argc; ++i){
                printf("argv[%i] = %s\n", i, argv[i]);
        }
}
