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

The brief description of the alogrithm is:
```
   For each word
      crc32
      word goes to crc32 % 1000 element
      add the word in order to a list (switch to balanced tree later)
      add synonyms as array to word node

   Search
   Find a word:
      crc32 of word
      go to crc32 % 1000 elem
      search list for crc32 in each node
      verify word by doing actual word compare
      if word not right, continue with search

   Normalize search work around case.  Make all lower case.


   crc32 is NOT unique.  But should be unique enough to
   require multiple string checks of actual word only
   very RARELY.

End brief alogrithm description
```


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
   make all

   will create two executables.  srvr and test.
   srvr is the server.
   test is the client
```

Running:
```
   Run server:
   srvr

   Run client:
   test http://blah.blah.blah 8080 "query"
   (you can use localhost and run both parts locally)
   or you can run the shell script named clientside.
   This will run 50000(ish) queries.
   Run srvr and test in different windows if you want to
   see timings appropriate to each half.
   the srvr side show how long it took to do the synonym replace.
   the test/clientside show the end to end (there and back) throughput.
```

Currently there are only built in cases and a single synonyms file.  The 
synonyms file is not that big and only results in about 70000 words in the
dictionary.  I would appreciate a larger list of synonyms.

With those synonyms loaded the program reaches max size of about 7.5M where 
it remains (added words in dictionary will make it bigger).  Run times for 
the stuff that is here(srvr side):
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

Round trip timings:
```
text:((AZ|Arizona|Phoenix|Ariz.), (MA|Massachusetts|Boston|Mass.), (FL|Financial leverage|Florida|Tallahassee|Fla.), (Hawaii|HI|Honolulu))
elapsed:  156.879425 microseconds
count:  49978
text:("United States of America","High German",(color|red|orange|yellow|green|blue|indigo|violet),"United Kingdom",purple,black,white,"human activity")
elapsed:  105.142593 microseconds
count:  49979
text:((AZ|Arizona|Phoenix|Ariz.), (MA|Massachusetts|Boston|Mass.), (FL|Florida|Tallahassee|Fla.), (car|coupe|sedan|sub|convertible|station wagon|van|mini van))
elapsed:  129.938126 microseconds
count:  49980
text:((color|colour|colour|coloring|colouring|coloring material|colouring material|colour|colour|colored|coloring|colors),(blue|bluish|blueish|blued|blueing|bluer|blues|bluest|bluing),(red|reddish|ruddy|blood-red|carmine|cerise|cherry|cherry-red|crimson|ruby|ruby-red|scarlet|redder|reddest|reds),(brown|brownish|chocolate-brown|dark-brown|browned|browner|brownest|browning|browns),(purple|imperial|majestic|regal|royal|violet|purplish|purpler|purples|purplest),(black|dark|blacked|blacker|blackest|blacking|blacks),(white|whiter|whites|whitest),horton)
elapsed:  189.065933 microseconds
count:  49981
text:("(United States of America|United States|the States|US|U.S.|USA|U.S.A.|United States|US|U.S.|U.S.A.|U.S. of A.|States)","(High German|German|German language)",(red|reddish|ruddy|blood-red|carmine|cerise|cherry|cherry-red|crimson|ruby|ruby-red|scarlet|redder|reddest|reds),"(United Kingdom|UK|U.K.|Britain|United Kingdom of Great Britain and Northern Ireland|Great Britain|United Kingdom of Great Britain and Northern Ireland|UK)",(purple|imperial|majestic|regal|royal|violet|purplish|purpler|purples|purplest),(black|dark|blacked|blacker|blackest|blacking|blacks),(white|whiter|whites|whitest),"(human activity|act|deed|human action)")
elapsed:  194.072723 microseconds
```

The times are in microseconds which is correct.  The dictionary load takes the
longest at about 140 milliseconds.  Once loaded, most things are pretty quick.
