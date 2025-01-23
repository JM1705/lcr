#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFSIZE 1024
#define MAX_ITEM_LENGTHS 256
#define MAX_ATTR_COUNT 64
#define MAX_DEVICE_COUNT 256

typedef struct BLOCK {
  char charbuf[BUFSIZE+1];
  void *next; // ptr to next block 
  void *before;
} BLOCK;

typedef struct LIBINPUT_DEVICE {
  char name[MAX_ITEM_LENGTHS];
  char path[MAX_ITEM_LENGTHS];
  int group;
  char seat[MAX_ATTR_COUNT][MAX_ITEM_LENGTHS];
  char capabilities[MAX_ATTR_COUNT][MAX_ITEM_LENGTHS];
} DEVICE;

int min(int a, int b) {
  if (a <= b) {
    return a;
  }
  return b;
}

char* get_command_output(char *command) {
  FILE *fp = popen(command, "r");
  BLOCK *mainbuf = (BLOCK*) malloc(sizeof(BLOCK));
  BLOCK *currentbuf = mainbuf;
  int len = 0;

  // store output progressively in 1kb linked buffers
  char outchar;
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
  int charcount = 0;
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

int find_next_blank_gap(char *str, int start) {
  int len = strlen(str);
  for (int i=start; i<len; i++) {
    if (str[i] == '\n') {
      int offset = 1;
      while (str[i+offset] != '\0') {
        if (str[i+offset] == '\n') return i;
        // if (str[i+offset] == '\n' && str[i+offset+1] != '\0') return i;
        else if (isblank(str[i+offset])) offset ++;
        else break;
      }
    }
  }
  return -1;
}

struct LIBINPUT_DEVICE* parse_libinput_list(char *libinput_list) {
  int len = strlen(libinput_list);

  int current_pos = 0;
  int device_count = 0;
  while (1) {
    int next_blank = find_next_blank_gap(libinput_list, current_pos);
    if (next_blank == -1) break;
    device_count ++;
    current_pos = next_blank+1;
  }
  printf("device count: %d\n", device_count);

  
  // return devices;
}

int main() {
  // char *dest = get_command_output("libinput list-devices");
  char *dest = get_command_output("cat ../testdata/libinput_list-devices");
  parse_libinput_list(dest);
  return 0;
}
