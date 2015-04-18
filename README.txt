# rabbit
stl compatible hashtable (rabbit::unordered_map, rabbit::unordered_set)

It also seems to be performing well

Some timing results with google dense hash compiled with VS 11 (2012)
----------------------------------------------------------------------

======
Average over 10000000 iterations
Current time (GMT): Sat Apr 18 07:18:28 2015

*** WARNING ***: sys/resources.h was not found, so all times
                 reported are wall-clock time, not user time

DENSE_HASH_MAP (4 byte objects, 10000000 iterations):
map_grow               40.6 ns  (26777220 hashes, 123886195 copies)  257.0 MB
map_predict/grow       12.5 ns  (10000000 hashes, 73554475 copies)  256.0 MB
map_replace             7.8 ns  (36777220 hashes, 123886195 copies)
map_fetch_random        9.4 ns  (10000000 hashes, 123886195 copies)  256.0 MB
map_fetch_sequential    3.1 ns  (10000000 hashes, 123886195 copies)  256.0 MB
map_fetch_empty         1.6 ns  (       0 hashes,       35 copies)
map_remove              3.1 ns  (36777220 hashes, 133886195 copies)
map_toggle             32.8 ns  (20624999 hashes, 74999995 copies)    0.0 MB
map_iterate             3.1 ns  (26777220 hashes, 123886195 copies)

stresshashfunction map_size=256 stride=1: 37.5ns/insertion
stresshashfunction map_size=256 stride=256: 25.0ns/insertion
stresshashfunction map_size=1024 stride=1: 43.8ns/insertion
stresshashfunction map_size=1024 stride=1024: 43.8ns/insertion

RABBIT UNORDERED_MAP (4 byte objects, 10000000 iterations):
map_grow               42.2 ns  (26782067 hashes, 127479808 copies)  132.6 MB
map_predict/grow        7.8 ns  (10000000 hashes, 10000288 copies)  132.0 MB
map_replace             6.3 ns  (36782067 hashes, 127479808 copies)
map_fetch_random        9.4 ns  (10000000 hashes, 127479808 copies)  132.0 MB
map_fetch_sequential    4.7 ns  (10000000 hashes, 127479808 copies)  132.0 MB
map_fetch_empty         3.1 ns  (10000000 hashes,        0 copies)
map_remove              7.8 ns  (36782067 hashes, 137479808 copies)
map_toggle             67.2 ns  (20000000 hashes, 19999998 copies)
map_iterate             4.7 ns  (26782067 hashes, 157479808 copies)

stresshashfunction map_size=256 stride=1: 6.3ns/insertion
stresshashfunction map_size=256 stride=256: 18.8ns/insertion
stresshashfunction map_size=1024 stride=1: 0.0ns/insertion
stresshashfunction map_size=1024 stride=1024: 12.5ns/insertion






Using
------------------------------------------------------------------

##include <rabbit\unordered_map>

##include <rabbit\unordered_set>



