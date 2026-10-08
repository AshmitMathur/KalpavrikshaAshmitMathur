/*
    Created By: Ashmit Mathur
    Assignment: Student Performance Analyzer
    Date: 08/10/2026    
*/
#include <stdio.h>
int studentCount = 0;

struct Student {
    int rollNo;
    char name[100];
    int marks1, marks2, marks3;
    int total;
    float average;
    char grade;
    int stars;
};

void calculatePerformance(struct Student *s){
    s->total = s->marks1 + s->marks2 + s->marks3;
    s->average = s->total / 3.0f;

    if(s->average >= 85.0f){
        s->grade = 'A';
        s->stars = 5;
    } else if(s->average >= 70.0f){
        s->grade = 'B';
        s->stars = 4;
    } else if(s->average >= 50.0f){
        s->grade = 'C';
        s->stars = 3;
    } else if(s->average >= 35.0f){
        s->grade = 'D';
        s->stars = 2;
    } else{
        s->grade = 'F';
        s->stars = 0;
    }
}

void printRollNoNumbers(struct Student students[], int index, int totalStudents){
    if(index == totalStudents){
        printf("\n");
        return;
    }
    printf("%d ", students[index].rollNo);
    printRollNoNumbers(students, index + 1, totalStudents);
}

void Input(int n, struct Student students[100]){
    for(int i=0 ; i<n ; i++){
        scanf("%d %99s %d %d %d", &students[i].rollNo,
        students[i].name, &students[i].marks1, &students[i].marks2, 
        &students[i].marks3);

        calculatePerformance(&students[i]);
        studentCount++;
    }
}

void Output(int n, struct Student students[100]){
    for(int i=0 ; i<n ; i++){
        printf("Roll: %d\n", students[i].rollNo);
        printf("Name: %s\n", students[i].name);
        printf("Total: %d\n", students[i].total);
        printf("Average: %.2f\n", students[i].average);
        printf("Grade: %c\n", students[i].grade);

        if(students[i].average < 35.0f) continue;
        printf("Performance: ");
        for(int j=0 ; j<students[i].stars; j++){
            printf("*");
        }
        printf("\n");
    }
    printf("List of Roll Numbers (via recursion): ");
    printRollNoNumbers(students, 0, n);
}

int main(){
    int n;
    scanf("%d", &n);
    if(n > 100 || n < 1){
        printf("Invalid Number of Students");
        return 0;
    }
    struct Student students[100];
    Input(n, students);
    Output(n, students);
    return 0;
}