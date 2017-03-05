/*
 * Small prototype implementing newton numerical method
 * https://en.wikipedia.org/wiki/Newton's_method
 */
extern int printf(const char*, ...);
extern double fabs(double); 
extern double cos(double);

double diff( double(*f)(double), double x, double epsilon ){
        return ( f(x + epsilon / 2 ) - f(x - epsilon / 2 ) ) / epsilon;
}


typedef struct model{
        const char* title;
        double x0;
        double(*f)(double);
        double epsilon;
        double diff_epsilon;
}model;

double f1(double x){
        return x*x - 612;
}
static model example_0 = {
        " solve x^2 = 612   {x(0) = 10}",
        10,
        f1,
        1e-5,
        1e-5};


double f2(double x){
        return cos(x) - x*x*x;
}
static model example_1 = {
        " solve cos(x) = x^3     {x(0) = 0.5}",
        0.5,
        f2,
        1e-5,
        1e-5};

void driver( model* m){
        printf("%s\n", m->title);

        double x = m->x0;

        printf("%-10s,%-10s,%-10s,%-10s\n", "x_n", "x_{n+1}", "|.|", "residue");

        for(;;){
                double next = x - m->f(x) / diff(m->f, x, m->diff_epsilon );
                double d = fabs( next - x );
                double res = m->f(next);
                printf("%-10.4f,%-10.4f,%-10.4f,%-10.4f\n", x, next, d, res);

                if( d < m->epsilon )
                        break;
                x = next;
        }
}


int main(){
        driver( &example_0 );
        driver( &example_1 );

}
