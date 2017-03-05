source_filename = "fib.ll"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

declare i32 @printf(i8*, ...) #1

; calc the nth fibanaci number 
;     fib(n) -> 1 where n == 0
;            -> 1 where n == 1
;            -> fib(n-2) + fib(n-1) otherwise
;define i32 fib(i32){
;  ; get the val argument
;  %val = alloca i32
;  store i32 %0, i32* %val
;  %cond = icmp slt i32 %val, i32 1
;  br i1 %cond, label %Return_One, label %Recursive
;Return_One:
;  ret i32 1
;Recursive:
;  %minus_one
;  %minus_1 = call i32 @fib(i32
;}

define i32 @print_int(i32 %val){
  call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %val)
  ret i32 0
}

define i32 @add1(i32 %val){
  %result = add i32 %val, 1
  ret i32 %result
}


define void @test_add1(){
  %add1_1 = call i32(i32) @add1(i32 1)
  call i32(i32) @print_int(i32 %add1_1)
  %add1_32 = call i32(i32) @add1(i32 32)
  call i32(i32) @print_int(i32 %add1_32)
  ret void
}


define i32 @return_abs(i32 %val){
  %is_pos = icmp sgt i32 %val, 0
  br i1 %is_pos, label %Ret_Identity, label %Ret_Neg
Ret_Identity:
  ret i32 %val
Ret_Neg:
  %inv = mul i32 %val, -1
  ret i32 %inv
}

define void @test_return_abs(){
  %abs_p_0 = call i32(i32) @return_abs(i32 0 )
  call i32(i32) @print_int(i32 %abs_p_0)
  %abs_p_2 = call i32(i32) @return_abs(i32 2 )
  call i32(i32) @print_int(i32 %abs_p_2)
  %abs_n_2 = call i32(i32) @return_abs(i32 -2 )
  call i32(i32) @print_int(i32 %abs_n_2)
  ret void
}

define i32 @fib(i32 %val){
  %cond = icmp slt i32 %val, 2
  br i1 %cond, label %base, label %rec
base:
  ret i32 1
rec:
  %m1 = sub i32 %val, 1
  %m2 = sub i32 %val, 2
  %fib_m1 = call i32(i32) @fib( i32 %m1 )
  %fib_m2 = call i32(i32) @fib( i32 %m2 )
  %result = add i32 %fib_m1, %fib_m2
  ret i32 %result
}

define void @debug_fib(i32 %val){
  %result = call i32(i32) @fib(i32 %val)
  call i32(i32) @print_int(i32 %result)
  ret void
}

define i32 @main(){

  ;%result = alloca i32
  ;store i32 1, i32* %result
  ;%aux = load i32, i32* %result
  ;call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %aux)
  ;call void() @test_add1()
  ;call void() @test_return_abs()

  %i.ptr = alloca i32
  store i32 0, i32* %i.ptr

  br label %BeginLoop

BeginLoop:
  %i = load i32, i32* %i.ptr

  %cond = icmp slt i32 %i, 10
  call void(i32) @debug_fib(i32 %i)

  %i.next = add i32 %i, 1
  store i32 %i.next, i32* %i.ptr

  br i1 %cond, label %BeginLoop, label %Break

Break:

  ret i32 0
}

