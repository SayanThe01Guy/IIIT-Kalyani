/*
 * Calculator...Server Side
 */
#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdbool.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>

/*
 * TCP Uses 2 types of sockets, the connection socket and the listen socket.
 * The Goal is to separate the connection phase from the data exchange phase.
 * 
 */

int main(int argc, char *argv[]) {
	// port to start the server on
	int SERVER_PORT = 8877;

	// socket address used for the server
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;

	// htons: host to network short: transforms a value in host byte
	// ordering format to a short value in network byte ordering format
	server_address.sin_port = htons(SERVER_PORT);

	// htonl: host to network long: same as htons but to long
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);

	// create a TCP socket, creation returns -1 on failure
	int listen_sock;
	if((listen_sock = socket(PF_INET, SOCK_STREAM, 0))<0){
		printf("could not create listen socket\n");
		return 1;
	}

	// bind it to listen to the incoming connections on the created server
	// address, will return -1 on error
	if ((bind(listen_sock, (struct sockaddr *)&server_address, sizeof(server_address)))<0){
		perror("could not bind socket\n");
		return 1;
	}

	int wait_size = 16;  // maximum number of waiting clients, after which
	                     // dropping begins
	if (listen(listen_sock, wait_size)<0) {
		perror("could not open socket for listening\n");
		return 1;
	}

	// socket address used to store client address
	struct sockaddr_in client_address;
	int client_address_len = 0;

	// run indefinitely
	while (true) {
		// open a new socket to transmit data per connection
		int sock;
		if((sock = accept(listen_sock, (struct sockaddr *)&client_address, &client_address_len))<0){
			printf("could not open a socket to accept data\n");
			return 1;
		}

		int n = 1;
		int len = 0, maxlen = 100,a,b;
		char r_msg[maxlen];
		char *pbuffer = r_msg;

		printf("client connected with ip address: %s\n", inet_ntoa(client_address.sin_addr));

		// keep running as long as the client keeps the connection open
		while (n>0){
			n = recv(sock, pbuffer, maxlen, 0);
			pbuffer += n;
			maxlen -= n;
			len += n;
			a=atoi(r_msg);
			n = recv(sock, pbuffer, maxlen, 0);
			pbuffer += n;
			maxlen -= n;
			len += n;
			b=atoi(r_msg);
			n = recv(sock, pbuffer, maxlen, 0);
			pbuffer += n;
			maxlen -= n;
			len += n;
			if(strcmp("+",r_msg)==0)
				b+=a;
			else if(strcmp("-",r_msg)==0)
				b-=a;
			else if(strcmp("*",r_msg)==0)
				b*=a;
			else if(strcmp("/",r_msg)==0)
				b/=a;
			itoa(b,r_msg,10);	// int to str (int, str, base)
			// echo received content back
			send(sock, r_msg, len, 0);
		}

		// Open connection until received_message != "quit"
		while(strcmp(r_msg,"quit")==0)
			close(sock);
	}

	close(listen_sock);
	return 0;
}
