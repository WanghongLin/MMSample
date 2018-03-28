//
//  Copyright (c) 2018 mutter
//
//  This file is part of MMSample.
//
//  MMSample is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  MMSample is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with MMSample.  If not, see <http://www.gnu.org/licenses/>.
//  
//
// Created by mutter on 3/28/18.
//
// libinput example

#include <stdio.h>
#include <libinput.h>
#include <fcntl.h>
#include <unistd.h>

#define KEY_CODE_ESC 1

static int open_restricted(const char* path, int flags, void* user_data)
{
    printf("open_restricted %s\n", path);
    return open(path, flags);
}

static void close_restricted(int fd, void* user_data)
{
    printf("close_restricted %d\n", fd);
    close(fd);
}

static int handle_input_event(struct libinput_event* event)
{
    struct libinput_event_gesture* gevent;
    struct libinput_event_keyboard* kevent;

    switch (libinput_event_get_type(event)) {
        case LIBINPUT_EVENT_DEVICE_ADDED:
            printf("device added\n");
            break;
        case LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN:
            if ((gevent = libinput_event_get_gesture_event(event)) != NULL) {
                int finger_count = libinput_event_gesture_get_finger_count(gevent);
                printf("gesture event finger count %d\n", finger_count);
            }
            break;
        case LIBINPUT_EVENT_POINTER_MOTION:
            printf("pointer motion\n");
            break;
        case LIBINPUT_EVENT_POINTER_AXIS:
            printf("pointer axis\n");
            break;
        case LIBINPUT_EVENT_KEYBOARD_KEY:
            if ((kevent = libinput_event_get_keyboard_event(event)) != NULL) {
                int key_code = libinput_event_keyboard_get_key(kevent);
                enum libinput_key_state state = libinput_event_keyboard_get_key_state(kevent);

                printf("kevent key %d\n", key_code);

                if (state == LIBINPUT_KEY_STATE_RELEASED && key_code == KEY_CODE_ESC) {
                    return -1;
                }
            }
            break;
        default:
            break;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    struct libinput_interface inf = {
            open_restricted,
            close_restricted
    };

    struct libinput* tinput; // touchpad
    struct libinput* kinput; // keyboard
    struct libinput_device* t_device;
    struct libinput_device* k_device;
    struct libinput_event* ievent;

    tinput = libinput_path_create_context(&inf, NULL);
    kinput = libinput_path_create_context(&inf, NULL);

    t_device = libinput_path_add_device(tinput, "/dev/input/event13");
    k_device = libinput_path_add_device(kinput, "/dev/input/event3");

    while (1) {

        if (libinput_dispatch(tinput) == 0 && (ievent = libinput_get_event(tinput)) != NULL) {
            if (handle_input_event(ievent) != 0) {
                break;
            }
            libinput_event_destroy(ievent);
        }

        if (libinput_dispatch(kinput) == 0 && (ievent = libinput_get_event(kinput)) != NULL) {
            if (handle_input_event(ievent) != 0) {
                break;
            }
            libinput_event_destroy(ievent);
        }
    }

    libinput_path_remove_device(t_device);
    libinput_path_remove_device(k_device);
    return 0;
}

