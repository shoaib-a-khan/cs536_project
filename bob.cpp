/* ------------ bob -------------*/

#include "includes.h"

#define ALICE_PORT 1111
#define CAROL_PORT 2222
#define BOB_PORT 3333

/* ---------- Synchronization Variables --------*/

float state_B = 0;
extern float state_A;
extern float state_C;

int send_to_Alice = 0;	//flag to signal client thread to write to socket
int send_to_Carol = 0; 	//flag to signal server thread to write to socket
int rcv_from_Alice = 0;	//flag to signal client thread to read from socket
int rcv_from_Carol = 0; //flag to signal server thread to read from socket
int client_rcv = 0;	//flag to signal main that client thread has recvd data
int server_rcv = 0;	//flag to signal main that servr thread has rcvd data
int terminate_client = 0; //flag to signal terminate to client thread
int terminate_server = 0; //flag to signal terminate to server thread


						  
void client(char *ip, int portno, char op);
int server(int portno, char op);

/* ----------- Abstract Data Types -------------*/

struct message
{
	/*int P[100];
	int D[100];
	int len;*/
	int scalar1 = 0;
	int scalar2 = 0;
	char src = 'B';
	char dest = 0;
	int state = 0;

}msg_for_Alice, msg_for_Carol, msg_from_Alice, msg_from_Carol;


/*-------------- Main Program ----------------*/

