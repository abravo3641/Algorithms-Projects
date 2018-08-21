//Name: Anthony Bravo
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>


using namespace std;

struct edgelist {
    int u; //Vetex u
    int v; //Vertex v
    struct edgelist *next; // Edge connection
};

struct edgelist * Dijkstra(int n, int *graph, int start);

int main()
{
    long i,j,k,l, dist, length;
    int *graph;
    struct edgelist * result, *tmp;
    graph = (int *) malloc( 1000000*sizeof(int) );
    for(i=0; i<1000; i++)
        for(j=0; j<1000;j++)
            *(graph+1000*i+j) = 100;
    /* now create 30 by 30 grid */
    for(k=0; k<29; k++)
        for(l=0; l<30;l++)
        {  i = 30*k + l;
            j = 30*(k+1 ) + l;
            *(graph+1000*i+j) = 1;
            *(graph+1000*j+i) = 1;
        }
    for(k=0; k<30; k++)
        for(l=0; l<29;l++)
        {  i = 30*k + l;
            j = 30*k + l+1;
            *(graph+1000*i+j) = 1;
            *(graph+1000*j+i) = 1;
        }
    for(k=0; k<10; k++)
        for(l=0; l<10;l++)
        {  i = 900+10*k + l;
            j = 30*3*k + 3*l;
            *(graph+1000*i+j) = 21;
            *(graph+1000*j+i) = 21;
        }
    for(k=0; k<10; k++)
        for(l=0; l<9;l++)
        {  i = 900+10*k + l;
            j = 900+10*k + l +1;
            *(graph+1000*i+j) = 2;
            *(graph+1000*j+i) = 2;
        }
    for(k=0; k<9; k++)
        for(l=0; l<10;l++)
        {  i = 900+10*k + l;
            j = 900+10*(k+1) + l;
            *(graph+1000*i+j) = 2;
            *(graph+1000*j+i) = 2;
        }
    result = Dijkstra(1000, graph, 31);
    dist = 0;
    if( result-> v != 31 && result-> u != 31 )
    {  printf("Wrong startvertex; should be 31, but first edges goes from %d to %d.\n", result->v, result->u ); fflush(stdout);
        exit(-1);
    }
    tmp = result;
    printf("Path is: ");
    while( tmp != NULL )
    {  printf("%d-%d, ", tmp->u, tmp->v);
        tmp = tmp-> next;
    }
    printf("\n"); fflush(stdout);
    dist = *(graph + 1000*(result->v) + (result->u));
    while( result->next != NULL)
    {  result = result-> next;
        length = *(graph + 1000*(result->v) + (result->u));
        if( length >= 100 )
        {  printf("Path uses non-edge: no edge between %d and %d\n",
                  result->u, result->v ); fflush(stdout);
            exit(-1);
        }
        dist += length;
    }
    if( dist != 57 )
    {  printf("Length of path is %d, should be 57.\n",dist); fflush(stdout);
        exit(-1);
    }
    printf("Passed Test\n"); fflush(stdout);
}


struct edgelist * Dijkstra(int n, int *graph, int start) {
    int distance[n];
    bool visited[n]; //Keeps track of which vertex we have visited
    int previus_v[n]; //Keeps track for each vertex the last vertex for whom it was visited by
    int infinity = -1;
    
    // Filling our shortest distance array with infinity except start & making all vertex unvisited
    for(int i=0;i<n;i++) {
        distance[i] = infinity;
        visited[i] = false;
    }
    distance[start] = 0;
    
    for(int size=1; size<=n; size++) {
        //Choose the smallest distance vertex from start that has not been visited
        
        int closest_v;
        //Finding right element to compare all elements to
        for(int k=0; k<n; k++) {
            if(visited[k] == false && distance[k] != infinity) {
                closest_v = k;
                break;
            }
        }
        for(int k=0;k<n;k++) {
            if((distance[k] < distance[closest_v] && visited[k] == false) && distance[k]!= -1)
                closest_v = k;
        }
        // We have know found the smallest distance unvisited vertex, now we will visit it and explore its neighboors

        visited[closest_v] = true; // Becomes part of visited vertex
        
        //Explore all the closest_v neighboors and update the distance array
        for(int v=0; v<n; v++) {
            int edge = graph[n*closest_v +v];
            // If not visited and its a valid edge, then update distance array if distance is lower
            if(visited[v] == false && edge < 100) {
                int total = distance[closest_v]+edge;
                //If we update the distance, then we update the previous vertex
                if(total<distance[v] || distance[v] == -1) {
                    distance[v] = total;
                    previus_v[v] = closest_v;
                }
            }
        }

    }
    previus_v[start] = -1; //This is where we want to end up
    
    //Look for the vertex # with the largest distance
    int largest_v_d = 0;
    for(int i=0;i<n;i++) {
        if(distance[i]>distance[largest_v_d])
            largest_v_d = i;
    }
    
    int j = largest_v_d;
    int stack[2*n];
    int k=0; //index for stack
    
    // loop until we've reached the starting vertex
    while(previus_v[j] != -1) {
        //Place the path from largest distant vertex to start vertex on a stack to get the right order
        stack[k++] = j;
        stack[k++] = previus_v[j];
        j = previus_v[j];
    }
    
    edgelist * path = NULL;
    edgelist * tail = NULL;
    edgelist * temp = NULL;


    int vertex_u; int vertex_v;
    //Loop until stack is not empty and make a new node for each 2 pops
    while(k>=1) {
        vertex_u = stack[--k];
        vertex_v = stack[--k];
        temp = new edgelist();
        temp->u= vertex_u;
        temp->v = vertex_v;
        temp->next = NULL;
        //If its our first insert
        if(path==NULL) {
            path = temp;
            tail = path;
        }
        //All other inserts are at the end
        else {
            tail->next = temp;
            tail = temp;
        }
    }
    return path;
}







