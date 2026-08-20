#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    size_t size;
    int is_free;
    void *next_ptr;
    void *prev_ptr;
} Block;

Block* header = NULL;
Block* init_heap(size_t size){
    Block* head = (Block *)sbrk(sizeof(Block) + (size));
    if (head == (void*)-1){
        return NULL;
    }
    head->size = (size);
    head->is_free = 1;
    head->next_ptr = NULL;
    head->prev_ptr = NULL;
    header = head;
    return head;
}


Block* free_block(size_t size){
    Block* current = header;
    while(current && !(current->is_free && current->size >= size)){
        current = current->next_ptr;
    }
    return current;
}

Block* find_last_block(){
    Block* current = header;
    while(current->next_ptr != NULL){
        current = current->next_ptr;
    }
    return current;
}

Block* extend_heap(size_t size){
    Block* head = find_last_block();
    Block* new_block = (Block* )sbrk(sizeof(Block) + size);
    if (new_block == (void*)-1){
        return NULL;
    }
    new_block->size = size;
    new_block->is_free = 1;
    new_block->next_ptr = NULL;
    new_block->prev_ptr = head;

    head->next_ptr = new_block;
    return new_block;
}


void split(Block* block, size_t size){
    if (block->size >= size + sizeof(Block) + 1){
        Block* remainder = (Block*)((char*)block + sizeof(Block) + size);
        
        remainder->size = block->size - size - sizeof(Block);
        remainder->is_free = 1;
        
        remainder->next_ptr = block->next_ptr;
        remainder->prev_ptr = block;
        block->next_ptr = remainder;
        if((remainder->next_ptr) != NULL){
            ((Block*)remainder->next_ptr)->prev_ptr = remainder;
        }
        block->size = size;
    }
}

void* sballoc(int size){
    if (header == NULL){
        Block* head = init_heap(size);
        head->is_free = 0;
        return (char*)head + sizeof(Block);
    }
    Block* current = free_block(size);
    if(current == NULL){
        Block* new_block = extend_heap(size);
        if(new_block != NULL){
            new_block->is_free = 0;
            return (char*)new_block + sizeof(Block);
        }else{
            return NULL;
        }
    }
    split(current, size);
    current->is_free = 0;
    return (char*)current + sizeof(Block);

}


void collease(Block* block){
    if(block->next_ptr != NULL && (((Block*)block->next_ptr)->is_free == 1)){
        block->size += sizeof(Block) + ((Block*)block->next_ptr)->size;
        block->next_ptr = ((Block*)block->next_ptr)->next_ptr;
        if((Block*)block->next_ptr != NULL){
            ((Block*)block->next_ptr)->prev_ptr = block;
        }
    }
    if(block->prev_ptr != NULL && (((Block*)block->prev_ptr)->is_free == 1)){
        Block* prev  = (Block*)block->prev_ptr;
        prev->size += sizeof(Block) + block->size;
        prev->next_ptr = block->next_ptr;
        if(block->next_ptr != NULL){
            ((Block*)block->next_ptr)->prev_ptr = prev;
        }
    }
}

void sbfree(void* ptr){
    Block* block = (Block*)((char*)ptr - sizeof(Block));
    block->is_free = 1;
    collease(block);
}

int main(){
    int* a = sballoc(sizeof(int));
    *a = 42;
    printf("%d\n", *a);
    sbfree(a);
    int* b = sballoc(sizeof(int));
    printf("%p vs %p\n", (void*)a, (void*)b);  

     // allocate three adjacent blocks
    int* d = sballoc(100);
    int* e = sballoc(100);
    int* f = sballoc(100);
    
    // free all three — should coalesce into one big block
    sbfree(d);
    sbfree(e);
    sbfree(f);
    
    // now try allocating something bigger than any single block
    // if coalescing worked, this should reuse the merged space
    int* g = sballoc(250);
    printf("g: %p, d: %p — should be same address if coalesced\n", 
           (void*)g, (void*)d);

    // allocate a big block
    char* n = sballoc(200);
    sbfree(n);
    
    // now allocate something small — should split the 200-byte block
    char* m = sballoc(10);
    char* o = sballoc(10);
    
    // m and o should be different addresses, both within the original 200-byte region
    printf("m: %p\n", (void*)m);
    printf("o: %p\n", (void*)o);
    // o should be close to m (within the split remainder), not a brand new sbrk allocation
}

