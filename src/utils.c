#include "utils.h"
#include <stdlib.h>

typedef struct BLOCK {
  char charbuf[BUFSIZE+1];
  void *next; // ptr to next block 
  void *before;
} BLOCK;

int min(int a, int b) {
  if (a <= b) {
    return a;
  }
  return b;
}

char* read_file(FILE *fp) {
  BLOCK *mainbuf = (BLOCK*) malloc(sizeof(BLOCK));
  BLOCK *currentbuf = mainbuf;
  int len = 0;

  // store output progressively in 1kb linked buffers
  char outchar = ' '; // assign dummy
  while (outchar != EOF) {
    for (int i=0; i<BUFSIZE; i++) {
      outchar = fgetc(fp);
      len++;
      if (outchar == EOF) {
        currentbuf->charbuf[i] = '\0';
        break;
      }
      currentbuf->charbuf[i] = outchar;
    }

    currentbuf->next = (void*) malloc(sizeof(BLOCK));
    BLOCK* nextbuf = currentbuf->next;
    nextbuf->before = currentbuf;
    currentbuf = (BLOCK*) currentbuf->next;
  }

  // consolidate output stored in linked buffers into a char string
  char *dest = malloc(sizeof(char)*len);
  int buf_count = 0;
  // int charcount = 0;
  int left = len;
  currentbuf = mainbuf;
  while (1) {
    int loops = min(BUFSIZE, left);
    for (int i=0; i<loops; i++) {
      dest[buf_count*BUFSIZE+i] = currentbuf->charbuf[i];
      left -= 1;
    }
    if (left<=0) {
      break;
    }
    currentbuf = (BLOCK*) currentbuf->next;
    buf_count ++;
  }

  // deallocate linked buffers from the bottom
  for (int i=0; i<buf_count; i++) {
    BLOCK* beforebuf = currentbuf->before;
    free(currentbuf);
    currentbuf = beforebuf;
  }
  free(currentbuf);
  
  return dest;
}
