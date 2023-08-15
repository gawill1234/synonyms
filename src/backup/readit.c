#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/mman.h>

#include "words.h"
#define LEFT 0
#define RIGHT 1

int readline(FILE *, char *);
void removetrailingspaces(char *);
char **split_string(char *, int);
char **one_way(char *);
void one_way_words(char **, char **, int, int, int);
void lets_make_some_words(char **, int, int);
char *find_synonyms(char *);
float elapsed(struct timeval, struct timeval);
struct file_list *read_directory(const char *);

extern struct words **wordhash;
extern struct file_list *listhead;
extern struct words_by_collection *all_word_lists;

//
//   Part of dump function.  Print
//   word/synonym data
//
int printit(struct words *thing) {
int i, j;

   j = 0;
   while (thing != NULL) {
     
      printf("###################\n");
      j++;
      printf("WORD %s\n", thing->word);
      printf("   CRC: %lu\n", thing->crc32);

      for (i = 0; i < thing->syncount; i++) {
         printf("   SYN:  %s\n", thing->synonyms[i]);
      }
      printf("###################\n");

      thing = thing->next;
   }
   return(j);
}

//
//   Dump word/synonym data
//
void dumpit() {
int i = 0;
int total = 0;
struct words *thing;

   for (i = 0; i < HASHSIZE; i++) {
      if (wordhash[i] != NULL) {
         total += printit(wordhash[i]);
      }
   }
   printf("TOTAL:  %d\n", total);
}

//
//   Make sure the buffer I read into is full of nothing.
//
void resetbuffer(char *buffer, int len) {
int i;

   for (i = 0; i < len; i++) {
      buffer[i] = '\0';
   }

   return;
}

//
//   Test function.
//
char *get_and_time(char *word) {
   struct timeval st, et;
   char *l2;

   gettimeofday(&st, NULL);
   l2 = find_synonyms(word);
   gettimeofday(&et, NULL);
   printf("%s\n",  l2);
   printf("elapsed:  %f microseconds\n", (elapsed(st, et) * 1000000));

   return(l2);
}

struct words_by_collection *make_multi_list(char *filename) {
struct words_by_collection *node;
int len;

   len = strlen(filename);

   node = (struct words_by_collection *)malloc(sizeof(struct words_by_collection));
   node->collection_name = (char *)calloc(len - 2, 1);
   //
   //   The filename has .syn on the end.  Remove the
   //   last 4 characters and make it a pure collection id.
   //
   strncpy(node->collection_name, filename, len - 4);
   node->crc32 = update_crc(-1, (byte *)node->collection_name, strlen(node->collection_name));
   printf("CRC32:  %lu, %s\n", node->crc32, node->collection_name);
   fflush(stdout);
   node->wordhash = (struct words **)malloc(HASHSIZE * sizeof(struct words *));
   node->next = NULL;

   return(node);
}

void add_word_list_to_collection_list(struct words_by_collection *node) {
struct words_by_collection *tracker;

   if (all_word_lists == NULL) {
      all_word_lists = node;
   } else {
      tracker = all_word_lists;
      while (tracker->next != NULL) {
         tracker = tracker->next;
      }
      tracker->next = node;
   }

   return;

}

//
//   Open synonyms file.  Read data.
//   Store data in hash/dictionary
//   Will work for lemmatizing dictionary as well.
//
void set_up_synonyms() {

   char *line, *l2;
   FILE *myfp;
   char **words, **leftright;;
   int i = 0;
   struct file_list *tracker;
   struct words_by_collection *node;

   listhead = read_directory(".");

   tracker = listhead;

   while (tracker != NULL) {
      myfp = fopen(tracker->file_name, "r");
      if (myfp == NULL)
          return;

      printf("LOADING: %s\n", tracker->file_name);
      fflush(stdout);
      //
      //   Allocate the linked list heads in the hash.
      //
      node = make_multi_list(tracker->file_name);
      add_word_list_to_collection_list(node);
      wordhash = node->wordhash;

      //
      //   Lock the hash in memory
      //
      mlock(wordhash, (HASHSIZE * sizeof(struct words *)));

      //
      //  Initialize it all
      //
      for (i = 0; i < HASHSIZE; i++) {
         wordhash[i] = NULL;
      }

      //
      //   Line for reading data file
      //
      line = (char *)calloc(256, 1);

      while (readline(myfp, line)) {

         removetrailingspaces(line);

         //
         //   line sets up to receive the max line size.
         //   l2 is a copy based on the actual length
         //   of the string.  This makes sure we do not
         //   store a bunch of empty memory in the hash
         //   table.  l2 is also the string that is 
         //   parsed and held in memory as the word "lists".
         //
         l2 = (char *)calloc(strlen(line) + 1, 1);
         strcpy(l2, line);
         resetbuffer(line, 256);

         if ( (leftright = one_way(l2)) != NULL) {
            char **left, **right;
            left = split_string(leftright[LEFT], ',');
            right = split_string(leftright[RIGHT], ',');
            one_way_words(left, right, 0, 0, 0);
         } else {
            words = split_string(l2, ',');
            lets_make_some_words(words, 0, 0);
         }
      }

      free(line);
      fclose(myfp);
      tracker = tracker->next;
   }

   return;
}

/*
int main(int argc, char **argv) {

   gen_crc_table();
   set_up_synonyms();
   dumpit();

   get_and_time("acme");
   get_and_time("stove");
   get_and_time("precipitation");
   get_and_time("rain");
}
*/
