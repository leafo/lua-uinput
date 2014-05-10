# lua-uinput

Lua bindings for uinput on Linux.

## Install

```bash
$ luarocks install https://raw.githubusercontent.com/leafo/lua-uinput/master/uinput-dev-1.rockspec
```

## Example

```lua
local uinput = require("uinput")

-- luasocket is used for a sleep function
local socket = require("socket")

-- create a virtual keyboard
local keyboard = uinput.create_keyboard()

-- we sleep after creating a keyboard before sending a key press to give X a
-- chance to discover the keyboard. This is not required if your program naturally
-- has some delay before key events are sent or you arent in X
socket.sleep(0.1)

-- press the a key
keyboard:press(uinput.KEY_A)

-- release the a key
keyboard:release(uinput.KEY_A)
```

Make sure `uinput` is loaded before running the script:

```bash
$ modprobe uinput
```

When running your script you'll have to use `sudo` unless you've changed the
permissions on `/dev/uinput`


## Key constants

Can be found by printing the contents of the module:


```lua
for key in pairs(require("uinput")) do
  if key:match("^KEY_") then
    print(key)
  end
end

```


