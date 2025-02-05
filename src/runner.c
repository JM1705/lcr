#include "libinput_interface.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define PRINT_SPACING 4

char *spacing_print_str_devs(LIBINPUT_DEVICES_ARRAY *devarr) {
  char **dev_strs = malloc(sizeof(char*)*devarr->device_count);
  char *buffer = malloc(sizeof(char)*(MAX_ITEM_LENGTHS+16)); //idk 16 sounds nice
  for (int info_type=0; info_type<3; info_type++) {
    int maxlen = 0;
    int* lens = malloc(sizeof(int)*devarr->device_count);
    for (int i=0; i<devarr->device_count; i++) {
      switch (info_type) {
        case 0:
          dev_strs[i] = (char*) malloc(sizeof(char)*(MAX_ITEM_LENGTHS*3+16));
          sprintf(dev_strs[i], "[%d]", i);
          break;
        case 1:
          sprintf(buffer, "%s", devarr->devices[i]->name);
          strcat(dev_strs[i], buffer);
          break;
        case 2:
          sprintf(buffer, "%s", devarr->devices[i]->path);
          strcat(dev_strs[i], buffer);
          break;
      }
      lens[i] = strlen(dev_strs[i]);
      if (lens[i]>maxlen) maxlen = lens[i];
    }
    for (int i=0; i<devarr->device_count; i++) {
      int spaces_to_add = maxlen-lens[i]+PRINT_SPACING;
      for (int j=0; j<spaces_to_add; j++) {
        strcat(dev_strs[i], " ");
      }
    }
    free(lens);
  }
  free(buffer);
  int final_str_len = (MAX_ITEM_LENGTHS*3+16)*devarr->device_count;
  char* final_str = malloc(sizeof(char)*final_str_len);
  final_str[0] = '\0';
  for (int i=0; i<devarr->device_count; i++) {
    strcat(final_str, dev_strs[i]);
    char ret = '\n';
    strcat(final_str, &ret);
    free(dev_strs[i]);
  }
  free(dev_strs);
  return final_str;
}

int main() {
  struct LIBINPUT_DEVICES_ARRAY *devarr = get_devices();
  char* print_str = spacing_print_str_devs(devarr);
  printf("Choose device to remap: (0-%d)\n%s> ", devarr->device_count-1, print_str);
  int user_selection;
  scanf("%d", &user_selection);
  // printf("You have picked device %d\n", user_selection);
  struct LIBINPUT_DEVICE* dev_to_remap = devarr->devices[user_selection];
  printf("Remapping device: %s\n", dev_to_remap->name);
  char* lcr_path = realpath("bin/lcr", NULL);
  char* command = malloc(sizeof(char)*(100+MAX_ITEM_LENGTHS));
  sprintf(command, "konsole -e \"intercept %s | %s\"", dev_to_remap->path, lcr_path);
  printf("Command to be run: %s\n", command);
  FILE *lcr_output = popen(command, "r");
}
