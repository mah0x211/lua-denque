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


typedef union {
    int ref;
    lua_Number num;
} deq_data_u;

typedef struct deq_st deq_t;
typedef struct deq_elm_st deq_elm_t;

struct deq_elm_st {
    deq_t *dq;
    deq_elm_t *prev;
    deq_elm_t *next;
    int ref;
    int type;
    deq_data_u data;
};


struct deq_st {
    size_t len;
    deq_elm_t *head;
    deq_elm_t *tail;
};


static inline deq_elm_t *dq_newelm( lua_State *L, deq_t *dq )
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
        elm->dq = dq;
        elm->prev = elm->next = NULL;
        elm->type = type;
        elm->data = data;
        elm->ref = lauxh_ref( L );
    }

    return elm;
}


#define DEQ_DATA_RETAIN   0
#define DEQ_DATA_NORETAIN 1

#define dq_pushdata( L, elm, retain ) do{            \
    /* check data type */                           \
    switch( elm->type ){                            \
        /* maintain reference */                    \
        case LUA_TBOOLEAN:                          \
        case LUA_TLIGHTUSERDATA:                    \
        case LUA_TSTRING:                           \
        case LUA_TTABLE:                            \
        case LUA_TFUNCTION:                         \
        case LUA_TUSERDATA:                         \
        case LUA_TTHREAD:                           \
            lauxh_pushref( L, elm->data.ref );      \
            if( retain == DEQ_DATA_NORETAIN ){    \
                lauxh_unref( L, elm->data.ref );    \
            }                                       \
            break;                                  \
        /* copy data */                             \
        case LUA_TNUMBER:                           \
            lua_pushnumber( L, elm->data.num );     \
            break;                                  \
        /* unsupported data */                      \
        default:                                    \
            luaL_error( L, "invalid implements" );  \
    }                                               \
}while(0)


static int unshift_lua( lua_State *L )
{
    deq_t *dq = luaL_checkudata( L, 1, DEQ_MT );
    deq_elm_t *elm = dq_newelm( L, dq );

    if( elm )
    {
        if( dq->len++ ){
            elm->next = dq->head;
            dq->head->prev = elm;
            dq->head = elm;
        }
        else {
            dq->head = dq->tail = elm;
        }

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
    deq_elm_t *elm = dq_newelm( L, dq );

    if( elm )
    {
        if( dq->len++ ){
            elm->prev = dq->tail;
            dq->tail->next = elm;
            dq->tail = elm;
        }
        else {
            dq->head = dq->tail = elm;
        }

        // return element
        lauxh_pushref( L, elm->ref );

        return 1;
    }

    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );

    return 2;
}


static inline int rmhead_lua( lua_State *L, deq_t *dq )
{
    if( dq->head )
    {
        dq->len--;
        dq->head->ref = lauxh_unref( L, dq->head->ref );
        dq_pushdata( L, dq->head, DEQ_DATA_NORETAIN );
        dq->head = dq->head->next;
        if( dq->head ){
            dq->head->prev = NULL;
        }
    }
    else {
        lua_pushnil( L );
    }

    return 1;
}


static int shift_lua( lua_State *L )
{
    deq_t *dq = luaL_checkudata( L, 1, DEQ_MT );

    return rmhead_lua( L, dq );
}


static inline int rmtail_lua( lua_State *L, deq_t *dq )
{
    if( dq->tail )
    {
        dq->len--;
        dq->tail->ref = lauxh_unref( L, dq->tail->ref );
        dq_pushdata( L, dq->tail, DEQ_DATA_NORETAIN );
        dq->tail = dq->tail->prev;
        if( dq->tail ){
            dq->tail->next = NULL;
        }
    }
    else {
        lua_pushnil( L );
    }

    return 1;
}


static int pop_lua( lua_State *L )
{
    deq_t *dq = luaL_checkudata( L, 1, DEQ_MT );

    return rmtail_lua( L, dq );
}


static inline int remove_elm_lua( lua_State *L, deq_t *dq, deq_elm_t *elm )
{
    if( elm->ref != LUA_NOREF )
    {
        if( elm == dq->head ){
            return rmhead_lua( L, dq );
        }
        else if( elm == dq->tail ){
            return rmtail_lua( L, dq );
        }

        dq->len--;
        elm->ref = lauxh_unref( L, elm->ref );
        dq_pushdata( L, elm, DEQ_DATA_NORETAIN );

        elm->prev->next = elm->next;
        elm->next->prev = elm->prev;
    }
    else {
        lua_pushnil( L );
    }

    return 1;
}


static int remove_lua( lua_State *L )
{
    deq_t *dq = luaL_checkudata( L, 1, DEQ_MT );
    deq_elm_t *elm = luaL_checkudata( L, 2, DEQ_ELM_MT );

    return remove_elm_lua( L, dq, elm );
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


static int tostring_lua( lua_State *L )
{
    lua_pushfstring( L, DEQ_MT ": %p", lua_touserdata( L, 1 ) );
    return 1;
}


static int gc_lua( lua_State *L )
{
    deq_t *dq = lua_touserdata( L, 1 );

    if( dq->len )
    {
        deq_elm_t *elm = dq->head;

        do
        {
            elm->ref = lauxh_unref( L, elm->ref );
            if( elm->type != LUA_TNUMBER ){
                lauxh_unref( L, elm->data.ref );
            }

            elm = elm->next;
        } while( elm );
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


static int elm_remove_lua( lua_State *L )
{
    deq_elm_t *elm = luaL_checkudata( L, 1, DEQ_ELM_MT );

    return remove_elm_lua( L, elm->dq, elm );
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


static int elm_data_lua( lua_State *L )
{
    deq_elm_t *elm = luaL_checkudata( L, 1, DEQ_ELM_MT );

    // replace new data
    if( lua_gettop( L ) > 1 )
    {
        int type = lua_type( L, 2 );
        deq_data_u data;

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
                lauxh_argerror( L, 2, "cannot replace the data with a nil" );
        }

        // release old data reference
        switch( elm->type ){
            // maintain reference
            case LUA_TBOOLEAN:
            case LUA_TLIGHTUSERDATA:
            case LUA_TSTRING:
            case LUA_TTABLE:
            case LUA_TFUNCTION:
            case LUA_TUSERDATA:
            case LUA_TTHREAD:
                lauxh_unref( L, elm->data.ref );
                break;
        }

        elm->data = data;
    }

    dq_pushdata( L, elm, DEQ_DATA_RETAIN );

    return 1;
}


static int elm_tostring_lua( lua_State *L )
{
    lua_pushfstring( L, DEQ_ELM_MT ": %p", lua_touserdata( L, 1 ) );
    return 1;
}


static int elm_gc_lua( lua_State *L )
{
    deq_elm_t *elm = lua_touserdata( L, 1 );

    if( elm->ref != LUA_NOREF )
    {
        elm->dq->len--;
        if( elm->type != LUA_TNUMBER ){
            lauxh_unref( L, elm->data.ref );
        }
    }

    return 0;
}


static void init_deq_elm_mt( lua_State *L )
{
    struct luaL_Reg mmethods[] = {
        { "__gc", elm_gc_lua },
        { "__tostring", elm_tostring_lua },
        { NULL, NULL }
    };
    struct luaL_Reg methods[] = {
        { "data", elm_data_lua },
        { "prev", elm_prev_lua },
        { "next", elm_next_lua },
        { "remove", elm_remove_lua },
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


