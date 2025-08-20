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
   const char* rasppi03 = "192.168.50.31"; 
   const char* rasppi04 = "192.168.50.32";

   int sockfd;
   sockaddr_in addr01;
   sockaddr_in addr02;
   sockaddr_in addr03;
   sockaddr_in addr04;

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

   // rasppi03のアドレスを設定
   memset(&addr03, 0, sizeof(addr03));
   addr03.sin_family = AF_INET;
   addr03.sin_port = htons(port);
   addr03.sin_addr.s_addr = inet_addr(rasppi03);
   
   // rasppi04のアドレスを設定
   memset(&addr04, 0, sizeof(addr04));
   addr04.sin_family = AF_INET;
   addr04.sin_port = htons(port);
   addr04.sin_addr.s_addr = inet_addr(rasppi04);
   
   char msg_send01[128] = "1";
   char msg_send02[128] = "1";
   char msg_send03[128] = "1";
   char msg_send04[128] = "1";

   sendto(sockfd, msg_send01, std::size(msg_send01), 0,(sockaddr*)&addr01,sizeof(addr01)); 
   sendto(sockfd, msg_send02, std::size(msg_send02), 0,(sockaddr*)&addr02,sizeof(addr02));
   sendto(sockfd, msg_send03, std::size(msg_send03), 0,(sockaddr*)&addr03,sizeof(addr03));
   sendto(sockfd, msg_send04, std::size(msg_send04), 0,(sockaddr*)&addr04,sizeof(addr04));

   close(sockfd); 
   return 0;
}