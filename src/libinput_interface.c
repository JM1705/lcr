#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "libinput_interface.h"
#include "utils.h"

char* get_command_output(char *command) {
  FILE *fp = popen(command, "r");
  return read_file(fp);
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
  // int marker = 0;
 
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
  int fieldlen = 0;
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
    if (libinput_entry[i] == ':' && found == 0) {
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

struct LIBINPUT_DEVICES_ARRAY *parse_libinput_list(char *libinput_list) {
  // int len = strlen(libinput_list);
  int divider_count = get_libinput_list_count(libinput_list);
  int *dividers = find_libinput_list_gaps(libinput_list, divider_count);
  char **dev_strs = get_libinput_list_str(libinput_list, dividers, divider_count);
  struct LIBINPUT_DEVICE **devices = malloc(sizeof(struct LIBINPUT_DEVICE*)*divider_count);
  for (int i=0; i<divider_count; i++) {
    devices[i] = parse_libinput_entry(dev_strs[i]);
  }
  struct LIBINPUT_DEVICES_ARRAY *devarr = malloc(sizeof(LIBINPUT_DEVICES_ARRAY));
  devarr->devices = devices;
  devarr->device_count = divider_count;
  return devarr;
}

struct LIBINPUT_DEVICES_ARRAY *get_devices() {
  FILE* fp = popen("libinput list-devices", "r");
  char* dest = read_file(fp);

  struct LIBINPUT_DEVICES_ARRAY *devarr = parse_libinput_list(dest);
  return devarr;
}
