# rabbit
stl compatible hashtable (rabbit::unordered_map, rabbit::unordered_set)

It also seems to be performing well

Some timing results with google dense hash compiled with VS 11 (2012)
----------------------------------------------------------------------
======
Average over 10000000 iterations
Current time (GMT): Sun Apr 26 09:43:14 2015

*** WARNING ***: sys/resources.h was not found, so all times
                 reported are wall-clock time, not user time

DENSE_HASH_MAP (4 byte objects, 10000000 iterations):
map_grow               40.3 ns  (26777220 hashes, 123886195 copies)  257.0 MB
map_predict/grow       12.1 ns  (10000000 hashes, 73554475 copies)  256.0 MB
map_replace             7.5 ns  (36777220 hashes, 123886195 copies)
map_fetch_random        8.9 ns  (10000000 hashes, 123886195 copies)  256.0 MB
map_fetch_sequential    2.9 ns  (10000000 hashes, 123886195 copies)  256.0 MB
map_fetch_empty         1.3 ns  (       0 hashes,       35 copies)
map_remove              3.3 ns  (36777220 hashes, 133886195 copies)
map_toggle             33.5 ns  (20624999 hashes, 74999995 copies)    0.0 MB
map_iterate             3.0 ns  (26777220 hashes, 123886195 copies)

stresshashfunction map_size=256 stride=1: 37.2ns/insertion
stresshashfunction map_size=256 stride=256: 18.4ns/insertion
stresshashfunction map_size=1024 stride=1: 57.6ns/insertion
stresshashfunction map_size=1024 stride=1024: 40.8ns/insertion

RABBIT UNORDERED_MAP (4 byte objects, 10000000 iterations):
map_grow               35.9 ns  (26796665 hashes, 26796646 copies)  106.7 MB
map_predict/grow       17.0 ns  (10000000 hashes, 10000000 copies)  105.6 MB
map_replace             4.3 ns  (36796665 hashes, 26796646 copies)
map_fetch_random        7.6 ns  (10000000 hashes, 26796646 copies)  106.2 MB
map_fetch_sequential    3.4 ns  (10000000 hashes, 26796646 copies)  106.2 MB
map_fetch_empty         1.0 ns  (       0 hashes,        0 copies)
map_remove              9.3 ns  (36796665 hashes, 26796646 copies)
map_toggle             25.6 ns  (20000000 hashes, 10000000 copies)
map_iterate             4.1 ns  (26796665 hashes, 56796646 copies)

stresshashfunction map_size=256 stride=1: 17.2ns/insertion
stresshashfunction map_size=256 stride=256: 14.8ns/insertion
stresshashfunction map_size=1024 stride=1: 16.0ns/insertion
stresshashfunction map_size=1024 stride=1024: 16.4ns/insertion

DENSE_HASH_MAP (8 byte objects, 5000000 iterations):
map_grow               51.2 ns  (13388611 hashes, 61943147 copies)  192.7 MB
map_predict/grow       10.2 ns  ( 5000000 hashes, 36777259 copies)  192.0 MB
map_replace             3.8 ns  (18388611 hashes, 61943147 copies)
map_fetch_random       19.2 ns  ( 5000000 hashes, 61943147 copies)  191.3 MB
map_fetch_sequential    8.0 ns  ( 5000000 hashes, 61943147 copies)  192.6 MB
map_fetch_empty         4.4 ns  (       0 hashes,       35 copies)
map_remove              9.0 ns  (18388611 hashes, 66943147 copies)
map_toggle             37.6 ns  (10312499 hashes, 37499995 copies)    0.0 MB
map_iterate             3.4 ns  (13388611 hashes, 61943147 copies)

stresshashfunction map_size=256 stride=1: 39.2ns/insertion
stresshashfunction map_size=256 stride=256: 18.4ns/insertion
stresshashfunction map_size=1024 stride=1: 58.4ns/insertion
stresshashfunction map_size=1024 stride=1024: 41.6ns/insertion

RABBIT UNORDERED_MAP (8 byte objects, 5000000 iterations):
map_grow               45.0 ns  (13407032 hashes, 13407014 copies)   85.6 MB
map_predict/grow       19.2 ns  ( 5000000 hashes,  5000000 copies)   85.1 MB
map_replace             5.4 ns  (18407032 hashes, 13407014 copies)
map_fetch_random       12.2 ns  ( 5000000 hashes, 13407014 copies)   84.2 MB
map_fetch_sequential    5.0 ns  ( 5000000 hashes, 13407014 copies)   86.1 MB
map_fetch_empty         1.8 ns  (       0 hashes,        0 copies)
map_remove             11.0 ns  (18412388 hashes, 13407014 copies)
map_toggle             56.8 ns  (20411850 hashes,  9208401 copies)   84.6 MB
map_iterate             5.4 ns  (13407032 hashes, 28407014 copies)

stresshashfunction map_size=256 stride=1: 14.4ns/insertion
stresshashfunction map_size=256 stride=256: 14.4ns/insertion
stresshashfunction map_size=1024 stride=1: 16.8ns/insertion
stresshashfunction map_size=1024 stride=1024: 17.6ns/insertion

DENSE_HASH_MAP (16 byte objects, 2500000 iterations):
map_grow              102.9 ns  ( 6694306 hashes, 30971619 copies)  161.2 MB
map_predict/grow       35.6 ns  ( 2500000 hashes, 18388651 copies)  160.0 MB
map_replace            26.4 ns  ( 9194306 hashes, 30971619 copies)
map_fetch_random       43.2 ns  ( 2500000 hashes, 30971619 copies)  159.4 MB
map_fetch_sequential   22.8 ns  ( 2500000 hashes, 30971619 copies)  161.3 MB
map_fetch_empty        12.8 ns  (       0 hashes,       35 copies)
map_remove             22.4 ns  ( 9194306 hashes, 33471619 copies)
map_toggle             74.0 ns  ( 5156249 hashes, 18749995 copies)    0.0 MB
map_iterate             4.4 ns  ( 6694306 hashes, 30971619 copies)

RABBIT UNORDERED_MAP (16 byte objects, 2500000 iterations):
map_grow               72.0 ns  ( 6708716 hashes,  6708699 copies)   75.1 MB
map_predict/grow       35.2 ns  ( 2500000 hashes,  2500000 copies)   74.1 MB
map_replace            21.2 ns  ( 9208716 hashes,  6708699 copies)
map_fetch_random       37.6 ns  ( 2500000 hashes,  6708699 copies)   73.8 MB
map_fetch_sequential   21.2 ns  ( 2500000 hashes,  6708699 copies)   75.3 MB
map_fetch_empty        12.0 ns  (       0 hashes,        0 copies)
map_remove             28.0 ns  ( 9259119 hashes,  6708699 copies)
map_toggle             57.2 ns  ( 5000000 hashes,  2500000 copies)    0.0 MB
map_iterate             6.4 ns  ( 6708716 hashes, 14208699 copies)



Using Rabbit
------------------------------------------------------------------

##include <rabbit\unordered_map>

##include <rabbit\unordered_set>



