# rabbit v 1.0 r1

stl compatible hashtable (rabbit::unordered_map, rabbit::unordered_set)

It also seems to be performing well

Some timing results with google dense hash compiled with VS 11 (2012)
----------------------------------------------------------------------

======
Average over 15000000 iterations
Current time (GMT): Sun Apr 26 13:37:41 2015

*** WARNING ***: sys/resources.h was not found, so all times
                 reported are wall-clock time, not user time

DENSE_HASH_MAP (4 byte objects, 15000000 iterations):
map_grow               31.0 ns  (31777220 hashes, 143886195 copies)  257.0 MB
map_predict/grow       12.0 ns  (15000000 hashes, 93554475 copies)  256.0 MB
map_replace             7.7 ns  (46777220 hashes, 143886195 copies)
map_fetch_random        9.0 ns  (15000000 hashes, 143886195 copies)  256.0 MB
map_fetch_sequential    2.7 ns  (15000000 hashes, 143886195 copies)  256.0 MB
map_fetch_empty         1.3 ns  (       0 hashes,       35 copies)
map_remove              3.3 ns  (46777220 hashes, 158886195 copies)
map_toggle             34.0 ns  (30937499 hashes, 112499995 copies)    0.0 MB
map_iterate             2.3 ns  (31777220 hashes, 143886195 copies)

stresshashfunction map_size=256 stride=1: 38.7ns/insertion
stresshashfunction map_size=256 stride=256: 18.7ns/insertion
stresshashfunction map_size=1024 stride=1: 56.0ns/insertion
stresshashfunction map_size=1024 stride=1024: 45.3ns/insertion

RABBIT UNORDERED_MAP (4 byte objects, 15000000 iterations):
map_grow               30.3 ns  (31796665 hashes, 31796646 copies)  125.8 MB
map_predict/grow       17.0 ns  (15000000 hashes, 15000000 copies)  124.7 MB
map_replace             4.3 ns  (46796665 hashes, 31796646 copies)
map_fetch_random        7.7 ns  (15000000 hashes, 31796646 copies)  125.2 MB
map_fetch_sequential    3.3 ns  (15000000 hashes, 31796646 copies)  125.2 MB
map_fetch_empty         1.0 ns  (       0 hashes,        0 copies)
map_remove              7.0 ns  (46796665 hashes, 31796646 copies)
map_toggle             24.0 ns  (30000000 hashes, 15000000 copies)
map_iterate             3.3 ns  (31796665 hashes, 76796646 copies)

stresshashfunction map_size=256 stride=1: 17.3ns/insertion
stresshashfunction map_size=256 stride=256: 17.3ns/insertion
stresshashfunction map_size=1024 stride=1: 16.0ns/insertion
stresshashfunction map_size=1024 stride=1024: 17.3ns/insertion

DENSE_HASH_MAP (8 byte objects, 7500000 iterations):
map_grow               38.0 ns  (15888611 hashes, 71943147 copies)  192.7 MB
map_predict/grow       10.0 ns  ( 7500000 hashes, 46777259 copies)  192.0 MB
map_replace             3.3 ns  (23388611 hashes, 71943147 copies)
map_fetch_random       19.3 ns  ( 7500000 hashes, 71943147 copies)  191.3 MB
map_fetch_sequential    8.0 ns  ( 7500000 hashes, 71943147 copies)  192.6 MB
map_fetch_empty         4.7 ns  (       0 hashes,       35 copies)
map_remove              9.3 ns  (23388611 hashes, 79443147 copies)
map_toggle             37.3 ns  (15468749 hashes, 56249995 copies)    0.0 MB
map_iterate             2.7 ns  (15888611 hashes, 71943147 copies)

stresshashfunction map_size=256 stride=1: 42.7ns/insertion
stresshashfunction map_size=256 stride=256: 18.7ns/insertion
stresshashfunction map_size=1024 stride=1: 61.3ns/insertion
stresshashfunction map_size=1024 stride=1024: 40.0ns/insertion

RABBIT UNORDERED_MAP (8 byte objects, 7500000 iterations):
map_grow               36.0 ns  (15907032 hashes, 15907014 copies)   95.3 MB
map_predict/grow       18.7 ns  ( 7500000 hashes,  7500000 copies)   94.6 MB
map_replace             5.3 ns  (23407032 hashes, 15907014 copies)
map_fetch_random       12.0 ns  ( 7500000 hashes, 15907014 copies)   95.5 MB
map_fetch_sequential    5.3 ns  ( 7500000 hashes, 15907014 copies)   93.7 MB
map_fetch_empty         2.0 ns  (       0 hashes,        0 copies)
map_remove              8.7 ns  (23412388 hashes, 15907014 copies)
map_toggle             48.7 ns  (25411850 hashes, 11708401 copies)   94.1 MB
map_iterate             4.0 ns  (15907032 hashes, 38407014 copies)

