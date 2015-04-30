# rabbit v 1.0 r4

stl compatible hashtable (rabbit::unordered_map, rabbit::unordered_set)

It also seems to be performing well

Using Rabbit
------------------------------------------------------------------

##include <rabbit\unordered_map>
Sample:

if(1){rabbit::sparse_unordered_map<k,v> m1;}
if(2){rabbit::unordered_map<k,v> m2;}

##include <rabbit\unordered_set>



Some timing results with google dense/sparse hash compiled with VS 11 (2012)
----------------------------------------------------------------------------

======
Average over 10000000 iterations
Current time (GMT): Wed Apr 29 18:34:04 2015

*** WARNING ***: sys/resources.h was not found, so all times
                 reported are wall-clock time, not user time

SPARSE_HASH_MAP (4 byte objects, 10000000 iterations):
map_grow              273.8 ns  (23421757 hashes, 53421815 copies)   85.6 MB
map_predict/grow      132.0 ns  (10000000 hashes, 40000005 copies)   84.8 MB
map_replace            22.4 ns  (33421757 hashes, 53421815 copies)
map_fetch_random       98.9 ns  (10000000 hashes, 53421815 copies)   84.6 MB
map_fetch_sequential   27.2 ns  (10000000 hashes, 53421815 copies)   84.6 MB
map_fetch_empty        10.1 ns  (       0 hashes,        1 copies)
map_remove             31.3 ns  (33421757 hashes, 63421815 copies)
map_toggle            192.6 ns  (20399999 hashes, 51599997 copies)
map_iterate             3.1 ns  (23421757 hashes, 53421815 copies)

stresshashfunction map_size=256 stride=1: 344.8ns/insertion
stresshashfunction map_size=256 stride=256: 202.4ns/insertion
stresshashfunction map_size=1024 stride=1: 544.9ns/insertion
stresshashfunction map_size=1024 stride=1024: 449.3ns/insertion

RABBIT SPARSE_UNORDERED_MAP (4 byte objects, 10000000 iterations):
map_grow               45.7 ns  (24279145 hashes, 24279122 copies)   85.9 MB
map_predict/grow       16.6 ns  (10000000 hashes, 10000000 copies)   90.9 MB
map_replace            13.7 ns  (34279145 hashes, 24279122 copies)
map_fetch_random       13.7 ns  (10000000 hashes, 24279122 copies)   84.4 MB
map_fetch_sequential   11.0 ns  (10000000 hashes, 24279122 copies)   84.4 MB
map_fetch_empty        12.0 ns  (10000000 hashes,        0 copies)
map_remove             15.5 ns  (34279145 hashes, 24279122 copies)
map_toggle             49.6 ns  (20000000 hashes, 10000000 copies)
map_iterate             3.0 ns  (24279145 hashes, 54279122 copies)

stresshashfunction map_size=256 stride=1: 14.8ns/insertion
stresshashfunction map_size=256 stride=256: 28.8ns/insertion
stresshashfunction map_size=1024 stride=1: 16.0ns/insertion
stresshashfunction map_size=1024 stride=1024: 15.2ns/insertion

DENSE_HASH_MAP (4 byte objects, 10000000 iterations):
map_grow               41.4 ns  (26777220 hashes, 123886195 copies)  256.0 MB
map_predict/grow       12.1 ns  (10000000 hashes, 73554475 copies)  256.0 MB
map_replace             6.9 ns  (36777220 hashes, 123886195 copies)
map_fetch_random        8.8 ns  (10000000 hashes, 123886195 copies)  256.0 MB
map_fetch_sequential    2.9 ns  (10000000 hashes, 123886195 copies)  256.0 MB
map_fetch_empty         1.3 ns  (       0 hashes,       35 copies)
map_remove              3.3 ns  (36777220 hashes, 133886195 copies)
map_toggle             33.7 ns  (20624999 hashes, 74999995 copies)
map_iterate             3.0 ns  (26777220 hashes, 123886195 copies)

stresshashfunction map_size=256 stride=1: 38.0ns/insertion
stresshashfunction map_size=256 stride=256: 19.2ns/insertion
stresshashfunction map_size=1024 stride=1: 55.6ns/insertion
stresshashfunction map_size=1024 stride=1024: 42.4ns/insertion

