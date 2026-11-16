#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constants 
#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define MAX_NAME 51
#define MAX_CMD_LINE 2048

// Doubly Linked List for unallocated disk blocks
typedef struct FreeBlock{
    int index;
    struct FreeBlock *prev, *next;
} FreeBlock;

// Circular Linked List node for both files and directories
typedef struct FileNode{
    char name[MAX_NAME];
    int isDirectory;
    int size;
    int blockCount;
    int blockPointers[NUM_BLOCKS];
    struct FileNode *parent, *childhead, *next;
} FileNode;

// Global System State
char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];
FreeBlock *freeHead = NULL;
FreeBlock *freeTail = NULL;
int freeBlockCount = 0;

FileNode *root = NULL;
FileNode *cwd = NULL;

// Function ProtoTypes
int isSpace(char ch);
void appendFreeBlock(int index);
int allocateBlock();
FileNode *createNode(char *name, int isDirectory, FileNode *parent);
void insertChild(FileNode *parent, FileNode *newNode);
FileNode *findChild(FileNode *dir, char *name);
void removeChildFromList(FileNode *parent, FileNode *target);
void freeFileBlocks(FileNode *file);
void initDisk();
void initFileSystem();
void cmd_mkdir(char *name);
void cmd_create(char *name);
void cmd_write(char *name, char *content);
void cmd_read(char *name);
void cmd_delete(char *name);
void cmd_rmdir(char *name);
void cmd_ls();
void getFullPath(FileNode *dir, char *buffer, int buffSize);
void cmd_cd(char *path);
void cmd_pwd();
void cmd_df();
void freeTree(FileNode *node);
void freeBlockList();
void cleanUpandExit();
char *nextToken(char **cursor);

// Helper to check WhiteSpaces 
int isSpace(char ch){
    return (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || 
    ch == '\v' || ch == '\f');
}

// Appends Free block to the tail
void appendFreeBlock(int index){
    FreeBlock *newNode = (FreeBlock*)malloc(sizeof(FreeBlock));
    if(newNode == NULL){
        printf("Memory Allocation failed\n");
        exit(EXIT_FAILURE);
    }
    newNode->index = index;
    newNode->next = NULL;
    newNode->prev = freeTail;

    if(freeTail != NULL){
        freeTail->next = newNode;
    } else{
        freeHead = newNode;
    }
    freeTail = newNode;
    freeBlockCount++;
}

// Allocates Block and remove from free block and return block index from Head of doubly LL
int allocateBlock(){
    if(freeHead == NULL){
        return -1;
    }
    FreeBlock *temp = freeHead;
    int blockIndex = temp->index;

    freeHead = freeHead->next;
    if(freeHead != NULL){
        freeHead->prev = NULL;
    } else{
        freeTail = NULL;
    }
    free(temp);
    freeBlockCount--;
    return blockIndex;
}

// Creates a new File Nade (Either Directory or File)
FileNode *createNode(char *name, int isDirectory, FileNode *parent){
    FileNode *node = (FileNode*)malloc(sizeof(FileNode));

    if(node == NULL){
        printf("Memory Allocation failed. \n");
        exit(EXIT_FAILURE);
    }
    strncpy(node->name, name, MAX_NAME - 1);
    node->name[MAX_NAME - 1] = '\0';
    node->isDirectory = isDirectory;
    node->size = 0;
    node->blockCount = 0;
    node->childhead = NULL;
    node->parent = parent;
    node->next = NULL;
    return node;
}

// Inserts new node at end of parent circular LL
void insertChild(FileNode *parent, FileNode *newNode){
    if(parent->childhead == NULL){
        parent->childhead = newNode;
        newNode->next = newNode;
    } else{
        FileNode *curr = parent->childhead;
        while(curr->next != parent->childhead){
            curr = curr->next;
        }
        curr->next = newNode;
        newNode->next = parent->childhead;
    }
}

// Searches for a child by name in directory's circular LL
FileNode *findChild(FileNode *dir, char *name){
    if(dir->childhead == NULL) return NULL;
    FileNode *curr = dir->childhead;

    do{
        if(strcmp(curr->name, name) == 0){
            return curr;
        }
        curr = curr->next;
    } while(curr != dir->childhead);

    return NULL;
}

