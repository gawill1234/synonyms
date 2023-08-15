#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>

#define FIELD 0
#define WORD 1
#define PHRASE 2
#define RESERVED 3

char *get_and_time(char *);
void gen_crc_table();
void set_up_synonyms();
float elapsed(struct timeval, struct timeval);
char *find_synonyms(char *);
void set_word_hash(char *);


struct marker {
   char *wordorstring;
   char *newquerystring;
   char *start;
   char *end;
};

struct marker querymark[20];
int current_marker;

//
//   Is the character part of the reserved/special
//   character list.
//
int inreserved(char x) {
int i, j;
char reserved[] = ":,|()[]!~*.\"";

   j = 12;

   for (i = 0; i < j; i++) {
      if (reserved[i] == x)
         return(1);
   }

   return(0);
}

//
//   The ':' and '"' characters can have special 
//   meaning.  Deal with that.
//
int dealwithreserved(char *src, int i) {
int k;

   //printf("RESERVED:  %c\n", src[i]);
   if (src[i] == ':') {
      current_marker--;
      querymark[current_marker].start = NULL;
      querymark[current_marker].end = NULL;
      free(querymark[current_marker].wordorstring);
      querymark[current_marker].wordorstring = NULL;
      return(0);
   }

   if (src[i] == '"') {
      querymark[current_marker].start = &src[i + 1];
      for (k = i + 1; k < strlen(src); k++) {
         if (src[k] == '"') {
            break;
         }
      }

      querymark[current_marker].end = &src[k];
      querymark[current_marker].wordorstring = (char *)calloc(k - i + 1, 1);
      strncpy(querymark[current_marker].wordorstring, &src[i + 1], k - i - 1);

      //printf("NEW WORD:  %s\n", querymark[current_marker].wordorstring);
      current_marker++;
      return(k - i);
   }

   return(0);

}

//
//   A simple search for the last character in an alphanumeric
//   string.  This needs to be updated to accurately 
//   represent what a query can contain.
//
int findendofstring(char *src, int i) {
int k;

   //printf("ALPHANUM:  %c\n", src[i]);
   for (k = i; k < strlen(src); k++) {
      if (!isalnum(src[k])) {
         break;
      }
   }

   querymark[current_marker].end = &src[k];
   querymark[current_marker].wordorstring = (char *)calloc(k - i + 1, 1);
   strncpy(querymark[current_marker].wordorstring, &src[i], k - i);

   //printf("NEW WORD:  %s\n", querymark[current_marker].wordorstring);
   return(strlen(querymark[current_marker].wordorstring) - 1);
}

//
//   Smash two strings together.
//   Assumes str1 is large enough to deal with all of the data.
//
char *str_append(char *str1, char *str2) {

   if (str1 == '\0') {
      sprintf(str1, "%s", str2);
   } else {
      sprintf(str1, "%s%s", str1, str2);
   }

   return(str1);
}

//
//   Puts the new query together.  This creates the new,
//   enlarged query based on the old query and the new
//   synonym/lemma data.
//
char *reassemble_query(char *src) {
int new_len = 0;
int base_len = 0;
int i, done;
char *new_query;
char *tmp;

   base_len = new_len = strlen(src);

   for (i = 0; i < 20; i++) {
      if (querymark[i].newquerystring != NULL) {
         new_len = new_len + strlen(querymark[i].newquerystring);
         querymark[i].start[0] = '\0';
      }
   }

   new_query = (char *)calloc(new_len, 1);

   i = 0;
   done = 0;
   tmp = &src[0];

   if (*tmp != '\0') {
      sprintf(new_query, "%s", tmp);
   }

   while (done == 0) {
      if (querymark[i].newquerystring != NULL) {
         new_query = str_append(new_query, querymark[i].newquerystring);
         tmp = &querymark[i].end[0];
      } else {
         done = 1;
      }
      if (tmp != NULL) {
         if (*tmp != '\0') {
            new_query = str_append(new_query, tmp);
            tmp = NULL;
         }
      }
      i++;
   }

   printf("NEW QUERY:  %s\n", new_query);

   return(new_query);

}


