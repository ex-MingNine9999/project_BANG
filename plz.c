#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include "plz.h"
#include "information.h"

extern card c[ACARD_MAX];

/*
   int main(void)
   {
   pthread_t pid;
   int sd[5] = {1, 1, 1, 1, 1};
   pthread_create(&pid, NULL, thread_main, (void *)sd);
   pthread_join(pid, NULL);

   exit(0);
   }
   */

void swap(char *a, char *b)
{
	char t = *a;
	*a = *b;
	*b = t;
}

void shuffle(char *deck, int n)
{
	srand(time(NULL));
	for (int i = 0; i < SUFFLE; i++) {
		swap(&deck[i % n], &deck[rand() % n]);
	}
}

void *thread_main(void *arg)
{
	int n = 0;            // 게임 인원 수
	int *sd = (int *)arg;
	player p[NUM_PLAY] = {0};
	dList *deck = (dList *)malloc(sizeof(dList));

	for (int i = 0; i < NUM_PLAY; i++) {
		if (sd[i] > 0) {
			n++;
		}
	}

	clnt_set(sd);
	game_set(p, deck, n);
	game_procedure(p, deck, n, sd);

	return NULL;
}

void game_set(player *p, dList *deck, const int n)
{
	char jdeck[JCARD_MAX];
	char cdeck[CCARD_MAX];
	char pdeck[PCARD_MAX];

	for (int i = 0; i < JCARD_MAX; i++) {
		jdeck[i] = (char)JCARD + i;
	}
	shuffle(jdeck, JCARD_MAX);            // 직업 카드

	for (int i = 0; i < CCARD_MAX; i++) {
		cdeck[i] = (char)CCARD + i;
	}
	shuffle(cdeck, CCARD_MAX);            // 캐릭터 카드

	for (int i = 0; i < n; i++) {
		p[i].pos = i;
		p[i].status[0] = jdeck[i];
		p[i].status[1] = cdeck[i];
		p[i].life = c[cdeck[i]].shape;
		if (jdeck[i] == 1) {
			p[i].life++;
		}
	}

	list_init(deck);
	for (int i = 0; i < PCARD_MAX; i++) {
		pdeck[i] = (char)PCARD + i;
	}
	shuffle(pdeck, PCARD_MAX);            // 플레이 카드
	for (int i = 0; i < PCARD_MAX; i++) {
		list_pushBack(deck, pdeck[i]);
	}

}

