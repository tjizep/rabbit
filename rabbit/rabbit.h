#ifndef _RABBIT_H_CEP_20150303_
#define  _RABBIT_H_CEP_20150303_
/**
The MIT License (MIT)
Copyright (c) 2015 Christiaan Pretorius
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
**/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <assert.h>
#include <memory>
#include <string>
#include <random>
#include <limits>
#include <iomanip>
/// the rab-bit hash
/// probably the worlds simplest working hashtable - only kiddingk
/// it uses linear probing for the first level of fallback and then a overflow area or secondary hash

#ifdef _MSC_VER
#define RABBIT_NOINLINE_PRE _declspec(noinline)
#define RABBIT_NOINLINE_
#else
#define RABBIT_NOINLINE_PRE
#define RABBIT_NOINLINE_ __attribute__((noinline))
#endif
namespace rabbit {

	/// a very basic version of std::pair which keeps references only
	struct end_iterator {
	};
	template
		<	class _Ty1
		, class _Ty2
		>
		struct ref_pair {
		// store references to a pair of values

		typedef ref_pair<_Ty1, _Ty2> _Myt;
		typedef _Ty1 first_type;
		typedef _Ty2 second_type;

		// construct from specified non const values
		ref_pair(_Ty1& _Val1, _Ty2& _Val2)
			: first(_Val1)
			, second(_Val2)
		{
		}
		/// rely on the compiler default to do this
		//ref_pair(_Myt& _Right)
		//:	first(_Right.first)
		//,	second(_Right.second)
		//{
		//}
		_Myt& operator=(const _Myt& _Right) {
			first = _Right.first;
			second = _Right.second;
			return (*this);
		}

		operator std::pair<_Ty1, _Ty2>() {
			return std::make_pair(first, second);
		}

		operator const std::pair<_Ty1, _Ty2>() const {
			return std::make_pair(first, second);
		}

		_Ty1& first;	// the first stored value
		_Ty2& second;	// the second stored value
	};

	template <class _Config>
	struct _ModMapper {
		typedef _Config config_type;
		typedef typename config_type::size_type size_type;
		size_type extent;
		size_type random_val;
		double backoff;
		_Config config;
		_ModMapper() {
		}
		_ModMapper(size_type new_extent, const _Config& config) {
			this->backoff = get_max_backoff();
			this->config = config;

			this->extent = new_extent;
			srand((unsigned int)time(NULL));
			this->random_val = rand();
		}
		inline size_type nearest_larger(size_type any) {
			return any;
		}
		inline size_type randomize(size_type other) const {
			return (other ^ random_val) % this->extent;
		}
		inline size_type operator()(size_type h) const {

			return h % this->extent;
		}
		double get_min_backoff() const {
			return 2;
		}
		double get_max_backoff() const {
			return 8;
		}
		double resize_factor() const {
			return 1.3;
		}

		double recalc_growth_factor(size_type elements) {
			return 1.80;
		}
		inline size_type next_size() {
			double r = recalc_growth_factor(this->extent) * this->extent;
			assert(r > (double)extent);
			return (size_type)r;
		}
	};

	template <class _Config>
	struct _BinMapper {
		typedef typename _Config::size_type size_type;
		typedef _Config config_type;
		size_type extent;
		size_type extent1;
		size_type extent2;
		size_type primary_bits;
		size_type random_val;
		unsigned long long gate_bits;
		_Config config;
		_BinMapper() {
		}
		_BinMapper(size_type new_extent, const _Config& config) {
			this->config = config;
			this->extent = ((size_type)1) << this->config.log2(new_extent);
			this->extent1 = this->extent - 1;
			this->extent2 = this->config.log2(new_extent);
			this->primary_bits = extent2;
			//std::minstd_rand rd;
			//std::mt19937 gen(rd());
			//std::uniform_int_distribution<size_type> dis(1ll<<4, std::numeric_limits<size_type>::max());
			this->random_val = 0; //(size_type)dis(gen);
			if (new_extent < (1ll << 32ll)) {
				this->gate_bits = (1ll << 32ll) - 1ll;
			}
			else {
				this->gate_bits = (1ll << 62ll) - 1ll;
			}
		}
		inline size_type nearest_larger(size_type any) {
			size_type l2 = this->config.log2(any);
			return (size_type)(2ll << (l2 + 1ll));
		}
		inline size_type randomize(size_type other) const {
			size_type r = other >> this->primary_bits;
			return other + (r*r); //(other ^ random_val) & this->extent1;
		}
		inline size_type operator()(size_type h_n) const {
			size_type h = h_n & this->gate_bits;
			//h += (h>>this->primary_bits);
			return  h & this->extent1; //

		}
		double resize_factor() const {
			return 2;
		}
		double recalc_growth_factor(size_type elements) {
			return 2;
		}

		inline size_type next_size() {

			double r = recalc_growth_factor(this->extent) * this->extent;
			assert(r > (double)extent);
			return (size_type)r;
		}
	};
	template<typename _Ht>
	struct rabbit_hash {
		size_t operator()(const _Ht& k) const {
			return (size_t)std::hash<_Ht>()(k); ///
		};
	};
	template<>
	struct rabbit_hash<long> {
		unsigned long operator()(const long& k) const {
			return (unsigned long)k;
		};
	};
	template<>
	struct rabbit_hash<unsigned long> {
		inline unsigned long operator()(const unsigned long& k) const {
			return k;
		};
	};
	template<>
	struct rabbit_hash<unsigned int> {
		inline unsigned int operator()(const unsigned int& k) const {
			return k;
		};
	};
	template<>
	struct rabbit_hash<int> {
		inline unsigned int operator()(const int& k) const {
			return k;
		};
	};
	template<>
	struct rabbit_hash<unsigned long long> {
		inline unsigned long long operator()(const unsigned long long& k) const {
			return k;
		};
	};
	template<>
	struct rabbit_hash<long long> {
		inline unsigned long long operator()(const long long& k) const {
			return (unsigned  long)k;
		};
	};

	class basic_config {
	public:
		typedef unsigned long long int _Bt; /// exists ebucket type - not using vector<bool> - interface does not support bit bucketing
											/// if even more speed is desired but you'r willing to live with a 4 billion key limit then
											//typedef unsigned long size_type;
		typedef std::size_t size_type;

		size_type log2(size_type n) {
			size_type r = 0;
			while (n >>= 1)
			{
				r++;
			}
			return r;
		}
		_Bt CHAR_BITS;
		_Bt BITS_SIZE;
		_Bt BITS_SIZE1;
		_Bt ALL_BITS_SET;
		/// maximum probes per access
		size_type PROBES; /// a value of 32 gives a little more speed but much larger table size(> twice the size in some cases)
		size_type RAND_PROBES;
		size_type BITS_LOG2_SIZE;
		/// this distributes the h values which are powers of 2 a little to avoid primary clustering when there is no
		///	hash randomizer available

