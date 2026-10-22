#include <stdio.h>
// Used for Calloc and Malloc functions
#include <stdlib.h>

// Constant Declaration
#define MAX_NAME_LENGTH 100
#define MAX_DESIGNATION_LENGTH 100

// Structure Declaration

// Forward Declaration of Report Node for usage in Employee Structure
struct ReportNode;

typedef struct Employee{
    int id;
    char name[MAX_NAME_LENGTH];
    char designation[MAX_DESIGNATION_LENGTH];
    struct Employee *manager;
    struct ReportNode *reports;
} Employee;

typedef struct ReportNode{
    Employee *employee;
    struct ReportNode *next;
} ReportNode;

typedef struct EmpNode{
    Employee *employee;
    struct EmpNode *next;
} EmpNode;

EmpNode *allEmployees = NULL;

// Function Prototypes
void displayMenu();
void addEmployee();
Employee *findEmployee(int id);
void traverseReportsList(ReportNode *reportnode, int level);
int countSubordinatesList(ReportNode *reportnode);
void addReport(Employee *manager, Employee *report);
void removeReportFromManager(Employee *manager, Employee *report);
void removeEmployee();
int isSubordinate(Employee *employee, Employee *potentialManager);
void displayCompleteHierarchy();
void displayEmployeeHierarchy();
void displayHierarchyRecursively(Employee *employee, int level);
void displayAllTopLevel(EmpNode *node, int *count);
int isValidName(char *name);
void copyString(char *dest, char *src, int maxLen);

