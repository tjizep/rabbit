# rabbit v 1.0 r5

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
-O3;-fexpensive-optimizations;-std=c++11;-Wall
-------------------------------------------------------------------------------

======
Average over 10000000 iterations
Current time (GMT): Fri May 08 11:01:11 2015

*** WARNING ***: sys/resources.h was not found, so all times
                 reported are wall-clock time, not user time

SPARSE_HASH_MAP (4 byte objects, 10000000 iterations):
map_grow              337.9 ns  (23421757 hashes, 43421815 copies)   86.0 MB
map_predict/grow      142.8 ns  (10000000 hashes, 30000005 copies)   84.5 MB
map_replace            24.3 ns  (33421757 hashes, 43421815 copies)
map_fetch_random       63.7 ns  (10000000 hashes, 43421815 copies)   84.5 MB
map_fetch_sequential   34.0 ns  (10000000 hashes, 43421815 copies)   84.3 MB
map_fetch_empty        13.6 ns  (       0 hashes,        1 copies)
map_remove             38.0 ns  (33421757 hashes, 53421815 copies)
map_toggle            182.9 ns  (20399999 hashes, 41599997 copies)
map_iterate             3.1 ns  (23421757 hashes, 43421815 copies)

stresshashfunction map_size=256 stride=1: 318.6ns/insertion
stresshashfunction map_size=256 stride=256: 169.3ns/insertion
stresshashfunction map_size=1024 stride=1: 507.0ns/insertion
stresshashfunction map_size=1024 stride=1024: 388.9ns/insertion

DENSE_HASH_MAP (4 byte objects, 10000000 iterations):
map_grow               34.3 ns  (26777220 hashes, 113886195 copies)  256.0 MB
map_predict/grow       11.1 ns  (10000000 hashes, 63554475 copies)  256.0 MB
map_replace             4.3 ns  (36777220 hashes, 113886195 copies)
map_fetch_random       10.0 ns  (10000000 hashes, 113886195 copies)  256.3 MB
map_fetch_sequential    2.8 ns  (10000000 hashes, 113886195 copies)  255.3 MB
map_fetch_empty         1.3 ns  (       0 hashes,       35 copies)
map_remove              3.3 ns  (36777220 hashes, 123886195 copies)
map_toggle             34.5 ns  (20624999 hashes, 64999995 copies)
map_iterate             3.1 ns  (26777220 hashes, 113886195 copies)

stresshashfunction map_size=256 stride=1: 23.6ns/insertion
stresshashfunction map_size=256 stride=256: 11.8ns/insertion
stresshashfunction map_size=1024 stride=1: 41.2ns/insertion
stresshashfunction map_size=1024 stride=1024: 27.6ns/insertion

RABBIT (LESS SPARSE) UNORDERED_MAP (4 byte objects, 10000000 iterations):
map_grow               29.9 ns  (26783084 hashes, 26784575 copies)   80.3 MB
map_predict/grow       11.1 ns  (10000000 hashes, 10000072 copies)   80.3 MB
map_replace             8.0 ns  (36783084 hashes, 26784575 copies)
map_fetch_random        7.0 ns  (10000000 hashes, 26784575 copies)   80.3 MB
map_fetch_sequential    2.9 ns  (10000000 hashes, 26784575 copies)   80.3 MB
map_fetch_empty         2.3 ns  (10000000 hashes,        0 copies)
map_remove              3.2 ns  (36783084 hashes, 26784575 copies)
map_toggle             13.1 ns  (20000000 hashes, 10000000 copies)
map_iterate             2.2 ns  (26783084 hashes, 46784575 copies)

stresshashfunction map_size=256 stride=1: 7.8ns/insertion
stresshashfunction map_size=256 stride=256: 7.8ns/insertion
stresshashfunction map_size=1024 stride=1: 8.8ns/insertion
stresshashfunction map_size=1024 stride=1024: 8.9ns/insertion

