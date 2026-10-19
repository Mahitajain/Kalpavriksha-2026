#include<stdio.h>

void printTriangle(int n){
 for(int row = 1; row <= n ; row++){
        if(row % 2 == 0){
            for(int col = 1 ; col <= row ; col++){
                printf("# ");
            }
        }
        else{
            for(int col = 1 ; col <= row ; col++){
                printf("* ");
            }
        }
        printf("\n");
    }
}
int main(){
    int n;
    printf("Enter the number of rows you want in the triangle : ");
    scanf("%d", &n);
    printTriangle(n);
    return 0;
}