		size_type MIN_EXTENT;
		size_type MAX_OVERFLOW_FACTOR;

		basic_config(const basic_config& right) {
			*this = right;
		}

		basic_config& operator=(const basic_config& right) {
			CHAR_BITS = right.CHAR_BITS;
			BITS_SIZE = right.BITS_SIZE;
			BITS_SIZE1 = right.BITS_SIZE1;
			BITS_LOG2_SIZE = right.BITS_LOG2_SIZE;
			ALL_BITS_SET = right.ALL_BITS_SET;
			PROBES = right.PROBES; /// a value of 32 gives a little more speed but much larger table size(> twice the size in some cases)
			RAND_PROBES = right.RAND_PROBES;
			MIN_EXTENT = right.MIN_EXTENT;
			MAX_OVERFLOW_FACTOR = right.MAX_OVERFLOW_FACTOR;
			return *this;
		}

		basic_config() {
			CHAR_BITS = 8;
			BITS_SIZE = (sizeof(_Bt) * CHAR_BITS);
			BITS_SIZE1 = BITS_SIZE - 1;
			BITS_LOG2_SIZE = (size_type)log2((size_type)BITS_SIZE);
			ALL_BITS_SET = ~(_Bt)0;
			PROBES = 16;
			RAND_PROBES = 8;
			MIN_EXTENT = 4; /// start size of the hash table
			MAX_OVERFLOW_FACTOR = 8 * 32768; //BITS_SIZE*8/sizeof(_Bt);

		}
	};
	template<class _InMapper>
	struct basic_traits {
		typedef typename _InMapper::config_type rabbit_config;
		typedef typename rabbit_config::_Bt _Bt;
		typedef typename rabbit_config::size_type size_type;
		typedef ptrdiff_t difference_type;
		typedef _InMapper _Mapper;
	};
	typedef basic_traits<_BinMapper<basic_config> > default_traits;
	typedef basic_traits<_ModMapper<basic_config> > sparse_traits;


	template
		<	class _K
		, class _V
		, class _H = rabbit_hash<_K>
		, class _E = std::equal_to<_K>
		, class _Allocator = std::allocator<_K>
		, class _Traits = default_traits
		>
		class basic_unordered_map {
		public:
			typedef _K key_type;

			typedef _V mapped_type;

			typedef ref_pair<_K, _V> _ElPair;
			typedef ref_pair<const _K, const _V> _ConstElPair;
			typedef _ElPair value_type;
			typedef _ConstElPair const_value_type;
			typedef typename _Traits::_Bt _Bt; /// exists ebucket type - not using vector<bool> - interface does not support bit bucketing
			typedef typename _Traits::size_type size_type;
			typedef typename _Traits::rabbit_config rabbit_config;
			typedef typename _Traits::_Mapper _Mapper;
			typedef typename _Traits::difference_type difference_type;

			typedef _Allocator allocator_type;
			typedef _ElPair* pointer;
			typedef const _ElPair* const_pointer;
			typedef _ElPair& reference;
			typedef const _ElPair& const_reference;
			// typedef typename _Base::reverse_iterator reverse_iterator;
			// typedef typename _Base::const_reverse_iterator
			// const_reverse_iterator;

			typedef _E key_equal;
			typedef _E key_compare;
			typedef _H hasher;
			typedef _V* _RV;

			static const size_type end_pos = std::numeric_limits<size_type>::max();
		protected:
			struct overflow_stats {
				size_type start_elements;
				size_type end_elements;
				overflow_stats() : start_elements(0), end_elements(0) {}
			};

			struct _KeySegment {

			public:
				_Bt overflows;
				_Bt exists;
				//_Bt overflowed;
			private:
				//_K keys[sizeof(_Bt)*8];
			private:
				void set_bit(_Bt& w, _Bt index, bool f) {

#ifdef _MSC_VER
#pragma warning(disable:4804)
#endif
					_Bt m = (_Bt)1ul << index;// the bit mask
					w ^= (-f ^ w) & m;
					///w = (w & ~m) | (-f & m);
				}

			public:

				inline bool all_exists() const {
					return (exists == ~(_Bt)0);
				}

				inline bool none_exists() const {
					return (exists == (_Bt)0);
				}

				inline bool is_exists(_Bt bit) const {
					return ((exists >> bit) & (_Bt)1ul);
				}

				inline bool is_overflows(_Bt bit) const {
					return ((overflows >> bit) & (_Bt)1ul);
				}

				inline void set_exists(_Bt index, bool f) {
					set_bit(exists, index, f);
				}

				inline void toggle_exists(_Bt index) {
					exists ^= ((_Bt)1 << index);
				}

				void set_overflows(_Bt index, bool f) {
					set_bit(overflows, index, f);
				}

				void clear() {
					exists = 0;
					overflows = 0;
				}

				_KeySegment() {
					exists = 0;
					overflows = 0;
				}
			};
			//typedef _PairSegment _Segment;
			typedef _KeySegment _Segment;
			/// the vector that will contain the segmented mapping pairs and flags

			typedef std::vector<_Segment, _Allocator> _Segments;
			typedef std::vector<_K, _Allocator> _Keys;
			typedef std::vector<_V, _Allocator> _Values;

			struct hash_kernel {

				/// settings configuration
				rabbit_config config;
				size_type elements;
				size_type initial_probes;
				size_type probes;
				size_type rand_probes; /// used when there might be an attack
				size_type last_modified;
				size_type random_val;
				/// the existence bit set is a factor of BITS_SIZE+1 less than the extent
				_Segment* clusters;///a.k.a. pages
				_Keys keys;
				///_Values values;
				_V* values;

				size_type overflow;
				size_type overflow_elements;
				overflow_stats stats;
				_Mapper key_mapper;
				_H hf;
				_E eq_f;
				float mf;
				size_type buckets;
				size_type removed;
				bool keys_overflowed;
				_Allocator allocator;
				_K empty_key;
				bool sparse;
				size_type logarithmic;

				typename _Allocator::template rebind<_Segment>::other get_segment_allocator() {
					return typename _Allocator::template rebind<_Segment>::other(allocator);
				}

				typename _Allocator::template rebind<_V>::other get_value_allocator() {
					return typename _Allocator::template rebind<_V>::other(allocator);
				}

				/// the minimum load factor
				float load_factor() const {
					return (float)((double)elements / (double)bucket_count());
				}

