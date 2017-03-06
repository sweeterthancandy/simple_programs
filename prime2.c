
extern int printf(const char* fmt, ...);

int main(){
        int max_prime = 20;
        for( int i = 3 ; i <= max_prime ; i += 2)
        {
                int flag = 0;
                for( int j = 3; j*j <= i ; j += 2 )
                {
                        if( i % j == 0 )
                        {
                                flag = 1;
                                break;
                        }
                }
                if( flag != 1 )
                {
                        printf("%d\n", i);
                }
        }
}

