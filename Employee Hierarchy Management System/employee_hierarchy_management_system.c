
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHAR 100

// Considered the organization as one root: CEO

struct Employee {
    int emp_id;
    char name[MAX_CHAR + 1];
    char designation[MAX_CHAR + 1];
    struct Employee *reportingManager;
    struct Employee **reports;
    int countReports;
    int capacity;
};

struct Employee *ceo = NULL;

// clear remaining input from the current line
void clearInputBuffer() {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

// function to increase the capacity reports the manager has
int increaseCapacity(struct Employee ***arr, int *capacity, int required) {
    if (required <= *capacity) {
        return 1;
    }

    int newcapacity;

    if (*capacity == 0) {
        newcapacity = 2;
    } else {
        newcapacity = *capacity;
    }
    while (newcapacity < required) {
        newcapacity = newcapacity * 2;
    }
    struct Employee **temp = realloc(
        *arr, newcapacity * sizeof(struct Employee *)
    );
    if (temp == NULL) {
        return 0;
    }
    *arr = temp;
    *capacity = newcapacity;
    return 1;
}

// find employee using recursion
struct Employee *findEmployee(struct Employee *emp, int emp_id) {
    if (emp == NULL) {
        return NULL;
    }
    if (emp->emp_id == emp_id) {
        return emp;
    }
    for (int i = 0; i < emp->countReports; i++) {
        struct Employee *foundemp = findEmployee(emp->reports[i], emp_id);

        if (foundemp != NULL) {
            return foundemp;
        }
    }
    return NULL;
}

// searching from ceo(root)
struct Employee *searchEmployee(int emp_id) {
    return findEmployee(ceo, emp_id);
}

// adding employee
void addEmployee() {
    struct Employee *emp;
    struct Employee *manager = NULL;

    emp = malloc(sizeof(struct Employee));

    if (emp == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    emp->reports = NULL;
    emp->countReports = 0;
    emp->capacity = 0;
    emp->reportingManager = NULL;

    printf("Enter Employee ID: ");
    int inputStatus;
    inputStatus = scanf("%d", &emp->emp_id);
    clearInputBuffer();
    if (inputStatus != 1) {
        printf("Invalid input.\n");
        free(emp);
        return;
    }
    if (emp->emp_id <= 0 || searchEmployee(emp->emp_id) != NULL) {
        printf("Invalid or not unique Employee ID.\n");
        free(emp);
        return;
    }

    // getting name
    while (1) {
        printf("Enter Name (maximum 100 characters): ");
        inputStatus = scanf(" %100[^\n]", emp->name);
        if (inputStatus != 1) {
            clearInputBuffer();
            printf("Invalid name. Try again.\n");
            continue;
        }
        int ch = getchar();
        if (ch == '\n' || ch == EOF) {
            break;
        }
        clearInputBuffer();
        printf("Name must not exceed 100 characters.\n");
    }

    // getting designation
    while (1) {
        printf("Enter Designation (maximum 100 characters): ");
        inputStatus = scanf(" %100[^\n]", emp->designation);
        if (inputStatus != 1) {
            clearInputBuffer();
            printf("Invalid designation. Try again.\n");
            continue;
        }
        int ch = getchar();
        if (ch == '\n' || ch == EOF) {
            break;
        }
        clearInputBuffer();
        printf("Designation must not exceed 100 characters.\n");
    }

    printf("Enter Manager ID (-1 for CEO): ");
    int managerid;
    inputStatus = scanf("%d", &managerid);

    if (inputStatus != 1) {
        printf("Invalid Input\n");
        clearInputBuffer();
        free(emp);
        return;
    }

    clearInputBuffer();

    if (managerid == -1) {
        // ie. ceo
        if (strcmp(emp->designation, "CEO") != 0) {
            printf("Only the CEO can be a top-level employee.\n");
            printf("Employee not added.\n");
            free(emp);
            return;
        }

        if (ceo != NULL) {
            printf("CEO already exists.\n");
            free(emp);
            return;
        }

        emp->reportingManager = NULL;
        ceo = emp;
    } else {
        manager = searchEmployee(managerid);
        if (manager == NULL) {
            printf("Manager not found. Add CEO first.\n");
            free(emp);
            return;
        }
        if (strcmp(emp->designation, "CEO") == 0) {
            printf("The CEO must be the root employee.\n");
            free(emp);
            return;
        }

        // allocating space before adding employee
        if (!increaseCapacity(&manager->reports,&manager->capacity,manager->countReports + 1)) {
            printf("Memory allocation failed.\n");
            free(emp);
            return;
        }
        emp->reportingManager = manager;
        manager->reports[manager->countReports] = emp;
        manager->countReports++;
    }

    printf("Employee added.\n");
}

// counting number of employees under a manager
int countEmployees(struct Employee *emp) {
    if (emp == NULL) {
        return 0;
    }
    int countemp = 0;
    for (int i = 0; i < emp->countReports; i++) {
        countemp += 1 + countEmployees(emp->reports[i]);
    }
    return countemp;
}

// displaying hierarchy
void displayHierarchy(struct Employee *emp, int level) {
    if (emp == NULL) {
        return;
    }
    for (int i = 0; i < level; i++) {
        printf("    ");
    }
    printf("%d - %s - %s\n",
           emp->emp_id, emp->name, emp->designation);
    for (int i = 0; i < emp->countReports; i++) {
        displayHierarchy(emp->reports[i], level + 1);
    }
}

// display hierarchy of a selected employee
void displayEmployeeHierarchy() {
    int emp_id;
    printf("Enter Employee ID: ");
    if (scanf("%d", &emp_id) != 1) {
        printf("Invalid input.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    struct Employee *emp = searchEmployee(emp_id);
    if (emp == NULL) {
        printf("Employee not found.\n");
        return;
    }

    displayHierarchy(emp, 0);

    printf("Total Employees Under %d: %d\n",emp_id, countEmployees(emp));
}

// display complete hierarchy of the organization
void displayCompleteHierarchy() {
    if (ceo == NULL) {
        printf("Organization is empty. Add the CEO first.\n");
        return;
    }
    printf("\n===== ORGANIZATION HIERARCHY =====\n");
    displayHierarchy(ceo, 0);
    printf("Total Employees: %d\n", 1 + countEmployees(ceo));
}

// remove employee from manager's reports
void removeFromManager(struct Employee **arr, int *count,
                       struct Employee *emp) {
    for (int i = 0; i < *count; i++) {
        if (arr[i] == emp) {
            for (int j = i; j < *count - 1; j++) {
                arr[j] = arr[j + 1];
            }
            (*count)--;
            return;
        }
    }
}

// remove employee
void removeEmployee() {
    int emp_id;
    printf("Enter Employee ID to remove: ");
    if (scanf("%d", &emp_id) != 1) {
        printf("Invalid input.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    struct Employee *emp = searchEmployee(emp_id);
    if (emp == NULL) {
        printf("Employee not found.\n");
        return;
    }
    if (emp == ceo) {
        printf("The CEO cannot be removed.\n");
        return;
    }
    struct Employee *parent = emp->reportingManager;
    struct Employee *destination = NULL;

    // finding siblings with fewest reports, if none then transferring it to employee's reporting manager
    for (int i = 0; i < parent->countReports; i++) {
        struct Employee *sibling = parent->reports[i];
        if (sibling == emp) {
            continue;
        }
        if (destination == NULL ||
            sibling->countReports < destination->countReports) {
            destination = sibling;
        }
    }

    if (destination == NULL && emp->countReports > 0) {
        destination = parent;
    }

    // reserving space for allocation
    if (destination != NULL && emp->countReports > 0) {
        if (!increaseCapacity(&destination->reports,
                              &destination->capacity,
                              destination->countReports +
                              emp->countReports)) {
            printf("Memory allocation failed. Removal cancelled.\n");
            return;
        }
    }

    // transferring reports and updating manager
    for (int i = 0; i < emp->countReports; i++) {
        struct Employee *report = emp->reports[i];
        destination->reports[destination->countReports] = report;
        destination->countReports++;
        report->reportingManager = destination;
    }

    // removing employee from manager's reports
    removeFromManager(parent->reports, &parent->countReports, emp);

    // free the employee's memory
    free(emp->reports);
    free(emp);

    printf("Employee removed successfully.\n");
}

// free the entire organization hierarchy memory
void freeHierarchy(struct Employee *emp) {
    if (emp == NULL) {
        return;
    }
    for (int i = 0; i < emp->countReports; i++) {
        freeHierarchy(emp->reports[i]);
    }
    free(emp->reports);
    free(emp);
}

int main() {
    int choice;

    while (1) {
        printf("==== EMPLOYEE HIERARCHY MANAGEMENT ====\n");
        printf("1. Add Employee\n");
        printf("2. Remove Employee\n");
        printf("3. Display Employee Hierarchy\n");
        printf("4. Display Complete Hierarchy\n");
        printf("5. Exit\n");

        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Enter a valid choice.\n");
            clearInputBuffer();
            continue;
        }

        clearInputBuffer();

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
                freeHierarchy(ceo);
                ceo = NULL;

                printf("Exiting the program...\n");
                return 0;

            default:
                printf("Enter valid choice\n");
        }
    }

    return 0;
}