				/// there are a variable ammount of buckets there are at most this much
				///
				size_type bucket_count() const {

					return get_data_size();
				}
				/// the size of a bucket can be calculated based on the
				/// hash value of its first occupant
				/// mainly to satisfy stl conventions
				size_type bucket_size(size_type n) const {
					size_type pos = n;
					if (!overflows_(pos)) {
						if (exists_(pos))
							return 1;
						else return 0;
					}
					size_type m = pos + probes;
					size_type r = 0;
					for (; pos < m; ++pos) {
						if (!exists_(pos)) {
						}
						else if (map_key(get_segment_key(pos)) == n) {
							++r;
						}
					}
					size_type e = end();
					for (pos = get_o_start(); pos < e; ) {
						if (!exists_(pos)) {
						}
						else if (map_key(get_segment_key(pos)) == n) {
							++r;
						}
						++pos;
					}
					return r;
				}

				float max_load_factor() const {
					return mf;
				}

				void max_load_factor(float z) {
					mf = z;
				}

				/// total data size, never less than than size()
				size_type get_data_size() const {
					return get_extent() + initial_probes + overflow;
				}

				/// the overflow start
				size_type get_o_start() const {
					return get_extent() + initial_probes;
				}

				size_type get_segment_number(size_type pos) const {
					return (pos >> config.BITS_LOG2_SIZE);
				}

				_Bt get_segment_index(size_type pos) const {
					return (_Bt)(pos & (config.BITS_SIZE1));
				}

				_Segment &get_segment(size_type pos) {
					return clusters[pos >> config.BITS_LOG2_SIZE];
				}

				const _Segment &get_segment(size_type pos) const {
					return clusters[get_segment_number(pos)];
				}

				const _ElPair get_segment_pair(size_type pos) const {
					return std::make_pair(get_segment_key(pos), get_segment_value(pos));

				}

				const _K & get_segment_key(size_type pos) const {
					return keys[pos];
				}

				_ElPair get_segment_pair(size_type pos) {
					return _ElPair(get_segment_key(pos), get_segment_value(pos));
				}

				_K & get_segment_key(size_type pos) {
					return keys[pos];
				}
				const _V & get_segment_value(size_type pos) const {
					return values[pos];
				}

				_V & get_segment_value(size_type pos) {
					return values[pos];
				}
				void set_segment_key(size_type pos, const _K &k) {
					keys[pos] = k;
				}
				void destroy_segment_value(size_type pos) {
					values[pos].~_V();
				}
				_V* create_segment_value(size_type pos, const _V &v) {
					_V* r = &values[pos];
					new (r) _V(v);
					return r;
				}
				_V* create_segment_value(size_type pos) {
					_V* r = &values[pos];
					new (r) _V();
					return r;
				}
				void set_segment_value(size_type pos, const _V &v) {
					values[pos] = v;
				}

				void set_exists(size_type pos, bool f) {
					last_modified = pos;
					get_segment(pos).set_exists(get_segment_index(pos), f);
				}

				void set_overflows(size_type pos, bool f) {
					get_segment(pos).set_overflows(get_segment_index(pos), f);
				}

				inline bool exists_(size_type pos) const {
					return get_segment(pos).is_exists(get_segment_index(pos));
				}

				inline bool overflows_(size_type pos) const {
					return get_segment(pos).is_overflows(get_segment_index(pos));
				}

				inline bool overflowed_(size_type pos) const {
					return false;
				}

				inline size_type map_key(const _K& k) const {
					size_type h = (size_type)_H()(k);

					return key_mapper(h);

				}
				inline size_type map_rand_key(const _K& k) const {
					size_type h = (size_type)_H()(k);
					if (this->rand_probes)
						return key_mapper(randomize(h));
					return key_mapper(h); //
				}
				size_type get_e_size() const {
					return (size_type)(get_data_size() / config.BITS_SIZE) + 1;
				}

				void clear_data() {
					if (values) {
						for (size_type pos = 0; pos < get_data_size(); ++pos) {
							if (exists_(pos)) {
								get_value_allocator().destroy(&values[pos]);
							}
						}
					}
				}

				void free_values() {
					if (values) {
						clear_data();
						get_value_allocator().deallocate(values, get_data_size());
						values = nullptr;
					}
				}
				void free_data() {
					free_values();
					if (clusters) {
						size_type esize = get_e_size();
						for (size_type c = 0; c < esize; ++c) {
							get_segment_allocator().destroy(&clusters[c]);
						}
						get_segment_allocator().deallocate(clusters, get_e_size());
					}
					values = nullptr;
					clusters = nullptr;
				}
				double get_resize_factor() const {
					return key_mapper.resize_factor();
				}
				size_type get_probes() const {
					return this->probes;
				}
				void set_rand_probes() {
					this->rand_probes = this->probes;
				}
				void set_rand_probes(size_type rand_probes) {
					this->rand_probes = rand_probes;
				}

				/// clears all data and resize the new data vector to the parameter
				void resize_clear(size_type new_extent) {
					/// inverse of factor used to determine overflow list
					/// when overflow list is full rehash starts
					free_data();

					key_mapper = _Mapper(new_extent, config);

					mf = 1.0;
					assert(config.MAX_OVERFLOW_FACTOR > 0);
					if (is_logarithmic()) {
						probes = config.log2(new_extent)*logarithmic;
						overflow = config.log2(new_extent)*logarithmic;
					}
					else {
						probes = config.PROBES; //config.log2(new_extent); //-config.log2(config.MIN_EXTENT/2);  /// start probes config.PROBES; //
						overflow = std::max<size_type>(config.PROBES, new_extent / config.MAX_OVERFLOW_FACTOR); //config.log2(new_extent); // *16*
					}

					rand_probes = 0;

					initial_probes = probes;
					//std::cout << "rehash with overflow:" << overflow  << std::endl;
					elements = 0;
					removed = 0;
					empty_key = _K();
					keys_overflowed = false;
					overflow_elements = get_o_start();
					size_type esize = get_e_size();
					keys.resize(get_data_size());
					clusters = get_segment_allocator().allocate(esize);
					values = get_value_allocator().allocate(get_data_size());
					_KeySegment ks;
					for (size_type c = 0; c < esize; ++c) {
						get_segment_allocator().construct(&clusters[c], ks);
					}
					set_exists(get_data_size(), true);
					buckets = 0;

				};

				void clear() {
					size_type esize = get_e_size();
					//clear_data();
					for (size_type c = 0; c < esize; ++c) {
						clusters[c].clear();
					}
					set_exists(get_data_size(), true);
					elements = 0;
					removed = 0;
					//resize_clear(config.MIN_EXTENT);
				}

				hash_kernel(const key_compare& compare, const allocator_type& allocator)
					: clusters(nullptr), values(nullptr), eq_f(compare), mf(1.0f), allocator(allocator), sparse(false), logarithmic(0) {
					resize_clear(config.MIN_EXTENT);
				}

