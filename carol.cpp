/* ------------ carol -------------*/

#include "includes.h"

#define ALICE_PORT 1111
#define CAROL_PORT 2222
#define BOB_PORT 3333
using namespace std;
/* structure of message that is sent and received */

struct message
{
	/*int P[100][5];
	int D[100];
	int len; */
	float scalar1;
	float scalar2;
	char src;
	char dest;
	int state;

}message;

/* Variables for syncing threads */
float state_C = 0.0;
int client_done = 0, server_done = 0, main_done_1 = 0, main_done_2 = 0;
int p_A, p_B, q_A, q_B;

/* Signatures of subroutines */
void server(int, char);
void client(char *, int, char);

int main(int argc, char* argv[])
{
	char op = argv[1][0];
	int p, q, send_A, send_B;
	if (op == '+') // Carol has nothing to do in case of addition
		return 1;

	char ip[] = "127.0.0.1";
	std::thread server_thread(server, CAROL_PORT, op); //server to Alice
	sleep(5);
	std::thread client_thread(client, ip, BOB_PORT, op);

	switch (op)
	{
<<<<<<< HEAD
	case 'x': //Carol in multiplication (helper) protocol
		while (client_done != 1 || server_done != 1);
		client_done = 0;
		server_done = 0;
		srand(0);
		q_B = rand() % 1000;
		q_A = p_A * p_B - q_B;
		main_done_1 = 1;

		while (client_done != 1 || server_done != 1);
		client_done = 0;
		server_done = 0;
		q_B = rand() % 1000;
		q_A = p_A * p_B - q_B;
		main_done_2 = 1;
		break;

	case '>':
		while (client_done != 1 || server_done != 1);
		client_done = 0;
		server_done = 0;
		p = p_A + p_B;
		q = q_A + q_B;
		printf("Received %d and %d from Alice\n", p_A, q_A);
		printf("Received %d and %d from Bob\n", p_B, q_B);
		srand(time(NULL));
		send_A = rand() % 1000;
		send_B = p > q ? 1 - send_A : -1 * send_A;
		p_A = send_A;
		p_B = send_B;
		main_done_1 = 1;

		break;
	default:
		printf("Operation not permitted.\n");
		return 1;
=======
		case 'x': //Carol in multiplication (helper) protocol
			while(client_done == 0 ||  server_done == 0);
			client_done = 0;
			server_done = 0;
			srand(0);				
			q_A = rand();
			q_B = p_A * p_B - q_A;
			main_done_1 = 1;

			while(client_done == 0 || server_done == 0);
			client_done = 0;
			server_done = 0;				
			q_A = rand();
			q_B = p_A * p_B - q_A;
			main_done_2 = 1;
			break;

		default:
			printf("Operation not permitted.\n");
			return 1;
>>>>>>> dd2315fd73d15d62a365b69ad63770cae8149c79
	}

	server_thread.join();
	client_thread.join();

	return 1;

}

