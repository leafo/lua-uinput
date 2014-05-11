-- Converts the sustain pedal on a midi device into a shfit key

file = io.popen "amidi -d -p hw:2,0,0", "r"

hex = "([%a%d][%a%d])"
patt = "#{hex} #{hex} #{hex}$"

down = false

uinput = require "uinput"
keyboard = uinput.create_keyboard!

read_event = (file) ->
  buff = ""
  while true
    buff ..= file\read 1
    a,b,c = buff\match patt
    return a,b,c if a

while true
  a,b,c = read_event file
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

