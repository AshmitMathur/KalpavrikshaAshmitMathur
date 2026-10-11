/*
    Created By: Ashmit Mathur
    Assignment: Student Performance Analyzer
    Date: 11/10/2026    
*/
#include <stdio.h>

// Constants 
#define MAX_STUDENTS 100
#define NUM_SUBJECTS 3.0f
#define MIN_MARKS 0
#define MAX_MARKS 100

// Global Student Count
int studentCount = 0;

struct Student {
    int rollNo;
    char name[100];
    int marks1, marks2, marks3;
};

// Function ProtoTypes
int RollNoExists(struct Student students[], int currIdx, int rollNo);
int isValidName(char *name);
int calculateTotal(int m1, int m2,int m3);
float calculateAverage(int total);
char assignGrade(float average);
int getStars(char grade);
void printRollNo(struct Student students[], int index, int totalStudents);
void Input(int n, struct Student students[]);
void Output(int n, struct Student students[]);

// Function to Check if Roll No already Exists
int RollNoExists(struct Student students[], int currIdx, int rollNo){
    for(int i=0 ; i<currIdx ; i++){
        if(students[i].rollNo == rollNo) return 1;
    }
    return 0;
}

// Function to check for Valid Name
int isValidName(char *name){
    for(int i=0 ; name[i] != '\0'; i++){
        if((name[i] >= 'A' && name[i] <= 'Z') || (name[i] >= 'a' && name[i] <= 'z')){
            return 1;
        }
    }
    return 0;
}

// Calculate Total Marks for Student
int calculateTotal(int m1,int m2,int m3){
    return m1 + m2 + m3;
}

// Calculate Average for Student
float calculateAverage(int total){
    return total / NUM_SUBJECTS;
}

// Function to Assign Grade according to Average
char assignGrade(float average){
    if(average >= 85.0f) return 'A';
    else if(average >= 70.f) return 'B';
    else if(average >= 50.f) return 'C';
    else if(average >= 35.0f) return 'D';
    return 'F';
}

// Function to get Stars based on Grade
int getStars(char grade){
    switch(grade){
        case 'A' : return 5;
        case 'B' : return 4;
        case 'C' : return 3;
        case 'D' : return 2;
        default: break;
    }
    return -1;
}

// Recursive function to print Roll Numbers
void printRollNo(struct Student students[],int index, int totalStudents){
    if(index == totalStudents){
        printf("\n");
        return;
    }
    printf("%d ", students[index].rollNo);
    printRollNo(students, index + 1, totalStudents);
}

// Function to take input
void Input(int n, struct Student students[]){
    for(int i=0 ; i<n ; i++){
        scanf("%d %99s %d %d %d", &students[i].rollNo, students[i].name, 
        &students[i].marks1, &students[i].marks2, &students[i].marks3);

        if(RollNoExists(students, i, students[i].rollNo)){
            printf("Error: Roll No already exists. Please re-enter for this student.\n");
            i--;
            continue;
        }

        if(!isValidName(students[i].name)){
            printf("Error: Invalid name. Please re-enter for this student.\n");
            i--;
            continue;
        }

        if(students[i].marks1 < MIN_MARKS || students[i].marks1 > MAX_MARKS || 
        students[i].marks2 < MIN_MARKS || students[i].marks2 > MAX_MARKS || 
        students[i].marks3 < MIN_MARKS || students[i].marks3 > MAX_MARKS){
            printf("Error: Marks must be between %d and %d. Please re-enter.\n", MIN_MARKS, MAX_MARKS);
            i--;
            continue;
        }
        studentCount++;
    }
}

// Function to take output
void Output(int n, struct Student students[]){
    for(int i=0 ; i<n ; i++){
        int total = calculateTotal(students[i].marks1, students[i].marks2, students[i].marks3);
        float avg = calculateAverage(total);
        char grade = assignGrade(avg);

        printf("Roll: %d\n", students[i].rollNo);
        printf("Name: %s\n", students[i].name);
        printf("Total: %d\n", total);
        printf("Average: %.2f\n", avg);
        printf("Grade: %c\n", grade);

        if(avg < 35.0f) continue;
        int stars = getStars(grade);
        printf("Performance: ");
        for(int i=0 ; i<stars ; i++){
            printf("*");
        }
        printf("\n \n");
    }
printf("List of Roll Numbers (via recursion): ");
printRollNo(students, 0, n);
}

int main(){
    int n;
    scanf("%d", &n);

    if(n > MAX_STUDENTS || n < 1){
        printf("Invalid Number of Students");
        return 0;
    }
    struct Student students[MAX_STUDENTS];
    Input(n, students);
    Output(n, students);
    return 0;
}