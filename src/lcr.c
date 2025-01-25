#include <linux/uinput.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "json_io.h"
#include "lcr.h"
// #include "libinput_interface.h"


#define SLEEP_US 15000

void emit(int fd, int type, int code, int val) { 
  // fd is file descriptor, in tis case it's /dev/uinput 
  struct input_event ie;
  ie.type = type;
  ie.code = code;
  ie.value = val;
  ie.time.tv_sec = 0;
  ie.time.tv_usec = 0;
  write(fd, &ie, sizeof(ie));
  // usleep(SLEEP_US);
}

void setup_input(int fd, struct device gamepad_device) {
  printf("%s has \n", gamepad_device.name);
  if (gamepad_device.key_length) {ioctl(fd, UI_SET_EVBIT, EV_KEY); printf("%d KEY ", gamepad_device.key_length);}
  usleep(SLEEP_US);
  if (gamepad_device.abs_length) {ioctl(fd, UI_SET_EVBIT, EV_ABS); printf("%d ABS ", gamepad_device.abs_length);}
  usleep(SLEEP_US);
  if (gamepad_device.rel_length) {ioctl(fd, UI_SET_EVBIT, EV_REL); printf("%d REL ", gamepad_device.rel_length);}
  usleep(SLEEP_US);
  if (gamepad_device.sw_length) {ioctl(fd, UI_SET_EVBIT, EV_SW); printf("%d SW", gamepad_device.sw_length);}
  for (int i=0; i<gamepad_device.key_length; i++) { ioctl(fd, UI_SET_KEYBIT, gamepad_device.key[i]); usleep(SLEEP_US);}
  for (int i=0; i<gamepad_device.abs_length; i++) { ioctl(fd, UI_SET_ABSBIT, gamepad_device.abs[i]); usleep(SLEEP_US);}
  for (int i=0; i<gamepad_device.rel_length; i++) { ioctl(fd, UI_SET_RELBIT, gamepad_device.rel[i]); usleep(SLEEP_US);}
  for (int i=0; i<gamepad_device.sw_length; i++) { ioctl(fd, UI_SET_SWBIT, gamepad_device.sw[i]); usleep(SLEEP_US);}
  printf("\n");
}

void create_device(int fd, struct device gamepad_device) {  
  struct uinput_setup usetup;
  memset(&usetup, 0, sizeof(usetup));
  // usetup.id.bustype = BUS_VIRTUAL;
  usetup.id.vendor = 0x1234;
  usetup.id.product = 0x5678;
  // strcpy(usetup.name, "Skibidi controller");
  usetup.id.bustype = BUS_USB;
  // usetup.id.vendor = 0x045e;
  // usetup.id.product = 0x02ea;
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

void key2abs(struct input_event event, int fd, struct map mapping) {
  if (event.type == EV_KEY && event.code == mapping.io[0]) {
    if (event.value == 0 || event.value == 1) {
      int new_value = event.value*mapping.attrs[0];
      printf("value is now %d for %d\n", new_value, mapping.io[1]);
      emit(fd, EV_ABS, mapping.io[1], new_value);
    }
  }
}

void keys2abs(struct input_event event, int fd, struct map *mapping) {
  if (event.type == EV_KEY && (event.code == mapping->io[0] || event.code == mapping->io[1])) {
    if (event.value == 0 || event.value == 1) {
      // Save key status
      int code_index;
      for (int i=0; i<2; i++) {
        if (event.code == mapping->io[i]) {
          code_index = i;
          mapping->status[i] = event.value;
        }
      }
      int new_value;
      if (mapping->status[0] == 1 && mapping->status[1] == 0) {
        new_value = mapping->attrs[0];
      }
      if (mapping->status[1] == 1 && mapping->status[0] == 0) {
        new_value = mapping->attrs[1];
      }
      if (mapping->status[1] == 1 && mapping->status[0] == 1) {
        // Change to the new key
        new_value = mapping->attrs[code_index];
      }
      if (mapping->status[1] == 0 && mapping->status[0] == 0) {
        new_value = 0;
      }

      printf("value is now %d for %d\n", new_value, mapping->io[2]);
      emit(fd, EV_ABS, mapping->io[2], new_value);
    }
  }
}

void incrementabs(struct input_event event, int fd, struct map *mapping) {
  if (event.type == EV_KEY && (event.code == mapping->io[0] || event.code == mapping->io[1])) {
    if (event.value == 1) {
      int now_mult = mapping->status[0];

      if (event.code == mapping->io[0]) {
        now_mult += 1; //increment
      }
      if (event.code == mapping->io[1]) {
        now_mult -= 1; //decrement
      }   

      int now_val = now_mult*mapping->attrs[2];
      if (now_val <= mapping->attrs[1] && now_val >= mapping->attrs[0]) {
        mapping->status[0] = now_mult;
      }
      else {
        if (now_val > mapping->attrs[1]) {
          now_val = mapping->attrs[1];
        }
        if (now_val < mapping->attrs[0]) {
          now_val = mapping->attrs[0];
        }
      }

      printf("value is now %d for %d\n", now_val, mapping->io[2]);
      emit(fd, EV_ABS, mapping->io[2], now_val);
    }
  }
}

void main_loop(struct input_event event, int fd, struct map *mappings, int map_count) {
  while (fread(&event, sizeof(event), 1, stdin) == 1) {
    // printf("Recieved %d with status %d \n", event.code, event.value);
    for (int i=0; i<map_count; i++) {
      struct map mapping = mappings[i];
      switch (mapping.type) {
        case 1: // key press directly corresponds to button press (KEY_Z for BTN_SOUTH)
          key2btn(event, fd, mapping); break;
        case 2: // key press directly corresponds to absolute axis (KEY_E for ABS_Z)
          key2abs(event, fd, mapping); break;
        case 3: // 2 keys affect 1 axis (e.g. KEY_A and KEY_B for ABS_X)
          keys2abs(event, fd, &mapping); break; 
        case 4:
          incrementabs(event, fd, &mapping); break;
      }
      mappings[i] = mapping;
    }
    if (event.type == EV_SYN) {
      emit(fd, EV_SYN, SYN_REPORT, 0);
    }
  }
}

void init_maps(int fd, struct map *mappings) {
  for (int i=0; i<MAX_MAPPINGS; i++) {
    switch (mappings[i].type) {
      case 2: 
        for (int j=0; j<STATUS_LENGTH; j++) {
          mappings[i].status[j] = 0;
        }
        emit(fd, EV_ABS, mappings[i].io[1], 0);
      case 3: 
        for (int j=0; j<STATUS_LENGTH; j++) {
          mappings[i].status[j] = 0;
        }
        emit(fd, EV_ABS, mappings[i].io[2], 0);
      case 4: 
        for (int j=0; j<STATUS_LENGTH; j++) {
          mappings[i].status[j] = 0;
        }
        emit(fd, EV_ABS, mappings[i].io[2], 0);
    }
  }
}

int main(void) {
  struct map mappings[MAX_MAPPINGS];
  struct device gamepad_device;
  int map_count = load_json("../mappings/rivals.json", mappings, &gamepad_device);
  printf("map count: %d\n", map_count);

  setbuf (stdin, NULL);
  struct input_event event;
  int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK); 

  setup_input(fd, gamepad_device);
  create_device(fd, gamepad_device);
  init_maps(fd, mappings);

  usleep(SLEEP_US);
  printf("Initialised\n");

  // printf("slept");
  main_loop(event, fd, mappings, map_count);
  usleep(SLEEP_US);

  ioctl(fd, UI_DEV_DESTROY);

  close(fd);

  return 0;
}
