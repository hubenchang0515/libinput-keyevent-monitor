#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include <libinput.h>
#include <libudev.h>

#include "_cgo_export.h"

static int running = 1;

static int open_restricted(const char *path, int flags, void *user_data)
{
        int fd = open(path, flags);
        return fd < 0 ? -errno : fd;
}
 
static void close_restricted(int fd, void *user_data)
{
        close(fd);
}
 
const static struct libinput_interface interface = {
        .open_restricted = open_restricted,
        .close_restricted = close_restricted,
};

void handle_keyboard_event(struct libinput *li)
{
        libinput_dispatch(li);
        struct libinput_event *event = NULL;
        while ((event = libinput_get_event(li)) != NULL)
        {
                if(libinput_event_get_type(event) == LIBINPUT_EVENT_KEYBOARD_KEY)
                {
                        struct libinput_event_keyboard* keyevent = libinput_event_get_keyboard_event(event);
                        uint32_t keycode = libinput_event_keyboard_get_key(keyevent) ;    
                        enum libinput_key_state state = libinput_event_keyboard_get_key_state(keyevent);
                        pushKeyEvent(keycode, (uint32_t)state);
                }

                libinput_event_destroy(event);  
                libinput_dispatch(li);
        }
}

int loop_stop()
{
        running = 0;
}
 
int loop_startup(void) 
{
        struct udev* udev = udev_new();
        if(udev == NULL)
        {
            fprintf(stderr, "failed to open udev\n");
            return 1;
        }
        struct libinput *li = libinput_udev_create_context(&interface, NULL, udev);
        libinput_udev_assign_seat(li, "seat0");
        libinput_dispatch(li);
 
        struct pollfd fds;
        fds.fd = libinput_get_fd(li);
        fds.events = POLLIN;
        fds.revents = 0;

        while(running && poll(&fds, 1, -1) > -1) {
                handle_keyboard_event(li);
        }
 
        libinput_unref(li);
 
        return 0;
}