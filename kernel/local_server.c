#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>

char* responsePacket() {
   int randNum = rand() % 16;
   char buf[4];
   sprintf(buf, "%d", randNum);
   
   if(randNum < 10) {  
     char response[256] = "HTTP/1.1 200 OK\r\nContent-Length: 1\r\nContent-Type: text/plain\r\n\r\n";
     strcat(response, buf);
     return response;
   } else {
     char response[256] = "HTTP/1.1 200 OK\r\nContent-Length: 2\r\nContent-Type: text/plain\r\n\r\n";
     strcat(response, buf);
     return response;
   } 
}

int main(int argc, char *args[]) {

	int sock;
	int client;
	struct sockaddr_in addr;
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(addr);
	int bind_port = 8080;
	int on = 1;
	char buffer[1024] = {0,};
	size_t read_len;

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		perror("socket() error");
		return 1;
	}
	
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*)&on, sizeof(on)) != 0) {
		perror("setsockopt() error");
		return 1;
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(bind_port);
	if (bind(sock, (struct sockaddr*)&addr, addr_len) != 0) {
		perror("bind() error");
		return 1;
	}

	if (listen(sock, 5) != 0) {
		perror("listen() error");
		return 1;
	}

	printf("listening... %d\n", bind_port);

        while(1) {
  	   client = accept(sock, (struct sockaddr*)&client_addr, &addr_len);
	   if (client < 0) {
		perror("accept() error");
		return 1;
           } 

	   read_len = recv(client, buffer, sizeof(buffer), 0);
	   if (read_len > 0) {
	       	printf("RECV: %s", buffer);
	   }

	   send(client, responsePacket(), strlen(responsePacket()), 0);

	   close(client);
        }
	close(sock);
	
        return 0;
}

