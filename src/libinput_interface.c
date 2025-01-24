#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
// #include <regex.h>

#define BUFSIZE 1024
#define MAX_ITEM_LENGTHS 256
#define MAX_DEVICE_STR_LENGTH 4096

typedef struct BLOCK {
  char charbuf[BUFSIZE+1];
  void *next; // ptr to next block 
  void *before;
} BLOCK;

// to add more fields, add here and add at the line where it says:
// if (strcmp(attr, "Device")==0) strcpy(dev->name, field);

typedef struct LIBINPUT_DEVICE {
  char name[MAX_ITEM_LENGTHS];
  char path[MAX_ITEM_LENGTHS];
  char group[MAX_ITEM_LENGTHS];
  char seat[MAX_ITEM_LENGTHS];
  char capabilities[MAX_ITEM_LENGTHS];
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

int get_libinput_list_count(char *libinput_list) {
  int current_pos = 0;
  int divider_count = 0;
  while (1) {
    int next_blank = find_next_blank_gap(libinput_list, current_pos);
    if (next_blank == -1) break;
    divider_count ++;
    current_pos = next_blank+1;
  }
  return divider_count;
}

char **get_libinput_list_str(char *libinput_list, int* dividers, int divider_count) {
  char **strs;
  int pos = 0;
  int count = 0;
  int division = 0;
  int marker = 0;
 
  strs = malloc(sizeof(char*)*divider_count);
  for (int i=0; i<(divider_count); i++) strs[i] = malloc(sizeof(char)*MAX_DEVICE_STR_LENGTH);

  while (1) {
    if (pos == dividers[division]) {
      strs[division][count+1] = '\0';
      if (division == divider_count-1) {
        break;
      }
      division ++;
      count = 0;
    }
    strs[division][count] = libinput_list[pos];
    pos++;
    count++;
  }

  return strs;
}

int *find_libinput_list_gaps(char *libinput_list, int divider_count) {
  int *dividers = malloc(sizeof(int)*divider_count);
  int current_pos = 0;
  for (int i=0; i<divider_count; i++) {
    dividers[i] = find_next_blank_gap(libinput_list, current_pos);
    current_pos = dividers[i]+1;
  }
  return dividers;
}

struct LIBINPUT_DEVICE* parse_libinput_entry(char *libinput_entry) {
  struct LIBINPUT_DEVICE *dev = malloc(sizeof(struct LIBINPUT_DEVICE));
  
  int len = strlen(libinput_entry);
  int found = 0;

  char attrbuffer[len+1];
  int attrlen = 0;
  char *attr = malloc(0);

  char fieldbuffer[len+1];
  char fieldlen = 0;
  char *field = malloc(0);
  
  for (int i=0; i<(len+1); i++) {
    // reset every newline
    if (libinput_entry[i] == '\n' || libinput_entry[i] == '\0') {
      free(field);
      field = malloc(sizeof(char)*(fieldlen+1));
      strncpy(field, fieldbuffer, fieldlen);
      field[fieldlen] = '\0';
      // printf(" with value '%s'\n", field);

      if (strcmp(attr, "Device")==0) strcpy(dev->name, field);
      if (strcmp(attr, "Kernel")==0) strcpy(dev->path, field);
      if (strcmp(attr, "Group")==0) strcpy(dev->group, field);
      if (strcmp(attr, "Seat")==0) strcpy(dev->seat, field);
      if (strcmp(attr, "Capabilities")==0) strcpy(dev->capabilities, field);
      
      found = 0; // first stage of 'found' is having found attr, second stage is having found start of field
      attrlen = 0;
      fieldlen = 0;
      continue;
    }
    if (found == 1 && libinput_entry[i] != ' ') found = 2;
    if (libinput_entry[i] == ':') {
      free(attr);
      attr = malloc(sizeof(char)*(attrlen+1));
      strncpy(attr, attrbuffer, attrlen);
      attr[attrlen] = '\0';
      // printf("Attribute found: '%s'", attr);
      found = 1;
    }
    if (found == 2) {
      fieldbuffer[fieldlen] = libinput_entry[i]; 
      fieldlen++;
    }

    attrbuffer[attrlen] = libinput_entry[i];
    attrlen++;
  }
  free(field);
  free(attr);

  return dev;
}

struct LIBINPUT_DEVICE **parse_libinput_list(char *libinput_list) {
  int len = strlen(libinput_list);
  int divider_count = get_libinput_list_count(libinput_list);
  int *dividers = find_libinput_list_gaps(libinput_list, divider_count);
  char **dev_strs = get_libinput_list_str(libinput_list, dividers, divider_count);
  struct LIBINPUT_DEVICE **devices = malloc(sizeof(struct LIBINPUT_DEVICE*)*divider_count);
  for (int i=0; i<divider_count; i++) {
    devices[i] = parse_libinput_entry(dev_strs[i]);
  }
  return devices;
}

int main() {
  // char *dest = get_command_output("libinput list-devices");
  char *dest = get_command_output("cat ../testdata/libinput_list-devices");
  struct LIBINPUT_DEVICE **devices = parse_libinput_list(dest);
  printf("Device name is %s\n", devices[0]->name);
  printf("Device path is %s\n", devices[0]->path);
  printf("Device group is %s\n", devices[0]->group);
  printf("Device seat is %s\n", devices[0]->seat);
  printf("Device capabilties is %s\n", devices[0]->capabilities);
  return 0;
}
