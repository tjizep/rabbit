# rabbit v 1.0 r1

stl compatible hashtable (rabbit::unordered_map, rabbit::unordered_set)

It also seems to be performing well

Some timing results with google dense hash compiled with VS 11 (2012)
----------------------------------------------------------------------

======
Average over 20000000 iterations
Current time (GMT): Mon Apr 27 10:00:03 2015

*** WARNING ***: sys/resources.h was not found, so all times
                 reported are wall-clock time, not user time

DENSE_HASH_MAP (4 byte objects, 20000000 iterations):
map_grow               42.2 ns  (53554437 hashes, 247772283 copies)  513.0 MB
map_predict/grow       12.4 ns  (20000000 hashes, 147108907 copies)  512.0 MB
map_replace             7.0 ns  (73554437 hashes, 247772283 copies)
map_fetch_random        9.1 ns  (20000000 hashes, 247772283 copies)  512.0 MB
map_fetch_sequential    3.0 ns  (20000000 hashes, 247772283 copies)  512.0 MB
map_fetch_empty         1.4 ns  (       0 hashes,       35 copies)
map_remove              3.4 ns  (73554437 hashes, 267772283 copies)
map_toggle             34.6 ns  (41249999 hashes, 149999995 copies)    0.0 MB
map_iterate             3.1 ns  (53554437 hashes, 247772283 copies)

stresshashfunction map_size=256 stride=1: 38.0ns/insertion
stresshashfunction map_size=256 stride=256: 17.8ns/insertion
stresshashfunction map_size=1024 stride=1: 58.2ns/insertion
stresshashfunction map_size=1024 stride=1024: 39.8ns/insertion

RABBIT UNORDERED_MAP (4 byte objects, 20000000 iterations):
map_grow               40.8 ns  (53562988 hashes, 53562964 copies)  212.3 MB
map_predict/grow       20.5 ns  (20000000 hashes, 20000000 copies)  212.3 MB
map_replace             5.4 ns  (73562988 hashes, 53562964 copies)
map_fetch_random        9.0 ns  (20000000 hashes, 53562964 copies)  211.3 MB
map_fetch_sequential    4.2 ns  (20000000 hashes, 53562964 copies)  212.3 MB
map_fetch_empty         1.1 ns  (       0 hashes,        0 copies)
map_remove              8.4 ns  (73562988 hashes, 53562964 copies)
map_toggle             32.4 ns  (40000000 hashes, 20000000 copies)
map_iterate             3.6 ns  (53562988 hashes, 113562964 copies)

stresshashfunction map_size=256 stride=1: 18.6ns/insertion
stresshashfunction map_size=256 stride=256: 19.6ns/insertion
stresshashfunction map_size=1024 stride=1: 18.8ns/insertion
stresshashfunction map_size=1024 stride=1024: 19.8ns/insertion

DENSE_HASH_MAP (8 byte objects, 10000000 iterations):
map_grow               52.8 ns  (26777220 hashes, 123886195 copies)  385.4 MB
map_predict/grow       10.5 ns  (10000000 hashes, 73554475 copies)  384.0 MB
map_replace             3.8 ns  (36777220 hashes, 123886195 copies)
map_fetch_random       19.4 ns  (10000000 hashes, 123886195 copies)  384.3 MB
map_fetch_sequential    8.6 ns  (10000000 hashes, 123886195 copies)  383.3 MB
map_fetch_empty         4.3 ns  (       0 hashes,       35 copies)
map_remove              9.5 ns  (36777220 hashes, 133886195 copies)
map_toggle             37.8 ns  (20624999 hashes, 74999995 copies)
map_iterate             3.6 ns  (26777220 hashes, 123886195 copies)

stresshashfunction map_size=256 stride=1: 38.0ns/insertion
stresshashfunction map_size=256 stride=256: 16.0ns/insertion
stresshashfunction map_size=1024 stride=1: 58.4ns/insertion
stresshashfunction map_size=1024 stride=1024: 41.6ns/insertion

RABBIT UNORDERED_MAP (8 byte objects, 10000000 iterations):
map_grow               51.3 ns  (26782806 hashes, 26782783 copies)  171.6 MB
map_predict/grow       23.9 ns  (10000000 hashes, 10000000 copies)  170.2 MB
map_replace             8.2 ns  (36782806 hashes, 26782783 copies)
map_fetch_random       14.0 ns  (10000000 hashes, 26782783 copies)  169.8 MB
map_fetch_sequential    6.0 ns  (10000000 hashes, 26782783 copies)  169.7 MB
map_fetch_empty         2.1 ns  (       0 hashes,        0 copies)
map_remove             10.5 ns  (36782807 hashes, 26782783 copies)
map_toggle             38.3 ns  (20000000 hashes, 10000000 copies)
map_iterate             4.7 ns  (26782806 hashes, 56782783 copies)

