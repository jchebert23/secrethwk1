
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
int debugPrint2 = 0;
int debugPrint3 = 0;
int debugPrint4 = 1;
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


void removeItem(struct linkedList *l, char *str)
{
	struct elt *e;
	e= l->head;
	if(strcmp(l->head->str, str)==0)
	{
		l->head=e->next;
		free(e->str);
		free(e);
	}
	while(e->next != 0)
	{
		if(strcmp(e->next->str, str)==0)
		{
			struct elt *rightElt=e->next;
			e->next = rightElt->next;
			free(rightElt->str);
			free(rightElt);
			break;
		}
		else
		{
			e=e->next;
		}

	}

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



void expandNamesHelper(char *name, struct linkedList *l)
{
  struct stat buff;
  if(lstat(name, &buff) == -1)
  {
    printf("File/Directory Name: %s Does not Exist", name);
    exit(0);
  }
  else
  {
//    printf("File type: %d\n", buff.st_mode);
      //IMPORTANT: AM I ALLOWED TO USE THIS
//      printf("File type: %d\n", S_ISREG(buff.st_mode));
      if(S_ISREG(buff.st_mode))
      {
        addToList(l,name);
      }
      else
      {
        DIR *currdir = opendir(name);
        struct dirent *contentOfDir;
        while((contentOfDir = readdir(currdir)) != NULL)
        {
	  if(strcmp(contentOfDir->d_name, ".")==0 || strcmp(contentOfDir->d_name, "..")==0)
	  {
		continue;
	  }
          char *path = malloc(sizeof(char) * (strlen(name) + strlen(contentOfDir->d_name)+2));
          strcpy(path, name);
          *(path+strlen(name))= '/';
          strcpy(path+strlen(name)+1, contentOfDir->d_name);
	  expandNamesHelper(path, l);
        }
        closedir(currdir);
	int curLength = strlen(name);
	name = realloc(name, sizeof(char) * (curLength +2));
	*(name + curLength) = '/';
	*(name + curLength+1)=0;
        addToList(l,name);
      }
  }
}



void expandNames(char **arguments, int numOfArgs, struct linkedList *l)
{  
for(int i=3; i<numOfArgs ; i++)
{
char *name= malloc(sizeof(char)* (strlen(arguments[i])+1));
strcpy(name, arguments[i]);
printf("name: %s\n", name);
expandNamesHelper(name, l);
}
}





void replace(struct linkedList *l, char *archiveFileName){
  //https://stackoverflow.com/questions/174531/how-to-read-the-content-of-a-file-to-a-string-in-c
  if(debugPrint4)
  {
	  printf("Line %d\n", __LINE__);
  }
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
    char *absolutePath = malloc(sizeof(char) * PATH_MAX);
    int pathNameIndex=0;
    while((c = fgetc(f)) != '\n')
    {
      absolutePath[pathNameIndex] = c;
      pathNameIndex++;
    }
    absolutePath[pathNameIndex] = 0;  
  if(debugPrint4)
  {
	  printf("Line %d\n", __LINE__);
	  printf("Absolute Path: %s\n", absolutePath);
  }
     //IMPORTANT: MIGHT HAVE TO SWITCH THESE CHARS TO INTS
    int fileSizeIndex = 0;
    char *fileSize = malloc(sizeof(char) *4);
    while((c = fgetc(f)) != '|')
     {
       fileSize[fileSizeIndex] = c;
       fileSizeIndex++;
     }

  if(debugPrint4)
  {
	  printf("Line %d\n", __LINE__);
  }

    char *fileContents = malloc(sizeof(char) *atoi(fileSize));
    int contentIndex = 0;
    
    while((c = fgetc(f)) != '\n')
    {
      fileContents[contentIndex] = c;
      contentIndex++;
    }

    if(debugPrint4)
    {
	    printf("Line %d\n", __LINE__);
	    printf("File Contents:%s\n", fileContents);
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

	  if(debugPrint4)
	  {
		  printf("Line %d\n", __LINE__);
	  }
      removeItem(l, absolutePath);      
    }
    else
    {
      
  if(debugPrint4)
  {
	  printf("Line %d\n", __LINE__);
	  printf("%s not found in path\n", absolutePath);
  }
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

  if(debugPrint4)
  {
	  printf("Line %d\n", __LINE__);
  }
  //Now at point where add all things that are not currently in the archive
  for(struct elt *e=l->head; e!=0; e=e->next)
  {
    struct stat buff;
    if(lstat(e->str, &buff) ==-1)
    {
	    printf("File %s Does Not Exist", e->str);
	    exit(0);
    }
    else
    {
	    
  if(debugPrint4)
  {
	  printf("Line %d\n", __LINE__);
  }

        fprintf(tempFile2, "%ld", strlen(e->str));
      fputs("|", tempFile2);
      fputs(e->str, tempFile2);
      fputs("\n", tempFile2);
      if(S_ISREG(buff.st_mode))
      {      
      //GOT THIS LINE FROM ANDREW SHEINBERG, properly formats the size returned by the lstat function to write it to the file
      fprintf(tempFile2, "%jd", (intmax_t)(buff.st_size));
      //GOT ABOVE LINE FROM ANDREW SHEINBERG
      fputs("|", tempFile2);
      FILE *NEWCONTENTS = fopen(e->str , "r");
      int fileChar;
      while((fileChar = fgetc(NEWCONTENTS)) != EOF)
      {
        fputc(fileChar, tempFile2);
      }
      
      fputs("\n",tempFile2);
      fclose(NEWCONTENTS);
      }
      else
      {
	      fputs("0|\n",tempFile2);
      }
//HAVE TO REMOVE THESE ITEMS
    }
  }
  
  
  fclose(tempFile2);
  fclose(f);
}



int main(int argc, char**argv)
{

linkedList *names = linkedListCreate();

for(int i=0; i<argc; i++)
{ printf("Arg %d %s\n", i, argv[i]);
}


expandNames(argv, argc, names);
struct elt *e=names->head;
while(e!=0)
{
	printf("Element: %s\n", e->str);
	e=e->next;

}
printf("HERE\n");
if(debugPrint4)
{
	printf("Line %d\n", __LINE__);
}

if(*argv[1] == 'r'){ 
  replace(names, argv[2]);
}




}