// Copies one string to another
void copyString(char *dest, char *src, int maxLen){
    int i = 0;
    while(src[i] != '\0' && i < maxLen - 1){
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

// Check for Valid Name of Employee
int isValidName(char *name){
    if(name[0] == '\0') return 0;
    int valid = 0;

    for(int i=0 ; name[i] != '\0' ; i++){
        char ch = name[i];

        if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')){
            valid = 1;
        } else if(ch != ' ' && ch != '-'){
            return 0;
        }
    }
    return valid;
}

// Finds Existing Employee based on id
Employee* findEmployee(int id){
    EmpNode *curr = allEmployees;
    while(curr != NULL){
        if(curr->employee->id == id){
            return curr->employee;
        }
        curr = curr->next;
    }
    return NULL;
}

// Adds Reporting Manager to each Employee
void addReport(Employee *manager, Employee *report){
    ReportNode *newNode = (ReportNode*)malloc(sizeof(ReportNode));
    newNode->employee = report;
    newNode->next = NULL;

    if(manager->reports == NULL){
        manager->reports = newNode;
    } else{
        ReportNode *curr = manager->reports;
        while(curr->next != NULL){
            curr = curr->next;
        }
        curr->next = newNode;
    }
}

// Adds Employee
void addEmployee(){
    int id, managerId;
    char name[MAX_NAME_LENGTH];
    char designation[MAX_DESIGNATION_LENGTH];

    printf("Enter Employee ID: ");
    scanf("%d", &id);
    while(getchar() != '\n');
    
        if(findEmployee(id) != NULL){
            printf("Error: Employee Id %d already exists.\n", id);
            return;
        }

        printf("Enter Name: ");
        fgets(name, sizeof(name), stdin);
        for(int i=0 ; name[i] != '\0' ; i++){
            if(name[i] == '\n'){
                name[i] = '\0';
                break;
            }
        }

        if(!isValidName(name)){
            printf("Error: Invalid Employee Name\n");
            return;
        }

        printf("Enter Designation: ");
        fgets(designation, sizeof(designation), stdin);
        for(int i=0 ; designation[i] != '\0' ; i++){
            if(designation[i] == '\n'){
                designation[i] = '\0';
                break;
            }
        }

        printf("Enter Reporting Manager Id: ");
        printf("\n (Enter 0 for Top Level Employee/ CEO)");
        scanf("%d", &managerId);
        while(getchar() != '\n');

        Employee *manager = NULL;
        if(managerId != 0){
            if(managerId == id){
                printf("Error: An Employee cannot report to themselves.\n");
                return;
            }
            manager = findEmployee(managerId);
            if(manager == NULL){
                printf("Error: Reporting Manager with ID %d not found\n", managerId);
                return;
            }
            Employee *existingEmployee = findEmployee(id);
            if(existingEmployee != NULL && isSubordinate(existingEmployee, manager)){
                printf("Error: Circular Reporting relationship Detected\n");
                return;
            }
        }
        Employee *newEmployee = (Employee*)malloc(sizeof(Employee));
        newEmployee->id = id;
        copyString(newEmployee->name, name, MAX_NAME_LENGTH);
        copyString(newEmployee->designation, designation, MAX_DESIGNATION_LENGTH);
        newEmployee->manager = manager;
        newEmployee->reports = NULL;

        if(manager != NULL){
            addReport(manager, newEmployee);
        }

        EmpNode *GlobalNode = (EmpNode*)malloc(sizeof(EmpNode));
        GlobalNode->employee = newEmployee;
        GlobalNode->next = allEmployees;
        allEmployees = GlobalNode;

        printf("Employee added Successfully\n");
    }

// Displays Menu for User Selection
void displayMenu(){
    printf("\n==== EMPLOYEE HIERARCHY MANAGEMENT ====\n");
    printf("1. Add Employee\n");
    printf("2. Remove Employee\n");
    printf("3. Display Employee Hierarhcy\n");
    printf("4. Display Complete Hierarchy\n");
    printf("5. Exit\n");
    printf("Enter Your choice: ");
}

// Removes a specific report entry from a manager's direct reports list and frees its list node
void removeReportFromManager(Employee *manager, Employee *report){
    if(manager == NULL || manager->reports == NULL) return;

    ReportNode *curr = manager->reports;
    ReportNode *prev = NULL;

    while(curr != NULL && curr->employee != report){
        prev = curr;
        curr = curr->next;
    }

    if(curr != NULL){
        if(prev == NULL){
            manager->reports = curr->next;
        } else{
            prev->next = curr->next;
        }
        free(curr);
    }
}

// Removes Employee
void removeEmployee(){
    int id;
    printf("Enter Employee Id to Remove: ");
    scanf("%d", &id);
    while(getchar() != '\n');

    Employee *employee = findEmployee(id);
    if(employee == NULL){
        printf("Error: Employee Id %d not found. \n", id);
        return;
    }

    if(employee->manager != NULL){
        removeReportFromManager(employee->manager, employee);
    }

    ReportNode *repCurr = employee->reports;
    while(repCurr != NULL){
        repCurr->employee->manager = employee->manager;
        if(employee->manager != NULL){
            addReport(employee->manager, repCurr->employee);
        }
        ReportNode *temp = repCurr;
        repCurr = repCurr->next;
        free(temp);
    }
    EmpNode *curr = allEmployees;
    EmpNode *prev = NULL;

    while(curr != NULL && curr->employee != employee){
        prev = curr;
        curr = curr->next;
    }

    if(curr != NULL){
        if(prev == NULL){
            allEmployees = curr->next;
        } else{
            prev->next = curr->next;
        }
        free(curr);
    }
    free(employee);
    printf("Employee %d removed Successfully\n", id);
}

// Checks for Potential Reporting Manager
int isSubordinate(Employee *employee, Employee *potentialManager){
    if(employee == NULL || potentialManager == NULL) return 0;

    ReportNode *curr = employee->reports;
    while(curr != NULL){
        if(curr->employee == potentialManager) return 1;
        if(isSubordinate(curr->employee, potentialManager)) return 1;
        curr = curr->next;
    }
    return 0;
}

// Display Hierarchy Recursively for Employees
void displayHierarchyRecursively(Employee *employee, int level){
    if(employee == NULL) return;
    printf("%d - %s - %s\n", employee->id, employee->name, employee->designation);
    traverseReportsList(employee->reports, level + 1);
}

// Traverses sibling node in reports list to display employee sub-tree
void traverseReportsList(ReportNode *reportNode, int level){
    if(reportNode == NULL) return;
    displayHierarchyRecursively(reportNode->employee, level);
    traverseReportsList(reportNode->next, level);
}

// Subordinates count for a given Employee
int countSubordinatesRecursive(Employee *employee){
    if(employee == NULL || employee->reports == NULL) return 0;
    return countSubordinatesList(employee->reports);
}

// Aggregates subordinates count across depth and breadth
int countSubordinatesList(ReportNode *reportnode){
    if(reportnode == NULL) return 0;
    return 1 + countSubordinatesRecursive(reportnode->employee) + countSubordinatesList(reportnode->next);
}

// Searches for ID, displays reporting sub-tree and outpus total subordinates
void displayEmployeeHierarchy(){
    int id;
    printf("Enter Employee Id: ");
    scanf("%d", &id);
    while(getchar() != '\n');

    Employee *employee = findEmployee(id);
    if(employee == NULL){
        printf("Error: Employee Id %d not found\n", id);
        return;
    }

    displayHierarchyRecursively(employee, 0);
    printf("Total Employees under %d: %d\n", id, countSubordinatesRecursive(employee));
}

// Traverses global list to print all top level Employees
void displayAllTopLevel(EmpNode *node, int *count){
    if(node == NULL) return;

    if(node->employee->manager == NULL){
        displayHierarchyRecursively(node->employee, 0);
    }
    (*count)++;
    displayAllTopLevel(node->next, count);
}

// Displays Entire Organization tree 
void displayCompleteHierarchy(){
    if(allEmployees == NULL){
        printf("No Employees in the organization\n");
        return;
    }

    printf("==== ORGANIZATION HIERARCHY ====\n");
    int total = 0;
    displayAllTopLevel(allEmployees, &total);
    printf("Total Employees: %d\n", total);
}

int main(){
    int choice;
    while(1){
        displayMenu();
        scanf("%d", &choice);
        while(getchar() != '\n');
            switch(choice){
                case 1:
                addEmployee();
                break;
                case 2:
                removeEmployee();
                break;
                case 3:
                displayEmployeeHierarchy();
                break;
                case 4:
                displayCompleteHierarchy();
                break;
                case 5:
                printf("Exiting Program\n");
                return 0;
                default:
                printf("Invalid Choice.\n");
            }
        }
        return 0;
    }