#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constant Declaration
#define MIN_POOL_SIZE 1
#define MAX_POOL_SIZE 1000000

typedef enum {
    FREE,
    ALLOCATED
} Blockstatus;

typedef struct{
    int id;
    int offset;
    int size;
    Blockstatus status;
} Block;

typedef struct {
    char *pool;
    int poolSize;
    Block *blocks;
    int blockCount;
    int capacity;
    int nextId;
    int strategy;
    int lastFailedsize;
} MemoryManager;


// Function ProtoTypes
int readInt(char *input);
void insertBlock(MemoryManager *memory, int index, Block b);
void removeBlock(MemoryManager *memory, int index);
void mergedFreeBlocks(MemoryManager *memory);
int findFreeblock(MemoryManager *memory, int reqSize);
void allocateMemory(MemoryManager *memory);
void releaseMemory(MemoryManager *memory);
void SelectStrategy(MemoryManager *memory);
void displayMemory(MemoryManager *memory);
void displayMap(MemoryManager *memory);
void resizeAllocation(MemoryManager *memory);
void displayMenu();

// Function to read Integer Safely
int readInt(char *input){
    int val, ch;
    while(1){
        if(input) printf("%s", input);
        if(scanf("%d", &val) == 1){
            while((ch = getchar()) != '\n' && ch != EOF);
            return val;
        }
        while((ch = getchar()) != '\n' && ch != EOF);
        printf("Invalid input. Please enter a valid Number");
    }
}

// Insert Block and reallocate memory
void insertBlock(MemoryManager *memory, int index, Block b){
    if(memory->blockCount >= memory->capacity){
        memory->capacity *= 2;
        memory->blocks = realloc(memory->blocks, memory->capacity * sizeof(Block));

        if(!memory->blocks){
            printf("Netadata reallocation failed. \n");
            exit(1);
        }
    }

    for(int i=memory->blockCount; i > index ; i--){
        memory->blocks[i] = memory->blocks[i - 1];
    }

    memory->blocks[index] = b;
    memory->blockCount++;
}

// Remove block from Memory
void removeBlock(MemoryManager *memory, int index){
    for(int i=index ; i<memory->blockCount - 1; i++){
        memory->blocks[i] = memory->blocks[i + 1];
    }
    memory->blockCount--;
}

// Merges adjacent free blocks into a single block to prevent fragmentation
void mergedFreeBlocks(MemoryManager *memory){
    int i=0;
    while(i < memory->blockCount - 1){
        if(memory->blocks[i].status == FREE && memory->blocks[i + 1].status == FREE){
            memory->blocks[i].size += memory->blocks[i + 1].size;
            removeBlock(memory, i + 1);
        } else{
            i++;
        }
    }
}

// Select the Free block based on user choice
int findFreeblock(MemoryManager *memory, int reqSize){
    int bestIdx = -1;
    int bestSize = -1;

    for(int i=0 ; i<memory->blockCount ; i++){
        if(memory->blocks[i].status == FREE && memory->blocks[i].size >= reqSize){
            if(memory->strategy == 1){
                return i;
            } else if(memory->strategy == 2){
                if(bestIdx == -1 || memory->blocks[i].size < bestSize){
                    bestIdx = i;
                    bestSize = memory->blocks[i].size;
                }
            } else if(memory->strategy == 3){
                if(bestIdx == -1 || memory->blocks[i].size > bestSize){
                    bestIdx = i;
                    bestSize = memory->blocks[i].size;
                }
            }
        }
    }
    return bestIdx;
}

// Allocate Memory based on size of Memory
void allocateMemory(MemoryManager *memory){
    int size = readInt("Requsted Size: ");
    if(size <= 0 || size > memory->poolSize){
        printf("Invalid size. Request must be greater than 0 and fit within total pool size. \n");
        return;
    }

    int idx = findFreeblock(memory, size);
    if(idx == -1){
        printf("Allocation failed: no contigious free block is large enough\n");
        memory->lastFailedsize = size;
        return;
    }

    memory->lastFailedsize = 0;

        if(memory->blocks[idx].size > size){
            Block freePart = {
                .id = 0,
                .offset = memory->blocks[idx].offset + size,
                .size = memory->blocks[idx].size - size,
                .status = FREE
            };
            memory->blocks[idx].size = size;
            memory->blocks[idx].status = ALLOCATED;
            memory->blocks[idx].id = memory->nextId++;
            insertBlock(memory, idx + 1, freePart);
        } else{
            memory->blocks[idx].status = ALLOCATED;
            memory->blocks[idx].id = memory->nextId++;
        }
        printf("Allocation Successfull. ID: %d, Start: %d, Size: %d\n", 
        memory->blocks[idx].id, memory->blocks[idx].offset, memory->blocks[idx].size);
    }

