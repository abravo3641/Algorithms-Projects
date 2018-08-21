//Name: Anthony Bravo

#include <stdio.h>
#include <stdlib.h>

#define BLOCKSIZE 256

typedef int object_t;
typedef int key_t;
typedef struct tr_n_t { key_t        key;
                    struct tr_n_t  *left;
                    struct tr_n_t *right;
                    int           height;
                    int           leaves;
                      } tree_node_t;



tree_node_t *currentblock = NULL;
int    size_left;
tree_node_t *free_list = NULL;

tree_node_t *get_node()
{ tree_node_t *tmp;
  if( free_list != NULL )
  {  tmp = free_list;
     free_list = free_list -> left;
  }
  else
  {  if( currentblock == NULL || size_left == 0)
     {  currentblock = 
                (tree_node_t *) malloc( BLOCKSIZE * sizeof(tree_node_t) );
        size_left = BLOCKSIZE;
     }
     tmp = currentblock++;
     size_left -= 1;
  }
  return( tmp );
}


void return_node(tree_node_t *node)
{  node->left = free_list;
   free_list = node;
}


tree_node_t *create_tree(void)
{  tree_node_t *tmp_node;
   tmp_node = get_node();
   tmp_node->left = NULL;
   return( tmp_node );
}

void left_rotation(tree_node_t *n)
{  tree_node_t *tmp_node;
   key_t        tmp_key;
   tmp_node = n->left; 
   tmp_key  = n->key;
   n->left  = n->right;        
   n->key   = n->right->key;
   n->right = n->left->right;  
   n->left->right = n->left->left;
   n->left->left  = tmp_node;
   n->left->key   = tmp_key;
}

void right_rotation(tree_node_t *n)
{  tree_node_t *tmp_node;
   key_t        tmp_key;
   tmp_node = n->right; 
   tmp_key  = n->key;
   n->right = n->left;        
   n->key   = n->left->key;
   n->left  = n->right->left;  
   n->right->left = n->right->right;
   n->right->right  = tmp_node;
   n->right->key   = tmp_key;
}

object_t *find(tree_node_t *tree, key_t query_key)
{  tree_node_t *tmp_node;
   if( tree->left == NULL )
     return(NULL);
   else
   {  tmp_node = tree;
      while( tmp_node->right != NULL )
      {   if( query_key < tmp_node->key )
               tmp_node = tmp_node->left;
          else
               tmp_node = tmp_node->right;
      }
      if( tmp_node->key == query_key )
         return( (object_t *) tmp_node->left );
      else
         return( NULL );
   }
}


int insert(tree_node_t *tree, key_t new_key, object_t *new_object)
{  tree_node_t *tmp_node;
   int finished;
   if( tree->left == NULL )
   {  tree->left = (tree_node_t *) new_object;
      tree->key  = new_key;
      tree->height = 0;
      tree->right  = NULL; 
   }
   else
     {  tree_node_t * path_stack[100]; int  path_st_p = 0;
      tmp_node = tree; 
      while( tmp_node->right != NULL )
      {   path_stack[path_st_p++] = tmp_node;
          if( new_key < tmp_node->key )
               tmp_node = tmp_node->left;
          else
               tmp_node = tmp_node->right;
      }
      /* found the candidate leaf. Test whether key distinct */
      if( tmp_node->key == new_key )
         return( -1 );
      /* key is distinct, now perform the insert */ 
      {  tree_node_t *old_leaf, *new_leaf;
         old_leaf = get_node();
         old_leaf->left = tmp_node->left; 
         old_leaf->key = tmp_node->key;
         old_leaf->right  = NULL;
         old_leaf->height = 0;
         new_leaf = get_node();
         new_leaf->left = (tree_node_t *) new_object; 
         new_leaf->key = new_key;
         new_leaf->right  = NULL;
         new_leaf->height = 0; 
         if( tmp_node->key < new_key )
         {   tmp_node->left  = old_leaf;
             tmp_node->right = new_leaf;
             tmp_node->key = new_key;
         } 
         else
         {   tmp_node->left  = new_leaf;
             tmp_node->right = old_leaf;
         } 
         tmp_node->height = 1;
      }
      /* rebalance */
      finished = 0;
      while( path_st_p > 0 && !finished )
      {  int tmp_height, old_height;
         tmp_node = path_stack[--path_st_p];
         old_height= tmp_node->height;
         if( tmp_node->left->height - 
                                 tmp_node->right->height == 2 )
         {  if( tmp_node->left->left->height - 
                                 tmp_node->right->height == 1 )
            {  right_rotation( tmp_node );
               tmp_node->right->height = 
                            tmp_node->right->left->height + 1;
               tmp_node->height = tmp_node->right->height + 1;
            }
            else
            {  left_rotation( tmp_node->left );
               right_rotation( tmp_node );
               tmp_height = tmp_node->left->left->height; 
               tmp_node->left->height  = tmp_height + 1; 
               tmp_node->right->height = tmp_height + 1; 
               tmp_node->height = tmp_height + 2; 
            }
         }
         else if ( tmp_node->left->height - 
                                tmp_node->right->height == -2 )
         {  if( tmp_node->right->right->height - 
                                  tmp_node->left->height == 1 )
            {  left_rotation( tmp_node );
               tmp_node->left->height = 
                           tmp_node->left->right->height + 1;
               tmp_node->height = tmp_node->left->height + 1;
            }
            else
            {  right_rotation( tmp_node->right );
               left_rotation( tmp_node );
               tmp_height = tmp_node->right->right->height; 
               tmp_node->left->height  = tmp_height + 1; 
               tmp_node->right->height = tmp_height + 1; 
               tmp_node->height = tmp_height + 2; 
            }
         }
         else /* update height even if there was no rotation */ 
         {  if( tmp_node->left->height > tmp_node->right->height )
               tmp_node->height = tmp_node->left->height + 1;
            else
               tmp_node->height = tmp_node->right->height + 1;
         }
         if( tmp_node->height == old_height )
            finished = 1;
      }
      
   }
   return( 0 );
}



