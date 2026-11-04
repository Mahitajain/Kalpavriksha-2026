#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>

#define FIRST_FIT 1
#define BEST_FIT 2
#define WORST_FIT 3
#define MEMORY_SIZE 1000000
#define INITIAL_CAPACITY 4

typedef struct{
    int start ; 
    int size; 
    int id;
    int is_free; // 1 if freee and 0 if allocated
} Block;

char *pool = NULL; //stimulated memory
Block *blocks = NULL;
int blockcount = 0;
int blockcapacity = 0;
int poolsize = 0;
int nextid = 1;
int strategy = FIRST_FIT; 
int lastfailedsize = 0;
void freeall(){
    free(pool);
    free(blocks);
    pool = NULL;
    blocks = NULL;
}

int readInt(const char *message , int *value){
    int result ,currvalue;
    printf("%s" , message);
    result = scanf("%d" , value);
    if(result == EOF){
        printf("Input ended. Exiting.\n");
        freeall();
        exit(0);
    }
    if(result != 1){
        while((currvalue = getchar()) != '\n' && currvalue != EOF) { }
        return 0;
    }
    return 1;
}

int readValid(const char *message , int min, int max , int *value){
    if(!readInt(message , value)){
        printf("Invalid input. Please enter a number.\n");
        return 0;
    }
    if(*value < min || *value > max){
        printf("Value must be between %d and %d.\n", min, max);
        return 0;
    }
    return 1;
}

int ensurecapacity(int neededcount){
    Block *newarray;
    int newcapacity = blockcapacity == 0 ? INITIAL_CAPACITY : blockcapacity * 2;
    if(neededcount <= blockcapacity){
        return 1;
    }
    if(newcapacity < neededcount){
        newcapacity = neededcount;
    }
    newarray = (Block *)realloc(blocks, newcapacity * sizeof(Block));
    if(newarray == NULL){
        printf("Error : could not grow the metadata array.\n");
        return 0;
    }
    blocks = newarray;
    blockcapacity = newcapacity;
    return 1;
}

int insertBlock(int index , int start , int size , int id , int is_free){
    int curr_idx; 
    if(!ensurecapacity(blockcount + 1)){
        return 0;
    }
    for(curr_idx = blockcount; curr_idx > index ; curr_idx--){
        blocks[curr_idx] = blocks[curr_idx - 1];
    }
    blocks[index].start = start;
    blocks[index].size = size;
    blocks[index].id = id;
    blocks[index].is_free = is_free;
    blockcount++;
    return 1;
}

void removeblock (int index){
    int curr_idx;
    for(curr_idx = index; curr_idx < blockcount-1; curr_idx++){
        blocks[curr_idx] = blocks[curr_idx +1];
    }
    blockcount--;
}

int initializeMemory(){
    while(!readValid("Enter memory pool size : " , 1, MEMORY_SIZE , &poolsize)) { }
    pool = (char*) malloc(poolsize);
    blocks = (Block*) malloc(INITIAL_CAPACITY* sizeof(Block));
    if(pool == NULL || blocks == NULL){
        printf("Error : memory allocation failed.\n");
        freeall();
        return 0;
    }
    memset(pool,'.',poolsize); // '.' for unused byte
    blockcapacity = INITIAL_CAPACITY;
    insertBlock(0,0,poolsize,0,1);
    return 1;
}

void changeStrategy(void) {
    int choice;
    do {
        printf("\nSelect allocation strategy:\n");
        printf("1.First Fit\n");
        printf("2.Best Fit\n");
        printf("3.Worst Fit\n");

    } while(!readValid("Enter your choice: " , 1, 3 , &choice));
    strategy = choice;
    if (strategy == 1) {
        printf("Strategy set to First Fit.\n");
    }
    else if (strategy == 2) {
        printf("Strategy set to Best Fit.\n");
    }
    else {
        printf("Strategy set to Worst Fit.\n");
    }
}

