# rabbit
stl compatible hashtable (rabbit::unordered_map, rabbit::unordered_set)

It also seems to be performing well

Some timing results with google dense hash compiled with VS 11 (2012)
----------------------------------------------------------------------
======
Average over 10000000 iterations
Current time (GMT): Fri Apr 24 20:31:21 2015

*** WARNING ***: sys/resources.h was not found, so all times
                 reported are wall-clock time, not user time

DENSE_HASH_MAP (4 byte objects, 10000000 iterations):
map_grow               41.0 ns  (26777220 hashes, 123886195 copies)  257.0 MB
map_predict/grow       12.5 ns  (10000000 hashes, 73554475 copies)  256.0 MB
map_replace             7.5 ns  (36777220 hashes, 123886195 copies)
map_fetch_random        9.0 ns  (10000000 hashes, 123886195 copies)  256.0 MB
map_fetch_sequential    3.0 ns  (10000000 hashes, 123886195 copies)  256.0 MB
map_fetch_empty         1.0 ns  (       0 hashes,       35 copies)
map_remove              3.5 ns  (36777220 hashes, 133886195 copies)
map_toggle             33.0 ns  (20624999 hashes, 74999995 copies)    0.0 MB
map_iterate             3.0 ns  (26777220 hashes, 123886195 copies)

stresshashfunction map_size=256 stride=1: 38.0ns/insertion
stresshashfunction map_size=256 stride=256: 16.0ns/insertion
stresshashfunction map_size=1024 stride=1: 60.0ns/insertion
stresshashfunction map_size=1024 stride=1024: 42.0ns/insertion

RABBIT UNORDERED_MAP (4 byte objects, 10000000 iterations):
map_grow               47.0 ns  (26782623 hashes, 26782602 copies)  136.0 MB
map_predict/grow       18.5 ns  (10000000 hashes, 10000000 copies)  136.0 MB
map_replace             4.0 ns  (36782623 hashes, 26782602 copies)
map_fetch_random        8.5 ns  (10000000 hashes, 26782602 copies)  136.0 MB
map_fetch_sequential    3.5 ns  (10000000 hashes, 26782602 copies)  136.0 MB
map_fetch_empty         1.0 ns  (       0 hashes,        0 copies)
map_remove             11.0 ns  (36782623 hashes, 36782602 copies)
map_toggle             30.0 ns  (20000000 hashes, 20000000 copies)
map_iterate             5.5 ns  (26782623 hashes, 56782602 copies)

stresshashfunction map_size=256 stride=1: 22.0ns/insertion
stresshashfunction map_size=256 stride=256: 18.0ns/insertion
stresshashfunction map_size=1024 stride=1: 22.0ns/insertion
stresshashfunction map_size=1024 stride=1024: 20.0ns/insertion

DENSE_HASH_MAP (8 byte objects, 5000000 iterations):
map_grow               53.0 ns  (13388611 hashes, 61943147 copies)  193.1 MB
map_predict/grow        9.0 ns  ( 5000000 hashes, 36777259 copies)  192.0 MB
map_replace             3.0 ns  (18388611 hashes, 61943147 copies)
map_fetch_random       20.0 ns  ( 5000000 hashes, 61943147 copies)  193.1 MB
map_fetch_sequential    8.0 ns  ( 5000000 hashes, 61943147 copies)  192.3 MB
map_fetch_empty         4.0 ns  (       0 hashes,       35 copies)
map_remove              9.0 ns  (18388611 hashes, 66943147 copies)
map_toggle             38.0 ns  (10312499 hashes, 37499995 copies)    0.0 MB
map_iterate             3.0 ns  (13388611 hashes, 61943147 copies)

stresshashfunction map_size=256 stride=1: 44.0ns/insertion
stresshashfunction map_size=256 stride=256: 12.0ns/insertion
stresshashfunction map_size=1024 stride=1: 56.0ns/insertion
stresshashfunction map_size=1024 stride=1024: 44.0ns/insertion

RABBIT UNORDERED_MAP (8 byte objects, 5000000 iterations):
map_grow               59.0 ns  (13393601 hashes, 13393581 copies)  100.0 MB
map_predict/grow       24.0 ns  ( 5000000 hashes,  5000000 copies)  100.0 MB
map_replace             5.0 ns  (18393601 hashes, 13393581 copies)
map_fetch_random       12.0 ns  ( 5000000 hashes, 13393581 copies)  100.0 MB
map_fetch_sequential    6.0 ns  ( 5000000 hashes, 13393581 copies)  100.0 MB
map_fetch_empty         2.0 ns  (       0 hashes,        0 copies)
map_remove             14.0 ns  (18396305 hashes, 18393581 copies)
map_toggle             37.0 ns  (10000000 hashes, 10000000 copies)
map_iterate             7.0 ns  (13393601 hashes, 28393581 copies)

stresshashfunction map_size=256 stride=1: 24.0ns/insertion
stresshashfunction map_size=256 stride=256: 20.0ns/insertion
stresshashfunction map_size=1024 stride=1: 16.0ns/insertion
stresshashfunction map_size=1024 stride=1024: 24.0ns/insertion

DENSE_HASH_MAP (16 byte objects, 2500000 iterations):
map_grow              102.0 ns  ( 6694306 hashes, 30971619 copies)  160.0 MB
map_predict/grow       34.0 ns  ( 2500000 hashes, 18388651 copies)  160.0 MB
map_replace            26.0 ns  ( 9194306 hashes, 30971619 copies)
map_fetch_random       44.0 ns  ( 2500000 hashes, 30971619 copies)  159.4 MB
map_fetch_sequential   24.0 ns  ( 2500000 hashes, 30971619 copies)  159.4 MB
map_fetch_empty        12.0 ns  (       0 hashes,       35 copies)
map_remove             22.0 ns  ( 9194306 hashes, 33471619 copies)
map_toggle             72.0 ns  ( 5156249 hashes, 18749995 copies)
map_iterate             4.0 ns  ( 6694306 hashes, 30971619 copies)

RABBIT UNORDERED_MAP (16 byte objects, 2500000 iterations):
map_grow               64.0 ns  ( 3026621 hashes,  3026601 copies)   81.5 MB
map_predict/grow       38.0 ns  ( 2500000 hashes,  2500000 copies)   81.3 MB
map_replace            20.0 ns  ( 5526621 hashes,  3026601 copies)
map_fetch_random       36.0 ns  ( 2500000 hashes,  3026601 copies)   82.0 MB
map_fetch_sequential   20.0 ns  ( 2500000 hashes,  3026601 copies)   82.0 MB
map_fetch_empty        12.0 ns  (       0 hashes,        0 copies)
map_remove             28.0 ns  ( 5536621 hashes,  5526601 copies)
map_toggle             62.0 ns  ( 5000000 hashes,  5000000 copies)
map_iterate             8.0 ns  ( 3026621 hashes, 10526601 copies)



Using
------------------------------------------------------------------

##include <rabbit\unordered_map>

##include <rabbit\unordered_set>



