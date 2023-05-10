/**
 *  Copyright (C) 2016-2022 Masatoshi Fukunaga
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 **/

#include <lauxhlib.h>

#define DEQ_MT     "deque"
#define DEQ_ELM_MT "deque.element"

typedef struct deq_st deq_t;
typedef struct deq_elm_st deq_elm_t;

struct deq_elm_st {
    deq_t *dq;
    deq_elm_t *prev;
    deq_elm_t *next;
    int ref;
    int data;
};

struct deq_st {
    size_t len;
    deq_elm_t *head;
    deq_elm_t *tail;
};

static inline int checkdata(lua_State *L, int idx)
{
    switch (lua_type(L, idx)) {
    // maintain reference
    case LUA_TBOOLEAN:
    case LUA_TLIGHTUSERDATA:
    case LUA_TSTRING:
    case LUA_TTABLE:
    case LUA_TFUNCTION:
    case LUA_TUSERDATA:
    case LUA_TTHREAD:
    case LUA_TNUMBER:
        lua_pushvalue(L, idx);
        return lauxh_ref(L);

    // unsupported data
    default:
        return lauxh_argerror(L, idx, "nil data cannot be set");
    }
}

static inline deq_elm_t *dq_newelm(lua_State *L, deq_t *dq)
{
    int data       = checkdata(L, 2);
    deq_elm_t *elm = lua_newuserdata(L, sizeof(deq_elm_t));

    elm->dq   = dq;
    elm->prev = NULL;
    elm->next = NULL;
    elm->data = data;
    lauxh_setmetatable(L, DEQ_ELM_MT);
    elm->ref = lauxh_ref(L);

    return elm;
}

static int unshift_lua(lua_State *L)
{
    deq_t *dq      = luaL_checkudata(L, 1, DEQ_MT);
    deq_elm_t *elm = dq_newelm(L, dq);

    if (dq->len++) {
        elm->next      = dq->head;
        dq->head->prev = elm;
        dq->head       = elm;
    } else {
        dq->head = dq->tail = elm;
    }

    // return element
    lauxh_pushref(L, elm->ref);

    return 1;
}

static int push_lua(lua_State *L)
{
    deq_t *dq      = luaL_checkudata(L, 1, DEQ_MT);
    deq_elm_t *elm = dq_newelm(L, dq);

    if (dq->len++) {
        elm->prev      = dq->tail;
        dq->tail->next = elm;
        dq->tail       = elm;
    } else {
        dq->head = dq->tail = elm;
    }

    // return element
    lauxh_pushref(L, elm->ref);

    return 1;
}

static inline int rmhead_lua(lua_State *L, deq_t *dq)
{
    if (dq->head) {
        dq->len--;
        lauxh_pushref(L, dq->head->data);
        dq->head->data = lauxh_unref(L, dq->head->data);
        dq->head->ref  = lauxh_unref(L, dq->head->ref);

        dq->head = dq->head->next;
        if (dq->head) {
            dq->head->prev = NULL;
        } else {
            dq->tail = NULL;
        }
    } else {
        lua_pushnil(L);
    }

    return 1;
}

static int shift_lua(lua_State *L)
{
    deq_t *dq = luaL_checkudata(L, 1, DEQ_MT);

    return rmhead_lua(L, dq);
}

static inline int rmtail_lua(lua_State *L, deq_t *dq)
{
    if (dq->tail) {
        dq->len--;
        lauxh_pushref(L, dq->tail->data);
        dq->tail->data = lauxh_unref(L, dq->tail->data);
        dq->tail->ref  = lauxh_unref(L, dq->tail->ref);

        dq->tail = dq->tail->prev;
        if (dq->tail) {
            dq->tail->next = NULL;
        } else {
            dq->head = NULL;
        }
    } else {
        lua_pushnil(L);
    }

    return 1;
}

static int pop_lua(lua_State *L)
{
    deq_t *dq = luaL_checkudata(L, 1, DEQ_MT);

    return rmtail_lua(L, dq);
}

static inline int remove_elm_lua(lua_State *L, deq_t *dq, deq_elm_t *elm)
{
    if (elm->ref != LUA_NOREF) {
        if (elm == dq->head) {
            return rmhead_lua(L, dq);
        } else if (elm == dq->tail) {
            return rmtail_lua(L, dq);
        }

        dq->len--;
        lauxh_pushref(L, elm->data);
        elm->data = lauxh_unref(L, elm->data);
        elm->ref  = lauxh_unref(L, elm->ref);

        elm->prev->next = elm->next;
        elm->next->prev = elm->prev;
    } else {
        lua_pushnil(L);
    }

    return 1;
}

static int remove_lua(lua_State *L)
{
    deq_t *dq      = luaL_checkudata(L, 1, DEQ_MT);
    deq_elm_t *elm = luaL_checkudata(L, 2, DEQ_ELM_MT);

    return remove_elm_lua(L, dq, elm);
}

