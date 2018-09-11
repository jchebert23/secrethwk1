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


typedef struct fileInformation{
    char *fileNameSize;
    char *fileName;
    char *fileSize;
    char *fileContent;
}fileInformation;

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
	else
	{
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

void linkedListDestroy(struct linkedList *l)
{
	struct elt *e;
	while((l->head)!=0)
	{	
		e=l->head;
		l->head=e->next;
		
		free(e->str);
		free(e);
	}
	free(l);
}


void removeHead(struct linkedList *l)
{
	struct elt *e;
	e = l->head;
	if(l->head == 0)
	{
	    l->tail = 0;
	}
	l->head = e->next;
	free(e->str);
	free(e);
}



void expandNamesHelper(char *name, struct linkedList *l)
{
  struct stat buff;
  if(lstat(name, &buff) == -1)
  {
    printf("File/Directory Name: %s Does not Exist", name);
    linkedListDestroy(l);
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

void destroyFileInformation(struct fileInformation *f)
{

    free(f->fileNameSize);
    free(f->fileName);
    free(f->fileSize);
    if(f->fileContent)
    {
    free(f->fileContent);
    }
    free(f);
}

void expandNames(char **arguments, int numOfArgs, struct linkedList *l)
{  
for(int i=3; i<numOfArgs ; i++)
{
char *name= malloc(sizeof(char)* (strlen(arguments[i])+1));
strcpy(name, arguments[i]);
expandNamesHelper(name, l);
}
}

fileInformation *  getFileInformation(FILE *f)
{
struct fileInformation *fileInfo= malloc(sizeof(struct fileInformation));
    fileInfo->fileContent=0;
    int fileNameSizeIndex=0;
    char *fileNameSize = malloc(sizeof(char) * 5);
    char *fileSize = malloc(sizeof(char)*5);
    char *absolutePath = malloc(sizeof(char) * (PATH_MAX + 1)); 
    char c;
    while((c = fgetc(f)) != '|')
    {
      
      //if we reach end of file, got out of this
      if(c==EOF)
      {
	//IMPORTANT: CAN THEY MAKE an emtpy file NAME???
	free(fileNameSize);
	free(fileSize);
	free(absolutePath);
	fileInfo->fileNameSize=0 ;
        return fileInfo;
      }
      fileNameSize[fileNameSizeIndex] = c;
      fileNameSizeIndex++;
    }
    fileNameSize[fileNameSizeIndex]=0;
    int pathNameIndex=0;
    while(pathNameIndex<atoi(fileNameSize))
    {
      
      c=fgetc(f);
      
      absolutePath[pathNameIndex] = c;
      pathNameIndex++;
    }
    absolutePath[pathNameIndex] = 0;
    //getting rid of new line character
    fgetc(f); 
    ///
     //IMPORTANT: MIGHT HAVE TO SWITCH THESE CHARS TO INTS
    int fileSizeIndex = 0;
    while((c = fgetc(f)) != '|')
     {
	
       fileSize[fileSizeIndex] = c;
       fileSizeIndex++;
     }
    fileSize[fileSizeIndex]=0;

    char *fileContents = malloc(sizeof(char) *(atoi(fileSize)+1));
    int contentIndex = 0;
    
    while(contentIndex<atoi(fileSize))
    {
      c=fgetc(f);
      
      fileContents[contentIndex] = c;
      contentIndex++;
    }
    fileContents[contentIndex]=0;
    //getting rid of new line character
    fgetc(f);
    fileInfo->fileContent = fileContents;
    fileInfo->fileNameSize = fileNameSize;
    fileInfo->fileSize= fileSize;
    fileInfo->fileName = absolutePath;
    return fileInfo;
}


void deleteOrReplace(struct linkedList *l, char *archiveFileName, int dor){
  FILE *f;
  char temp[]="tempArchiveXXXXXX";
  int tempFile;
  FILE *tempFile2;
  //https://stackoverflow.com/questions/174531/how-to-read-the-content-of-a-file-to-a-string-in-c
  if(strcmp(archiveFileName, "-"))
  {
  f = fopen(archiveFileName, "r");
  tempFile = mkstemp(temp);
  tempFile2 = fdopen(tempFile, "w");
  }
  else
  {
  f = stdin;
  tempFile2 = stdout;
  }
  while(f != 0)
  {
    //if the absolute path appears in the arguments
    struct fileInformation *fileInfo = getFileInformation(f);
    if(fileInfo->fileNameSize == 0)
    {
	    free(fileInfo);
	    break;
    }
    char *absolutePath = fileInfo->fileName;
    char *fileSize = fileInfo->fileSize;
    char *fileContents = fileInfo->fileContent;
    if(searchList(l, absolutePath))
    {
      struct stat buff;
      if(dor)
      {
      if(lstat(absolutePath, &buff) == -1)
      {
        printf("File %s Does Not Exist", absolutePath);
	destroyFileInformation(fileInfo);
	linkedListDestroy(l);
        exit(0);
      }
      else
      {
        fprintf(tempFile2, "%ld", strlen(absolutePath));
      fputs("|", tempFile2);
      fputs(absolutePath, tempFile2);
      fputs("\n", tempFile2);
      //GOT THIS LINE FROM ANDREW SHEINBERG, properly formats the size returned by the lstat function to write it to the file
     
      if(S_ISREG(buff.st_mode))
      {
      fprintf(tempFile2, "%jd", (intmax_t)(buff.st_size));
      }
      else
      {
      fputs("0", tempFile2);
      }

      //GOT ABOVE LINE FROM ANDREW SHEINBERG
      fputs("|", tempFile2);
      FILE *NEWCONTENTS = fopen(absolutePath , "r");
      int fileChar;
      while((fileChar = fgetc(NEWCONTENTS)) != EOF)
      {
        fputc(fileChar, tempFile2);
      }
      fclose(NEWCONTENTS);
      fputs("\n", tempFile2);
      }
      }

      removeItem(l, absolutePath);
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
    destroyFileInformation(fileInfo);
  }
  //If there were not at least one deletion from command line argument
  if(dor ==0 && l->head != 0)
  {
	  struct elt *cantDelete = l->head;
	  while(cantDelete!=0)
	  {
		  printf("Cant Delete %s From Archive Since It Does Not Exist\n", cantDelete->str);
		  cantDelete=cantDelete->next;
	  }
	  linkedListDestroy(l);
	  exit(0);

  }
  //Now at point where add all things that are not currently in the archive
  for(struct elt *e=l->head; e!=0; e=e->next)
  {
    struct stat buff;
    if(lstat(e->str, &buff) ==-1)
    {
	    printf("File %s Does Not Exist", e->str);
	    linkedListDestroy(l);
	    exit(0);
    }
    else
    {
	    
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
  if(strcmp(archiveFileName, "-"))
  {
	  fclose(tempFile2);
	  if(f)
	  {
	  fclose(f);
	  unlink(archiveFileName);
	  }
	  rename(temp, archiveFileName);
  }
}

void archiveInformationOrExtraction(char *archiveName, linkedList *names, int aoe)
{
  FILE *f;
  if(strcmp(archiveName, "-"))
  {
  f = fopen(archiveName, "r");
  }
  else
  {
  f = stdin;
  }
  if(!f)
  {
	  printf("Tried to extract from  archive/print archive but file does not exist\n");
	  linkedListDestroy(names);
	  exit(0);

  }
  while(f != 0)
  {
    //if the absolute path appears in the arguments
    struct fileInformation *fileInfo = getFileInformation(f);
    if(fileInfo->fileNameSize == 0)
    {
	    free(fileInfo);
	    break;
    }
    char *absolutePath = fileInfo->fileName;
    char *fileSize = fileInfo->fileSize;
    char *fileContents = fileInfo->fileContent;
    if(searchList(names, absolutePath) || names->head==0)
    {
	if(aoe)
	{
	printf("%s\t%s\n", fileSize, absolutePath);
	}
	else
	{
		struct stat buff;
		lstat(absolutePath, &buff);
		if(!S_ISDIR(buff.st_mode))
		{
			FILE *currFile = fopen(absolutePath, "w");
			if(currFile)
			{
			    fputs(fileContents, currFile);
			    fclose(currFile);
			}
			else
			{
			    printf("File %s could not be updated from archive, since it does not exist",absolutePath);
			    linkedListDestroy(names);
			    exit(0);
			}
		}
	}
    }
    destroyFileInformation(fileInfo);
 }
 if(strcmp(archiveName, "-") && f)
 {
 fclose(f);
 }
}

int main(int argc, char**argv)
{

linkedList *names = linkedListCreate();
expandNames(argv, argc, names);
if(*argv[1] == 'r'){ 
  deleteOrReplace(names, argv[2], 1);
}
else if(*argv[1] == 'd'){
  deleteOrReplace(names, argv[2], 0);
}

else if(*argv[1] == 't'){
archiveInformationOrExtraction(argv[2],names,1);
}
else if(*argv[1] == 'x')
{
archiveInformationOrExtraction(argv[2],names,0);
}
else{
printf("Improper Command Entered\n");
}
linkedListDestroy(names);

}
