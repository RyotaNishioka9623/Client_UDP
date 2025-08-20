#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring> 

int main ()
{
   int port = 1910;
   const char* ipv4 = "192.168.50.1"; 

   int sockfd;
   sockaddr_in addr;

   sockfd = socket(AF_INET, SOCK_DGRAM, 0); // ★SOCK_DGRAMに修正
   if (sockfd < 0) 
   {
      std::cerr << "Cannot create socket" << std::endl;
      return -1;
   }

   memset(&addr, 0, sizeof(addr));
   addr.sin_family = AF_INET;
   addr.sin_port = htons(port);
   addr.sin_addr.s_addr = inet_addr(ipv4);

   char msg_send[128] = "Hello, server!";

   sendto(sockfd, msg_send, std::size(msg_send), 0,(sockaddr*)&addr,sizeof(addr)); // ★sockfdに修正

   close(sockfd); // ★sockfdに修正
   return 0;
}