RABBIT (LESS SPARSE) UNORDERED_MAP (4 byte objects, 10000000 iterations):
map_grow               29.0 ns  (26783655 hashes, 26783635 copies)  106.5 MB
map_predict/grow        8.7 ns  (10000000 hashes, 10000000 copies)  106.2 MB
map_replace             5.6 ns  (36783655 hashes, 26783635 copies)
map_fetch_random        7.7 ns  (10000000 hashes, 26783635 copies)  107.7 MB
map_fetch_sequential    3.3 ns  (10000000 hashes, 26783635 copies)  104.7 MB
map_fetch_empty         3.0 ns  (10000000 hashes,        0 copies)
map_remove              7.3 ns  (36783655 hashes, 26783635 copies)
map_toggle             33.6 ns  (20000000 hashes, 10000000 copies)
map_iterate             3.5 ns  (26783655 hashes, 56783635 copies)

stresshashfunction map_size=256 stride=1: 7.2ns/insertion
stresshashfunction map_size=256 stride=256: 5.2ns/insertion
stresshashfunction map_size=1024 stride=1: 7.6ns/insertion
stresshashfunction map_size=1024 stride=1024: 7.2ns/insertion

SPARSE_HASH_MAP (8 byte objects, 5000000 iterations):
map_grow              335.2 ns  (11710870 hashes, 26710925 copies)   62.3 MB
map_predict/grow      180.7 ns  ( 5000000 hashes, 20000005 copies)   60.5 MB
map_replace            44.8 ns  (16710870 hashes, 26710925 copies)
map_fetch_random       60.4 ns  ( 5000000 hashes, 26710925 copies)   61.0 MB
map_fetch_sequential   34.4 ns  ( 5000000 hashes, 26710925 copies)   61.1 MB
map_fetch_empty        16.6 ns  (       0 hashes,        1 copies)
map_remove             39.0 ns  (16710870 hashes, 31710925 copies)
map_toggle            230.4 ns  (10199999 hashes, 25799997 copies)
map_iterate             3.4 ns  (11710870 hashes, 26710925 copies)

stresshashfunction map_size=256 stride=1: 389.7ns/insertion
stresshashfunction map_size=256 stride=256: 200.9ns/insertion
stresshashfunction map_size=1024 stride=1: 546.7ns/insertion
stresshashfunction map_size=1024 stride=1024: 451.5ns/insertion

RABBIT SPARSE_UNORDERED_MAP (8 byte objects, 5000000 iterations):
map_grow               55.6 ns  (12844647 hashes, 12844625 copies)   69.6 MB
map_predict/grow       20.6 ns  ( 5000000 hashes,  5000000 copies)   70.2 MB
map_replace            14.8 ns  (17844647 hashes, 12844625 copies)
map_fetch_random       19.6 ns  ( 5000000 hashes, 12844625 copies)   68.4 MB
map_fetch_sequential   13.2 ns  ( 5000000 hashes, 12844625 copies)   69.0 MB
map_fetch_empty        12.6 ns  ( 5000000 hashes,        0 copies)
map_remove             17.4 ns  (17850003 hashes, 12844625 copies)
map_toggle             55.2 ns  (10000000 hashes,  5000000 copies)
map_iterate             4.2 ns  (12844647 hashes, 27844625 copies)

stresshashfunction map_size=256 stride=1: 14.4ns/insertion
stresshashfunction map_size=256 stride=256: 26.4ns/insertion
stresshashfunction map_size=1024 stride=1: 16.0ns/insertion
stresshashfunction map_size=1024 stride=1024: 15.2ns/insertion

DENSE_HASH_MAP (8 byte objects, 5000000 iterations):
map_grow               52.6 ns  (13388611 hashes, 61943147 copies)  190.9 MB
map_predict/grow       10.4 ns  ( 5000000 hashes, 36777259 copies)  192.0 MB
map_replace             3.8 ns  (18388611 hashes, 61943147 copies)
map_fetch_random       19.6 ns  ( 5000000 hashes, 61943147 copies)  192.0 MB
map_fetch_sequential    8.6 ns  ( 5000000 hashes, 61943147 copies)  193.5 MB
map_fetch_empty         4.2 ns  (       0 hashes,       35 copies)
map_remove              9.6 ns  (18388611 hashes, 66943147 copies)
map_toggle             39.0 ns  (10312499 hashes, 37499995 copies)    0.0 MB
map_iterate             3.4 ns  (13388611 hashes, 61943147 copies)

stresshashfunction map_size=256 stride=1: 38.4ns/insertion
stresshashfunction map_size=256 stride=256: 16.8ns/insertion
stresshashfunction map_size=1024 stride=1: 57.6ns/insertion
stresshashfunction map_size=1024 stride=1024: 40.0ns/insertion

