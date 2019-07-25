#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

char *ip_to_host(const char *IP)
{
	static char host[128] = "";

	struct sockaddr_in s; 
	s.sin_family		= AF_INET; 
	s.sin_addr.s_addr	= inet_addr(IP); 
	s.sin_port		= 0; 

	getnameinfo( (struct sockaddr *)&s, sizeof(s), host, 128, NULL, 0, 0); 

	return host;
}

int main()
{
	char *IP;
	printf("Enter the IP Address : ");
	scanf("%s",IP);
	// printf("%s\n",IP );
	printf("\n");
	char *Name = ip_to_host(IP) ;
	printf("The Host Name is : %s\n",Name );

}
