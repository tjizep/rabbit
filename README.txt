# rabbit v 1. r11
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

1. Very Fast and Small when set_sparse(false)(default) or just Fast and Very Small when set_sparse(true)
2. Strong guarantees for hash table size in sparse mode
   i.e. Sparse version of hash table is nearly always smaller(90% of the time) than google sparse hash
   even though it has a step shaped memory use curve
3. Std api compatible with stl however only deviation is iterator cannot modify values in table
4. sparseness can be dialled in dynamically when need arises - only effective after rehash

Disadvantages
-------------

iterator * operator does not return a normal std::pair<K,V> its rather a std::pair<proxy<K>,proxy<V>>. 
which may make certain type inferences impossible.

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

Some timing results with google dense hash compiled with TDM-MinGW 64 GCC 5.1
-----------------------------------------------------------------------------

These results are with the google sparse hash library and the maximum bits 
randomness is only log2(10000000) ~ 23 bits

======
Average over 10000000 iterations
Current time (GMT): Thu Jul 28 16:22:58 2016

*** WARNING ***: sys/resources.h was not found, so all times
                 reported are wall-clock time, not user time

DENSE_HASH_MAP (8 byte objects, 5000000 iterations):
map_grow               68.1 ns  (13388611 hashes, 56943147 copies)  193.4 MB
map_predict/grow       22.3 ns  ( 5000000 hashes, 31777259 copies)  192.0 MB
map_replace            12.0 ns  (18388611 hashes, 56943147 copies)
map_fetch_random       29.0 ns  ( 5000000 hashes, 56943147 copies)  192.0 MB
map_fetch_sequential   13.8 ns  ( 5000000 hashes, 56943147 copies)  192.0 MB
map_fetch_empty         4.9 ns  (       0 hashes,       35 copies)
map_remove             27.9 ns  (18388611 hashes, 61943147 copies)
map_toggle             65.9 ns  (10312499 hashes, 32499995 copies)    0.0 MB
map_iterate             5.9 ns  (13388611 hashes, 56943147 copies)

stresshashfunction map_size=256 stride=1: 67.1ns/insertion
stresshashfunction map_size=256 stride=256: 27.8ns/insertion
stresshashfunction map_size=1024 stride=1: 100.4ns/insertion
stresshashfunction map_size=1024 stride=1024: 73.4ns/insertion

RABBIT (LESS SPARSE) UNORDERED_MAP (8 byte objects, 5000000 iterations):
map_grow               23.7 ns  ( 5588898 hashes,  5590349 copies)   61.3 MB
map_predict/grow       18.1 ns  ( 5000000 hashes,  5000071 copies)   61.2 MB
map_replace            12.7 ns  (10588898 hashes,  5590349 copies)
map_fetch_random       22.5 ns  ( 5000000 hashes,  5590349 copies)   61.3 MB
map_fetch_sequential   12.6 ns  ( 5000000 hashes,  5590349 copies)   61.2 MB
map_fetch_empty        16.7 ns  ( 5000000 hashes,        1 copies)
map_remove              7.7 ns  (10589298 hashes, 10590349 copies)
map_toggle             24.0 ns  (10000000 hashes, 10000001 copies)
map_iterate             3.5 ns  ( 5588898 hashes,  5590349 copies)

stresshashfunction map_size=256 stride=1: 7.7ns/insertion
stresshashfunction map_size=256 stride=256: 7.8ns/insertion
stresshashfunction map_size=1024 stride=1: 8.7ns/insertion
stresshashfunction map_size=1024 stride=1024: 9.1ns/insertion

DENSE_HASH_MAP (16 byte objects, 2500000 iterations):
map_grow              124.9 ns  ( 6694306 hashes, 28471619 copies)  160.0 MB
map_predict/grow       34.1 ns  ( 2500000 hashes, 15888651 copies)  160.0 MB
map_replace            23.9 ns  ( 9194306 hashes, 28471619 copies)
map_fetch_random       46.2 ns  ( 2500000 hashes, 28471619 copies)  158.8 MB
map_fetch_sequential   25.3 ns  ( 2500000 hashes, 28471619 copies)  160.6 MB
map_fetch_empty         5.6 ns  (       0 hashes,       35 copies)
map_remove             39.1 ns  ( 9194306 hashes, 30971619 copies)
map_toggle             81.7 ns  ( 5156249 hashes, 16249995 copies)    0.0 MB
map_iterate             6.7 ns  ( 6694306 hashes, 28471619 copies)

