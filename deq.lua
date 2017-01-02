--[[

  Copyright (C) 2016 Masatoshi Teruya

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.

  deq.lua
  lua-deq
  Created by Masatoshi Teruya on 16/12/27.

--]]
--- file scope variables
local setmetatable = setmetatable;


--- createElement
-- @param deq
-- @param data
-- @return elm
local function createElement( deq, data )
    local elm = { data = data };

    -- add to used
    deq.used[elm] = true;

    return elm;
end


--- unrefElement
-- @param deq
-- @param elm
local function unrefElement( deq, elm )
    -- remove references
    elm.prev = nil;
    elm.next = nil;
    elm.data = nil;
    deq.used[elm] = nil;
end


--- class Deq
local Deq = {};


--- unshift
-- @param data
-- @return elm
function Deq:unshift( data )
    local elm = createElement( self, data );

    -- set elm as head and tail
    if self.len == 0 then
        self.len = 1;
        self.head = elm;
        self.tail = elm;
    -- insert into head
    else
        self.len = self.len + 1;
        elm.next = self.head;
        self.head.prev = elm;
        self.head = elm;
    end

    return elm;
end


--- push
-- @param data
-- @return elm
function Deq:push( data )
    local elm = createElement( self, data );

    -- set elm as head and tail
    if self.len == 0 then
        self.len = 1;
        self.head = elm;
        self.tail = elm;
    -- insert into tail
    else
        self.len = self.len + 1;
        elm.prev = self.tail;
        self.tail.next = elm;
        self.tail = elm;
    end

    return elm;
end


--- shift
-- @return data
function Deq:shift()
    local elm = self.head;

    if elm then
        local data = elm.data;

        -- remove head and tail
        if self.len == 1 then
            self.len = 0;
            self.head = nil;
            self.tail = nil;
        -- set head to next
        else
            self.len = self.len - 1;
            self.head = elm.next;
            self.head.prev = nil;
        end

        -- remove references
        unrefElement( self, elm );

        return data;
    end
end


--- pop
-- @return data
function Deq:pop()
    local elm = self.tail;

    if elm then
        local data = elm.data;

        -- remove head and tail
        if self.len == 1 then
            self.len = 0;
            self.head = nil;
            self.tail = nil;
        -- set tail to prev
        else
            self.len = self.len - 1;
            self.tail = elm.prev;
            self.tail.next = nil;
        end

        -- remove references
        unrefElement( self, elm );

        return data;
    end
end


--- remove
-- @param elm
-- @return data
function Deq:remove( elm )
    if self.used[elm] == true then
        local data = elm.data;

        -- remove head and tail
        if self.len == 1 then
            self.head = nil;
            self.tail = nil;
        else
            if elm.prev then
                elm.prev.next = elm.next;
            else
                self.head = elm.next;
            end

            if elm.next then
                elm.next.prev = elm.prev;
            else
                self.tail = elm.prev;
            end
        end

        self.len = self.len - 1;
        -- remove references
        unrefElement( self, elm );

        return data;
    end
end


--- new
-- @return deq
local function new()
    return setmetatable({
        len = 0,
        used = setmetatable({},{ __mode = 'k' })
    }, {
        __index = Deq
    });
end


return {
    new = new
};
