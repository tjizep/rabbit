# rabbit r10
stl compatible hashtable (rabbit::unordered_map)

Using:
------------------------------------------------------------------

#include <rabbit\unordered_map>

void rabbits(){
	rabbit::unordered_map<int,int> int_map;
	int_map.insert(0,1);
	...
}

Advantages:
-----------

1. Very Fast and Small when set_sparse(false)(default) or just Fast and Very Small when set_sparse(true)
2. Strong guarantees for hash table size in sparse mode
   i.e. Sparse version of hash table is always smaller than google sparse hash
   even though it has a step shaped memory use curve
3. Std api compatible with stl however only deviation is iterator cannot modify values in table
4. sparseness can be dialled in dynamically when need arises - only effective after rehash

Disadvantages
-------------

iterator * operator is slow because it copies the hash values to a temporary variable. 
use the iterator::key() and iterator::value() functions instead.
This problem could be fixed using a std::pair<_K*,_v*> instead.

Algorithm Descriprion
---------------------

rabbit is both a closed *and* openly addressed hash table.

Open addressing part
--------------------

Keys are located via a truncated linear probe of constant length in case of the dense version.
The linear probe is logarithmically related to the hash size when the sparse flag is set
with the set_sparse(true) function.

rabbit maintains each key associated with two bits seperately.
The first bit is for a keys existence and a second bit is for a collision indicator.
The collision indicator removes the need to search for non existing keys which is a 
problem in the standard linear probing algorithm.

Closed addressing
-----------------

At the end of the key array rabbit also maintains a single bucket. If any key is inserted and
a open slot is not found within the current probe length it is added here.
In the semi dense version of the algoritm the size of this bucket is maintained at a constant
factor. In the sparse version the single bucket size is a logarithmically increasing number.

Once the single bucket is full a rehash is performed on a new table with twice as many keys.
In case of the sparse table a load factor of 0.9 is maintained

Some timing results with google dense hash compiled with TDM-MinGW 64 GCC 5.1
-----------------------------------------------------------------------------
These results are with the google sparse hash library and the maximum bits 
randomness is only log2(10000000) ~ 23 bits

======
Average over 10000000 iterations
Current time (GMT): Wed May 25 19:47:52 2016

*** WARNING ***: sys/resources.h was not found, so all times
                 reported are wall-clock time, not user time

SPARSE_HASH_MAP (4 byte objects, 10000000 iterations):
map_grow              333.8 ns  (23421757 hashes, 43421815 copies)   85.8 MB
map_predict/grow      161.9 ns  (10000000 hashes, 30000005 copies)   84.5 MB
map_replace            34.4 ns  (33421757 hashes, 43421815 copies)
map_fetch_random       68.7 ns  (10000000 hashes, 43421815 copies)   84.2 MB
map_fetch_sequential   50.1 ns  (10000000 hashes, 43421815 copies)   84.1 MB
map_fetch_empty        24.9 ns  (       0 hashes,        1 copies)
map_remove             71.4 ns  (33421757 hashes, 53421815 copies)
map_toggle            264.5 ns  (20399999 hashes, 41599997 copies)
map_iterate             4.3 ns  (23421757 hashes, 43421815 copies)

stresshashfunction map_size=256 stride=1: 357.0ns/insertion
stresshashfunction map_size=256 stride=256: 211.1ns/insertion
stresshashfunction map_size=1024 stride=1: 575.6ns/insertion
stresshashfunction map_size=1024 stride=1024: 457.6ns/insertion

RABBIT SPARSE_UNORDERED_MAP (4 byte objects, 10000000 iterations):
map_grow               35.5 ns  (26812178 hashes, 26812641 copies)   81.0 MB
map_predict/grow       13.0 ns  (10000000 hashes, 10000023 copies)   80.3 MB
map_replace            10.2 ns  (36812178 hashes, 26812641 copies)
map_fetch_random       15.6 ns  (10000000 hashes, 26812641 copies)   80.1 MB
map_fetch_sequential   10.5 ns  (10000000 hashes, 26812641 copies)   79.8 MB
map_fetch_empty        11.5 ns  (10000000 hashes,        1 copies)
map_remove              5.6 ns  (36812178 hashes, 36812641 copies)
map_toggle             17.0 ns  (20000000 hashes, 20000001 copies)
map_iterate             2.8 ns  (26812178 hashes, 46812641 copies)

