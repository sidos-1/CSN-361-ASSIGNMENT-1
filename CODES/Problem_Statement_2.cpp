#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/if.h>


int main()
{
  struct ifreq mac;

  int socket_1 = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

  /* AF_INET    : gives us a socket for internet domain
     SOCK_DGRAM : Creates Datagram socket ( for intramachine use )
     IPPROTO_IP : (Has value 0)Automatically selects the required protocol */ 

  if(socket_1 == -1){
    printf("Socket Error\n");
    return -1;
  }

  mac.ifr_addr.sa_family = AF_INET;
  strncpy(mac.ifr_name, "enp3s0",IFNAMSIZ-1);

  printf("PHYSICAL MAC ADDRESS : ");
  if (ioctl(socket_1, SIOCGIFHWADDR, &mac)==0) { //SIOCGIFHWADDR: to get the hardware address of the device.
    int i;
    for (i = 0; i < 5; ++i)
      printf("%.2x:", (unsigned char) mac.ifr_hwaddr.sa_data[i]);
      printf("%.2x", (unsigned char) mac.ifr_hwaddr.sa_data[5]);
    printf("\n");
    return 0;
  }
  else
  {
    printf("HW Address Error\n");  
    return 1;
  }
}