//  each reaturns the INDEX of the chosen free block , or -1 if none is big enough
int firstfit(int size){
    for(int curr_idx = 0;curr_idx<blockcount; curr_idx++){
        if(blocks[curr_idx].is_free && blocks[curr_idx].size >= size){
            return curr_idx;
        }
    }
    return -1;
}

int bestfit(int size){
    int curr_idx,best_idx = -1;
    for(curr_idx = 0; curr_idx<blockcount;curr_idx++){
        if(blocks[curr_idx].is_free && blocks[curr_idx].size >= size){
            if(best_idx == -1 || blocks[curr_idx].size < blocks[best_idx].size){
                best_idx = curr_idx;
            }
        }
    }
    return best_idx;
}
int worstfit(int size){
    int worst_idx = -1;
    for(int curr_idx = 0 ; curr_idx < blockcount; curr_idx++){
        if(blocks[curr_idx].is_free && blocks[curr_idx].size >= size){
            if(worst_idx == -1 || blocks[curr_idx].size > blocks[worst_idx].size){
                worst_idx = curr_idx;
            }
        }
    }
    return worst_idx;
}
int findfreeblock(int size){
    if (strategy == FIRST_FIT) {
        return firstfit(size);
    }
    if (strategy == BEST_FIT) {
        return bestfit(size);
    }
    return worstfit(size);
}

int findallocation(int id){
    for(int curr_idx = 0 ; curr_idx < blockcount;curr_idx++){
        if(!blocks[curr_idx].is_free && blocks[curr_idx].id == id){
            return curr_idx;
        }
    }
    return -1;
}

int findblockbystart(int start){
     for(int curr_idx = 0 ; curr_idx < blockcount;curr_idx++){
        if(blocks[curr_idx].start == start){
            return curr_idx;
        }
    }
    return -1;
}
int gettotalfree(){
    int total = 0;
    for(int curr_idx = 0 ; curr_idx < blockcount;curr_idx++){
        if(blocks[curr_idx].is_free){
            total = total + blocks[curr_idx].size;
        }
    }
    return total;
}

void carveblock(int index , int size, int id){
    int leftover = blocks[index].size - size;
    int leftoverstart = blocks[index].start + size;
    if(leftover > 0){
        insertBlock(index + 1, leftoverstart , leftover , 0,1);
    }
    blocks[index].size = size;
    blocks[index].id = id;
    blocks[index].is_free = 0;
}

void allocatememory(){
    int size, index, start;
    if(!readValid("Requested size: " , 1, poolsize , &size)){
        return;
    }
    index = findfreeblock(size);
    if(index == -1){
        printf("Allocation failed: no contiguous free block is large enough.\n");
        lastfailedsize = size;
        return;
    }
    // split need one more slot
    if(blocks[index].size > size && !ensurecapacity(blockcount+1)){
        printf("Allocation failed: not enough memory for metadata.\n");
        return;
    }
    if(nextid == INT_MAX){
        printf("Allocation failed: no more allocation IDs are available.\n");
        return;
    }
    start = blocks[index].start;
    carveblock(index, size ,nextid);
    memset(&pool[start],'A' + (nextid - 1) % 26 ,size);
    printf("Allocation successful. ID: %d, Start: %d, Size: %d\n" , nextid, start, size);
    nextid++;
}

void mergefreeblocks(){
    int curr_idx = 0;
    while(curr_idx < blockcount - 1){
        if(blocks[curr_idx].is_free && blocks[curr_idx + 1].is_free){
            blocks[curr_idx].size = blocks[curr_idx].size + blocks[curr_idx + 1].size;
            removeblock(curr_idx + 1);
        }
        else{
            curr_idx++;
        }
    }
}