SPARSE_HASH_MAP (8 byte objects, 5000000 iterations):
map_grow              295.6 ns  (11710870 hashes, 21710925 copies)   62.5 MB
map_predict/grow      146.9 ns  ( 5000000 hashes, 15000005 copies)   60.4 MB
map_replace            25.7 ns  (16710870 hashes, 21710925 copies)
map_fetch_random       63.4 ns  ( 5000000 hashes, 21710925 copies)   61.2 MB
map_fetch_sequential   34.9 ns  ( 5000000 hashes, 21710925 copies)   61.1 MB
map_fetch_empty        14.5 ns  (       0 hashes,        1 copies)
map_remove             43.6 ns  (16710870 hashes, 26710925 copies)
map_toggle            197.8 ns  (10199999 hashes, 20799997 copies)
map_iterate             3.8 ns  (11710870 hashes, 21710925 copies)

stresshashfunction map_size=256 stride=1: 367.2ns/insertion
stresshashfunction map_size=256 stride=256: 170.3ns/insertion
stresshashfunction map_size=1024 stride=1: 511.1ns/insertion
stresshashfunction map_size=1024 stride=1024: 394.0ns/insertion

DENSE_HASH_MAP (8 byte objects, 5000000 iterations):
map_grow               56.9 ns  (13388611 hashes, 56943147 copies)  192.0 MB
map_predict/grow       13.4 ns  ( 5000000 hashes, 31777259 copies)  192.0 MB
map_replace             5.6 ns  (18388611 hashes, 56943147 copies)
map_fetch_random       15.7 ns  ( 5000000 hashes, 56943147 copies)  192.0 MB
map_fetch_sequential    5.0 ns  ( 5000000 hashes, 56943147 copies)  192.0 MB
map_fetch_empty         1.9 ns  (       0 hashes,       35 copies)
map_remove              7.2 ns  (18388611 hashes, 61943147 copies)
map_toggle             32.7 ns  (10312499 hashes, 32499995 copies)
map_iterate             3.5 ns  (13388611 hashes, 56943147 copies)

stresshashfunction map_size=256 stride=1: 23.6ns/insertion
stresshashfunction map_size=256 stride=256: 11.8ns/insertion
stresshashfunction map_size=1024 stride=1: 40.4ns/insertion
stresshashfunction map_size=1024 stride=1024: 25.6ns/insertion

RABBIT (LESS SPARSE) UNORDERED_MAP (8 byte objects, 5000000 iterations):
map_grow               36.2 ns  (13393484 hashes, 13394904 copies)   58.9 MB
map_predict/grow       13.6 ns  ( 5000000 hashes,  5000072 copies)   59.2 MB
map_replace             9.2 ns  (18393484 hashes, 13394904 copies)
map_fetch_random       12.6 ns  ( 5000000 hashes, 13394904 copies)   59.2 MB
map_fetch_sequential    4.9 ns  ( 5000000 hashes, 13394904 copies)   59.2 MB
map_fetch_empty         4.1 ns  ( 5000000 hashes,        0 copies)
map_remove              7.3 ns  (18393484 hashes, 13394904 copies)
map_toggle             16.0 ns  (10000000 hashes,  5000000 copies)
map_iterate             3.1 ns  (13393484 hashes, 23394904 copies)

stresshashfunction map_size=256 stride=1: 7.8ns/insertion
stresshashfunction map_size=256 stride=256: 7.7ns/insertion
stresshashfunction map_size=1024 stride=1: 8.5ns/insertion
stresshashfunction map_size=1024 stride=1024: 8.6ns/insertion

SPARSE_HASH_MAP (16 byte objects, 2500000 iterations):
map_grow              339.7 ns  ( 5855426 hashes, 10855478 copies)   50.2 MB
map_predict/grow      174.4 ns  ( 2500000 hashes,  7500005 copies)   49.0 MB
map_replace            31.0 ns  ( 8355426 hashes, 10855478 copies)
map_fetch_random       78.8 ns  ( 2500000 hashes, 10855478 copies)   49.9 MB
map_fetch_sequential   44.5 ns  ( 2500000 hashes, 10855478 copies)   49.6 MB
map_fetch_empty        15.0 ns  (       0 hashes,        1 copies)
map_remove             50.5 ns  ( 8355426 hashes, 13355478 copies)
map_toggle            207.6 ns  ( 5099999 hashes, 10399997 copies)    0.0 MB
map_iterate             4.0 ns  ( 5855426 hashes, 10855478 copies)

