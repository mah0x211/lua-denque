package = "deq"
version = "0.2.0-1"
source = {
    url = "git://github.com/mah0x211/lua-deq.git",
    tag = "v0.2.0"
}
description = {
    summary = "double-ended queue",
    homepage = "https://github.com/mah0x211/lua-deq",
    license = "MIT/X11",
    maintainer = "Masatoshi Teruya"
}
dependencies = {
    "lua >= 5.1",
}
build = {
    type = "builtin",
    modules = {
        deq = "deq.lua"
    }
}