// Removes a specific node from its parent circular LL
void removeChildFromList(FileNode *parent, FileNode *target){
    if(parent->childhead == NULL || target == NULL) return;

    if(target->next == target && parent->childhead == target){
        parent->childhead = NULL;
        return;
    }

    FileNode *prev = parent->childhead;
    while(prev->next != target){
        prev = prev->next;
    }
    if(parent->childhead == target){
        parent->childhead = target->next;
    }
    prev->next = target->next;
}

// Helper to free file's allocated disk
void freeFileBlocks(FileNode *file){
    for(int i=0 ; i<file->blockCount ; i++){
        int blockIdx = file->blockPointers[i];
        memset(virtualDisk[blockIdx], 0, BLOCK_SIZE);
        appendFreeBlock(blockIdx);
    }
    file->blockCount = 0;
    file->size = 0;
}

// Initializes virtual disk and populates free block doubly LL
void initDisk(){
    memset(virtualDisk, 0, sizeof(virtualDisk));
    for(int i=0 ; i<NUM_BLOCKS ; i++){
        appendFreeBlock(i);
    }
}

// Initializes root '/' directory
void initFileSystem(){
    initDisk();
    root = createNode("/", 1, NULL);
    cwd = root;
}

// Virtual File System Functions

// Creates directory
void cmd_mkdir(char *name){
    if(name == NULL || strlen(name) == 0){
        printf("Error: Directory name required.\n");
        return;
    }

    if(strlen(name) >= MAX_NAME){
        printf("Error: Name exceeds maximum length of %d characters. \n", MAX_NAME - 1);
        return;
    }

    if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0 || strchr(name, '/') != NULL){
        printf("Error: Invalid directory name. \n");
        return;
    }

    if(findChild(cwd, name) != NULL){
        printf("Error: %s already exists in current Directory. \n", name);
        return;
    }

    FileNode *newDir = createNode(name, 1, cwd);
    insertChild(cwd, newDir);
    printf("Directory %s created Successfully. \n", name);
}

// Creates File
void cmd_create(char *name){
    if(name == NULL || strlen(name) == 0){
    printf("Error: File name required.\n");
    return;
    }

    if(strlen(name) >= MAX_NAME){
    printf("Error: Name exceeds maximum length of %d characters. \n", MAX_NAME - 1);
    return;
    }

    if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0 || strchr(name, '/') != NULL){
    printf("Error: Invalid File name. \n");
    return;
    }

    if(findChild(cwd, name) != NULL){
    printf("Error: %s already exists in current Directory. \n", name);
    return;
    }

    FileNode *newFile = createNode(name, 0, cwd);
    insertChild(cwd, newFile);
    printf("File %s created Successfully. \n", name);
}

// Write in memory block
void cmd_write(char *name, char *content){
    if(name == NULL || content == NULL){
        printf("Error: Usage: write <filename> <content> \n");
        return;
    }

    FileNode *target = findChild(cwd, name);
    if(target == NULL){
        printf("Error: File %s not found", name);
        return;
    }

    if(target->isDirectory == 1){
        printf("Error: %s is not a directory, not a file. \n", name);
        return;
    }
    int dataLen = strlen(content);
    int needed = (dataLen == 0) ? 1 : (dataLen + BLOCK_SIZE - 1) / BLOCK_SIZE;

    if(needed > freeBlockCount + target->blockCount){
        printf("Error: Disk Full. Not enough free blocks available. \n");
        return;
    }

    if(target->blockCount > 0){
        freeFileBlocks(target);
    }
    int bytesWritten = 0;
    for(int i=0 ; i<needed ; i++){
        int blockIdx = allocateBlock();
        target->blockPointers[i] = blockIdx;
        int chunks = dataLen - bytesWritten;
        if(chunks > BLOCK_SIZE) chunks = BLOCK_SIZE;

        if(chunks > 0){
            memcpy(virtualDisk[blockIdx], content + bytesWritten, chunks);
            bytesWritten += chunks;
        }
    }
    target->blockCount = needed;
    target->size = dataLen;
    printf("Data written Successfully (size = %d bytes). \n", dataLen);
}

