#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef unsigned int uint;

int verbose; /* non-zero if -v option was specified */

int msize; /* total memory size */

int takenIndex = 0;
int deferredIndex = 0;
int totalAllocateSize = 0; 

/*------------------------------------------------------------------------*/
/* The free list is ordered by address, from smallest address to largest. */
/* Although the nodes in the list are allocated using malloc, the region  */
/* addresses are simulated, and range from 0 to msize.                    */
/*------------------------------------------------------------------------*/
struct freg
{                      /* node for a free list region */
    struct freg *next; /* ptr to next node */
    struct freg *prev; /* ptr to previous node */
    uint size;         /* size of the region */
    uint addr;         /* starting address of the region */
} *freelist;           /* head of the free list */

/*---------------------------------------------------*/
/* Declaration of other data structures, as desired. */
/*---------------------------------------------------*/
struct taken
{
    int id;
    uint size;
    uint addr;
} takenlist[10];

struct deferred{
    int id;
    uint size;
} deferredlist[10];


/*---------------------------------------------*/
/* Display the free lists for all size blocks. */
/*---------------------------------------------*/
void show_free_list(void)
{
    uint size;
    int lx;
    struct freg *p;

    printf("  Free List\n");
    printf("-------------\n");
    if (freelist == NULL)
    {
        printf("(empty)\n");
        return;
    }
    printf("Address  Size\n");
    printf("-------  ----\n");
    for (p = freelist; p != NULL; p = p->next)
    {
        printf("%7d  %4d\n", p->addr, p->size);
    }
    putchar('\n');
}

/*---------------------------------------------*/
/* Display occupied memory */
/*---------------------------------------------*/
void show_taken(void){
    printf("Show takenlist: \n");
    for (int i = 0; i < takenIndex; i++){
        if(takenlist[i].id >0){
            printf("id: %d, size: %d, address: %d\n", takenlist[i].id, takenlist[i].size, takenlist[i].addr);
        }
    }
}

void show_deferredlist(void){
    printf("Show deferredlist: \n");
    for (int i = 0; i < deferredIndex; i++){
        if(deferredlist[i].id > 0 ){
            printf("id: %d, size: %d\n", deferredlist[i].id, deferredlist[i].size);
        }
    }
}

#define REQUEST_DEALLOCATE 0
#define REQUEST_ALLOCATE 1
/*--------------------------------------------------------------------*/
/* Get the next allocation/deallocation request and return 1, or      */
/* return 0 at end of file. On input error, diagnose and quit.        */
/* *rid == request ID                                                 */
/*  *rtype = 1 for allocation, 0 for deallocation                     */
/* *size = size of region requested (only if *rtype == 1). *su_20     */
/*--------------------------------------------------------------------*/
int get_request(int *rid, int *rtype, uint *size)
{
    int sfr; /* scanf result */
    int request_id;
    char request_type;
    int request_size;

    sfr = scanf("%d", &request_id);
    if (sfr == EOF)
        return 0;
    if (sfr == 0)
    {
        fprintf(stderr, "trouble reading request ID from input.");
        exit(1);
    }
    sfr = scanf(" %c", &request_type);
    if (sfr != 1)
    {
        fprintf(stderr, "trouble reading request type from input.");
        exit(1);
    }

    if (request_type == '-')
    { /* deallocation request */
        *rid = request_id;
        *rtype = REQUEST_DEALLOCATE;
        return 1;
    }
    else if (request_type == '+')
    { /* allocation request */
        sfr = scanf("%d", &request_size);
        if (sfr != 1)
        {
            fprintf(stderr, "trouble reading allocation request size.");
            exit(1);
        }
        if (request_size < 1 || request_size > msize)
        {
            fprintf(stderr, "input allocation request size (%d) is invalid.",
                    request_size);
            exit(1);
        }
        *rid = request_id;
        *rtype = REQUEST_ALLOCATE;
        *size = (uint)request_size;
        return 1;
    }

    fprintf(stderr, "unrecognized request type: %c", request_type);
    return 0; /* not normally reached */
}

