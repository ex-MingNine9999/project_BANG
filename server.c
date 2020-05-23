#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <string.h>
#include "plz.h"
#include "information.h"

command socketFunc(int *clnt_socks, int clnt_turn, player *play, char shape);
INFORMATION inform[20];
int serv_sock;
char name[PLAYER_NUM][10];
int info_num = 0;
card c[ACARD_MAX];

int main(int argc, char *argv[])
{
	int clnt_socks[ROOM_NUM][PLAYER_NUM];
	int room_num = 0;
	pthread_t t_id[ROOM_NUM];
	struct sockaddr_in serv_adr;
	int fd, i;
	
	if(argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	if ((fd = open("card.txt", O_RDONLY)) < 0) {
		fprintf(stderr, "open error for card.txt\n");
		exit(1);
	}

	for (int i = 0; i < 91; i++) {
		read(fd, (void *)&c[i], sizeof(card));
	}
	close(fd);

	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");

	memset(clnt_socks, 0, sizeof(int)*ROOM_NUM*PLAYER_NUM);
	memset(inform, 0, sizeof(INFORMATION)*100);
	receive_information();
	while(room_num < ROOM_NUM)
	{
		//write(1, "good\n", 6);
		pthread_create(&t_id[room_num], NULL, waiting_room, (void *)(clnt_socks+room_num));
		pthread_join(t_id[room_num], NULL);
		//write(1, "good2\n", 6);
		pthread_create(&t_id[room_num], NULL, thread_main, (void *)(clnt_socks[room_num]));
		if(room_num == ROOM_NUM-1)
			pthread_join(t_id[room_num], NULL);
		else
			pthread_detach(t_id[room_num]);
		//write(1, "good3\n", 6);
		room_num++;
	}
	close(serv_sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void *waiting_room(void *arg)
{
	int clnt_sock;
	int *clnt_socks = (int *)arg;
	int clnt_cnt = 0;
	int end = 0;
	struct sockaddr_in clnt_adr;
	socklen_t adr_sz;
	int str_len, i, j;
	char buf[BUF_SIZE];

	struct epoll_event *ep_events;
	struct epoll_event event;
	int epfd, event_cnt;
	epfd=epoll_create(PLAYER_NUM+1);
	ep_events=malloc(sizeof(struct epoll_event)*(PLAYER_NUM+1));


	event.events=EPOLLIN;
	event.data.fd=serv_sock;	
	epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

	while(1)
	{
		if(end == 1)
			break;
		event_cnt=epoll_wait(epfd, ep_events, PLAYER_NUM+1, -1);
		if(event_cnt==-1)
		{
			puts("epoll_wait() error");
			break;
		}
		for(i=0; i<event_cnt; i++)
		{
			if(ep_events[i].data.fd==serv_sock && clnt_cnt != PLAYER_NUM)
			{
				adr_sz=sizeof(clnt_adr);
				clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
				event.events=EPOLLIN;
				event.data.fd=clnt_sock;
				epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
				printf("connected client: %d ", clnt_sock);
				clnt_socks[clnt_cnt++] = clnt_sock;
			}
			else
			{
				memset(buf, 0, sizeof(buf));
				str_len=read(ep_events[i].data.fd, buf, BUF_SIZE);
				if(str_len==0)    // close request!
				{
					epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
					close(ep_events[i].data.fd);
					printf("closed client: %d \n", ep_events[i].data.fd);
					for(j = 0; j < clnt_cnt; j++)
					{
						if(ep_events[i].data.fd==clnt_socks[j])
						{
							while(j++<clnt_cnt-1)
							{
								clnt_socks[j] = clnt_socks[j+1];
								strcpy(name[j], name[j+1]);
							}
							break;
						}
					}
					clnt_cnt--;
				}
				else
				{
					if(buf[0] == 'R')
						show_information(ep_events[i].data.fd);
					else if(buf[0] == 'L')
					{
						strcpy(name[clnt_cnt-1], buf+1);
						compare_information(buf+1);
					}
				}
				if(clnt_cnt == PLAYER_NUM)
				{
					end = 1;
				}
			}
		}
	}
	close(epfd);
}

command socketFunc(int *clnt_socks, int clnt_turn, player *play, char shape)
{
	char buf[BUF_SIZE] = {0};
	struct epoll_event *ep_events;
	struct epoll_event event;
	int epfd, event_cnt;
	int str_len, i, j;
	int clnt_cnt = PLAYER_NUM;
	clock_t start,end;
	double result;
	command ret;

	buf[0] = 'C';
	memcpy(buf+1, name[clnt_turn], 10);
	strncat(buf+11, "It's MY TURN", 12);
	//sprintf(buf, "C%-10s It's MY TURN", name[clnt_turn]);
	for (i = 0; i < PLAYER_NUM; i++) {
		write(clnt_socks[i], buf, BUF_SIZE);
	}
	
	memset(buf, 0, sizeof(buf));
	epfd=epoll_create(PLAYER_NUM);
	ep_events=malloc(sizeof(struct epoll_event)*PLAYER_NUM);
	event.events=EPOLLIN;
	send_gamedata(play, clnt_socks, shape);
	if(clnt_turn == -1)
		return ret;
	for(i = 0; i < PLAYER_NUM; i++)
	{
		event.data.fd=clnt_socks[i];
		epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_socks[i], &event);
	}
	start = clock();
	while(1)
	{
		event_cnt=epoll_wait(epfd, ep_events, PLAYER_NUM, 1000000);
		if(event_cnt==-1)
		{
			puts("epoll_wait() error");
			break;
		}


		for(i=0; i<event_cnt; i++)
		{
			memset(buf, 0, sizeof(buf));
			str_len=read(ep_events[i].data.fd, buf, BUF_SIZE);
			if(str_len==0)    // close request!
			{
				epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
				close(ep_events[i].data.fd);
			}
			else if(buf[0] == 'C')
			{
				for(j = 0; j < clnt_cnt; j++)
					write(clnt_socks[j], buf, BUF_SIZE);
			}
			else if(ep_events[i].data.fd == clnt_socks[clnt_turn])
			{
				ret.use = buf[1];
				ret.target = buf[2];
				ret.aim = buf[3];
				printf("%d\n", buf[1]);
				printf("%d\n", buf[2]);
				printf("%d\n", buf[3]);
				close(epfd);
				free(ep_events);
				for (int k = 0; k < clnt_cnt; k++){
					if (play[k].status[1] == ret.target) {
						ret.target = k;
						break;
					}
				}
				return ret;
			}
		}
		end = clock();
		result = (double)(end - start)/CLOCKS_PER_SEC;
		if((int)result > 1000 || event_cnt == 0)
		{
			play[clnt_turn].life = 0;
			ret.use = -1;
			return ret;
		}
	}
}

void send_gamedata(player *p, int *clnt_socks, char dummy)
{
	char play_inform[PLAYER_NUM][BUF_SIZE];
	int i, j;
	memset(play_inform, 0, sizeof(play_inform));
	for(i = 0; i < PLAYER_NUM; i++)
	{
		play_inform[i][0] = 'G';
		for(j = 0; j < PLAYER_NUM; j++)
		{
			memcpy(play_inform[i] + 28 * j + 1, (void *)&p[(i+j)%PLAYER_NUM], 28);
		}
		play_inform[i][28+28*(j-1)+1] = dummy;
		play_inform[i][28+28*(j-1)+2] = c[dummy].shape;
		write(clnt_socks[i], play_inform[i], BUF_SIZE);
	}
}

void compare_information(char* message) // After Comparing user information, no -> add, yes -> do nothing
{
	int exist = 0;
	int i;
	for(i = 0; i < 20; i++)
	{
		if(inform[i].id[0] == 0)
			break;
		printf("!~%d~!\n", strncmp(inform[i].id, message, strlen(message)));
		if(strncmp(inform[i].id, message, strlen(message)) == 0)
		{
			exist = 1;
			break;
		}
	}
	if(exist == 0)
	{
		memcpy(inform[i].id, message, 10);
		inform[i].score = 0;
	}
}

void show_information(int clnt_sock) // send user informations to clnt_sock
{
	int i, j;
	char buf[BUF_SIZE];
	for(i = 0; i < 20; i++)
	{
		if(inform[i].id[0] == 0)
			break;
	}
	for(j = 0; j < i; j++)
	{
		memset(buf, 0, sizeof(buf));
		if(j < i-1)
			buf[0] = 'R';
		else
			buf[0] = 'r';
		memcpy(buf+1, inform[j].id, 10);
		memcpy(buf+11, &inform[j].score, sizeof(int));
		write(clnt_sock, buf, BUF_SIZE);
	}
}

void score_update(player *p, int winner)
{
	FILE *fp;
	char buf[14];
	int i;
	int j;
	write(1, "1", 1);
	if(winner == 1)
	{
		   /*for(i = 0; i < PLAYER_NUM; i++)
		   {
		   if(p[i].status[0] == 1)
		   break;
		   }
		   for(j = 0; j < 20; j++)
		   {
		   if(strcmp(name[i], inform[j].id) == 0)
		   inform[j].score += 5;
		   }
		   for(i = 0; i < PLAYER_NUM; i++)
		   {
		   if(p[i].status[0] == 2)
		   break;
		   }
		   for(j = 0; j < 20; j++)
		   {
		   if(strcmp(name[i], inform[j].id) == 0)
		   inform[j].score += 5;
		   }*/

		for (i = 0; i < PLAYER_NUM; i++) {
			if (p[i].status[0] < 3) {
				for (j = 0; j < 20; j++) { 
					if (strcmp(name[i], inform[j].id) == 0){
						inform[j].score += 5;
					}
				}
			}
		}	

	}
	else if(winner == 2)
	{
		/*for(i = 0; i < PLAYER_NUM; i++)
		{
			if(p[i].status[0] == 4)
				break;
		}
		for(j = 0; j < 20; j++)
		{
			if(strcmp(name[i], inform[j].id) == 0)
				inform[j].score += 7;
		}
		for(i = 0; i < PLAYER_NUM; i++)
		{
			if(p[i].status[0] == 5)
				break;
		}
		for(j = 0; j < 20; j++)
		{
			if(strcmp(name[i], inform[j].id) == 0)
				inform[j].score += 7;
		}*/
for (i = 0; i < PLAYER_NUM; i++) {
			if (p[i].status[0] > 3) {
				for (j = 0; j < 20; j++) { 
					if (strcmp(name[i], inform[j].id) == 0){
						inform[j].score += 7;
					}
				}
			}
		}	

	}
	else
	{
		/*for(i = 0; i < PLAYER_NUM; i++)
		{
			if(p[i].status[0] == 3)
				break;
		}
		for(j = 0; j < 20; j++)
		{
			if(strcmp(name[i], inform[j].id) == 0)
				inform[j].score += 10;
		}*/
for (i = 0; i < PLAYER_NUM; i++) {
			if (p[i].status[0] == 3) {
				for (j = 0; j < 20; j++) { 
					if (strcmp(name[i], inform[j].id) == 0){
						inform[j].score += 10;
					}
				}
			}
		}	

	}
	fp = fopen("information.txt", "w+");
	for(i = 0; i < 20; i++)
	{
		memset(buf, 0, sizeof(buf));
		if(inform[i].id[0] == 0)
			break;
		memcpy(buf, inform[i].id, 10);
		memcpy(buf+10, &inform[i].score, sizeof(int));
		fwrite(buf, 1, sizeof(buf), fp);
		write(1, "for", 3);
	}
	fclose(fp);
}

void receive_information() // From DB reading user informations
{
	FILE *fp;
	char buf[14];
	int size;
	if((fp = fopen("information.txt", "r+")) == NULL)
		return;
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	for(int i = 0; i < (size/14); i++)
	{
		memset(buf, 0, sizeof(buf));
		fread(buf, 1, sizeof(buf), fp);
		memcpy(inform[i].id, buf, 10);
		memcpy(&inform[i].score, buf+10, sizeof(int));
	}
	fclose(fp);
}

void clnt_set(const int *clnt_socks)
{
	char buf[BUF_SIZE];
	int i, j;

	memset(buf, 0, sizeof(buf));
	buf[0] = 'S';
	for(i = 0; i < PLAYER_NUM; i++)
		write(clnt_socks[i], buf, BUF_SIZE);
	memset(buf, 0, sizeof(buf));
	buf[0] = 'N';
	for(i = 0; i < PLAYER_NUM; i++)
	{
		for(j = 0; j < PLAYER_NUM; j++)
			strcat(buf+1+10*j, name[(i+j)%PLAYER_NUM]);
		write(clnt_socks[i], buf, BUF_SIZE);
		memset(buf+1, 0, sizeof(buf)-1);
	}
}
