
-- adapted from: http://thiemonge.org/getting-started-with-uinput

ffi = require "ffi"
import bor from require "bit"

ffi.cdef [[
  typedef int ssize_t;

  static const int O_WRONLY = 1;
  static const int O_NONBLOCK = 2048;

  static const int EV_SYN = 0;
  static const int EV_KEY = 1;

  static const int KEY_LEFTSHIFT = 42;

  static const int UI_SET_EVBIT = 1074025828;
  static const int UI_SET_KEYBIT = 1074025829;
  static const int UI_SET_RELBIT = 1074025830;
  static const int UI_DEV_CREATE = 21761;
  static const int UI_DEV_DESTROY = 21762;

  static const int BUS_USB = 3;

  int open(const char *path, int oflag, ...);
  int ioctl(int fildes, int request, ...);
  ssize_t write(int fildes, const void *buf, size_t nbyte);

  unsigned int sleep(unsigned int seconds);

  typedef unsigned int __u32;
  typedef signed int __s32;

  typedef unsigned short __u16;
  typedef signed short __s16;

  struct input_id {
    __u16 bustype;
    __u16 vendor;
    __u16 product;
    __u16 version;
  };

  struct uinput_user_dev {
    char name[80];
    struct input_id id;
    __u32 ff_effects_max;
    __s32 absmax[64];
    __s32 absmin[64];
    __s32 absfuzz[64];
    __s32 absflat[64];
  };

  struct timeval {
    long tv_sec;
    long tv_usec;
  };

  struct input_event {
    struct timeval time;
    __u16 type;
    __u16 code;
    __s32 value;
  };
]]

import
  O_WRONLY, O_NONBLOCK
  EV_KEY, EV_SYN
  UI_SET_EVBIT, UI_SET_KEYBIT
  UI_DEV_CREATE, UI_DEV_DESTROY

  KEY_LEFTSHIFT
  BUS_USB
  from ffi.C

fd = ffi.C.open "/dev/uinput", bor(O_WRONLY, O_NONBLOCK)
print bor(O_WRONLY, O_NONBLOCK)

if fd < 0
  print "Failed to open uinput file #{fd}"
  os.exit 1

assert 0 <= ffi.C.ioctl fd, UI_SET_EVBIT, EV_KEY
assert 0 <= ffi.C.ioctl fd, UI_SET_EVBIT, EV_SYN
assert 0 <= ffi.C.ioctl fd, UI_SET_KEYBIT, KEY_LEFTSHIFT

device = ffi.new "struct uinput_user_dev"
device.name = "uinput-sample"
device.id.bustype = BUS_USB
device.id.version = 1
device.id.vendor = 1
device.id.product = 1

assert 0 <= ffi.C.write fd, device, ffi.sizeof(device)
assert 0 <= ffi.C.ioctl fd, UI_DEV_CREATE

print "Creating..."
ffi.C.sleep 10 -- wait for X to find it

send_event = (typ, code, value)->
  input = ffi.new "struct input_event"
  input.type = typ if typ
  input.code = code if code
  input.value = value if value
  assert 0 <= ffi.C.write fd, input, ffi.sizeof(input)

send_event EV_KEY, KEY_LEFTSHIFT, 1
send_event EV_SYN

print "Shift on..."
ffi.C.sleep 10

send_event EV_KEY, KEY_LEFTSHIFT, 0
send_event EV_SYN

print "Shift off..."
ffi.C.sleep 1

ffi.C.ioctl fd, UI_DEV_DESTROY
