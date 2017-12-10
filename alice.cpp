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
int rcv_from_Bob = 0;	//flag to signal server thread to read from socket
int rcv_from_Carol = 0; //flag to signal client thread to read from socket
int client_rcv = 0;	//flag to signal main that client thread has recvd data
int server_rcv = 0;	//flag to signal main that servr thread has rcvd data
int terminate_client = 0; //flag to signal terminate to client thread
int terminate_server = 0; //flag to signal terminate to server thread

						  /* ----------- Subroutine Prototypes -----------*/
void client(char *ip, int portno, char op);
int server(int portno, char op);


/* ----------- Abstract Data Types -------------*/

struct message
{
	/*int P[100][5];
	int D[100];
	int len;*/
	float scalar1 = 0;
	float scalar2 = 0;
	char src = 'A';
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

	if (strcmp(argv[1], "+") == 0)	//Oblivious Addition
	{
		int s_A;
		x_A = std::atoi(argv[2]);
		y_A = std::atoi(argv[3]);
		s_A = x_A + y_A;
		printf("Alice's Share of Sum : %d\n", s_A);
		return 1;
	}
	else if (strcmp(argv[1], "x") == 0)	//Oblivious Multiplication
	{
		double p_A, p1_A, p2_A;
		x_A = std::atoi(argv[2]);
		y_A = std::atoi(argv[3]);
		p_A = x_A * y_A;	//Alice locally computes x'y'

							//Preparing to initiate OMHelper to compute x'y"
		srand(time(NULL));
		int a_1 = rand();	//Alice randomly splits x'
		int a_2 = x_A - a_1;
		msg_for_Bob.scalar1 = a_1; 	//Alice prepares a_1 for Bob
		msg_for_Bob.dest = 'B';
		msg_for_Carol.scalar1 = a_2;	//Alice prepares a_2 for Carol
		msg_for_Carol.dest = 'C';

		send_to_Bob = 1;	//Signal server thread to send a_1 to Bob
		send_to_Carol = 1;	//Signal client thread to send a_2 to Carol
		rcv_from_Bob = 1;	//Signal server thread to rcv b_1 from Bob
		while (!server_rcv);	//Waiting to recv b_1 from Bob
		server_rcv = 0;
		p1_A = a_2 * msg_from_Bob.scalar1; //computing a_2 b_1
		rcv_from_Carol = 1;	//Signal client thread to rcv (a_2 b_2 - r) from Carol		
		while (!client_rcv);	//Waiting to rcv (a_2 b_2 - r) from Carol
		client_rcv = 0;
		p1_A += msg_from_Carol.scalar1; //computing a_2 b_1 + a_2 b_2 - r

										//Preparing to initiate OMHelper to compute x"y'
		srand(time(NULL));
		a_1 = rand();	//Alice randomly splits x'
		a_2 = y_A - a_1;
		msg_for_Bob.scalar1 = a_1; 	//Alice prepares a_1 for Bob
		msg_for_Bob.dest = 'B';
		msg_for_Carol.scalar1 = a_2;	//Alice prepares a_2 for Carol
		msg_for_Carol.dest = 'C';

		send_to_Bob = 1;	//Signal server thread to send a_1 to Bob
		send_to_Carol = 1;	//Signal client thread to send a_2 to Carol
		rcv_from_Bob = 1;	//Signal server thread to rcv b_1 from Bob
		while (!server_rcv);	//Waiting to recv b_1 from Bob
		server_rcv = 0;
		p2_A = a_2 * msg_from_Bob.scalar1; //computing a_2 b_1
		rcv_from_Carol = 1;	//Signal client thread to rcv (a_2 b_2 - r) from Carol		
		while (!client_rcv);	//Waiting to recv (a_2 b_2 - r) from Carol
		client_rcv = 0;
		p2_A += msg_from_Carol.scalar1; //computing a_2 b_1 + a_2 b_2 - r

		p_A = p_A + p1_A + p2_A;	//computing p' = x'y' + p_1' + p_2'
		printf("Alice's Share of Product : %f\n", p_A);
		terminate_client = 1;
		terminate_server = 1;
		return 1;


	}
	else if (strcmp(argv[1], ">") == 0)		//Oblivious Multiplication
	{

	}
	//std::thread server_thread(server, ALICE_PORT); //Alice as server to Bob
	//sleep(5);		//wait 5 seconds for Bob and Carol to go live
	//std::thread client_thread(client, ip, CAROL_PORT); //Alice as client to Carol
	//server_thread.join();
	//client_thread.join();


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
		printf("ERROR opening server socket in Alice!\n");
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
		sizeof(serv_addr)) < 0)
		printf("ERROR on binding server socket in Alice!\n");

	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd,
		(struct sockaddr *) &cli_addr,
		&clilen);
	if (newsockfd < 0)
	{
		printf("ERROR on accept at Alice's Server!\n");
		return 1;
	}
	while (!terminate_server)
	{
		if (send_to_Bob)
		{
			n = write(newsockfd, &msg_for_Bob, sizeof(msg_for_Bob));
			send_to_Bob = 0;
			if (n < 0)
				printf("ERROR in Alice writing to Bob's socket!\n");

		}
		if (rcv_from_Bob)
		{
			n = read(sockfd, &msg_from_Bob, sizeof(msg_from_Bob));
			if (n < 0)
				printf("ERROR in Alice reading from Bob's socket!\n");
			else
			{
				server_rcv = 1;
				rcv_from_Bob = 0;
			}

		}

		sleep(1);
	}
	close(newsockfd);
	close(sockfd);

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
		printf("ERROR opening socket client socket in Alice!\n");
	server = gethostbyname(ip);
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host(Carol is not live)!\n");
		//exit(0);
	}
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
		(char *)&serv_addr.sin_addr.s_addr,
		server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		printf("ERROR connecting Alice to Carol!\n");

	while (!terminate_client)
	{
		if (send_to_Carol)
		{
			n = write(sockfd, &msg_for_Carol, sizeof(msg_for_Carol));
			send_to_Carol = 0;
			if (n < 0)
				printf("ERROR writing to client socket in Alice!\n");
		}
		n = read(sockfd, &msg_from_Carol, sizeof(msg_from_Carol));
		if (n < 0)
			printf("ERROR reading from socket client socket in Alice!\n");
		else
			client_rcv = 1;
	}

	close(sockfd);
}
