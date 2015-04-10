# rabbit
stl compatible hashtable (rabbit::unordered_map, rabbit::unordered_set)

It also seems to be performing well

Some timing results with google dense hash compiled with MSVC 2012
------------------------------------------------------------------

(4 byte objects, 10000000 iterations):

Average over 10000000 iterations
Current time (GMT): Fri Apr 10 20:06:38 2015

*** WARNING ***: sys/resources.h was not found, so all times
                 reported are wall-clock time, not user time

DENSE_HASH_MAP (4 byte objects, 10000000 iterations):
map_grow               81.4 ns  (26777220 hashes, 113886195 copies)  256.6 MB
map_predict/grow       53.1 ns  (10000000 hashes, 63554475 copies)  256.0 MB
map_replace             2.7 ns  (36777220 hashes, 113886195 copies)
map_fetch_random       14.7 ns  (36777220 hashes, 113886195 copies)  256.0 MB
map_fetch_sequential    6.2 ns  (36777220 hashes, 113886195 copies)  256.0 MB
map_fetch_empty         5.9 ns  (       0 hashes,       35 copies)
map_remove             17.1 ns  (36777220 hashes, 123886195 copies)
map_toggle             90.8 ns  (20624999 hashes, 64999995 copies)    0.0 MB
map_iterate             4.1 ns  (26777220 hashes, 113886195 copies)

stresshashfunction map_size=256 stride=1: 70.4ns/insertion
stresshashfunction map_size=256 stride=256: 61.6ns/insertion
stresshashfunction map_size=1024 stride=1: 76.8ns/insertion
stresshashfunction map_size=1024 stride=1024: 80.4ns/insertion

RABBIT UNORDERED_MAP (4 byte objects, 10000000 iterations):
map_grow               28.6 ns  (19227894 hashes, 19227886 copies)  102.1 MB
map_predict/grow       12.7 ns  (10000000 hashes, 10000000 copies)  125.9 MB
map_replace            12.6 ns  (29227894 hashes, 19227886 copies)
map_fetch_random       26.8 ns  (29227894 hashes, 19227886 copies)  101.9 MB
map_fetch_sequential   13.3 ns  (29227894 hashes, 19227886 copies)  101.9 MB
map_fetch_empty         1.1 ns  (       0 hashes,        0 copies)
map_remove             22.4 ns  (29227894 hashes, 29227886 copies)
map_toggle             64.1 ns  (20000000 hashes, 20000000 copies)
map_iterate             2.7 ns  (19227894 hashes, 19227886 copies)

stresshashfunction map_size=256 stride=1: 10.0ns/insertion
stresshashfunction map_size=256 stride=256: 12.4ns/insertion
stresshashfunction map_size=1024 stride=1: 10.4ns/insertion
stresshashfunction map_size=1024 stride=1024: 12.4ns/insertion

Using:
------------------------------------------------------------------

include rabbit\unordered_map

