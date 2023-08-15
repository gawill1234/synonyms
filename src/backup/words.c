#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "words.h"

#define WORD 0
#define LEMMA 1
#define SYNONYM 2

extern struct words **wordhash;
extern struct words_by_collection *all_word_lists;

void set_word_hash(char *collection) {
struct words_by_collection *tracker;
unsigned long src_crc32;

   tracker = all_word_lists;

   src_crc32 = update_crc(-1, (byte *)collection, strlen(collection));
   while (tracker != NULL) {
      if (src_crc32 == tracker->crc32) {
         if (strcmp(collection, tracker->collection_name) == 0) {
            wordhash = tracker->wordhash;
            return;
         }
      }
      tracker = tracker->next;
   }

   return;
}
//
//   Create and initialize the word node.
//   Will hold the synonyms and lemmatization
//   data as well.
//
struct words *initialize_words_node() {
struct words *mynewnode;

   mynewnode = (struct words *)malloc(sizeof(struct words));

   mynewnode->word = NULL;
   mynewnode->crc32 = 0;
   mynewnode->synonyms = NULL;
   mynewnode->syncount = 0;
   mynewnode->lemma = NULL;
   mynewnode->next = NULL;

   return(mynewnode);
}

//
//   The node goes into an existing list.
//   Search the list, find where it goes
//   and put it there.
//
void putinlist(int where, struct words *wrkng) {
struct words *tmp;

   tmp = wordhash[where];;

   if (tmp->crc32 > wrkng->crc32) {
      wrkng->next = wordhash[where];
      wordhash[where] = wrkng;
   } else {
      if (tmp->next != NULL) {
         while (tmp->next != NULL)
            if (tmp->next->crc32 < wrkng->crc32)
               tmp = tmp->next;
            else
               break;
      }


      if (tmp->next == NULL) {
         tmp->next = wrkng;
      } else {
         wrkng->next = tmp->next;
         tmp->next = wrkng;
      }
   }

   return;
}

//
//   Find which hash element this node is part of
//   If head is null, make node head.  Otherwise,
//   Find where it goes and put it there.
//
void putinarray(struct words *wrkng) {
unsigned long where;

   where = wrkng->crc32 % HASHSIZE;

   if (wordhash[where] == NULL) {
      wordhash[where] = wrkng;
   } else {
      putinlist(where, wrkng);
   }
   return;
}

//
//   Initialize/set the indicated node values
//   Can be the word itself, the word synonyms or
//   the lemma of the word.
//
struct words *set_string_value(struct words *wrkng, char *mystring, int which) {

   if (which == WORD) {
     wrkng->word = mystring;
     wrkng->crc32 = update_crc(-1, (byte *)mystring, strlen(mystring));
     return(wrkng);
   }

   if (which == LEMMA) {
     wrkng->lemma = mystring;
     return(wrkng);
   }

   if (which == SYNONYM) {
      if (wrkng->synonyms == NULL) {
         wrkng->synonyms = (char **)malloc( (wrkng->syncount + 1) * sizeof(char *) );
      } else {
         wrkng->synonyms = (char **)realloc(wrkng->synonyms, (wrkng->syncount + 1) * sizeof(char *) );
      }
      wrkng->synonyms[wrkng->syncount] = mystring;
      wrkng->syncount++;
   }

   return(wrkng);
}

//
//   How many words are there?
//
int how_many(char **words) {
int i = 0;

   while (words[i] != NULL) {
      i++;
   }

   return(i);
}

//
//   Does the word already exist in the table.
//   If it does, return that node
//
struct words *doesitexist(char *mystring) {
unsigned long mycrc32;
int arrref;
struct words *tmp;

   mycrc32 = update_crc(-1, (byte *)mystring, strlen(mystring));
   arrref = mycrc32 % HASHSIZE;

   if (wordhash[arrref] != NULL) {
      tmp = wordhash[arrref];
      while (tmp != NULL) {
         if (tmp->crc32 == mycrc32) {
            if (strcmp(tmp->word, mystring) == 0) {
               return(tmp);
            }
         }
         tmp = tmp->next;
      }
   }
   return(NULL);
}

