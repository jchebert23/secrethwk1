#include "linkedList.h"


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

int searchList(struct linkedList *l, char *str)
{

	for(struct elt *e=l->head; e; e=e->next)
	{

		if(strcmp(str, e->str)==0)
		{
			return 1;
		}


	}
	return 0;
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





