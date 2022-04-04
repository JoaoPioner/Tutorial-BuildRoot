/*
	Simple http server 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

 
#define BUFLEN	1024	//Max length of buffer
#define PORT	8000	//The port on which to listen for incoming data

char http_ok[] = "HTTP/1.0 200 OK\r\nContent-type: text/html\r\nServer: Test\r\n\r\n";
char http_error[] = "HTTP/1.0 400 Bad Request\r\nContent-type: text/html\r\nServer: Test\r\n\r\n";
char page[1000000];
char page1[] = "<!DOCTYPE html>\n<html>\n<head>\n</head>\n<body>\n<p>";
char page2[] = "</p>\n</body>\n</html>";


void die(char *s)
{
	perror(s);
	exit(1);
}
 
int main(void)
{
	struct sockaddr_in si_me, si_other;

	int s, i, slen = sizeof(si_other) , recv_len, conn;
	char buf[BUFLEN];
     
	/* create a TCP socket */
	if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
		die("socket");
    
	/* zero out the structure */
	memset((char *) &si_me, 0, sizeof(si_me));
     
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
	/* bind socket to port */
	if (bind(s, (struct sockaddr*)&si_me, sizeof(si_me)) == -1)
		die("bind");
	
	/* allow 10 requests to queue up */ 
	if (listen(s, 10) == -1)
		die("listen");
    

	FILE *fp;
    char path[1035];

    /* Open the command for reading. */
    fp = popen("uptime", "r");
    if (fp == NULL) {
        printf("Failed to run command\n" );
        exit(1);
    }

    /* Read the output a line at a time - output it. */
    while (fgets(path, sizeof(path), fp) != NULL) {
        printf("%s", path);
    }

    /* close */
    pclose(fp);

	char* strC[1000000];
	strcat(strC, page1);
	strcat(strC, path);
	strcat(strC, page2);
	strcpy(page, strC);
	/* close the connection */
	close(conn);
	/* keep listening for data */
	while (1) {
		memset(buf, 0, sizeof(buf));
		printf("Waiting a connection...");
		fflush(stdout);
		
		conn = accept(s, (struct sockaddr *) &si_other, &slen);
		if (conn < 0)
			die("accept");

		printf("Client connected: %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));

		/* try to receive some data, this is a blocking call */
		recv_len = read(conn, buf, BUFLEN);
		if (recv_len < 0)
			die("read");

		/* print details of the client/peer and the data received */
		printf("Data: %s\n" , buf);
         
		if (strstr(buf, "GET")) {
			/* now reply the client with the same data */
			if (write(conn, http_ok, strlen(http_ok)) < 0)
				die("write");
			if (write(conn, page, strlen(page)) < 0)
				die("write");
		} else {
			if (write(conn, http_error, strlen(http_error)) < 0)
				die("write");
		}

	}
	close(s);
	
	return 0;
}
