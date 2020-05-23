#ifndef _INFORMATION_H_
#define _INFORMATION_H_

#define BUF_SIZE 256
#define ROOM_NUM 20
#define PLAYER_NUM NUM_PLAY
#define INFORMATION_NUM 20

typedef struct _Information
{
	char id[10];
	int score;
} INFORMATION;

void error_handling(char *message);
void *waiting_room(void *arg);
void receive_information();
void compare_information(char *buf);
void show_information(int clnt_sock);
void send_gamedata(player *p, int *clnt_socks, char shape);
void score_update(player *p, int winner);
void update_scoreDB();
void clnt_set(const int *clnt_socks);
#endif
