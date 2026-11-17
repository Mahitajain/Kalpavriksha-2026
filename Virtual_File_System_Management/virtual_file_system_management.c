#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 512
#define TOTAL_BLOCK_COUNT 1024
#define MAX_NAME_LENGTH 50
#define MAX_WRITE_BLOCK_COUNT 8
#define MAX_WRITE_CONTENT_LENGTH (BLOCK_SIZE * MAX_WRITE_BLOCK_COUNT)
#define COMMAND_LINE_BUFFER_OVERHEAD 256
#define MAX_COMMAND_LINE_LENGTH (MAX_WRITE_CONTENT_LENGTH + COMMAND_LINE_BUFFER_OVERHEAD)
#define PATH_BUFFER_SIZE 4096

typedef struct FreeBlock {
    int blockIndex;
    struct FreeBlock *next;
    struct FreeBlock *previous;
} FreeBlock;

typedef struct FileNode {
    char name[MAX_NAME_LENGTH + 1];
    int isDirectory;
    struct FileNode *parent;
    struct FileNode *nextSibling;
    struct FileNode *previousSibling;
    struct FileNode *child;
    int fileSize;
    int *blockPointers;
    int blockCount;
} FileNode;


static unsigned char virtualDisk[TOTAL_BLOCK_COUNT][BLOCK_SIZE];
static FreeBlock *freeBlockHead = NULL;
static FreeBlock *freeBlockTail = NULL;
static int freeBlockCount = 0;
static FileNode *root = NULL;
static FileNode *currentDirectory = NULL;


static void appendFreeBlock(int blockIndex) {
    FreeBlock *freeBlock = malloc(sizeof(FreeBlock));
    if (!freeBlock) {
        perror("malloc");
        exit(1);
    }

    freeBlock->blockIndex = blockIndex;
    freeBlock->next = NULL;
    freeBlock->previous = freeBlockTail;
    if (freeBlockTail) {
        freeBlockTail->next = freeBlock;
    } else {
        freeBlockHead = freeBlock;
    }
    freeBlockTail = freeBlock;
    freeBlockCount++;
}

static int takeFreeBlock(void) {
    FreeBlock *freeBlock = freeBlockHead;
    int blockIndex = freeBlock->blockIndex;
    freeBlockHead = freeBlock->next;
    if (freeBlockHead) {
        freeBlockHead->previous = NULL;
    } else {
        freeBlockTail = NULL;
    }
    free(freeBlock);
    freeBlockCount--;
    return blockIndex;
}


static FileNode *createNode(const char *name, int isDirectory, FileNode *parent) {
    FileNode *node = calloc(1, sizeof(FileNode));
    if (!node) {
        perror("calloc");
        exit(1);
    }
    strncpy(node->name, name, MAX_NAME_LENGTH);
    node->isDirectory = isDirectory;
    node->parent = parent;
    return node;
}

static FileNode *findChildByName(FileNode *directory, const char *name) {
    FileNode *child = directory->child;
    if (!child) {
        return NULL;
    }
    do {
        if (strcmp(child->name, name) == 0) {
            return child;
        }
        child = child->nextSibling;
    } while (child != directory->child);
    return NULL;
}

static void addChild(FileNode *directory, FileNode *node) {
    if (!directory->child) {
        node->nextSibling = node;
        node->previousSibling = node;
        directory->child = node;
    } else {
        FileNode *head = directory->child;
        FileNode *tail = head->previousSibling;
        node->nextSibling = head;
        node->previousSibling = tail;
        tail->nextSibling = node;
        head->previousSibling = node;
    }
}

static void removeChild(FileNode *directory, FileNode *node) {
    if (node->nextSibling == node) {
        directory->child = NULL;
    } else {
        node->previousSibling->nextSibling = node->nextSibling;
        node->nextSibling->previousSibling = node->previousSibling;
        if (directory->child == node) {
            directory->child = node->nextSibling;
        }
    }
    node->nextSibling = NULL;
    node->previousSibling = NULL;
}

static void releaseFileBlocks(FileNode *file) {
    for (int index = 0; index < file->blockCount; index++) {
        memset(virtualDisk[file->blockPointers[index]], 0, BLOCK_SIZE);
        appendFreeBlock(file->blockPointers[index]);
    }
    free(file->blockPointers);
    file->blockPointers = NULL;
    file->blockCount = 0;
    file->fileSize = 0;
}

static void destroyFileTree(FileNode *node) {
    if (!node) {
        return;
    }
    if (node->isDirectory) {
        while (node->child) {
            FileNode *child = node->child;
            removeChild(node, child);
            destroyFileTree(child);
        }
    }
    else {
        free(node->blockPointers);
    }
    free(node);
}


