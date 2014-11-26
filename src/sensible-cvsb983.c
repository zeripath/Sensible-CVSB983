#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <libudev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <signal.h>

#define die(str, args...) do { \
        perror(str); \
        exit(EXIT_FAILURE); \
    } while(0)

#define MOD_SHIFT 1
#define MOD_CTRL 2
#define MOD_ALT 4
#define MOD_META 8

void send_key(int code, int value, int fd) {
    struct input_event ev;
    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_KEY;
    ev.code = code;
    ev.value = value;
    if (write(fd, &ev, sizeof(struct input_event)) < 0) die("error: write");
}

void send_syn(int fd) {
    struct input_event ev;
    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_SYN;
    ev.code = 0;
    ev.value = 0;
    if (write(fd, &ev, sizeof(struct input_event)) < 0) die("error: write");
}

void set_key_bits(int fd) {
    if(ioctl(fd, UI_SET_KEYBIT, KEY_C) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_I) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_M) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_RED) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_GREEN) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_YELLOW) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_BLUE) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_PREVIOUSSONG) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_NEXTSONG) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_LEFT) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_RIGHT) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_R) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_F) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_PLAYPAUSE) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_STOPCD) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_HOME) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_VOLUMEDOWN) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_VOLUMEUP) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_Q) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_TAB) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_RIGHTMETA) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_BACKSPACE) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_ENTER) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_PAGEUP) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_PAGEDOWN) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_O) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_DOWN) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_UP) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_ESC) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_COMPUTER) < 0) die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, KEY_MEDIA) < 0) die("error: ioctl");
}

