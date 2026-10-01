/* 
    Created By: Ashmit Mathur
    Date: 01/10/26
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define File_name "users.txt"

struct User{
    int id;
    char name[100];
    int age;
};

void CreateFile(){
    FILE *file = fopen(File_name, "a");
    if(file == NULL){
        printf("Error: Unable to Create File");
        return;
    }
    fclose(file);
}

int IdExists(int id){
    FILE *file = fopen(File_name, "r");
    struct User user;
    if(file == NULL) return 0;
    /* %d -> Reads Integer, | -> Literal Pipe Seperator
    %99 -> Reads up to 99 characters \n -> Consumes newline at end of Record*/
    while(fscanf(file, "%d|%99[^|]|%d\n", &user.id, 
    user.name, &user.age) == 3){
        if(user.id == id){
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

void CreateUser(){
    FILE *file = fopen(File_name, "a");
    struct User user;

    if(file == NULL){
        printf("Error: Unable to open File\n");
        return;
    }

    printf("Enter ID:");
    scanf("%d", &user.id);

    if(IdExists(user.id)){
        printf("Error: ID already exists\n");
        fclose(file);
        return;
    }
    printf("Enter Name: ");
    /* %d -> Reads Integer, | -> Literal Pipe Seperator 
    %99[^] -> Reads up to 99 characters of name pipe hit*/
    scanf(" %99[^\n]", user.name);

    printf("Enter Age: ");
    scanf("%d", &user.age);

    fprintf(file, "%d|%s|%d\n", user.id, user.name, user.age);
    fclose(file);

    printf("User added Successfully\n");
}

void ReadUsers(){
    FILE *file = fopen(File_name, "r");
    struct User user;
    int found = 0;

    if(file == NULL){
        printf("Error: Unable to open file\n");
        return;
    }
    printf("\n------------ User Records --------------\n");
    while(fscanf(file, "%d|%99[^|]|%d\n", &user.id, user.name, &user.age) == 3){
        printf("ID: %d | Name: %s | Age: %d\n", user.id, user.name, user.age);
        found = 1;
    }
    if(!found){
        printf("No users Found\n");
    }
    fclose(file);
}

void UpdateUser(){
    FILE *file = fopen(File_name, "r");
    FILE *temp = fopen("temp.txt", "w");

    struct User user;
    int id, found = 0;

    if(file == NULL || temp == NULL){
        printf("Error: Unable to open file\n");

        if(file != NULL) fclose(file);
        if(temp != NULL) fclose(temp);
        return;
    }
    printf("Enter ID to update:");
    scanf("%d", &id);

    while(fscanf(file, "%d|%99[^|]|%d\n", &user.id, user.name, &user.age) == 3){
        if(user.id == id){
            printf("Enter New Name: ");
            scanf(" %99[^\n]", user.name);

            printf("Enter New Age:");
            scanf("%d", &user.age);

            found = 1;
        }
        fprintf(temp, "%d|%s|%d\n", user.id, user.name, user.age);
    }
    fclose(file);
    fclose(temp);

    if(!found){
        printf("Error: User ID not found\n");
        remove("temp.txt");
        return;
    }
    remove(File_name);
    rename("temp.txt", File_name);
    printf("User updated Successfully\n");
}

void DeleteUser(){
    FILE *file = fopen(File_name, "r");
    FILE *temp = fopen("temp.txt", "w");

    struct User user;
    int id;
    int found = 0;

    if(file == NULL || temp == NULL){
        printf("Error: Unable to open File\n");

        if(file != NULL) fclose(file);
        if(temp != NULL) fclose(temp);

        return;
    }

    printf("Enter ID to delete");
    scanf("%d", &id);

    while(fscanf(file, "%d|%99[^|]|%d\n", &user.id, user.name, &user.age) == 3){
        if(user.id == id){
            found = 1;
            continue;
        }
        fprintf(temp, "%d|%s|%d\n", user.id, user.name, user.age);
    }
    fclose(file);
    fclose(temp);

    if(!found){
        printf("Error: User ID not found\n");
        remove("temp.txt");
        return;
    }
    remove(File_name);
    rename("temp.txt", File_name);
    printf("User deleted Successfully\n");
}
int main(){
    int choice;
    CreateFile();
    do{
        printf("\n User Management System \n");
        printf("1. Create User\n");
        printf("2. Read Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");

        printf("Enter your choice");
        scanf("%d", &choice);

        switch(choice){
            case 1:
            CreateUser();
            break;
            case 2: 
            ReadUsers();
            break;
            case 3:
            UpdateUser();
            break;
            case 4:
            DeleteUser();
            break;
            case 5:
            printf("Exiting Program\n");
            break;
            default:
            printf("Invalid Choice. Try Again\n");
        }
    }
    while(choice != 5);
    return 0;
}