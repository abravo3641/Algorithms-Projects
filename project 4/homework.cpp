// Name: Anthony Bravo

// Comment: Needed the ulimit -s unlimited command to compile it on my computer

#include<iostream>
#include<cstdlib>

using namespace std;

struct edgelist {
    int u; //Vetex u
    int v; //Vertex v
    int r; //Residual capacity of the edge
    struct edgelist *next; // Edge connection
};

void maximum_flow(int n, int s, int t, int *capacity, int *flow);
bool bfs(int *residual, int s, int t, int n, int *previus_v);  //Helper function


int main(void)
{  int cap[1000][1000], flow[1000][1000];
    int i,j, flowsum;
    for(i=0; i< 1000; i++)
        for( j =0; j< 1000; j++ )
            cap[i][j] = 0;
    
    for(i=0; i<499; i++)
        for( j=i+1; j<500; j++)
            cap[i][j] = 2;
    for(i=1; i<500; i++)
        cap[i][500 + (i/2)] =4;
    for(i=500; i < 750; i++ )
    { cap[i][i-250]=3;
        cap[i][750] = 1;
        cap[i][751] = 1;
        cap[i][752] = 5;
    }
    cap[751][753] = 5;
    cap[752][753] = 5;
    cap[753][750] = 20;
    for( i=754; i< 999; i++)
    {  cap[753][i]=1;
        cap[i][500]=3;
        cap[i][498]=5;
        cap[i][1] = 100;
    }
    cap[900][999] = 1;
    cap[910][999] = 1;
    cap[920][999] = 1;
    cap[930][999] = 1;
    cap[940][999] = 1;
    cap[950][999] = 1;
    cap[960][999] = 1;
    cap[970][999] = 1;
    cap[980][999] = 1;
    cap[990][999] = 1;
    printf("prepared capacity matrix, now executing maxflow code\n");
    maximum_flow(1000,0,999,&(cap[0][0]),&(flow[0][0]));
    for(i=0; i<=999; i++)
        for(j=0; j<=999; j++)
        {  if( flow[i][j] > cap[i][j] )
        {  printf("Capacity violated\n"); exit(0);}
        }
    flowsum = 0;
    for(i=0; i<=999; i++)
        flowsum += flow[0][i];
    printf("Outflow of  0 is %d, should be 10\n", flowsum);
    flowsum = 0;
    for(i=0; i<=999; i++)
        flowsum += flow[i][999];
    printf("Inflow of 999 is %d, should be 10\n", flowsum);
    
    printf("End Test\n");
}




void maximum_flow(int n, int s, int t, int *capacity, int *flow) {
    
    int residual[n][n];
    bool backward_edges[n][n];
    
    //Initial a flow of 0 for all edges
    for(int i=0;i<n;i++) {
        for (int j=0;j<n;j++)
            flow[n*i +j] = 0;
    }
    
    //Make Residual graph equal to capacity
    for(int i=0;i<n;i++) {
        for (int j=0;j<n;j++)
            residual[i][j]= capacity[n*i +j];
    }
    
    // Keep track of backward edges
    for(int i=0;i<n;i++) {
        for (int j=0;j<n;j++)
            if(capacity[n*i +j] != 0) {
                backward_edges[i][j] = false;
            }
            else {
                backward_edges[i][j] = true;
            }
    }
    
    //Compute augmented path on Residual graph
    int previus_v[n]; // Keeps track of the parents during the path
    
    bool path = bfs((int *) residual,s,t,n,previus_v);
    
    
    //Run while theres an augmented path from s to t
    while(path) {
        
        //linked list representation for augmented path
        int j = t;
        int stack[2*n];
        int k=0;
        while(previus_v[j] != -1) {
            //Place the path from target vertex to start vertex on a stack to get the right order
            stack[k++] = j;
            stack[k++] = previus_v[j];
            j = previus_v[j];
        }
        edgelist * head = NULL;
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
            temp->r = residual[temp->u][temp->v];
            temp->next = NULL;
            //If its our first insert
            if(head==NULL) {
                head = temp;
                tail = head;
            }
            //All other inserts are at the end
            else {
                tail->next = temp;
                tail = temp;
            }
        }
        
        
        //Finding Smallest residual capacity along augmented path
        temp = head;
        int residual_c = temp->r;
        while(temp!=NULL) {
            if(temp->r < residual_c)
                residual_c = temp->r;
            temp = temp->next;
        }
        
        
        //Update the flow graph by pushing residual capacity
        temp = head;
        while(temp!= NULL) {
            int u = temp->u;
            int v = temp->v;
            
            //Push normaly if traveling in regular edge
            if(backward_edges[u][v]==false)
                flow[n*u +v] += residual_c;
            //Subtact when traveling on backward edge
            else
                flow[n*v +u] -= residual_c;
            
            temp = temp->next;
        }
        
        //Update the residual graph
        temp = head;
        while(temp!= NULL) {
            residual[temp->u][temp->v] -= residual_c;
            residual[temp->v][temp->u] += residual_c;
            temp = temp->next;
        }
        
        //Find if possible augmented path on updated residual graph
        path = bfs((int *) residual,s,t,n,previus_v);
    }
}

bool bfs(int *residual, int s, int t, int n, int *previus_v) {
    
    bool visited[n];
    // Creating array based queue
    int queue[n];
    int start,end;
    
    //Mark all vertex as unvisited
    for(int i=0;i<n;i++)
        visited[i] = false;
    
    //Set all the path to unreachable = -1
    for(int i=0;i<n;i++)
        previus_v[i] = -1;
    
    //Visit start vertex, mark it as visited and placed on queue
    visited[s] = true;
    queue[0] = s;
    start = 0;
    end = 0;
    
    //Run until we've reached target vertex & stack is not empty
    bool done = false;
    while(!done && start <= end) {
        
        //Store and pop next queue element & check if its targert
        int i = queue[start++];
        if(i==t)
            done = true;
        
        //Explore non-visited neighboors(j) of the poopep element
        for(int j=0;j<n;j++)
            if(residual[n*i +j] > 0 && visited[j] == false) {
                queue[++end] = j;
                visited[j] = true;
                previus_v[j] = i;
            }
    }
    return visited[t];
}
