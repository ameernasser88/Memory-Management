#include <stdio.h>
#include <stdlib.h>
#include <string.h>

////////--GLOBAL VARIABLES--////////////

// 1st user input (size of buffer)
int number_of_pages=0;            ////
//algorithm name
char algorithm[32];             ////
// pages stream
int  pages_references[1024];  ////
//number of pages stream
int number_of_references;   ////

////////////////////////////// 

// struct similar to map to hold page number and its position relative to the current page entering the buffer
// used in OPTIMAL and LRU
typedef struct map
{
    int page;
    int position;
}map;

/////////////////////////////////////////////////--QUEUE (Buffer)--////////////////////////////////////////////////////////


struct Queue 
{ 
    int front, rear, size; 
    unsigned capacity; 
    int* array; 
}; 

struct Queue* createQueue(unsigned capacity) 
{ 
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue)); 
    queue->capacity = capacity; 
    queue->front = queue->size = 0;  
    queue->rear = capacity - 1; 
    queue->array = (int*) malloc(queue->capacity * sizeof(int)); 
    return queue; 
} 
 
int isFull(struct Queue* queue) 
{  return (queue->size == queue->capacity);  } 

int isEmpty(struct Queue* queue) 
{  return (queue->size == 0); } 

void enqueue(struct Queue* queue, int item) 
{ 
    if (isFull(queue)) 
        return; 
    queue->rear = (queue->rear + 1)%queue->capacity; 
    queue->array[queue->rear] = item; 
    queue->size = queue->size + 1; 
} 

int dequeue(struct Queue* queue) 
{ 
    if (isEmpty(queue)) 
        return 0; 
    int item = queue->array[queue->front]; 
    queue->front = (queue->front + 1)%queue->capacity; 
    queue->size = queue->size - 1; 
    return item; 
} 



/////////////////////// 2 functions used by all 4 policies

// 1- check whether a page exists in buffer or not
int contains(struct Queue* queue , int value) 
{ 
    if (isEmpty(queue)) 
        return 0; 
    else
    {   int i;
        for(i=0;i<queue->capacity ;i++)
        {
            if(queue->array[i]==value)
            {
            return 1;
            }
            
        }
    }
    return 0;
    
} 

// 2- print current contents of buffer in a specific format
void print_contents(struct Queue* queue)
{
    int i;
     for(i=0;i<queue->size ;i++)
        {
            printf("%02d ",queue->array[i]);
        }
     
}
//////////////////////////////////////////////////--ALGORITHMS--//////////////////////////////////////////////////

////////////////////////--FIFO--////////////////////////

void FIFO()
{
    //buffer
    struct Queue* queue = createQueue(number_of_pages);
    int i;
    //counter to keep track of faults
    int total_faults=0;

    printf("Replacement Policy = FIFO");
    printf("\n"); 
    printf("-------------------------------------");
    printf("\n"); 
    printf("Page   Content of Frames");
    printf("\n"); 
    printf("----   -----------------");
    printf("\n"); 

     //looping over the page stream   
     for ( i = 0; i < number_of_references; i++)
    {
        //if the buffer is not full and doesnt contain the requesting page , enqueue that page into the buffer
        if(!isFull(queue)&&!contains(queue,pages_references[i]))
        {
            enqueue(queue, pages_references[i]);
            printf("%02d     ",pages_references[i]);
            print_contents(queue);
            printf("\n");   
        }
        // if the buffer is full or the requesting page already exists in buffer
        else
        {
            // if already exists , do nothing
            if(contains(queue,pages_references[i]))
            {
                printf("%02d     ",pages_references[i]);
                print_contents(queue);
                printf("\n");   
            }
            // buffer is full and the requesting page doesnt exist in buffer, dequeue the first item entered the buffer and enqueue the requesting page
            // and increment the number of faults
            else
            {   //////// FIFO LOGIC
                dequeue(queue);
                enqueue(queue, pages_references[i]);
                total_faults++;
                printf("%02d F   ",pages_references[i]);
                print_contents(queue);
                printf("\n");   

            }
        }
    }
    printf("-------------------------------------");
    printf("\n"); 
    printf("Number of page faults = %d",total_faults);
    printf("\n");
    
}


////////////////////////--LRU--////////////////////////

