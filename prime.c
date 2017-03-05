extern int printf(const char*, ...);

int is_prime(int val){
        int cand = 3;
        if( val % 2 == 0 ){
                return 0;
        }
        while( cand * cand <= val ){
                if( val % cand == 0 ){
                        return 0;
                }
                cand += 2;
        }
        return 1;
}

int main(){
        int iter = 2;
        int last = 30;
        while( iter < last ){
                if( is_prime(iter) ){
                        printf("%d\n", iter);
                }
                ++iter;
        }
}
