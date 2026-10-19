#include<stdio.h>

#define MAX_CHARACTERS 20

int main(){
    int n;
    int total = 0;
    printf("Enter the number of characters you want to enter: ");
    scanf("%d" , &n);
    char characters[MAX_CHARACTERS];
    printf("Enter the characters : ");
    for(int i = 0 ; i < n ;i++){
        char ch;
        scanf(" %c" , &ch);  // skips all whitespaces before the character was entered
        characters[i] = ch;
    }
    for(int i = 0 ; i < n; i++){
        printf("%c ",characters[i]);
        printf("%d ",(int)characters[i]);
        total += (int)characters[i];
        printf("%c\n",characters[i]+1);
    }
    printf("Sum = %d" ,total);
    return 0;
}