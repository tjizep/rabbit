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



Some timing results with google dense/sparse hash compiled with GCC 4.9.1 (TDM)
-------------------------------------------------------------------------------

======
Average over 10000000 iterations
Current time (GMT): Fri May 01 17:01:39 2015

*** WARNING ***: sys/resources.h was not found, so all times
                 reported are wall-clock time, not user time

SPARSE_HASH_MAP (4 byte objects, 10000000 iterations):
map_grow              260.5 ns  (23421757 hashes, 43421815 copies)   86.0 MB
map_predict/grow      120.1 ns  (10000000 hashes, 30000005 copies)   84.5 MB
map_replace            22.8 ns  (33421757 hashes, 43421815 copies)
map_fetch_random       52.0 ns  (10000000 hashes, 43421815 copies)   84.5 MB
map_fetch_sequential   32.0 ns  (10000000 hashes, 43421815 copies)   84.3 MB
map_fetch_empty        13.9 ns  (       0 hashes,        1 copies)
map_remove             38.0 ns  (33421757 hashes, 53421815 copies)
map_toggle            178.0 ns  (20399999 hashes, 41599997 copies)    0.0 MB
map_iterate             3.1 ns  (23421757 hashes, 43421815 copies)

stresshashfunction map_size=256 stride=1: 321.2ns/insertion
stresshashfunction map_size=256 stride=256: 173.0ns/insertion
stresshashfunction map_size=1024 stride=1: 508.7ns/insertion
stresshashfunction map_size=1024 stride=1024: 394.6ns/insertion

DENSE_HASH_MAP (4 byte objects, 10000000 iterations):
map_grow               34.1 ns  (26777220 hashes, 113886195 copies)  256.0 MB
map_predict/grow       10.6 ns  (10000000 hashes, 63554475 copies)  256.0 MB
map_replace             5.2 ns  (36777220 hashes, 113886195 copies)
map_fetch_random       10.2 ns  (10000000 hashes, 113886195 copies)  256.3 MB
map_fetch_sequential    2.8 ns  (10000000 hashes, 113886195 copies)  255.3 MB
map_fetch_empty         1.3 ns  (       0 hashes,       35 copies)
map_remove              3.4 ns  (36777220 hashes, 123886195 copies)
map_toggle             34.3 ns  (20624999 hashes, 64999995 copies)
map_iterate             3.3 ns  (26777220 hashes, 113886195 copies)

stresshashfunction map_size=256 stride=1: 23.5ns/insertion
stresshashfunction map_size=256 stride=256: 12.0ns/insertion
stresshashfunction map_size=1024 stride=1: 42.1ns/insertion
stresshashfunction map_size=1024 stride=1024: 26.8ns/insertion

RABBIT (LESS SPARSE) UNORDERED_MAP (4 byte objects, 10000000 iterations):
map_grow               32.0 ns  (26782870 hashes, 26784697 copies)   81.1 MB
map_predict/grow       11.8 ns  (10000000 hashes, 10000088 copies)   80.3 MB
map_replace             8.5 ns  (36782870 hashes, 26784697 copies)
map_fetch_random        7.2 ns  (10000000 hashes, 26784697 copies)   79.8 MB
map_fetch_sequential    2.9 ns  (10000000 hashes, 26784697 copies)   81.1 MB
map_fetch_empty         2.3 ns  (10000000 hashes,        0 copies)
map_remove              7.4 ns  (36782870 hashes, 26784697 copies)
map_toggle             17.2 ns  (20000000 hashes, 10000000 copies)
map_iterate             2.8 ns  (26782870 hashes, 46784697 copies)

stresshashfunction map_size=256 stride=1: 7.9ns/insertion
stresshashfunction map_size=256 stride=256: 7.9ns/insertion
stresshashfunction map_size=1024 stride=1: 9.0ns/insertion
stresshashfunction map_size=1024 stride=1024: 8.9ns/insertion