				hash_kernel() : clusters(nullptr), values(nullptr), mf(1.0f), sparse(false), logarithmic(0) {
					resize_clear(config.MIN_EXTENT);
				}

				hash_kernel(const hash_kernel& right) : clusters(nullptr), values(nullptr), mf(1.0f), sparse(false), logarithmic(0) {
					*this = right;
				}

				~hash_kernel() {
					free_data();
				}
				inline size_type get_extent() const {
					return key_mapper.extent;
				}
				void set_logarithmic(size_type loga) {
					logarithmic = loga;
				}
				size_type get_logarithmic() const {
					return this->logarithmic;
				}
				bool is_logarithmic() const {
					return this->logarithmic > 0;
				}
				void set_quadratic(bool sparse) {
					this->sparse = sparse;
				}
				bool is_quadratic() const {
					return this->sparse;
				}
				hash_kernel& operator=(const hash_kernel& right) {
					config = right.config;
					key_mapper = right.key_mapper;
					free_data();
					buckets = right.buckets;
					removed = right.removed;
					mf = right.mf;
					elements = right.elements;
					size_type esize = get_e_size();

					clusters = get_segment_allocator().allocate(esize);
					values = get_value_allocator().allocate(get_data_size());

					std::copy(values, right.values, right.values + right.get_data_size());
					keys = right.keys;
					std::copy(clusters, right.clusters, right.clusters + esize);

					return *this;
				}
				inline bool raw_equal_key(size_type pos, const _K& k) const {
					const _K& l = get_segment_key(pos); ///.key(get_segment_index(pos));
					return eq_f(l, k);
				}
				inline bool segment_equal_key_exists(size_type pos, const _K& k) const {
					_Bt index = get_segment_index(pos);
					const _Segment& s = get_segment(pos);
					return  eq_f(get_segment_key(pos), k) && s.is_exists(index);

				}

				bool equal_key(size_type pos, const _K& k) const {
					const _K& l = get_segment_key(pos);
					return eq_f(l, k);
				}

				inline size_type randomize(size_type v) const {
					return key_mapper.randomize(v);
				}

				inline size_type hash_probe_incr(size_type i) const {
					if (is_quadratic())
						return i*i;
					return 1;

				}
				/// when all inputs to this function is unique relative to current hash map(i.e. they dont exist in the hashmap)
				/// and there where no erasures. for maximum fillrate in rehash
				_V* unique_subscript_rest(const _K& k, size_type origin) {

					size_type pos = map_rand_key(k);

					size_type start = 0;
					for (unsigned int i = 0; i < probes && pos < get_extent(); ++i) {

						if (!exists_(pos)) {
							set_exists(pos, true);
							set_segment_key(pos, k);
							set_overflows(origin, true);
							keys_overflowed = true;
							++elements;
							last_modified = pos;
							return create_segment_value(pos);
						}
						pos += hash_probe_incr(i);
					}

					if (overflow_elements < end()) {
						pos = overflow_elements++;

						if (!exists_(pos)) {
							set_overflows(origin, true);
							keys_overflowed = true;
							set_exists(pos, true);
							set_segment_key(pos, k);
							++elements;

							last_modified = pos;
							return create_segment_value(pos);
						}
					};

					return nullptr;
				}
				_V* unique_subscript(const _K& k) {

					/// eventualy an out of memory (bad_allocation) exception will occur
					size_type pos = map_key(k);
					_Bt si = get_segment_index(pos);
					_Segment &s = clusters[pos >> config.BITS_LOG2_SIZE];/// get_segment(pos)

					if (!s.is_overflows(si) && !s.is_exists(si)) {
						s.toggle_exists(si);
						set_segment_key(pos, k);
						++elements;

						return create_segment_value(pos);
					}

					return unique_subscript_rest(k, pos);
				}

				_V* subscript_rest(const _K& k, size_type origin) {
					size_type pos = map_rand_key(k);
					for (unsigned int i = 0; i < probes && pos < get_extent(); ++i) {
						_Bt si = get_segment_index(pos);
						_Segment& s = get_segment(pos);
						if (!s.is_exists(si)) {
							s.toggle_exists(si);
							//s.key(si)=k;
							set_segment_key(pos, k);
							++elements;
							set_overflows(origin, true);
							keys_overflowed = true;
							return create_segment_value(pos);
						}
						pos += hash_probe_incr(i);
					}

					size_type at_empty = end();

					if (overflow_elements < end()) {
						if (!exists_(overflow_elements)) {
							at_empty = overflow_elements++;

						}
					}
					else if (removed) {
						size_type e = end();
						for (pos = get_o_start(); pos < e; ) {
							if (!exists_(pos)) {

								at_empty = pos; break;
							}
							++pos;
						}
					}

					pos = at_empty;
					if (pos != end()) {
						set_overflows(origin, true);
						keys_overflowed = true;
						set_exists(pos, true);
						set_segment_key(pos, k);
						size_type os = (overflow_elements - (get_extent() + initial_probes));
						if (os == 1) {
							stats.start_elements = elements;
							//std::cout << "overflow start: hash table size " << elements << " elements in over flow:" << os << std::endl;
						}

						if (overflow_elements == end() && stats.start_elements) {
							stats.end_elements = elements;
							size_type saved = stats.end_elements - stats.start_elements - os;
							double percent_saved = (100.0*((double)saved / (double)elements));

							// std::cout << "overflow end: hash table size " << elements << " elements in over flow:" << os << " saved : " << saved <<
							// std::endl <<  " percent saved " << std::setprecision(4) << percent_saved <<
							// std::endl;
						}

						++elements;
						return create_segment_value(pos);

					}
					return nullptr;
				}
				_V* subscript(const _K& k) {
					size_type pos = map_key(k);
					_Bt si = get_segment_index(pos);
					_Segment& s = get_segment(pos);
					bool key_overflows = s.is_overflows(si);
					bool key_exists = s.is_exists(si);
					//key_overflows = s.is_overflows(si);
					if (!key_overflows && !s.is_exists(si)) {
						s.toggle_exists(si);
						set_segment_key(pos, k);
						++elements;
						return create_segment_value(pos);
					}
					else if (s.is_exists(si) && equal_key(pos, k)) {
						return &(get_segment_value(pos));
					}

					size_type h = pos;
					if (key_overflows) {
						pos = find_rest(k, h);
						if (pos != end()) {
							return &(get_segment_value(pos));
						}
					}
					return subscript_rest(k, h);
				}
				size_type erase_rest(const _K& k, size_type origin)
					RABBIT_NOINLINE_ /// this function must never be inlined
				{
					size_type pos = find_rest(k, origin);

					if (pos != (*this).end()) {
						set_exists(pos, false);
						++removed;
						set_segment_key(pos, empty_key);
						destroy_segment_value(pos);
						--elements;
						if (pos >= get_o_start()) {
							size_type c = get_o_start();
							for (; c < overflow_elements; ++c) {
								if (origin == map_key(get_segment_key(c))) {
									break;
								}
							}
						}
						return 1;
					}
					return 0;
				}
				size_type erase(const _K& k) {

					size_type pos = map_key(k);

					_Bt si = get_segment_index(pos);
					_Segment& s = get_segment(pos);
					if (s.is_exists(si) && equal_key(pos, k)) { ///get_segment(pos).exists == ALL_BITS_SET ||
						set_segment_key(pos, empty_key);
						s.toggle_exists(si);
						destroy_segment_value(pos);
						--elements;
						++removed;
						return 1;
					}
					if (!s.is_overflows(si)) {
						return 0;
					}
					else
						return erase_rest(k, pos);

				}
				/// not used (could be used where hash table must actually shrink too)
				bool is_small() const {
					return (get_extent() > (config.MIN_EXTENT << 3)) && (elements < get_extent() / 8);
				}

