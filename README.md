lua-denque
===

[![test](https://github.com/mah0x211/lua-denque/actions/workflows/test.yml/badge.svg)](https://github.com/mah0x211/lua-denque/actions/workflows/test.yml)
[![codecov](https://codecov.io/gh/mah0x211/lua-denque/branch/master/graph/badge.svg)](https://codecov.io/gh/mah0x211/lua-denque)


Double-ended queue module.


## Installation

```sh
luarocks install denque
```
***


## Create a `denque` object.

### dq = denque.new()

returns a new `denque` object.

**Returns**

- `deq:denque`: a `denque` object.

**Example**

```lua
local denque = require('deq')
local dq = denque.new()
```


## n = #denque

get number of elements held in the `denque` object.

```lua
local denque = require('deq')
local dq = denque.new()
local len = #dq
```

**Returns**

- `n:integer`: number of items held in the `denque` object.


## elm = denque:unshift( data )

insert data at the head.

**Parameters**

- `data:any`: a non-nil data.

**Returns**

- `elm:denque.element`: inserted `denque.element` object.


## elm = denque:head()

get the head element.

**Returns**

- `elm:denque.element`: `denque.element` object.


## elm = denque:push( data )

insert data at the tail.

**Parameters**

- `data:any`: a non-nil data.


**Returns**

- `elm:denque.element`: inserted `denque.element` object.


## elm = denque:tail()

get the tail element.

**Returns**

- `elm:denque.element`: `denque.element` object.


## data = denque:shift()

remove the head element and return the data held by the head element.

**Returns**

- `data:any`: a data.


## data = denque:pop()

remove the tail element and return the data held by the tail element.

**Returns**

- `data:any`: a data.


## data = denque:remove( elm )

remove the specified element and return the data held by the specified element.

**Parameters**

- `elm:denque.element`: `denque.element` object.


**Returns**

- `data`: a data.


## `denque.element` object


## data = elm:data( [newdata] )

set a newdata if `newdata` argument passed, and return a data held in element.

**Parameters**

- `newdata:any`: a non-nil data.

**Returns**

- `data`: a data.


## elm = elm:prev()

get an previous `denque.element` object.

**Returns**

- `elm:denque.element`: previous `denque.element` object.


## elm = elm:next()

get a next `denque.element` object.


**Returns**

- `elm:deq.element`: next `denque.element` object.


## data = elm:remove()

remove element from associated denque and return the data held by the element.

**Returns**

- `data:any`: a data.

