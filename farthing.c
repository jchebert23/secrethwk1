
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <linux/limits.h>

int debugPrint1 = 0;
int debugPrint2 = 1;

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

void replace(struct linkedList *l, char *archiveFileName){
  //https://stackoverflow.com/questions/174531/how-to-read-the-content-of-a-file-to-a-string-in-c
  
  FILE *f = fopen(archiveFileName, "r");
  char c;
 int  endOfFile = 0;
  while(!feof(f))
  {
  
 
    while((c = fgetc(f)) != '|')
    {
      if(c==EOF)
      {
        endOfFile= 1;
        break;
      }
    }
    if(endOfFile)
    {
      break;
    }
  
    char *absolutePath = malloc(sizeof(char) * PATH_MAX);
    int pathNameIndex=0;
    while((c = fgetc(f)) != '\n')
    {
      absolutePath[pathNameIndex] = c;
      pathNameIndex++;
    }
    
    
    
    char *fileContents = malloc(sizeof(char) *8192);
    int contentIndex = 0;
    while((c = fgetc(f) != '|'))
    {}

    while((c = fgetc(f)) != '\n')
    {
      fileContents[contentIndex] = c;
      contentIndex++;
    }
    

  }
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

linkedList *names = linkedListCreate();

for(int i=0; i<=argc; i++)
{ printf("Arg %d %s\n", i, argv[i]);
}

for(int i=3; i<=argc; i++){
    addToList(names, argv[i]);
}

  if(debugPrint2)
  {
    printf("Line %d\n", __LINE__);
  }

if(debugPrint1)
{
  printf("%s, %s, %s", names->head->str, names->head->next->str, names->head->next->next->str);
}



if(*argv[1] == 'r'){
  
  if(debugPrint2)
  {
    printf("Line %d\n", __LINE__);
  }
  
  replace(names, argv[2]);
}




}