/*------------------------------------------*/
/* Try to perform allocation for a request. */
/* Return 1 if successful, 0 if deferred.   */
/*------------------------------------------*/
int allocate(int *rid, uint *rsize)
{
    int returnAddress = 0;
    struct freg *p;
    /* XXX - TO BE WRITTEN */
    //printf("Doing allocation\n");
    // Check in the freelist to see which one is best fit
    for (p = freelist; p != NULL; p = p->next)
    {
        //printf("%7d  %4d\n", p->addr, p->size);
        if(p->size >= *rsize){
            //printf("Assign to this freelist: %7d  %4d\n", p->addr, p->size);

            //insert into taken array
            takenlist[takenIndex].id = *rid;
            takenlist[takenIndex].size = *rsize;
            takenlist[takenIndex].addr = p->addr;
            takenIndex++;

            returnAddress = p->addr;

            // update totalAllocateSize
            totalAllocateSize += *rsize;

            // use p instead of freelist
            p->size = p->size - *rsize;

            //update freelist address
            p->addr += *rsize;
            // if(p->addr == 0){
            //     p->addr += *rsize;
            // }else{
            //     p->addr += (*rsize - 1);
            // }

            //show_free_list();
            return returnAddress;
            //break;
        }
    }

    // Reach here if there the freelist is not big enough
    // Push to the deferredlist
    deferredlist[deferredIndex].id = *rid;
    deferredlist[deferredIndex].size = *rsize;
    deferredIndex++;

    //update freelist size
    //freelist->size = freelist->size - *rsize;

    //insert into taken array
    // takenlist[takenIndex].id = *rid;
    // takenlist[takenIndex].size = *rsize;
    // takenlist[takenIndex].addr = freelist->addr;
    // takenIndex++;

    //update freelist address
    // if(freelist->addr == 0){
    //     freelist->addr += (*rsize - 1);
    // }else{
    //     freelist->addr += *rsize;
    // }
    
    //printf("size: %d", freelist->size);

    // show_free_list();
    // // printf("Head is pointing to: %d %d\n",freelist->addr, freelist->size);
    // show_taken();
    // show_deferredlist();
    return -1;
}

/*--------------------------*/
/* Deallocate an allocation */
/*--------------------------*/
void deallocate(int *rid)
{
    struct freg *p1;

    //printf("Doing deallocation\n");
    //printf("Before deallocation, Head is pointing to: %d %d\n", freelist->addr, freelist->size);
    //show_free_list();
    p1 = freelist;
    //printf("P1 is pointing to: %d %d\n\n\n", p1->addr, p1->size);
    for (int i = 0; i < 10; i++){
        // Find the corresponding segment in the takenlist 
        if(takenlist[i].id == *rid){    // Find by id
            //show_free_list();
            struct freg *temp = (struct freg *)malloc(sizeof(struct freg));
            // printf("Address of freelist = %x\n", freelist);
            // printf("Address of temp = %x\n", temp);
            // printf("Head is pointing to: %d %d\n\n\n", freelist->addr, freelist->size);
            // printf("Found id %d\n", *rid);
            temp->addr = takenlist[i].addr;
            temp->size = takenlist[i].size;
            // printf("Found block at %d\n", temp->addr);
            // printf("Head is pointing to: %d %d\n\n\n", freelist->addr, freelist->size);
            // printf("P1 is pointing to: %d %d\n\n\n", p1->addr, p1->size);
            //show_free_list();

            //show_free_list();
            // update the doubly linked list

            //printf("Head is pointing to: %d %d\n",freelist->addr, freelist->size);
            
            if(temp->addr < freelist->addr){
                //printf("Block %d is on the left side of the freelist \n", *rid);
                if(temp->addr + temp->size == freelist->addr){
                    freelist->addr = temp->addr;
                    freelist->size += temp->size;
                }else{
                    temp->next = freelist;
                    freelist->prev = temp;
                    freelist = temp;
                }
            }else{
                // printf("Block %d is on the right side of the freelist \n", *rid);
                // //p1 = freelist;
                // printf("freelist->addr = %d  ,   freelist->size = %d\n", freelist->addr, freelist->size);
                // printf("p1->addr = %d  ,   p1->size = %d\n", p1->addr, p1->size);
                while(p1 != NULL){
                    // printf("Inside while loop!\n");
                    // // If the block is on right of the p
                    // printf("p->addr = %d", p1->addr);
                    // printf("p->size = %d", p1->size);
                    // printf("temp->addr = %d\n", temp->addr);
                    // printf("temp->size = %d\n", temp->size);

                    if(p1->addr + p1->size == temp->addr && temp->addr + temp->size == p1->next->addr){ // temp is right the middle p1 / temp / p1->next
                        //printf("Inside 1st IF\n");
                        p1->size = p1->size + temp->size + p1->next->size;
                        if(p1->next->next != NULL){
                            p1->next = p1->next->next;
                            p1->next->next->prev = p1;
                        }else{
                            p1->next = NULL;
                        }
                        break;
                    }

                    if(p1->addr + p1->size == temp->addr){
                        //printf("First IF\n");
                        // temp->next = p1->next;
                        // temp->prev = p1;
                        // p1->next->prev = temp;
                        // p1->next = temp;

                        p1->size += temp->size;
                        break;
                    }

                    if (temp->addr + temp->size == p1->addr){
                        //printf("Second IF\n");
                        // temp->prev = p1->prev;
                        // temp->next = p1;
                        // p1->prev->next = temp;

                        p1->addr = temp->addr;
                        p1->size += temp->size;
                        break;
                    }
                    p1 = p1->next;
                }            
            }

            // Check if temp (deallocate block) is on the left or right of freelist (head)
            // if(temp->addr < freelist->addr){
            //     printf("Block %d is on the left side of the freelist \n", *rid);
            //     temp->next = freelist;
            //     freelist->prev = temp;
            //     freelist = temp;
            // }else{
            //     printf("Block %d is on the right side of the freelist \n", *rid);
            //     //p1 = freelist;
            //     printf("freelist->addr = %d  ,   freelist->size = %d\n", freelist->addr, freelist->size);
            //     printf("p1->addr = %d  ,   p1->size = %d\n", p1->addr, p1->size);
            //     while(p1 != NULL){
            //         printf("Inside while loop!\n");
            //         // If the block is on right of the p
            //         printf("p->addr + p->size = %d    ", p1->addr + p1->size);
            //         printf("temp->addr = %d\n", temp->addr);
            //         if(p1->addr + p1->size == temp->addr){
            //             printf("Updating the doubly linked list\n");
            //             temp->next = p1->next;
            //             temp->prev = p1;
            //             p1->next->prev = temp;
            //             p1->next = temp;
            //             break;
            //         }
            //         p1 = p1->next;
            //     }            
            // }

            //printf("Block %d is on the left side of the freelist with addr = %d\n", *rid, temp->addr);

            //printf("After dealloction: \n");
            //show_free_list();
            //printf("Head is pointing to: %d %d\n\n\n", freelist->addr, freelist->size);

            //show_free_list();
            takenlist[i].id = 0;

            //update totalAllocateSize
            totalAllocateSize -= takenlist[i].size;
            break;
        }
    }

    //show_free_list();
    //show_taken();
    /* XXX - TO BE WRITTEN */
}

