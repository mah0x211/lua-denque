package = "deque"
version = "0.4.5-1"
source = {
    url = "git+https://github.com/mah0x211/lua-deque.git",
    tag = "v0.4.5",
}
description = {
    summary = "double-ended queue",
    homepage = "https://github.com/mah0x211/lua-deque",
    license = "MIT/X11",
    maintainer = "Masatoshi Fukunaga"
}
dependencies = {
    "lua >= 5.1",
    "lauxhlib >= 0.3.1",
}
build = {
    type = "make",
    build_variables = {
        PACKAGE         = "deque",
        CFLAGS          = "$(CFLAGS)",
        WARNINGS        = "-Wall -Wno-trigraphs -Wmissing-field-initializers -Wreturn-type -Wmissing-braces -Wparentheses -Wno-switch -Wunused-function -Wunused-label -Wunused-parameter -Wunused-variable -Wunused-value -Wuninitialized -Wunknown-pragmas -Wshadow -Wsign-compare",
        CPPFLAGS        = "-I$(LUA_INCDIR)",
        LDFLAGS         = "$(LIBFLAG)",
        LIB_EXTENSION   = "$(LIB_EXTENSION)",
        DEQUE_COVERAGE  = "$(DEQUE_COVERAGE)",
    },
    install_variables = {
        PACKAGE         = "deque",
        LIB_EXTENSION   = "$(LIB_EXTENSION)",
        CONFDIR         = '$(CONFDIR)',
        LIBDIR          = "$(LIBDIR)",
    }
}