void releasememory(){
    int id, index;
    if(!readInt("Allocation ID: " , &id)){
        printf("Invalid input. Please enter a number.\n");
        return;
    }
    index = findallocation(id);
    if(index == -1){
        if(id > 0 && id < nextid){
            printf("Error: allocation %d was already released.\n" , id);
        }
        else{
            printf("Error: unknown allocation ID.\n");
        }
        return;
    }
    memset(&pool[blocks[index].start] , '.' , blocks[index].size);
    blocks[index].is_free = 1;
    blocks[index].id = 0;
    mergefreeblocks();
    printf("Allocation released.\n");
}

// shrink : returns 1 if successful, else 0
int shrinkallocation(int index , int newsize){
    int leftover = blocks[index].size - newsize;
    int next = index + 1;
    if(next < blockcount && blocks[next].is_free){
        blocks[next].start = blocks[next].start - leftover;
        blocks[next].size = blocks[next].size + leftover;
    }
    else if(!insertBlock(next , blocks[index].start + newsize , leftover , 0 , 1)){
        return 0;
    }
    memset(&pool[blocks[index].start + newsize] , '.' , leftover);
    blocks[index].size = newsize;
    return 1;
}
 // Returns 1 if done, 0 if impossible.
int expandinplace(int index , int newsize){
    int extra = newsize - blocks[index].size;
    int next = index + 1;
    if(next >= blockcount || !blocks[next].is_free || blocks[next].size < extra){
        return 0;
    }
    memset(&pool[blocks[index].start + blocks[index].size] ,'A' + (blocks[index].id - 1) % 26 , extra);
    blocks[index].size = newsize;
    blocks[next].start = blocks[next].start + extra;
    blocks[next].size = blocks[next].size - extra;
    if(blocks[next].size == 0){
        removeblock(next);
    }
    return 1;
}

//returns 1 if successful, else 0
int moveallocation(int index , int newsize){
    int id = blocks[index].id;
    int oldstart = blocks[index].start;
    int oldsize = blocks[index].size;
    int newindex = findfreeblock(newsize);
    int newstart, oldindex;
    if(newindex == -1){
        return 0;
    }
    if(blocks[newindex].size > newsize && !ensurecapacity(blockcount + 1)){
        return 0;
    }
    newstart = blocks[newindex].start;
    carveblock(newindex , newsize , id);
    memcpy(&pool[newstart] , &pool[oldstart] , oldsize);
    memset(&pool[newstart +oldsize] ,'A' +(id - 1) % 26,newsize - oldsize);
    // release the OLD block. The split may have shifted the array, so we find it  again by its start offset instead of trusting the old index.
    oldindex = findblockbystart(oldstart);
    memset(&pool[oldstart] , '.' , oldsize);
    blocks[oldindex].is_free = 1;
    blocks[oldindex].id = 0;
    mergefreeblocks();
    return 1;
}

void resizememory(){
    int id, newsize, index, oldsize;
    if(!readInt("Allocation ID: " , &id)){
        printf("Invalid input. Please enter a number.\n");
        return;
    }
    index = findallocation(id);
    if(index == -1){
        printf("Error: unknown or released allocation ID.\n");
        return;
    }
    if(!readValid("New size: " , 1, poolsize , &newsize)){
        return;
    }
    oldsize = blocks[index].size;
    if(newsize == oldsize){
        printf("The allocation already has that size. Nothing changed.\n");
    }
    else if(newsize < oldsize){
        if(shrinkallocation(index , newsize)){
            mergefreeblocks();
            printf("Allocation %d shrunk from %d to %d bytes.\n" , id, oldsize, newsize);
        }
        else{
            printf("Resize failed: not enough memory for metadata. Allocation unchanged.\n");
        }
    }
    else if(expandinplace(index , newsize)){
        printf("Allocation %d expanded in place from %d to %d bytes.\n" , id, oldsize, newsize);
    }
    else if(moveallocation(index , newsize)){
        index = findallocation(id); // array may have shifted
        printf("Allocation %d moved to Start: %d and resized from %d to %d bytes.\n" , id, blocks[index].start, oldsize, newsize);
    }
    else{
        printf("Resize failed: cannot expand in place and no free block is large enough.\n");
        printf("The original allocation is unchanged.\n");
        lastfailedsize = newsize;
    }
}