SPARSE_HASH_MAP (8 byte objects, 5000000 iterations):
map_grow              294.2 ns  (11710870 hashes, 21710925 copies)   62.5 MB
map_predict/grow      147.4 ns  ( 5000000 hashes, 15000005 copies)   60.4 MB
map_replace            25.8 ns  (16710870 hashes, 21710925 copies)
map_fetch_random       62.0 ns  ( 5000000 hashes, 21710925 copies)   61.2 MB
map_fetch_sequential   35.4 ns  ( 5000000 hashes, 21710925 copies)   61.1 MB
map_fetch_empty        14.6 ns  (       0 hashes,        1 copies)
map_remove             44.5 ns  (16710870 hashes, 26710925 copies)
map_toggle            198.2 ns  (10199999 hashes, 20799997 copies)
map_iterate             3.8 ns  (11710870 hashes, 21710925 copies)

stresshashfunction map_size=256 stride=1: 354.2ns/insertion
stresshashfunction map_size=256 stride=256: 175.9ns/insertion
stresshashfunction map_size=1024 stride=1: 495.0ns/insertion
stresshashfunction map_size=1024 stride=1024: 391.7ns/insertion

DENSE_HASH_MAP (8 byte objects, 5000000 iterations):
map_grow               55.7 ns  (13388611 hashes, 56943147 copies)  191.9 MB
map_predict/grow       12.9 ns  ( 5000000 hashes, 31777259 copies)  192.0 MB
map_replace             5.9 ns  (18388611 hashes, 56943147 copies)
map_fetch_random       15.7 ns  ( 5000000 hashes, 56943147 copies)  192.0 MB
map_fetch_sequential    5.1 ns  ( 5000000 hashes, 56943147 copies)  192.0 MB
map_fetch_empty         1.6 ns  (       0 hashes,       35 copies)
map_remove              6.8 ns  (18388611 hashes, 61943147 copies)
map_toggle             31.6 ns  (10312499 hashes, 32499995 copies)
map_iterate             3.7 ns  (13388611 hashes, 56943147 copies)

stresshashfunction map_size=256 stride=1: 23.6ns/insertion
stresshashfunction map_size=256 stride=256: 11.8ns/insertion
stresshashfunction map_size=1024 stride=1: 41.6ns/insertion
stresshashfunction map_size=1024 stride=1024: 25.5ns/insertion

RABBIT (LESS SPARSE) UNORDERED_MAP (8 byte objects, 5000000 iterations):
map_grow               36.9 ns  (13393335 hashes, 13395075 copies)   58.9 MB
map_predict/grow       14.1 ns  ( 5000000 hashes,  5000088 copies)   59.2 MB
map_replace             9.3 ns  (18393335 hashes, 13395075 copies)
map_fetch_random       12.1 ns  ( 5000000 hashes, 13395075 copies)   58.3 MB
map_fetch_sequential    5.0 ns  ( 5000000 hashes, 13395075 copies)   59.3 MB
map_fetch_empty         3.9 ns  ( 5000000 hashes,        0 copies)
map_remove              8.4 ns  (18393335 hashes, 13395075 copies)
map_toggle             21.0 ns  (10000000 hashes,  5000000 copies)    0.0 MB
map_iterate             3.2 ns  (13393335 hashes, 23395075 copies)

stresshashfunction map_size=256 stride=1: 8.1ns/insertion
stresshashfunction map_size=256 stride=256: 8.1ns/insertion
stresshashfunction map_size=1024 stride=1: 9.0ns/insertion
stresshashfunction map_size=1024 stride=1024: 9.0ns/insertion

SPARSE_HASH_MAP (16 byte objects, 2500000 iterations):
map_grow              344.5 ns  ( 5855426 hashes, 10855478 copies)   50.3 MB
map_predict/grow      177.2 ns  ( 2500000 hashes,  7500005 copies)   49.0 MB
map_replace            32.6 ns  ( 8355426 hashes, 10855478 copies)
map_fetch_random       78.3 ns  ( 2500000 hashes, 10855478 copies)   49.8 MB
map_fetch_sequential   43.7 ns  ( 2500000 hashes, 10855478 copies)   49.7 MB
map_fetch_empty        15.1 ns  (       0 hashes,        1 copies)
map_remove             51.9 ns  ( 8355426 hashes, 13355478 copies)
map_toggle            216.8 ns  ( 5099999 hashes, 10399997 copies)
map_iterate             4.0 ns  ( 5855426 hashes, 10855478 copies)

