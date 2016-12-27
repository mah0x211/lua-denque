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

- `deq:table`: deq object.

**Example**

```lua
local deq = require('deq').new();
```


---


## Deq Methods


### elm = deq:unshift( data )

insert a data at front.


**Parameters**

- `data`: a data.


**Returns**

- `elm:table`: inserted element object.



### elm = deq:push( data )

insert a data at back.


**Parameters**

- `data`: a data.


**Returns**

- `elm:table`: inserted element object.



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

- `elm:table`: an element.


**Returns**

- `data`: a data

