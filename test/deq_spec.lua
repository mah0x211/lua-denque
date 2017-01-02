--[[

  test/deque_spec.lua
  lua-deque
  Created by Masatoshi Teruya on 16/12/27.

--]]
--- file scope variables
local Deque = require('deq');


describe('Deque(Double-ended queue):', function()
    local deq = Deque.new();


    describe('test a initial properties -', function()
        it('length must be 0', function()
            assert.are.equal( 0, deq.len );
        end);

        it('head must be nil', function()
            assert.are.equal( nil, deq.tail );
        end);

        it('tail must be nil', function()
            assert.are.equal( nil, deq.tail );
        end);

        it('used must be empty', function()
            assert.are.same( {}, deq.used );
        end);
    end);


    describe('test unshift and shift methods -', function()
        it('unshift method should insert a data to head', function()
            local a = deq:unshift('a');
            local b = deq:unshift('b');

            assert.are.equal( 2, deq.len );
            assert.are.equal( b, deq.head );
            assert.are.equal( a, deq.tail );
        end);


        it('shift method should remove a data from head', function()
            assert.are.equal( 'b', deq:shift() );
            assert.are.equal( 'a', deq:shift() );
            assert.are.equal( 0, deq.len );
        end);
    end);


    describe('test push and pop methods -', function()
        it('push method should insert a data to tail', function()
            local a = deq:push('a');
            local b = deq:push('b');

            assert.are.equal( 2, deq.len );
            assert.are.equal( a, deq.head );
            assert.are.equal( b, deq.tail );
        end);

        it('pop method should remove a data from tail', function()
            assert.are.equal( 'b', deq:pop() );
            assert.are.equal( 'a', deq:pop() );
            assert.are.equal( 0, deq.len );
        end);
    end);


    describe('test the first insertions -', function()
        it('unshift method should insert a data to head and tail', function()
            local a = deq:unshift('a');
            assert.are.equal( 1, deq.len );
            assert.are.equal( a, deq.head );
            assert.are.equal( a, deq.tail );
            assert.are.equal( 'a', deq:shift() );
        end);

        it('push method should insert a data to head and tail', function()
            local a = deq:push('a');
            assert.are.equal( 1, deq.len );
            assert.are.equal( a, deq.head );
            assert.are.equal( a, deq.tail );
            assert.are.equal( 'a', deq:pop() );
        end);
    end);


    describe('test a remove method -', function()
        it('remove method should remove an element', function()
            local a = deq:push('a');
            local b = deq:push('b');
            local c = deq:push('c');
            local d = deq:push('d');
            local e = deq:push('e');

            assert.are.equal( 5, deq.len );
            assert.are.equal( a, deq.head );
            assert.are.equal( e, deq.tail );
            assert.are.equal( nil, e.next );

            assert.are.equal( 'b', deq:remove( b ) );
            assert.are.equal( 4, deq.len );
            assert.are.equal( a, deq.head );
            assert.are.equal( e, deq.tail );
            assert.are.equal( c, a.next );

            assert.are.equal( 'a', deq:remove( a ) );
            assert.are.equal( 3, deq.len );
            assert.are.equal( c, deq.head );
            assert.are.equal( e, deq.tail );
            assert.are.equal( nil, c.prev );

            assert.are.equal( 'e', deq:remove( e ) );
            assert.are.equal( 2, deq.len );
            assert.are.equal( c, deq.head );
            assert.are.equal( d, deq.tail );
            assert.are.equal( nil, d.next );

            assert.are.equal( 'd', deq:remove( d ) );
            assert.are.equal( 1, deq.len );
            assert.are.equal( c, deq.head );
            assert.are.equal( c, deq.tail );
            assert.are.equal( nil, c.next );

            assert.are.equal( 'c', deq:remove( c ) );
            assert.are.equal( 0, deq.len );
            assert.are.equal( nil, deq.head );
            assert.are.equal( nil, deq.tail );
        end);
    end);
end);