// Release Allocated Memory
void releaseMemory(MemoryManager *memory){
    int id = readInt("Allocation ID: ");
    int found = 0;

    for(int i=0 ; i<memory->blockCount ; i++){
        if(memory->blocks[i].status == ALLOCATED && memory->blocks[i].id == id){
            memory->blocks[i].status = FREE;
            memory->blocks[i].id = 0;
            found = 1;
            break;
        }
    }

    if(!found){
        printf("Release failed: Invalid ID or block already is free\n");
        return;
    }

    mergedFreeBlocks(memory);
    printf("Memory Freed\n");
}

// Asks user to set global allocation strategy
void SelectStrategy(MemoryManager *memory){
    while(1){
        memory->strategy = readInt("Select Allocation strategy (1.First Fit, 2. Best Fit, 3. Worst Fit): ");
        if(memory->strategy >= 1 && memory->strategy <= 3) break;
        printf("Invalid Strategy. Please pick 1, 2 or 3. \n");
    }
    if(memory->strategy == 1) printf("Strategy set to First Fit\n");
    else if(memory->strategy == 2) printf("Strategy set to Best Fit \n");
    else printf("Strategy set to Worst Fit\n");
}

// Calculates and prints current pool stats
void displayMemory(MemoryManager *memory){
    int allocatedBytes = 0;
    int freeBytes = 0;
    int allocatedCount = 0;
    int freeCount = 0;
    int maxFree = 0;

    for(int i=0 ; i<memory->blockCount ; i++){
        if(memory->blocks[i].status == FREE){
            freeBytes += memory->blocks[i].size;
            freeCount++;

            if(memory->blocks[i].size > maxFree){
                maxFree = memory->blocks[i].size;
            }
        } else{
            allocatedBytes += memory->blocks[i].size;
            allocatedCount++;
        }
    }

    printf("\n Total Memory: %d bytes\n", memory->poolSize);
    printf("Allocated: %d bytes\n", allocatedBytes);
    printf("Free: %d \n", freeBytes);
    printf("Free Blocks: %d \n", freeCount);
    printf("Largest Free Block: %d bytes\n", maxFree);

    if(memory->lastFailedsize > 0 && freeBytes >= memory->lastFailedsize){
        printf("\n Note: The request for %d bytes failed even though %d bytes are free, because free memory is fragmented\n", memory->lastFailedsize, freeBytes);
    }
}

// Prints the sequence of allocated and free blocks
void displayMap(MemoryManager *memory){
    printf("\n Memory Map (%d bytes): \n", memory->poolSize);
    for(int i=0 ; i<memory->blockCount ; i++){
        if(memory->blocks[i].status == FREE){
            printf("[Free: %d bytes, Start: %d]\n", memory->blocks[i].size, memory->blocks[i].offset);
        } else{
            printf("[Allocated #%d: %d bytes, Start: %d]\n", memory->blocks[i].id, memory->blocks[i].size, memory->blocks[i].offset);
        }
    }
}

