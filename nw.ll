
source_filename = "nw.ll"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

declare i32 @printf(i8*, ...)


declare double @cos(double)
declare double @fabs(double)


; print a double to stdout
@print_double.fmt =  private unnamed_addr constant [4 x i8] c"%f\0A\00", align 1
define void @print_double(double %x){
  call i32(i8*, ...) @printf( i8* getelementptr inbounds ([4 x i8], [4 x i8]* @print_double.fmt, i32 0, i32 0), double %x)
  ret void
}

; Do numerical differntiaion at point x on f
;
;                /     epsilon \      /     epsilon \
;      f'(x) =  f|x +  ------- |  -  f|x -  ------- |
;                \        2    /      \        2    /
;                -------------------------------------
;                            epsilon
;
define double @diff(double(double)* %f, double %x, double %epsilon){
  %half_ep = fdiv double %epsilon, 2.0

  %x_add = fadd double %x, %half_ep
  %x_sub = fsub double %x, %half_ep
  
  %f_x_add = call double(double) %f(double %x_add)
  %f_x_sub = call double(double) %f(double %x_sub)

  %nume = fsub double %f_x_add, %f_x_sub
  %result = fdiv double %nume, %epsilon

  ret double %result
}


; run the newton numerical method
;
;                        f(x_n)
;    x_{n+1} = x_{n} -   --------
;                        f'(x_n)
;
@driver.fmt   =  private unnamed_addr constant [25 x i8] c"%-10.4f,%-10.4f,%-10.4f\0A\00", align 1
@driver.title =  private unnamed_addr constant [34 x i8] c"x_{n}     ,x_{n+1}   ,|.|       \0A\00", align 1
define void @driver(double(double)* %f, double %init, double %epsilon){
  %x.ptr = alloca double

  store double %init, double* %x.ptr

  call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([34 x i8], [34 x i8]* @driver.title, i32 0, i32 0) )
  
  br label %BeginLoop

BeginLoop:
  %x = load double, double* %x.ptr
  %f.x = call double(double) %f(double %x)
  %diff.f.x = call double(double(double)*,double,double) @diff( double(double)* %f, double %x, double %epsilon)
  %lparam = fdiv double %f.x, %diff.f.x
  %next = fsub double %x, %lparam
  %d = fsub double %x, %next
  %abs_d = call double(double) @fabs(double %d)


  
  call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([25 x i8], [25 x i8]* @driver.fmt, i32 0, i32 0), double %x, double %next, double %abs_d)


  store double %next, double* %x.ptr


  %cond = fcmp olt double %abs_d, %epsilon
  br i1 %cond, label %finish, label %BeginLoop

finish:

  
  ret void
}


;    f(x) = x^2 - 612
define double @f1(double %x){
  %xsq = fmul double %x, %x
  %result = fsub double %xsq, 612.0
  ret double %result
}
;    f(x) = cos(x) - x^3
define double @f2(double %x){
  %xx = fmul double %x, %x
  %xxx = fmul double %xx, %x
  %cosx = call double(double) @cos(double %x)
  %result = fsub double %cosx, %xxx
  ret double %result
}

define i32 @main(){
  call void(double(double)*, double, double) @driver(double(double)* @f1, double 10.0, double 1.0e-3 );
  call void(double(double)*, double, double) @driver(double(double)* @f2, double 0.5, double 1.0e-3 );
  ret i32 0
}
