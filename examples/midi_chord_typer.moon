
file = io.popen "amidi -d -p hw:2,0,0", "r"
hex = "([%a%d][%a%d])"
patt = "#{hex} #{hex} #{hex}$"

total_down = 0
is_down = {}

uinput = require "uinput"
keyboard = uinput.create_keyboard!

OFFSET = 48

-- Left hand
-- thumb: 7
-- pointer: 5
-- middle: 4
-- ring: 2
-- pinky: 0

-- Right hand
-- thumb: 12
-- pointer: 14
-- middle: 16
-- ring: 17
-- pinky: 19

CHORDS = {
  "0": { "KEY_A" }
  "2": { "KEY_B" }
  "4": { "KEY_C" }
  "5": { "KEY_D" }
  "7": { "KEY_E" }

  "12": { "KEY_F" }
  "14": { "KEY_G" }
  "16": { "KEY_H" }
  "17": { "KEY_I" }
  "19": { "KEY_J" }

  -- thumb on opposite
  "0-12": { "KEY_K" }
  "2-12": { "KEY_L" }
  "4-12": { "KEY_M" }
  "5-12": { "KEY_N" }
  "7-12": { "KEY_O" }

  "7-13": { "KEY_P" }
  "7-14": { "KEY_Q" }
  "7-16": { "KEY_R" }
  "7-17": { "KEY_S" }
  "7-19": { "KEY_T" }

  -- pointer on opposite
  "0-14": { "KEY_U" }
  "2-14": { "KEY_V" }
  "4-14": { "KEY_W" }
  "5-14": { "KEY_X" }
  "7-14": { "KEY_Y" }

  "5-12": { "KEY_Z" }
  "5-14": { "KEY_SPACE" }
  "5-16": { "KEY_BACKSPACE" }
  "5-17": { "KEY_ENTER" }
  -- "5-19": { }
}

send_keys = (keys) ->
  for k in *keys
    keyboard\press assert uinput[k], "invalid key"

  for k in *keys
    keyboard\release assert uinput[k], "invalid key"

handle_chord = (keys) ->
  for k, key in pairs keys
    keys[k] = keys[k] - OFFSET

  table.sort keys
  chord_string = table.concat keys, "-"
  keys = CHORDS[chord_string]
  if keys
    send_keys keys
  else
    print "unknown chord", chord_string

read_event = (file) ->
  buff = ""
  while true
    buff ..= file\read 1
    a,b,c = buff\match patt
    return a,b,c if a

while true
  a,b,c = read_event file

  event = tonumber a, 16
  key = tonumber b, 16
  velocity = tonumber c, 16

  switch event
    when 176 -- foot pedal
      if velocity == 0
        keyboard\press uinput.KEY_LEFTSHIFT
      else
        keyboard\release uinput.KEY_LEFTSHIFT

    when 144 -- down
      continue if is_down[key]
      is_down[key] = true
      total_down += 1

    when 128 -- up
      continue unless is_down[key]
      total_down -= 1

      if total_down == 0
        handle_chord [key for key in pairs is_down]
        is_down = {}



