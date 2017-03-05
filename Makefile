LDFLAGS=-lm

%.ll: %.c
	@clang -emit-llvm -S -c $< -o $@

%: %.ll
	@clang $(LDFLAGS) $< -o $@