				size_type count(const _K& k) const {
					size_type pos = (*this).find(k);
					if (pos == (*this).end()) {
						return 0;
					}
					else return 1;
				}
				const _V& at(const _K& k) const {
					size_type pos = find(k);
					if (pos != (*this).end()) {
						return get_segment_value(pos);
					}
					throw std::exception();
				}
				_V& at(const _K& k) {
					size_type pos = find(k);
					if (pos != (*this).end()) {
						return get_segment_value(pos);
					}
					throw std::exception();
				}

				bool get(const _K& k, _V& v) const {
					size_type pos = find(k);
					if (pos != (*this).end()) {
						v = get_segment_value(pos);
						return true;
					}
					return false;
				}

				size_type find_rest(const _K& k, size_type origin) const {
					/// randomization step for attack mitigation
					size_type pos = map_rand_key(k);

					for (unsigned int i = 0; i < probes && pos < get_extent();) {
						_Bt si = get_segment_index(pos);
						if (segment_equal_key_exists(pos, k)) {
							return pos;
						}
						pos += hash_probe_incr(i);
						++i;
					}

					for (pos = get_o_start(); pos < overflow_elements; ) {
						if (equal_key(pos, k) && exists_(pos)) return pos;
						++pos;
					}

					return end();
				}
				size_type find(const _K& k, size_type& pos) const {
					pos = map_key(k);
					bool is_empty = eq_f(empty_key, k);

					if (is_empty) {
						_Bt index = get_segment_index(pos);
						const _Segment& s = get_segment(pos);
						if (s.is_exists(index) && equal_key(pos, k)) { ///get_segment(pos).exists == ALL_BITS_SET ||
							return pos;
						}
						if (!s.is_overflows(index)) {
							return end();
						}
					}
					else {
						if (equal_key(pos, k)) return pos;

					}
					_Bt index = get_segment_index(pos);
					const _Segment& s = get_segment(pos);
					if (!s.is_overflows(index)) {
						return end();
					}

					return find_rest(k, pos);
				}
				size_type find(const _K& k) const {

					size_type pos;
					return find(k, pos);
				}

				size_type begin() const {
					if (elements == 0)
						return end();
					size_type pos = 0;
					while (!exists_(pos)) {
						++pos;

					}
					return pos;
				}
				size_type end() const {
					return get_data_size();
				}
				size_type size() const {
					return elements;
				}
				typedef std::shared_ptr<hash_kernel> ptr;
			}; /// hash_kernel
			typedef std::shared_ptr<hash_kernel> _KernelPtr;
			typedef std::vector<_KernelPtr> _Kernels;
			_Kernels versions;
		public:

			struct iterator {
				typedef hash_kernel* kernel_ptr;
				const basic_unordered_map* h;				
				size_type pos;
				mutable char rdata[sizeof(_ElPair)];
			private:
				_Bt index;
				_Bt exists;
				_Bt bsize;
				inline const kernel_ptr get_kernel() const {					
					return const_cast<basic_unordered_map*>(h)->current.get();

				}
				inline kernel_ptr get_kernel() {					
					return h->current.get();
				}
				void set_index() {
					if (h != nullptr && !is_end(*this)) {//
						const _Segment& s = get_kernel()->get_segment(pos);
						exists = s.exists;
						index = get_kernel()->get_segment_index(pos);
						bsize = get_kernel()->config.BITS_SIZE;
					}
				}
				void check_index() {

				}
				void increment() {
					++pos;
					++index;
					if (index == bsize) {
						set_index();
					}

				}
			public:
				iterator() : h(nullptr), pos(0) { 
				}

				iterator(const end_iterator&) : h(nullptr), pos(end_pos) {
				}
				iterator(const basic_unordered_map* h, size_type pos) :  pos(pos) {
					this->h = h;					
					set_index();
				}

				iterator(const iterator& r) {
					(*this) = r;
				}

				//~iterator() {
				//}

				iterator& operator=(const iterator& r) {
					pos = r.pos;
					h = r.h;
					set_index();

					return (*this);
				}
				inline iterator& operator++() {
					do {
						increment();
					} while ((exists & (((_Bt)1) << index)) == (_Bt)0);
					//increment();
					//while ((exists & (((_Bt)1) << index)) == (_Bt)0) {
					//	increment();
					//}

					return (*this);
				}
				iterator operator++(int) {
					iterator t = (*this);
					++(*this);
					return t;
				}
				inline _V& get_value() {
					return get_kernel()->get_segment_value((*this).pos);
				}
				inline const _V& get_value() const {
					return get_kernel()->get_segment_value((*this).pos);
				}
				inline _K& get_key() {
					return get_kernel()->get_segment_key((*this).pos);
				}
				inline const _K& get_key() const {
					return get_kernel()->get_segment_key((*this).pos);
				}
				const _ElPair operator*() const {
					return get_kernel()->get_segment_pair((*this).pos);
				}
				inline _ElPair operator*() {
					return get_kernel()->get_segment_pair((*this).pos);
				}
				inline _ElPair* operator->() const {
					/// can reconstruct multiple times on same memory because _ElPair is only references
					_ElPair* ret = new ((void *)rdata) _ElPair(get_kernel()->get_segment_pair(pos));
					return ret;
				}
				inline const _ElPair *operator->() {
					/// can reconstruct multiple times on same memory because _ElPair is only references
					_ElPair* ret = new ((void *)rdata) _ElPair(get_kernel()->get_segment_pair(pos));
					return ret;
				}
				inline bool operator==(const iterator& r) const {
					if (r.pos == end_pos) return is_end();
					return (pos == r.pos);
				}
				bool operator!=(const iterator& r) const {
					if (r.pos == end_pos) return !is_end();
					return (pos != r.pos);
				}
				inline bool operator==(const end_iterator& r) const {
					return is_end();
				}
				bool operator!=(const end_iterator& r) const {
					return !is_end();

				}
				bool is_end(const iterator& r) const {
					if (h == nullptr) return pos == end_pos;
					return r.pos >= get_kernel()->end();
				}
				bool is_end() const {
					return is_end(*this);
				}
				size_type get_pos() const {
					return pos;
				}

			};