//
//   Break up the query and find the words.
//   Very rudimentary.  Needs some work.
//
char *parse_it(char *collection, char *src)
{
int i = 0;
unsigned int cival;
char *new_query;

   if (src == NULL) {
      return(NULL);
   }

   set_word_hash(collection);
   
   printf("#######################\n");
   printf("QUERY STRING:  %s\n", src);
   for (i = 0; i < strlen(src); i++) {
      cival = (unsigned int)src[i];
      if (cival < 128) {
         /*  Unecoded are alphanumeric, hyphen, period, underscore and tilde */
         /*  By common use standard, question mark and slash may be unecoded */
         if (isalnum(src[i])) {
            querymark[current_marker].start = &src[i];
            i += findendofstring(src, i);
            current_marker++;
         } else if (inreserved(src[i])) {
            i += dealwithreserved(src, i);
         } else {
            /*  Not so ASCII stuff */
            continue;
            //printf("OTHER ASCII:  %c\n", src[i]);
         }
      } else {
         //printf("OTHER NON-ASCII:  %c\n", src[i]);
         continue;
      }
   }

   for (i = 0; i < 20; i++) {
      if (querymark[i].wordorstring != NULL) {
         //printf("%d   %s\n", i, querymark[i].wordorstring);
         //querymark[i].newquerystring = (char *)calloc(48, 1);
         querymark[i].newquerystring = (char *)find_synonyms(querymark[i].wordorstring);
         //printf("%s\n", querymark[i].newquerystring);
         //sprintf(querymark[i].newquerystring, "(This|is|a|test|of|the|emergency)");
      }
   }

   new_query = reassemble_query(src);

   printf("#######################\n");
   
   return(new_query);
}

//
//   Reset the query word holder and marker
//   to point at nothing.  Free the memory
//   we are no logner using.
//
void resetquerymark() {
int i;

   current_marker = 0;
   for (i = 0; i < 20; i++) {
      if (querymark[i].wordorstring != NULL) {
         free(querymark[i].wordorstring);
      }
      if (querymark[i].newquerystring != NULL) {
         free(querymark[i].newquerystring);
      }
      querymark[i].wordorstring = NULL;
      querymark[i].newquerystring = NULL;
      querymark[i].start = NULL;
      querymark[i].end = NULL;
      querymark[i].wordorstring = NULL;
   }
}

void makeitgo(char *mystring) {
char thing[64];
 
   sprintf(thing, "%s", mystring);
   parse_it("synonynms", thing);
   return;
}


//
//   Test
//
/*
int main(int argc, char **argv) {
struct timeval st, et;

   gettimeofday(&st, NULL);
   gen_crc_table();
   gettimeofday(&et, NULL);
   printf("gen CRC elapsed:  %f microseconds\n", (elapsed(st, et) * 1000000));

   gettimeofday(&st, NULL);
   set_up_synonyms();
   gettimeofday(&et, NULL);
   printf("data load elapsed:  %f microseconds\n", (elapsed(st, et) * 1000000));

   resetquerymark();

   gettimeofday(&st, NULL);
   makeitgo("text:stove,text:rain");
   gettimeofday(&et, NULL);
   printf("elapsed:  %f microseconds\n", (elapsed(st, et) * 1000000));
   resetquerymark();

   //sleep(1);

   gettimeofday(&st, NULL);
   makeitgo("text:acme");
   gettimeofday(&et, NULL);
   printf("elapsed:  %f microseconds\n", (elapsed(st, et) * 1000000));
   resetquerymark();

   //sleep(1);

   gettimeofday(&st, NULL);
   makeitgo("acme");
   gettimeofday(&et, NULL);
   printf("elapsed:  %f microseconds\n", (elapsed(st, et) * 1000000));
   resetquerymark();

   //sleep(1);

   gettimeofday(&st, NULL);
   makeitgo("text:(rain,snow)");
   gettimeofday(&et, NULL);
   printf("elapsed:  %f microseconds\n", (elapsed(st, et) * 1000000));
   resetquerymark();

   //sleep(1);

   gettimeofday(&st, NULL);
   makeitgo("text:(rain,snow),author:(dog),furr:April,country:Italy");
   gettimeofday(&et, NULL);
   printf("elapsed:  %f microseconds\n", (elapsed(st, et) * 1000000));
   resetquerymark();

   //sleep(10);

   gettimeofday(&st, NULL);
   makeitgo("text:abating,author:(kitty),furr:April,country:Italy");
   gettimeofday(&et, NULL);
   printf("elapsed:  %f microseconds\n", (elapsed(st, et) * 1000000));
   resetquerymark();

   makeitgo("text:abcd,author:joe");
   resetquerymark();
   makeitgo("text:abcd,author:\"joe cool\"");
   resetquerymark();

   makeitgo("text:abcd|text:defg");
   resetquerymark();
   makeitgo("text:(abcd|defg)");
   resetquerymark();
   makeitgo("text:abcd|author:joe");
   resetquerymark();
   makeitgo("text:abcd|author:\"joe cool\"");
   resetquerymark();
   makeitgo("text:\"abcd and efgh\"|author:\"joe cool\"");
   resetquerymark();
}
*/
