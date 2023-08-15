#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#ifdef __APPLE__
#include <ctype.h>
#endif


//
//   Return a pointer into mystring with all of
//   the leading blank type things removed.
//
char *removeleadingspaces_new(char *mystring, char *others)
{
int i, j, mystlen, seclen, firstnonspace, none;
char *newstring;

   mystlen = strlen(mystring);
   if (mystlen <= 0)
      return(NULL);

   if (others != NULL)
      seclen = strlen(others);
   i = 0;
   firstnonspace = -1;
   do {
      switch (mystring[i]) {
         case '\0':
         case ' ':
         case '\v':
         case '\f':
         case '\t':
         case '\r':
         case '\n':
                     i++;
                     break;
         default:
                     none = 0;
                     if (others != NULL) {
                        for (j = 0; j < seclen; j++) {
                           if (mystring[i] == others[j]) {
                              i++;
                              none++;
                           }
                        }
                        if (none == 0) {
                           firstnonspace = i;
                        }
                     } else {
                        firstnonspace = i;
                     }
                     break;
      }
   } while ((firstnonspace < 0) && (i < mystlen));

   mystlen = strlen(&mystring[i]);

   newstring = (char *)calloc(mystlen + 1, 1);

   if (newstring == NULL) {
      return(NULL);
   }

   strcpy(newstring, &mystring[i]);

   return(newstring);
}

//
//   Return a pointer into mystring with all of
//   the leading blank type things removed.
//
char *removeleadingspaces(char *mystring, char *others)
{
int i, j, mystlen, seclen, firstnonspace, none;

   mystlen = strlen(mystring);
   if (mystlen <= 0)
      return(NULL);

   if (others != NULL)
      seclen = strlen(others);

   i = 0;
   firstnonspace = -1;
   do {
      switch (mystring[i]) {
         case '\0':
         case ' ':
         case '\v':
         case '\f':
         case '\t':
         case '\r':
         case '\n':
                     i++;
                     break;
         default:
                     none = 0;
                     if (others != NULL) {
                        for (j = 0; j < seclen; j++) {
                           if (mystring[i] == others[j]) {
                              i++;
                              none++;
                           }
                        }
                        if (none == 0) {
                           firstnonspace = i;
                        }
                     } else {
                        firstnonspace = i;
                     }
                     break;
      }
   } while ((firstnonspace < 0) && (i < mystlen));

   return(&mystring[i]);
}

void removetrailingspaces(char *mystring)
{
int i, mystlen, firstnonspace;

   mystlen = strlen(mystring);

   i = mystlen - 1;
   firstnonspace = -1;
   do {
      switch (mystring[i]) {
         case '\0':
                     break;
         case ' ':
         case '\v':
         case '\f':
         case '\t':
         case '\r':
         case '\n':
                     mystring[i] = '\0';
                     break;
         default:
                     firstnonspace = i;
                     break;
      }
      i--;
   } while ((firstnonspace < 0) && (i >= 0));

   return;
}

void removetrailinggarbage(char *mystring)
{
int i, mystlen, firstnonspace;

   mystlen = strlen(mystring);

   i = mystlen - 1;
   firstnonspace = -1;
   do {
      switch (mystring[i]) {
         case '\0':
                     break;
         case '\\':
         case '/':
         case ' ':
         case '\v':
         case '\f':
         case '\t':
         case '\r':
         case '\n':
                     mystring[i] = '\0';
                     break;
         default:
                     firstnonspace = i;
                     break;
      }
      i--;
   } while ((firstnonspace < 0) && (i >= 0));

   return;
}

void removenewlines(char *mystring)
{
int i, mystlen, firstnonspace;

   mystlen = strlen(mystring);

   i = mystlen - 1;
   firstnonspace = -1;
   do {
      switch (mystring[i]) {
         case '\0':
                     break;
         case '\r':
         case '\n':
                     mystring[i] = '\0';
                     break;
         default:
                     firstnonspace = i;
                     break;
      }
      i--;
   } while ((firstnonspace < 0) && (i >= 0));

   return;
}

void upperstring(char *mystring)
{
int i, len;

   len = strlen(mystring);

   for (i = 0; i < len; i++) {
      mystring[i] = toupper(mystring[i]);
   }
 
   return;
}

