#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
//#include <linux/kernel.h>


int main(){

int a = syscall(334);


printf("\n..VALUE : %d\n",a);
return 0;


}
