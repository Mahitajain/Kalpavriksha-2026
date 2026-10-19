#include<stdio.h>

#define MAX_CHARACTERS 20

// function to get number of characters from the user
int getCharacterInput(){
    int n;
    while(1){
        printf("Enter the number of characters you want to enter (1-%d): ", MAX_CHARACTERS);
        scanf("%d", &n);
        while(getchar() != '\n');   // clears leftover input
        if(n >= 1 && n <= MAX_CHARACTERS){
            break;
        }
        printf("Invalid input. Please enter a number between 1 and %d.\n", MAX_CHARACTERS);
    }
    return n;
}

// function to read character inputs from the user
void readCharacterInput(char characters[], int n){
    while(1){
        printf("Enter %d characters : ", n);
        for(int i = 0 ; i < n ; i++){
            char ch;
            scanf(" %c", &ch);  // skips all whitespaces before the character was entered
            characters[i] = ch;
        }

        // checking if anything other than whitespace is left in the input
        int extra = 0;
        int currchar;   // using int because EOF is negative
        while((currchar = getchar()) != '\n' && currchar != EOF){
            if(currchar != ' ' && currchar != '\t'){
                extra = 1;
                break;
            }
        }

        if(extra == 0){
            break;   // exactly n characters were entered
        }

        // clear the rest of the line before asking again
        currchar = getchar();
        while(currchar != '\n' && currchar != EOF){
            currchar = getchar(); //read next char
        }
        printf("Invalid input. Enter only %d characters.\n", n);
    }
}

int main(){
    int total = 0;
    int n = getCharacterInput();
    char characters[MAX_CHARACTERS];
    readCharacterInput(characters, n);

    // printing the result in the desired output format
    for(int i = 0 ; i < n; i++){
        printf("%c ", characters[i]);
        printf("%d ", (int)characters[i]);
        total += (int)characters[i];
        printf("%c\n", characters[i] + 1);
    }
    printf("Sum = %d", total);
    return 0;
}