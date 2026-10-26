#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 100
#define MAX_DESIGNATION_LENGTH MAX_NAME_LENGTH
// room for the longest text, the newline and the terminating '\0'
#define LINE_BUFFER_SIZE (MAX_NAME_LENGTH + 2)
#define INITIAL_REPORT_CAPACITY 2
#define CAPACITY_GROWTH_FACTOR 2
#define CEO_MANAGER_ID -1

// Considered the organization as one root: CEO

struct Employee {
    int emp_id;
    char name[MAX_NAME_LENGTH + 1];
    char designation[MAX_DESIGNATION_LENGTH + 1];
    struct Employee *reportingManager;
    struct Employee **reports;
    int countReports;
    int capacity;
};

struct Employee *ceo = NULL;

void clearInputBuffer() {
    int input_character;
    while ((input_character = getchar()) != '\n' && input_character != EOF) {
    }
}

// returns 1 on success, 0 if input ended, keeps asking on empty or too long text
int readText(const char *label, char *buffer, size_t max_length) {
    char line[LINE_BUFFER_SIZE];

    while (1) {
        printf("Enter %s (maximum %zu characters): ", label, max_length);
        if (fgets(line, sizeof(line), stdin) == NULL) {
            return 0;
        }

        size_t length = strcspn(line, "\n");
        if (line[length] == '\0' && length == sizeof(line) - 1) {
            // buffer filled without reaching the end of the line
            clearInputBuffer();
            printf("%s must not exceed %zu characters.\n", label, max_length);
            continue;
        }
        line[length] = '\0';

        if (length == 0) {
            printf("Invalid input. Try again.\n");
            continue;
        }
        strcpy(buffer, line);
        return 1;
    }
}

int increaseCapacity(struct Employee ***arr, int *capacity, int required) {
    if (required <= *capacity) {
        return 1;
    }

    int new_capacity;

    if (*capacity == 0) {
        new_capacity = INITIAL_REPORT_CAPACITY;
    } else {
        new_capacity = *capacity;
    }
    while (new_capacity < required) {
        if (new_capacity > INT_MAX / CAPACITY_GROWTH_FACTOR) {
            return 0;
        }
        new_capacity = new_capacity * CAPACITY_GROWTH_FACTOR;
    }
    if ((size_t)new_capacity > SIZE_MAX / sizeof(struct Employee *)) {
        return 0;
    }
    struct Employee **temp = realloc(*arr, (size_t)new_capacity * sizeof(struct Employee *));
    if (temp == NULL) {
        return 0;
    }
    *arr = temp;
    *capacity = new_capacity;
    return 1;
}

struct Employee *findEmployee(struct Employee *employee, int employee_id) {
    if (employee == NULL) {
        return NULL;
    }
    if (employee->emp_id == employee_id) {
        return employee;
    }
    for (int child_index = 0; child_index < employee->countReports; child_index++) {
        struct Employee *found_employee = findEmployee(employee->reports[child_index], employee_id);

        if (found_employee != NULL) {
            return found_employee;
        }
    }
    return NULL;
}

struct Employee *searchEmployee(int employee_id) {
    return findEmployee(ceo, employee_id);
}

struct Employee *createEmployee(int employee_id, const char *name, const char *designation) {
    struct Employee *employee = malloc(sizeof(struct Employee));
    if (employee == NULL) {
        return NULL;
    }

    employee->emp_id = employee_id;
    strcpy(employee->name, name);
    strcpy(employee->designation, designation);
    employee->reportingManager = NULL;
    employee->reports = NULL;
    employee->countReports = 0;
    employee->capacity = 0;
    return employee;
}

int readEmployeeId(int *employee_id) {
    printf("Enter Employee ID: ");
    if (scanf("%d", employee_id) != 1) {
        printf("Invalid input.\n");
        clearInputBuffer();
        return 0;
    }
    clearInputBuffer();
    if (*employee_id <= 0 || searchEmployee(*employee_id) != NULL) {
        printf("Invalid or not unique Employee ID.\n");
        return 0;
    }
    return 1;
}

int readName(char *name) {
    return readText("Name", name, MAX_NAME_LENGTH);
}

int readDesignation(char *designation) {
    return readText("Designation", designation, MAX_DESIGNATION_LENGTH);
}

// returns 1 if employee is attached, 0 otherwise (caller frees it)
int assignEmployeeToManager(struct Employee *employee) {
    struct Employee *manager = NULL;

    printf("Enter Manager ID (-1 for CEO): ");
    int manager_id;
    if (scanf("%d", &manager_id) != 1) {
        printf("Invalid Input\n");
        clearInputBuffer();
        return 0;
    }

    clearInputBuffer();

    if (manager_id == CEO_MANAGER_ID) {
        // ie. ceo
        if (strcmp(employee->designation, "CEO") != 0) {
            printf("Only the CEO can be a top-level employee.\n");
            printf("Employee not added.\n");
            return 0;
        }

        if (ceo != NULL) {
            printf("CEO already exists.\n");
            return 0;
        }

        employee->reportingManager = NULL;
        ceo = employee;
    }
    else {
        manager = searchEmployee(manager_id);
        if (manager == NULL) {
            printf("Manager not found. Add CEO first.\n");
            return 0;
        }
        if (strcmp(employee->designation, "CEO") == 0) {
            printf("The CEO must be the root employee.\n");
            return 0;
        }

        // allocating space before adding employee
        if (!increaseCapacity(&manager->reports,&manager->capacity,manager->countReports + 1)) {
            printf("Memory allocation failed.\n");
            return 0;
        }
        employee->reportingManager = manager;
        manager->reports[manager->countReports] = employee;
        manager->countReports++;
    }

    return 1;
}

