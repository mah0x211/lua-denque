local testcase = require('testcase')
local deque = require('deque')

function testcase.new()
    -- test that create new deque object
    local dq = assert(deque.new())
    assert.match(tostring(dq), 'deque: ')
    assert.equal(#dq, 0);
    assert.is_nil(dq:head())
    assert.is_nil(dq:tail())

    -- test that create new deque object by __call metamethod
    dq = assert(deque())
    assert.match(tostring(dq), 'deque: ')
    assert.equal(#dq, 0);
    assert.is_nil(dq:head())
    assert.is_nil(dq:tail())
end

function testcase.unshift()
    local dq = assert(deque.new())

    -- test that insert a data at the head
    local a = dq:unshift('a')
    assert.match(tostring(a), 'deque.element: ')
    assert.equal(dq:head(), a)
    assert.equal(dq:tail(), a)

    local b = dq:unshift('b')
    assert.equal(#dq, 2)
    assert.equal(dq:head(), b)
    assert.equal(dq:tail(), a)

    -- test that insert a nil data
    local err = assert.throws(dq.unshift, dq, nil)
    assert.match(err, 'nil data cannot be set')
end

function testcase.push()
    local dq = assert(deque.new())

    -- test that insert a data at the tail
    local a = dq:push('a')
    assert.match(tostring(a), 'deque.element: ')
    assert.equal(dq:head(), a)
    assert.equal(dq:tail(), a)

    local b = dq:push('b')
    assert.equal(#dq, 2)
    assert.equal(dq:head(), a)
    assert.equal(dq:tail(), b)
end

function testcase.shift()
    local dq = assert(deque.new())
    dq:unshift('a');
    dq:unshift('b');

    -- test that remove a data from the head
    assert.equal(dq:shift(), 'b')
    assert.equal(dq:shift(), 'a')
    assert.equal(#dq, 0);

    -- test that return nil
    assert.is_nil(dq:shift())
end

function testcase.pop()
    local dq = assert(deque.new())
    dq:push('a')
    dq:push('b')

    -- test that remove a data from tail
    assert.equal(dq:pop(), 'b')
    assert.equal(dq:pop(), 'a')
    assert.equal(#dq, 0)

    -- test that return nil
    assert.is_nil(dq:pop())
end

function testcase.remove()
    local dq = assert(deque.new())
    local a = dq:push('a')
    local b = dq:push('b')
    local c = dq:push('c')
    local d = dq:push('d')
    local e = dq:push('e')
    assert.equal(#dq, 5)
    assert.equal(dq:head(), a)
    assert.equal(dq:tail(), e)
    assert.is_nil(a:prev())
    assert.is_nil(e:next())

    -- test that remove second element
    assert.equal(dq:remove(b), 'b')
    assert.equal(#dq, 4)
    assert.equal(dq:head(), a)
    assert.equal(dq:tail(), e)
    assert.equal(a:next(), c)
    assert.equal(c:prev(), a)

    -- test that return nil if element is already removed
    assert.is_nil(dq:remove(b))

    -- test that remove head element
    assert.equal(dq:remove(a), 'a')
    assert.equal(#dq, 3)
    assert.equal(dq:head(), c)
    assert.equal(dq:tail(), e)
    assert.is_nil(c:prev())
    assert.equal(c:next(), d)
    assert.equal(d:prev(), c)

    -- test that remove tail element
    assert.equal(dq:remove(e), 'e')
    assert.equal(#dq, 2)
    assert.equal(dq:head(), c)
    assert.equal(dq:tail(), d)
    assert.equal(c:next(), d)
    assert.equal(d:prev(), c)
    assert.is_nil(d:next())

    -- test that remove tail element
    assert.equal(dq:remove(d), 'd')
    assert.equal(#dq, 1)
    assert.equal(dq:head(), c)
    assert.equal(dq:tail(), c)
    assert.is_nil(c:prev())
    assert.is_nil(c:next())

    -- test that remove head element
    assert.equal(dq:remove(c), 'c')
    assert.equal(#dq, 0)
    assert.is_nil(dq:head())
    assert.is_nil(dq:tail())
end

function testcase.elm_remove()
    local dq = assert(deque.new())
    local a = dq:push('a')
    local b = dq:push('b')
    local c = dq:push('c')
    local d = dq:push('d')
    local e = dq:push('e')
    assert.equal(#dq, 5)
    assert.equal(dq:head(), a)
    assert.equal(dq:tail(), e)
    assert.is_nil(a:prev())
    assert.is_nil(e:next())

    -- test that remove second element
    assert.equal(b:remove(), 'b')
    assert.equal(#dq, 4)
    assert.equal(dq:head(), a)
    assert.equal(dq:tail(), e)
    assert.equal(a:next(), c)
    assert.equal(c:prev(), a)

    -- test that remove head element
    assert.equal(a:remove(), 'a')
    assert.equal(#dq, 3)
    assert.equal(dq:head(), c)
    assert.equal(dq:tail(), e)
    assert.is_nil(c:prev())
    assert.equal(c:next(), d)
    assert.equal(d:prev(), c)

    -- test that remove tail element
    assert.equal(e:remove(), 'e')
    assert.equal(#dq, 2)
    assert.equal(dq:head(), c)
    assert.equal(dq:tail(), d)
    assert.equal(c:next(), d)
    assert.equal(d:prev(), c)
    assert.is_nil(d:next())

    -- test that remove tail element
    assert.equal(d:remove(), 'd')
    assert.equal(#dq, 1)
    assert.equal(dq:head(), c)
    assert.equal(dq:tail(), c)
    assert.is_nil(c:prev())
    assert.is_nil(c:next())

    -- test that remove head element
    assert.equal(c:remove(), 'c')
    assert.equal(#dq, 0)
    assert.is_nil(dq:head())
    assert.is_nil(dq:tail())
end

function testcase.elm_data()
    local dq = assert(deque.new())
    local a = dq:push('a')

    -- test that return a data
    assert.equal(a:data(), 'a')

    -- test that set a newdata
    assert.equal(a:data('foo'), 'foo')

    -- test that throws an error if newdata is nil
    local err = assert.throws(a.data, a, nil)
    assert.match(err, 'nil data cannot be set')
end

