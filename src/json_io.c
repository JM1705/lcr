#include <stdio.h>
#include <string.h>
#include <linux/uinput.h>
#include <stdlib.h>
#include "json_io.h"
#include "cJSON/cJSON.h"
#include "utils.h"


// int read_json(char *raw_json, struct map* mappings, struct output_device *out_dev) {
struct maps_config* read_json(char *raw_json) {
  struct maps_config* config = malloc(sizeof(struct maps_config));
  config->out_dev = malloc(sizeof(struct output_device));
  config->mappings = malloc(sizeof(struct map*)*MAX_MAPPINGS);
  for (int i=0; i<MAX_MAPPINGS; i++) config->mappings[i] = malloc(sizeof(struct map));
  
  cJSON *json = cJSON_Parse(raw_json);
  // char str[1024];
  // strncpy(str, json->valuestring, 1024);
  const cJSON *mapping;
  const cJSON *io_elem;
  const cJSON *attrs_elem;
  const cJSON *code;

  cJSON *output = cJSON_GetObjectItemCaseSensitive(json, "output");
  cJSON *inputs = cJSON_GetObjectItemCaseSensitive(json, "inputs");
  cJSON *maps = cJSON_GetObjectItemCaseSensitive(json, "mappings");

  // read output info
  strncpy(config->out_dev->name, cJSON_GetObjectItemCaseSensitive(output, "name")->valuestring, DEV_NAME_MAX);

  cJSON *key = cJSON_GetObjectItemCaseSensitive(output, "KEY");
  cJSON *abs = cJSON_GetObjectItemCaseSensitive(output, "ABS");
  cJSON *rel = cJSON_GetObjectItemCaseSensitive(output, "REL");
  cJSON *sw = cJSON_GetObjectItemCaseSensitive(output, "SW");

  int i=0;
  cJSON_ArrayForEach(code, key) { config->out_dev->key[i] = code->valueint; i++; }
  config->out_dev->key_length = i;
  i=0;
  cJSON_ArrayForEach(code, abs) { config->out_dev->abs[i] = code->valueint; i++; }
  config->out_dev->abs_length = i;
  i=0;
  cJSON_ArrayForEach(code, rel) { config->out_dev->rel[i] = code->valueint; i++; }
  config->out_dev->rel_length = i;
  i=0;
  cJSON_ArrayForEach(code, sw) { config->out_dev->sw[i] = code->valueint; i++; }
  config->out_dev->sw_length = i;
  
  // read input info
  config->input_count = cJSON_GetArraySize(inputs);
  config->inputs = malloc(sizeof(char*)*config->input_count);
  for (int i=0; i<config->input_count; i++) config->inputs[i] = malloc(sizeof(char)*DEV_NAME_MAX);
  cJSON *input = NULL;
  i=0;
  cJSON_ArrayForEach(input, inputs) {
    strcpy(config->inputs[i], input->valuestring);
    i++;
  }
  
  // read mapping info
  i=0;
  cJSON_ArrayForEach(mapping, maps) {
    cJSON *type= cJSON_GetObjectItemCaseSensitive(mapping, "type");
    cJSON *io= cJSON_GetObjectItemCaseSensitive(mapping, "io");
    cJSON *desc = cJSON_GetObjectItemCaseSensitive(mapping, "desc");
    cJSON *attrs= cJSON_GetObjectItemCaseSensitive(mapping, "attrs");

    config->mappings[i]->type = type->valueint;
    int j=0;
    cJSON_ArrayForEach(io_elem, io) { config->mappings[i]->io[j] = io_elem->valueint; j++; } // can overflow i think, same with lwoer
    j=0;
    cJSON_ArrayForEach(attrs_elem, attrs) { config->mappings[i]->attrs[j] = attrs_elem->valueint; j++; }
    strncpy(config->mappings[i]->desc, desc->valuestring, DESC_LENGTH);
    i++;
  }
  config->map_count = i;

  // return i;
  return config;
}

struct maps_config* load_json(char *filename) {
  FILE* fp = fopen(filename, "r");
  char* file_contents = read_file(fp);
  struct maps_config* config = read_json(file_contents);
  // printf("Inputs: \n");
  // for (int i=0; i<config->input_count; i++) {
  //   printf("%s\n", config->inputs[i]);
  // }
  // for (int i=0; i<config->map_count; i++) {
  //   printf("Description: %s, input keycode: %d output keycode: %d\n", config->mappings[i]->desc, config->mappings[i]->io[0], config->mappings[i]->io[1]);
  // }
  return config;
}

// int main() {
//   struct maps_config *config = load_json("mappings/testmap.json");
// }
