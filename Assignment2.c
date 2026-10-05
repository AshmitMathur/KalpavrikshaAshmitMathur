/* 
    Created By: Ashmit Mathur
    Date: 01/10/26
*/
#include <stdio.h>
#define File_name "users.txt"

struct User{
    int id;
    char name[100];
    int age;
};

// Read Integer
int readInteger(char *prompt){
    int value;
    printf("%s", prompt);
    while(scanf("%d", &value) != 1){
        while(getchar() != '\n');
        printf("Error: Invalid Input. Please enter a Number\n%s", prompt);
    }
    return value;
}

// Check Valid Name
int isValidName(char *name){
    for(int i=0 ; name[i] != '\0'; i++){
        if((name[i] >= 'A' && name[i] <= 'Z') || (name[i] >= 'a' && name[i] <= 'z')){
            return 1;
        }
    }
    return 0;
}

// Menu
void displayMenu(){
    printf("\n User Management System \n");
    printf("1. Create User\n");
    printf("2. Read Users\n");
    printf("3. Update Users\n");
    printf("4. Delete User\n");
    printf("5. Exit\n");
}

// Creates File
void CreateFile(){
    FILE *file = fopen(File_name, "a");
    if(file == NULL){
        printf("Error: Unable to Create File\n");
        return;
    }
    fclose(file);
}

// Checks for Duplicate ID
int IdExists(int id){
    FILE *file = fopen(File_name, "r");
    struct User user;
    if(file == NULL) return 0;

    while(fscanf(file, "%d|%99[^|]|%d\n", &user.id, user.name, &user.age) == 3){
        if(user.id == id){
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

// Creates User
void CreateUser(){
    FILE *file = fopen(File_name, "a");
    struct User user;

    if(file == NULL){
        printf("Error: Unable to open File\n");
        return;
    }

    user.id = readInteger("Enter ID: ");

    if(IdExists(user.id)){
        printf("Error: ID already exists\n");
        fclose(file);
        return;
    }

    // Name Validation
    do{
        printf("Enter Name: ");
        scanf(" %99[^\n]", user.name);

        if(!isValidName(user.name)){
            printf("Error: Name must contain letters\n");
        }
    } while(!isValidName(user.name));
    
    // Age Validation
    do{
        user.age = readInteger("Enter Age: ");
        if(user.age <= 0){
            printf("Error: Age must be a positive number \n");
        }
    } while(user.age <= 0);

    fprintf(file, "%d|%s|%d\n", user.id, user.name, user.age);
    fclose(file);

    printf("User added Succcessfully \n");
}

// Reads User
void ReadUsers(){
    FILE *file = fopen(File_name, "r");
    struct User user;
    int found = 0;

    if(file == NULL){
        printf("Error: Unable to open file\n");
        return;
    }

    printf("\n User Records \n");
    printf("%-10s | %-20s | %-5s\n", "ID", "Name", "Age");

    while(fscanf(file, "%d|%99[^|]|%d\n", &user.id, user.name, &user.age) == 3){
        printf("%-10d | %-20s | %-5d\n", user.id, user.name, user.age);
        found = 1;
    }

    if(!found){
        printf("No users Found\n");
    }
    fclose(file);
}

// Updates User
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

    id = readInteger("Enter ID to update: ");
    while(fscanf(file, "%d|%99[^|]|%d\n", &user.id, user.name, &user.age) == 3){
        if(user.id == id){
            do{
                printf("Enter new Name: ");
                scanf(" %99[^\n]", user.name);
                if(!isValidName(user.name)){
                    printf("Error: Name must contain Letters\n");
                }
            } while(!isValidName(user.name));

            do{
                user.age = readInteger("Enter New Age: ");
                if(user.age <= 0){
                    printf("Error: Age must be a positive number \n");
                }
            } while(user.age <= 0);
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
    printf("User Updated Successfully\n");
}

// Deletes User
void DeleteUser(){
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

    id = readInteger("Enter ID to Delete: ");
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
    printf("User Deleted Successfully\n");
}

int main(){
    int choice;
    CreateFile();

    do{
        displayMenu();
        choice = readInteger("Enter Your Choice: ");

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
    } while(choice != 5);
    return 0;
}