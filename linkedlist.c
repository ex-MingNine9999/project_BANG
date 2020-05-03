#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "plz.h"

void list_init(dList *list)		// 리스트를 초기화 하는 함수
{
	list->tail = (node *)malloc(sizeof(node));
	list->tail->num = -1;
	list->tail->next = NULL;
	list->tail->prev = NULL;
	list->head = list->cur = list->tail;
	list->size = 0;
}

void list_pushBack(dList *list, char num)		// 리스트의 맨뒤에 노드를 추가하는 함수 
{
	node *new = (node *)malloc(sizeof(node));
	new->num = num;
	new->next = NULL;
	list->tail->next = new;

	list->cur = list->tail;
	list->tail = list->tail->next;
	list->tail->prev = list->cur;

	list->size++;
}

void list_pushFront(dList *list, char num)
{
	node *new = (node *)malloc(sizeof(node));
	new->num = num;
	
	new->next = list->head->next;
	new->prev = list->head;
	list->head->next = new;
	new->next->prev = new;
	
	list->size++;
}

char list_popFront(dList *list)
{
	char front;

	list->cur = list->head->next;
	front = list->cur->num;

	list->cur->prev->next = list->cur->next;
	list->cur->next->prev = list->cur->prev;

	free(list->cur);

	list->size--;

	return front;
}

int list_size(dList *list)
{
	return list->size;
}

void list_print(dList *list)
{
	list->cur = list->head->next;

	while (list->cur != NULL) {
		printf("%d ", list->cur->num);

		list->cur = list->cur->next;
	}
}
