#include <linux/uinput.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "json_io.h"
#include "lcr.h"

void emit(int fd, int type, int code, int val) { 
  // fd is file descriptor, in tis case it's /dev/uinput 
  struct input_event ie;
  ie.type = type;
  ie.code = code;
  ie.value = val;
  ie.time.tv_sec = 0;
  ie.time.tv_usec = 0;
  write(fd, &ie, sizeof(ie));
}

void setup_input(int fd, struct device gamepad_device) {
  printf("Adding buttons to virtual uinput device\n");
  int key_empty = 1;
  int abs_empty = 1;
  int rel_empty = 1;
  int sw_empty = 1;
  for (int i=0; i<KEY_ASSIGN_MAX; i++) { if(gamepad_device.key[i]) {key_empty = 0; }}
  for (int i=0; i<KEY_ASSIGN_MAX; i++) { if(gamepad_device.abs[i]) {abs_empty = 0; }}
  for (int i=0; i<KEY_ASSIGN_MAX; i++) { if(gamepad_device.rel[i]) {rel_empty = 0; }}
  for (int i=0; i<KEY_ASSIGN_MAX; i++) { if(gamepad_device.sw[i]) {sw_empty = 0; }}
  if (!key_empty) {ioctl(fd, UI_SET_EVBIT, EV_KEY); }
  if (!abs_empty) {ioctl(fd, UI_SET_EVBIT, EV_ABS); }
  if (!rel_empty) {ioctl(fd, UI_SET_EVBIT, EV_REL); }
  if (!sw_empty) {ioctl(fd, UI_SET_EVBIT, EV_SW); }
  for (int i=0; i<KEY_ASSIGN_MAX; i++) { ioctl(fd, UI_SET_KEYBIT, gamepad_device.key[i]); }
  for (int i=0; i<KEY_ASSIGN_MAX; i++) { ioctl(fd, UI_SET_ABSBIT, gamepad_device.abs[i]); }
  for (int i=0; i<KEY_ASSIGN_MAX; i++) { ioctl(fd, UI_SET_RELBIT, gamepad_device.rel[i]); }
  for (int i=0; i<KEY_ASSIGN_MAX; i++) { ioctl(fd, UI_SET_SWBIT, gamepad_device.sw[i]); }
  printf("\n");
  // ioctl(fd, UI_SET_KEYBIT, BTN_NORTH);
}

void create_device(int fd, struct device gamepad_device) {  
  struct uinput_setup usetup;
  memset(&usetup, 0, sizeof(usetup));
  // usetup.id.bustype = BUS_VIRTUAL;
  // usetup.id.vendor = 0x1234;
  // usetup.id.product = 0x5678;
  // strcpy(usetup.name, "Skibidi controller");
  usetup.id.bustype = BUS_USB;
  usetup.id.vendor = 0x045e;
  usetup.id.product = 0x02ea;
  strcpy(usetup.name, gamepad_device.name);

  ioctl(fd, UI_DEV_SETUP, &usetup);
  ioctl(fd, UI_DEV_CREATE);
}

void key2btn(struct input_event event, int fd, struct map mapping) {
  if (event.type == EV_KEY && event.code == mapping.io[0]) {
    if (event.value == 0 || event.value == 1) {
      // printf("Recieved %d with status %d, pressing %d\n", mapping.io[0], event.value, mapping.io[1]);
      emit(fd, EV_KEY, mapping.io[1], event.value);
    }
  }
}

void main_loop(struct input_event event, int fd, struct map *mappings, int map_count) {
  while (fread(&event, sizeof(event), 1, stdin) == 1) {
    // printf("Recieved %d with status %d \n", event.code, event.value);
    for (int i=0; i<map_count; i++) {
      struct map mapping = mappings[i];
      switch (mapping.type) {
        case 1: key2btn(event, fd, mapping); break;
      }
    }
    if (event.type == EV_SYN) {
    emit(fd, EV_SYN, SYN_REPORT, 0);
    }
  }
}

int main(void) {
  struct map mappings[MAX_MAPPINGS];
  struct device gamepad_device;
  int map_count = load_json("../mapping.json", mappings, &gamepad_device);

  setbuf (stdin, NULL);
  struct input_event event;
  int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK); 

  setup_input(fd, gamepad_device);
  create_device(fd, gamepad_device);

  usleep(100000);
  printf("Initialised\n");

  // printf("slept");
  main_loop(event, fd, mappings, map_count);

  usleep(100000);
  ioctl(fd, UI_DEV_DESTROY);

  close(fd);

  return 0;
}
