//Name: Anthony Bravo

#include <iostream>
#include <cstdlib>

struct listnode { struct listnode * next;
    unsigned int key; };

struct listnode * sort( struct listnode * list);

int main(void)
{
    unsigned int i;
    struct listnode *list, *tmp;
    struct listnode space[1000000];
    for( i=0; i< 1000000; i++ )
        (space[i]).key  = 2*((139*i)%1000000);
    for( i=0; i< 1000000 -1; i++ )
        (space[i]).next = &(space[i+1]);
    (space[1000000 -1]).next = NULL;
    printf(" Finished preparing list\n"); fflush(stdout);
    list = &(space[0]);
    list = sort( list);
    for( tmp = list, i=0; tmp != NULL; tmp = tmp->next, i++)
        ;
    printf(" before sort: %d list items.\n", i); fflush( stdout);
    for( tmp = list, i=0; tmp != NULL; tmp = tmp->next, i++)
        ;
    printf(" after  sort: %d list items.\n", i); fflush( stdout);
    for( i=0; i< 1000000; i++ )
    {  if( list-> key != 2*i )
    { printf("something wrong\n"); fflush(stdout); exit(-1);
    }
        list = list-> next;
    }
    printf(" all sorted\n"); fflush(stdout);
}

struct listnode * sort(struct listnode * list) {
    
    struct listnode* bucket_start[65536];
    struct listnode* bucket_end[65536];
    for(int i=0;i<65536;i++) {
        bucket_start[i] = NULL;
        bucket_end[i] = NULL;
    }

    //Placing list on buckets
    while(list!=NULL) {
        struct listnode* temp = list;
        list = list->next;
        temp->next = NULL;
        int a = temp->key;
        int index = a & 0xFFFF;
        if(bucket_start[index]==NULL) {
            bucket_start[index] = temp;
            bucket_end[index] = temp;
        }
        else {
            bucket_end[index]->next=temp;
            bucket_end[index] = temp ;
        }
    }
    
    //Making a list back again from buckets array
    struct listnode* tail;
    for(int i=0;i<65536;i++) {
        //if list is empty just insert it
        if(bucket_start[i] != NULL && list==NULL ) {
            list = bucket_start[i];
            tail = bucket_end[i];
        }
        //other wise insert at the end
        else if (bucket_start[i] != NULL)
        {
            tail->next = bucket_start[i];
            tail = bucket_end[i];
        }
    }
    
    //Initializing buckets to NULL again
    for(size_t i=0;i<65536;i++) {
        bucket_start[i] = NULL;
        bucket_end[i] = NULL;
    }
    
    
    //Upper 16 bits part:
    
    while(list!=NULL) {
        struct listnode* temp = list;
        list = list->next;
        temp->next = NULL;
        int a = temp->key;
        int index = (a>>16) & 0xFFFF;
        if(bucket_start[index]==NULL) {
            bucket_start[index] = temp;
            bucket_end[index] = temp;
        }
        else {
            bucket_end[index]->next=temp;
            bucket_end[index] = temp ;
        }
    }
    
    
    //Making a list back again from buckets array
    for(int i=0;i<65536;i++) {
        if(bucket_start[i] != NULL && list==NULL ) {
            list = bucket_start[i];
            tail = bucket_end[i];
        }
        
        else if (bucket_start[i] != NULL) {
            tail->next = bucket_start[i];
            tail = bucket_end[i];
        }
    }

    return list;
}



