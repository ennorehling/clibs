# Single-File C Libraries

A collection of single-file libraries for frequently used algorithms and 
data types. Each library depends only on its header file for compiltion.


## CritBit-Trie (critbit)

Implementation of djb's critbit tree structure, a clever kind of trie
that allows fast search among a set of strings, including prefix-search.

See also: [djb's site](http://cr.yp.to/critbit.html)


## Space-Efficient List (selist)

A space-efficient unrolled linked list that is superior to (value, next)
tuples in terms of speed and memory use.

From http://en.wikipedia.org/wiki/Unrolled_linked_list:
In computer programming, an unrolled linked list is a variation on the
linked list which stores multiple elements in each node. It can
dramatically increase cache performance, while decreasing the memory
overhead associated with storing list metadata such as references. It
is related to the B-tree.


## tests:

All tests in this repository use the very simple CuTest test library.

[![endorse](https://api.coderwall.com/enno/endorsecount.png)](https://coderwall.com/enno)