void displaymemorymap(){
    int curr_idx;
    printf("Memory Map (%d bytes):\n" , poolsize);
    for(curr_idx = 0; curr_idx < blockcount; curr_idx++){
        if(blocks[curr_idx].is_free){
            printf("[Free: %d bytes, Start: %d]\n", blocks[curr_idx].size, blocks[curr_idx].start);
        }
        else{
            printf("[Allocated #%d: %d bytes, Start: %d]\n",blocks[curr_idx].id, blocks[curr_idx].size, blocks[curr_idx].start);
        }
    }
}

void displaystatistics(){
    int allocatedbytes = 0,freeblocks = 0,largestfree = 0;
    int totalfree = gettotalfree();
    for(int curr_idx = 0; curr_idx < blockcount; curr_idx++){
        if(blocks[curr_idx].is_free){
            freeblocks++;
            if(blocks[curr_idx].size>largestfree){
                largestfree = blocks[curr_idx].size;
            }
        }
        else{
            allocatedbytes = allocatedbytes + blocks[curr_idx].size;
        }
    }
    printf("Total memory: %d bytes\n" , poolsize);
    printf("Allocated: %d bytes\n" , allocatedbytes);
    printf("Free: %d bytes\n" , totalfree);
    printf("Free blocks: %d\n" , freeblocks);
    printf("Largest free block: %d bytes\n" , largestfree);
    if(freeblocks > 1 && totalfree > largestfree){
        printf("External fragmentation: Yes\n");
    }
    else{
        printf("Extrnal fragmentation: No\n");
    }
    if(lastfailedsize > largestfree && totalfree >= lastfailedsize){
        printf("The request for %d bytes failed even though %d bytes are free, because the free memory is fragmented.\n" ,lastfailedsize, totalfree);
    }
}

void checkmemorymap(){
    int expectedstart = 0;
    if(pool == NULL || blocks == NULL){
        printf("Memory is not initialized.\n");
        return;
    }

    for(int curr_idx = 0; curr_idx < blockcount; curr_idx++){

        if(blocks[curr_idx].start != expectedstart || blocks[curr_idx].size <= 0){
            printf("MAP ERROR at block %d\n", curr_idx);
            return;
        }
        if(blocks[curr_idx].is_free == 1 && blocks[curr_idx].id != 0){
            printf("MAP ERROR at block %d\n", curr_idx);
            return;
        }
        if(blocks[curr_idx].is_free == 0 && blocks[curr_idx].id <= 0){
            printf("MAP ERROR at block %d\n", curr_idx);
            return;
        }
        if(curr_idx > 0 && blocks[curr_idx].is_free == 1 && blocks[curr_idx - 1].is_free == 1){
            printf("MAP ERROR: adjacent free blocks found.\n");
            return;
        }
        expectedstart = expectedstart + blocks[curr_idx].size;
    }
    if(expectedstart != poolsize){
        printf("MAP ERROR: memory size does not match.\n");
        return;
    }
}

int main(){
    int choice;
    if(!initializeMemory()){
        return 1;
    }
    changeStrategy();
    printf("\nMenu:\n");
    printf("1.Allocate Memory\n");
    printf("2.Release Memory\n");
    printf("3.Resize Memory\n");
    printf("4.Display Memory Map\n");
    printf("5.Display Statistics\n");
    printf("6.Change Allocation Strategy\n");
    printf("7.Exit\n");
    while(1){
        if(!readValid("Choice: " , 1, 7 , &choice)){
            continue;
        }
        switch(choice){
            case 1:
                allocatememory();
                break;
            case 2:
                releasememory();
                break;
            case 3:
                resizememory();
                break;
            case 4:
                displaymemorymap();
                break;
            case 5:
                displaystatistics();
                break;
            case 6:
                changeStrategy();
                break;
            case 7:
                freeall();
                return 0;
        }
        checkmemorymap();
    }
}
