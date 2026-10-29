#include <stdio.h>

// Constant Declaration
#define MAX_NAME_LENGTH 100
#define MAX_DESIGNATION_LENGTH 100
#define MAX_EMPLOYEES 100
#define MAX_REPORTS 20

// Structure Declaration
typedef struct Employee {
    int id;
    char name[MAX_NAME_LENGTH];
    char designation[MAX_DESIGNATION_LENGTH];
    struct Employee *manager;
    struct Employee *reports[MAX_REPORTS];
    int reportCount;
    int isActive;
} Employee;

Employee employeePool[MAX_EMPLOYEES];

// Function ProtoTypes
void copyString(char *dest, const char *src, int maxLen);
int readLine(char *buffer, int maxLen);
int readInteger(const char *prompt, int *output);
Employee* findEmployee(int id);
int addReport(Employee *manager, Employee *report);
void removeReportFromManager(Employee *manager, Employee *report);
int isSubordinate(Employee *employee, Employee *potentialManager);
void displayHierarchyRecursively(Employee *employee, int level);
int countSubordinates(Employee *employee);
void displayTopLevelRecursively(int index);
void addEmployee();
void removeEmployee();
void displayEmployeeHierarchy();
void displayCompleteHierarchy();
void displayMenu();

// Copies String from Source to Destination
void copyString(char *dest, const char *src, int maxLen) {
    int i = 0;
    while (src[i] != '\0' && i < maxLen - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

// Safely Takes input from Console
int readLine(char *buffer, int maxLen) {
    int count = 0;
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF) {
        if (count < maxLen - 1) {
            buffer[count++] = (char)ch;
        }
    }
    buffer[count] = '\0';

    if (ch == EOF && count == 0) {
        return 0;
    }
    return 1;
}

// Safely Passes integer from User
int readInteger(const char *prompt, int *output) {
    char buffer[128];

    while (1) {
        printf("%s", prompt);

        if (!readLine(buffer, sizeof(buffer))) {
            return 0;
        }

        if (buffer[0] == '\0') {
            printf("Error: Input cannot be empty. Please enter a valid number.\n");
            continue;
        }

        int i = 0;
        int isValid = 1;
        long result = 0;

        while (buffer[i] == ' ' || buffer[i] == '\t') {
            i++;
        }

        if (buffer[i] == '\0') {
            printf("Error: Input contains only whitespace.\n");
            continue;
        }

        for (; buffer[i] != '\0'; i++) {
            if (buffer[i] < '0' || buffer[i] > '9') {
                isValid = 0;
                break;
            }
            result = result * 10 + (buffer[i] - '0');
            if (result > 2147483647) {
                isValid = 0;
                break;
            }
        }

        if (!isValid) {
            printf("Error: Non-numeric or invalid input detected. Please enter digits only.\n");
            continue;
        }

        *output = (int)result;
        return 1;
    }
}

// Validates Name of User
int isValidName(const char *name) {
    if (name[0] == '\0') return 0;
    int hasAlpha = 0;

    for (int i = 0; name[i] != '\0'; i++) {
        char ch = name[i];
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
            hasAlpha = 1;
        } else if (ch != ' ' && ch != '-') {
            return 0;
        }
    }
    return hasAlpha;
}

// Finds Employee by ID
Employee* findEmployee(int id) {
    if (id <= 0) return NULL;
    for (int i = 0; i < MAX_EMPLOYEES; i++) {
        if (employeePool[i].isActive && employeePool[i].id == id) {
            return &employeePool[i];
        }
    }
    return NULL;
}

// Adds report pointer to manager's report array
int addReport(Employee *manager, Employee *report) {
    if (manager == NULL || report == NULL) return 0;
    if (manager->reportCount >= MAX_REPORTS) {
        printf("Error: Manager ID %d reached maximum reports capacity (%d).\n", manager->id, MAX_REPORTS);
        return 0;
    }
    manager->reports[manager->reportCount++] = report;
    return 1;
}

// Removes report pointer from manager's report array
void removeReportFromManager(Employee *manager, Employee *report) {
    if (manager == NULL || report == NULL) return;

    for (int i = 0; i < manager->reportCount; i++) {
        if (manager->reports[i] == report) {
            for (int j = i; j < manager->reportCount - 1; j++) {
                manager->reports[j] = manager->reports[j + 1];
            }
            manager->reports[--manager->reportCount] = NULL;
            break;
        }
    }
}

// Check for Cyclic subordinate relationship
int isSubordinate(Employee *employee, Employee *potentialManager) {
    if (employee == NULL || potentialManager == NULL) return 0;

    for (int i = 0; i < employee->reportCount; i++) {
        if (employee->reports[i] == potentialManager) return 1;
        if (isSubordinate(employee->reports[i], potentialManager)) return 1;
    }
    return 0;
}

// Helper function to display Hierarchy Recursively
void displayHierarchyRecursively(Employee *employee, int level) {
    if (employee == NULL || !employee->isActive) return;

    for (int i = 0; i < level; i++) {
        printf("    ");
    }
    printf("%d - %s - %s\n", employee->id, employee->name, employee->designation);

    for (int i = 0; i < employee->reportCount; i++) {
        displayHierarchyRecursively(employee->reports[i], level + 1);
    }
}

// Counts subordinates of Employee
int countSubordinates(Employee *employee) {
    if (employee == NULL || !employee->isActive) return 0;
    int total = employee->reportCount;
    for (int i = 0; i < employee->reportCount; i++) {
        total += countSubordinates(employee->reports[i]);
    }
    return total;
}

// Recursively displays all top-level Employees
void displayTopLevelRecursively(int index) {
    if (index >= MAX_EMPLOYEES) return;

    if (employeePool[index].isActive && employeePool[index].manager == NULL) {
        displayHierarchyRecursively(&employeePool[index], 0);
    }
    displayTopLevelRecursively(index + 1);
}

