#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>

int main() 
{ 
	printf("*ppid : parent pid (this program will take some time to print)*\n");
    int child1, child2, grandchild1, grandchild2, grandchild3, grandchild4 ; 
    child1 = fork(); 
  	
    if (child1 == 0) { 
  
  		printf("child 1 : pid = %d and ppid = %d\n", getpid(), getppid()); 
        sleep(10);
        // exit(7);
        
        grandchild1 = fork();
        if(grandchild1 == 0)
        {
        	printf("grandchild 1 : pid = %d and ppid = %d\n", getpid(), getppid());
        	sleep(2);
        	// exit(5);
        	 
        }
        else
        {
        	sleep(3);
        	grandchild2 = fork();
        	
        	if(grandchild2 == 0)
        	{
        		printf("grandchild 2 : pid = %d and ppid = %d\n", getpid(), getppid()); 
        
        	}
        }
    } 
  
    else {
    	sleep(1); 
        child2 = fork();

	        if (child2 == 0) 
	        { 
	  			printf("child 2 : pid = %d and ppid = %d\n", getpid(), getppid()); 
		        sleep(11);
		        
		        grandchild3 = fork();
		        if(grandchild3 == 0)
		        {
		        	sleep(2);
		        	printf("grandchild 3 : pid = %d and ppid = %d\n", getpid(), getppid()); 
		        	sleep(2);
		        	
		        }

		        else
		        {
		        	sleep(4);
		        	grandchild4 = fork();
		        	
		        	if(grandchild4 == 0)
		        	{
		        		printf("grandchild 4 : pid = %d and ppid = %d\n", getpid(), getppid()); 
		        
		        	}
		        }
		    }  
	  
	        else 
	        { 
	           
	                sleep(17); 
	                printf("parent --> pid = %d\n", getpid()); 
	        } 
        }  
  
    return 0; 
} 