extern int printf(const char*, ...);

int Strlen(const char* str){
	const char* iter = str;
	for(; *iter != 0; ++iter);
	return iter - str;
}

int main(){
	const char* str = "hello world";
	int len = Strlen(str);
	printf("strlen(%s) = %d\n", str, len);
}