DENSE_HASH_MAP (16 byte objects, 2500000 iterations):
map_grow               98.9 ns  ( 6694306 hashes, 28471619 copies)  160.6 MB
map_predict/grow       22.4 ns  ( 2500000 hashes, 15888651 copies)  160.0 MB
map_replace            15.0 ns  ( 9194306 hashes, 28471619 copies)
map_fetch_random       30.2 ns  ( 2500000 hashes, 28471619 copies)  160.0 MB
map_fetch_sequential   14.2 ns  ( 2500000 hashes, 28471619 copies)  160.0 MB
map_fetch_empty         2.0 ns  (       0 hashes,       35 copies)
map_remove             14.6 ns  ( 9194306 hashes, 30971619 copies)
map_toggle             48.5 ns  ( 5156249 hashes, 16249995 copies)
map_iterate             4.9 ns  ( 6694306 hashes, 28471619 copies)

RABBIT (LESS SPARSE) UNORDERED_MAP (16 byte objects, 2500000 iterations):
map_grow               57.3 ns  ( 6697953 hashes,  6699606 copies)   48.7 MB
map_predict/grow       19.9 ns  ( 2500000 hashes,  2500088 copies)   48.8 MB
map_replace            16.7 ns  ( 9197953 hashes,  6699606 copies)
map_fetch_random       26.5 ns  ( 2500000 hashes,  6699606 copies)   48.8 MB
map_fetch_sequential   12.1 ns  ( 2500000 hashes,  6699606 copies)   48.8 MB
map_fetch_empty        10.1 ns  ( 2500000 hashes,        0 copies)
map_remove             13.9 ns  ( 9197953 hashes,  6699606 copies)
map_toggle             29.8 ns  ( 5000000 hashes,  2500000 copies)
map_iterate             4.3 ns  ( 6697953 hashes, 11699606 copies)

SPARSE_HASH_MAP (256 byte objects, 312500 iterations):
map_grow             1755.2 ns  (  731912 hashes,  1356955 copies)   85.8 MB
map_predict/grow     1000.3 ns  (  312500 hashes,   937505 copies)   92.7 MB
map_replace           236.0 ns  ( 1044412 hashes,  1356955 copies)
map_fetch_random      254.6 ns  (  312500 hashes,  1356955 copies)   86.2 MB
map_fetch_sequential  245.0 ns  (  312500 hashes,  1356955 copies)   84.6 MB
map_fetch_empty        29.7 ns  (       0 hashes,        1 copies)
map_remove            289.1 ns  ( 1044412 hashes,  1669455 copies)
map_toggle            809.1 ns  (  637499 hashes,  1299997 copies)    0.6 MB
map_iterate            12.9 ns  (  731912 hashes,  1356955 copies)

DENSE_HASH_MAP (256 byte objects, 312500 iterations):
map_grow             1126.6 ns  (  836787 hashes,  3559015 copies)  261.0 MB
map_predict/grow      279.5 ns  (  312500 hashes,  1986119 copies)  260.0 MB
map_replace           214.8 ns  ( 1149287 hashes,  3559015 copies)
map_fetch_random      222.5 ns  (  312500 hashes,  3559015 copies)  261.0 MB
map_fetch_sequential  216.9 ns  (  312500 hashes,  3559015 copies)  260.0 MB
map_fetch_empty        21.3 ns  (       0 hashes,       35 copies)
map_remove            252.9 ns  ( 1149287 hashes,  3871515 copies)
map_toggle            428.4 ns  (  644531 hashes,  2031275 copies)
map_iterate            30.9 ns  (  836787 hashes,  3559015 copies)

RABBIT (LESS SPARSE) UNORDERED_MAP (256 byte objects, 312500 iterations):
map_grow              613.9 ns  (  837048 hashes,   838440 copies)   88.1 MB
map_predict/grow      233.9 ns  (  312500 hashes,   312588 copies)   88.9 MB
map_replace           221.3 ns  ( 1149548 hashes,   838440 copies)
map_fetch_random      220.3 ns  (  312500 hashes,   838440 copies)   88.9 MB
map_fetch_sequential  213.5 ns  (  312500 hashes,   838440 copies)   88.9 MB
map_fetch_empty       136.5 ns  (  312500 hashes,        0 copies)
map_remove            217.5 ns  ( 1149548 hashes,   838440 copies)
map_toggle            305.5 ns  (  625000 hashes,   312500 copies)
map_iterate            40.9 ns  (  837048 hashes,  1463440 copies)