/*Client of Bob */
void client(char *ip, int portno, char op)
{

	int sockfd, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	struct message* buffer = (struct message *)malloc(sizeof(struct message));
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
<<<<<<< HEAD

	switch (op)
	{
	case 'x':
		/* read b_2 */
		//bzero(buffer, sizeof(struct message));
		n = read(sockfd, buffer, sizeof(struct message));
		printf("Finished reading. Receieved %d from B\n", buffer->scalar1);
		p_B = buffer->scalar1;
		client_done = 1;
		while (!main_done_1);
		buffer->scalar1 = q_B;
		n = write(sockfd, buffer, sizeof(struct message));

		//bzero(buffer, sizeof(struct message));
		n = read(sockfd, buffer, sizeof(struct message));
		p_B = buffer->scalar1;
		client_done = 1;
		while (!main_done_2);
		buffer->scalar1 = q_B;
		n = write(sockfd, buffer, sizeof(struct message));
		break;
	case '>':
		printf("Entered >=\n");
		n = read(sockfd, buffer, sizeof(struct message));
		p_B = buffer->scalar1;
		q_B = buffer->scalar2;
		client_done = 1;
		while (!main_done_1);
		buffer->scalar1 = p_B;
		buffer->scalar2 = 0;
		n = write(sockfd, buffer, sizeof(struct message));
	default:
		return;

=======
	printf("Entering client, with operation %c\n", op);
	switch(op)
	{
		case 'x':
			printf("In multiplication case of Carol\n"); 
			/* read b_2 */
			//bzero(buffer, sizeof(struct message));
			n = read(sockfd,buffer,sizeof(struct message));
			printf("Received %f from Bob\n", buffer->scalar1);
			p_B = buffer->scalar1;
			client_done = 1;
			while(!main_done_1);
			buffer->scalar1 = q_B;
			printf("Sending %f to Bob\n", buffer->scalar1);
			n = write(sockfd, buffer, sizeof(struct message));

			//bzero(buffer, sizeof(struct message));
			n = read(sockfd,buffer,sizeof(struct message));
			printf("Received %f from Bob\n", buffer->scalar1);
			p_B = buffer->scalar1;
			client_done = 1;
			while(!main_done_2);
			buffer->scalar1 = q_B;
			printf("Sending %f to Bob\n", buffer->scalar1);
			n = write(sockfd, buffer, sizeof(struct message));
			break;
			
		default:
			return;
			
>>>>>>> dd2315fd73d15d62a365b69ad63770cae8149c79
	}





	close(sockfd);
}

/*Server to Alice */
void server(int portno, char op)
{
	std::thread t;
	int n;
	int sockfd, newsockfd;
	socklen_t clilen;

	struct sockaddr_in serv_addr, cli_addr;
	struct message* buffer = (struct message *)malloc(sizeof(struct message));
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		printf("ERROR opening socket");

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
		printf("ERROR on binding");

	listen(sockfd, 5);

	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd,
		(struct sockaddr *) &cli_addr,
		&clilen);

	if (newsockfd < 0)
		printf("ERROR on accept");


	switch (op)
	{
<<<<<<< HEAD
	case 'x':
		/* read a_2 */
		//bzero(buffer, sizeof(int));
		n = read(newsockfd, buffer, sizeof(struct message));
		printf("Finished reading. Receieved %d from A\n", buffer->scalar1);
		p_A = buffer->scalar1;
		server_done = 1;
		while (!main_done_1);
		buffer->scalar1 = q_A;
		n = write(newsockfd, buffer, sizeof(message));

		//bzero(buffer, sizeof(int));
		n = read(newsockfd, buffer, sizeof(struct message));
		p_A = buffer->scalar1;
		server_done = 1;
		while (!main_done_2);
		buffer->scalar1 = q_A;
		n = write(newsockfd, buffer, sizeof(struct message));
		break;
	case '>':
		n = read(newsockfd, buffer, sizeof(struct message));
		p_A = buffer->scalar1;
		q_A = buffer->scalar2;
		server_done = 1;
		while (!main_done_1);
		buffer->scalar1 = p_A;
		buffer->scalar2 = 0;
		n = write(newsockfd, buffer, sizeof(struct message));

	default:
		return;
=======
		case 'x': 
			/* read a_2 */
			//bzero(buffer, sizeof(int));
			n = read(newsockfd, buffer,sizeof(struct message));
			printf("Received %f from Alice\n", buffer->scalar1);
			p_A = buffer->scalar1;		
			server_done = 1;	
			while(!main_done_1);
			buffer->scalar1 = q_A;
			printf("Sending %f to Alice\n", buffer->scalar1);
			n = write(newsockfd, buffer, sizeof(struct message));

			//bzero(buffer, sizeof(int));
			n = read(newsockfd, buffer,sizeof(struct message));
			printf("Received %f from Alice\n", buffer->scalar1);
			p_A = buffer->scalar1;		
			server_done = 1;	
			while(!main_done_2);
			buffer->scalar1 = q_A;
			printf("Sending %f to Alice\n", buffer->scalar1);
			n = write(newsockfd, buffer, sizeof(struct message));
			break;

		default:
			return;
>>>>>>> dd2315fd73d15d62a365b69ad63770cae8149c79

	}

	return;
}