#include "unp.h" //include appropriate header file(s) for in-build socket
//functions etc...
#include <time.h> //required to calculate date and time
#include <stdio.h>

void clearInput();

int main(int argc, char **argv)  {
	int listenfd, connfd; //socket IDs; one for the listening socket and one
	//for the connected socket
	struct sockaddr_in servaddr; //address structure to hold his server's 
	//address
	char buff[MAXLINE]; //buffer to hold send data
	time_t ticks; //required to calculate datea and time

	if (argc != 3) //verifies number of command-line args; two in this 
	//instant
		err_quit("usage: <Program Name> <Port No.>");

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
	
	//servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //connect to any local IP Addr
	if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)  { //convert ip to network byte order
		err_quit("inet_pton error for %s", argv[1]);
	}




	servaddr.sin_port = htons(atoi(argv[2])); //daytime server port number


	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
	//connects the socket to an external interface

	Listen(listenfd, LISTENQ);
	//Changes the socket to a "passive listening" socket

	while(1)  {
		connfd = Accept(listenfd, (SA *) NULL, NULL);
		//Accept next connection

		ticks = time(NULL);
		snprintf(buff,sizeof(buff), "%.24s\r\n", ctime(&ticks));
		//construct the data to return to the client
		Write(connfd, buff, strlen(buff));//write data to the client
		
		//getchar();
		clearInput();
		Close(connfd);
	}
}

void clearInput()  {
	int ch;

	while((ch=getchar()) != EOF && ch != '\n');

}