			struct const_iterator {
			private:
				typedef hash_kernel* kernel_ptr;
				const basic_unordered_map* h;
				//mutable kernel_ptr h;
				_Bt index;
				_Bt exists;
				mutable char rdata[sizeof(_ElPair)];
				inline const kernel_ptr get_kernel() const {
				
					return const_cast<basic_unordered_map*>(h)->pcurrent; // current.get();
				}
				inline kernel_ptr get_kernel() {
				
					return const_cast<basic_unordered_map*>(h)->pcurrent; // current.get();
				}
				void set_index() {
					if (get_kernel() != nullptr) { ///  && !is_end(*this)
						const _Segment& s = get_kernel()->get_segment(pos);
						exists = s.exists;
						index = get_kernel()->get_segment_index(pos);
					}
				}
				void check_index() {

				}
				void increment() {
					++pos;
					++index;
					if (index == get_kernel()->config.BITS_SIZE) {
						set_index();
					}

				}
			public:
				size_type pos;

				const_iterator() : h(nullptr){

				}
                const_iterator(const end_iterator&) : h(nullptr), pos(end_pos) {
				}
				//~const_iterator() {

				//}
				const_iterator(const basic_unordered_map* h, size_type pos) : pos(pos) {
					this->h = h; // ->current.get();
					set_index();
				}
				const_iterator(const iterator& r) : h(nullptr){
					(*this) = r;
				}

				const_iterator& operator=(const iterator& r) {
					pos = r.pos;
					h = r.h;
					set_index();
					return (*this);
				}

				const_iterator& operator=(const const_iterator& r) {
					pos = r.pos;
					h = r.h;
					index = r.index;
					return (*this);
				}

				const_iterator& operator++() {
					do {
						increment();
					} while ((exists & (((_Bt)1) << index)) == (_Bt)0);
					//increment();
					//while ((exists & (((_Bt)1) << index)) == (_Bt)0) {
					//	increment();
					//}


					return (*this);
				}
				const_iterator operator++(int) {
					return (*this);
				}
				const _ElPair operator*() const {
					return get_kernel()->get_segment_pair(pos);

				}
				const _ElPair *operator->() const {
					/// can reconstruct multiple times on same memory because _ElPair is only references
					_ElPair* ret = new ((void *)rdata) _ElPair(get_kernel()->get_segment_pair(pos));
					return ret;
				}

				inline bool operator==(const const_iterator& r) const {
					if (r.pos == end_pos) return is_end();
					return (pos == r.pos);
				}
				bool operator!=(const const_iterator& r) const {
					if (r.pos == end_pos) return !is_end();
					return (pos != r.pos);
				}
				bool is_end(const const_iterator& r) const {
					if (h == nullptr) return false;
					return r.pos >= get_kernel()->end();
				}
				bool is_end() const {
					return is_end(*this);
				}
				size_type get_pos() const {
					return pos;
				}

			};

		protected:
			/// the default config for each hash instance
			rabbit_config default_config;
			key_compare key_c;
			allocator_type alloc;

			double recalc_growth_factor(size_type elements) {
				return 1.8;
			}

			void rehash() {
				size_type to = current->key_mapper.next_size();
				rehash(to);
			}
			void set_current(typename hash_kernel::ptr c) {

				current = c;
				pcurrent = current.get();
			}

			typename hash_kernel::ptr current;
			hash_kernel* pcurrent;
			inline void create_current() {
				if (current == nullptr)

					set_current(std::allocate_shared<hash_kernel>(alloc, key_c, alloc));
			}
		public:
			float load_factor() const {
				if (current == nullptr) return 0;
				return current->load_factor();
			}
			size_type bucket_count() const {
				if (current == nullptr) return 0;
				return current->bucket_count();
			}
			size_type bucket_size(size_type n) const {
				if (current == nullptr) return 0;
				return current->bucket_size(n);
			}
			float max_load_factor() const {
				if (current == nullptr) 1;
				return current->max_load_factor();
			}

			void max_load_factor(float z) {
				create_current();
				current->max_load_factor(z);
			}
			bool empty() const {
				if (current == nullptr) return true;
				return current->size() == 0;
			}
			void reserve(size_type atleast) {
				create_current();
				rehash((size_type)((double)atleast*current->get_resize_factor()));
			}
			void resize(size_type atleast) {
				create_current();
				rehash(current->key_mapper.nearest_larger(atleast));
			}
			void rehash(size_type to_) {
				create_current();
				rabbit_config config;
				size_type to = std::max<size_type>(to_, config.MIN_EXTENT);
				/// can cause oom e because of recursive rehash'es

				typename hash_kernel::ptr rehashed = std::allocate_shared<hash_kernel>(alloc);
				size_type extent = current->get_extent();
				size_type new_extent = to;
				size_type nrand_probes = 0;
				hash_kernel * reh = rehashed.get();
				hash_kernel * cur = current.get();
				try {

					rehashed->set_quadratic(current->is_quadratic());
					rehashed->set_logarithmic(current->get_logarithmic());
					rehashed->resize_clear(new_extent);
					rehashed->mf = (*this).current->mf;
					//std::cout << " load factor " << current->load_factor() << std::endl;
					if (current->load_factor() < 0.2) {
						/// std::cout << "possible attack/bad hash detected : using random probes : " << current->get_probes() << std::endl;
						nrand_probes = 1;
						rehashed->set_rand_probes(nrand_probes);
					}
					using namespace std;


					while (true) {
						iterator e = end();
						size_type ctr = 0;
						bool rerehashed = false;

						//_K k;
						for (iterator i = begin(); i != e; ++i) {
							//std::swap(k,(*i).first);
							_RV v = rehashed->subscript((*i).first);
							if (v != nullptr) {
								*v = i->second;
								/// a cheap check to illuminate subtle bugs during development
								if (++ctr != rehashed->elements) {
									cout << "iterations " << ctr << " elements " << rehashed->elements << " extent " << rehashed->get_extent() << endl;
									cout << "inside rehash " << rehashed->get_extent() << endl;
									cout << "new " << rehashed->elements << " current size:" << current->elements << endl;
									throw bad_alloc();
								}

							}
							else {
								//std::cout << "rehashing in rehash " << ctr << " of " << current->elements << std::endl;
								rerehashed = true;
								new_extent = rehashed->key_mapper.next_size();
								rehashed = std::allocate_shared<hash_kernel>(alloc);
								rehashed->resize_clear(new_extent);
								rehashed->mf = (*this).current->mf;
								rehashed->set_rand_probes(nrand_probes);
								// i = begin(); // start over
								//ctr = 0;
								break;

							}
						}
						if (rehashed->elements == current->elements) {
							break;
						}
						else if (!rerehashed) {
							cout << "hash error: unequal key count - retry rehash " << endl;
							cout << "iterations " << ctr << " elements " << rehashed->elements << " extent " << rehashed->get_extent() << endl;
							cout << "new " << rehashed->elements << " current size:" << current->elements << endl;
							throw bad_alloc();
						}
						else {

							//rehashed->resize_clear(rehashed->get_extent());
							//break;
						}

					}/// for

				}
				catch (std::bad_alloc &e) {
					std::cout << "bad allocation: rehash failed in temp phase :" << new_extent << std::endl;
					size_t t = 0;
					std::cin >> t;
					throw e;
				}
				set_current(rehashed);

			}
			void clear() {
				if (current != nullptr)
					current->clear();
				current = nullptr;
				///set_current(std::allocate_shared<hash_kernel>(alloc));
			}

