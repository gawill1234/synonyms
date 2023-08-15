This is a prototype for a query expansion/synonym service.
It is based on the feature that is part of elastic search, but it
is MUCH faster.

The dictionary is currently 1000 element hash that points at the various
lists of words.

Which element a word is associated with is a modulo 1000 of the 32 bit
CRC of the word.

The CRC of the word is stored with it.  Subsequent searches for the word
use the CRC to find the word.  Note:  CRC's are not unique.  They are
PROBABLY unique for this use, but not guaranteed so there is only a
string compare (is this actually my word) after the CRCs match.

The synonyms are stored with the word.  Variants of the list are created
at the same time.  So for the following example:
```
   a, b, c, d  (word and synonyms)
   the entries created are:
   word = a, synonyms = b, c, d
   word = b, synonyms = a, c, d
   word = c, synonyms = b, a, d
   word = d, synonyms = b, c, a
```

Things to do:
```
   make the query tear down and rebuild complete and more robust
   add in levenschtein for spelling
   make the separation for lemmatization (easy)
   Set it up as an actual service (communication/sockets, etc).
```

What it finally should do:
```
   Break apart the query
   Find the words in the query
   Check word spelling and update
   Lemmatize based on the words
   Add synonyms based on the words
   Rebuild the query
   Return query
```

What is already done:
```
   Rudimentary query break down
   Find words in query
   Add synonyms based on words
   Rebuild the query
   Return the query
```

Building:
```
   Build the service:
   gcc -c <each of the c files>
   gcc -o <whatever> *.o -lcurl

   Build the client:
   gcc -o test suckit.o utils.o
```

Running:
```
   Run client:
   test http://blah.blah.blah 8080 "query"

   Run server:
   <whatever>
```

Currently there are only built in cases and a single synonyms file.  The 
synonyms file is not that big and only results in about 70000 words in the
dictionary.  I would appreciate a larger list of synonyms.

With those synonyms loaded the program reaches max size of about 7.5M where 
it remains (added words in dictionary will make it bigger).  Run times for 
the stuff that is here:
```
gen CRC elapsed:  15.974045 microseconds
data load elapsed:  143684.859375 microseconds
#######################
QUERY STRING:  text:stove,text:rain
NEW QUERY:  text:(stove|range|cooktop|stave|stoves),text:(rain|pelting|rain down|rainfall|atmospheric precipitation|precipitation|snow|hail|sleet|rained|raining|rains)
#######################
elapsed:  37.908554 microseconds
#######################
QUERY STRING:  text:acme
NEW QUERY:  text:(acme|height|elevation|peak|pinnacle|summit|superlative|meridian|tiptop|top)
#######################
elapsed:  6.914139 microseconds
#######################
QUERY STRING:  acme
NEW QUERY:  (acme|height|elevation|peak|pinnacle|summit|superlative|meridian|tiptop|top)
#######################
elapsed:  5.960464 microseconds
#######################
QUERY STRING:  text:(rain,snow)
NEW QUERY:  text:((rain|pelting|rain down|rainfall|atmospheric precipitation|precipitation|snow|hail|sleet|rained|raining|rains),(snow|snowfall|atmospheric precipitation|precipitation|rain|hail|sleet|snowed|snowing|snows))
#######################
elapsed:  11.920929 microseconds
#######################
QUERY STRING:  text:(rain,snow),author:(dog),furr:April,country:Italy
NEW QUERY:  text:((rain|pelting|rain down|rainfall|atmospheric precipitation|precipitation|snow|hail|sleet|rained|raining|rains),(snow|snowfall|atmospheric precipitation|precipitation|rain|hail|sleet|snowed|snowing|snows)),author:((dog|canine|bitch|puppy|pup|domestic dog|Canis familiaris|dogged|dogging|dogs)),furr:(April|Apr|Aprils),country:(Italy|Italian Republic|Italia|Italian Republic)
#######################
elapsed:  17.166138 microseconds
#######################
QUERY STRING:  text:abating,author:(kitty),furr:April,country:Italy
NEW QUERY:  text:(abating|abate),author:((kitty|cat|feline|pussy cat|kitties)),furr:(April|Apr|Aprils),country:(Italy|Italian Republic|Italia|Italian Republic)
#######################
elapsed:  56.982040 microseconds
```

The times are in microseconds which is correct.  The dictionary load takes the
longest at about 140 milliseconds.  Once loaded, most things are pretty quick.
