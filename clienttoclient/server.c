#include "header.c"

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;
typedef pthread_t th;

#define LEN 500
#define MAX_CLIENT 10
#define MAXIMUM 1000000


char buf[LEN];
int fd[MAXIMUM];


void init(){

	int i = 0;
	for(i=0;i<MAXIMUM;i++)
		fd[i] = -1;

}

void printonline(){

	int i=0;
	printf("Online friends:\n");

	for(i=0;i<MAXIMUM;i++){
		if(fd[i]!=-1){
			printf("\t %d\n",i);
		}
	}

}


void *threadreceivesend(void *sock){

	int cli = *((int *)sock);
	int r = 0,s = 0,i = 0;
	char temp[LEN];

	while(1){

		r = recv(cli,buf,sizeof(buf),0);

		if(r>0){

			printf("Message Received : %s\n",buf);
			buf[strlen(buf)-1] = '\0';

			if(strcmp(buf,"online")==0){

				strcpy(buf,"\0");
				snprintf(buf,LEN,"Online friends:\n");

				for(i=0;i<MAXIMUM;i++){
					if(fd[i]!=-1){
						snprintf(temp,LEN,"%d ",i);
						strcat(buf,temp);
					}
				}

				send(cli,buf,sizeof(buf),0);
				continue;

			}
			
			if(strcmp(buf,"mineto")==0){

				strcpy(buf,"\0");
				snprintf(buf,2,"%d",cli);
				send(cli,buf,sizeof(buf),0);
				continue;

			}

			if(fd[(int)(buf[0]-'0')]!=-1){

				s = send((int)(buf[0]-'0'),buf,sizeof(buf),0);
				
				if(s<0){
					strcpy(temp,"\0");
					snprintf(temp,LEN,"Invalid client. Refresh to check online clients OR Invalid codeword");
					send((int)(buf[0]-'0'),temp,sizeof(temp),0);
					fd[(int)(buf[0]-'0')] = -1;
				}

			}

			else{

				strcpy(temp,"\0");
				snprintf(temp,LEN,"Invalid client. Refresh to check online clients OR Invalid codeword");
				send(cli,temp,sizeof(temp),0);

			}
		}

		else{

			fd[cli] = -1;
			printf("Client %d got disconnected.\n", cli);
			break;

		}
	}
}

void *threadconnection(void *sock){

	int sock1 = *((int *)sock),i = 0;
	int cli,l = sizeof(sockaddr_in);
	th arr[LEN];
	sockaddr_in client;

	while(1){

		if((cli = accept(sock1,(sockaddr *)&client,&l))==-1){
			perror("accept error");
			exit(-1);
		}

		else{
			printf("New client got connected. Client id = %d\n",cli);
		}

		fd[cli] = 1;
		pthread_create(&arr[i++],NULL,threadreceivesend,&cli);
	}
}



int main(int argc, char const *argv[])
{

	int sock1,cli,l,pid;
	sockaddr_in server,client;
	th thread1,thread2,thread3;
	init();
	

	if((sock1=socket(AF_INET,SOCK_STREAM,0))==-1){
		perror("Socket couldn't be created");
		exit(-1);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(4000);
	server.sin_addr.s_addr = INADDR_ANY;
	bzero(&server.sin_zero,8);

	l = sizeof(sockaddr_in);

	if(bind(sock1,(sockaddr *)&server,l)==-1){
		perror("Error in binding");
		exit(-1);
	}

	if(listen(sock1,MAX_CLIENT)==-1){
		perror("listen");
		exit(-1);
	}

	pthread_create(&thread1,NULL,threadconnection,&sock1);
	printf("This is server side\n");
	pthread_join(thread1,NULL);
	close(sock1);
	return 0;
}