object_t *delete(tree_node_t *tree, key_t delete_key)
{  tree_node_t *tmp_node, *upper_node, *other_node;
   object_t *deleted_object; int finished;
   if( tree->left == NULL )
      return( NULL );
   else if( tree->right == NULL )
   {  if(  tree->key == delete_key )
      {  deleted_object = (object_t *) tree->left;
         tree->left = NULL;
         return( deleted_object );
      }
      else
         return( NULL );
   }
   else
   {  tree_node_t * path_stack[100]; int path_st_p = 0;
      tmp_node = tree;
      while( tmp_node->right != NULL )
      {   path_stack[path_st_p++] = tmp_node;  
          upper_node = tmp_node;
          if( delete_key < tmp_node->key )
          {  tmp_node   = upper_node->left; 
             other_node = upper_node->right;
          } 
          else
          {  tmp_node   = upper_node->right; 
             other_node = upper_node->left;
          } 
      }
      if( tmp_node->key != delete_key )
         deleted_object = NULL;
      else
      {  upper_node->key   = other_node->key;
         upper_node->left  = other_node->left;
         upper_node->right = other_node->right;
         upper_node->height = other_node->height;
         deleted_object = (object_t *) tmp_node->left;
         return_node( tmp_node );
         return_node( other_node );

      }
      /*start rebalance*/  
      finished = 0; path_st_p -= 1;
      while( path_st_p > 0 && !finished )
      {  int tmp_height, old_height;
         tmp_node = path_stack[--path_st_p];
         old_height= tmp_node->height;
         if( tmp_node->left->height - 
                                 tmp_node->right->height == 2 )
         {  if( tmp_node->left->left->height - 
                                 tmp_node->right->height == 1 )
	    {  right_rotation( tmp_node ); 
               tmp_node->right->height = 
                            tmp_node->right->left->height + 1;
               tmp_node->height = tmp_node->right->height + 1;
            }
            else
	    {  left_rotation( tmp_node->left ); 
               right_rotation( tmp_node );
               tmp_height = tmp_node->left->left->height; 
               tmp_node->left->height  = tmp_height + 1; 
               tmp_node->right->height = tmp_height + 1; 
               tmp_node->height = tmp_height + 2; 
            }
         }
         else if ( tmp_node->left->height - 
                                tmp_node->right->height == -2 )
         {  if( tmp_node->right->right->height - 
                                  tmp_node->left->height == 1 )
	    {  left_rotation( tmp_node ); 
               tmp_node->left->height = 
                           tmp_node->left->right->height + 1;
               tmp_node->height = tmp_node->left->height + 1;
            }
            else
            {  right_rotation( tmp_node->right );
               left_rotation( tmp_node );
               tmp_height = tmp_node->right->right->height; 
               tmp_node->left->height  = tmp_height + 1; 
               tmp_node->right->height = tmp_height + 1; 
               tmp_node->height = tmp_height + 2; 
            }
         }
         else /* update height even if there was no rotation */ 
         {  if( tmp_node->left->height > tmp_node->right->height )
               tmp_node->height = tmp_node->left->height + 1;
            else
               tmp_node->height = tmp_node->right->height + 1;
         }
         if( tmp_node->height == old_height )
	    finished = 1;
      }
      /*end rebalance*/
      return( deleted_object );
   }
}


