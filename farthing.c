#define _GNU_SOURCE
#include "farthing.h"
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
int debugPrint1=0;
int debugPrint2=0;
int debugPrint3=0;
int debugPrint6=0;
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

typedef struct elt {
    struct elt *next;
    char *str;
    int flag;
    int haveToCauseDeletion;
}elt;

linkedList *
linkedListCreate(void)
{
    struct linkedList *l;

    l = malloc(sizeof(struct linkedList));

    l->head = l->tail = 0;

    return l;
}

void addToList(struct linkedList *l, char *str, int htcd)
{
	int flag=0;
	int haveToCauseDeletion=htcd;
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
	e->haveToCauseDeletion=haveToCauseDeletion;
	e->flag=flag;
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

int partOfDirectory(char *directoryName, char *string)
{
	 //char *string3 = string;
	if(directoryName[strlen(directoryName)-1]!= '/')
	{
		return 0;
	}
	int length = strlen(directoryName)-1;
	if(length<=strlen(string))
	{
	while(length>=0)
	{
		if(string[length] != directoryName[length])
		{
			printf("%s is not apart of directory %s\n", string, directoryName);
			return 0;
		}
	length=length-1;
	}
	
    
	return 1;
	}
	else
	{
	
	return 0;
	}
}


void isActuallyDirectory(struct elt *e, char *string)
{
    int length = strlen(e->str)-1;
    int notEqual=0;

    if(debugPrint6)
    {
	    printf("Name argument: %s File Name: %s\n", e->str, string);
    }
if(length<strlen(string))
{
    while(length>=0)
    {
	    if(e->str[length]!=string[length])
	    {
		    
	    if(debugPrint6)
	    {
		    printf("Name arg char: %c, not equal to File Name char: %c at Index%d\n", e->str[length], e->str[length], length);
	    }
		    notEqual=1;
	    }
	    length=length-1;
    }
    
    int curLength = strlen(e->str);
    if(notEqual==0 && string[curLength]=='/')
    {
		e->str = realloc(e->str, sizeof(char) * (curLength +2));
		*(e->str + curLength) = '/';
		*(e->str + curLength+1)=0;
    }
}
}

elt * searchList(struct linkedList *l, char *string)
{
  struct elt *e;
  e = l-> head;
  while(e != 0)
  {
    isActuallyDirectory(e, string);

    if(strcmp(string, e->str)==0)
    {
      return e;
    }
    if(partOfDirectory(e->str, string))
    {
	
	return e;
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


fileInformation *  getFileInformation(FILE *f)
{
struct fileInformation *fileInfo= malloc(sizeof(struct fileInformation));
    fileInfo->fileContent=0;
    int fileNameSizeIndex=0;
    char *fileNameSize = malloc(sizeof(char) * 16);
    char *fileSize = malloc(sizeof(char)*16);
    char *absolutePath = malloc(sizeof(char) * (PATH_MAX + 1)); 
    char c;

    if(0)
    {
	    printf("Line %d\n", __LINE__);
    }
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
      if(0)
      {
	      printf("Char at fileName Size: %c\n", c);
      }
      //printf("FileNameSize Index: %d\n", fileNameSizeIndex);
      fileNameSize[fileNameSizeIndex] = c;
      fileNameSizeIndex++;
    }
    
    if(debugPrint2)
    {
	    printf("Line %d\n", __LINE__);
    }
    fileNameSize[fileNameSizeIndex]=0;
    if(debugPrint3)
    {
	    printf("File Name Size: %s\n", fileNameSize);
    }
    int pathNameIndex=0;
    while(pathNameIndex<atoi(fileNameSize))
    {
      
      c=fgetc(f);
      
      absolutePath[pathNameIndex] = c;
      pathNameIndex++;
    }

    if(debugPrint2)
    {
	    printf("Line %d\n", __LINE__);
    }
    absolutePath[pathNameIndex] = 0;
    //getting rid of new line character
    fgetc(f); 
    ///
     //IMPORTANT: MIGHT HAVE TO SWITCH THESE CHARS TO INTS
    int fileSizeIndex = 0;
    while((c = fgetc(f)) != '|')
     {
  //     printf("Char %c\n", c);	
       fileSize[fileSizeIndex] = c;
       fileSizeIndex++;
     }

//    printf("File Size Index: %d\n" , fileSizeIndex);
    fileSize[fileSizeIndex]=0;
    
    if(debugPrint3)
    {
	    printf("File Size: %s\n", fileSize);
    }
    char *fileContents=0;
    if(atoi(fileSize)<=8192)
    {
    fileContents=malloc(sizeof(char)*(atoi(fileSize)+1));
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
    }
    fileInfo->fileContent = fileContents;
    fileInfo->fileNameSize = fileNameSize;
    fileInfo->fileSize= fileSize;
    fileInfo->fileName = absolutePath;
    return fileInfo;
}

void writeToFile(int index, FILE *file, FILE *newFile, int actuallyWrite)
{

    int c;

    while(index>0)
    {
	    if(index%10000==0)
	    {
//		printf("AT index %d\n", index);
	    }
	    if(actuallyWrite)
	    {
		    c=fgetc(file);
		    fputc(c, newFile);
	    }
	    else
	    {
		    c=fgetc(file);
	    }
	    index = index - 1;
	    if(index==0)
	    {
		    if(debugPrint3)
		    {
			    printf("Last character of Contents: %c\n", c); 
		    }
	    }
    }
    
    fgetc(file);

		    if(debugPrint3)
		    {
			    printf("NewLine character of Contents: %c\n", c); 
		    }
}

void expandNamesHelper(char *name, struct linkedList *l, char *archiveFileName, int htcd)
{

      struct stat buff;
      if(lstat(name,&buff)==-1)
      {
	addToList(l, name, htcd);
      }
      else if(S_ISREG(buff.st_mode))
      {
        addToList(l,name,htcd);
      }
      else if(S_ISDIR(buff.st_mode))
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
	  expandNamesHelper(path, l, archiveFileName,0);
        }
        closedir(currdir);
	int curLength = strlen(name);
	name = realloc(name, sizeof(char) * (curLength +2));
	*(name + curLength) = '/';
	*(name + curLength+1)=0;
        addToList(l,name,htcd);
      }


}



void expandNames(char **arguments, int numOfArgs, struct linkedList *l, char *archiveFileName)
{  
for(int i=3; i<numOfArgs ; i++)
{
char *name= malloc(sizeof(char)* (strlen(arguments[i])+1));
strcpy(name, arguments[i]);
expandNamesHelper(name, l, archiveFileName, 1);
}
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
  if(f ==0 && dor == 0)
  {
    fclose(tempFile2);
    linkedListDestroy(l);
    WARN("Tried to delete from archive but archive does not exist%s\n", "");	  
    exit(0);
  }

  while(f != 0)
  {
    
    if(debugPrint2)
    {
	    printf("Line %d\n", __LINE__);
    }
    //if the absolute path appears in the arguments
    int fileActuallyDoesntExist=1;
    struct fileInformation *fileInfo = getFileInformation(f);
    
    
    if(debugPrint2)
    {
	    printf("Line %d\n", __LINE__);
    }
    if(fileInfo->fileNameSize == 0)
    {
	    free(fileInfo);
	    break;
    }
    char *absolutePath = fileInfo->fileName;
    char *fileSize = fileInfo->fileSize;
    char *fileContents = fileInfo->fileContent;
    if(debugPrint2)
    {
	    printf("Line %d\n", __LINE__);
    }
    if(searchList(l, absolutePath))
    {
    
    if(debugPrint2)
    {
	    printf("Line %d\n", __LINE__);
    }
      fileActuallyDoesntExist=0;	    
     //repeat code  
      struct stat buff;
      //repeat code
      if(dor)
      {
      
    if(debugPrint2)
    {
	    printf("Line %d\n", __LINE__);
    }
      if(lstat(absolutePath, &buff) == -1)
      {
	struct elt *e= searchList(l, absolutePath);
	e->flag=1;
	fileActuallyDoesntExist=1;
	//linkedListDestroy(l);
        WARN("File %s Does Not Exist", absolutePath);	
	//destroyFileInformation(fileInfo);
	//exit(0);
      }
      else
      {
      fprintf(tempFile2, "%ld", strlen(absolutePath));
      fputs("|", tempFile2);
      fputs(absolutePath, tempFile2);
      fputs("\n", tempFile2);
      //GOT THIS LINE FROM ANDREW SHEINBERG, properly formats the size returned by the lstat function to write it to the file
     
    if(debugPrint2)
    {
	    printf("Line %d\n", __LINE__);
    }
      if(S_ISREG(buff.st_mode))
      {
      fprintf(tempFile2, "%jd", (intmax_t)(buff.st_size));
      }
      else
      {
      fputs("0", tempFile2);
      }

    if(debugPrint2)
    {
	    printf("Line %d\n", __LINE__);
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

    if(debugPrint2)
    {
	    printf("AbsolutePath: %s\n", absolutePath);
	    printf("Line %d\n", __LINE__);
    }
      struct elt *e = searchList(l, absolutePath);
      e->flag=1;
      e=l->head;
      while(e!=0)
      {
	if(partOfDirectory(e->str, absolutePath))
	{
	    e->flag=1;
	}
	e=e->next;
      }
    
    }
    if(fileActuallyDoesntExist)
    {
      
      fprintf(tempFile2, "%ld" , strlen(absolutePath));
      fputs("|", tempFile2);
      fputs(absolutePath, tempFile2);
      fputs("\n", tempFile2);
      fputs(fileSize, tempFile2);
      fputs("|", tempFile2);

	    if(atoi(fileInfo->fileSize)>8192)
	    {
		    writeToFile(atoi(fileInfo->fileSize), f, tempFile2, 1);
	    }
	    else
	    {
      fputs(fileContents, tempFile2); 
	    }
      fputs("\n", tempFile2);
    }
    else
    {
	    if(atoi(fileInfo->fileSize)>8192)
	    {
		    if(debugPrint3)
		    {
			    printf("Line %d where about to get past contents\n", __LINE__);
		    }
		    writeToFile(atoi(fileInfo->fileSize), f, 0, 0);
	    }
    }
    destroyFileInformation(fileInfo);
    
    if(debugPrint2)
    {
	    printf("Line %d\n", __LINE__);
    }
  }

    if(debugPrint2)
    {
	    printf("Line %d\n", __LINE__);
    }
  //If there were not at least one deletion from command line argument
  if(dor ==0 && l->head != 0)
  {
	struct elt *e= l->head;
	while(e!=0)
	{
	  if(e->flag==0 && e->haveToCauseDeletion==1)
	  {
	  if(f)
	  {
	  fclose(f);
	  }
	  unlink(temp);

	  WARN("DID NOT CAUSE AT LEAST ONE DELETION: %s", e->str);
	  linkedListDestroy(l);
	  exit(0);
	  }
	  e=e->next;
	}
  }
  //Now at point where add all things that are not currently in the archive
  for(struct elt *e=l->head; e!=0; e=e->next)
  {
    struct stat buff;
    if(lstat(e->str, &buff) ==-1)
    {
	    if(e->flag==0)
	    {
	    WARN("File %s Does Not Exist", e->str);
	    continue;
	    }
	//    linkedListDestroy(l);
	//    exit(0);
    }

    else
    {
      if(e->flag==0)
      {
        fprintf(tempFile2, "%ld", strlen(e->str));
      fputs("|", tempFile2);
      fputs(e->str, tempFile2);
      fputs("\n", tempFile2);
      if(S_ISREG(buff.st_mode))
      {      
      //GOT THIS LINE FROM ANDREW SHEINBERG, properly formats the size returned by the lstat function to write it to the file
      fprintf(tempFile2, "%jd", (intmax_t)(buff.st_size));
      //printf("Size of New File Contents: %jd\n", (intmax_t)(buff.st_size));
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
	  linkedListDestroy(names);
	  WARN("ARCHIVE FILE DOES NOT EXIST%s", "");
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
	printf("%8d %s\n", atoi(fileSize), absolutePath);

	    if(atoi(fileInfo->fileSize)>8192)
	    {
		    writeToFile(atoi(fileInfo->fileSize), f, 0, 0);
	    }
	}
	else
	{
		struct stat buff;
		if(lstat(absolutePath, &buff)==-1 && *(absolutePath+strlen(absolutePath)-1) == '/')
		{
		
		mkdir(absolutePath,0777);
		}
		else if(lstat(absolutePath, &buff)==-1)
		{
			struct stat buff2;			
			char *stringTemp = absolutePath;
			while(stringTemp != 0)
			{
			const char *stringTemp2 = stringTemp;	
			char *locationOfSlash =strchr(stringTemp2, '/');
			if(locationOfSlash == 0)
			{
				break;
			}

			int numOfChars = locationOfSlash-absolutePath;
			char *tempDir = malloc(sizeof(char)*((locationOfSlash-absolutePath)+1));
			while(numOfChars>=0)
			{
				tempDir[numOfChars]= absolutePath[numOfChars];
				numOfChars=numOfChars-1;
			}
			numOfChars = locationOfSlash-absolutePath;
			tempDir[numOfChars]= '\0';
		//	tempDir = (char *)memcpy(tempDir,absolutePath,sizeof(char)*(locationOfSlash-absolutePath+1));
			//tempDir[strlen(tempDir)]='\0';
			//printf("TempDir: %s\n", tempDir);
			if(lstat(tempDir, &buff2)==-1)
			{
			    mkdir(tempDir, 0777);
			  //  printf("Making this dir:%s\n", tempDir);
			}
			stringTemp = absolutePath+(locationOfSlash-absolutePath)+1;
			free(tempDir);
			}

		}
		if(*(absolutePath+strlen(absolutePath)-1)!= '/' || S_ISDIR(buff.st_mode)==0)
		{
			FILE *currFile = fopen(absolutePath, "w");
			if(currFile)
			{

			    if(atoi(fileInfo->fileSize)>8192)
			    {
				    writeToFile(atoi(fileInfo->fileSize), f, currFile,1);
			    }

			 else
			 {
			 fputs(fileContents, currFile);
			 }
			 fclose(currFile);
			}
			else
			{
			    
			    if(atoi(fileInfo->fileSize)>8192)
			    {
				    writeToFile(atoi(fileInfo->fileSize), f, 0, 0);
			    }
			    WARN("COULD NOT UPDATE BECAUSE DOES NOT HAVE READ PERMISSIONS: %s", absolutePath);
			}
			
		}
	}
    }
    else
    {
			    if(atoi(fileInfo->fileSize)>8192)
			    {
				    writeToFile(atoi(fileInfo->fileSize), f, 0, 0);
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
expandNames(argv, argc, names, argv[2]);
if(0)
{
	struct elt *e=names->head;
	while(e!=0)
	{
		printf("Name: %s\n", e->str);
		e=e->next;
	}
}
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
exit(0);
}
linkedListDestroy(names);
}
