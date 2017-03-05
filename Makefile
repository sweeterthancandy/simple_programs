LDFLAGS=-lm
CFLAGS=-ggdb3

%.ll: %.c
	@clang -emit-llvm -S -c $< -o $@

%: %.ll
	@clang $(LDFLAGS) $< -o $@
