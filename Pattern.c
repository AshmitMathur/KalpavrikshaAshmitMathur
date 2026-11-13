#include <stdio.h>

int main(){
    int n;
    scanf("%d", &n);
    int LEN = 2 * n - 1;

    for(int i=0 ; i<LEN ; i++){
        for(int j=0 ; j<LEN ; j++){
            int top = i;
            int left = j;
            int right = LEN - 1 - j;
            int bottom = LEN - 1 - i;

            int minn = top;
            if(bottom < minn) minn = bottom;
            if(left < minn) minn = left;
            if(right < minn) minn = right;

            printf("%d ", n - minn);
        }
        printf("\n");
    }
    return 0;
}