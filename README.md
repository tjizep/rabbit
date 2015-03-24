# rabbit
stl compatible hashtable (rabbit::unordered_map, rabbit::unordered_set)

It also seems to be performing well

Some timing results with google dense hash compiled with MSVC 2012
------------------------------------------------------------------

(4 byte objects, 10000000 iterations):

| TEST TYPE            | RABBIT UNORDERED_MAP                           | DENSE_HASH_MAP                                      |
| --------------------- | ---------------------------------------------: | --------------------------------------------------: |
| map_grow              | 156.3 ns  (29464764 hashes, 19732378 copies)   | *151.6 ns  (26777220 hashes, 123886195 copies)      |
| map_predict/grow      | 156.3 ns  (29464764 hashes, 19732378 copies)   | *114.1 ns  (10000000 hashes, 73554475 copies)       |
| map_replace           | *89.1 ns  (39464764 hashes, 19732378 copies)   | 110.9 ns  (36777220 hashes, 123886195 copies)       |
| map_fetch_random      | *89.1 ns  (39464764 hashes, 19732378 copies)   | 106.2 ns  (36777220 hashes, 123886195 copies)       |
| map_fetch_sequential  | *87.5 ns  (39464764 hashes, 19732378 copies)   | 104.7 ns  (36777220 hashes, 123886195 copies)       |
| map_fetch_empty       | 1.6 ns  (       0 hashes,        0 copies)     | *1.5 ns  (       0 hashes,       35 copies)         |
| map_remove            | *103.2 ns  (39464764 hashes, 29732378 copies)  | 112.5 ns  (36777220 hashes, 133886195 copies)       |
| map_toggle            | *53.2 ns  (20000000 hashes, 20000000 copies)   | 57.8 ns  (20624929 hashes, 74997195 copies)         |
| map_iterate           | *3.2 ns  (29464764 hashes, 19732378 copies)    | 7.8 ns  (26777220 hashes, 123886195 copies)         |

Using:
include rabbit\unordered_map

