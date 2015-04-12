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

DENSE_HASH_MAP (8 byte objects, 5000000 iterations):
map_grow              110.0 ns  (13388611 hashes, 56943147 copies)  192.2 MB
map_predict/grow       66.0 ns  ( 5000000 hashes, 31777259 copies)  192.0 MB
map_replace             5.0 ns  (18388611 hashes, 56943147 copies)
map_fetch_random       27.0 ns  (18388611 hashes, 56943147 copies)  192.0 MB
map_fetch_sequential   13.0 ns  (18388611 hashes, 56943147 copies)  192.0 MB
map_fetch_empty         8.0 ns  (       0 hashes,       35 copies)
map_remove             25.0 ns  (18388611 hashes, 61943147 copies)
map_toggle            114.0 ns  (10312499 hashes, 32499995 copies)    0.0 MB
map_iterate             4.0 ns  (13388611 hashes, 56943147 copies)

stresshashfunction map_size=256 stride=1: 68.0ns/insertion
stresshashfunction map_size=256 stride=256: 56.0ns/insertion
stresshashfunction map_size=1024 stride=1: 80.1ns/insertion
stresshashfunction map_size=1024 stride=1024: 88.1ns/insertion

RABBIT UNORDERED_MAP (8 byte objects, 5000000 iterations):
map_grow               59.3 ns  (12813307 hashes, 12813296 copies)   98.3 MB
map_predict/grow       19.0 ns  ( 5000000 hashes,  5000000 copies)   98.0 MB
map_replace            13.0 ns  (17813307 hashes, 12813296 copies)
map_fetch_random       27.0 ns  (17813307 hashes, 12813296 copies)   98.0 MB
map_fetch_sequential   15.0 ns  (17813307 hashes, 12813296 copies)   98.0 MB
map_fetch_empty         2.0 ns  (       0 hashes,        0 copies)
map_remove             27.0 ns  (17813307 hashes, 17813296 copies)
map_toggle             75.0 ns  (10000000 hashes, 10000000 copies)
map_iterate             5.0 ns  (12813307 hashes, 12813296 copies)

stresshashfunction map_size=256 stride=1: 12.0ns/insertion
stresshashfunction map_size=256 stride=256: 32.0ns/insertion
stresshashfunction map_size=1024 stride=1: 12.0ns/insertion
stresshashfunction map_size=1024 stride=1024: 68.0ns/insertion
Using:
------------------------------------------------------------------

include rabbit\unordered_map

