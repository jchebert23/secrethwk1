
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <linux/limits.h>
//IMPORTANT AM I ALLOWED TO INCLUDE BELOW LINE
#include <inttypes.h>


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

int searchList(struct linkedList *l, char *string)
{
  struct elt *e;
  e = l-> head;
  while(e != 0)
  {
    if(strcmp(string, e->str)==0)
    {
      return 1;
    }
    else
    {
      e = e->next;
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

void replace(struct linkedList *l, char *archiveFileName){
  //https://stackoverflow.com/questions/174531/how-to-read-the-content-of-a-file-to-a-string-in-c
  
  FILE *f = fopen(archiveFileName, "r");
  char c;
  int  endOfFile = 0;
  char template[]= "tempArchiveXXXXXX";
  int tempFile = mkstemp(template);
  FILE *tempFile2 = fdopen(tempFile, "w");
  while(!feof(f))
  {
    int fileNameSizeIndex=0;
    char *fileNameSize = malloc(sizeof(char) *4);
    while((c = fgetc(f)) != '|')
    {
      //if we reach end of file, got out of this
      if(c==EOF)
      {
        endOfFile= 1;
        break;
      }
      fileNameSize[fileNameSizeIndex] = c;
      fileNameSizeIndex++;
    }
    if(endOfFile)
    {
      break;
    }
  
    if(debugPrint2)
    {
      printf("file name size: %s\n", fileNameSize);
    }
    char *absolutePath = malloc(sizeof(char) * PATH_MAX);
    int pathNameIndex=0;
    while((c = fgetc(f)) != '\n')
    {
      absolutePath[pathNameIndex] = c;
      pathNameIndex++;
    }
     //IMPORTANT: MIGHT HAVE TO SWITCH THESE CHARS TO INTS
    int fileSizeIndex = 0;
    char *fileSize = malloc(sizeof(char) *4);
    while((c = fgetc(f)) != '|')
     {
       fileSize[fileSizeIndex] = c;
       fileSizeIndex++;
     }

    if(debugPrint2)
    {
      printf("file size: %s\n", fileSize);
    }

    char *fileContents = malloc(sizeof(char) *atoi(fileSize));
    int contentIndex = 0;
    
    while((c = fgetc(f)) != '\n')
    {
      fileContents[contentIndex] = c;
      contentIndex++;
    }
    //if the absolute path appears in the arguments
    if(searchList(l, absolutePath))
    {
      struct stat buff;
      if(lstat(absolutePath, &buff) == -1)
      {
        printf("File %s Does Not Exist", absolutePath);
        exit(0);
      }
      else
      {
        fprintf(tempFile2, "%ld", strlen(absolutePath));
      fputs("|", tempFile2);
      fputs(absolutePath, tempFile2);
      fputs("\n", tempFile2);
      //GOT THIS LINE FROM ANDREW SHEINBERG, properly formats the size returned by the lstat function to write it to the file
      fprintf(tempFile2, "%jd", (intmax_t)(buff.st_size));
      //GOT ABOVE LINE FROM ANDREW SHEINBERG
      fputs("|", tempFile2);
      FILE *NEWCONTENTS = fopen(absolutePath , "r");
      int fileChar;
      while((fileChar = fgetc(NEWCONTENTS)) != EOF)
      {
        fputc(fileChar, tempFile2);
      }
      fclose(NEWCONTENTS);
      }
      
    }
    else
    {
      fprintf(tempFile2, "%ld" , strlen(absolutePath));
      fputs("|", tempFile2);
      fputs(absolutePath, tempFile2);
      fputs("\n", tempFile2);
      fputs(fileSize, tempFile2);
      fputs("|", tempFile2);
      fputs(fileContents, tempFile2); 
      fputs("\n", tempFile2);
    }

  }
  fclose(tempFile2);
  fclose(f);
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

for(int i=0; i<argc; i++)
{ printf("Arg %d %s\n", i, argv[i]);
}

for(int i=3; i<argc; i++){
    addToList(names, argv[i]);
}



if(*argv[1] == 'r'){
  
  if(debugPrint2)
  {
    printf("Line %d\n", __LINE__);
  }
  
  replace(names, argv[2]);
}




}

