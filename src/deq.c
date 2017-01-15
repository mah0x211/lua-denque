/*
 *  Copyright (C) 2016 Masatoshi Teruya
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
 *
 *
 *  deq.c
 *  lua-deq
 *
 *  Created by Masatoshi Teruya on 17/01/02.
 */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <errno.h>
#include <lua.h>
#include <lauxlib.h>
#include "lauxhlib.h"


#define DEQ_MT            "deq"
#define DEQ_ELM_MT        "deq.element"
#define DEQ_WEAKTBL_MT    "deq.weaktbl"


typedef union {
    int ref;
    lua_Number num;
} deq_data_u;

typedef struct deq_elm_st deq_elm_t;

struct deq_elm_st {
    deq_elm_t *prev;
    deq_elm_t *next;
    int ref;
    int type;
    deq_data_u data;
};


typedef struct {
    size_t len;
    deq_elm_t *head;
    deq_elm_t *tail;
} deq_t;


static inline deq_elm_t *dq_newelm( lua_State *L )
{
    deq_elm_t *elm = NULL;
    int type = lua_type( L, 2 );
    deq_data_u data = { 0 };

    // check data type
    switch( type ){
        // maintain reference
        case LUA_TBOOLEAN:
        case LUA_TLIGHTUSERDATA:
        case LUA_TSTRING:
        case LUA_TTABLE:
        case LUA_TFUNCTION:
        case LUA_TUSERDATA:
        case LUA_TTHREAD:
            lua_settop( L, 2 );
            data.ref = lauxh_ref( L );
            break;

        // copy data
        case LUA_TNUMBER:
            data.num = lua_tonumber( L, 2 );
            break;

        // unsupported data
        default:
            lauxh_argerror( L, 2, "cannot add a nil" );
    }


    if( ( elm = lua_newuserdata( L, sizeof( deq_elm_t ) ) ) ){
        lauxh_setmetatable( L, DEQ_ELM_MT );
        elm->prev = elm->next = NULL;
        elm->type = type;
        elm->data = data;
        elm->ref = lauxh_ref( L );
    }

    return elm;
}


static inline void dq_pushdata( lua_State *L, deq_elm_t *elm )
{
    // check data type
    switch( elm->type ){
        // maintain reference
        case LUA_TBOOLEAN:
        case LUA_TLIGHTUSERDATA:
        case LUA_TSTRING:
        case LUA_TTABLE:
        case LUA_TFUNCTION:
        case LUA_TUSERDATA:
        case LUA_TTHREAD:
            lauxh_pushref( L, elm->data.ref );
            lauxh_unref( L, elm->data.ref );
            break;

        // copy data
        case LUA_TNUMBER:
            lua_pushnumber( L, elm->data.num );
            break;

        // unsupported data
        default:
            luaL_error( L, "invalid implements" );
    }
}


static int unshift_lua( lua_State *L )
{
    deq_t *dq = luaL_checkudata( L, 1, DEQ_MT );
    deq_elm_t *elm = dq_newelm( L );

    if( elm )
    {
        if( dq->len ){
            elm->next = dq->head;
            dq->head->prev = elm;
            dq->head = elm;
        }
        else {
            dq->head = dq->tail = elm;
        }

        dq->len++;
        // return element
        lauxh_pushref( L, elm->ref );

        return 1;
    }

    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );

    return 2;
}


static int push_lua( lua_State *L )
{
    deq_t *dq = luaL_checkudata( L, 1, DEQ_MT );
    deq_elm_t *elm = dq_newelm( L );

    if( elm )
    {
        if( dq->len ){
            elm->prev = dq->tail;
            dq->tail->next = elm;
            dq->tail = elm;
        }
        else {
            dq->head = dq->tail = elm;
        }

        dq->len++;
        // return element
        lauxh_pushref( L, elm->ref );

        return 1;
    }

    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );

    return 2;
}


static int shift_lua( lua_State *L )
{
    deq_t *dq = luaL_checkudata( L, 1, DEQ_MT );

    if( dq->head )
    {
        lauxh_unref( L, dq->head->ref );
        dq_pushdata( L, dq->head );

        dq->len--;
        if( dq->len ){
            dq->head = dq->head->next;
            dq->head->prev = NULL;
        }
        else {
            dq->head = dq->tail = NULL;
        }

        return 1;
    }

    return 0;
}


static int pop_lua( lua_State *L )
{
    deq_t *dq = luaL_checkudata( L, 1, DEQ_MT );

    if( dq->tail )
    {
        lauxh_unref( L, dq->tail->ref );
        dq_pushdata( L, dq->tail );

        dq->len--;
        if( dq->len ){
            dq->tail = dq->tail->prev;
            dq->tail->next = NULL;
        }
        else {
            dq->head = dq->tail = NULL;
        }

        return 1;
    }

    return 0;
}


static int remove_lua( lua_State *L )
{
    deq_t *dq = luaL_checkudata( L, 1, DEQ_MT );
    deq_elm_t *target = luaL_checkudata( L, 2, DEQ_ELM_MT );

    dq_pushdata( L, target );
    lauxh_unref( L, target->ref );

    if( dq->len == 1 ){
        lauxh_unref( L, dq->head->ref );
        dq->head = dq->tail = NULL;
    }
    else if( target == dq->head ){
        dq->head = dq->head->next;
        dq->head->prev = NULL;
    }
    else if( target == dq->tail ){
        dq->tail = dq->tail->prev;
        dq->tail->next = NULL;
    }
    else {
        target->prev->next = target->next;
        target->next->prev = target->prev;
    }
    dq->len--;

    return 1;
}


