; sample program iterating thought program arguments
source_filename = "ll_args.ll"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

declare i32 @printf(i8*, ...)

;                           12345678901234--5--                                                            
@fmt = constant [15 x i8] c"argv[%i] = %s\0A\00"

define i32 @main(i32 %argc, i8** %argv ){
  %i.ptr = alloca i32 
  store i32 0, i32* %i.ptr

  ;                                                         /--------- This means to dereference the ptr
  ;                                                         |
  ;                                                         |     /--- This means 0 index, a value of
  ;                                                         |     |    1 would give use the "rgv..." string
  ;                                                        \/    \/
  %fmt-aux = getelementptr [15 x i8], [15 x i8]* @fmt, i32 0, i32 0

  br label %loop
loop:
  %i = load i32, i32* %i.ptr

  %arg.ptr = getelementptr i8*, i8** %argv, i32 %i
  %arg = load i8*, i8** %arg.ptr

  call i32(i8*,...) @printf( i8* %fmt-aux, i32 %i, i8* %arg )

  %i_next = add i32 %i, 1
  store i32 %i_next, i32* %i.ptr

  %cond = icmp ult i32 %i_next, %argc
  br i1 %cond, label %loop, label %end_loop

end_loop:

  ret i32 0

}
