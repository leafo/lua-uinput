
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stropts.h>
#include <string.h>

#include <linux/uinput.h>
#include <linux/input.h>

#define $(val) printf(#val ": %d\n", val)


int main(int argc, char** argv;) {
  int fd;

	printf("%d\n", O_WRONLY | O_NONBLOCK);
  fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

  if (fd < 0) {
    printf("failed to open: %d\n", fd);
    exit(1);
  }

	ioctl(fd, UI_SET_EVBIT, EV_KEY);
	ioctl(fd, UI_SET_EVBIT, EV_SYN);
	ioctl(fd, UI_SET_KEYBIT, KEY_LEFTSHIFT);

	struct uinput_user_dev uidev;
	memset(&uidev, 0, sizeof(uidev));
	snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample");
	uidev.id.bustype = BUS_USB;
	uidev.id.vendor  = 1;
	uidev.id.product = 1;
	uidev.id.version = 1;

	write(fd, &uidev, sizeof(uidev));
	ioctl(fd, UI_DEV_CREATE);

	puts("creating...");
	sleep(1);

	struct input_event ev;
	memset(&ev, 0, sizeof(ev));

	ev.type = EV_KEY;
	ev.code = KEY_LEFTSHIFT;
	ev.value = 1;
	write(fd, &ev, sizeof(ev));

	ev.type = EV_SYN;
	ev.code = 0;
	ev.value = 0;
	write(fd, &ev, sizeof(ev));

	puts("shift on...\n");
	sleep(1);

	// --

	ev.type = EV_KEY;
	ev.code = KEY_LEFTSHIFT;
	ev.value = 0;
	write(fd, &ev, sizeof(ev));

	ev.type = EV_SYN;
	ev.code = 0;
	ev.value = 0;
	write(fd, &ev, sizeof(ev));

	puts("shift off...\n");
	sleep(1);

	ioctl(fd, UI_DEV_DESTROY);
	close(fd);

}
