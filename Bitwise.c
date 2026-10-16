#include <stdio.h>

void calculate_the_maximum(int n,int k){
    int maxAnd = 0;
    int maxOr = 0;
    int maxXor = 0;

    for(int i=1 ; i<=n ; i++){
        for(int j=i+1 ; j<=n ; j++){
            int andRes = i & j;
            int orRes = i | j;
            int xorRes = i ^ j;

            if(andRes < k && andRes > maxAnd){
                maxAnd = andRes;
            }
            if(orRes < k && orRes > maxOr){
                maxOr = orRes;
            }
            if(xorRes < k && xorRes > maxXor){
                maxXor = xorRes;
            }
        }
    }
    printf("%d\n%d\n%d\n", maxAnd, maxOr, maxXor);
}
int main(){
    int n, k;
    scanf("%d %d", &n, &k);
    calculate_the_maximum(n, k);
    return 0;
}