void game_procedure(player *p, dList *deck, int n, int *sd)
{
	char pdummy[PCARD_MAX] = {0};
	int ndummy = 0;
	char life[NUM_PLAY] = {0};
	char dis[NUM_PLAY][NUM_PLAY] = {0};
	int o, r, v = 0;
	int check;
	int checkDeal = 0, checkBang = 0, checkDyna = 0;
	int ret, i, j;
	command com;
	card using;

	memset(pdummy, 0xFF, sizeof(pdummy));
	for (i = 0; i < n; i++) {
		life[i] = p[i].life;
		dealing(&p[i], deck, p[i].life);

		if (p[i].status[0] == 1) {         // 첫 번째 순서는 보안관
			o = i;
		}
	}
	calDis(life, dis);

	while (1) {
		if (checkDeal == 0) {
			dealing(&p[o], deck, 2);               // 턴이 시작할때 카드 2장 받기

			checkDeal = 1;
			ret = 0;
			check = 0;
			checkBang = 0;
			continue;
		}
		arrangement(p);

		switch (v) {
			case 0 :
				arrangement(p);
				break;
			case 1 :
				for (i = 0; i < n; i++) {
					if (p[i].status[0] < 3) {
						p[i].event[0] = VICTORY;
					}
					else {
						p[i].event[0] = DEFEAT;
					}
				}
				break;
			case 2 :
				for (i = 0; i < n; i++) {
					if (p[i].status[0] > 3) {
						p[i].event[0] = VICTORY;
					}
					else {
						p[i].event[0] = DEFEAT;
					}
				}
				break;
			case 3 :
				for (i = 0; i < n; i++) {
					if (p[i].status[0] == 3) {
						p[i].event[0] = VICTORY;
					}
					else {
						p[i].event[0] = DEFEAT;
					}
				}
				break;
		}

		print_log(p, o);

		if (v) {
			score_update(p, v);
			o = -1;
		}
		// command 구조체 wait
		com = socketFunc(sd, o, p, ndummy > 0 ? pdummy[ndummy - 1] : 0);

		if (v) {
			return;
		}

		for (i = 0; i < n; i++) {
			p[i].event[0] = 0;
			p[i].event[1] = 0;
		}

		if (com.use == -1) {
			checkDeal = 0;
			checkBang = 0;
			do {
				o = (o + 1) % n;
			} while (p[o].life == 0);

			if (checkDyna) {
				checkDyna = openCard(deck);
				if (ISBOMBED(c[checkDyna].shape)) {
					p[o].life -= 3;
					pdummy[ndummy++] = p[o].status[7];
					p[o].status[7] = 0;

					for (i = 0; i < NUM_PLAY; i++) {
						p[i].event[0] = DYNAMITE;
					}

					checkDyna = 0;
					if (p[o].life <= 0){
						p[o].life = 0;
						life[o] = 0;
					}

					while (p[o].life == 0) {
						o = (o + 1) % n;
					}
				}
				else {
					p[(o + 1) % n].status[7] = p[o].status[7];
					p[o].status[7] = 0;
				}
				pdummy[ndummy++] = checkDyna;
			}

			if (p[o].status[6]) {
				check = openCard(deck);
				pdummy[ndummy++] = p[o].status[6];
				p[o].status[6] = 0;

				if (!ISESCAPE(c[check].shape)) {
					do {
						o = (o + 1) % n;
					} while (p[o].life == 0);
				}
				pdummy[ndummy++] = check;
			}

			continue;
		}

		if (com.target == -1) {         // 카드 버리기
			for (i = 0; i < 15; i++) {
				if (p[o].cards[i] == com.use) {
					p[o].cards[i] = 0;
					break;
				}
			}

			pdummy[ndummy++] = com.use;
			continue;
		}

		using = c[com.use];
		printf("%d %d %d\n", using.num, using.type, using.shape);
		//test
		//using.num = com.use;
		//using.type = DYNAMITE;
		//test
		switch (using.type) {
			case BANG :
				if (checkBang) {
					p[o].event[0] = -1;
					continue;
				}

				ret = bang(&p[o], &p[com.target], dis, deck, using.type);
				if (ret == -1) {
					p[o].event[0] = -1;
					continue;
				}
				checkBang++;

				if (p[o].status[2]) {            // 볼케닉 일때 확인
					if (c[p[o].status[2]].range == 1) {
						checkBang = 0;
					}
				}
				if (c[p[o].status[1]].type == 3) {
					checkBang = 0;
				}
				break;
			case MACHINGUN :
				for (i = 0; i < NUM_PLAY; i++) {
					if (i != o && life[i] != 0) {
						ret = bang(&p[o], &p[i], dis, deck, using.type);

						if (ret >= 10) {
							pdummy[ndummy++] = ret;
						}
					}
				}

				ret = 0;
				break;
			case INDIAN :
				for (i = 0; i < NUM_PLAY; i++) {
					if (i != o && life[i] != 0) {
						ret = bang(&p[o], &p[i], dis, deck, using.type);

						if (ret >= 10) {
							pdummy[ndummy++] = ret;
						}
					}
				}

				ret = 0;
				break;
			case COMBAT :
				combat(&p[o], &p[com.target], pdummy, &ndummy);
				break;
			case STAGECOACH :
				dealing(&p[o], deck, 2);
				break;
			case WELLSFARGO :
				dealing(&p[o], deck, 3);
				break;
			case ROB :
				ret = robNcat(&p[o], &p[com.target], dis, com.aim, ROB);
				if (ret == -1) {
					p[o].event[0] = -1;
					continue;
				}
				break;
			case CATBALOU :
				ret = robNcat(&p[o], &p[com.target], dis, com.aim, CATBALOU);
				if (ret >= 10) {
					pdummy[ndummy++] = ret;
				}
				break;
			case BEER :
				if (p[o].life == life[o]) {
					ret = -1;
					p[o].event[0] = -1;
					continue;
				}
				else {
					ret = 0;
					p[o].life++;
				}
				break;
			case HOF :
				for (i = 0; i < NUM_PLAY; i++) {
					if (p[i].life < life[i] && life[i]) {
						p[i].life++;
					}
				}
				break;
			case STORE :
				for (i = 0; i < NUM_PLAY; i++) {
					r = (o + i) % n;

					if (life[r]) {
						dealing(&p[r], deck, 1);
					}
				}
				break;
			case WEAPON :
				if (p[o].status[2]) {
					pdummy[ndummy++] = p[o].status[2];
				}
				p[o].status[2] = com.use;
				break;
			case BARREL :
				if (p[o].status[3]) {
					pdummy[ndummy++] = p[o].status[3];
				}
				p[o].status[3] = com.use;
				break;
			case MUSTANG :
				if (p[o].status[4]) {
					check = -1;
				}
				p[o].status[4] = com.use;
				break;
			case SIGHT :
				if (p[o].status[5]) {
					check = -1;
				}
				p[o].status[5] = com.use;
				break;
			case JAIL :
				if (p[com.target].status[6]) {
					check = -1;
				}
				p[com.target].status[6] = com.use;
				break;
			case DYNAMITE :
				p[o].status[7] = com.use;
				checkDyna = openCard(deck);
				if (ISBOMBED(checkDyna)) {
					p[o].life -= 3;

					pdummy[ndummy++] = p[o].status[7];
					p[o].status[7] = 0;

					checkDyna = 0;
				}
				else {
					p[(o + 1) % n].status[7] = p[o].status[7];
					p[o].status[7] = 0;
				}

				break;
			default :
				fprintf(stderr, "com error\n");
		}

		if (using.type < 100 && check == 0) {
			pdummy[ndummy++] = using.num;

			for (i = 0; i < NUM_PLAY; i++) {
				p[i].event[0] = using.type;
			}
		}

		if (ret >= 10) {
			pdummy[ndummy++] = ret;
		}

		for (i = 0; i < 15; i++) {
			if (p[o].cards[i] == using.num) {
				p[o].cards[i] = 0;
				break;
			}
		}

		for (i = 0; i < n; i++) {
			if (life[i] && p[i].life <= 0) {
				p[i].life = 0;
				life[i] = 0;
				v = victory(p, n);
				calDis(life, dis);
			}
		}

		if (deck->size < 10) {
			dummyToDeck(deck, pdummy, &ndummy);
		}
	}
}

