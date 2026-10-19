#include<stdio.h>

#define MAX_CHARACTERS 20

// function to  get number of characters from the user
int getCharacterInput(){
    int n ;
    while(1){
        printf("Enter the number of characters you want to enter (1-%d): ",MAX_CHARACTERS);
        scanf("%d" , &n);
        if(n >= 1 && n <= MAX_CHARACTERS){
            break;
        }
        printf("Invalid input. Please enter a number between 1 and %d.\n", MAX_CHARACTERS);
    }
    return n;
}

// function to read character inputs from the user

void readCharacterInput(char characters[],int n){
    for(int i = 0 ; i < n ;i++){
        char ch;
        scanf(" %c" , &ch);  // skips all whitespaces before the character was entered
        characters[i] = ch;
    }
}

int main(){
    int total = 0;
    int n = getCharacterInput();
    char characters[MAX_CHARACTERS];
    printf("Enter the characters : ");
    readCharacterInput(characters, n);
    // printing the result in the desired output format
    for(int i = 0 ; i < n; i++){
        printf("%c ",characters[i]);
        printf("%d ",(int)characters[i]);
        total += (int)characters[i];
        printf("%c\n",characters[i]+1);
    }
    printf("Sum = %d" ,total);
    return 0;
}