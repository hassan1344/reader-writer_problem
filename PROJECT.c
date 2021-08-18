#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>



struct read{
	int id;              //sem_post (signals)
    int time;
};

sem_t wrt;           //return 0 on success / -1 on failure   / semaphore for readcnt safety
sem_t mutex;   //return 0 on succes / -1 on failure        /semaphore for writer entry
int readercnt = 0;
int time_taken;

FILE *fptr;
void *writer(void *a);
void *reader(void *a);


void *reader(void *param){

fptr = fopen("criticalsection.txt", "r"); //just for purpose of reading
if(fptr == NULL){
printf("File not found!");
exit(1);
}
	
struct read *p = (struct read*)param;	
time_t start_t, end_t;
double diff_t;
int randomtimespent = (rand()%4)+1;
printf("Reader Thread Request. Thread ID : %d\n\n", p->id);

sem_wait(&mutex);
readercnt++;

if(readercnt == 1){
	sem_wait(&wrt);
}	

sem_post(&mutex);

//reading section (CS)
time(&start_t);
struct tm *timinginfo = localtime(&start_t);
printf("Reader Entry . Thread ID :%d at %02d:%02d\n\n",p->id,timinginfo->tm_min,timinginfo->tm_sec);

sleep(randomtimespent);
//CS end

sem_wait(&mutex);
readercnt--;

if(readercnt == 0)
	sem_post(&wrt);

time(&end_t);
timinginfo = localtime(&end_t);
diff_t = difftime(end_t,start_t);
printf("Exit by Reader . Thread ID : %d at %02d:%02d\n\n", p->id,timinginfo->tm_min,timinginfo->tm_sec);
printf("Reader . Thread ID : %d spends %.2f secs in Critical Section\n", p->id,diff_t);


sem_post(&mutex);


}


void *writer(void *param){

fptr = fopen("criticalsection.txt", "a");
int id = (intptr_t)param;
time_t start_t , end_t;
double diff_t;
printf("Writer Request. Thread ID : %d\n\n", id);	
sem_wait(&wrt);   //wait writer
int randomtimespent = (rand()%4)+1;
time (&start_t);
struct tm *timinginfo = localtime (&start_t);
//CS
printf("Entry by Writer. Thread ID :  %d at %02d:%02d\n\n", id, timinginfo->tm_min, timinginfo->tm_sec);

fprintf(fptr,  "Entry by Writer. Thread ID : %d at %02d:%02d\n\n", id, timinginfo->tm_min, timinginfo->tm_sec);

sleep(randomtimespent);

time(&end_t);
timinginfo = localtime(&end_t);
diff_t = difftime(end_t,start_t);
printf ("Exit by Writer . Thread ID : %d at %02d:%02d\n\n", id,timinginfo->tm_min, timinginfo->tm_sec);

fprintf (fptr,"Exit by Writer . Thread ID : %d at %02d:%02d\n\n", id,timinginfo->tm_min, timinginfo->tm_sec);
//CS END
printf("Writer . Thread ID : %d spends %.2f secs in Critical Section\n", id,diff_t);
sem_post(&wrt);	

//fclose(fptr);	
}


int main(){

srand(time(0));	
int r ,w ;
scanf("%d" ,&r); scanf("%d", &w);
pthread_t readers[r], writers[w];
sem_init(&mutex,0,1); //initializes unlocked mutex (for readers)
sem_init(&wrt,0,1); //initializes an unnamed semaphore   (for writers)

struct read read_ids[r];   //for reader id's


for(int i=0;i<r;i++)
{
read_ids[i].id = i+1; 
pthread_create(&readers[i],NULL, (void*) reader,(void*)(intptr_t)&read_ids[i]);

}
for(int i=0;i<w;i++)
pthread_create(&writers[i],NULL, (void*) writer,(void*)(intptr_t)(i+1));

for(int i = 0; i < r; i++) 
pthread_join(readers[i], NULL);
    
for(int i = 0; i < w; i++) 
pthread_join(writers[i], NULL);


sem_destroy(&mutex); //destroying reader locks
sem_destroy(&wrt);        //destroying writer semaphores

fclose(fptr);    
	
}
