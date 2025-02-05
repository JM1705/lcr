#include <linux/input.h>
#include <stdio.h>
#include "json_io.h"
#include "lcr.h"

void key2btn(struct input_event* event, int fd, struct map* mapping, int updated_device) {
  if (event->type == EV_KEY && event->code == mapping->io[0] && updated_device == mapping->attrs[0]) {
    if (event->value == 0 || event->value == 1) {
      // printf("Recieved %d with status %d, pressing %d\n", mapping->io[0], event->value, mapping->io[1]);
      emit(fd, EV_KEY, mapping->io[1], event->value);
    }
  }
}

void key2abs(struct input_event* event, int fd, struct map* mapping, int updated_device) {
  if (event->type == EV_KEY && event->code == mapping->io[0] && updated_device == mapping->attrs[0]) {
    if (event->value == 0 || event->value == 1) {
      int new_value = event->value*mapping->attrs[1];
      // printf("value is now %d for %d\n", new_value, mapping->io[1]);
      emit(fd, EV_ABS, mapping->io[1], new_value);
    }
  }
}

void keys2abs(struct input_event* event, int fd, struct map* mapping, int updated_device) {
  if (event->type == EV_KEY && (event->code == mapping->io[0] || event->code == mapping->io[1]) && updated_device == mapping->attrs[0]) {
    if (event->value == 0 || event->value == 1) {
      // Save key status
      int code_index;
      for (int i=0; i<2; i++) {
        if (event->code == mapping->io[i]) {
          code_index = i;
          mapping->status[i] = event->value;
        }
      }
      int new_value;
      if (mapping->status[0] == 1 && mapping->status[1] == 0) {
        new_value = mapping->attrs[1];
      }
      if (mapping->status[1] == 1 && mapping->status[0] == 0) {
        new_value = mapping->attrs[2];
      }
      if (mapping->status[1] == 1 && mapping->status[0] == 1) {
        // Change to the new key
        new_value = mapping->attrs[code_index];
      }
      if (mapping->status[1] == 0 && mapping->status[0] == 0) {
        new_value = 0;
      }

      // printf("value is now %d for %d\n", new_value, mapping->io[2]);
      emit(fd, EV_ABS, mapping->io[2], new_value);
    }
  }
}

void incrementabs(struct input_event* event, int fd, struct map* mapping, int updated_device) {
  if (event->type == EV_KEY && (event->code == mapping->io[0] || event->code == mapping->io[1]) && updated_device == mapping->attrs[0]) {
    if (event->value == 1) {
      int now_mult = mapping->status[0];

      if (event->code == mapping->io[0]) {
        now_mult += 1; //increment
      }
      if (event->code == mapping->io[1]) {
        now_mult -= 1; //decrement
      }   

      int now_val = now_mult*mapping->attrs[3];
      if (now_val <= mapping->attrs[2] && now_val >= mapping->attrs[1]) {
        mapping->status[0] = now_mult;
      }
      else {
        if (now_val > mapping->attrs[2]) {
          now_val = mapping->attrs[2];
        }
        if (now_val < mapping->attrs[1]) {
          now_val = mapping->attrs[1];
        }
      }

      // printf("value is now %d for %d\n", now_val, mapping->io[2]);
      emit(fd, EV_ABS, mapping->io[2], now_val);
    }
  }
}
