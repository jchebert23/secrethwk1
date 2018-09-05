#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <linux/limits.h>

int debugPrint1 = 1;

typedef struct linkedList {
    struct elt *head; 
    struct elt *tail; 
}linkedList;

struct elt {
    struct elt *next;
    char *str;
};

linkedList *
linkedListCreate(void)
{
    struct linkedList *l;

    l = malloc(sizeof(struct linkedList));

    l->head = l->tail = 0;

    return l;
}

void addToList(struct linkedList *l, char *str)
{
	struct elt *e;
	e = malloc(sizeof(struct elt));
	e->str=str;
	e->next=0;
	if(l->head==0)
	{
		l->head=e;
	}
	else
	{
		l->tail->next=e;
	}
	l->tail=e;
}

void linkedListDestroy(struct linkedList *l, int t)
{
	struct elt *e;
	while((l->head)!=0)
	{	
		e=l->head;
		l->head=e->next;
		if(t==1)
		{
			free(e->str);
		}
		free(e);
	}
	free(l);
}


void removeHead(struct linkedList *l)
{
	struct elt *e;
	if(debugPrint1)
	{
		printf("Line %d\n", __LINE__);
	}
	e = l->head;
	if(l->head == 0)
	{
	    l->tail = 0;
	}
	l->head = e->next;
	if(debugPrint1)
	{
		printf("line %d\n", __LINE__);
	}
	free(e->str);
	if(debugPrint1)
	{
		printf("line %d\n", __LINE__);
	}
	free(e);
}

int main(int argc, char**argv)
{
  linkedList *l = linkedListCreate();
  char *temp;
  char temp2[6] = "hello";
  char temp4[4] = "bye";
  temp=malloc(sizeof(char)*6);
  strcpy(temp,temp2);
  addToList(l, temp);
  char *temp3;
  temp3=malloc(sizeof(char)*4);
  strcpy(temp3,temp4);
  addToList(l,temp3);
  printf("head: %s\n", l->head->str);
  printf("tail: %s\n", l->tail->str);
  removeHead(l);
  printf("head: %s\n", l->head->str);
  printf("tail: %s\n", l->tail->str);
  removeHead(l);
  if(debugPrint1)
  {
    printf("Line %d\n", __LINE__);
  } 
}