// Reads from Memory Block
void cmd_read(char *name){
    if(name == NULL){
        printf("Error: File name Required. \n");
        return;
    }

    FileNode *target = findChild(cwd, name);

    if(target == NULL){
        printf("Error: File %s not found", name);
        return;
    }

    if(target->isDirectory == 1){
        printf("Error: %s is not a directory. \n", name);
        return;
    }

    if(target->size == 0){
        printf("\n");
        return;
    }
    int remainingBytes = target->size;
    for(int i=0 ; i<target->blockCount && remainingBytes > 0; i++){
        int blockIdx = target->blockPointers[i];
        int toRead = (remainingBytes > BLOCK_SIZE) ? BLOCK_SIZE : remainingBytes;
        fwrite(virtualDisk[blockIdx], 1, toRead, stdout);
        remainingBytes -= toRead;
    }
    printf("\n");
}

// Deletes  File
void cmd_delete(char *name){
    if(name == NULL){
        printf("Error: FIle name required. \n");
        return;
    }

    FileNode *target = findChild(cwd, name);
    if(target == NULL){
        printf("Error: File %s is not found. \n", name);
        return;
    }

    if(target->isDirectory == 1){
        printf("Error: %s is not a directory. \n", name);
        return;
    }
    freeFileBlocks(target);
    removeChildFromList(cwd, target);
    free(target);
    printf("File deleted Successfully.\n");
}

// Removes Directory
void cmd_rmdir(char *name){
    if(name == NULL){
        printf("Error: Directory name required. \n");
        return;
    }
    FileNode *target = findChild(cwd, name);
    if(target == NULL){
        printf("Error: Directory %s not found. \n", name);
        return;
    }

    if(target->isDirectory == 0){
        printf("Error: %s is a file. \n", name);
        return;
    }

    if(target->childhead != NULL){
        printf("Error: Directory %s is not empty. \n", name);
        return;
    }
    removeChildFromList(cwd, target);
    free(target);
    printf("Directory removed successfully. \n");
}

// Displays all directories
void cmd_ls(){
    if(cwd->childhead == NULL){
        printf("(Empty). \n");
        return;
    }
    FileNode *curr = cwd->childhead;
    do{
        if(curr->isDirectory == 1){
            printf("%s/\n", curr->name);
        } else{
            printf("%s\n", curr->name);
        }
        curr = curr->next;
    } while(curr != cwd->childhead);
}

// Helper to build full path
void getFullPath(FileNode *dir, char *buffer, int buffSize){
    if(dir == root){
        snprintf(buffer, buffSize, "/");
        return;
    }

    FileNode *stack[256];
    int depth = 0;
    FileNode *curr = dir;
    while(curr != NULL && curr != root && depth < 256){
        stack[depth++] = curr;
        curr = curr->parent;
    }
    buffer[0] = '\0';
    for(int i=depth - 1; i>=0 ; i--){
        strncat(buffer, "/", buffSize - strlen(buffer) - 1);
        strncat(buffer, stack[i]->name, buffSize - strlen(buffer) - 1);
    }
}

// Change Directory using this function
void cmd_cd(char *path){
    if(path == NULL || strlen(path) == 0){
        printf("Error: Path Required. \n");
        return;
    }

    if(strcmp(path, "/") == 0){
        cwd = root;
    } else if(strcmp(path, "..") == 0){
        if(cwd->parent != NULL){
            cwd = cwd->parent;
        }
    } else if(strcmp(path, ".") == 0){
        // Condition to stay in the current directory.
    } else{
        FileNode *target = findChild(cwd, path);
        if(target == NULL){
            printf("Error: Directory %s not found. \n", path);
            return;
        }

        if(target->isDirectory == 0){
            printf("Error: %s is not a directory. \n", path);
            return;
        }
        cwd = target;
    }
    char pathBuff[1024];
    getFullPath(cwd, pathBuff, sizeof(pathBuff));
    printf("Moved to %s\n", pathBuff);
}

// Print the absolute path of current working directory
void cmd_pwd(){
    char pathBuff[1024];
    getFullPath(cwd, pathBuff, sizeof(pathBuff));
    printf("%s\n", pathBuff);
}

