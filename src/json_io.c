#include "cJSON/cJSON.h"
#include <stdio.h>
#include <string.h>
#include "lcr.h"

int file_length(char *filename) {
  FILE *file_ptr;
  file_ptr = fopen(filename, "r");
  int len=0;
  char c;
  if (file_ptr) {
    while ((c=getc(file_ptr)) != EOF) {
      len++;
    }
    len++;
  }
  return len;
}

void read_file_2(char *filename, char *output) {
  FILE *file_ptr;
  int size = 0;
  char c;
  file_ptr = fopen(filename, "r");
  if (file_ptr) {
    while ((c=getc(file_ptr)) != EOF) {
      output[size] = c;
      size++;
    }
    output[size] = '\0';
  }
}

int read_json(char *raw_json, int raw_length, struct map *mappings) {
  cJSON *json = cJSON_ParseWithLength(raw_json, raw_length);
  // char str[1024];
  // strncpy(str, json->valuestring, 1024);
  const cJSON *map;
  const cJSON *io_elem;
  const cJSON *attrs_elem;
  int i=0;
  cJSON_ArrayForEach(map, json) {
    cJSON *type= cJSON_GetObjectItemCaseSensitive(map, "type");
    cJSON *io= cJSON_GetObjectItemCaseSensitive(map, "io");
    cJSON *desc = cJSON_GetObjectItemCaseSensitive(map, "desc");
    cJSON *attrs= cJSON_GetObjectItemCaseSensitive(map, "attrs");

    mappings[i].type = type->valueint;
    int j=0;
    cJSON_ArrayForEach(io_elem, io) { mappings[i].io[j] = io_elem->valueint; j++; } // can overflow i think, same with lwoer
    j=0;
    cJSON_ArrayForEach(attrs_elem, attrs) { mappings[i].attrs[j] = attrs_elem->valueint; j++; }
    strncpy(mappings[i].desc, desc->valuestring, DESC_LENGTH);
    i++;
  }
  return i;
}

int load_json(char *filename, struct map *mappings) {
  int length = file_length(filename);
  char output[length];
  memset(output, '\0', sizeof(length));
  read_file_2(filename, output);
  printf("JSON file length: %d characters \n", length);
  int map_count = read_json(output, length, mappings);
  for (int i=0; i<map_count; i++) {
    printf("Description: %s, input keycode: %d output keycode: %d\n", mappings[i].desc, mappings[i].io[0], mappings[i].io[1]);
  }
  return map_count;
}

// int main(void) {
//   struct map mappings[MAX_MAPPINGS];
//   load_json("../mapping.json", mappings);
//   // int length = file_length("mapping.json");
//   // char output[length];
//   // memset(output, '\0', sizeof(length));
//   // read_file_2("mapping.json", output);
//   // printf("JSON file length: %d characters \n", length);
//   // // printf("%s\n", output);

//   // struct map mappings[MAX_MAPPINGS];
//   // read_json(output, length, mappings);
//   // printf("Description: %s, input keycode: %d\n", mappings[0].desc, mappings[0].io[0]);
// }