void lowerstring(char *mystring)
{
int i, len;

   len = strlen(mystring);

   for (i = 0; i < len; i++) {
      mystring[i] = tolower(mystring[i]);
   }
 
   return;
}

void titlestring(char *mystring)
{
int i, len, j;

   len = strlen(mystring);

   for (i = 0; i < len; i++) {
      if (i == 0) {
         mystring[i] = toupper(mystring[i]);
      } else {
         j = i - 1;
         if (isspace(mystring[j])) {
            mystring[i] = toupper(mystring[i]);
         } 
      }
   }
 
   return;
}

/**********************************************/
/*  Get the date.
 */
char *get_date()
{
struct tm *tm_ptr;
time_t the_time;
char *buf;

   buf = (char *)calloc(32, 1);
   if (buf != NULL) {
      (void) time(&the_time);
      tm_ptr = localtime(&the_time);
      strftime(buf, 32, "%F", tm_ptr);
   }

   return(buf);
}


//
//   An string equality function that returns
//   true if the two strings are equal.
//
int streq(char *one, char *two)
{

   if (one == NULL)
      return(0);

   if (two == NULL)
      return(0);

   if (strcmp(one, two) == 0) {
      return(1);
   } else {
      return(0); 
   } 

   return(0);
}

int strneq(char *one, char *two, int len)
{

   if (one == NULL)
      return(0);

   if (two == NULL)
      return(0);

   if (strncmp(one, two, len) == 0) {
      return(1);
   } else {
      return(0);
   }

   return(0);
}

//
//   Create a new string from mystring which
//   begins at the character after cutchar.
//
char *cutatchar(char *mystring, char cutchar)
{
int i, len, found;
char *newmystring;

   found = i = len = 0;
   len = strlen(mystring);

   for (i = 0; i < len; i++) {
      if (mystring[i] == cutchar) {
         found = i;
         break;
      }
   }

   newmystring = (char *)calloc(len - found, 1);
   found++;
   strcpy(newmystring, &mystring[found]);

   return(newmystring);
}

//
//   Swap a character in a string from "from" to "to"
//   Changes are done in place, so the original you
//   pass in will be changed.
//
void swap_char(char *mystring, char from, char to)
{
int i, len;

   i = len = 0;
   len = strlen(mystring);

   for (i = 0; i < len; i++) {
      if (mystring[i] == from) {
         mystring[i] = to;
         break;
      }
   }

   return;
}

char **one_way(char *mystring) {
char *one_way_string = "=>";
char **args;
int i, j, k;

   j = strlen(mystring);
   for (i = 0; i < j - 1; i++) {
      if (strncmp(&mystring[i], one_way_string, 2) == 0) {
         mystring[i] = '\0';

         args = (char **)malloc((2 + 1) * sizeof(char *));

         for (k = 0; k < 2 + 1; k++) {
            args[k] = NULL;
         }
         args[0] = mystring;
         args[1] = &mystring[i + 2];
         removetrailingspaces(args[0]);
         args[1] = removeleadingspaces_new(args[1], NULL);

         return(args);
         
      }
   }

   return(NULL);

}


//
//   Split a string into up in to parts.
//   The number of parts is based on the split char count.
//   Split on, but do not include, the split char.
//   Destructive.  Done in place.  Make a copy of
//   mystring if you need it again.
//
char **split_string(char *mystring, char split)
{
char **args;
char *cootie;
int i, numwords;

   numwords = 1;

   for (i = 0; i < strlen(mystring); i++) {
      if (mystring[i] == split) {
         numwords++;
      }
   }
   args = (char **)malloc((numwords + 1) * sizeof(char *));

   for (i = 0; i < (numwords + 1); i++) {
      args[i] = NULL;
   }

   i = 0;
   args[i] = mystring;

   i++;

   cootie = mystring;
   while (*cootie != '\0') {
      if (*cootie == split) {
         *cootie = '\0';
         cootie++;
         while (*cootie == ' ')
            cootie++;
         args[i] = cootie;
         cootie++;
         i++;
      } else {
         cootie++;
      }
   }

   return(args);
}

