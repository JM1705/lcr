#include <stdio.h>
#include <sys/time.h>
#include <linux/input.h> // needed for input_event struct 
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include "libinput_interface.h"
#include "json_io.h"

#define POLL_TIMEOUT 5000

// int main_loop(char **device_paths, int device_count, int (*loop_cb)(int, int, struct input_event*)) {
int main_loop(struct maps_config* config, LIBINPUT_DEVICES_ARRAY* devarr, int* selected_device_indices, int fd, int (*loop_cb)(struct maps_config*, LIBINPUT_DEVICES_ARRAY*, int*, int, struct input_event*, int fd)) {
  int input_size = sizeof(struct input_event);
  int updated_device;
  int ret_poll;
  ssize_t ret_read;
  struct pollfd dev_fds[config->input_count];
  struct input_event ie;

  for (int i=0; i<config->input_count; i++) {
    dev_fds[i].fd = open(devarr->devices[selected_device_indices[i]]->path, O_RDONLY|O_NONBLOCK);

    if (dev_fds[i].fd<0) {printf("Cannot open device %s\n", config->inputs[i]); return (0);}

    dev_fds[i].events = POLLIN;
  }

  while (1) {
    // poll device, -1 ret will mean can't poll
    ret_poll = poll(dev_fds, config->input_count, POLL_TIMEOUT);

    if (ret_poll <=0) {printf("Cannot poll devices (this seems to happen randomly?)\n"); continue;}

    // check which device is updated
    updated_device = -1;
    for (int i=0; i<config->input_count; i++) if (dev_fds[i].revents) updated_device = i;
    if (updated_device == -1) {printf("No device was updated in poll timeout\n"); break;}

    // read input event from fd
    ret_read = read(dev_fds[updated_device].fd, &ie, input_size); 

    if (ret_read<0) {printf("Error value returned fd\n"); break;}

    // printf("updated device: %d, code: %d, value: %d\n", updated_device, ie.code, ie.value);      
    loop_cb(config, devarr, selected_device_indices, updated_device, &ie, fd);
    
  }

  for (int i=0; i<config->input_count; i++) close(dev_fds[i].fd);
  return 0;
}
