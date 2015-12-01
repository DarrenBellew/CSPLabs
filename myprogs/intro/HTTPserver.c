#include "unp.h" //include appropriate header file(s) for in-build socket
//functions etc...
#include <time.h> //required to calculate date and time
#include <stdio.h>
#include <string.h>
#define HOME_PAGE "HTTP/1.1 200 OK\r\n\r\n<html><body>Hey shit head</body></html>\r\n"
#define ERROR_PAGE "HTTP/1.1 404 Not OK\r\n\r\n<html><body>Goodbye</body></html>\r\n"


void clearInput();

int main(int argc, char **argv)  {
	char cmd[16];
	char path[64];
	char vers[16];


	int listenfd, connfd; //socket IDs; one for the listening socket and one
	//for the connected socket
	struct sockaddr_in servaddr; //address structure to hold his server's 
	//address
	char buff[MAXLINE+1], recvline[MAXLINE+1]; //buffer to hold send data


	if (argc != 2) //verifies number of command-line args; two in this 
	//instant
		err_quit("usage: ./httpserver <Port No.>");

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	/*
	Create socket. Note this slightly different to the client code
	(uses lower case 's') as the
	error checking is wrapping inside a new function called Socket 
	(upper case 'S)
	*/
	bzero(&servaddr, sizeof(servaddr)); //zero and fill in server
	//address structure
	servaddr.sin_family = AF_INET;

	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //connect to any local IP Addr
//	if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)  { //convert ip to network byte order
//		err_quit("inet_pton error for %s", argv[1]);
//	}




	servaddr.sin_port = htons(atoi(argv[1])); //daytime server port number


	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
	//connects the socket to an external interface

	Listen(listenfd, LISTENQ);
	//Changes the socket to a "passive listening" socket

	while(1)  {
		connfd = Accept(listenfd, (SA *) NULL, NULL);
		int n;
		//Accept next connection
		while((n=read(connfd, recvline, MAXLINE)) >0)  {

			recvline[n] = 0;
			if(fputs(recvline, stdout) ==EOF)  {
				err_sys("fputs error");
			}
			if(strstr(recvline, "\r\n\r\n")>0)  {
				break;
			}
		}
		sscanf(recvline, "%s %s %s", cmd, path, vers);

		if(strcmp(path, "/index.html") == 0)  {
			snprintf(buff, sizeof(buff), HOME_PAGE);
		}
		else  {
			snprintf(buff, sizeof(buff), ERROR_PAGE);
		}


		//ticks = time(NULL);
		//snprintf(buff,sizeof(buff), "%.24s\r\n", ctime(&ticks));

		//construct the data to return to the client
		Write(connfd, buff, strlen(buff));//write data to the client

		//getchar();
		//clearInput();
		Close(connfd);
	}
}

void clearInput()  {
	int ch;

	while((ch=getchar()) != EOF && ch != '\n');

}