stresshashfunction map_size=256 stride=1: 7.7ns/insertion
stresshashfunction map_size=256 stride=256: 7.7ns/insertion
stresshashfunction map_size=1024 stride=1: 8.4ns/insertion
stresshashfunction map_size=1024 stride=1024: 8.6ns/insertion

DENSE_HASH_MAP (4 byte objects, 10000000 iterations):
map_grow               45.0 ns  (26777220 hashes, 113886195 copies)  256.0 MB
map_predict/grow       16.9 ns  (10000000 hashes, 63554475 copies)  256.0 MB
map_replace             8.8 ns  (36777220 hashes, 113886195 copies)
map_fetch_random       18.6 ns  (10000000 hashes, 113886195 copies)  256.3 MB
map_fetch_sequential    9.7 ns  (10000000 hashes, 113886195 copies)  255.3 MB
map_fetch_empty         6.2 ns  (       0 hashes,       35 copies)
map_remove             17.9 ns  (36777220 hashes, 123886195 copies)
map_toggle             54.4 ns  (20624999 hashes, 64999995 copies)
map_iterate             5.4 ns  (26777220 hashes, 113886195 copies)

stresshashfunction map_size=256 stride=1: 34.4ns/insertion
stresshashfunction map_size=256 stride=256: 11.8ns/insertion
stresshashfunction map_size=1024 stride=1: 66.5ns/insertion
stresshashfunction map_size=1024 stride=1024: 38.1ns/insertion

RABBIT (LESS SPARSE) UNORDERED_MAP (4 byte objects, 10000000 iterations):
map_grow               32.6 ns  (26777745 hashes, 26778208 copies)   80.9 MB
map_predict/grow       13.0 ns  (10000000 hashes, 10000023 copies)   80.3 MB
map_replace            10.2 ns  (36777745 hashes, 26778208 copies)
map_fetch_random       20.1 ns  (10000000 hashes, 26778208 copies)   79.8 MB
map_fetch_sequential   11.6 ns  (10000000 hashes, 26778208 copies)   81.1 MB
map_fetch_empty         6.8 ns  (       0 hashes,        0 copies)
map_remove              5.6 ns  (36777745 hashes, 36778208 copies)
map_toggle             17.0 ns  (20000000 hashes, 20000001 copies)
map_iterate             2.7 ns  (26777745 hashes, 46778208 copies)

stresshashfunction map_size=256 stride=1: 7.7ns/insertion
stresshashfunction map_size=256 stride=256: 7.7ns/insertion
stresshashfunction map_size=1024 stride=1: 8.4ns/insertion
stresshashfunction map_size=1024 stride=1024: 8.5ns/insertion

UNORDERED_MAP (4 byte objects, 10000000 iterations):
map_grow              111.5 ns  (10000000 hashes, 10000000 copies)  444.4 MB
map_predict/grow       74.0 ns  (10000000 hashes, 10000000 copies)  385.3 MB
map_replace            17.9 ns  (20000000 hashes, 10000000 copies)
map_fetch_random       46.1 ns  (10000000 hashes, 10000000 copies)  443.4 MB
map_fetch_sequential   14.5 ns  (10000000 hashes, 10000000 copies)  443.5 MB
map_fetch_empty        11.3 ns  (10000000 hashes,        0 copies)
map_remove             59.9 ns  (20000000 hashes, 10000000 copies)
map_toggle             98.6 ns  (20000000 hashes, 10000000 copies)
map_iterate            10.0 ns  (10000000 hashes, 10000000 copies)

stresshashfunction map_size=256 stride=1: 70.0ns/insertion
stresshashfunction map_size=256 stride=256: 70.2ns/insertion
stresshashfunction map_size=1024 stride=1: 65.3ns/insertion
stresshashfunction map_size=1024 stride=1024: 65.5ns/insertion

Process returned 0 (0x0)   execution time : 57.501 s
Press any key to continue.

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