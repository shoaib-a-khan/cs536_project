/* ------------ bob -------------*/

#include "includes.h"

#define ALICE_PORT 1111
#define CAROL_PORT 2222
#define BOB_PORT 3333

/* ---------- Synchronization Variables --------*/

float state_B = 0;
extern float state_A;
extern float state_C;
int send_to_Alice = 0;	//flag to signal server thread to write to socket
int send_to_Carol = 0; 	//flag to signal client thread to write to socket


						/* ----------- Subroutine Prototypes -----------*/
void client(char *ip, int portno, char op);
int server(int portno, char op);

/* ----------- Abstract Data Types -------------*/

struct message
{
	/*int P[100];
	int D[100];
	int len;*/
	int scalar1;
	int scalar2;
	char src;
	char dest;
	int state;

}msg_for_Alice, msg_for_Carol, msg_from_Alice, msg_from_Carol;


/*-------------- Main Program ----------------*/

int main(int argc, char* argv)
{
	char ip[] = "127.0.0.1";	//localhost
	int x_B, y_B;
	std::thread server_thread(server, BOB_PORT);	//As server to Carol
	sleep(5);		//wait 5 seconds for Alice & Carol to go live
	std::thread client_thread(client, ip, ALICE_PORT);	//As client to Alice
	if (argv[1] == '+')
	{
		int s_B;
		x_B = atoi(argv[2]);
		y_B = atoi(argv[3]);
		s_B = x_B + y_B;
		printf("Bob's Share of Sum : %d\n", s_B);
		return 1;
	}
	else if (argv[1] == 'x')
	{
		int p_B, p1_B, p2_B;
		x_B = atoi(argv[2]);
		y_B = atoi(argv[3]);
		p_B = x_B * y_B;	//Bob locally computes x"y"

							//Preparing to initiate OMHelper
		srand(time(NULL));
		int b_1 = rand();	//Bob randomly splits y"
		int b_2 = y_B - b_1;
		msg_for_Alice.scalar1 = b_1; 	//Bob prepares b_1 for Alice 
		msg_for_Carol.scalar1 = b_2;	//Bob prepares b_2 for Carol

		send_to_Bob = 1;	//Signal client thread to send b_1 to Alice
		send_to_Carol = 1;	//Signal server thread to send b_2 to Carol

	}

	server_thread.join();
	client_thread.join();
	return 1;

}


/*---------------- Subroutines ----------------*/

int server(int portno, char op)
{
	int sockfd, newsockfd;
	socklen_t clilen;
	char buffer[256];
	ssize_t r;
	int n;

	struct sockaddr_in serv_addr, cli_addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		printf("ERROR opening socket");
	bzero((char *)&serv_addr, sizeof(serv_addr));
	//portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
		sizeof(serv_addr)) < 0)
		printf("ERROR on binding");

	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd,
		(struct sockaddr *) &cli_addr,
		&clilen);
	if (newsockfd < 0)
	{
		printf("ERROR on accept at Alice's Server!");
		return 1;
	}
	while (1)
	{
		//bzero(buffer,256);
		//n = read(newsockfd,buffer,255);
		//if (n < 0) printf("ERROR reading from socket");
		//printf("Here is the message: %s\n",buffer);
		if (send_to_Carol)
		{
			send_to_Carol = 0;
			n = write(newsockfd, &msg_for_Carol, sizeof(msg_for_Carol), 0);
		}
		if (n < 0) printf("ERROR writing to socket");
		sleep(1);
	}
	//close(newsockfd);
	//close(sockfd);

	return 0;
}

void client(char *ip, int portno, char op)
{
	int sockfd, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[256];
	//portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		printf("ERROR opening socket");
	server = gethostbyname(ip);
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host\n");
		//exit(0);
	}
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
		(char *)&serv_addr.sin_addr.s_addr,
		server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		printf("ERROR connecting");
	while (1)
	{

		//printf("Please enter the message: ");
		//bzero(buffer,256);
		//fgets(buffer,255,stdin);
		if (send_to_Alice)
		{
			send_to_Alice = 0;
			n = write(sockfd, &msg_to_Alice, sizeof(msg_to_Alice), 0);
		}
		if (n < 0)
			printf("ERROR writing to socket");
		//bzero(buffer,256);
		//n = read(sockfd,buffer,255);
		//if (n < 0) 
		//	printf("ERROR reading from socket");
		//printf("%s\n",buffer);
	}
	close(sockfd);
}
