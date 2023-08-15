struct words {
   char *word;
   unsigned long crc32;
   int syncount;
   char **synonyms;
   char *lemma;
   struct words *next;
};

struct file_list {
   char *file_name;
   struct file_list *next;
};

struct words_by_collection {
   char *collection_name;
   struct words **wordhash;
   struct words_by_collection *next;
};

typedef unsigned char      byte;    // Byte is a char
typedef unsigned short int word16;  // 16-bit word is a short int
typedef unsigned int       word32;  // 32-bit word is an int

#define POLYNOMIAL 0x04c11db7L      // Standard CRC-32 ppolynomial
#define BUFFER_LEN       4096L      // Length of buffer

static word32 crc_table[256];       // Table of 8-bit remainders

void gen_crc_table(void);
word32 update_crc(word32 crc_accum, byte *data_blk_ptr, word32 data_blk_size);