RABBIT (LESS SPARSE) UNORDERED_MAP (8 byte objects, 5000000 iterations):
map_grow               36.8 ns  (13393014 hashes, 13392995 copies)   85.4 MB
map_predict/grow       11.4 ns  ( 5000000 hashes,  5000000 copies)   85.1 MB
map_replace             7.0 ns  (18393014 hashes, 13392995 copies)
map_fetch_random       12.4 ns  ( 5000000 hashes, 13392995 copies)   85.1 MB
map_fetch_sequential    4.6 ns  ( 5000000 hashes, 13392995 copies)   85.1 MB
map_fetch_empty         3.8 ns  ( 5000000 hashes,        0 copies)
map_remove              9.2 ns  (18393014 hashes, 13392995 copies)
map_toggle             37.0 ns  (10000000 hashes,  5000000 copies)
map_iterate             4.6 ns  (13393014 hashes, 28392995 copies)

stresshashfunction map_size=256 stride=1: 5.6ns/insertion
stresshashfunction map_size=256 stride=256: 5.6ns/insertion
stresshashfunction map_size=1024 stride=1: 6.4ns/insertion
stresshashfunction map_size=1024 stride=1024: 7.2ns/insertion

SPARSE_HASH_MAP (16 byte objects, 2500000 iterations):
map_grow              395.5 ns  ( 5855426 hashes, 13355478 copies)   50.3 MB
map_predict/grow      233.0 ns  ( 2500000 hashes, 10000005 copies)   49.3 MB
map_replace            41.2 ns  ( 8355426 hashes, 13355478 copies)
map_fetch_random       86.9 ns  ( 2500000 hashes, 13355478 copies)   49.5 MB
map_fetch_sequential   51.2 ns  ( 2500000 hashes, 13355478 copies)   49.4 MB
map_fetch_empty        24.4 ns  (       0 hashes,        1 copies)
map_remove            110.1 ns  ( 8355426 hashes, 15855478 copies)
map_toggle            396.7 ns  ( 5099999 hashes, 12899997 copies)
map_iterate             3.6 ns  ( 5855426 hashes, 13355478 copies)

RABBIT SPARSE_UNORDERED_MAP (16 byte objects, 2500000 iterations):
map_grow               84.5 ns  ( 6368658 hashes,  6368637 copies)   64.4 MB
map_predict/grow       38.0 ns  ( 2500000 hashes,  2500000 copies)   59.9 MB
map_replace            32.8 ns  ( 8868658 hashes,  6368637 copies)
map_fetch_random       48.0 ns  ( 2500000 hashes,  6368637 copies)   64.2 MB
map_fetch_sequential   28.8 ns  ( 2500000 hashes,  6368637 copies)   64.2 MB
map_fetch_empty        26.4 ns  ( 2500000 hashes,        0 copies)
map_remove             33.6 ns  ( 8932561 hashes,  6368637 copies)
map_toggle             87.3 ns  ( 5000000 hashes,  2500000 copies)
map_iterate             5.6 ns  ( 6368658 hashes, 13868637 copies)

DENSE_HASH_MAP (16 byte objects, 2500000 iterations):
map_grow              104.1 ns  ( 6694306 hashes, 30971619 copies)  160.5 MB
map_predict/grow       36.4 ns  ( 2500000 hashes, 18388651 copies)  160.0 MB
map_replace            26.4 ns  ( 9194306 hashes, 30971619 copies)
map_fetch_random       44.0 ns  ( 2500000 hashes, 30971619 copies)  158.8 MB
map_fetch_sequential   23.2 ns  ( 2500000 hashes, 30971619 copies)  160.0 MB
map_fetch_empty        13.2 ns  (       0 hashes,       35 copies)
map_remove             22.8 ns  ( 9194306 hashes, 33471619 copies)
map_toggle             72.4 ns  ( 5156249 hashes, 18749995 copies)
map_iterate             4.8 ns  ( 6694306 hashes, 30971619 copies)

RABBIT (LESS SPARSE) UNORDERED_MAP (16 byte objects, 2500000 iterations):
map_grow               65.2 ns  ( 6696175 hashes,  6696157 copies)   73.2 MB
map_predict/grow       26.0 ns  ( 2500000 hashes,  2500000 copies)   74.6 MB
map_replace            25.2 ns  ( 9196175 hashes,  6696157 copies)
map_fetch_random       38.4 ns  ( 2500000 hashes,  6696157 copies)   74.6 MB
map_fetch_sequential   21.2 ns  ( 2500000 hashes,  6696157 copies)   74.6 MB
map_fetch_empty        17.6 ns  ( 2500000 hashes,        0 copies)
map_remove             25.2 ns  ( 9196175 hashes,  6696157 copies)
map_toggle             68.4 ns  ( 5000000 hashes,  2500000 copies)
map_iterate             6.4 ns  ( 6696175 hashes, 14196157 copies)

