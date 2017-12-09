/* ------------ carol -------------*/

#include "includes.h"

#define ALICE_PORT 1111
#define CAROL_PORT 2222
#define BOB_PORT 3333
using namespace std;
/* structure of message that is sent and received */

typedef struct message
{
	/*int P[100][5];
	int D[100];
	int len; */
	int scalar1;
	int scalar2;
	char src;
	char dest;
	int state;
	
}message;

/* Variables for syncing threads */
float state_C = 0.0;
int client_done = 0, server_done = 0, main_done_1 = 0, main_done_2 = 0;
int p_A, p_B, q_A, q_B;

/* Signatures of subroutines */
void server(int , char );
void client(char *, int , char );

int main(int argc, char* argv[])
{
	char op = argv[1][0];
	if(op =='+') // Carol has nothing to do in case of addition
		return 1;
	
	char ip[] = "127.0.0.1";
	std::thread server_thread(server, CAROL_PORT, op); //server to Alice
	sleep(5);	
	std::thread client_thread(client, ip, BOB_PORT, op);

	switch(op)
	{
		case '*': //Carol in multiplication (helper) protocol
			while(client_done != 1 && server_done != 1);
			client_done = 0;
			server_done = 0;
			srand(0);				
			q_A = rand();
			q_B = p_A * p_B - q_A;
			main_done_1 = 1;

			while(client_done != 1 && server_done != 1);
			client_done = 0;
			server_done = 0;				
			q_A = rand();
			q_B = p_A * p_B - q_A;
			main_done_2 = 1;
			break;

		default:
			printf("Operation not permitted.\n");
			return 1;
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
		fprintf(stderr,"ERROR, no such host\n");
		//exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, 
			(char *)&serv_addr.sin_addr.s_addr,
			server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		printf("ERROR connecting");

	switch(op)
	{
		case '*': 
			/* read b_2 */
			//bzero(buffer, sizeof(struct message));
			n = read(sockfd,buffer,sizeof(struct message));
			p_B = buffer->scalar1;
			client_done = 1;
			while(!main_done_1);
			buffer->scalar1 = q_B;
			n = write(sockfd, buffer, sizeof(struct message));

			//bzero(buffer, sizeof(struct message));
			n = read(sockfd,buffer,sizeof(struct message));
			p_B = buffer->scalar1;
			client_done = 1;
			while(!main_done_2);
			buffer->scalar1 = q_B;
			n = write(sockfd, buffer, sizeof(struct message));
			break;
			
		default:
			return;
			
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
	bzero((char *) &serv_addr, sizeof(serv_addr));
	//portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
				sizeof(serv_addr)) < 0) 
		printf("ERROR on binding");

	listen(sockfd,5);

	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd,
			(struct sockaddr *) &cli_addr, 
			&clilen);
		
	if (newsockfd < 0) 
		printf("ERROR on accept");


	switch(op)
	{
		case '*': 
			/* read a_2 */
			//bzero(buffer, sizeof(int));
			n = read(sockfd,buffer,sizeof(int));
			p_A = buffer->scalar1;		
			server_done = 1;	
			while(!main_done_1);
			n = write(sockfd, &q_A, sizeof(int));

			//bzero(buffer, sizeof(int));
			n = read(sockfd, buffer,sizeof(int));
			p_A = buffer->scalar1;		
			server_done = 1;	
			while(!main_done_2);
			n = write(sockfd, &q_A, sizeof(int));
			break;

		default:
			return;

	}

	return; 
}
