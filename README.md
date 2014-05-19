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


# Contact

Author: Leaf Corcoran (leafo) ([@moonscript](http://twitter.com/moonscript))  
Email: leafot@gmail.com  
Homepage: <http://leafo.net>  

## License (MIT)

Copyright (C) 2014 by Leaf Corcoran

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

