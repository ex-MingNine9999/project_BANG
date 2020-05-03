#define NUM_PLAY 5
#define ACARD_MAX 90
#define JCARD_MAX 5
#define CCARD_MAX 5
#define PCARD_MAX 80
#define JCARD 1
#define CCARD 6
#define PCARD 11

#define HANDCARD_MAX 15

#define EXP_MAX 256

#define OUT_RNG -1

#define SUFFLE 0xFFFF

#define BANG 1
#define MISSSHOT 2
#define MACHINGUN 3
#define INDIAN 4
#define COMBAT 5
#define STAGECOACH 6
#define WELLSFARGO 7
#define ROB 8
#define CATBALOU 9
#define BEER 10
#define HOF 11
#define STORE 12
#define WEAPON 100
#define MUSTANG 101
#define SIGHT 102
#define BARREL 103
#define JAIL 104
#define DYNAMITE 105
#define END -1

#define VICTORY 77
#define DEFEAT 66

#define ISBOMBED(a) (2 <= (a) && (a) <= 9)
#define ISESCAPE(a) (((a) - 1) / 13 == 2)

typedef struct _card {
    char num;
    char type;
    char shape;
    char range;
} card;

typedef struct _player {
    char pos;
    char life;
    char status[8];
    char cards[15];
    char event[2];
    char range;
} player;

typedef struct _command {
    char use;
    char target;
    char aim;
} command;

typedef struct _node {
    char num;
    struct _node *prev;
    struct _node *next;
} node;

typedef struct _dList {
    node *head;
    node *tail;
    node *cur;
    int size;
} dList;


void list_init(dList *list);      // 리스트를 초기화 하는 함수
void list_pushBack(dList *list, char num);      // 리스트의 맨뒤에 노드를 추가하는 함수
void list_pushFront(dList *list, char num);
char list_popFront(dList *list);
int list_size(dList *list);
void list_print(dList *list);

void swap(char *a, char *b);
void shuffle(char *deck, int n);

void *thread_main(void *arg);
void clnt_set(const int *clnt_socks);
void game_set(player *p, dList *deck, const int n);
void game_procedure(player *p, dList *deck, int n, int *sd);
int calDis(const char *life, char dis[NUM_PLAY][NUM_PLAY]);         // 거리 측정 함수
int dummyToDeck(dList *deck, char *pdummy, int *ndummy);      // 더미에 있는 카드를 섞어서 덱에 추가
int openCard(dList *list);
int dealing(player *p, dList *deck, int n);         // 카드 받기
//int comHandling(const command com, player *p, const char dis[NUM_PLAY][NUM_PLAY]);
void arrangement(player *p);
int bang(player *a, player *b, const char dis[NUM_PLAY][NUM_PLAY], dList *deck, const int mode);
int combat(player *a, player *b, char *pdummy, int *ndummy);
int robNcat(player *a, player *b, const char dis[NUM_PLAY][NUM_PLAY], const char aim, const int mode);
int victory(player *p, int n);
command socketFunc(int *clnt_socks, int clnt_turn, player *play, char shape);

void print_log(player *p, int o);
