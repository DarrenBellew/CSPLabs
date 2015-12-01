#include "unp.h" //include appropriate header file(s) for in-build socket
//functions etc...
#include <stdio.h>
#include <string.h>

void clearInput();

int main(int argc, char **argv)  {
	//declare usual socket variables and extra vars for reading from file
	int n, listenfd, connfd, char_in, count = 0;
	//declare address structures for both the client and server 
	//addressing data
	struct sockaddr_in servaddr;
	//Declare read and write buggers
	char wbuff[MAXLINE], rbuff[MAXLINE], cmd[16];
	//extra buffers for parsing the client request
	//and printing client address
	char path1[64] =".", path[64], vers[16];
	//declare FILE pointer
	FILE * hFile;

	//making sure 2 arguments sent to C program
	if (argc != 2)  {
		err_quit("Usage: a.out <Port>");
	}
	//open local socket
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	//populate server addressing details
	//converts address to different format
	bzero(&servaddr, sizeof(servaddr));
	//1. Setting that any address can contact this (STRUCT)
	//2. Setting address of this server to listen to as nothing (any)
	//3. Setting port to listen on
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	//Bind the socket to the IP interface
	//Binding above settings to the socket (listenfd)
	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
	//Transform socket to a listening socker
	//Enable the socket to start listening
	Listen(listenfd, LISTENQ);

	//Inf loop, dealing with client connections
	for(;;)  {
		//Accept next client connection req and retrieve client address
		connfd = Accept(listenfd, (SA *) NULL, NULL);
		//Reading loop
		while((n = read(connfd, rbuff, MAXLINE)) >0)  {
			//Null terminate rbuff prioor to screen print
			rbuff[n] = 0;
			//screenprint contents of read buffer
			if(fputs(rbuff, stdout) == EOF)  {
				err_sys("fputs error");
			}
			//Looks for a needle in a haystack char *strstr(const
			if(strstr(rbuff,"\r\n\r\n")>0)  {
				break;
			}
			//end read loop
		}
		//end check on read loop
		if(n<0)  {
			err_sys("read error");
		}
		//parsing incoming client request
		sscanf(rbuff, "%s %s %s", cmd, path, vers);
		//concatenate the resource name to a 
		//full-stop to refer to "this" directory
		strcat(path1, path);
		if(strcmp(path1, "./") == 0)  {
			//check for empty "slash" and 
			//replace with default page
			strcpy(path1, "htmlFiles/index.html");
		}
		//open the requested file
		hFile = fopen(path1, "r");

		//if requested file does not exist
		if(hFile == NULL)  {
			//open the error file
			hFile = fopen("htmlFiles/error.html", "r");
		}
		//empty the outgoing buffer
		strcpy(wbuff,"");

		//reading one char at a time from open file
		while((char_in = fgetc(hFile)) != EOF)  {
			//storing the char in outgoing buffer
			wbuff[count] = char_in;
			//increment the buffer index ready for next character
			count++;
		}
		//null terminate outgoing buffer ready for waiting to socket
		wbuff[count] = 0;
		//write contents of wbuff to the socket
		Write(connfd, wbuff, strlen(wbuff));
		//reset buffer index
		count = 0;
		//close the file
		fclose(hFile);
		//reset path1 buffer back to "."
		strcpy(path1, ".");
		//close TCP connection;
		Close(connfd);

	}
}



void clearInput()  {
	int ch;

	while((ch=getchar()) != EOF && ch != '\n');

}
