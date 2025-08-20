#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring> 

int main ()
{
   int port = 1910;
   const char* rasppi01 = "192.168.50.12"; 
   const char* rasppi02 = "192.168.50.56";

   int sockfd;
   sockaddr_in addr01;
   sockaddr_in addr02;

   sockfd = socket(AF_INET, SOCK_DGRAM, 0); // ★SOCK_DGRAMに修正
   if (sockfd < 0) 
   {
      std::cerr << "Cannot create socket" << std::endl;
      return -1;
   }

   // rasppi01のアドレスを設定
   memset(&addr01, 0, sizeof(addr01));
   addr01.sin_family = AF_INET;
   addr01.sin_port = htons(port);
   addr01.sin_addr.s_addr = inet_addr(rasppi01);
   
   // rasppi02のアドレスを設定
   memset(&addr02, 0, sizeof(addr02));
   addr02.sin_family = AF_INET;
   addr02.sin_port = htons(port);
   addr02.sin_addr.s_addr = inet_addr(rasppi02);
   
   char msg_send01[128] = "Hello, World!";
   char msg_send02[128] = "Hello, Ikuto!";

   sendto(sockfd, msg_send01, std::size(msg_send01), 0,(sockaddr*)&addr01,sizeof(addr01)); 
   sendto(sockfd, msg_send02, std::size(msg_send02), 0,(sockaddr*)&addr02,sizeof(addr02));

   close(sockfd); 
   return 0;
}