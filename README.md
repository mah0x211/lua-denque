lua-deq
===

[![test](https://github.com/mah0x211/lua-deque/actions/workflows/test.yml/badge.svg)](https://github.com/mah0x211/lua-deque/actions/workflows/test.yml)
[![codecov](https://codecov.io/gh/mah0x211/lua-deque/branch/master/graph/badge.svg)](https://codecov.io/gh/mah0x211/lua-deque)


Double-ended queue module.


## Installation

```sh
luarocks install deque
```
***


## Create a Deq Object.

### dq = deque.new()

returns a new `deque` object.

**Returns**

- `deq:deque`: a `deque` object.

**Example**

```lua
local deque = require('deq')
local dq = deque.new()
```


## n = #deque

get number of elements held in the `deque` object.

```lua
local deque = require('deq')
local dq = deque.new()
local len = #dq
```

**Returns**

- `n:integer`: number of items held in the `deque` object.


## elm = deque:unshift( data )

insert data at the head.

**Parameters**

- `data:any`: a non-nil data.

**Returns**

- `elm:deque.element`: inserted `deque.element` object.


## elm = deque:head()

get the head element.

**Returns**

- `elm:deque.element`: `deque.element` object.


## elm = deque:push( data )

insert data at the tail.

**Parameters**

- `data:any`: a non-nil data.


**Returns**

- `elm:deque.element`: inserted `deque.element` object.


## elm = deque:tail()

get the tail element.

**Returns**

- `elm:deque.element`: `deque.element` object.


## data = deque:shift()

remove the head element and return the data held by the head element.

**Returns**

- `data:any`: a data.


## data = deque:pop()

remove the tail element and return the data held by the tail element.

**Returns**

- `data:any`: a data.


## data = deque:remove( elm )

remove the specified element and return the data held by the specified element.

**Parameters**

- `elm:deque.element`: `deque.element` object.


**Returns**

- `data`: a data.


## `deque.element` object


## data = elm:data( [newdata] )

set a newdata if `newdata` argument passed, and return a data held in element.

**Parameters**

- `newdata:any`: a non-nil data.

**Returns**

- `data`: a data.


## elm = elm:prev()

get an previous `deque.element` object.

**Returns**

- `elm:deque.element`: previous `deque.element` object.


## elm = elm:next()

get a next `deque.element` object.


**Returns**

- `elm:deq.element`: next `deque.element` object.


## data = elm:remove()

remove element from associated deque and return the data held by the element.

**Returns**

- `data:any`: a data.

