lua-deq
===

Double-ended queue module.


***


## Installation

```sh
luarocks install deq --from=http://mah0x211.github.io/rocks/
```

---


## Create a Deq Object.

### deq = deq.new()

returns a new `Deq` object.

**Returns**

- `deq:deq`: deq object.

**Example**

```lua
local deq = require('deq').new();
```


---

## Deq Properties


### __len

```
local len = #deq;
```


## Deq Methods


### elm = deq:unshift( data )

insert a data at front.


**Parameters**

- `data`: a data.


**Returns**

- `elm:deq.element`: inserted [element object](#deq-element-object).


### elm = deq:head()

examine first element.

**Returns**

- `elm:deq.element`: first [element object](#deq-element-object).


### elm = deq:push( data )

insert a data at back.


**Parameters**

- `data`: a data.


**Returns**

- `elm:deq.element`: inserted [element object](#deq-element-object).


### elm = deq:tail()

examine last element.


**Returns**

- `elm:deq.element`: last [element object](#deq-element-object).



### data = deq:shift()

remove first data.


**Returns**

- `data`: a data



### data = deq:pop()

remove last data.


**Returns**

- `data`: a data



### data = deq:remove( elm )

remove an element.


**Parameters**

- `elm:deq.element`: an element.


**Returns**

- `data`: a data


## Deq Element Object


### data = elm:data( [newdata] )

set a data if newdata argument passed, and return a data of element.

**Parameters**

- `newdata`: a data.

**Returns**

- `data`: a data.


### elm = elm:prev()

get an previous element.

**Returns**

- `elm:deq.element`: previous [element object](#deq-element-object).


### elm = elm:next()

get a next element.


**Returns**

- `elm:deq.element`: next [element object](#deq-element-object).


### data = elm:remove()

remove element from deq then return data of element.


**Returns**

- `data`: a data.

