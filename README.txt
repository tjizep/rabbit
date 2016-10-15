# rabbit v 1.1 r11
stl compatible hashtable (rabbit::unordered_map)

Using:
------------------------------------------------------------------

#include <rabbit\unordered_map>

void rabbits(){
	rabbit::unordered_map<int,int> int_map;
	int_map.insert(0,1);
	if(int_map[0] == 1){
		/// xyz
	}
	...
}

Advantages:
-----------

1. Very Fast and Small when set_sparse(false)(default) or just Fast and Very Small when set_logarithmic(>=4)
2. Strong guarantees for hash table size in sparse mode
   i.e. Sparse version of hash table is nearly always smaller(90% of the time) than google sparse hash
   even though it has a step shaped memory use curve
3. Std api compatible with stl 
4. sparseness can be dialled in dynamically when need arises - only effective after rehash (use set_logarithmic(1..32))

Disadvantages
-------------

If a rehash takes place during iteration (because of inserts during iteration) the iterator becomes 
invalid. Best is to rehash to aproximate future size before starting iteration which will cause 
inserts. Erases and updates are stable. 

Algorithm Descriprion
---------------------

rabbit is both a closed *and* openly addressed hash table.

Open addressing part
--------------------

Keys are located via a truncated linear probe of constant length in case of the dense version.
The linear probe is logarithmically related to the hash size when the sparse flag is set
with the set_sparse(true) function.

Rabbit maintains each key associated with two bits seperately.
The first bit is for a keys existence and a second bit is for a collision indicator.
The collision indicator removes the need to search for non existing keys which is a 
problem in the standard linear probing algorithm.

The bits, keys and values are each stored in separate arrays to provide better CPU cache
behaviour. For instance the existence bits will stay in cache longer so that memory access
to these structures are reduced.

Closed addressing
-----------------

At the end of the key array rabbit also maintains a single bucket. If any key is inserted and
a open slot is not found within the current probe length it is added here. This bucket is a 
accessed like a stack. removing items in the middle will reduce its height and new items are added
at the back.

In the semi dense variation of the algoritm the size of this bucket is maintained at a constant
factor. In the sparse version the single bucket size is a logarithmically increasing number.

Once the single bucket is full a rehash is performed on a new table with twice as many keys.
In case of the sparse table a load factor of 0.9 is maintained.


