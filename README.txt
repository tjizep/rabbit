# rabbit v 1.2 r2
stl compatible hashtable (rabbit::unordered_map or rabbit::sparse_unordered_map)

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
	rabbit::sparse_unordered_map<int,int> sparse_int_map;
	int_map.insert(0,1);
	if(int_map[0] == 1){
		/// xyz
	}
}

Advantages:
-----------

1. Very Fast and sometimes small or just Fast and Very Small when set_min_load_factor(> 0.7),set_logarithmic(>=4)
   you can also use rabbit::sparse_unordered_map to get the same effect
2. Strong guarantees for hash table size in sparse mode
   i.e. Sparse version of hash table is close to the size of google sparse hash
   even though it has a step shaped memory use curve
3. Std api compatible with stl 
4. sparseness can be dialled in dynamically when need arises - only effective after rehash (use set_logarithmic(1..32))

Disadvantages
-------------

If a rehash takes place during iteration (because of inserts during iteration) the iterator becomes 
invalid. It wont crash but it might skip previously added elements. 
Best is to rehash to aproximate future size before starting iteration which will cause 
inserts. Erases and updates are stable. 

Algorithm Description
---------------------

rabbit is both a closed *and* openly addressed hash table.

Open addressing part
--------------------

Keys are located via a truncated linear probe of constant length in case of the dense version.
The linear probe is logarithmically related to the hash size when the sparse flag is set
with the set_logarithmic(>=1) function.

Rabbit maintains each key associated with two bits seperately.
The first bit is for a keys existence and a second bit is for a collision indicator.
The collision indicator removes the need to search for non existing keys which is a 
problem in the standard linear probing algorithm.

The bits, key value pairs are each stored in separate arrays to provide better CPU cache
behaviour. For instance the existence bits will stay in cache longer so that memory access
to these structures are reduced.

Closed addressing
-----------------

At the end of the key array rabbit also maintains a single bucket. If any key is inserted and
a open slot is not found within the current probe length it is added here. This bucket is a 
accessed like a stack although removing items in the middle will not reduce its height.
items are added at the back.

In the semi dense variation of the algoritm the size of this bucket is maintained at a constant
factor. In the sparse version the single bucket size is a logarithmically increasing number.

Once the single bucket is full a rehash is performed on a new table with twice as many keys.
In case of the sparse table a load factor of ~0.75 is maintained.

Safety
------

A randomization protocol is activated when the table rehashes and a minimum load factor is not
reached.

Changes to algorithm for 1.2 (Minimum load factor)
--------------------------------------------------

The probe length is not constant anymore but changes (increases) when keys are added to the bucket
while the minimum load factor value is not reached. A set_min_load_factor(x e (0,1])) function is added.
This can be used to optimize for speed or memory use.
The min. load factor is defaulted to 0.5.
This results in much less variation in memory use while not affecting performance.
Values around 0.25 give high read performance and slightly slower growth while using more memory,
around 0.7 will give much better growth and slightly slower random read while using about half
the memory.

Experimentation also revealed that the hash table is much less sensitive to bad hash functions after this 
change.

Note
----

The previous version stored keys and values separately which reduced memory use when 
sizeof(key)+sizeof(value) < sizeof(std::pair<key,value>). This behaviour isn't 
available anymore as the penalty for random read access is usually too high. 