			void clear(const key_compare& compare, const allocator_type& allocator) {
				set_current(std::allocate_shared<hash_kernel>(allocator, compare, allocator));
			}

			basic_unordered_map() :current(nullptr) {
				//
			}

			basic_unordered_map(const key_compare& compare, const allocator_type& allocator) : key_c(compare), alloc(allocator) {

			}

			basic_unordered_map(const basic_unordered_map& right) {
				*this = right;
			}

			~basic_unordered_map() {

			}

			void swap(basic_unordered_map& with) {
				typename hash_kernel::ptr t = with.current;
				with.set_current(this->current);
				this->set_current(t);
			}

			void move(basic_unordered_map& from) {
				(*this).current = from.current;
				from.current = nullptr;
			}

			basic_unordered_map& operator=(const basic_unordered_map& right) {
				(*this).set_current(std::allocate_shared<hash_kernel>(alloc));
				(*this).reserve(right.size());
				const_iterator e = right.end();
				for (const_iterator c = right.begin(); c != e; ++c) {
					(*this)[(*c).first] = (*c).second;
				}

				return *this;
			}

			hasher hash_function() const {
				return (this->current->hf);
			}

			key_equal key_eq() const {
				if (current != nullptr)
					return (this->current->eq_f);
				return key_equal();
			}
			iterator insert(const _K& k, const _V& v) {
				create_current();
				(*this)[k] = v;
				return iterator(this, current->last_modified);
			}

			iterator insert(const std::pair<_K, _V>& p) {

				return iterator(this, insert(p.first, p.second));
			}
			/// generic template copy
			template<class _Iter>
			iterator insert(_Iter start, _Iter _afterLast) {
				create_current();
				for (_Iter i = start; i != _afterLast; ++i) {
					insert((*i).first, (*i).second);
				}
				return iterator(this, current->last_modified);
			}
			/// fast getter that doesnt use iterators and doesnt change the table without letting you know
			bool get(const _K& k, _V& v) const {
				if (current != nullptr)
					return (*this).current->get(k, v);
				return false;
			}
			/// throws a exception when value could not match the key
			const _V& at(const _K& k) const {
				if (current == nullptr) throw std::exception();
				return (*this).current->at(k);
			}
			_V& at(const _K& k) {
				create_current();
				return (*this).current->at(k);
			}

			bool error(const _K& k) {
				_V *rv = current->subscript(k);
				return rv == nullptr;
			}

			_V& operator[](const _K& k) {
				create_current();
				_V *rv = current->subscript(k);
				while (rv == nullptr) {
					this->rehash();
					rv = current->subscript(k);
				}
				return *rv;
			}
			size_type erase(const _K& k) {
				if (current == nullptr) return size_type();
				//if(current->is_small()){
				//	rehash(1);
				//}
				return current->erase(k);
			}
			size_type erase(iterator i) {
				return erase((*i).first);
			}
			size_type erase(const_iterator i) {
				return erase((*i).first);
			}
			size_type count(const _K& k) const {
				if (current == nullptr)return size_type();
				return current->count(k);
			}
			iterator find(const _K& k) const {
				if (current == nullptr) return iterator(this, size_type());

				return iterator(this, current->find(k));
			}
			iterator begin() const {
				if (current == nullptr)return iterator(this, size_type());
				return iterator(this, current->begin());
			}
			end_iterator end() const {
				return end_iterator(); // iterator(end_pos);
			}
			const_iterator cbegin() const {
				if (current == nullptr)return const_iterator(this, size_type());
				return const_iterator(this, current->begin());
			}
			const_iterator cend() const {
				return iterator(end_pos);
			}
			size_type size() const {
				if (current == nullptr)return size_type();
				return current->size();
			}
			bool is_sparse() const {
				if (current == nullptr)return false;
				return this->current->is_sparse();
			}
			void set_quadratic(bool quadratic) {
				create_current();
				this->current->set_quadratic(quadratic);
			}
			void set_logarithmic(size_type logarithmic) {
				create_current();
				this->current->set_logarithmic(logarithmic);
			}
	};

	/// the stl compatible unordered map interface
	template
		<	class _Kty
		, class _Ty
		, class _Hasher = rabbit_hash<_Kty>
		, class _Keyeq = std::equal_to<_Kty>
		, class _Alloc = std::allocator<std::pair<const _Kty, _Ty> >
		, class _Traits = default_traits
		>
		class unordered_map : public basic_unordered_map<_Kty, _Ty, _Hasher, _Keyeq, _Alloc, _Traits>
	{	// hash table of {key, mapped} values, unique keys
	public:
		typedef basic_unordered_map<_Kty, _Ty, _Hasher, _Keyeq, _Alloc, _Traits> _Base;

		typedef unordered_map<_Kty, _Ty, _Hasher, _Keyeq, _Alloc, _Traits> _Myt;

		typedef _Hasher hasher;
		typedef _Kty key_type;
		typedef _Ty mapped_type;
		typedef _Keyeq key_equal;
		typedef typename _Base::key_compare key_compare;

		//	typedef typename _Base::value_compare value_compare;
		typedef typename _Base::allocator_type allocator_type;
		typedef typename _Base::size_type size_type;
		typedef typename _Base::difference_type difference_type;
		typedef typename _Base::pointer pointer;
		typedef typename _Base::const_pointer const_pointer;
		typedef typename _Base::reference reference;
		typedef typename _Base::const_reference const_reference;
		typedef typename _Base::iterator iterator;
		typedef typename _Base::const_iterator const_iterator;
		//	typedef typename _Base::reverse_iterator reverse_iterator;
		//	typedef typename _Base::const_reverse_iterator
		//		const_reverse_iterator;
		typedef typename _Base::value_type value_type;