void LRU()
{
    //buffer
    struct Queue* queue = createQueue(number_of_pages);
    int i;
    //counter to keep track of faults
    int total_faults=0;

    printf("Replacement Policy = LRU");
    printf("\n"); 
    printf("-------------------------------------");
    printf("\n"); 
    printf("Page   Content of Frames");
    printf("\n"); 
    printf("----   -----------------");
    printf("\n"); 
    
     //looping over the page stream
    for ( i = 0; i < number_of_references; i++)
    {
        //if the buffer is not full and doesnt contain the requesting page , enqueue that page into the buffer
        if(!isFull(queue)&&!contains(queue,pages_references[i]))
        {
            
            enqueue(queue, pages_references[i]);
            printf("%02d     ",pages_references[i]);
            print_contents(queue);
            printf("\n");   

        }
        // if the buffer is full or the requesting page already exists in buffer
        else
        {
            // if already exists , do nothing
            if(contains(queue,pages_references[i]))
            {
                printf("%02d     ",pages_references[i]);
                print_contents(queue);
                printf("\n");   
            }
            else
            //buffer is full and the requesting page doesnt exist in buffer
            { ////////////// LRU LOGIC
             ///////////--get lru

             //make a map array and loop over all the previous streams , save each page stream with its position 
             //then compare them and get the map with minimum position 
             //map with minimum position has the lru page
             //replace the lru page with the requesting page   
             map lru_map[number_of_pages];
             
             for(int k =0 ; k <queue->size ; k++)
             {
                for(int j=i-1;j>0;j--)
                {
                    if(queue->array[k]==pages_references[j])
                    {
                        lru_map[k].page=queue->array[k];
                        lru_map[k].position =j;
                        break;
                    }
                }     
             }
             map min = lru_map[0];
             for(int k=0; k <number_of_pages ;k++)
             {
                if(lru_map[k].position<min.position)
                min=lru_map[k];
             }

            for(int k =0 ; k < queue->size ; k++)
             {
              if(queue->array[k]==min.page){
              queue->array[k]=pages_references[i];
              }
             }  
                
                total_faults++;
                printf("%02d F   ",pages_references[i]);
                print_contents(queue);
                printf("\n");   
            }
            
            
        }
        

    }    
    printf("-------------------------------------");
    printf("\n"); 
    printf("Number of page faults = %d",total_faults);
    printf("\n");

}


////////////////////////--OPTIMAL--////////////////////////

void OPTIMAL()
{   
    //buffer
    struct Queue* queue = createQueue(number_of_pages);
    int i;
    //counter to keep track of faults
    int total_faults=0;

    printf("Replacement Policy = OPTIMAL");
    printf("\n"); 
    printf("-------------------------------------");
    printf("\n"); 
    printf("Page   Content of Frames");
    printf("\n"); 
    printf("----   -----------------");
    printf("\n"); 

    //looping over the page stream
    for ( i = 0; i < number_of_references; i++)
    {
        //if the buffer is not full and doesnt contain the requesting page , enqueue that page into the buffer
        if(!isFull(queue)&&!contains(queue,pages_references[i]))
        {
            
            enqueue(queue, pages_references[i]);
            printf("%02d     ",pages_references[i]);
            print_contents(queue);
            printf("\n");   

        }
        // if the buffer is full or the requesting page already exists in buffer
        else
        {   // if already exists , do nothing
            if(contains(queue,pages_references[i]))
            {
                printf("%02d     ",pages_references[i]);
                print_contents(queue);
                printf("\n");   
            }
            else
            { /////////////// OPTIMAL LOGIC
              //make a map array and loop over all the next streams , save each page stream with its position 
              //then compare them and get the map with maximumm position 
              //map with maximum position has the optimal page  
              //replace the page with maximum position (longest next reference time) with the requesting page
              //if a page doesnt exist in the next refereneces then it will have the maximum position and it is the one to be replaced

             map optimal_map[number_of_pages];
             
             for(int k =0 ; k <queue->size ; k++)
             {
                 //if it doesn't exist next
                 optimal_map[k].position=number_of_references+1;
                 optimal_map[k].page=queue->array[k];

                for(int j=i+1;j<number_of_references;j++)
                {
                    if(queue->array[k]==pages_references[j])
                    {
                        optimal_map[k].page=queue->array[k];
                        optimal_map[k].position =j;
                        break;
                    }
                } 

             }
             map max = optimal_map[0];
             for(int k=0; k <number_of_pages ;k++)
             {
                if(optimal_map[k].position>max.position)
                max=optimal_map[k];
             }


            for(int k =0 ; k < queue->size ; k++)
             {
                  if(queue->array[k]==max.page)
                  {
                    queue->array[k]=pages_references[i];
                  }
             }  
                
                total_faults++;
                printf("%02d F   ",pages_references[i]);
                print_contents(queue);
                printf("\n");   
            }
            
            
        }
        

    }    
    printf("-------------------------------------");
    printf("\n"); 
    printf("Number of page faults = %d",total_faults);
    printf("\n");

}