int main(int argc, char* argv[])
{
	char ip[] = "127.0.0.1";	//localhost
	int x_B, y_B;

	//Establish connections with remote hosts i.e Bob & Carol
	std::thread server_thread(server, BOB_PORT, argv[1][0]);	//As server to Carol
	sleep(5);		//wait 5 seconds for Alice & Carol to go live
	std::thread client_thread(client, ip, ALICE_PORT, argv[1][0]);	//As client to Alice

	if (strcmp(argv[1], "+") == 0)	//Oblivious Addition


	{
		int s_B;
		x_B = std::atoi(argv[2]);
		y_B = std::atoi(argv[3]);
		s_B = x_B + y_B;
		printf("Bob's Share of Sum : %d\n", s_B);
		return 1;
	}

	else if (strcmp(argv[1], "x") == 0)	//Oblivious Multiplication

	{
		int p_B, p1_B, p2_B;
		x_B = std::atoi(argv[2]);
		y_B = std::atoi(argv[3]);
		p_B = x_B * y_B;	//Bob locally computes x"y"

		printf("1. Bob: p_B = %f\n", p_B);
		printf("1. Bob: x_B = %d\n", x_B);
		printf("1. Bob: y_B = %d\n", y_B);

		//Preparing to initiate OMHelper to compute x'y"
		srand(time(NULL));
		int b_1 = rand() % 1000;	//Bob randomly splits y"
		int b_2 = y_B - b_1;
		msg_for_Alice.scalar1 = b_1; 	//Bob prepares b_1 for Alice
		msg_for_Alice.dest = 'A';
		msg_for_Carol.scalar1 = b_2;	//Bob prepares b_2 for Carol
		msg_for_Carol.dest = 'C';
		printf("2. Bob -> Alice: b_1 = %d\n", msg_for_Alice.scalar1);
		printf("2. Bob -> Carol: b_2 = %d\n", msg_for_Carol.scalar1);


		send_to_Alice = 1;	//Signal client thread to send b_1 to Alice
		send_to_Carol = 1;	//Signal server thread to send b_2 to Carol
		rcv_from_Alice = 1;	//Signal client thread to rcv a_1 from Alice
		while (!client_rcv);	//Waiting to recv a_1 from Alice
		client_rcv = 0;
		p1_B = b_1 * msg_from_Alice.scalar1; //computing a_1 b_1
		p1_B += b_2 * msg_from_Alice.scalar1; //computing a_1 b_1 + a_1 b_2
		printf("3. Bob <- Alice: a_1 = %d\n", msg_from_Alice.scalar1);
		rcv_from_Carol = 1;	//Signal server thread to rcv r from Carol		
		while (!server_rcv);	//Waiting to rcv r from Carol
		server_rcv = 0;
		p1_B += msg_from_Carol.scalar1; //computing a_1 b_1 + a_1 b_2 + r
		printf("3. Bob <- Carol: r = %d\n", msg_from_Carol.scalar1);
		//Preparing to initiate OMHelper to compute x"y'

		b_1 = rand() % 1000;	//Bob randomly splits x"
		b_2 = x_B - b_1;
		msg_for_Alice.scalar1 = b_1; 	//Bob prepares b_1 for Alice
		msg_for_Alice.dest = 'A';
		msg_for_Carol.scalar1 = b_2;	//Bob prepares b_2 for Carol
		msg_for_Carol.dest = 'C';

		send_to_Alice = 1;	//Signal client thread to send b_1 to Alice
		send_to_Carol = 1;	//Signal server thread to send b_2 to Carol
		rcv_from_Alice = 1;	//Signal client thread to rcv a_1 from Alice
		while (!client_rcv);	//Waiting to recv a_1 from Alice
		client_rcv = 0;
		p2_B = b_1 * msg_from_Alice.scalar1; //computing a_1 b_1
		p2_B += b_2 * msg_from_Alice.scalar1; //computing a_1 b_1 + a_1 b_2
		rcv_from_Carol = 1;	//Signal server thread to rcv r from Carol		
		while (!server_rcv);	//Waiting to rcv r from Carol
		server_rcv = 0;
		p2_B += msg_from_Carol.scalar1; //computing a_1 b_1 + a_1 b_2 + r

		p_B = p_B + p1_B + p2_B;	//computing p' = x"y" + p_1" + p_2"
		printf("Bob's Share of Product : %d\n", p_B);
		terminate_client = 1;
		terminate_server = 1;
		return 1;
	}
	else if (strcmp(argv[1], ">=") == 0)
	{
		printf("Entered >=\n");
		int r_1, r_2, sgn, p_B, q_B;
		rcv_from_Alice = 1;
		while (!client_rcv);
		sgn = msg_from_Alice.scalar1;
		x_B = 2 * std::atoi(argv[2]) * sgn; //Reduction of > to >=
		y_B = 2 * std::atoi(argv[3]) * sgn; //Reduction of > to >=

		srand(time(NULL));
		r_2 = rand() % 1000;
		msg_for_Alice.scalar1 = r_2;

		send_to_Alice = 1;
		while (send_to_Alice);
		rcv_from_Alice = 1;
		while (!client_rcv);
		r_1 = msg_from_Alice.scalar1; // Alice and Bob have agreed on r_1 and r_2

		p_B = r_1 * x_B;
		q_B = r_1 * y_B;

		msg_for_Carol.scalar1 = p_B;
		msg_for_Carol.scalar2 = q_B;

		send_to_Carol = 1;
		while (send_to_Carol);
		rcv_from_Carol = 1;
		while (!server_rcv);

		terminate_client = 1;
		terminate_server = 1;
		printf("Bob's share of the answer is: %d\n", msg_from_Carol.scalar1);


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
		printf("ERROR opening server socket in Bob!\n");

	int enable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		printf("setsockopt(SO_REUSEADDR) failed");

	bzero((char *)&serv_addr, sizeof(serv_addr));
	//portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
		sizeof(serv_addr)) < 0)
		printf("ERROR on binding server socket in Bob!\n");

	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd,
		(struct sockaddr *) &cli_addr,
		&clilen);
	if (newsockfd < 0)
	{
		printf("ERROR on accept at Bob's Server!");
		return 1;
	}
	while (!terminate_server)
	{
		if (send_to_Carol)
		{
			n = write(newsockfd, &msg_for_Carol, sizeof(msg_for_Carol));
			send_to_Carol = 0;
			if (n < 0)
				printf("ERROR in Bob writing to Carol's socket!\n");

		}
		if (rcv_from_Carol)
		{
			n = read(newsockfd, &msg_from_Carol, sizeof(msg_from_Carol));
			if (n < 0)
				printf("ERROR in Bob reading from Alice's socket!\n");
			else
			{
				server_rcv = 1;
				rcv_from_Carol = 0;
			}

		}

		//sleep(1);
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
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		printf("ERROR opening client socket in Bob!\n");
	server = gethostbyname(ip);
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host(Alice is not live!)\n");
		//exit(0);
	}
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
		(char *)&serv_addr.sin_addr.s_addr,
		server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		printf("ERROR connecting Bob to Alice!\n");
	while (!terminate_client)
	{
		if (send_to_Alice)
		{
			n = write(sockfd, &msg_for_Alice, sizeof(msg_for_Alice));
			send_to_Alice = 0;
			if (n < 0)
				printf("ERROR writing to client socket in Bob!\n");
		}
		if (rcv_from_Alice)
		{
			n = read(sockfd, &msg_from_Alice, sizeof(msg_from_Alice));
			if (n < 0)
				printf("ERROR reading from client socket in Bob!\n");
			else
			{
				client_rcv = 1;
				rcv_from_Alice = 0;
			}
		}
	}
	close(sockfd);
}