static void buildPath(FileNode *node, char *path) {
    if (node == root) {
        strcpy(path, "/");
        return;
    }
    buildPath(node->parent, path);
    if (strcmp(path, "/") != 0) {
        strcat(path, "/");
    }
    strcat(path, node->name);
}

static int isValidName(const char *name) {
    size_t nameLength = strlen(name);
    if (nameLength == 0 || nameLength > MAX_NAME_LENGTH) {
        printf("Error: name must be 1-%d characters.\n", MAX_NAME_LENGTH);
        return 0;
    }
    if (strchr(name, '/')) {
        printf("Error: name cannot contain '/'.\n");
        return 0;
    }
    return 1;
}


static void createDirectory(const char *name) {
    if (!isValidName(name)) {
        return;
    }
    if (findChildByName(currentDirectory, name)) {
        printf("Error: '%s' already exists.\n", name);
        return;
    }
    addChild(currentDirectory, createNode(name, 1, currentDirectory));
    printf("Directory '%s' created successfully.\n", name);
}

static void createFile(const char *name) {
    if (!isValidName(name)) {
        return;
    }
    if (findChildByName(currentDirectory, name)) {
        printf("Error: '%s' already exists.\n", name);
        return;
    }
    addChild(currentDirectory, createNode(name, 0, currentDirectory));
    printf("File '%s' created successfully.\n", name);
}