////////////////////////--CLOCK--////////////////////////

void CLOCK()
{
    //buffer
    struct Queue* queue = createQueue(number_of_pages);
    int i;
    //counter to keep track of faults
    int total_faults=0;

    printf("Replacement Policy = CLOCK");
    printf("\n"); 
    printf("-------------------------------------");
    printf("\n"); 
    printf("Page   Content of Frames");
    printf("\n"); 
    printf("----   -----------------");
    printf("\n"); 
    
    //represents a use bit for each buffer element
    //initially set to 0
    int use_bits[number_of_pages];
    
                for( int j=0 ; j<number_of_pages ; j++)
                {
                    use_bits[j]=1;
                }
    //looping over the page stream
    for ( i = 0; i < number_of_references; i++)
    {
        //if the buffer is not full and doesnt contain the requesting page , enqueue that page into the buffer
        if(!isFull(queue)&&!contains(queue,pages_references[i]))
        {
            
            enqueue(queue, pages_references[i]);
            printf("%02d     ",pages_references[i]);
            print_contents(queue);
            printf("\n");  
            use_bits[i%number_of_pages]=1; 
           

        }
        // if the buffer is full or the requesting page already exists in buffer
        else
        {
            // if already exists , do nothing
            if(contains(queue,pages_references[i]))
            {
                printf("%02d     ",pages_references[i]);  
                print_contents(queue);
                printf("\n");   
                
                for( int j=0 ; j<number_of_pages; j++)
                {
                    if(pages_references[i]==queue->array[j])
                        use_bits[j]=1;
                }
            }
            else
            {
                //////CLOCK LOGIC
                int j;

                //flag to determine if all elements in buffer have use bits of 1    
                int all_one=1;
                for( j=0 ; j<number_of_pages; j++)
                {
                    if(use_bits[j]==0)
                    {
                        all_one=0;
                    }
                }

                // if flag is set replace first element
                 if (all_one==1)
                {
                    queue->array[0]= pages_references[i];
                    use_bits[0]=1;
                    for( j=1 ; j<number_of_pages; j++)
                    {
                        use_bits[j]=0;
                    }
                }
                //else replace element with first 0 use bit
                else
                 {
                    for( j=0 ; j<number_of_pages ; j++)
                    {
                        if(use_bits[j]==0)
                        {
                            queue->array[j]=pages_references[i];
                            use_bits[j]=1;
                            break;
                        }
                    }
                 }

                total_faults++;
                printf("%02d F   ",pages_references[i]);
                print_contents(queue);
                printf("\n");   
            }
            
            
        }
        

    }    
    printf("-------------------------------------");
    printf("\n"); 
    printf("Number of page faults = %d",total_faults);
    printf("\n");


}




int main()
{
    
    scanf("%d",&number_of_pages);
    scanf("%s",algorithm);

    int x=0;
    int i=0;

    while(x!=-1)
    {
        scanf("%d",&x);
     
        if(x!=-1)
        {
            pages_references[i]=x;
            i++;
        }
    }

    number_of_references = i;

    if(strcmp(algorithm,"FIFO")==0)
    {
        FIFO();
    }
    else if(strcmp(algorithm,"LRU")==0)
    {
        LRU();
    }
    else if(strcmp(algorithm,"OPTIMAL")==0)
    {
        OPTIMAL();
    }
    else if(strcmp(algorithm,"CLOCK")==0)
    {
        CLOCK();
    }

    return 0;
}