int calDis(const char *life, char dis[NUM_PLAY][NUM_PLAY])
{
	char r, l;
	int n = 0;

	for (int i = 0; i < NUM_PLAY; i++) {
		n += (life[i]) ? 1 : 0;
	}

	for (int a = 0; a < NUM_PLAY; a++) {
		if (life[a]) {
			for (int b = a + 1; b < NUM_PLAY; b++) {
				if (life[b]) {
					r = l = 0;
					for (int i = a; i != b; i++) {
						if (i == NUM_PLAY) {
							i = -1;
							continue;
						}
						else if (life[i] == 0) {
							continue;
						}

						l++;
					}

					for (int i = a; i != b; i--) {
						if (i == -1) {
							i = NUM_PLAY;
							continue;
						}
						else if (life[i] == 0) {
							continue;
						}

						r++;
					}

					dis[a][b] = l < r ? l : r;
					dis[b][a] = dis[a][b];

				}
			}
		}
	}

	return 0;
}

int dummyToDeck(dList *deck, char *pdummy, int *ndummy)
{
	int n = *ndummy - 1;
	node back;

	shuffle(pdummy, n);

	for (int i = 0; i < n; i++) {
		list_pushBack(deck, pdummy[i]);
		pdummy[i] = -1;
	}
	pdummy[0] = pdummy[n];
	pdummy[n] = -1;

	*ndummy = 1;

	return n;
}

int openCard(dList *deck)
{
	return (int)list_popFront(deck);
}

int dealing(player *p, dList *deck, int n)
{
	int num = strlen(p->cards);

	for (int i = 0; i < n; i++) {
		p->cards[num + i] = list_popFront(deck);
	}

	return n;
}

void arrangement(player *p)
{
	int i, j, k;
	char tmp;

	for (i = 0; i < NUM_PLAY; i++) {
		for (j = 0; j < 15; j++) {
			if (p[i].cards[j] == 0) {
				for (k = j + 1; k < 15; k++) {
					p[i].cards[k - 1] = p[i].cards[k];
				}
				p[i].cards[k - 1] = 0;
			}
		}

		for (j = 1; j < 15; j++) {
			if (p[i].cards[j] == 0) {
				break;
			}
			tmp = p[i].cards[j];
			for (k = j - 1; k >= 0; k--) {
				if (p[i].cards[k] < tmp) {
					break;
				}
				p[i].cards[k + 1] = p[i].cards[k];
			}
			p[i].cards[k + 1] = tmp;
		}
	}
}