void addEmployee() {
    int employee_id;
    char name[MAX_NAME_LENGTH + 1];
    char designation[MAX_DESIGNATION_LENGTH + 1];

    if (!readEmployeeId(&employee_id)) {
        return;
    }
    // getting name and designation
    if (!readName(name) || !readDesignation(designation)) {
        printf("Input ended. Employee not added.\n");
        return;
    }

    struct Employee *employee = createEmployee(employee_id, name, designation);
    if (employee == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    if (!assignEmployeeToManager(employee)) {
        free(employee);
        return;
    }

    printf("Employee added.\n");
}

int countEmployees(struct Employee *employee) {
    if (employee == NULL) {
        return 0;
    }
    int employee_count = 0;
    for (int child_index = 0; child_index < employee->countReports; child_index++) {
        employee_count += 1 + countEmployees(employee->reports[child_index]);
    }
    return employee_count;
}

void displayHierarchy(struct Employee *employee, int hierarchy_level) {
    if (employee == NULL) {
        return;
    }
    for (int level_index = 0; level_index < hierarchy_level; level_index++) {
        printf("    ");
    }
    printf("%d - %s - %s\n", employee->emp_id, employee->name, employee->designation);
    for (int child_index = 0; child_index < employee->countReports; child_index++) {
        displayHierarchy(employee->reports[child_index], hierarchy_level + 1);
    }
}

void displayEmployeeHierarchy() {
    int employee_id;
    printf("Enter Employee ID: ");
    if (scanf("%d", &employee_id) != 1) {
        printf("Invalid input.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    struct Employee *employee = searchEmployee(employee_id);
    if (employee == NULL) {
        printf("Employee not found.\n");
        return;
    }

    displayHierarchy(employee, 0);

    printf("Total Employees Under %d: %d\n", employee_id,
           countEmployees(employee));
}

void displayCompleteHierarchy() {
    if (ceo == NULL) {
        printf("Organization is empty. Add the CEO first.\n");
        return;
    }
    printf("\n===== ORGANIZATION HIERARCHY =====\n");
    displayHierarchy(ceo, 0);
    printf("Total Employees: %d\n", 1 + countEmployees(ceo));
}

void removeFromManager(struct Employee **arr, int *count, struct Employee *employee) {
    for (int employee_index = 0; employee_index < *count; employee_index++) {
        if (arr[employee_index] == employee) {
            for (int shift_index = employee_index; shift_index < *count - 1; shift_index++) {
                arr[shift_index] = arr[shift_index + 1];
            }
            (*count)--;
            return;
        }
    }
}

void removeEmployee() {
    int employee_id;
    printf("Enter Employee ID to remove: ");
    if (scanf("%d", &employee_id) != 1) {
        printf("Invalid input.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    struct Employee *employee = searchEmployee(employee_id);
    if (employee == NULL) {
        printf("Employee not found.\n");
        return;
    }
    if (employee == ceo) {
        printf("The CEO cannot be removed.\n");
        return;
    }
    struct Employee *parent = employee->reportingManager;
    struct Employee *destination = NULL;

    // finding siblings with fewest reports, if none then transferring it to employee's reporting manager
    for (int sibling_index = 0; sibling_index < parent->countReports; sibling_index++) {
        struct Employee *sibling = parent->reports[sibling_index];
        if (sibling == employee) {
            continue;
        }
        if (destination == NULL || sibling->countReports < destination->countReports) {
            destination = sibling;
        }
    }

    if (destination == NULL && employee->countReports > 0) {
        destination = parent;
    }

    // reserving space for allocation
    if (destination != NULL && employee->countReports > 0) {
        if (!increaseCapacity(&destination->reports,&destination->capacity,destination->countReports + employee->countReports)) {
            printf("Memory allocation failed. Removal cancelled.\n");
            return;
        }
    }

    // transferring reports and updating manager
    for (int report_index = 0; report_index < employee->countReports; report_index++) {
        struct Employee *report = employee->reports[report_index];
        destination->reports[destination->countReports] = report;
        destination->countReports++;
        report->reportingManager = destination;
    }

    // removing employee from manager's reports
    removeFromManager(parent->reports, &parent->countReports, employee);

    // free the employee's memory
    free(employee->reports);
    free(employee);

    printf("Employee removed successfully.\n");
}

void freeHierarchy(struct Employee *employee) {
    if (employee == NULL) {
        return;
    }
    for (int child_index = 0; child_index < employee->countReports; child_index++) {
        freeHierarchy(employee->reports[child_index]);
    }
    free(employee->reports);
    free(employee);
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

        int inputStatus = scanf("%d", &choice);
        if (inputStatus == EOF) {
            freeHierarchy(ceo);
            ceo = NULL;
            printf("\nInput ended. Exiting the program...\n");
            return 0;
        }
        if (inputStatus != 1) {
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