DENSE_HASH_MAP (16 byte objects, 2500000 iterations):
map_grow               88.7 ns  ( 6694306 hashes, 28471619 copies)  161.2 MB
map_predict/grow       20.6 ns  ( 2500000 hashes, 15888651 copies)  160.0 MB
map_replace            10.6 ns  ( 9194306 hashes, 28471619 copies)
map_fetch_random       24.1 ns  ( 2500000 hashes, 28471619 copies)  160.0 MB
map_fetch_sequential    9.9 ns  ( 2500000 hashes, 28471619 copies)  160.0 MB
map_fetch_empty         1.6 ns  (       0 hashes,       35 copies)
map_remove             11.8 ns  ( 9194306 hashes, 30971619 copies)
map_toggle             39.4 ns  ( 5156249 hashes, 16249995 copies)
map_iterate             4.8 ns  ( 6694306 hashes, 28471619 copies)

RABBIT (LESS SPARSE) UNORDERED_MAP (16 byte objects, 2500000 iterations):
map_grow               49.9 ns  ( 6698115 hashes,  6699464 copies)   47.5 MB
map_predict/grow       18.9 ns  ( 2500000 hashes,  2500072 copies)   48.8 MB
map_replace            13.7 ns  ( 9198115 hashes,  6699464 copies)
map_fetch_random       25.1 ns  ( 2500000 hashes,  6699464 copies)   48.8 MB
map_fetch_sequential   10.2 ns  ( 2500000 hashes,  6699464 copies)   48.8 MB
map_fetch_empty         7.8 ns  ( 2500000 hashes,        0 copies)
map_remove             10.2 ns  ( 9198115 hashes,  6699464 copies)
map_toggle             23.0 ns  ( 5000000 hashes,  2500000 copies)
map_iterate             4.2 ns  ( 6698115 hashes, 11699464 copies)

SPARSE_HASH_MAP (256 byte objects, 312500 iterations):
map_grow             1606.8 ns  (  731912 hashes,  1356955 copies)   86.4 MB
map_predict/grow      936.2 ns  (  312500 hashes,   937505 copies)   93.0 MB
map_replace           163.3 ns  ( 1044412 hashes,  1356955 copies)
map_fetch_random      199.9 ns  (  312500 hashes,  1356955 copies)   86.2 MB
map_fetch_sequential  172.5 ns  (  312500 hashes,  1356955 copies)   85.4 MB
map_fetch_empty        29.4 ns  (       0 hashes,        1 copies)
map_remove            211.2 ns  ( 1044412 hashes,  1669455 copies)
map_toggle            632.2 ns  (  637499 hashes,  1299997 copies)    0.6 MB
map_iterate            12.9 ns  (  731912 hashes,  1356955 copies)

DENSE_HASH_MAP (256 byte objects, 312500 iterations):
map_grow              929.8 ns  (  836787 hashes,  3559015 copies)  261.0 MB
map_predict/grow      210.1 ns  (  312500 hashes,  1986119 copies)  260.0 MB
map_replace           139.7 ns  ( 1149287 hashes,  3559015 copies)
map_fetch_random      142.8 ns  (  312500 hashes,  3559015 copies)  261.0 MB
map_fetch_sequential  136.6 ns  (  312500 hashes,  3559015 copies)  260.0 MB
map_fetch_empty        20.7 ns  (       0 hashes,       35 copies)
map_remove            177.3 ns  ( 1149287 hashes,  3871515 copies)
map_toggle            272.4 ns  (  644531 hashes,  2031275 copies)
map_iterate            31.3 ns  (  836787 hashes,  3559015 copies)

RABBIT (LESS SPARSE) UNORDERED_MAP (256 byte objects, 312500 iterations):
map_grow              406.9 ns  (  837047 hashes,   838183 copies)   88.1 MB
map_predict/grow      158.4 ns  (  312500 hashes,   312572 copies)   88.9 MB
map_replace           150.2 ns  ( 1149547 hashes,   838183 copies)
map_fetch_random      141.9 ns  (  312500 hashes,   838183 copies)   88.9 MB
map_fetch_sequential  136.9 ns  (  312500 hashes,   838183 copies)   89.6 MB
map_fetch_empty        56.5 ns  (  312500 hashes,        0 copies)
map_remove            139.7 ns  ( 1149547 hashes,   838183 copies)
map_toggle            155.1 ns  (  625000 hashes,   312500 copies)    0.0 MB
map_iterate            41.1 ns  (  837047 hashes,  1463183 copies)