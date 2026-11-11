#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<stdbool.h>

#define FIRST_FIT 1
#define BEST_FIT 2
#define WORST_FIT 3
#define MEMORY_SIZE 1000000
#define INITIAL_CAPACITY 4

typedef struct{
    int start ; 
    int size; 
    int id;
    bool is_free;
} Block;

char *pool = NULL; //stimulated memory
Block *blocks = NULL;
int blockcount = 0;
int blockcapacity = 0;
int poolsize = 0;
int nextid = 1;
int strategy = FIRST_FIT; 
int lastfailedsize = 0;
void free_all(){
    free(pool);
    free(blocks);
    pool = NULL;
    blocks = NULL;
}

int read_int(const char *message , int *value){
    int result ,currvalue;
    printf("%s" , message);
    result = scanf("%d" , value);
    if(result == EOF){
        return -1;
    }
    if(result != 1){
        while((currvalue = getchar()) != '\n' && currvalue != EOF) { }
        return 0;
    }
    return 1;
}

int read_valid(const char *message , int min, int max , int *value){
    int status = read_int(message , value);
    if(status < 0){
        return -1;
    }
    if(status == 0){
        printf("Invalid input. Please enter a number.\n");
        return 0;
    }
    if(*value < min || *value > max){
        printf("Value must be between %d and %d.\n", min, max);
        return 0;
    }
    return 1;
}

