#include <linux/init.h>
#include <linux/semaphore.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/random.h>
#include <asm/delay.h>
#include <linux/delay.h>

#define r 2
#define w 2

struct read{
	int id;
	int time;
};
static struct semaphore wrt;
static struct semaphore mutex;

int readercnt=0;
int time_taken;
int i,j,k,l;

int *writer(void *a);
int *reader(void *a);

int *reader(void *param){
	
struct read *p = (struct read*)param;	
struct timespec start_t, end_t;
long int diff_t;

int randomtimespent;
randomtimespent = 0;
get_random_bytes(&randomtimespent, sizeof(randomtimespent));
if(randomtimespent < 0){
randomtimespent = -1 * randomtimespent;
randomtimespent = (randomtimespent % 4) +1;
randomtimespent = randomtimespent * 1000;
}

printk("Reader Thread Request. Thread ID : %d\n\n", p->id);

down(&mutex);
readercnt++;

if(readercnt == 1){
	down(&wrt);
}	

up(&mutex);


getnstimeofday(&start_t);


printk("Reader Entry . Thread ID :%d at %.2lu:%.2lu\n\n",p->id,(start_t.tv_sec/60)%60, start_t.tv_sec%60);

msleep_interruptible(randomtimespent);


down(&mutex);
readercnt--;

if(readercnt == 0)
	up(&wrt);

getnstimeofday(&end_t);


diff_t = end_t.tv_sec - start_t.tv_sec;
printk("Exit by Reader . Thread ID : %d at %.2lu:%.2lu\n\n", p->id,(end_t.tv_sec/60)%60, end_t.tv_sec%60);
printk("Reader . Thread ID : %d spends %ld secs in Critical Section\n", p->id,diff_t);


up(&mutex);
return 0;
}

int *writer(void *param){

int id = *(int*)param;
struct timespec start_t , end_t;
long int diff_t;
printk("Writer Request. Thread ID : %d\n\n", id);	
down(&wrt);   

int randomtimespent;
randomtimespent =0 ;
get_random_bytes(&randomtimespent, sizeof(randomtimespent));
if(randomtimespent < 0){
randomtimespent = -1 * randomtimespent;
randomtimespent = (randomtimespent % 4) +1;
randomtimespent = randomtimespent * 1000;
}

getnstimeofday(&start_t);



printk("Entry by Writer. Thread ID :  %d at %.2lu:%.2lu\n\n", id,(start_t.tv_sec/60)%60, start_t.tv_sec%60);


msleep_interruptible(randomtimespent);

getnstimeofday(&end_t);

diff_t = end_t.tv_sec - start_t.tv_sec;
printk ("Exit by Writer . Thread ID : %d at %.2lu:%.2lu\n\n", id,(end_t.tv_sec/60)%60, end_t.tv_sec%60);

printk("Writer . Thread ID : %d spends %ld secs in Critical Section\n", id,diff_t);

up(&wrt);	

return 0;

}


asmlinkage long sys_mainfunc(void){

static struct task_struct *readers[r], *writers[w];

sema_init(&mutex,1); 
sema_init(&wrt,1); 

struct read read_ids[r];   //for reader id's
int writerids[w];

for(i=0;i<r;i++)
{
read_ids[i].id = i+1; 
readers[i] = kthread_create((void*)reader,&read_ids[i],"reader");
if(readers[i])
wake_up_process(readers[i]);
}

for(j=0;j<w;j++){
writerids[j] = j + 1;
writers[j] = kthread_create((void*)writer,&writerids[j], "writer");
if(writers[j])
wake_up_process(writers[j]);
}

for(k = 0; k < r; k++) 
kthread_stop(readers[k]);
    
for(l= 0; l < w; l++) 
kthread_stop(writers[l]);

return 0;


}