int bang(player *a, player *b, const char dis[NUM_PLAY][NUM_PLAY], dList *deck, const int mode)
{
	int miss;
	int card;
	int range;
	int check = 0;

	switch (mode) {
		case BANG :
			if (a->status[2]) {
				range = c[a->status[2]].range;
			}
			else {
				range = 1;
			}

			if (c[a->status[1]].type == 2) {
				range++;
			}
			if (c[b->status[1]].type == 1) {
				range--;
			}

			if (a->status[5]) {
				range++;
			}
			if (b->status[4]) {
				range--;
			}

			if (dis[a->pos][b->pos] > range) {
				return OUT_RNG;
			}
			miss = MISSSHOT;
			break;
		case MACHINGUN :
			miss = MISSSHOT;
			break;
		case INDIAN :
			miss = BANG;
			break;
		default :
			fprintf(stderr, "??\n");
	}

	if (miss == MISSSHOT) {
		if (b->status[3]) {
			card = openCard(deck);

			if (ISESCAPE(c[card].shape)) {
				return card;
			}
		}

		if (c[b->status[1]].type == 4) {
			card = openCard(deck);

			if (ISESCAPE(c[card].shape)) {
				return card;
			}
		}
	}

	for (int i = 0; b->cards[i] != 0; i++) {
		card = b->cards[i];

		if (c[card].type == miss) {
			b->cards[i] = 0;
			check = 1;
			return card;
		}
	}

	if (check == 0) {
		b->life--;
	}

	return 0;
}

int combat(player *a, player *b, char *pdummy, int *ndummy)
{
	int miss = BANG;
	int flag = 0;

	while (flag == 0) {
		for (int i = 0; i < HANDCARD_MAX; i++) {
			if (c[b->cards[i]].type == miss) {
				pdummy[(*ndummy)++] = b->cards[i];
				b->cards[i] = 0;
				flag = 1;
			}
		}

		if (flag == 0) {
			break;
		}

		for (int i = 0; i < HANDCARD_MAX; i++) {
			if (c[a->cards[i]].type == miss) {
				pdummy[(*ndummy)++] = a->cards[i];
				a->cards[i] = 0;
				flag = 0;
			}
		}
	}

	if (flag == 0) {
		b->life--;
	}
	else {
		a->life--;
	}

	return 1;
}

int robNcat(player *a, player *b, const char dis[NUM_PLAY][NUM_PLAY], const char aim, const int mode)
{
	int i, card = 0;
	char range = 1;

	if (mode == ROB) {
		if (c[a->status[1]].type == 2) {
			range++;
		}
		if (c[b->status[1]].type == 1) {
			range--;
		}

		if (a->status[5]) {
			range++;
		}
		if (b->status[4]) {
			range--;
		}

		if (dis[a->pos][b->pos] > range) {
			return OUT_RNG;
		}
	}

	for (i = 0; b->cards[i] != 0; i++) {
		if (b->cards[i] == aim) {
			b->cards[i] = 0;
			card = i;
			break;
		}
	}

	if (card == 0) {
		for (i = 0; i < 8; i++) {
			if (b->status[i] == aim) {
				b->status[i] = 0;
				card = i;
				break;
			}
		}
	}


	if (mode == ROB) {
		a->cards[strlen(a->cards)] = aim;
	}

	return card;
}

int victory(player *p, int n)
{
	int i;
	int chiff = 0, outlaw = 0, betray = 0;

	for (i = 0; i < n; i++) {
		if (p[i].life) {
			if (p[i].status[0] == 1) {
				chiff++;
			}
			else if (p[i].status[0] > 3) {
				outlaw++;
			}
			else if (p[i].status[0] == 3) {
				betray++;
			}
		}
	}

	if (chiff == 0) {
		if (outlaw == 0) {
			return 3;
		}
		else {
			return 2;
		}
	}
	else {
		if (outlaw == 0 && betray == 0) {
			return 1;
		}
	}

	return 0;
}

void print_log(player *p, int o) 
{
	int r;

	for (int i = 0; i < n; i++) {
		r = (o + i) % n;
		printf("\npos : %d\tlife : %d\tcharacter : %d\n"
				"weapon : %3d barrel : %3d mustang : %3d sight : %3d jail : %3d dynamite : %3d\ncards : "
				, p[r].pos, p[r].life, p[r].status[0], p[r].status[1]
				, p[r].status[2], p[r].status[3], p[r].status[4], p[r].status[5], p[r].status[6], p[r].status[7]);
		for (int j = 0; p[r].cards[j] != 0; j++) {
			printf("%3d ", p[r].cards[j]);
		}

		print("\nevent : %3d\n", p[r].event[0]);
	}
}
