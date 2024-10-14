#include "cJSON/cJSON.h"
#include <stdio.h>
#include <string.h>
#include "lcr.h"
#include <linux/uinput.h>

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

int read_json(char *raw_json, int raw_length, struct map *mappings, struct device *gamepad_device) {
  cJSON *json = cJSON_ParseWithLength(raw_json, raw_length);
  // char str[1024];
  // strncpy(str, json->valuestring, 1024);
  const cJSON *mapping;
  const cJSON *io_elem;
  const cJSON *attrs_elem;
  const cJSON *code;

  cJSON *device = cJSON_GetObjectItemCaseSensitive(json, "device");
  cJSON *maps = cJSON_GetObjectItemCaseSensitive(json, "mappings");

  // Read the "device" section
  strncpy(gamepad_device->name, cJSON_GetObjectItemCaseSensitive(device, "name")->valuestring, DEV_NAME_MAX);

  cJSON *key = cJSON_GetObjectItemCaseSensitive(device, "KEY");
  cJSON *abs = cJSON_GetObjectItemCaseSensitive(device, "ABS");
  cJSON *rel = cJSON_GetObjectItemCaseSensitive(device, "REL");
  cJSON *sw = cJSON_GetObjectItemCaseSensitive(device, "SW");

  int i=0;
  cJSON_ArrayForEach(code, key) { gamepad_device->key[i] = code->valueint; i++; }
  gamepad_device ->key_length = i;
  i=0;
  cJSON_ArrayForEach(code, abs) { gamepad_device->abs[i] = code->valueint; i++; }
  gamepad_device ->abs_length = i;
  i=0;
  cJSON_ArrayForEach(code, rel) { gamepad_device->rel[i] = code->valueint; i++; }
  gamepad_device ->rel_length = i;
  i=0;
  cJSON_ArrayForEach(code, sw) { gamepad_device->sw[i] = code->valueint; i++; }
  gamepad_device ->sw_length = i;
  
  // Read the "mappings" section
  i=0;
  cJSON_ArrayForEach(mapping, maps) {
    cJSON *type= cJSON_GetObjectItemCaseSensitive(mapping, "type");
    cJSON *io= cJSON_GetObjectItemCaseSensitive(mapping, "io");
    cJSON *desc = cJSON_GetObjectItemCaseSensitive(mapping, "desc");
    cJSON *attrs= cJSON_GetObjectItemCaseSensitive(mapping, "attrs");

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

int load_json(char *filename, struct map *mappings, struct device *gamepad_device) {
  int length = file_length(filename);
  char output[length];
  memset(output, '\0', sizeof(length));
  read_file_2(filename, output);
  printf("JSON file length: %d characters \n", length);
  int map_count = read_json(output, length, mappings, gamepad_device);
  for (int i=0; i<map_count; i++) {
    printf("Description: %s, input keycode: %d output keycode: %d\n", mappings[i].desc, mappings[i].io[0], mappings[i].io[1]);
  }
  return map_count;
}

// int main(void) {
//   struct map mappings[MAX_MAPPINGS];
//   struct device gamepad_device;
//   load_json("../mapping.json", mappings, &gamepad_device);
//   for (int i=0; i<KEY_ASSIGN_MAX; i++) {
//     printf("%d \n", gamepad_device.key[i]);
//   }
// }