static int tail_lua( lua_State *L )
{
    deq_t *dq = luaL_checkudata( L, 1, DEQ_MT );

    if( dq->tail ){
        lauxh_pushref( L, dq->tail->ref );
    }
    else {
        lua_pushnil( L );
    }

    return 1;
}


static int head_lua( lua_State *L )
{
    deq_t *dq = luaL_checkudata( L, 1, DEQ_MT );

    if( dq->head ){
        lauxh_pushref( L, dq->head->ref );
    }
    else {
        lua_pushnil( L );
    }

    return 1;
}


static int len_lua( lua_State *L )
{
    deq_t *dq = luaL_checkudata( L, 1, DEQ_MT );

    lua_pushinteger( L, dq->len );

    return 1;
}


static int tostringelm_lua( lua_State *L )
{
    lua_pushfstring( L, DEQ_ELM_MT ": %p", lua_touserdata( L, 1 ) );
    return 1;
}


static int tostring_lua( lua_State *L )
{
    lua_pushfstring( L, DEQ_MT ": %p", lua_touserdata( L, 1 ) );
    return 1;
}


static int gc_lua( lua_State *L )
{
    deq_t *dq = luaL_checkudata( L, 1, DEQ_MT );
    deq_elm_t *elm = dq->head;

    while( elm ){
        lauxh_unref( L, elm->ref );
        elm = elm->next;
    }

    return 0;
}


static int new_lua( lua_State *L )
{
    deq_t *dq = lua_newuserdata( L, sizeof( deq_t ) );

    if( dq ){
        lauxh_setmetatable( L, DEQ_MT );
        dq->len = 0;
        dq->head = dq->tail = NULL;
        return 1;
    }

    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );

    return 2;
}


static int elm_next_lua( lua_State *L )
{
    deq_elm_t *elm = luaL_checkudata( L, 1, DEQ_ELM_MT );

    if( elm->next ){
        lauxh_pushref( L, elm->next->ref );
    }
    else {
        lua_pushnil( L );
    }

    return 1;
}


static int elm_prev_lua( lua_State *L )
{
    deq_elm_t *elm = luaL_checkudata( L, 1, DEQ_ELM_MT );

    if( elm->prev ){
        lauxh_pushref( L, elm->prev->ref );
    }
    else {
        lua_pushnil( L );
    }

    return 1;
}


static void init_deq_elm_mt( lua_State *L )
{
    struct luaL_Reg mmethods[] = {
        { "__tostring", tostringelm_lua },
        { NULL, NULL }
    };
    struct luaL_Reg methods[] = {
        { "prev", elm_prev_lua },
        { "next", elm_next_lua },
        { NULL, NULL }
    };
    struct luaL_Reg *ptr = mmethods;

    // failed to create metatable
    if( !luaL_newmetatable( L, DEQ_ELM_MT ) ){
        luaL_error( L, "failed to create metatable " DEQ_ELM_MT );
    }

    // metamethods
    while( ptr->name ){
        lauxh_pushfn2tbl( L, ptr->name, ptr->func );
        ptr++;
    }
    // methods
    ptr = methods;
    lua_pushstring( L, "__index" );
    lua_newtable( L );
    while( ptr->name ){
        lauxh_pushfn2tbl( L, ptr->name, ptr->func );
        ptr++;
    }
    lua_rawset( L, -3 );
    // remove metatable from stack
    lua_pop( L, 1 );
}


static void init_deq_mt( lua_State *L )
{
    struct luaL_Reg mmethods[] = {
        { "__gc", gc_lua },
        { "__tostring", tostring_lua },
        { "__len", len_lua },
        { NULL, NULL }
    };
    struct luaL_Reg methods[] = {
        { "head", head_lua },
        { "tail", tail_lua },
        { "unshift", unshift_lua },
        { "shift", shift_lua },
        { "push", push_lua },
        { "pop", pop_lua },
        { "remove", remove_lua },
        { NULL, NULL }
    };
    struct luaL_Reg *ptr = mmethods;

    // failed to create metatable
    if( !luaL_newmetatable( L, DEQ_MT ) ){
        luaL_error( L, "failed to create metatable " DEQ_MT );
    }

    // metamethods
    while( ptr->name ){
        lauxh_pushfn2tbl( L, ptr->name, ptr->func );
        ptr++;
    }
    // methods
    ptr = methods;
    lua_pushstring( L, "__index" );
    lua_newtable( L );
    while( ptr->name ){
        lauxh_pushfn2tbl( L, ptr->name, ptr->func );
        ptr++;
    }
    lua_rawset( L, -3 );
    // remove metatable from stack
    lua_pop( L, 1 );
}


LUALIB_API int luaopen_deq( lua_State *L )
{
    init_deq_mt( L );
    init_deq_elm_mt( L );

    // register allocator
    lua_createtable( L, 0, 1 );
    lauxh_pushfn2tbl( L, "new", new_lua );

    return 1;
}


