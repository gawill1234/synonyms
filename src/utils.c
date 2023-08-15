#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

//
//   Return the elapsed time based on end_time (et)
//   minus start_time (st).
//
float elapsed(struct timeval st, struct timeval et) {
float elapsed_time = 0.0;

   elapsed_time = (et.tv_sec + (et.tv_usec/1000000.0)) - (st.tv_sec + (st.tv_usec/1000000.0));
   return(elapsed_time);
}

//
//   function name says it all.
//
int readline(FILE *fp, char *mystr)
{
int readres;

   readres = 1;
   if ((mystr = fgets(mystr, 128, fp)) == NULL) {
      readres = 0;
   }

   return(readres);
}
