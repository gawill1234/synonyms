#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#define BLOCK_SIZE 4096
#define DIRLEN 512

struct file_list {
   char *file_name;
   struct file_list *next;
};

int is_it_syn(char *str_to_reverse) {
char matcher[] = "nys.";
int len, i, j;

   len = strlen(str_to_reverse);
   j = 0;

   if (len > 4) {
      for (i = len - 1; i > len - 5; i--) {
         if (str_to_reverse[i] != matcher[j]) {
            return(0);
         }
         j++;
      }
   } else {
      return(0);
   }

   return(1);
}

struct file_list *newfilenode(char *filename) {
struct file_list *node;

   node = (struct file_list *)malloc(sizeof(struct file_list));

   node->file_name = (char *)malloc(strlen(filename) + 1);
   strcpy(node->file_name, filename);
   node->next = NULL;

   return(node);
}

int direxist(const char *directory_name)
{
struct stat buf;

   if (stat(directory_name, &buf) == 0) {
      if ((S_IFMT & buf.st_mode) != S_IFDIR)
         return(-1);
   } else {
      return(-1);
   }
   return(0);
}

struct file_list *read_directory(char *directory_name)
{
DIR *dirp, *opendir();
struct dirent *dp, *readdir();
struct file_list *list, *listhead;

   listhead = list = NULL;
   /*************************************/
   /*   Check that directory names are 
    *   not null.
    */
   if (directory_name == NULL)
      return(NULL);

   /**************************************/
   /*   If the source directory exists, open it.
    *   Otherwise return.
    */
   if (direxist(directory_name) == 0) {
      dirp = opendir(directory_name);
   } else {
      return(NULL);
   }

   /*************************************/
   /*   Read the files from the directory and copy them
    *   to the destination.
    */
   if (dirp != NULL) {

      while ((dp = readdir(dirp)) != NULL) {

         /*************************************/
         /*   Do not want to copy "." and ".."
          */
         if (strcmp(dp->d_name, ".") != 0) {
            if (strcmp(dp->d_name, "..") != 0) {
               if (direxist(dp->d_name) != 0) {
                  if (is_it_syn(dp->d_name) == 1) {
                     if (listhead == NULL) {
                        list = listhead = newfilenode(dp->d_name);
                     } else {
                        list->next = newfilenode(dp->d_name);
                        list = list->next;
                     }
                  }
               }
            }
         }
      }
      closedir(dirp);
   }

   return(listhead);
}

/*
int main(int argc, char **argv) 
{
char *destdir, *sourcedir, *newdir;
struct file_list *thing;

   thing = read_directory(argv[1]);
   while (thing != NULL) {
      printf("xxx  FILE:  %s\n", thing->file_name);
      thing = thing->next;
   }
   fflush(stdout);

   exit(0);
}
*/