// Shrink or Expand an existing allocation
void resizeAllocation(MemoryManager *memory){
    int id = readInt("Allocation ID: ");
    int idx = -1;

    for(int i=0 ; i<memory->blockCount ; i++){
        if(memory->blocks[i].status == ALLOCATED && memory->blocks[i].id == id){
            idx = i;
            break;
        }
    }

    if(idx == -1){
        printf("Invalid Allocation ID. \n");
        return;
    }

    int newSize = readInt("New Requested Size: ");
    if(newSize <= 0 ||  newSize > memory->poolSize){
        printf("Invalid size.\n");
        return;
    }

    int oldSize = memory->blocks[idx].size;
    if(newSize == oldSize){
        printf("Size is unchanged. No operation performed\n");
        return;
    }

    if(newSize < oldSize){
        Block freePart = {
            .id = 0,
            .offset = memory->blocks[idx].offset + newSize,
            .size = oldSize - newSize,
            .status = FREE
        };
        memory->blocks[idx].size = newSize;
        insertBlock(memory, idx + 1, freePart);
        mergedFreeBlocks(memory);
        printf("Allocation shrunk successfully\n");
        memory->lastFailedsize = 0;
    } else{
        int diff = newSize - oldSize;
        if(idx + 1 < memory->blockCount && memory->blocks[idx + 1].status == FREE && memory->blocks[idx + 1].size >= diff){
            memory->blocks[idx + 1].size -= diff;
            memory->blocks[idx + 1].offset += diff;
            memory->blocks[idx].size = newSize;

            if(memory->blocks[idx + 1].size == 0){
                removeBlock(memory, idx + 1);
            }

            printf("Allocation expanded in-place successfully\n");
            memory->lastFailedsize = 0;
        } else{
            int newidx = findFreeblock(memory, newSize);
            if(newidx == -1){
                printf("Resize failed: no contigious free block is large enough\n");
                memory->lastFailedsize = newSize;
                return;
            }
            memory->lastFailedsize = 0;

            int oldoffSet = memory->blocks[idx].offset;
            int newoffSet = memory->blocks[newidx].offset;

            memcpy(memory->pool + newoffSet, memory->pool + oldoffSet, oldSize);

            if(memory->blocks[newidx].size > newSize){
                Block freePart = {
                    .id = 0,
                    .offset = newoffSet + newSize,
                    .size = memory->blocks[newidx].size - newSize,
                    .status = FREE
                };
                memory->blocks[newidx].size = newSize;
                memory->blocks[newidx].status = ALLOCATED;
                memory->blocks[newidx].id = id;
                insertBlock(memory, newidx + 1, freePart);
            } else{
                memory->blocks[newidx].status = ALLOCATED;
                memory->blocks[newidx].id = id;
            }

            for(int i=0 ; i<memory->blockCount ; i++){
                if(memory->blocks[i].offset == oldoffSet && memory->blocks[i].id == id){
                    memory->blocks[i].status = FREE;
                    memory->blocks[i].id = 0;
                    break;
                }
            }
            mergedFreeBlocks(memory);
            printf("Allocation moved and expanded successfully\n");
        }
    }
}

// Menu for User
void displayMenu(){
    printf("\n Menu: \n");
    printf("1. Allocate memory\n");
    printf("2. Release memory\n");
    printf("3. Resize an allocation\n");
    printf("4. Display memory map\n");
    printf("5. Display memory statistics\n");
    printf("6. change allocation strategy\n");
    printf("7. Exit\n");
}

int main(){
    MemoryManager memory = {0};

    while(1){
        memory.poolSize = readInt("Enter memory pool size: ");
        if(memory.poolSize >= MIN_POOL_SIZE && memory.poolSize <= MAX_POOL_SIZE) break;
        printf("Invalid pool size (must be between %d and %d bytes). \n", MIN_POOL_SIZE, MAX_POOL_SIZE);
    }

    memory.pool = malloc(memory.poolSize);
    memory.capacity = 10;
    memory.blocks = malloc(memory.capacity * sizeof(Block));

    if(!memory.pool || !memory.blocks){
        printf("Initialzation failed: Initial allocations failed\n");
        if(memory.pool) free(memory.pool);
        if(memory.blocks) free(memory.blocks);
        return 1;
    }

    memory.blocks[0].id = 0;
    memory.blocks[0].offset = 0;
    memory.blocks[0].size = memory.poolSize;
    memory.blocks[0].status = FREE;

    memory.blockCount = 1;
    memory.nextId = 1;
    memory.lastFailedsize = 0;

    SelectStrategy(&memory);

    while(1){
        displayMenu();

        int choice = readInt("Choice: ");
        switch(choice){
            case 1: allocateMemory(&memory); break;
            case 2: releaseMemory(&memory); break;
            case 3: resizeAllocation(&memory); break;
            case 4: displayMap(&memory); break;
            case 5: displayMemory(&memory); break;
            case 6: SelectStrategy(&memory); break;
            case 7:
            free(memory.pool);
            free(memory.blocks);
            printf("Memory freed. Exiting program.\n");
            return 0;
            default: 
            printf("Invalid choice. Try again\n");
        }
    }
}