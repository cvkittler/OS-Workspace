#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include "goatmalloc.h"

int statusno = ERR_UNINITIALIZED;
node_t *_arena_start;

int cleanFwd(node_t * curNode);
int cleanBwd(node_t * curNode);

extern int init(size_t size){
    
    //check to see if positive size
    if((int)size < 0){
        return ERR_BAD_ARGUMENTS;
    }
    printf("Initializing arena:\n");
    printf("...requested size %ld bytes\n",size);
    int pageSize = getpagesize();
    printf("...pagesize is %d bytes\n",pageSize);

    //adjust the size to fit the data
    int numPages;
    if((int)size % pageSize != 0){
        numPages = ((int)size / pageSize) + 1;
    }else{
        numPages = ((int)size / pageSize);
    }
    int adjSize = pageSize * numPages;
    printf("...adjusted size is %d bytes\n", adjSize);

    //get the arena
    printf("...mapping arena with mmap()\n");
    int fd=open("/dev/zero",O_RDWR);
    _arena_start = mmap(NULL, adjSize , PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    //check to see if the arena was gotten
    if(_arena_start == MAP_FAILED){
        return ERR_SYSCALL_FAILED;
    }
    printf("...arena starts at %p\n", _arena_start);
    printf("...arena ends at %p\n", _arena_start + adjSize);

    //initialize the header
    printf("...initializing header for initial free chunk\n");
    node_t * header = _arena_start;
    (*header).size = (size_t)adjSize -sizeof(node_t);
    (*header).is_free = (unsigned short)1;
    (*header).bwd = (void*)NULL;
    (*header).fwd = (void*)NULL;
    printf("...header size is %ld bytes\n",sizeof(*header));
    return adjSize;
}

int destroy(){
    printf("Destroying Arena:\n");
    if(_arena_start == NULL){//no data
        printf("...error: cannot destroy unintialized arena. Setting error status\n");
        return ERR_UNINITIALIZED;
    }else{//there is data
        munmap(_arena_start, _arena_start->size);
        printf("...unmapping arena with munmap()\n");
        _arena_start = NULL;
        statusno = ERR_UNINITIALIZED;
    }
    return 0;
}

void* walloc(size_t size){
    if(_arena_start == NULL){//no data
        statusno =  ERR_UNINITIALIZED;
        printf("Error: Unitialized. Setting status code\n");
        return NULL;
    }else{
        printf("Allocating memory:\n...looking for free chunk of >= %ld bytes\n", size);
        node_t *curNode = _arena_start;
        while(curNode->fwd != NULL && !curNode->is_free){
            curNode = curNode->fwd;
        }
        //check to see if there is space
        if((unsigned short)curNode->size >= (unsigned short)size && curNode->is_free){
            printf("...found free chunk of %ld bytes with header at %p\n", size, curNode);
            printf("...free chunk->fwd currently points to %p\n",curNode->fwd);
            printf("...free chunk->bwd currently points to %p\n",curNode->bwd);
            printf("...checking if splitting is required\n");
            if((unsigned short)curNode->size - (unsigned short)size < (unsigned short)sizeof(node_t) + 1){
                //dont split
                printf("...splitting not required\n");
                printf("...updating chunk header at %p\n",curNode);
                printf("...being careful with my pointer arthimetic and void pointer casting\n");
            }else{//spilt chunk
                printf("...splitting free chunk\n");
                //make another header
                node_t * header = (node_t *)((unsigned long)curNode + (unsigned long)size + (unsigned long)sizeof(node_t));
                (*header).size = (size_t)curNode->size - sizeof(node_t) - size;
                (*header).is_free = (unsigned short)1;
                (*header).bwd = curNode;
                (*header).fwd = (void*)NULL;

                //update header
                (*curNode).size = size;
                (*curNode).fwd = header;
            }
            (*curNode).is_free = (unsigned short)0;
            printf("...allocation starts at %p\n",&curNode);
            return (void *)((unsigned long)curNode + (unsigned long)sizeof(node_t));
        }else{
            statusno = ERR_OUT_OF_MEMORY;
            printf("...no such free chunk exists\n...setting error code\n");
            return NULL;
        }
        
    }
}
extern void wfree(void *ptr){
    printf("Freeing allocated memory:\n");
    printf("...supplied pointer %p\n...being careful with my pointer arthimetic and void pointer casting\n", ptr);
    node_t * chunk = ptr - sizeof(node_t);
    printf("...accessing chunk header at %p\n", chunk);
    printf("...chunk of size %ld\n",chunk->size);
    printf("...checking if coalescing is needed\n");
    (*chunk).is_free = (unsigned short)1;
    //check for coalescing
    int fwdFlag = 0, bwdFlag = 0;
    if((chunk->fwd != NULL)){
        if(chunk->fwd->is_free){
            fwdFlag = cleanFwd(chunk);
        }
    }
    if((chunk->bwd != NULL)){
        if(chunk->bwd->is_free){
            bwdFlag = cleanBwd(chunk);
        }
    }
    if(fwdFlag && bwdFlag){//case 1
        printf("...col. case 1: previous, current, and next chunks all free.\n");
    }else if(!fwdFlag && bwdFlag){
        printf("...col. case 2: previous and current chunks free.\n");
    }else if (fwdFlag && !bwdFlag){
        printf("..col. case 3: current and next chunks free.\n");
    }else{
        printf("...coalescing not needed.\n");
    }
    
}

int cleanFwd(node_t * curNode){
    int numCleans = 1;
    (*curNode).size = curNode->size + curNode->fwd->size + sizeof(node_t);
    (*curNode).fwd = curNode->fwd->fwd;
    if(curNode->fwd != NULL){
        (*curNode->fwd).bwd = curNode;
    }
    if((curNode->fwd != NULL)){
        if(curNode->fwd->is_free){
            numCleans = numCleans + cleanFwd(curNode);
        }
    }
    return numCleans;
}

int cleanBwd(node_t * curNode){
    int numCleans = 1;
    (*curNode->bwd).size = curNode->size + curNode->bwd->size + sizeof(node_t);
    (*curNode->bwd).fwd = curNode->fwd;
    if(curNode->fwd != NULL){
        (*curNode->fwd).bwd = curNode->bwd;
    }
    if((curNode->bwd->bwd != NULL)){
        if(curNode->bwd->is_free){
            numCleans = numCleans + cleanBwd(curNode->bwd);
        }
    }
    return numCleans;
}