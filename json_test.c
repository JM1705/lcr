#include "cJSON/cJSON.h"
#include <stdio.h>
#include <string.h>

void readfile(char *filename, char *output) {
  char c;
  char raw_json[1024] = "";
  FILE *file_ptr; // can be 0
  int i = 0;
  int length = 0;
  // file_ptr = fopen("mapping.json", "r");
  file_ptr = fopen(filename, "r");
  if (file_ptr) { // checks if file_ptr != 0 (been written to by fopen)
    while ((c=getc(file_ptr)) != EOF) {
      // printf("%c", c);
      raw_json[i] = c; 
      i=i+1;
    }
  }
  // printf("%s", raw_json);
  strncpy(output, raw_json, 1024);
  // output = raw_json;
}

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

void read_json(char *raw_json, int raw_length) {
  cJSON *json = cJSON_ParseWithLength(raw_json, raw_length);
  // char str[1024];
  // strncpy(str, json->valuestring, 1024);
  const cJSON *map;
  int i=0;
  cJSON_ArrayForEach(map, json) {
    // cJSON *type = cJSON_GetObjectItemCaseSensitive(map, "type");
    // printf("Type of mapping %d: %d\n", i, type->valueint);
    cJSON *desc = cJSON_GetObjectItemCaseSensitive(map, "desc");
    printf("Description of mapping %d: %s\n", i, desc->valuestring);
    i++;
  }

}

int main(void) {
  int length = file_length("mapping.json");
  char output[length];
  memset(output, '\0', sizeof(length));
  read_file_2("mapping.json", output);
  printf("JSON file length: %d characters \n", length);
  // printf("%s\n", output);

  read_json(output, length);
}

