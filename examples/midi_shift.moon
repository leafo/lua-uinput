-- Converts the sustain pedal on a midi device into a shfit key

file = io.popen "amidi -d -p hw:2,0,0", "r"

hex = "([%a%d][%a%d])"
patt = "^#{hex} #{hex} #{hex}$"

down = false

uinput = require "uinput"
keyboard = uinput.create_keyboard!

while true
  line = file\read "*l"

  a,b,c = line\match patt
  continue unless a
  switch a
    when "B0"
      if c == "7F"
        continue if down
        down = true
        keyboard\press uinput.KEY_LEFTSHIFT
      else
        continue unless down
        down = false
        keyboard\release uinput.KEY_LEFTSHIFT