		typedef typename _Base::iterator local_iterator;
		typedef typename _Base::const_iterator const_local_iterator;

		unordered_map()
			: _Base(key_compare(), allocator_type())
		{	// construct empty map from defaults
		}

		explicit unordered_map(const allocator_type& a)
			: _Base(key_compare(), a)
		{	// construct empty map from defaults, allocator
		}

		unordered_map(const _Myt& _Right)
			: _Base(_Right)
		{	// construct map by copying _Right
		}

		//unordered_map(const _Myt& _Right, const allocator_type& _Al)
		//	: _Base(_Right, _Al)
		//	{	// construct map by copying _Right, allocator
		//	}

		explicit unordered_map(size_type _Buckets)
			: _Base(key_compare(), allocator_type())
		{	// construct empty map from defaults, ignore initial size
			this->rehash(_Buckets);
		}

		unordered_map(size_type _Buckets, const hasher& _Hasharg)
			: _Base(key_compare(_Hasharg), allocator_type())
		{	// construct empty map from hasher
			this->rehash(_Buckets);
		}

		unordered_map
		(size_type _Buckets
			, const hasher& _Hasharg
			, const _Keyeq& _Keyeqarg
		)
			: _Base(key_compare(_Hasharg, _Keyeqarg), allocator_type())
		{	// construct empty map from hasher and equality comparator
			this->rehash(_Buckets);
		}

		unordered_map
		(size_type _Buckets
			, const hasher& _Hasharg
			, const _Keyeq& _Keyeqarg
			, const allocator_type& a
		)
			: _Base(key_compare(_Hasharg, _Keyeqarg), a)
		{	// construct empty map from hasher and equality comparator
			this->rehash(_Buckets);
		}

		template<class _Iter>
		unordered_map
		(_Iter _First
			, _Iter _Last
		)
			: _Base(key_compare(), allocator_type())
		{	// construct map from sequence, defaults
			_Base::insert(_First, _Last);
		}

		template<class _Iter>
		unordered_map
		(_Iter _First
			, _Iter _Last
			, size_type _Buckets
		)
			: _Base(key_compare(), allocator_type())
		{	// construct map from sequence, ignore initial size
			this->rehash(_Buckets);
			_Base::insert(_First, _Last);
		}

		template<class _Iter>
		unordered_map
		(_Iter _First
			, _Iter _Last
			, size_type _Buckets
			, const hasher& _Hasharg
		)
			: _Base(key_compare(_Hasharg), allocator_type())
		{
			this->rehash(_Buckets);
			_Base::insert(_First, _Last);
		}

		template<class _Iter>
		unordered_map
		(_Iter _First
			, _Iter _Last
			, size_type _Buckets
			, const hasher& _Hasharg
			, const _Keyeq& _Keyeqarg
		)
			: _Base(key_compare(_Hasharg, _Keyeqarg), allocator_type())
		{
			this->rehash(_Buckets);
			_Base::insert(_First, _Last);
		}

		template<class _Iter>
		unordered_map
		(_Iter _First
			, _Iter _Last
			, size_type _Buckets
			, const hasher& _Hasharg
			, const _Keyeq& _Keyeqarg
			, const allocator_type& _Al
		)
			: _Base(key_compare(_Hasharg, _Keyeqarg), _Al)
		{
			this->rehash(_Buckets);
			_Base::insert(_First, _Last);
		}

		_Myt& operator=(const _Myt& _Right) {	// assign by copying _Right
			_Base::operator=(_Right);
			return (*this);
		}

		unordered_map(_Myt&& from)
		{
			_Base::move(from);
		}

		unordered_map(_Myt&& from, const allocator_type& _Al)
			: _Base(key_compare(), _Al)
		{	// construct map by moving _Right, allocator
			_Base::move(from);
		}

		_Myt& operator=(_Myt&& from) {	// assign by moving _Right
			_Base::move(from);
			return (*this);
		}
		const mapped_type& at(const key_type& k) const {
			return _Base::at(k);
		}

		mapped_type& at(const key_type& k) {
			return _Base::at(k);
		}

		mapped_type& operator[](const key_type& k) {
			// find element matching _Keyval or insert with default mapped
			return _Base::operator[](k);
		}

		// find element matching _Keyval or insert with default mapped
		mapped_type& operator[](key_type&& k) {
			return (*this)[k];
		}

		void swap(_Myt& _Right) {	// exchange contents with non-movable _Right
			_Base::swap(_Right);
		}



	};
	template
		<	class _Kty
		, class _Ty
		, class _Hasher = rabbit_hash<_Kty>
		, class _Keyeq = std::equal_to<_Kty>
		, class _Alloc = std::allocator<std::pair<const _Kty, _Ty> >
		, class _Traits = sparse_traits
		>
		class sparse_unordered_map : public unordered_map<_Kty, _Ty, _Hasher, _Keyeq, _Alloc, _Traits>
	{
	public:
		typedef _Hasher hasher;
		typedef _Kty key_type;
		typedef _Ty mapped_type;
		typedef _Keyeq key_equal;
		typedef unordered_map<_Kty, _Ty, _Hasher, _Keyeq, _Alloc, _Traits> _Base;
		typedef typename _Base::key_compare key_compare;

		//	typedef typename _Base::value_compare value_compare;
		typedef typename _Base::allocator_type allocator_type;
		typedef typename _Base::size_type size_type;
		typedef typename _Base::difference_type difference_type;
		typedef typename _Base::pointer pointer;
		typedef typename _Base::const_pointer const_pointer;
		typedef typename _Base::reference reference;
		typedef typename _Base::const_reference const_reference;
		typedef typename _Base::iterator iterator;
		typedef typename _Base::const_iterator const_iterator;
		//	typedef typename _Base::reverse_iterator reverse_iterator;
		//	typedef typename _Base::const_reverse_iterator
		//		const_reverse_iterator;
		typedef typename _Base::value_type value_type;

		typedef typename _Base::iterator local_iterator;
		typedef typename _Base::const_iterator const_local_iterator;
		sparse_unordered_map() {
		}
		~sparse_unordered_map() {
		}
	};

	/// the unordered set
	template <typename _K, typename _H = rabbit_hash<_K>>
	class unordered_set : public basic_unordered_map<_K, char, _H> {
	protected:
		typedef basic_unordered_map<_K, char, _H> _Container;
	public:


		unordered_set() {
		}

		~unordered_set() {
		}

		void insert(const _K& k) {
			_Container::insert(k, '0');
		}

	}; /// unordered set
}; // rab-bit

#endif ///  _RABBIT_H_CEP_20150303_