void check_tree( tree_node_t *tr, int depth, int lower, int upper )
{  if( tr->left == NULL )
   {  printf("Tree Empty\n"); return; }
   if( tr->key < lower || tr->key >= upper )
         printf("Wrong Key Order \n");
   if( tr->right == NULL )
   {  if( *( (int *) tr->left) == 10*tr->key + 2 )
         printf("%d(%d)  ", tr->key, depth );
      else
         printf("Wrong Object \n");
   }
   else
   {  check_tree(tr->left, depth+1, lower, tr->key ); 
      check_tree(tr->right, depth+1, tr->key, upper ); 
   }
}






object_t *find_by_number(tree_node_t *tree, int k) {
    tree_node_t *temp;
    if(k<=0 || k>tree->leaves) { //Possible invalid inputs
        return NULL;
    }
    else {
        temp = tree;
        while(temp->right!=NULL) { //Not a leaft
            if(k<=temp->left->leaves) {
                temp = temp->left;
            }
            else {
                k-=temp->left->leaves; //update k
                temp = temp->right;
            }
        }
    }
    return (object_t *) temp->left;
}



void insert_by_number(tree_node_t *tree,int k, object_t *new_obj) {
    
    if(k<=0 || k>tree->leaves+1) { //Possible invalid inputs
        return;
    }
    
    else {
        int last = 0; //if we are inserting at the k+1 leaf
        
        //Tree is empty:just insert
        if(tree->left == NULL) {
            tree->left = (tree_node_t *) new_obj;
            tree->height = 0;
            tree->leaves = 1;
            tree->right = NULL;
        }
        else {
            //if we are inserting at the k+1 leaf
            if(k==tree->leaves+1)
                last = 1;
            
            //Find the right kth leaf, but keep track of path to the root in a stack
            tree_node_t * path_stack[100]; int path_st_p = 0;
            tree_node_t *tmp_node = tree;
 
            while(tmp_node->right != NULL) {
                path_stack[path_st_p++] = tmp_node;
                if(k<=tmp_node->left->leaves) {
                    tmp_node = tmp_node->left;
                }
                else {
                    k-=tmp_node->left->leaves;
                    tmp_node = tmp_node->right;
                }
            }
            //Found the right kth leaf stored in tmp_node : perform insert
            
            //Forming the 2 nodes
            tree_node_t *old_leaf, *new_leaf;
            old_leaf = get_node();
            old_leaf->left = tmp_node->left;
            old_leaf->height = 0;
            old_leaf->leaves = 1;
            old_leaf->right = NULL;
            new_leaf = get_node();
            new_leaf->left = (tree_node_t *) new_obj;
            new_leaf->height = 0;
            new_leaf->leaves = 1;
            new_leaf->right = NULL;
            
            // inserting between [1,k]
            if(!last) {
                //Attaching the 2 nodes
                tmp_node->left = new_leaf;
                tmp_node->right = old_leaf;
            }
            // inserting at k+1
            else {
                //Attaching the 2 nodes
                tmp_node->left =  old_leaf;
                tmp_node->right = new_leaf;
            }
            
            //updating tmp
            tmp_node->height = 1;
            tmp_node->leaves = 2;
            
            
            //Rebalance && update leaf field as we travel path to the root
            while( path_st_p > 0) {
                int tmp_height, old_height;
                tmp_node = path_stack[--path_st_p];
                old_height= tmp_node->height;
                if( tmp_node->left->height - tmp_node->right->height == 2 ) {
                    
                    if(tmp_node->left->left->height - tmp_node->right->height == 1) {
                        right_rotation( tmp_node );
                        tmp_node->right->height = tmp_node->right->left->height + 1;
                        tmp_node->height = tmp_node->right->height + 1;
                        
                        //Update leaves after right rotation
                        tmp_node->right->leaves = tmp_node->right->left->leaves + tmp_node->right->right->leaves;
                        tmp_node->leaves=tmp_node->left->leaves+tmp_node->right->leaves;
                    }
                    
                    else {
                        left_rotation( tmp_node->left );
                        //Update leaves after left rotation
                        tmp_node->left->left->leaves = tmp_node->left->left->left->leaves + tmp_node->left->left->right->leaves;
                        
                        right_rotation(tmp_node);
                        //Update leaves after right rotation
                        tmp_node->right->leaves = tmp_node->right->left->leaves+ tmp_node->right->right->leaves;
                        tmp_node->leaves=tmp_node->left->leaves+tmp_node->right->leaves;
                        
                        tmp_height = tmp_node->left->left->height;
                        tmp_node->left->height  = tmp_height + 1;
                        tmp_node->right->height = tmp_height + 1;
                        tmp_node->height = tmp_height + 2;
                    }
                }
                
                else if (tmp_node->left->height - tmp_node->right->height == -2) {
                    if(tmp_node->right->right->height - tmp_node->left->height == 1){
                        left_rotation( tmp_node );
                        tmp_node->left->height = tmp_node->left->right->height + 1;
                        tmp_node->height = tmp_node->left->height + 1;
                        
                        //Update leaves after left rotation
                        tmp_node->left->leaves = tmp_node->left->left->leaves+ tmp_node->left->right->leaves;
                        tmp_node->leaves=tmp_node->left->leaves+tmp_node->right->leaves;
                    }
                    
                    else {
                        
                        right_rotation( tmp_node->right );
                        //Update leaves after right rotation
                        tmp_node->right->right->leaves = tmp_node->right->right->left->leaves + tmp_node->right->right->right->leaves;
                        
                        left_rotation( tmp_node );
                        //Update leaves after left rotation
                        tmp_node->left->leaves = tmp_node->left->left->leaves+ tmp_node->left->right->leaves;
                        tmp_node->leaves=tmp_node->left->leaves+tmp_node->right->leaves;
                        
                        tmp_height = tmp_node->right->right->height;
                        tmp_node->left->height  = tmp_height + 1;
                        tmp_node->right->height = tmp_height + 1;
                        tmp_node->height = tmp_height + 2;
                    }
                }
                /* update height even if there was no rotation */
                else {
                    if(tmp_node->left->height > tmp_node->right->height) {
                        tmp_node->height = tmp_node->left->height + 1;
                    }
                    else {
                        tmp_node->height = tmp_node->right->height + 1;
                    }
                    // Update leaves due to added node
                    tmp_node->leaves = tmp_node->left->leaves+tmp_node->right->leaves;
                }
            }
        }
    }
}

