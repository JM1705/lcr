#include "libinput_interface.h"
#include <stdio.h>

int main() {
  struct LIBINPUT_DEVICES_ARRAY *devarr = get_devices();
  for (int i=0; i<devarr->device_count; i++) {
    printf("Device: %s\n", devarr->devices[i]->name);
  }
}
