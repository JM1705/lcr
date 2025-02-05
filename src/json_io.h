#ifndef JSONIO_H
#define JSONIO_H

#define IO_LENGTH 32
#define ATTRS_LENGTH 32
#define DESC_LENGTH 256
#define MAX_MAPPINGS 256

#define DEV_NAME_MAX 256
#define KEY_ASSIGN_MAX 256
#define STATUS_LENGTH 256

#define INPUT_DEV_COUNT 16

struct map {
  int type;
  int io[IO_LENGTH];
  char desc[DESC_LENGTH];
  int attrs[ATTRS_LENGTH];
  int status[STATUS_LENGTH];
};

struct output_device {
  char name[DEV_NAME_MAX];
  int key[KEY_ASSIGN_MAX];
  int abs[KEY_ASSIGN_MAX];
  int rel[KEY_ASSIGN_MAX];
  int sw[KEY_ASSIGN_MAX];

  int key_length;
  int abs_length;
  int rel_length;
  int sw_length;
};

struct maps_config {
  struct output_device* out_dev; 
  char** inputs;
  int input_count;
  // struct map** mappings[MAX_MAPPINGS];
  struct map** mappings;
  int map_count;
};

// int load_json(char *filename, struct map *mappings, struct output_device *out_dev);
struct maps_config* load_json(char *filename);

#endif