int main(int argc, char* argv[])
{
    struct input_event input_ev[64];
    struct udev* _udev;
    struct udev_device* udev_dev;
    struct uinput_user_dev uidev;
    const char* devnode;
    char* devPath;
    int orig_errno;
    struct input_event ev;
    int fevdev = -1;
    int fd = -1;
    int result = 0;
    int size = sizeof(struct input_event);
    int rd;
    int i;
    int modifier = 0;
    int last_modifier = 0;
    int value;
    int in_left = 0;
    int in_right = 0;
    char name[256] = "Unknown";
    char *device = "/dev/input/by-id/usb-1d57_ad02-event-kbd";
    char *output_device = "/dev/uinput";

    printf("Initializing uinput\n");

    fd = open(output_device, O_WRONLY | O_NONBLOCK);
    if (fd == -1) {
        printf("Failed to open uinput device.\n");
        exit(1);
    }

    _udev = udev_new();
    if (!_udev) {
        printf("Failed to open udev.\n");
        exit(1);
    }

    udev_dev = udev_device_new_from_subsystem_sysname(_udev, "misc", "uinput");
    if (!udev_dev) {
        printf("Failed to create udev device.\n");
        exit(1);
    }

    devnode = udev_device_get_devnode(udev_dev);
    if (!devnode) {
        printf("Failed to get devnode from udev device.\n");
        exit(1);
    }

    devPath = (char*) malloc(strlen(devnode) + 1);
    
    strcpy(devPath, devnode);

    udev_device_unref(udev_dev);
    udev_unref(_udev);

    uidev.id.bustype = BUS_VIRTUAL;
    if(ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)
        die("error: ioctl");

    set_key_bits(fd);

    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "virtual-remote");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 0x1;
    uidev.id.product = 0x1;
    uidev.id.version = 1;
    
    if(write(fd, &uidev, sizeof(uidev)) < 0)
        die("error: write");

    if(ioctl(fd, UI_DEV_CREATE) < 0)
        die("error: ioctl");


    fevdev = open(device, O_RDONLY);
    if (fevdev == -1) {
        printf("Failed to open event device.\n");
        exit(1);
    }

    result = ioctl(fevdev, EVIOCGNAME(sizeof(name)), name);
    printf ("Reading From : %s (%s)\n", device, name);

    printf("Getting exclusive access: ");
    result = ioctl(fevdev, EVIOCGRAB, 1);
    printf("%s\n", (result == 0) ? "SUCCESS" : "FAILURE");

    while (1)
    {
        if ((rd = read(fevdev, input_ev, size * 64)) < size) {
            break;
        }
        
        for (i =0; i < rd/size; i++) {
            if (input_ev[i].type == EV_KEY) {
                switch (input_ev[i].code) {
                    case KEY_LEFTSHIFT: 
                    case KEY_RIGHTSHIFT:
                        modifier = (modifier | MOD_SHIFT) ^ ( input_ev[i].value == 0 ? MOD_SHIFT : 0 );
                        break;
                    case KEY_LEFTALT:
                    case KEY_RIGHTALT:
                        modifier = (modifier | MOD_ALT) ^ ( input_ev[i].value == 0 ? MOD_ALT : 0 );
                        break;
                    case KEY_LEFTCTRL:
                    case KEY_RIGHTCTRL:
                        modifier = (modifier | MOD_CTRL) ^ ( input_ev[i].value == 0 ? MOD_CTRL : 0 );
                        break;
                    case KEY_LEFTMETA:
                        modifier = (modifier | MOD_META) ^ ( input_ev[i].value == 0 ? MOD_META : 0 );
                        break;
                    case KEY_MAIL:
                        //Remote Key Email
                        send_key(KEY_C, input_ev[i].value, fd);
                        break;
                    case KEY_HOMEPAGE:
                        //Remote Key WWW
                        send_key(KEY_I, input_ev[i].value, fd);
                        break;
                    case KEY_F4: 
                        if (modifier == MOD_ALT ||
                                (last_modifier == MOD_ALT && input_ev[i].value == 0)) {
                            //Remote Key Close
                            send_key(KEY_M, input_ev[i].value, fd);
                        }
                        break;
                    case KEY_A:
                        if (modifier == (MOD_CTRL | MOD_ALT) ||
                                (last_modifier == (MOD_CTRL | MOD_ALT) && input_ev[i].value == 0)) {
                            // Remote Key Green (A)
                            send_key(KEY_RED, input_ev[i].value, fd);
                        }
                        break;
                    case KEY_2:
                        if (modifier == (MOD_CTRL | MOD_ALT) ||
                                (last_modifier == (MOD_CTRL | MOD_ALT) && input_ev[i].value == 0)) {
                            // Remote Key Orange (B)
                            send_key(KEY_GREEN, input_ev[i].value, fd);
                        }
                        break;
                    case KEY_C:
                        if (modifier == (MOD_CTRL | MOD_ALT) ||
                                (last_modifier == (MOD_CTRL | MOD_ALT) && input_ev[i].value == 0)) {
                            // Remote Key Blue (C)
                            send_key(KEY_YELLOW, input_ev[i].value, fd);
                        }
                        break;
                    case KEY_4:
                        if (modifier == (MOD_CTRL | MOD_ALT) ||
                                (last_modifier == (MOD_CTRL | MOD_ALT) && input_ev[i].value == 0)) {
                            // Remote Key Yellow (D)
                            send_key(KEY_BLUE, input_ev[i].value, fd);
                        }
                        break;
                    case KEY_PREVIOUSSONG: 
                        // REMOTE_KEY_PREVIOUSSONG
                        send_key(KEY_PREVIOUSSONG, input_ev[i].value, fd);
                        break;
                    case KEY_NEXTSONG:
                        // REMOTE_KEY_NEXTSONG
                        send_key(KEY_NEXTSONG, input_ev[i].value, fd);
                        break;
                    case KEY_LEFT:
                        if (modifier == 0) {
                            switch (input_ev[i].value) {
                                case 1:
                                    in_left = 1;
                                    break;
                                case 2:
                                    if (in_left == 1) {
                                        send_key(KEY_LEFT, 1, fd);
                                        in_left = 0;
                                    }
                                    send_key(KEY_LEFT, 2, fd);
                                    break;
                                case 0: 
                                    if (in_left == 1) {
                                        send_key(KEY_LEFT, 1, fd);
                                        in_left = 0;
                                    }
                                    send_key(KEY_LEFT, 0, fd);
                                    break;
                            }
                        } else {
                            in_left = 0;
                        }
                        break;
                    case KEY_RIGHT:
                        if (modifier == 0) {
                            switch (input_ev[i].value) {
                                case 1:
                                    in_right = 1;
                                    break;
                                case 2:
                                    if (in_right == 1) {
                                        send_key(KEY_RIGHT, 1, fd);
                                        in_right = 0;
                                    }
                                    send_key(KEY_RIGHT, 2, fd);
                                    break;
                                case 0:
                                    if (in_right == 1) {
                                        send_key(KEY_RIGHT, 1, fd);
                                        in_right = 0;
                                    }
                                    send_key(KEY_RIGHT, 0, fd);
                                    break;
                            }
                        } else {
                            in_right = 0;
                        }
                        break;
                    case KEY_B:
                        if (modifier == (MOD_SHIFT | MOD_CTRL) || 
                                (last_modifier == (MOD_SHIFT | MOD_CTRL) && input_ev[i].value == 0)) {
                            // REMOTE_KEY_REWIND
                            send_key(KEY_R, input_ev[i].value, fd);
                        }
                        break;
                    case KEY_F:
                        if (modifier == (MOD_SHIFT | MOD_CTRL) || 
                                (last_modifier == (MOD_SHIFT | MOD_CTRL) && input_ev[i].value == 0)) {
                            // KEY_FASTFORWARD
                            send_key(KEY_F, input_ev[i].value, fd);
                        }
                        break;
                    case KEY_PLAYPAUSE:
                        // KEY_PLAYPAUSE
                        send_key(KEY_PLAYPAUSE, input_ev[i].value, fd);
                        break;
                    case KEY_STOPCD:
                        // KEY_STOPCD
                        send_key(KEY_STOPCD, input_ev[i].value, fd);
                        break;
                    case KEY_3:
                        if (modifier == MOD_CTRL ||
                                (last_modifier == MOD_CTRL && input_ev[i].value == 0)) {
                            // Was Fullscreen
                            send_key(KEY_HOME, input_ev[i].value, fd);
                        }
                        break;
                    case KEY_MUTE: 
                        // KEY_MUTE
                        send_key(KEY_MUTE, input_ev[i].value, fd);
                        break;
                    case KEY_VOLUMEDOWN:
                        // printf("KEY_VOLUMEDOWN");
                        send_key(KEY_VOLUMEDOWN, input_ev[i].value, fd);
                        break;
                    case KEY_VOLUMEUP:
                        // printf("KEY_VOLUMEUP");
                        send_key(KEY_VOLUMEUP, input_ev[i].value, fd);
                        break;
                    case KEY_TAB:
                        if (modifier == MOD_ALT || (last_modifier == MOD_ALT &&
                                    input_ev[i].value == 0)) {
                            // Was alt-tab
                            send_key(KEY_Q, input_ev[i].value, fd);
                        } else if (modifier == 0 || (last_modifier == 0 &&
                                    input_ev[i].value == 0)) {
                            // printf("KEY_TAB");
                            send_key(KEY_TAB, input_ev[i].value, fd);
                        }
                        break;
                    case KEY_UP:
                        //printf("KEY_UP");
                        send_key(KEY_UP, input_ev[i].value, fd);
                        break;
                    case KEY_RIGHTMETA:
                        //printf("KEY_RIGHTMETA");
                        send_key(KEY_RIGHTMETA, input_ev[i].value, fd);
                        break;
                    case KEY_BACKSPACE:
                        //printf("KEY_BACKSPACE");
                        send_key(KEY_BACKSPACE, input_ev[i].value, fd);
                        break;
                    case KEY_ENTER:
                        if (modifier == 0 || (last_modifier == 0 && input_ev[i].value == 0)) {
                            //printf("KEY_ENTER");
                            send_key(KEY_ENTER, input_ev[i].value, fd);
                        }
                        break;
                    case KEY_PAGEUP:
                        //printf("KEY_PAGEUP");
                        send_key(KEY_PAGEUP, input_ev[i].value, fd);
                        break;
                    case KEY_O:
                        if (modifier == MOD_CTRL || (last_modifier == MOD_CTRL && input_ev[i].value == 0)) {
                            //printf("CTRL KEY_O"); // Folder
                            send_key(KEY_O, input_ev[i].value, fd);
                        }
                        break;
                    case KEY_DOWN:
                        //printf("KEY_DOWN");
                        send_key(KEY_DOWN, input_ev[i].value, fd);
                        break;
                    case KEY_ESC:
                        //printf("KEY_ESC");
                        send_key(KEY_ESC, input_ev[i].value, fd);
                        break;
                    case KEY_PAGEDOWN:
                        //printf("KEY_PAGEDOWN");
                        send_key(KEY_PAGEDOWN, input_ev[i].value, fd);
                        break;
                    case KEY_E:
                        if (modifier == MOD_META || (last_modifier == MOD_META && input_ev[i].value == 0)) {
                            //printf("KEY_PC");
                            send_key(KEY_COMPUTER, input_ev[i].value, fd);
                        }
                        break;
                    case KEY_D:
                        if (modifier == MOD_META || (last_modifier == MOD_META && input_ev[i].value == 0)) {
                            //printf("KEY_DESKTOP");
                            send_key(KEY_END, input_ev[i].value, fd);
                        }
                        break;
                }
                switch (input_ev[i].code) {
                    case KEY_LEFTSHIFT: 
                    case KEY_RIGHTSHIFT:
                    case KEY_LEFTALT:
                    case KEY_RIGHTALT:
                    case KEY_LEFTCTRL:
                    case KEY_RIGHTCTRL:
                    case KEY_LEFTMETA: break;
                    default: last_modifier = modifier;
                }
            } else if (input_ev[i].type == EV_SYN) {
                send_syn(fd);
            }
        }
        fflush(stdout);
    }

    printf("Exiting.\n");
    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
    result = ioctl(fevdev, EVIOCGRAB, 1);
    close(fevdev);
    return 0;
}
