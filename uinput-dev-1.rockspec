package = "uinput"
version = "dev-1"

source = {
  url = "git://github.com/leafo/lua-uinput.git"
}

description = {
  summary = "A library for creating a virtual keyboard on Linux with uinput",
  homepage = "https://github.com/leafo/lua-uinput",
  license = "MIT"
}

dependencies = {
  "lua >= 5.1"
}

build = {
  type = "builtin",
  modules = {
    uinput = {
      sources = {"uinput.c"},
    }
  }
}

