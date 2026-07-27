#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    size_t size;
    int is_free;
    void *next_ptr;
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

    head->next_ptr = new_block;
    return new_block;
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
    current->is_free = 0;
    return (char*)current + sizeof(Block);

}

void sbfree(void* ptr){
    Block* block = (Block*)((char*)ptr - sizeof(Block));
    block->is_free = 1;
}

int main(){
    int* a = sballoc(sizeof(int));
    *a = 42;
    printf("%d\n", *a);
    sbfree(a);
    int* b = sballoc(sizeof(int));
    printf("%p vs %p\n", (void*)a, (void*)b);  // does b reuse a's freed slot?
}