object_t *delete_by_number(tree_node_t *tree,int k) {
    tree_node_t *tmp_node, *upper_node, *other_node;
    object_t *deleted_object;
    
    if(k<=0 || k>tree->leaves || tree->left == NULL) { //Possible invalid inputs
        return NULL;
    }
    else {
        //Only one node: delete it
        if(tree->right == NULL) {
            deleted_object = (object_t *) tree->left;
            tree->left = NULL;
            return (deleted_object);
        }
        else {
            tree_node_t * path_stack[100]; int path_st_p = 0;
            tmp_node = tree;
            //Find the riht kth leaf and keep track of path to the root
            while(tmp_node->right != NULL) {
                path_stack[path_st_p++] = tmp_node;
                upper_node = tmp_node;
                if(k<=tmp_node->left->leaves) {
                    tmp_node   = upper_node->left;
                    other_node = upper_node->right;
                }
                else {
                    k-=tmp_node->left->leaves;
                    tmp_node   = upper_node->right;
                    other_node = upper_node->left;
                }
            }
            //Acess to: kth leaf, its parent, and other child
            
            upper_node->left  = other_node->left;
            upper_node->right = other_node->right;
            upper_node->height = other_node->height;
            upper_node->leaves -= 1;
            deleted_object = (object_t *) tmp_node->left;
            return_node( tmp_node );
            return_node( other_node );
            
            
            // Rebalance tree
            
            path_st_p -= 1;
            while( path_st_p > 0)
            {  int tmp_height, old_height;
                tmp_node = path_stack[--path_st_p];
                old_height= tmp_node->height;
                if( tmp_node->left->height - tmp_node->right->height == 2 ) {
                    
                    if(tmp_node->left->left->height-tmp_node->right->height == 1) {
                        right_rotation( tmp_node );
                        tmp_node->right->height = tmp_node->right->left->height + 1;
                        tmp_node->height = tmp_node->right->height + 1;
                        
                        //Update leaves after right rotation
                        tmp_node->right->leaves = tmp_node->right->left->leaves + tmp_node->right->right->leaves;
                        tmp_node->leaves=tmp_node->left->leaves+tmp_node->right->leaves;
                    }
                    
                    else {
                        left_rotation( tmp_node->left );
                        //Update leaves after left rotation
                        tmp_node->left->left->leaves = tmp_node->left->left->left->leaves + tmp_node->left->left->right->leaves;
                        
                        right_rotation( tmp_node );
                        //Update leaves after right rotation
                        tmp_node->right->leaves = tmp_node->right->left->leaves+ tmp_node->right->right->leaves;
                        tmp_node->leaves=tmp_node->left->leaves+tmp_node->right->leaves;
                        
                        tmp_height = tmp_node->left->left->height;
                        tmp_node->left->height  = tmp_height + 1;
                        tmp_node->right->height = tmp_height + 1;
                        tmp_node->height = tmp_height + 2;
                        
                    }
                }
                else if ( tmp_node->left->height - tmp_node->right->height == -2 ) {
                    
                    if(tmp_node->right->right->height - tmp_node->left->height == 1) {
                        left_rotation( tmp_node );
                        tmp_node->left->height = tmp_node->left->right->height + 1;
                        tmp_node->height = tmp_node->left->height + 1;
                        
                        //Update leaves after left rotation
                        tmp_node->left->leaves = tmp_node->left->left->leaves+ tmp_node->left->right->leaves;
                        tmp_node->leaves=tmp_node->left->leaves+tmp_node->right->leaves;
                    }
                    
                    else {
                        right_rotation( tmp_node->right );
                        //Update leaves after right rotation
                        tmp_node->right->right->leaves = tmp_node->right->right->left->leaves + tmp_node->right->right->right->leaves;
                        
                        left_rotation( tmp_node );
                        //Update leaves after left rotation
                        tmp_node->left->leaves = tmp_node->left->left->leaves+ tmp_node->left->right->leaves;
                        tmp_node->leaves=tmp_node->left->leaves+tmp_node->right->leaves;
                        
                        tmp_height = tmp_node->right->right->height;
                        tmp_node->left->height  = tmp_height + 1;
                        tmp_node->right->height = tmp_height + 1;
                        tmp_node->height = tmp_height + 2;
                    }
                }
                /* update height even if there was no rotation */
                else {
                    
                    if(tmp_node->left->height > tmp_node->right->height) {
                        tmp_node->height = tmp_node->left->height + 1;
                    }
                    else {
                        tmp_node->height = tmp_node->right->height + 1;
                    }
                    // Update leaves due to deleted node
                    tmp_node->leaves = tmp_node->left->leaves+tmp_node->right->leaves;
                }
                
            } //End of while loop
            return(deleted_object);
        }
    }
}




