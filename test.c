#include <linux/uinput.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

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

int main(void) {
  struct uinput_setup usetup;
  int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK); 

  ioctl(fd, UI_SET_EVBIT, EV_KEY);
  ioctl(fd, UI_SET_KEYBIT, BTN_SOUTH);
  ioctl(fd, UI_SET_KEYBIT, BTN_NORTH);

  memset(&usetup, 0, sizeof(usetup));
  usetup.id.bustype = BUS_VIRTUAL;
  usetup.id.vendor = 0x1234;
  usetup.id.product = 0x5678;
  strcpy(usetup.name, "Example skibidi device");

  ioctl(fd, UI_DEV_SETUP, &usetup);
  ioctl(fd, UI_DEV_CREATE);

  printf("pressing buttons: \n");
  // usleep(100000);
  sleep(1);
  
  // SYN separates events
  emit(fd, EV_KEY, BTN_SOUTH, 1);
  emit(fd, EV_SYN, SYN_REPORT, 0);
  sleep(1);
  emit(fd, EV_KEY, BTN_SOUTH, 0);
  emit(fd, EV_SYN, SYN_REPORT, 0);
  sleep(1);
  emit(fd, EV_KEY, BTN_NORTH, 1);
  emit(fd, EV_SYN, SYN_REPORT, 0);
  sleep(1);
  emit(fd, EV_KEY, BTN_NORTH, 0);
  emit(fd, EV_SYN, SYN_REPORT, 0);

  usleep(100000);
  ioctl(fd, UI_DEV_DESTROY);

  close(fd);

  return 0;
}