RABBIT (LESS SPARSE) UNORDERED_MAP (16 byte objects, 2500000 iterations):
map_grow               33.3 ns  ( 3019635 hashes,  3021017 copies)   49.5 MB
map_predict/grow       25.5 ns  ( 2500000 hashes,  2500071 copies)   49.8 MB
map_replace            20.0 ns  ( 5519635 hashes,  3021017 copies)
map_fetch_random       35.4 ns  ( 2500000 hashes,  3021017 copies)   49.7 MB
map_fetch_sequential   18.2 ns  ( 2500000 hashes,  3021017 copies)   49.7 MB
map_fetch_empty        26.6 ns  ( 2500000 hashes,        1 copies)
map_remove             14.2 ns  ( 5519804 hashes,  5521017 copies)
map_toggle             33.5 ns  ( 5000000 hashes,  5000001 copies)
map_iterate             3.7 ns  ( 3019635 hashes,  3021017 copies)


More Tests
----------
These tests use approx 60 bits of randomness 
also displays memory behaviour as key count increases

memory used by script: 77.19 MB
google dense hash test
0: 1 in hash, bench total 0 secs, 0 MB
1000000: 1000001 in hash, bench total 0.1031 secs, 32.1 MB
2000000: 2000001 in hash, bench total 0.2178 secs, 64.1 MB
3000000: 3000001 in hash, bench total 0.3727 secs, 128.1 MB
4000000: 4000001 in hash, bench total 0.454 secs, 128.1 MB
5000000: 5000001 in hash, bench total 0.6981 secs, 256.1 MB
6000000: 6000001 in hash, bench total 0.771 secs, 256.1 MB
7000000: 7000001 in hash, bench total 0.8506 secs, 256.1 MB
8000000: 8000001 in hash, bench total 0.9376 secs, 256.1 MB
9000000: 9000001 in hash, bench total 1.356 secs, 512.1 MB
10000000 in hash, writes total 1.426 secs, 512.1 MB
0: bench read 0 secs
1000000: bench read 0.03511 secs
2000000: bench read 0.0716 secs
3000000: bench read 0.1096 secs
4000000: bench read 0.1499 secs
5000000: bench read 0.1908 secs
6000000: bench read 0.2334 secs
7000000: bench read 0.2776 secs
8000000: bench read 0.3234 secs
9000000: bench read 0.371 secs
time total 1.847 secs read 0.4213 secs. mem used 512.1 MB
google sparse hash test
0: 1 in hash, bench total 5e-006 secs, 0.01172 MB
1000000: 1000001 in hash, bench total 0.6074 secs, 22.77 MB
2000000: 2000001 in hash, bench total 1.317 secs, 45.28 MB
3000000: 3000001 in hash, bench total 1.805 secs, 76 MB
4000000: 4000001 in hash, bench total 2.834 secs, 86.59 MB
5000000: 5000001 in hash, bench total 3.344 secs, 116.4 MB
6000000: 6000001 in hash, bench total 3.921 secs, 147.4 MB
7000000: 7000001 in hash, bench total 5.563 secs, 144.1 MB
8000000: 8000001 in hash, bench total 6.093 secs, 169.1 MB
9000000: 9000001 in hash, bench total 6.65 secs, 203.4 MB
10000000 in hash, writes total 7.236 secs, 229.1 MB
0: bench read 0 secs
1000000: bench read 0.08507 secs
2000000: bench read 0.1689 secs
3000000: bench read 0.256 secs
4000000: bench read 0.3488 secs
5000000: bench read 0.449 secs
6000000: bench read 0.5595 secs
7000000: bench read 0.6803 secs
8000000: bench read 0.82 secs
9000000: bench read 0.9781 secs
time total 8.394 secs read 1.158 secs. mem used 229.1 MB
std hash test
0: 1 in hash, bench total 7e-006 secs, 0.007813 MB
1000000: 1000001 in hash, bench total 0.3532 secs, 53.53 MB
2000000: 2000001 in hash, bench total 0.7982 secs, 107.8 MB
3000000: 3000001 in hash, bench total 1.367 secs, 170.7 MB
4000000: 4000001 in hash, bench total 1.722 secs, 216.6 MB
5000000: 5000001 in hash, bench total 2.579 secs, 296.9 MB
6000000: 6000001 in hash, bench total 2.903 secs, 342.9 MB
7000000: 7000001 in hash, bench total 3.253 secs, 389 MB
8000000: 8000001 in hash, bench total 3.624 secs, 435 MB
9000000: 9000001 in hash, bench total 5.091 secs, 550.5 MB
10000000 in hash, writes total 5.405 secs, 596.6 MB
0: bench read 0 secs
1000000: bench read 0.2039 secs
2000000: bench read 0.4046 secs
3000000: bench read 0.5967 secs
4000000: bench read 0.7863 secs
5000000: bench read 0.9723 secs
6000000: bench read 1.158 secs
7000000: bench read 1.345 secs
8000000: bench read 1.534 secs
9000000: bench read 1.717 secs
time total 7.274 secs read 1.868 secs. mem used 596.6 MB
rabbit hash test
0: 1 in hash, bench total 1.1e-005 secs, 0 MB
1000000: 1000001 in hash, bench total 0.1056 secs, 49.02 MB
2000000: 2000001 in hash, bench total 0.223 secs, 98.02 MB
3000000: 3000001 in hash, bench total 0.2852 secs, 98.02 MB
4000000: 4000001 in hash, bench total 0.4789 secs, 196 MB
5000000: 5000001 in hash, bench total 0.5496 secs, 196 MB
6000000: 6000001 in hash, bench total 0.6201 secs, 196 MB
7000000: 7000001 in hash, bench total 0.7016 secs, 196 MB
8000000: 8000001 in hash, bench total 1.031 secs, 392 MB
9000000: 9000001 in hash, bench total 1.108 secs, 392 MB
10000000 in hash, writes total 1.186 secs, 392 MB
0: bench read 0 secs
1000000: bench read 0.04313 secs
2000000: bench read 0.0864 secs
3000000: bench read 0.1297 secs
4000000: bench read 0.1731 secs
5000000: bench read 0.2167 secs
6000000: bench read 0.2602 secs
7000000: bench read 0.3038 secs
8000000: bench read 0.3511 secs
9000000: bench read 0.4031 secs
time total 1.644 secs read 0.4573 secs. mem used 392 MB
rabbit sparse hash test
0: 1 in hash, bench total 0 secs, 0 MB
1000000: 1000001 in hash, bench total 0.2223 secs, 25 MB
2000000: 2000001 in hash, bench total 0.4677 secs, 49.04 MB
3000000: 3000001 in hash, bench total 0.6261 secs, 49.04 MB
4000000: 4000001 in hash, bench total 0.9925 secs, 98.04 MB
5000000: 5000001 in hash, bench total 1.148 secs, 98.04 MB
6000000: 6000001 in hash, bench total 1.35 secs, 98.04 MB
7000000: 7000001 in hash, bench total 1.604 secs, 98.04 MB
8000000: 8000001 in hash, bench total 2.098 secs, 196 MB
9000000: 9000001 in hash, bench total 2.258 secs, 196 MB
10000000 in hash, writes total 2.441 secs, 196 MB
0: bench read 0 secs
1000000: bench read 0.05074 secs
2000000: bench read 0.09949 secs
3000000: bench read 0.1481 secs
4000000: bench read 0.1967 secs
5000000: bench read 0.2454 secs
6000000: bench read 0.2948 secs
7000000: bench read 0.3446 secs
8000000: bench read 0.4039 secs
9000000: bench read 0.4862 secs
time total 3.022 secs read 0.5811 secs. mem used 196 MB

Process returned 0 (0x0)   execution time : 24.730 s
Press any key to continue.