// Displays disk usage statistics
void cmd_df(){
    int usedBlocks = NUM_BLOCKS - freeBlockCount;
    float usagePercent = ((float)usedBlocks / NUM_BLOCKS) * 100.0;

    printf("Total Blocks: %d\n", NUM_BLOCKS);
    printf("Used Blocks: %d\n", usedBlocks);
    printf("Free Blocks: %d\n", freeBlockCount);
    printf("Disk Usage: %.2f%%\n", usagePercent);
}

// Recursively frees the FileNode Tree
void freeTree(FileNode *node){
    if(node == NULL) return;

    if(node->isDirectory == 1 && node->childhead != NULL){
        FileNode *curr = node->childhead;
        FileNode *start = node->childhead;

        do{
            FileNode *nextNode = curr->next;
            freeTree(curr);
            curr = nextNode;
        } while(curr != start);
    }
    free(node);
}

// Frees all nodes in FreeBlock Doubly LL
void freeBlockList(){
    FreeBlock *curr = freeHead;
    while(curr != NULL){
        FreeBlock *next = curr->next;
        free(curr);
        curr = next;
    }
    freeHead = NULL;
    freeTail = NULL;
}

// Cleans up all the memory and exits the code
void cleanUpandExit(){
    freeTree(root);
    freeBlockList();
    printf("Memory Released. Exiting Program. \n");
    exit(0);
}

// Extracts the next-space delimited token
char *nextToken(char **cursor){
    while(**cursor && isSpace((char) **cursor)){
        (*cursor)++;
    }
    if(**cursor == '\0') return NULL;

    char *start;
    if(**cursor == '"' || **cursor == '\''){
        char quote = **cursor;
        (*cursor)++;
        start = *cursor;
        while(**cursor && **cursor != quote){
            (*cursor)++;
        }
        if(**cursor == quote){
            **cursor = '\0';
            (*cursor)++;
        }
    } else{
        start = *cursor;
        while(**cursor && !isSpace((char)**cursor)){
            (*cursor)++;
        }
        if(**cursor != '\0'){
            **cursor = '\0';
            (*cursor)++;
        }
    }
    return start;
}

int main(){
    initFileSystem();
    printf("Compact VFS - ready. Type 'exit' to quit.\n");

    char input[MAX_CMD_LINE];
    while(1){
        if(cwd == root){
            printf("/ >");
        } else{
            printf("%s > ", cwd->name);
        }
        fflush(stdout);

        if(!fgets(input, sizeof(input), stdin)) break;

        input[strcspn(input, "\r\n\t")] = '\0';

        char *cursor = input;
        char *cmd = nextToken(&cursor);
        if(cmd == NULL) continue;

        if (strcmp(cmd, "mkdir") == 0) {
            char* arg1 = nextToken(&cursor);
            cmd_mkdir(arg1);
        } else if (strcmp(cmd, "create") == 0) {
            char* arg1 = nextToken(&cursor);
            cmd_create(arg1);
        } else if (strcmp(cmd, "write") == 0) {
            char* arg1 = nextToken(&cursor);
            char* arg2 = nextToken(&cursor);
            cmd_write(arg1, arg2);
        } else if (strcmp(cmd, "read") == 0) {
            char* arg1 = nextToken(&cursor);
            cmd_read(arg1);
        } else if (strcmp(cmd, "delete") == 0) {
            char* arg1 = nextToken(&cursor);
            cmd_delete(arg1);
        } else if (strcmp(cmd, "rmdir") == 0) {
            char* arg1 = nextToken(&cursor);
            cmd_rmdir(arg1);
        } else if (strcmp(cmd, "ls") == 0) {
            cmd_ls();
        } else if (strcmp(cmd, "cd") == 0) {
            char* arg1 = nextToken(&cursor);
            cmd_cd(arg1);
        } else if (strcmp(cmd, "pwd") == 0) {
            cmd_pwd();
        } else if (strcmp(cmd, "df") == 0) {
            cmd_df();
        } else if (strcmp(cmd, "exit") == 0) {
            cleanUpandExit();
        } else {
            printf("Unknown command: '%s'\n", cmd);
        }
    }
    cleanUpandExit();
    return 0;
}