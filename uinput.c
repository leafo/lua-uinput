
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stropts.h>
#include <string.h>

#include <linux/uinput.h>
#include <linux/input.h>

#include <stdbool.h>

typedef struct {
  int fd;
  int open;
} Keyboard;

struct KeyTuple {
  const char* name;
  const int code;
};

#define $(val) {#val, val}

static const struct KeyTuple key_names[] = {
  $(KEY_RESERVED),
  $(KEY_ESC),
  $(KEY_1),
  $(KEY_2),
  $(KEY_3),
  $(KEY_4),
  $(KEY_5),
  $(KEY_6),
  $(KEY_7),
  $(KEY_8),
  $(KEY_9),
  $(KEY_0),
  $(KEY_MINUS),
  $(KEY_EQUAL),
  $(KEY_BACKSPACE),
  $(KEY_TAB),
  $(KEY_Q),
  $(KEY_W),
  $(KEY_E),
  $(KEY_R),
  $(KEY_T),
  $(KEY_Y),
  $(KEY_U),
  $(KEY_I),
  $(KEY_O),
  $(KEY_P),
  $(KEY_LEFTBRACE),
  $(KEY_RIGHTBRACE),
  $(KEY_ENTER),
  $(KEY_LEFTCTRL),
  $(KEY_A),
  $(KEY_S),
  $(KEY_D),
  $(KEY_F),
  $(KEY_G),
  $(KEY_H),
  $(KEY_J),
  $(KEY_K),
  $(KEY_L),
  $(KEY_SEMICOLON),
  $(KEY_APOSTROPHE),
  $(KEY_GRAVE),
  $(KEY_LEFTSHIFT),
  $(KEY_BACKSLASH),
  $(KEY_Z),
  $(KEY_X),
  $(KEY_C),
  $(KEY_V),
  $(KEY_B),
  $(KEY_N),
  $(KEY_M),
  $(KEY_COMMA),
  $(KEY_DOT),
  $(KEY_SLASH),
  $(KEY_RIGHTSHIFT),
  $(KEY_KPASTERISK),
  $(KEY_LEFTALT),
  $(KEY_SPACE),
  $(KEY_CAPSLOCK),
  $(KEY_F1),
  $(KEY_F2),
  $(KEY_F3),
  $(KEY_F4),
  $(KEY_F5),
  $(KEY_F6),
  $(KEY_F7),
  $(KEY_F8),
  $(KEY_F9),
  $(KEY_F10),
  $(KEY_NUMLOCK),
  $(KEY_SCROLLLOCK),
  $(KEY_KP7),
  $(KEY_KP8),
  $(KEY_KP9),
  $(KEY_KPMINUS),
  $(KEY_KP4),
  $(KEY_KP5),
  $(KEY_KP6),
  $(KEY_KPPLUS),
  $(KEY_KP1),
  $(KEY_KP2),
  $(KEY_KP3),
  $(KEY_KP0),
  $(KEY_KPDOT),

  $(KEY_ZENKAKUHANKAKU),
  $(KEY_102ND),
  $(KEY_F11),
  $(KEY_F12),
  $(KEY_RO),
  $(KEY_KATAKANA),
  $(KEY_HIRAGANA),
  $(KEY_HENKAN),
  $(KEY_KATAKANAHIRAGANA),
  $(KEY_MUHENKAN),
  $(KEY_KPJPCOMMA),
  $(KEY_KPENTER),
  $(KEY_RIGHTCTRL),
  $(KEY_KPSLASH),
  $(KEY_SYSRQ),
  $(KEY_RIGHTALT),
  $(KEY_LINEFEED),
  $(KEY_HOME),
  $(KEY_UP),
  $(KEY_PAGEUP),
  $(KEY_LEFT),
  $(KEY_RIGHT),
  $(KEY_END),
  $(KEY_DOWN),
  $(KEY_PAGEDOWN),
  $(KEY_INSERT),
  $(KEY_DELETE),
  $(KEY_MACRO),
  $(KEY_MUTE),
  $(KEY_VOLUMEDOWN),
  $(KEY_VOLUMEUP),
  $(KEY_POWER),
  $(KEY_KPEQUAL),
  $(KEY_KPPLUSMINUS),
  $(KEY_PAUSE),
  $(KEY_SCALE),

  {0, 0}
};

#define check_keyboard(l, idx)\
  (Keyboard*)luaL_checkudata(l, idx, "uinput_keyboard")

static int create_keyboard(lua_State *l) {
  int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

  if (fd < 0) {
    return luaL_error(l, "failed to open /dev/uinput: %d", fd);
  }

  ioctl(fd, UI_SET_EVBIT, EV_KEY);
  ioctl(fd, UI_SET_EVBIT, EV_SYN);

  int i = 0;
  while (true) {
    struct KeyTuple tuple = key_names[i++];

    if (tuple.name == 0) {
      break;
    }

    ioctl(fd, UI_SET_KEYBIT, tuple.code);
  }

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

  Keyboard* keyboard = lua_newuserdata(l, sizeof(Keyboard));
  luaL_getmetatable(l, "uinput_keyboard");
  lua_setmetatable(l, -2);

  keyboard->fd = fd;
  keyboard->open = true;

  return 1;
}

int send_key(int fd, int code, int value) {
  struct input_event ev;
  memset(&ev, 0, sizeof(ev));

  ev.type = EV_KEY;
  ev.code = code;
  ev.value = value;
  write(fd, &ev, sizeof(ev));

  ev.type = EV_SYN;
  ev.code = 0;
  ev.value = 0;
  write(fd, &ev, sizeof(ev));
}

static int keyboard_press(lua_State* l) {
  Keyboard* keyboard = check_keyboard(l, 1);
  int key = luaL_checkint(l, 2);
  send_key(keyboard->fd, key, 1);
  return 0;
}

static int keyboard_release(lua_State *l) {
  Keyboard* keyboard = check_keyboard(l, 1);
  int key = luaL_checkint(l, 2);
  send_key(keyboard->fd, key, 0);
  return 0;
}

static int keyboard_close(lua_State *l) {
  Keyboard* keyboard = check_keyboard(l, 1);

  if (keyboard->open) {
    ioctl(keyboard->fd, UI_DEV_DESTROY);
    close(keyboard->fd);
    keyboard->open = false;
  }

  return 0;
}

static int keyboard_gc(lua_State *l) {
  return keyboard_close(l);
}

static const struct luaL_Reg uinput_funcs[] = {
  {"create_keyboard", create_keyboard},
  {NULL, NULL}
};

static const struct luaL_Reg keyboard_methods[] = {
  {"close", keyboard_close},
  {"press", keyboard_press},
  {"release", keyboard_release},
  {NULL, NULL}
};

int luaopen_uinput(lua_State *l) {
  luaL_newmetatable(l, "uinput_keyboard");

  lua_newtable(l);
  luaL_register(l, NULL, keyboard_methods);
  lua_setfield(l, -2, "__index");

  lua_pushcfunction(l, keyboard_gc);
  lua_setfield(l, -2, "__gc");

  luaL_register(l, "uinput", uinput_funcs);

  int i = 0;
  while (true) {
    struct KeyTuple tuple = key_names[i++];

    if (tuple.name == 0) {
      break;
    }

    lua_pushnumber(l, tuple.code);
    lua_setfield(l, -2, tuple.name);
  }

  return 1;
}

