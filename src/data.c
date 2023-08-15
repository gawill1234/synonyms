#include <stdio.h>
#include "words.h"

//
//   This is the hash table head.
//   Initialized in set_up_synonyms() for
//   the moment.  Will probably move.
//
struct words **wordhash;
struct file_list *listhead = NULL;
struct words_by_collection *all_word_lists = NULL;
