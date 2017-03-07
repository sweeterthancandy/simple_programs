
; implenting string functions
;       strlen
;       strcmp
 

source_filename = "expr.ll"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

declare i32 @printf(i8*, ...)

;  Calculates the lengh of a null terminalated string
define i32 @Strlen(i8* %str){
  %iter.ptr = alloca i8*
  store i8* %str, i8** %iter.ptr

  br label %Loop

Loop:
  %iter = load i8*, i8** %iter.ptr
  
  ; get the char, by first derefercing the iter
  %char.ptr = getelementptr i8, i8* %iter, i32 0
  %char = load i8, i8* %char.ptr

  %cond = icmp eq i8 %char, 0

  br i1 %cond, label %Return, label %Next
Next:

  ; to increment to pointer, reference the next char location
  %iter.next = getelementptr i8, i8* %iter, i32 1
  store i8* %iter.next, i8** %iter.ptr

  br label %Loop

Return:

  ; now calculate the pointer difference
  %result = load i8*, i8** %iter.ptr
  %end = ptrtoint i8* %result to i64
  %start = ptrtoint i8* %str to i64
  %diff = sub i64 %end, %start

  ; return the value
  %diff_32 = trunc i64 %diff to i32
  ret i32 %diff_32
}

@driver_print_dbg = constant [ 4 x i8 ] c"%s\0A\00"
;                                           1234567890123456--7--
@driver_strlen_dbg = constant [ 17 x i8 ] c"strlen(%s) = %d\0A\00"

define void @driver(i8* %expr){
  %driver_strlen_dbg_aux = getelementptr [ 17 x i8 ],[ 17 x i8 ]* @driver_strlen_dbg, i32 0, i32 0
  %driver_print_dbg_aux = getelementptr [ 4 x i8 ],[ 4 x i8 ]* @driver_print_dbg, i32 0, i32 0

  call i32(i8*, ...) @printf(i8* %driver_print_dbg_aux, i8* %expr)

  %result_strlen = call i32(i8*) @Strlen(i8* %expr)
  
  call i32(i8*, ...) @printf(i8* %driver_strlen_dbg_aux, i8* %expr, i32 %result_strlen)

  ret void

}

;                                     123456789012--
@test_expr_0 = constant [ 12 x i8 ] c"hello world\00"

define i32 @main(){
  %expr_aux = getelementptr [ 12 x i8 ],[ 12 x i8 ]* @test_expr_0, i32 0, i32 0

  call void(i8*) @driver( i8* %expr_aux )

  ret i32 0
}
