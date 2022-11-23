/*--------------------------------------------------------------------*/
/* bestfit_temp.c                                                     */
/* This is a partial solution, or template, for the programming       */
/* assignment for CSCI4500. It is reasonably-well commented, and      */
/* those places where additional code must be provided                */
/* or changed are marked with comments like "XXX - TO BE WRITTEN".    */
/*--------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef unsigned int uint;

int verbose; /* non-zero if -v option was specified */

int msize; /* total memory size */

int takenIndex = 0;

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
    for (int i = 0; i < 10; i++){
        if(takenlist[i].id >0){
            printf("id: %d, size: %d, address: %d\n", takenlist[i].id, takenlist[i].size, takenlist[i].addr);
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
    struct freg *p;
    /* XXX - TO BE WRITTEN */
    printf("Doing allocation\n");
    // Check in the freelist to see which one is best fit
    for (p = freelist; p != NULL; p = p->next)
    {
        //printf("%7d  %4d\n", p->addr, p->size);
        if(p->size > *rsize){
            printf("Assign to this freelist: %7d  %4d\n", p->addr, p->size);
            // use p instead of freelist
            freelist->size = freelist->size - *rsize;
            break;
        }
    }

    //update freelist size
    //freelist->size = freelist->size - *rsize;

    //insert into taken array
    takenlist[takenIndex].id = *rid;
    takenlist[takenIndex].size = *rsize;
    takenlist[takenIndex].addr = freelist->addr;
    takenIndex++;

    //update freelist address
    if(freelist->addr == 0){
        freelist->addr += (*rsize - 1);
    }else{
        freelist->addr += *rsize;
    }
    
    //printf("size: %d", freelist->size);
    show_free_list();
    printf("Head is pointing to: %d %d\n",freelist->addr, freelist->size);
    show_taken();
    return 0;
}

/*--------------------------*/
/* Deallocate an allocation */
/*--------------------------*/
void deallocate(int *rid)
{
    printf("Doing deallocation\n");
    for (int i = 0; i < 10; i++){
        if(takenlist[i].id == *rid){
            struct freg *temp;
            temp->addr = takenlist[i].addr;
            temp->size = takenlist[i].size;
            temp->next = freelist;
            freelist = temp;  

            // update the doubly linked list
        }
        //remove from the takenlist
        takenlist[i].id = 0;
    }

    show_free_list();
    show_taken();
    /* XXX - TO BE WRITTEN */
}

/*---------------------------------------------------*/
/* Try to allocate memory for the deferred requests. */
/*---------------------------------------------------*/
void dodef(void)
{
    printf("Doing deferedAllocation\n");
    /* XXX - TO BE WRITTEN */
}

int main(int argc, char *argv[])
{
    int rid;    /* request ID */
    int rtype;  /* request type */
    uint rsize; /* request size */
    int ok;     /* non-zero if allocation succeeded */

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
            ok = allocate(&rid, &rsize);
            if (ok)
            { /* request was successful */
                /* XXX - TO BE WRITTEN */
                printf("   Request Success; addr = %u.\n", 999);
            }
            else
            { /* defer the request */
                printf("   Request deferred.\n");
                /* XXX - TO BE WRITTEN */
            }
        }
        else
        {                 /* deallocation request */
            deallocate(&rid); /* do the deallocation */
            printf("   Deallocation Success.\n");

            /*----------------------------------------------*/
            /* Try to perform deferred allocation requests. */
            /*----------------------------------------------*/
            dodef();
        }
    }

    return 0;
}
