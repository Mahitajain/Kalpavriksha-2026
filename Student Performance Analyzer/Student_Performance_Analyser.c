#include <stdio.h>

#define MAX_STUDENTS 100
#define NAME_SIZE 50
#define SUBJECTS 3

#define GRADE_A 85
#define GRADE_B 70
#define GRADE_C 50
#define GRADE_D 35

struct Student {
    unsigned int roll;
    char name[NAME_SIZE];
    unsigned short marks1;
    unsigned short marks2;
    unsigned short marks3;
};

unsigned int calculateTotal(struct Student student){
    return student.marks1 + student.marks2 + student.marks3;
}

float calculateAverage(unsigned int total){
    return total / (float)SUBJECTS;
}

char calculateGrade(float average){
    if (average >= GRADE_A){
        return 'A';
    }
    else if (average >= GRADE_B){
        return 'B';
    }
    else if (average >= GRADE_C){
        return 'C';
    }
    else if (average >= GRADE_D){
        return 'D';
    }
    else{
        return 'F';
    }
}

int getStars(char grade){
    if (grade == 'A'){
        return 5;
    }
    else if (grade == 'B'){
        return 4;
    }
    else if (grade == 'C'){
        return 3;
    }
    else if (grade == 'D'){
        return 2;
    }
    else{
        return 0;
    }
}

int inputStudentDetails(struct Student *student, unsigned int studentIndex){
    printf("\nEnter details for student %u:\n", studentIndex + 1);

    scanf("%u %49s %hu %hu %hu",&student->roll,student->name,&student->marks1,&student->marks2,&student->marks3);

    if (student->marks1 > 100 || student->marks2 > 100 || student->marks3 > 100){
        printf("Marks should be between 0 and 100.\n");
        return 0;
    }

    return 1;
}

void displayStudentDetails(struct Student student){
    unsigned int total;
    float average;
    char grade;

    total = calculateTotal(student);
    average = calculateAverage(total);
    grade = calculateGrade(average);

    printf("Roll: %u\n", student.roll);
    printf("Name: %s\n", student.name);
    printf("Total: %u\n", total);
    printf("Average: %.2f\n", average);
    printf("Grade: %c\n", grade);

    if (grade == 'F'){
        return;
    }

    int stars;

    stars = getStars(grade);

    printf("Performance: ");

    for (int starIndex = 0; starIndex < stars; starIndex++){
        printf("*");
    }

    printf("\n");
}

void printRollNumbers(struct Student students[], unsigned int studentCount, unsigned int index){
    if (index >= studentCount){
        return;
    }

    printf("%u", students[index].roll);

    if (index < studentCount - 1){
        printf(" ");
    }

    printRollNumbers(students, studentCount, index + 1);
}

int main(){
    unsigned int studentCount;

    printf("Enter number of students: ");
    scanf("%u", &studentCount);

    if (studentCount == 0 || studentCount > MAX_STUDENTS){
        printf("Invalid number of students.\n");
        return 1;
    }

    struct Student students[MAX_STUDENTS];

    for (unsigned int studentIndex = 0; studentIndex < studentCount; studentIndex++){
        if (!inputStudentDetails(&students[studentIndex], studentIndex)){
            return 1;
        }
    }

    printf("\n");

    for (unsigned int studentIndex = 0; studentIndex < studentCount; studentIndex++){
        displayStudentDetails(students[studentIndex]);
    }

    printf("List of Roll Numbers (via recursion): ");

    printRollNumbers(students, studentCount, 0);

    printf("\n");

    return 0;
}
