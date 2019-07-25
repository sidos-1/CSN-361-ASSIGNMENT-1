#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<netdb.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h> 
#include<string.h> 
#include<netinet/ip_icmp.h> 
#include<time.h> 
#include<fcntl.h> 
#include<signal.h> 


#define SIZE 64  
#define PORT 0  
#define TIMELIMIT 1  


int ping=1; 


struct data_packet 
{ 
    struct icmphdr hdr; 
    char msg[SIZE-sizeof(struct icmphdr)]; 
}; 

void interrupt_func(int n) 
{ 
    ping=0; 
} 

unsigned short checksum(void *data, int len) 
{    
	unsigned short *p = (unsigned short *)data; 
    unsigned int sum=0; 
    unsigned short result; 
  
    for(sum=0;len>1;len-=2)
        sum += *p++;
    if(len==1)
        sum+=*(unsigned char*)p;
    sum=(sum>>16)+(sum&0xFFFF);
    sum+=(sum>>16);
    result=~sum;
    return result;
}

char* reverse_DNS_lookup(char *ip) 
{
    struct sockaddr_in t;
    socklen_t len;
    char buf[NI_MAXHOST], *host_name;
    t.sin_family=AF_INET;
    t.sin_addr.s_addr=inet_addr(ip); // converts ipv4 form into binary form network byte
    len = sizeof(struct sockaddr_in);
  
    if(getnameinfo((struct sockaddr*)&t,len,buf,sizeof(buf),NULL,0,NI_NAMEREQD))  
    return NULL;
    host_name=(char*)malloc((strlen(buf)+1)*sizeof(char));
    strcpy(host_name,buf);
    return host_name;
}

char *DNS_lookup(char *addr_host, struct sockaddr_in *addr_info) 
{
	char *ip=(char*)malloc(NI_MAXHOST*sizeof(char));
    struct hostent *host_info;
    host_info=gethostbyname(addr_host);
    if(host_info==NULL)
        return NULL;
    strcpy(ip, inet_ntoa(*(struct in_addr *)host_info->h_addr));  // inet_ntoa() converts internet host address into string
    (*addr_info).sin_family=host_info->h_addrtype;
    (*addr_info).sin_port=htons(PORT); // converts host byte order to network byte order
    (*addr_info).sin_addr.s_addr=*(long*)host_info->h_addr;

    return ip;
}


void ping_testing(int ping_socket, struct sockaddr_in *addr_info, char *domain, char *ip, char *host_in) 
{

	struct data_packet data1;
    struct sockaddr_in receiver;
    struct timespec s, e, s1, e1;
    long double rec_time=0, total_time=0;
    struct timeval t;

    int ttl_val=64;
    int c=0;
    int i;
    int addr_len;
    int f=1;
    int c1=0;

    t.tv_sec = TIMELIMIT;
    t.tv_usec = 0;

    clock_gettime(CLOCK_MONOTONIC, &s1);

    if(setsockopt(ping_socket,SOL_IP,IP_TTL,&ttl_val,sizeof(ttl_val))!=0) // setting Internet Protocol time to live while sending data
    {
        printf("\nSetting socket for live time failed!\n");
        return;
    }

    setsockopt(ping_socket, SOL_SOCKET, SO_RCVTIMEO,(const char*)&t, sizeof t); // setting Internet Protocol time to live while receiving data

    while(ping)
    {
        f=1;
        bzero(&data1, sizeof(data1));

        data1.hdr.type = ICMP_ECHO; //internet control message protocol
        data1.hdr.un.echo.id=getpid();

        for (i=0;i<sizeof(data1.msg)-1;i++)
            data1.msg[i]='1'; //any data to be send
        
        data1.msg[i]=0;
        data1.hdr.un.echo.sequence=c++; 
        data1.hdr.checksum=checksum(&data1, sizeof(data1)); 
  
        usleep(1000000);

        clock_gettime(CLOCK_MONOTONIC, &s);
        if (sendto(ping_socket,&data1,sizeof(data1),0,(struct sockaddr*)addr_info,sizeof(*addr_info))<=0) //sending data 
        { 
            printf("\nSending Failed!\n"); 
            f=0;
        }

        addr_len=sizeof(receiver);

        if (recvfrom(ping_socket,&data1,sizeof(data1),0,(struct sockaddr*)&receiver,(socklen_t*)&addr_len)<= 0 && c>1) //receiving data 
        { 
            printf("\nReceiving failed!\n"); 
        }
        else
        { 
            clock_gettime(CLOCK_MONOTONIC, &e);

            double timeElapsed=((double)(e.tv_nsec -s.tv_nsec))/1000000.0 ;

            rec_time = (e.tv_sec- s.tv_sec) * 1000.0 + timeElapsed; 

            if(f)
            { 
                if(!(data1.hdr.type==69 && data1.hdr.code==0))  
                {
                    printf("Error with ICMP \n"); 
                }
                else
                {
                    printf("%d bytes from %s (%s) icmp_seq=%d ttl=%d  time = %.2Lf ms.\n",  SIZE, domain, ip, c, ttl_val, rec_time);
                    c1++;
                }
            }
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &e1);

    double timeElapsed = ((double)(e1.tv_nsec -s1.tv_nsec))/1000000.0; 

    total_time=(e1.tv_sec-s1.tv_sec)*1000.0+timeElapsed ;
    printf("\n------------------------------------------------------------------------------------------------\n");
    printf("\n%d Packets transmitted, %d received, %.2f %% Packet loss.  Total time: %Lf ms.\n\n",  c, c1, ((c - c1)/c) * 100.0, total_time);  
}


int main(int argc, char *argv[]) 
{
    struct sockaddr_in addr_info;
    int addrlen = sizeof(addr_info);

    char net_buf[NI_MAXHOST];
    int ping_socket;
    char *ip, *host_name;
    
    if(argc!=2)
    {
        printf("\nFormat Error\n");
        return 0;
    }

    ip=DNS_lookup(argv[1],&addr_info);
    if(ip==NULL)
    {
        printf("\nError\n");
        return 0;
    }

    host_name=reverse_DNS_lookup(ip);
    printf("\nPING '%s' IP: %s HOST-NAME: %s\n\n",argv[1],ip,host_name);

    ping_socket=socket(AF_INET,SOCK_RAW,IPPROTO_ICMP); // run in sudo mode otherwise SOCK_RAW will not work
    if(ping_socket<0)
    {
        printf("\nSocket not created\n");
        return 0;
    }

    signal(SIGINT, interrupt_func); // for interrupt handling

    ping_testing(ping_socket,&addr_info,host_name,ip,argv[1]);
    return 0;
} 
