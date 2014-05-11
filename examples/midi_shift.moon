-- Converts the sustain pedal on a midi device into a shfit key

file = io.popen "amidi -d -p hw:2,0,0", "r"

down = false

uinput = require "uinput"
keyboard = uinput.create_keyboard!

read_event = (file) ->
  event = {}
  curr = ""
  while true
    char = file\read 1
    continue if char\match "%s"

    curr ..= char
    if #curr == 2
      table.insert event, curr
      if #event == 3
        break
      else
        curr = ""

  unpack event

while true
  a,b,c = read_event file

  continue unless a
  switch a
    when "B0"
      if c == "00"
        continue if down
        down = true
        keyboard\press uinput.KEY_LEFTSHIFT
      else
        continue unless down
        down = false
        keyboard\release uinput.KEY_LEFTSHIFT