int ensure_capacity(int neededcount){
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

int insert_block(int index , int start , int size , int id , bool is_free){
    int curr_idx; 
    if(!ensure_capacity(blockcount + 1)){
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

void remove_block (int index){
    int curr_idx;
    for(curr_idx = index; curr_idx < blockcount-1; curr_idx++){
        blocks[curr_idx] = blocks[curr_idx +1];
    }
    blockcount--;
}

int initialize_memory(){
    int status;
    do {
        status = read_valid("Enter memory pool size : " , 1, MEMORY_SIZE , &poolsize);
        if(status < 0){
            return -1;
        }
    } while(status == 0);
    pool = (char*) malloc(poolsize);
    blocks = (Block*) malloc(INITIAL_CAPACITY* sizeof(Block));
    if(pool == NULL || blocks == NULL){
        printf("Error : memory allocation failed.\n");
        free_all();
        return 0;
    }
    memset(pool,'.',poolsize); // '.' for unused byte
    blockcapacity = INITIAL_CAPACITY;
    insert_block(0,0,poolsize,0,true);
    return 1;
}

int change_strategy(void) {
    int choice, status;
    do {
        printf("\nSelect allocation strategy:\n");
        printf("1.First Fit\n");
        printf("2.Best Fit\n");
        printf("3.Worst Fit\n");

        status = read_valid("Enter your choice: " , 1, 3 , &choice);
        if(status < 0){
            return -1;
        }
    } while(status == 0);
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
    return 0;
}

//  each reaturns the INDEX of the chosen free block , or -1 if none is big enough
int first_fit(int size){
    for(int curr_idx = 0;curr_idx<blockcount; curr_idx++){
        if(blocks[curr_idx].is_free && blocks[curr_idx].size >= size){
            return curr_idx;
        }
    }
    return -1;
}

int best_fit(int size){
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
int worst_fit(int size){
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
int find_free_block(int size){
    if (strategy == FIRST_FIT) {
        return first_fit(size);
    }
    if (strategy == BEST_FIT) {
        return best_fit(size);
    }
    return worst_fit(size);
}

int find_allocation(int id){
    for(int curr_idx = 0 ; curr_idx < blockcount;curr_idx++){
        if(!blocks[curr_idx].is_free && blocks[curr_idx].id == id){
            return curr_idx;
        }
    }
    return -1;
}

int find_block_by_start(int start){
     for(int curr_idx = 0 ; curr_idx < blockcount;curr_idx++){
        if(blocks[curr_idx].start == start){
            return curr_idx;
        }
    }
    return -1;
}
int get_total_free(){
    int total = 0;
    for(int curr_idx = 0 ; curr_idx < blockcount;curr_idx++){
        if(blocks[curr_idx].is_free){
            total = total + blocks[curr_idx].size;
        }
    }
    return total;
}

void display_memory_map(void);

void carve_block(int index , int size, int id){
    int leftover = blocks[index].size - size;
    int leftoverstart = blocks[index].start + size;
    if(leftover > 0){
        insert_block(index + 1, leftoverstart , leftover , 0,true);
    }
    blocks[index].size = size;
    blocks[index].id = id;
    blocks[index].is_free = false;
}

int allocate_memory(){
    int size, index, start, status, totalfree, largestfree = 0, curr_idx;
    status = read_valid("Requested size: " , 1, poolsize , &size);
    if(status < 0){
        return -1;
    }
    if(status == 0){
        return 0;
    }
    index = find_free_block(size);
    if(index == -1){
        printf("Allocation failed: no contiguous free block is large enough.\n");
        lastfailedsize = size;
        totalfree = get_total_free();
        for(curr_idx = 0; curr_idx < blockcount; curr_idx++){
            if(blocks[curr_idx].is_free && blocks[curr_idx].size > largestfree){
                largestfree = blocks[curr_idx].size;
            }
        }
        if(totalfree >= size && largestfree < size){
            printf("External fragmentation: %d bytes are free, but the largest contiguous block is %d bytes.\n",
                   totalfree, largestfree);
        }
        display_memory_map();
        return 0;
    }
    // split need one more slot
    if(blocks[index].size > size && !ensure_capacity(blockcount+1)){
        printf("Allocation failed: not enough memory for metadata.\n");
        return 0;
    }
    if(nextid == INT_MAX){
        printf("Allocation failed: no more allocation IDs are available.\n");
        return 0;
    }
    start = blocks[index].start;
    carve_block(index, size ,nextid);
    memset(&pool[start],'A' + (nextid - 1) % 26 ,size);
    printf("Allocation successful. ID: %d, Start: %d, Size: %d\n" , nextid, start, size);
    nextid++;
    return 0;
}

void merge_free_blocks(){
    int curr_idx = 0;
    while(curr_idx < blockcount - 1){
        if(blocks[curr_idx].is_free && blocks[curr_idx + 1].is_free){
            blocks[curr_idx].size = blocks[curr_idx].size + blocks[curr_idx + 1].size;
            remove_block(curr_idx + 1);
        }
        else{
            curr_idx++;
        }
    }
}

int release_memory(){
    int id, index, status;
    status = read_int("Allocation ID: " , &id);
    if(status < 0){
        return -1;
    }
    if(status == 0){
        printf("Invalid input. Please enter a number.\n");
        return 0;
    }
    index = find_allocation(id);
    if(index == -1){
        if(id > 0 && id < nextid){
            printf("Error: allocation %d was already released.\n" , id);
        }
        else{
            printf("Error: unknown allocation ID.\n");
        }
        return 0;
    }
    memset(&pool[blocks[index].start] , '.' , blocks[index].size);
    blocks[index].is_free = true;
    blocks[index].id = 0;
    merge_free_blocks();
    printf("Allocation released.\n");
    return 0;
}

// shrink : returns 1 if successful, else 0
int shrink_allocation(int index , int newsize){
    int leftover = blocks[index].size - newsize;
    int next = index + 1;
    if(next < blockcount && blocks[next].is_free){
        blocks[next].start = blocks[next].start - leftover;
        blocks[next].size = blocks[next].size + leftover;
    }
    else if(!insert_block(next , blocks[index].start + newsize , leftover , 0 , true)){
        return 0;
    }
    memset(&pool[blocks[index].start + newsize] , '.' , leftover);
    blocks[index].size = newsize;
    return 1;
}
 // Returns 1 if done, 0 if impossible.
int expand_in_place(int index , int newsize){
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
        remove_block(next);
    }
    return 1;
}

//returns 1 if successful, else 0
int move_allocation(int index , int newsize){
    int id = blocks[index].id;
    int oldstart = blocks[index].start;
    int oldsize = blocks[index].size;
    int newindex = find_free_block(newsize);
    int newstart, oldindex;
    if(newindex == -1){
        return 0;
    }
    if(blocks[newindex].size > newsize && !ensure_capacity(blockcount + 1)){
        return 0;
    }
    newstart = blocks[newindex].start;
    carve_block(newindex , newsize , id);
    memcpy(&pool[newstart] , &pool[oldstart] , oldsize);
    memset(&pool[newstart +oldsize] ,'A' +(id - 1) % 26,newsize - oldsize);
    // release the OLD block. The split may have shifted the array, so we find it  again by its start offset instead of trusting the old index.
    oldindex = find_block_by_start(oldstart);
    memset(&pool[oldstart] , '.' , oldsize);
    blocks[oldindex].is_free = true;
    blocks[oldindex].id = 0;
    merge_free_blocks();
    return 1;
}

int resize_memory(){
    int id, newsize, index, oldsize, status;
    status = read_int("Allocation ID: " , &id);
    if(status < 0){
        return -1;
    }
    if(status == 0){
        printf("Invalid input. Please enter a number.\n");
        return 0;
    }
    index = find_allocation(id);
    if(index == -1){
        printf("Error: unknown or released allocation ID.\n");
        return 0;
    }
    status = read_valid("New size: " , 1, poolsize , &newsize);
    if(status < 0){
        return -1;
    }
    if(status == 0){
        return 0;
    }
    oldsize = blocks[index].size;
    if(newsize == oldsize){
        printf("The allocation already has that size. Nothing changed.\n");
    }
    else if(newsize < oldsize){
        if(shrink_allocation(index , newsize)){
            merge_free_blocks();
            printf("Allocation %d shrunk from %d to %d bytes.\n" , id, oldsize, newsize);
        }
        else{
            printf("Resize failed: not enough memory for metadata. Allocation unchanged.\n");
        }
    }
    else if(expand_in_place(index , newsize)){
        printf("Allocation %d expanded in place from %d to %d bytes.\n" , id, oldsize, newsize);
    }
    else if(move_allocation(index , newsize)){
        index = find_allocation(id); // array may have shifted
        printf("Allocation %d moved to Start: %d and resized from %d to %d bytes.\n" , id, blocks[index].start, oldsize, newsize);
    }
    else{
        printf("Resize failed: cannot expand in place and no free block is large enough.\n");
        printf("The original allocation is unchanged.\n");
        lastfailedsize = newsize;
    }
    return 0;
}

void display_memory_map(){
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

void display_statistics(){
    int allocatedbytes = 0,allocatedblocks = 0,freeblocks = 0,largestfree = 0;
    int totalfree = get_total_free();
    for(int curr_idx = 0; curr_idx < blockcount; curr_idx++){
        if(blocks[curr_idx].is_free){
            freeblocks++;
            if(blocks[curr_idx].size>largestfree){
                largestfree = blocks[curr_idx].size;
            }
        }
        else{
            allocatedbytes = allocatedbytes + blocks[curr_idx].size;
            allocatedblocks++;
        }
    }
    printf("Total memory: %d bytes\n" , poolsize);
    printf("Allocated: %d bytes\n" , allocatedbytes);
    printf("Free: %d bytes\n" , totalfree);
    printf("Free blocks: %d\n" , freeblocks);
    printf("Largest free block: %d bytes\n" , largestfree);
    printf("Allocated blocks: %d\n", allocatedblocks);
    for(int curr_idx = 0; curr_idx < blockcount; curr_idx++){
        if(!blocks[curr_idx].is_free){
            printf("Allocation ID: %d, Start: %d, Size: %d\n",
                   blocks[curr_idx].id, blocks[curr_idx].start, blocks[curr_idx].size);
        }
    }
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

void check_memory_map(){
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
        if(blocks[curr_idx].is_free == true && blocks[curr_idx].id != 0){
            printf("MAP ERROR at block %d\n", curr_idx);
            return;
        }
        if(blocks[curr_idx].is_free == false && blocks[curr_idx].id <= 0){
            printf("MAP ERROR at block %d\n", curr_idx);
            return;
        }
        if(curr_idx > 0 && blocks[curr_idx].is_free == true && blocks[curr_idx - 1].is_free == true){
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
    int choice, status;
    status = initialize_memory();
    if(status <= 0){
        free_all();
        if(status < 0){
            printf("Input ended. Exiting.\n");
            return 0;
        }
        return 1;
    }
    status = change_strategy();
    if(status < 0){
        printf("Input ended. Exiting.\n");
        free_all();
        return 0;
    }
    printf("\nMenu:\n");
    printf("1.Allocate Memory\n");
    printf("2.Release Memory\n");
    printf("3.Resize Memory\n");
    printf("4.Display Memory Map\n");
    printf("5.Display Statistics\n");
    printf("6.Change Allocation Strategy\n");
    printf("7.Exit\n");
    while(1){
        status = read_valid("Choice: " , 1, 7 , &choice);
        if(status < 0){
            printf("Input ended. Exiting.\n");
            free_all();
            return 0;
        }
        if(status == 0){
            continue;
        }
        switch(choice){
            case 1:
                status = allocate_memory();
                break;
            case 2:
                status = release_memory();
                break;
            case 3:
                status = resize_memory();
                break;
            case 4:
                display_memory_map();
                break;
            case 5:
                display_statistics();
                break;
            case 6:
                status = change_strategy();
                break;
            case 7:
                free_all();
                return 0;
        }
        if(status < 0){
            printf("Input ended. Exiting.\n");
            free_all();
            return 0;
        }
        check_memory_map();
    }
}