int main()
{  tree_node_t *st1, *st2;
    int i, k;
    int o[4] = {0,2,4,6};
    st1 = create_tree();
    st2 = create_tree();
    printf("Made two Trees\n");
    for( i=0; i < 100000; i++)
    {  insert_by_number( st1, 1, &(o[1]) );
        insert_by_number( st2, 1, &(o[2]) );
    }
    /*inserted 100000 pointers each, to 2 in st1 and to 4 in st2 */
    printf("Passed 200000 inserts.\n"); fflush(stdout);
    for( i=100000; i >0 ; i--)
    {  insert_by_number( st1, i+1, &(o[3]) );
        insert_by_number( st1, i+1, &(o[2]) );
    }
    /* now st1 is a sequence 246246...246 of length 300000 */
    for( i=0; i < 100000; i++)
    {  insert_by_number( st2, 1, &(o[1]) );
    }
    /* now st2 is a sequence of 100000 2s, followed by 1000000 4s */
    for( i=0; i < 300000; i++)
    {  k = *find_by_number(st1, i+1 );
        if( 2*((i%3)+1) != k )
        {  printf("Found wrong number %d (should be %d) in tree st1 at line %d\n",
                  k, 2*((i%3)+1), i+1); fflush(stdout); exit(0);
        }
    }
    for( i=300000; i >0; i--)
    {  k = *find_by_number(st1, i );
        if( k==6 )
            delete_by_number(st1, i );
    }
    /* now st1 is sequence 24242424... of length 200000 */
    for( i=0; i < 200000; i++)
    {  k = *find_by_number(st1, i+1 );
        if( 2*((i%2)+1) != k )
        {  printf("Found wrong number %d (should be %d) in tree st1 at line %d\n",
                  k, 2*((i%2)+1), i+1); fflush(stdout); exit(0);
        }
    }
    printf("Passed Test\n"); fflush(stdout);
}
