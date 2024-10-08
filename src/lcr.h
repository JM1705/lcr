#ifndef LCR_H
#define LCR_H

#define IO_LENGTH 32
#define ATTRS_LENGTH 32
#define DESC_LENGTH 256
#define MAX_MAPPINGS 256

struct map {
  int type;
  int io[IO_LENGTH];
  char desc[DESC_LENGTH];
  int attrs[ATTRS_LENGTH];
};

#endif
