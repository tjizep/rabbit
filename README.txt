# rabbit
stl compatible hashtable (rabbit::unordered_map, rabbit::unordered_set)

It also seems to be performing well

Some timing results with google dense hash compiled with VS 11 (2012)
----------------------------------------------------------------------

Average over 10000000 iterations
Current time (GMT): Sat Apr 18 08:46:09 2015

*** WARNING ***: sys/resources.h was not found, so all times
                 reported are wall-clock time, not user time

DENSE_HASH_MAP (4 byte objects, 10000000 iterations):
map_grow               40.5 ns  (26777220 hashes, 123886195 copies)  257.0 MB
map_predict/grow       12.2 ns  (10000000 hashes, 73554475 copies)  256.0 MB
map_replace             7.4 ns  (36777220 hashes, 123886195 copies)
map_fetch_random        8.9 ns  (10000000 hashes, 123886195 copies)  256.0 MB
map_fetch_sequential    2.9 ns  (10000000 hashes, 123886195 copies)  256.0 MB
map_fetch_empty         1.3 ns  (       0 hashes,       35 copies)
map_remove              3.3 ns  (36777220 hashes, 133886195 copies)
map_toggle             33.4 ns  (20624999 hashes, 74999995 copies)    0.0 MB
map_iterate             3.0 ns  (26777220 hashes, 123886195 copies)

stresshashfunction map_size=256 stride=1: 37.6ns/insertion
stresshashfunction map_size=256 stride=256: 18.8ns/insertion
stresshashfunction map_size=1024 stride=1: 58.4ns/insertion
stresshashfunction map_size=1024 stride=1024: 43.2ns/insertion

RABBIT UNORDERED_MAP (4 byte objects, 10000000 iterations):
map_grow               45.1 ns  (26777938 hashes, 127446204 copies)  136.6 MB
map_predict/grow        8.0 ns  (10000000 hashes, 10000036 copies)  136.0 MB
map_replace             5.7 ns  (36777938 hashes, 127446204 copies)
map_fetch_random        8.2 ns  (10000000 hashes, 127446204 copies)  135.9 MB
map_fetch_sequential    4.2 ns  (10000000 hashes, 127446204 copies)  136.0 MB
map_fetch_empty         3.2 ns  (10000000 hashes,        0 copies)
map_remove              7.8 ns  (36777938 hashes, 137446204 copies)
map_toggle             29.2 ns  (20000000 hashes, 20000000 copies)
map_iterate             5.9 ns  (26777938 hashes, 157446204 copies)

stresshashfunction map_size=256 stride=1: 6.8ns/insertion
stresshashfunction map_size=256 stride=256: 6.4ns/insertion
stresshashfunction map_size=1024 stride=1: 6.0ns/insertion
stresshashfunction map_size=1024 stride=1024: 6.4ns/insertion


Using
------------------------------------------------------------------

##include <rabbit\unordered_map>

##include <rabbit\unordered_set>



