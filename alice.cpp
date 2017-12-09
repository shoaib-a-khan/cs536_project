/* --------------- Alice -----------------------*/
#include "includes.h"


#define ALICE_PORT 1111
#define CAROL_PORT 2222
#define BOB_PORT 3333


/* ---------- Synchronization Variables --------*/
float state_A = 0;
extern float state_B;
extern float state_C;
int send_to_Bob = 0;	//flag to signal server thread to write to socket
int send_to_Carol = 0; 	//flag to signal client thread to write to socket


						/* ----------- Subroutine Prototypes -----------*/
void client(char *ip, int portno, char op);
int server(int portno, char op);


/* ----------- Abstract Data Types -------------*/

struct message
{
	/*int P[100][5];
	int D[100];
	int len;*/
	int scalar1 = 0;
	int scalar2 = 0;
	char src = 0;
	char dest = 0;
	int state = 0;

} msg_for_Bob, msg_for_Carol, msg_from_Bob, msg_from_Carol;

/*-------------- Main Program ----------------*/

int main(int argc, char* argv[])
{

	char ip[] = "127.0.0.1";	//localhost
	int x_A, y_A;
	//Establish connections with remote hosts i.e Bob & Carol
	std::thread server_thread(server, ALICE_PORT, argv[1][0]); //Alice as server to Bob
	sleep(5);		//wait 5 seconds for Bob and Carol to go live
	std::thread client_thread(client, ip, CAROL_PORT, argv[1][0]); //Alice as client to Carol

	if (argv[1] == "+")		//Oblivious Addition
	{
		int s_A;
		x_A = std::atoi(argv[2]);
		y_A = std::atoi(argv[3]);
		s_A = x_A + y_A;
		printf("Alice's Share of Sum : %d\n", s_A);
		return 1;
	}
	else if (argv[1] == "x")		//Oblivious Multiplication
	{
		int p_A, p1_A, p2_A;
		x_A = std::atoi(argv[2]);
		y_A = std::atoi(argv[3]);
		p_A = x_A * y_A;	//Alice locally computes x'y'

							//Preparing to initiate OMHelper
		srand(time(NULL));
		int a_1 = rand();	//Alice randomly splits x'
		int a_2 = x_A - a_1;
		msg_for_Bob.scalar1 = a_1; 	//Alice prepares a_1 for Bob 
		msg_for_Carol.scalar1 = a_2;	//Alice prepares a_2 for Carol

		send_to_Bob = 1;	//Signal server thread to send a_1 to Bob
		send_to_Carol = 1;	//Signal client thread to send a_2 to Carol		


	}
	//std::thread server_thread(server, ALICE_PORT); //Alice as server to Bob
	//sleep(5);		//wait 5 seconds for Bob and Carol to go live
	//std::thread client_thread(client, ip, CAROL_PORT); //Alice as client to Carol
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
		printf("Here is the message: %s\n", buffer);
		n = write(newsockfd, "I got your message", 18);
		if (n < 0) printf("ERROR writing to socket");
		sleep(1);
	}
	//close(newsockfd);
	//close(sockfd);

	return 0;
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
		if (send_to_Bob)
		{
			send_to_Bob = 0;
			n = write(newsockfd, &msg_for_Bob, sizeof(msg_for_Bob), 0);
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
		if (send_to_Carol)
		{
			send_to_Carol = 0;
			n = write(sockfd, &msg_to_Carol, sizeof(msg_to_Carol), 0);
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