// Adds New Employee 
void addEmployee() {
    int freeIndex = -1;
    for (int i = 0; i < MAX_EMPLOYEES; i++) {
        if (!employeePool[i].isActive) {
            freeIndex = i;
            break;
        }
    }

    if (freeIndex == -1) {
        printf("Error: Organization reached maximum employee capacity (%d).\n", MAX_EMPLOYEES);
        return;
    }

    int id, managerId;
    char name[MAX_NAME_LENGTH];
    char designation[MAX_DESIGNATION_LENGTH];

    readInteger("Enter Employee ID: ", &id);
    if (id <= 0) {
        printf("Error: Employee ID must be a positive integer.\n");
        return;
    }

    if (findEmployee(id) != NULL) {
        printf("Error: Employee ID %d already exists.\n", id);
        return;
    }

    printf("Enter Name: ");
    if (!readLine(name, sizeof(name))) return;

    if (!isValidName(name)) {
        printf("Error: Invalid Employee Name. Only letters, spaces, and hyphens are allowed.\n");
        return;
    }

    printf("Enter Designation: ");
    if (!readLine(designation, sizeof(designation))) return;

    if (designation[0] == '\0') {
        printf("Error: Designation cannot be empty.\n");
        return;
    }

    printf("(Enter 0 for Top Level Employee / CEO)\n");
    readInteger("Enter Reporting Manager ID: ", &managerId);

    Employee *manager = NULL;
    if (managerId != 0) {
        if (managerId == id) {
            printf("Error: An employee cannot report to themselves.\n");
            return;
        }
        manager = findEmployee(managerId);
        if (manager == NULL) {
            printf("Error: Reporting Manager with ID %d not found.\n", managerId);
            return;
        }
        if (manager->reportCount >= MAX_REPORTS) {
            printf("Error: Target manager has reached max subordinates (%d).\n", MAX_REPORTS);
            return;
        }
    }

    Employee *newEmp = &employeePool[freeIndex];
    newEmp->id = id;
    copyString(newEmp->name, name, MAX_NAME_LENGTH);
    copyString(newEmp->designation, designation, MAX_DESIGNATION_LENGTH);
    newEmp->manager = manager;
    newEmp->reportCount = 0;
    newEmp->isActive = 1;

    for (int i = 0; i < MAX_REPORTS; i++) {
        newEmp->reports[i] = NULL;
    }

    if (manager != NULL) {
        addReport(manager, newEmp);
    }

    printf("Employee added successfully.\n");
}

// Removes Existing Employee
void removeEmployee() {
    int id;
    readInteger("Enter Employee ID to Remove: ", &id);

    Employee *emp = findEmployee(id);
    if (emp == NULL) {
        printf("Error: Employee ID %d not found.\n", id);
        return;
    }

    // Unlink from direct manager
    if (emp->manager != NULL) {
        int projectedReports = (emp->manager->reportCount - 1) + emp->reportCount;
        if (projectedReports > MAX_REPORTS) {
            printf("Error: Cannot remove employee. Manager ID %d exceeds maximum capacity of %d reports.\n",
                   emp->manager->id, MAX_REPORTS);
            return;
        }
    }

    // Reassign subordinates to the deleted employee's manager
    for (int i = 0; i < emp->reportCount; i++) {
        Employee *subordinate = emp->reports[i];
        subordinate->manager = emp->manager;
        if (emp->manager != NULL) {
            addReport(emp->manager, subordinate);
        }
    }

    // Reset slot
    emp->isActive = 0;
    emp->id = 0;
    emp->reportCount = 0;
    emp->manager = NULL;
    for (int i = 0; i < MAX_REPORTS; i++) {
        emp->reports[i] = NULL;
    }

    printf("Employee %d removed successfully.\n", id);
}

// Displays Hierarchy of Employees
void displayEmployeeHierarchy() {
    int id;
    readInteger("Enter Employee ID: ", &id);

    Employee *emp = findEmployee(id);
    if (emp == NULL) {
        printf("Error: Employee ID %d not found.\n", id);
        return;
    }

    printf("\nDisplay Hierarchy of Employee %d\n", id);
    displayHierarchyRecursively(emp, 0);
    printf("Total Employees Under %d: %d\n", id, countSubordinates(emp));
}

// Displays Entire Hierarchy of Employees
void displayCompleteHierarchy() {
    int totalCount = 0;

    for (int i = 0; i < MAX_EMPLOYEES; i++) {
        if (employeePool[i].isActive) {
            totalCount++;
        }
    }

    if (totalCount == 0) {
        printf("No employees in the organization.\n");
        return;
    }

    printf("\n===== ORGANIZATION HIERARCHY =====\n");
    displayTopLevelRecursively(0);
    printf("Total Employees: %d\n", totalCount);
}

void displayMenu() {
    printf("\n===== EMPLOYEE HIERARCHY MANAGEMENT =====\n");
    printf("1. Add Employee\n");
    printf("2. Remove Employee\n");
    printf("3. Display Employee Hierarchy\n");
    printf("4. Display Complete Hierarchy\n");
    printf("5. Exit\n");
}

int main() {
    for (int i = 0; i < MAX_EMPLOYEES; i++) {
        employeePool[i].isActive = 0;
        employeePool[i].reportCount = 0;
        employeePool[i].manager = NULL;
    }

    int choice = 0;
    while (1) {
        displayMenu();
        readInteger("Enter your choice: ", &choice);

        switch (choice) {
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
                printf("Exiting Program.\n");
                return 0;
            default:
                printf("Invalid choice. Please select an option between 1 and 5.\n");
        }
    }
    return 0;
}