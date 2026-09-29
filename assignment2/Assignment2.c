#include <stdio.h>
#include <string.h>

#define FILE_NAME "users.txt"
#define MAX_USERS 1000
#define NAME_SIZE 50

struct User
{
    int id;
    char name[NAME_SIZE];
    int age;
};

struct User users[MAX_USERS];
int userCount = 0;

void createFile()
{
    FILE *file;

    file = fopen(FILE_NAME, "a");

    if (file != NULL)
        fclose(file);
}

void loadUsers()
{
    FILE *file;

    file = fopen(FILE_NAME, "r");

    if (file == NULL)
        return;

    userCount = 0;

    while (userCount < MAX_USERS)
    {
        if (fscanf(file, "%d %49s %d",
                   &users[userCount].id,
                   users[userCount].name,
                   &users[userCount].age) != 3)
        {
            break;
        }

        userCount++;
    }

    fclose(file);
}

void saveUsers()
{
    FILE *file;
    int i;

    file = fopen(FILE_NAME, "w");

    if (file == NULL)
    {
        printf("Could not open file.\n");
        return;
    }

    for (i = 0; i < userCount; i++)
    {
        fprintf(file, "%d %s %d\n",
                users[i].id,
                users[i].name,
                users[i].age);
    }

    fclose(file);
}

int findUser(int id)
{
    int i;

    for (i = 0; i < userCount; i++)
    {
        if (users[i].id == id)
            return i;
    }

    return -1;
}

int getNumber(char message[])
{
    int number;
    int result;
    int ch;

    printf("%s", message);

    result = scanf("%d", &number);

    while (result != 1)
    {
        while ((ch = getchar()) != '\n' && ch != EOF)
        {
        }

        printf("Invalid input. Try again.\n");
        printf("%s", message);

        result = scanf("%d", &number);
    }

    return number;
}

void getName(char name[])
{
    printf("Enter name: ");
    scanf("%49s", name);
}

void createUser()
{
    struct User newUser;

    if (userCount >= MAX_USERS)
    {
        printf("Storage is full.\n");
        return;
    }

    newUser.id = getNumber("Enter ID: ");

    if (findUser(newUser.id) != -1)
    {
        printf("ID already exists.\n");
        return;
    }

    getName(newUser.name);

    newUser.age = getNumber("Enter age: ");

    users[userCount] = newUser;
    userCount++;

    saveUsers();

    printf("User created successfully.\n");
}

void showUsers()
{
    int i;

    if (userCount == 0)
    {
        printf("No users found.\n");
        return;
    }

    printf("\nID\tName\t\tAge\n");

    for (i = 0; i < userCount; i++)
    {
        printf("%d\t%-15s%d\n",
               users[i].id,
               users[i].name,
               users[i].age);
    }
}

void updateUser()
{
    int id;
    int index;

    id = getNumber("Enter ID to update: ");

    index = findUser(id);

    if (index == -1)
    {
        printf("User not found.\n");
        return;
    }

    getName(users[index].name);
    users[index].age = getNumber("Enter new age: ");

    saveUsers();

    printf("User updated successfully.\n");
}

void deleteUser()
{
    int id;
    int index;
    int i;

    id = getNumber("Enter ID to delete: ");

    index = findUser(id);

    if (index == -1)
    {
        printf("User not found.\n");
        return;
    }

    for (i = index; i < userCount - 1; i++)
    {
        users[i] = users[i + 1];
    }

    userCount--;

    saveUsers();

    printf("User deleted successfully.\n");
}

int main()
{
    int choice;

    createFile();
    loadUsers();

    do
    {
        printf("\n");
        printf("1. Create User\n");
        printf("2. Show Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");

        choice = getNumber("Enter your choice: ");

        if (choice == 1)
        {
            createUser();
        }
        else if (choice == 2)
        {
            showUsers();
        }
        else if (choice == 3)
        {
            updateUser();
        }
        else if (choice == 4)
        {
            deleteUser();
        }
        else if (choice == 5)
        {
            printf("Goodbye!\n");
        }
        else
        {
            printf("Invalid choice.\n");
        }

    } while (choice != 5);

    return 0;
}