/*---------------------------------------------------*/
/* Try to allocate memory for the deferred requests. */
/*---------------------------------------------------*/
void dodef(void)
{
    printf("Doing deferedAllocation\n");
    for (int i = 0; i < 10; i++){
        int returnAddress = 0;
        int rid = deferredlist[i].id;
        uint size = deferredlist[i].size;
        struct freg *p;
        for (p = freelist; p != NULL; p = p->next)
        {
            if(p->size >= size){
                //printf("Assign to this freelist: %7d  %4d\n", p->addr, p->size);

                //insert into taken array
                takenlist[takenIndex].id = rid;
                takenlist[takenIndex].size = size;
                takenlist[takenIndex].addr = p->addr;
                takenIndex++;

                returnAddress = p->addr;

                // update totalAllocateSize
                totalAllocateSize += size;

                // use p instead of freelist
                p->size = p->size - size;

                //update freelist address
                p->addr += size;
                // if(p->addr == 0){
                //     p->addr += *rsize;
                // }else{
                //     p->addr += (*rsize - 1);
                // }

                //show_free_list();
                //return returnAddress;
                //break;
                printf("    Deferred request %d allocated; addr = 0x%08x. Total allocate size = %d\n", rid, returnAddress, totalAllocateSize);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    int rid;    /* request ID */
    int rtype;  /* request type */
    uint rsize; /* request size */
    int addr;     /* non-negative if allocation succeeded */

    /*------------*/
    /* Read msize */
    /*------------*/
    scanf("%d", &msize);

    /*-----------------------------------------------------------*/
    /* Construct an freelist with one entry, size msize, addr 0. */
    /*-----------------------------------------------------------*/
    freelist = (struct freg *)malloc(sizeof(struct freg));
    
    if (freelist == NULL)
    {
        fprintf(stderr, "Out of memory.\n");
        exit(1);
    }

    freelist->addr = 0;
    freelist->size = msize;
    freelist->prev = NULL;
    freelist->next = NULL;

    /*----------------------------------------------------------*/
    /* Create and initialize other data structures, as desired. */
    /*----------------------------------------------------------*/
    /* XXX - TO BE WRITTEN */

    /*-----------------------*/
    /* Process the requests. */
    /*-----------------------*/
    while (get_request(&rid, &rtype, &rsize))
    {
        printf("Request ID %d: ", rid);
        if (rtype == REQUEST_ALLOCATE)
            printf("allocate %d unit%s.\n", rsize, rsize == 1 ? "" : "s");
        else
            printf("deallocate.\n");

        /*----------------------*/
        /* Process the request. */
        /*----------------------*/
        if (rtype == REQUEST_ALLOCATE)
        { /* allocation request */
            addr = allocate(&rid, &rsize);
            if (addr != -1)
            { /* request was successful */
                /* XXX - TO BE WRITTEN */
                printf("   Request Success; addr = 0x%08x. Totall allocate size = %d\n", addr, totalAllocateSize);
            }
            else
            { /* defer the request */
                printf("   Request deferred.  Totall allocate size = %d\n", totalAllocateSize);
                /* XXX - TO BE WRITTEN */
            }
        }
        else
        {                 /* deallocation request */
            deallocate(&rid); /* do the deallocation */
            printf("   Deallocation Success. Totall allocate size = %d\n", totalAllocateSize);

            /*----------------------------------------------*/
            /* Try to perform deferred allocation requests. */
            /*----------------------------------------------*/
            //dodef();
        }
    }

    return 0;
}