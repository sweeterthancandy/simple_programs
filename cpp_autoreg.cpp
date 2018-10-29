
extern "C" int printf(const char *fmt, ...);

int init(){
  printf("Hello today\n");
  return 0;
}


int A = init();

int main() {}