void one_way_words(char **left, char **right, int exclude, int lcount, int rcount) {
int i, j;
struct words *thing;
int doarray = 1;

   i = 0;

   //
   //   If count is 0 or less, we
   //   need to figure out what the
   //   word count in the list of new
   //   words is.  If it is greater than
   //   zero, we know what it is.
   //
   if (lcount <= 0)
      lcount = how_many(left);

   if (rcount <= 0)
      rcount = how_many(right);

   //
   //   Does the word exist in the dictionary
   //   If it does, do not add it again
   //   Just add the new synonyms to the syn list
   //   Otherwise, create a new node and
   //   add it.
   //
   for (j = 0; j < lcount; j++) {
      thing = doesitexist(left[j]);
      if (thing == NULL) {
         thing = initialize_words_node();
         thing = set_string_value(thing, left[j], WORD);
      } else {
         doarray = 0;
      }

      //
      //   Add the synonyms to the synonym list
      //
      for (i = 0; i < rcount; i++) {
         thing = set_string_value(thing, right[i], SYNONYM);
      }

      //
      //   If it is a new word, add it to
      //   the dictionary
      //
      if (doarray)
         putinarray(thing);

      doarray = 1;

   }

   //exclude += 1;
   //if (words[exclude] != NULL) {
   //   lets_make_some_words(words, exclude, count);
   //}

   return;
}

//
//   Add the word/synonym combinations to the dictionary.
//   This does all of them at once.
//
void lets_make_some_words(char **words, int exclude, int count) {
int i;
struct words *thing;
int doarray = 1;

   i = 0;

   //
   //   If count is 0 or less, we
   //   need to figure out what the
   //   word count in the list of new
   //   words is.  If it is greater than
   //   zero, we know what it is.
   //
   if (count <= 0)
      count = how_many(words);

   //
   //   Does the word exist in the dictionary
   //   If it does, do not add it again
   //   Just add the new synonyms to the syn list
   //   Otherwise, create a new node and
   //   add it.
   //
   thing = doesitexist(words[exclude]);
   if (thing == NULL) {
      thing = initialize_words_node();
      thing = set_string_value(thing, words[exclude], WORD);
   } else {
      doarray = 0;
   }

   //
   //   Add the synonyms to the synonym list
   //
   for (i = 0; i < count; i++) {
      if (i != exclude) {
         thing = set_string_value(thing, words[i], SYNONYM);
      }
   }

   //
   //   If it is a new word, add it to
   //   the dictionary
   //
   if (doarray)
      putinarray(thing);

   exclude += 1;
   if (words[exclude] != NULL) {
      lets_make_some_words(words, exclude, count);
   }

   return;
}

//
//   Create the new word list from the word
//   that was provided.
//
char *get_synonyms(struct words *node) {
int total_len = 0;
int i;
char *allwords;

   total_len = strlen(node->word);
   for (i = 0; i < node->syncount; i++) {
      total_len += strlen(node->synonyms[i]);
   }

   total_len = total_len + node->syncount + 6;

   allwords = (char *)calloc(total_len, 1);
   sprintf(allwords, "(%s", node->word);
   for (i = 0; i < node->syncount; i++) {
      sprintf(allwords, "%s|%s", allwords, node->synonyms[i]);
   }
   sprintf(allwords, "%s)", allwords);

   return(allwords);
   
}

//
//   Find the word in the dictionary.
//
char *find_synonyms(char *word) {
unsigned long mycrc32;
int arrref;
struct words *tmp;
char *allwords;

   mycrc32 = update_crc(-1, (byte *)word, strlen(word));
   arrref = mycrc32 % HASHSIZE;

   tmp = wordhash[arrref];

   while (tmp != NULL) {
      if (mycrc32 == tmp->crc32) {
         if (strcmp(tmp->word, word) == 0) {
            return(get_synonyms(tmp));
         }
      }
      tmp = tmp->next;
   }

   allwords = (char *)calloc(strlen(word) + 1, 1);
   strcpy(allwords, word);
   return(allwords);
}

