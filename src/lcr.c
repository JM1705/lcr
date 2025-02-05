#include <linux/uinput.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "json_io.h"
#include "direct.h"
#include "json_io.h"
#include "libinput_interface.h"
#include "mapping_funcs.h"


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

void setup_uinput_output(int fd, struct output_device *out_dev) {
  printf("%s has \n", out_dev->name);
  if (out_dev->key_length) {ioctl(fd, UI_SET_EVBIT, EV_KEY); printf("%d KEY ", out_dev->key_length);}
  usleep(SLEEP_US);
  if (out_dev->abs_length) {ioctl(fd, UI_SET_EVBIT, EV_ABS); printf("%d ABS ", out_dev->abs_length);}
  usleep(SLEEP_US);
  if (out_dev->rel_length) {ioctl(fd, UI_SET_EVBIT, EV_REL); printf("%d REL ", out_dev->rel_length);}
  usleep(SLEEP_US);
  if (out_dev->sw_length) {ioctl(fd, UI_SET_EVBIT, EV_SW); printf("%d SW", out_dev->sw_length);}
  for (int i=0; i<out_dev->key_length; i++) { ioctl(fd, UI_SET_KEYBIT, out_dev->key[i]); usleep(SLEEP_US);}
  for (int i=0; i<out_dev->abs_length; i++) { ioctl(fd, UI_SET_ABSBIT, out_dev->abs[i]); usleep(SLEEP_US);}
  for (int i=0; i<out_dev->rel_length; i++) { ioctl(fd, UI_SET_RELBIT, out_dev->rel[i]); usleep(SLEEP_US);}
  for (int i=0; i<out_dev->sw_length; i++) { ioctl(fd, UI_SET_SWBIT, out_dev->sw[i]); usleep(SLEEP_US);}
  printf("\n");
}

void create_uinput_device(int fd, struct output_device *out_dev) {  
  struct uinput_setup usetup;
  memset(&usetup, 0, sizeof(usetup));
  // usetup.id.bustype = BUS_VIRTUAL;
  usetup.id.vendor = 0x1234;
  usetup.id.product = 0x5678;
  // strcpy(usetup.name, "Skibidi controller");
  usetup.id.bustype = BUS_USB;
  // usetup.id.vendor = 0x045e;
  // usetup.id.product = 0x02ea;
  strcpy(usetup.name, out_dev->name);

  ioctl(fd, UI_DEV_SETUP, &usetup);
  ioctl(fd, UI_DEV_CREATE);
}

int main_loop_cb(struct maps_config* config, LIBINPUT_DEVICES_ARRAY* devarr, int* selected_device_indices, int updated_device, struct input_event *ie, int fd) {
  for (int i=0; i<config->map_count; i++) {
    struct map* mapping = config->mappings[i];
    switch (mapping->type) {
      case 1: // key press directly corresponds to button press (KEY_Z for BTN_SOUTH)
        key2btn(ie, fd, mapping, updated_device); break;
      case 2: // key press directly corresponds to absolute axis (KEY_E for ABS_Z)
        key2abs(ie, fd, mapping, updated_device); break;
      case 3: // 2 keys affect 1 axis (e.g. KEY_A and KEY_B for ABS_X)
        keys2abs(ie, fd, mapping, updated_device); break; 
      case 4:
        incrementabs(ie, fd, mapping, updated_device); break;
    }
    config->mappings[i] = mapping;
  }
  if (ie->type == EV_SYN) {
    emit(fd, EV_SYN, SYN_REPORT, 0);
  }
  return 0;
}

void init_maps(int fd, struct map **mappings) {
  for (int i=0; i<MAX_MAPPINGS; i++) {
    switch (mappings[i]->type) {
      case 2: 
        for (int j=0; j<STATUS_LENGTH; j++) {
          mappings[i]->status[j] = 0;
        }
        emit(fd, EV_ABS, mappings[i]->io[1], 0);
      case 3: 
        for (int j=0; j<STATUS_LENGTH; j++) {
          mappings[i]->status[j] = 0;
        }
        emit(fd, EV_ABS, mappings[i]->io[2], 0);
      case 4: 
        for (int j=0; j<STATUS_LENGTH; j++) {
          mappings[i]->status[j] = 0;
        }
        emit(fd, EV_ABS, mappings[i]->io[2], 0);
    }
  }
}

int* validate_input_devices(LIBINPUT_DEVICES_ARRAY* devarr, struct maps_config* config) {
  int* device_indices = malloc(sizeof(int)*config->input_count);
  for (int i=0; i<config->input_count; i++) {
    device_indices[i] = devarr->device_count; // just a value the loop won't reach
    for (int j=0; j<devarr->device_count; j++) {
      char* dev_name = devarr->devices[j]->name;
      if (strcmp(dev_name, config->inputs[i]) == 0) {
        device_indices[i] = j; 
      }
    }
    if (device_indices[i] == devarr->device_count) {
      printf("Device [%s] not found\nDevices currently recognised:\n", config->inputs[i]);
      for(int i=0; i<devarr->device_count; i++) {
        printf("  [%s]\n", devarr->devices[i]->name);
      }
      return 0;
    }
  }
  return device_indices;
}

int main(void) {
  struct maps_config* config = load_json("mappings/testmap.json");
  printf("map count: %d\n", config->map_count);

  LIBINPUT_DEVICES_ARRAY* devarr = get_devices();
  int* selected_device_indices = validate_input_devices(devarr, config);
  if (selected_device_indices == 0) {
    printf("device not found\n");
    return 0;
  }
  
  int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK); 

  setup_uinput_output(fd, config->out_dev);
  create_uinput_device(fd, config->out_dev);
  init_maps(fd, config->mappings);

  usleep(SLEEP_US);
  printf("Initialised\n");

  main_loop(config, devarr, selected_device_indices, fd, main_loop_cb);

  usleep(SLEEP_US);

  ioctl(fd, UI_DEV_DESTROY);

  close(fd);

  return 0;
}