stresshashfunction map_size=256 stride=1: 13.3ns/insertion
stresshashfunction map_size=256 stride=256: 18.7ns/insertion
stresshashfunction map_size=1024 stride=1: 18.7ns/insertion
stresshashfunction map_size=1024 stride=1024: 16.0ns/insertion

DENSE_HASH_MAP (16 byte objects, 3750000 iterations):
map_grow               80.0 ns  ( 7944306 hashes, 35971619 copies)  161.2 MB
map_predict/grow       34.7 ns  ( 3750000 hashes, 23388651 copies)  160.0 MB
map_replace            26.7 ns  (11694306 hashes, 35971619 copies)
map_fetch_random       44.3 ns  ( 3750000 hashes, 35971619 copies)  161.3 MB
map_fetch_sequential   22.7 ns  ( 3750000 hashes, 35971619 copies)  160.0 MB
map_fetch_empty        13.3 ns  (       0 hashes,       35 copies)
map_remove             22.7 ns  (11694306 hashes, 39721619 copies)
map_toggle             74.7 ns  ( 7734374 hashes, 28124995 copies)    0.0 MB
map_iterate             2.7 ns  ( 7944306 hashes, 35971619 copies)

RABBIT UNORDERED_MAP (16 byte objects, 3750000 iterations):
map_grow               58.7 ns  ( 7958716 hashes,  7958699 copies)   78.1 MB
map_predict/grow       34.7 ns  ( 3750000 hashes,  3750000 copies)   79.4 MB
map_replace            21.3 ns  (11708716 hashes,  7958699 copies)
map_fetch_random       37.3 ns  ( 3750000 hashes,  7958699 copies)   78.8 MB
map_fetch_sequential   21.3 ns  ( 3750000 hashes,  7958699 copies)   79.3 MB
map_fetch_empty        12.0 ns  (       0 hashes,        0 copies)
map_remove             25.3 ns  (11759119 hashes,  7958699 copies)
map_toggle             54.7 ns  ( 7500000 hashes,  3750000 copies)    0.0 MB
map_iterate             5.3 ns  ( 7958716 hashes, 19208699 copies)

DENSE_HASH_MAP (256 byte objects, 468750 iterations):
map_grow              928.1 ns  (  993037 hashes,  4496515 copies)  261.0 MB
map_predict/grow      341.4 ns  (  468750 hashes,  2923619 copies)  260.0 MB
map_replace           160.0 ns  ( 1461787 hashes,  4496515 copies)
map_fetch_random      170.7 ns  (  468750 hashes,  4496515 copies)  260.0 MB
map_fetch_sequential  170.7 ns  (  468750 hashes,  4496515 copies)  259.0 MB
map_fetch_empty        10.7 ns  (       0 hashes,       35 copies)
map_remove            245.4 ns  ( 1461787 hashes,  4965265 copies)
map_toggle            405.4 ns  (  966796 hashes,  3515625 copies)    0.1 MB
map_iterate            21.3 ns  (  993037 hashes,  4496515 copies)

RABBIT UNORDERED_MAP (256 byte objects, 468750 iterations):
map_grow              490.7 ns  (  708139 hashes,   708124 copies)  127.0 MB
map_predict/grow      298.7 ns  (  468750 hashes,   468750 copies)  128.1 MB
map_replace           160.0 ns  ( 1176889 hashes,   708124 copies)
map_fetch_random      170.7 ns  (  468750 hashes,   708124 copies)  128.9 MB
map_fetch_sequential  170.7 ns  (  468750 hashes,   708124 copies)  126.9 MB
map_fetch_empty        10.7 ns  (       0 hashes,        0 copies)
map_remove            224.0 ns  ( 1536865 hashes,   708124 copies)
map_toggle            213.4 ns  (  937500 hashes,   468750 copies)    0.0 MB
map_iterate            85.4 ns  (  708139 hashes,  2114374 copies)



Using Rabbit
------------------------------------------------------------------

##include <rabbit\unordered_map>

##include <rabbit\unordered_set>



