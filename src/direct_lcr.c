#include <stdio.h>
#include <sys/time.h>
#include <linux/input.h> // needed for input_event struct 
#include <poll.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE_0 "/dev/input/event12"
#define DEVICE_1 "/dev/input/event10"
#define POLL_TIMEOUT 5000

int main_loop(char **device_paths, int device_count, int (*loop_cb)(int, int, struct input_event*)) {
  int input_size = sizeof(struct input_event);
  int updated_device;
  int ret_poll;
  ssize_t ret_read;
  struct pollfd dev_fds[device_count];
  struct input_event ie;

  for (int i=0; i<device_count; i++) {
    dev_fds[i].fd = open(device_paths[i], O_RDONLY|O_NONBLOCK);

    if (dev_fds[i].fd<0) {printf("can't open device DEVICE\n"); return (0);}

    dev_fds[i].events = POLLIN;
  }

  while (1) {
    // poll device, -1 ret will mean can't poll
    ret_poll = poll(dev_fds, device_count, POLL_TIMEOUT);

    if (ret_poll <=0) {printf("cannot poll devices\n"); break;}

    // check which device is updated
    updated_device = -1;
    for (int i=0; i<device_count; i++) if (dev_fds[i].revents) updated_device = i;
    if (updated_device == -1) {printf("no device was updated\n"); break;}

    // read input event from fd
    ret_read = read(dev_fds[updated_device].fd, &ie, input_size); 

    if (ret_read<0) {printf("cannot read from device fd\n"); break;}

    // printf("updated device: %d, code: %d, value: %d\n", updated_device, ie.code, ie.value);      
    loop_cb(device_count, updated_device, &ie);
    
  }

  for (int i=0; i<device_count; i++) close(dev_fds[i].fd);
  return 0;
}

// called from main_loop every loop
int loop_cb(int device_count, int updated_device, struct input_event *ie) {
    printf("updated device: %d, code: %d, value: %d\n", updated_device, ie->code, ie->value);      
    return 1;
}

int main() {
  char **device_paths = malloc(sizeof(char*)*2);
  device_paths[0] = DEVICE_0;
  device_paths[1] = DEVICE_1;
  main_loop(device_paths, 2, &loop_cb);
}