stresshashfunction map_size=256 stride=1: 17.6ns/insertion
stresshashfunction map_size=256 stride=256: 20.0ns/insertion
stresshashfunction map_size=1024 stride=1: 19.2ns/insertion
stresshashfunction map_size=1024 stride=1024: 18.8ns/insertion

DENSE_HASH_MAP (16 byte objects, 5000000 iterations):
map_grow              103.7 ns  (13388611 hashes, 61943147 copies)  321.2 MB
map_predict/grow       35.2 ns  ( 5000000 hashes, 36777259 copies)  320.0 MB
map_replace            26.6 ns  (18388611 hashes, 61943147 copies)
map_fetch_random       47.2 ns  ( 5000000 hashes, 61943147 copies)  321.2 MB
map_fetch_sequential   23.0 ns  ( 5000000 hashes, 61943147 copies)  320.0 MB
map_fetch_empty        13.0 ns  (       0 hashes,       35 copies)
map_remove             22.8 ns  (18388611 hashes, 66943147 copies)
map_toggle             73.2 ns  (10312499 hashes, 37499995 copies)    0.0 MB
map_iterate             5.0 ns  (13388611 hashes, 61943147 copies)

RABBIT UNORDERED_MAP (16 byte objects, 5000000 iterations):
map_grow               80.7 ns  (13369456 hashes, 13369434 copies)  149.6 MB
map_predict/grow       40.8 ns  ( 5000000 hashes,  5000000 copies)  149.1 MB
map_replace            23.2 ns  (18369456 hashes, 13369434 copies)
map_fetch_random       40.2 ns  ( 5000000 hashes, 13369434 copies)  149.1 MB
map_fetch_sequential   21.8 ns  ( 5000000 hashes, 13369434 copies)  149.1 MB
map_fetch_empty        12.8 ns  (       0 hashes,        0 copies)
map_remove             27.2 ns  (18370236 hashes, 13369434 copies)
map_toggle             68.8 ns  (10000000 hashes,  5000000 copies)
map_iterate             6.0 ns  (13369456 hashes, 28369434 copies)

DENSE_HASH_MAP (256 byte objects, 625000 iterations):
map_grow             1234.4 ns  ( 1673576 hashes,  7742963 copies)  520.5 MB
map_predict/grow      344.2 ns  (  625000 hashes,  4597195 copies)  520.0 MB
map_replace           172.9 ns  ( 2298576 hashes,  7742963 copies)
map_fetch_random      171.3 ns  (  625000 hashes,  7742963 copies)  520.2 MB
map_fetch_sequential  164.9 ns  (  625000 hashes,  7742963 copies)  520.0 MB
map_fetch_empty        20.8 ns  (       0 hashes,       35 copies)
map_remove            253.0 ns  ( 2298576 hashes,  8367963 copies)
map_toggle            406.7 ns  ( 1289062 hashes,  4687515 copies)    0.0 MB
map_iterate            30.4 ns  ( 1673576 hashes,  7742963 copies)

RABBIT UNORDERED_MAP (256 byte objects, 625000 iterations):
map_grow              637.2 ns  ( 1626867 hashes,  1626848 copies)  189.5 MB
map_predict/grow      313.8 ns  (  625000 hashes,   625000 copies)  189.6 MB
map_replace           172.9 ns  ( 2251867 hashes,  1626848 copies)
map_fetch_random      177.7 ns  (  625000 hashes,  1626848 copies)  190.5 MB
map_fetch_sequential  169.7 ns  (  625000 hashes,  1626848 copies)  188.5 MB
map_fetch_empty        16.0 ns  (       0 hashes,        0 copies)
map_remove            169.7 ns  ( 2255270 hashes,  1626848 copies)
map_toggle            225.7 ns  ( 1250000 hashes,   625000 copies)
map_iterate            76.9 ns  ( 1626867 hashes,  3501848 copies)



Using Rabbit
------------------------------------------------------------------

##include <rabbit\unordered_map>

##include <rabbit\unordered_set>