static int tail_lua(lua_State *L)
{
    deq_t *dq = luaL_checkudata(L, 1, DEQ_MT);

    if (dq->tail) {
        lauxh_pushref(L, dq->tail->ref);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

static int head_lua(lua_State *L)
{
    deq_t *dq = luaL_checkudata(L, 1, DEQ_MT);

    if (dq->head) {
        lauxh_pushref(L, dq->head->ref);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

static int len_lua(lua_State *L)
{
    deq_t *dq = luaL_checkudata(L, 1, DEQ_MT);

    lua_pushinteger(L, dq->len);

    return 1;
}

static int tostring_lua(lua_State *L)
{
    lua_pushfstring(L, DEQ_MT ": %p", lua_touserdata(L, 1));
    return 1;
}

static int gc_lua(lua_State *L)
{
    deq_t *dq      = lua_touserdata(L, 1);
    deq_elm_t *elm = dq->head;

    while (elm) {
        elm->ref  = lauxh_unref(L, elm->ref);
        elm->data = lauxh_unref(L, elm->data);
        elm       = elm->next;
    }

    return 0;
}

static int new_lua(lua_State *L)
{
    deq_t *dq = lua_newuserdata(L, sizeof(deq_t));

    lauxh_setmetatable(L, DEQ_MT);
    dq->len  = 0;
    dq->head = dq->tail = NULL;

    return 1;
}

static int elm_remove_lua(lua_State *L)
{
    deq_elm_t *elm = luaL_checkudata(L, 1, DEQ_ELM_MT);

    return remove_elm_lua(L, elm->dq, elm);
}

static int elm_next_lua(lua_State *L)
{
    deq_elm_t *elm = luaL_checkudata(L, 1, DEQ_ELM_MT);

    if (elm->next) {
        lauxh_pushref(L, elm->next->ref);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

static int elm_prev_lua(lua_State *L)
{
    deq_elm_t *elm = luaL_checkudata(L, 1, DEQ_ELM_MT);

    if (elm->prev) {
        lauxh_pushref(L, elm->prev->ref);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

static int elm_data_lua(lua_State *L)
{
    deq_elm_t *elm = luaL_checkudata(L, 1, DEQ_ELM_MT);

    // replace new data
    if (lua_gettop(L) > 1) {
        int data = checkdata(L, 2);
        // release old data reference
        lauxh_unref(L, elm->data);
        elm->data = data;
    }

    lauxh_pushref(L, elm->data);

    return 1;
}

static int elm_tostring_lua(lua_State *L)
{
    lua_pushfstring(L, DEQ_ELM_MT ": %p", lua_touserdata(L, 1));
    return 1;
}

static int elm_gc_lua(lua_State *L)
{
    deq_elm_t *elm = lua_touserdata(L, 1);

    if (elm->data != LUA_NOREF) {
        lauxh_unref(L, elm->data);
    }

    return 0;
}

static void init_mt(lua_State *L, const char *tname, struct luaL_Reg *mmethods,
                    struct luaL_Reg *methods)
{
    // create metatable
    if (luaL_newmetatable(L, tname)) {
        struct luaL_Reg *ptr = mmethods;

        // metamethods
        while (ptr->name) {
            lauxh_pushfn2tbl(L, ptr->name, ptr->func);
            ptr++;
        }
        // methods
        ptr = methods;
        lua_pushstring(L, "__index");
        lua_newtable(L);
        while (ptr->name) {
            lauxh_pushfn2tbl(L, ptr->name, ptr->func);
            ptr++;
        }
        lua_rawset(L, -3);
    }

    // remove metatable from stack
    lua_pop(L, 1);
}

static void init_deq_elm_mt(lua_State *L)
{
    struct luaL_Reg mmethods[] = {
        {"__gc",       elm_gc_lua      },
        {"__tostring", elm_tostring_lua},
        {NULL,         NULL            }
    };
    struct luaL_Reg methods[] = {
        {"data",   elm_data_lua  },
        {"prev",   elm_prev_lua  },
        {"next",   elm_next_lua  },
        {"remove", elm_remove_lua},
        {NULL,     NULL          }
    };

    init_mt(L, DEQ_ELM_MT, mmethods, methods);
}

static void init_deq_mt(lua_State *L)
{
    struct luaL_Reg mmethods[] = {
        {"__gc",       gc_lua      },
        {"__tostring", tostring_lua},
        {"__len",      len_lua     },
        {NULL,         NULL        }
    };
    struct luaL_Reg methods[] = {
        {"head",    head_lua   },
        {"tail",    tail_lua   },
        {"unshift", unshift_lua},
        {"shift",   shift_lua  },
        {"push",    push_lua   },
        {"pop",     pop_lua    },
        {"remove",  remove_lua },
        {NULL,      NULL       }
    };

    init_mt(L, DEQ_MT, mmethods, methods);
}

LUALIB_API int luaopen_deque(lua_State *L)
{
    init_deq_mt(L);
    init_deq_elm_mt(L);

    // register allocator
    lua_createtable(L, 0, 1);
    lauxh_pushfn2tbl(L, "new", new_lua);
    lua_createtable(L, 0, 1);
    lauxh_pushfn2tbl(L, "__call", new_lua);
    lua_setmetatable(L, -2);

    return 1;
}