static void writeFileContents(const char *name, const char *contents) {
    FileNode *file = findChildByName(currentDirectory, name);
    if (!file) {
        printf("Error: file '%s' not found.\n", name);
        return;
    }
    if (file->isDirectory) {
        printf("Error: '%s' is a directory.\n", name);
        return;
    }

    int contentsLength = (int)strlen(contents);
    int requiredBlockCount = (contentsLength + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (requiredBlockCount == 0) {
        requiredBlockCount = 1;
    }

    if (requiredBlockCount > freeBlockCount + file->blockCount) {
        printf("Error: disk full. Need %d block(s), only %d free.\n",
               requiredBlockCount, freeBlockCount + file->blockCount);
        return;
    }
    releaseFileBlocks(file);

    file->blockPointers = malloc(sizeof(int) * requiredBlockCount);
    if (!file->blockPointers) {
        perror("malloc");
        exit(1);
    }
    for (int index = 0; index < requiredBlockCount; index++) {
        int blockIndex = takeFreeBlock();
        file->blockPointers[index] = blockIndex;
        int chunkLength = contentsLength - index * BLOCK_SIZE;
        if (chunkLength > BLOCK_SIZE) {
            chunkLength = BLOCK_SIZE;
        }
        if (chunkLength > 0) {
            memcpy(virtualDisk[blockIndex], contents + index * BLOCK_SIZE, chunkLength);
        }
    }
    file->blockCount = requiredBlockCount;
    file->fileSize = contentsLength;
    printf("Data written successfully (size=%d bytes).\n", contentsLength);
}

static void readFileContents(const char *name) {
    FileNode *file = findChildByName(currentDirectory, name);
    if (!file) {
        printf("Error: file '%s' not found.\n", name);
        return;
    }
    if (file->isDirectory) {
        printf("Error: '%s' is a directory.\n", name);
        return;
    }
    if (file->fileSize == 0) {
        printf("(empty)\n");
        return;
    }
    int remainingBytes = file->fileSize;
    for (int index = 0; index < file->blockCount && remainingBytes > 0; index++) {
        int chunkLength = remainingBytes > BLOCK_SIZE ? BLOCK_SIZE : remainingBytes;
        fwrite(virtualDisk[file->blockPointers[index]], 1, chunkLength, stdout);
        remainingBytes -= chunkLength;
    }
    printf("\n");
}

static void deleteFile(const char *name) {
    FileNode *file = findChildByName(currentDirectory, name);
    if (!file) {
        printf("Error: file '%s' not found.\n", name);
        return;
    }
    if (file->isDirectory) {
        printf("Error: '%s' is a directory. Use rmdir.\n", name);
        return;
    }
    releaseFileBlocks(file);
    removeChild(currentDirectory, file);
    free(file);
    printf("File deleted successfully.\n");
}

static void removeDirectory(const char *name) {
    FileNode *directory = findChildByName(currentDirectory, name);
    if (!directory) {
        printf("Error: directory '%s' not found.\n", name);
        return;
    }
    if (!directory->isDirectory) {
        printf("Error: '%s' is a file. Use delete.\n", name);
        return;
    }
    if (directory->child) {
        printf("Error: directory '%s' is not empty.\n", name);
        return;
    }
    removeChild(currentDirectory, directory);
    free(directory);
    printf("Directory removed successfully.\n");
}

static void listDirectoryContents(void) {
    FileNode *child = currentDirectory->child;
    if (!child) {
        printf("(empty)\n");
        return;
    }
    do {
        printf("%s%s\n", child->name, child->isDirectory ? "/" : "");
        child = child->nextSibling;
    } while (child != currentDirectory->child);
}

static void changeDirectory(const char *name) {
    if (strcmp(name, "..") == 0) {
        if (currentDirectory != root) {
            currentDirectory = currentDirectory->parent;
        }
    } 
    else if (strcmp(name, "/") == 0) {
        currentDirectory = root;
    } 
    else {
        FileNode *directory = findChildByName(currentDirectory, name);
        if (!directory) {
            printf("Error: directory '%s' not found.\n", name);
            return;
        }
        if (!directory->isDirectory) {
            printf("Error: '%s' is not a directory.\n", name);
            return;
        }
        currentDirectory = directory;
    }
    char path[PATH_BUFFER_SIZE];
    buildPath(currentDirectory, path);
    printf("Moved to %s\n", path);
}

static void printWorkingDirectory(void) {
    char path[PATH_BUFFER_SIZE];
    buildPath(currentDirectory, path);
    printf("%s\n", path);
}

static void printDiskUsage(void) {
    int usedBlockCount = TOTAL_BLOCK_COUNT - freeBlockCount;
    printf("Total Blocks: %d\n", TOTAL_BLOCK_COUNT);
    printf("Used Blocks: %d\n", usedBlockCount);
    printf("Free Blocks: %d\n", freeBlockCount);
    printf("Disk Usage: %.2f%%\n", 100.0 * usedBlockCount / TOTAL_BLOCK_COUNT);
}

static char *parseWriteData(char *input) {
    while (*input == ' ' || *input == '\t') {
        input++;
    }
    size_t inputLength = strlen(input);
    if (inputLength >= 2 && input[0] == '"' && input[inputLength - 1] == '"') {
        input[inputLength - 1] = '\0';
        return input + 1;
    }
    return input;
}

void executeCommand(char *commandLine) {
    char *command = strtok(commandLine, " \t");

    if (command == NULL) {
        return;
    }

    if (strcmp(command, "exit") == 0) {
        return;
    } 
    else if (strcmp(command, "ls") == 0) {
        listDirectoryContents();
    } 
    else if (strcmp(command, "pwd") == 0) {
        printWorkingDirectory();
    } 
    
    else if (strcmp(command, "df") == 0) {
        printDiskUsage();
    } 
    else if (strcmp(command, "write") == 0) {
        char *fileName = strtok(NULL, " \t");

        if (fileName == NULL) {
            printf("Usage: write <file> \"content\"\n");
            return;
        }

        char *remainingArguments = strtok(NULL, "");
        writeFileContents(fileName,remainingArguments ? parseWriteData(remainingArguments) : "");
        return;
    }

    char *argument = strtok(NULL, " \t");

    if (argument == NULL) {
        printf("Usage: %s <name>\n", command);
        return;
    }

    if (strcmp(command, "mkdir") == 0) {
        createDirectory(argument);
    } 
    else if (strcmp(command, "create") == 0) {
        createFile(argument);
    } 
    else if (strcmp(command, "read") == 0) {
        readFileContents(argument);
    } 
    else if (strcmp(command, "delete") == 0) {
        deleteFile(argument);
    } 
    else if (strcmp(command, "rmdir") == 0) {
        removeDirectory(argument);
    } 
    else if (strcmp(command, "cd") == 0) {
        changeDirectory(argument);
    } 
    else {
        printf("Unknown command: %s\n", command);
    }
}

int main() {
    for (int index = 0; index < TOTAL_BLOCK_COUNT; index++) {
        appendFreeBlock(index);
    }

    root = createNode("/", 1, NULL);
    currentDirectory = root;

    printf("Compact VFS - ready. Type 'exit' to quit.\n");

    char commandLine[MAX_COMMAND_LINE_LENGTH];

    while (1) {
        char path[PATH_BUFFER_SIZE];

        buildPath(currentDirectory, path);
        printf("%s > ", path);
        fflush(stdout);

        if (fgets(commandLine, sizeof(commandLine), stdin) == NULL) {
            break;
        }

        commandLine[strcspn(commandLine, "\r\n")] = '\0';

        executeCommand(commandLine);

        if (strcmp(commandLine, "exit") == 0) {
            break;
        }
    }

    destroyFileTree(root);

    while (freeBlockHead != NULL) {
        takeFreeBlock();
    }

    printf("Memory released. Exiting program...\n");

    return 0;
}