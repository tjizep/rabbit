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
#include <cstddef>
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
namespace rabbit{

	template <class _Config>
	struct _BinMapper{
		typedef typename _Config::size_type size_type;
		typedef _Config config_type;
		size_type extent;
		size_type extent1;
		size_type extent2;
		size_type primary_bits;
		size_type random_val;
		unsigned long long gate_bits;
		_Config config;
		_BinMapper(){
		}
		_BinMapper(size_type new_extent,const _Config& config){
			this->config = config;
			this->extent = ((size_type)1) << this->config.log2(new_extent);
			this->extent1 = this->extent-1;
			this->extent2 = this->config.log2(new_extent);
			this->primary_bits = extent2;
			//std::minstd_rand rd;
			//std::mt19937 gen(rd());
			//std::uniform_int_distribution<size_type> dis(1ll<<4, std::numeric_limits<size_type>::max());
			this->random_val = 0; //(size_type)dis(gen);
			if(new_extent < (1ll<<32ll)){
                this->gate_bits = (1ll<<32ll) - 1ll;
            }else{
                this->gate_bits = (1ll<<62ll) - 1ll;
            }
		}
		inline size_type nearest_larger(size_type any){
			size_type l2 = this->config.log2(any);
			return (size_type)(2ll << l2);
		}
		size_type randomize(size_type other) const {
		    size_type r = other>>this->primary_bits;
			return other + r; // ((r*r) >> 2); //(other ^ random_val) & this->extent1;
		}
		size_type operator()(size_type h_n) const {
			return h_n & this->extent1 ;
		}
		double resize_factor() const {
			return 2;
		}
		double recalc_growth_factor(size_type elements)  {
			return 2;
		}

		inline size_type next_size(){

			double r = recalc_growth_factor(this->extent) * this->extent;
			assert(r > (double)extent);
			return (size_type)r;
		}
	};
	template<typename _Ht>
	struct rabbit_hash{
		size_t operator()(const _Ht& k) const{
			return (size_t) std::hash<_Ht>()(k); ///
		};
	};
	template<>
	struct rabbit_hash<long>{
		unsigned long operator()(const long& k) const{
			return (unsigned long)k;
		};
	};
	template<>
	struct rabbit_hash<unsigned long>{
		inline unsigned long operator()(const unsigned long& k) const{
			return k;
		};
	};
	template<>
	struct rabbit_hash<unsigned int>{
		inline unsigned int operator()(const unsigned int& k) const{
			return k;
		};
	};
	template<>
	struct rabbit_hash<int>{
		inline unsigned int operator()(const int& k) const{
			return k;
		};
	};
	template<>
	struct rabbit_hash<unsigned long long>{
		inline unsigned long long operator()(const unsigned long long& k) const{
			return k;
		};
	};
	template<>
	struct rabbit_hash<long long>{
		inline unsigned long long operator()(const long long& k) const{
			return (unsigned  long)k;
		};
	};

    template<int logarithmic = 0>
	class basic_config{
	public:
		typedef unsigned long long int _Bt; /// exists ebucket type - not using vector<bool> - interface does not support bit bucketing
		/// if even more speed is desired but you'r willing to live with a 4 billion key limit then
		//typedef unsigned long size_type;
		typedef std::size_t size_type;

		size_type log2(size_type n){
			size_type r = 0;
			while (n >>= 1)
			{
				r++;
			}
			return r;
		}
		_Bt BUCKET_COUNT ;
		_Bt CHAR_BITS ;
		_Bt BITS_SIZE ;
		_Bt BITS_SIZE1 ;
		_Bt ALL_BITS_SET ;
		_Bt LOGARITHMIC ;
		/// maximum probes per access
		size_type PROBES; /// a value of 32 gives a little more speed but much larger table size(> twice the size in some cases)
		size_type BITS_LOG2_SIZE;
		/// this distributes the h values which are powers of 2 a little to avoid primary clustering when there is no
		///	hash randomizer available

		size_type MIN_EXTENT;
		size_type MAX_OVERFLOW_FACTOR ;

		basic_config(const basic_config& right){
			*this = right;
		}

		basic_config& operator=(const basic_config& right){
			BUCKET_COUNT = right.BUCKET_COUNT;
			CHAR_BITS = right.CHAR_BITS;
			BITS_SIZE = right.BITS_SIZE;
			BITS_SIZE1 = right.BITS_SIZE1;
			BITS_LOG2_SIZE = right.BITS_LOG2_SIZE;
			ALL_BITS_SET = right.ALL_BITS_SET;
			PROBES = right.PROBES; /// a value of 32 gives a little more speed but much larger table size(> twice the size in some cases)
			MIN_EXTENT = right.MIN_EXTENT;
			MAX_OVERFLOW_FACTOR = right.MAX_OVERFLOW_FACTOR;
			LOGARITHMIC = right.LOGARITHMIC;
			return *this;
		}

		basic_config(){
			BUCKET_COUNT = 1;
			CHAR_BITS = 8;
			BITS_SIZE = (sizeof(_Bt) * CHAR_BITS);
			BITS_SIZE1 = BITS_SIZE-1;
			BITS_LOG2_SIZE = (size_type) log2((size_type)BITS_SIZE);
			ALL_BITS_SET = ~(_Bt)0;
			PROBES = 16;
			MIN_EXTENT = 4; /// start size of the hash table
			MAX_OVERFLOW_FACTOR = 1<<17; //BITS_SIZE*8/sizeof(_Bt);
            LOGARITHMIC = logarithmic;
		}
	};
	template<class _InMapper>
	struct basic_traits{
		typedef typename _InMapper::config_type rabbit_config;
		typedef typename rabbit_config::_Bt _Bt;
		typedef typename rabbit_config::size_type size_type;
		typedef ptrdiff_t difference_type;
		typedef _InMapper _Mapper;
	};
	typedef basic_traits<_BinMapper<basic_config<0> > > default_traits;
	typedef basic_traits<_BinMapper<basic_config<8> > > sparse_traits;


    template
	<	class _K
	,	class _V
	,	class _H = rabbit_hash<_K>
	,	class _E = std::equal_to<_K>
	,	class _Allocator = std::allocator<_K>
	,	class _Traits = default_traits
	>
	class basic_unordered_map {
	public:
		typedef _K key_type;

		typedef _V mapped_type;

		typedef std::pair<_K,_V> _ElPair;
		typedef std::pair<const _K, const _V> _ConstElPair;
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

	protected:
        struct overflow_stats{
            size_type start_elements;
            size_type end_elements;
            overflow_stats() : start_elements(0),end_elements(0){}
        };

		struct _KeySegment{

		public:
			_Bt overflows;
			_Bt exists;
		private:
			void set_bit(_Bt& w, _Bt index, bool f){

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

			inline void set_exists(_Bt index, bool f){
				set_bit(exists,index,f);
			}

			inline void toggle_exists(_Bt index){
				exists ^= ((_Bt)1 << index);
			}

			void set_overflows(_Bt index, bool f){
				set_bit(overflows,index,f);
			}

			void clear(){
				exists = 0;
				overflows = 0;
			}
			_KeySegment(){
				exists = 0;
				overflows = 0;
			}
		};
	public:
		typedef _KeySegment _Segment;
		/// the vector that will contain the segmented mapping pairs and flags
		typedef std::vector<_Segment, _Allocator> _Segments;
		typedef std::vector<_ElPair, _Allocator> _Keys;

		struct hash_kernel{
			/// settings configuration
			rabbit_config config;
			size_type elements;
			size_type initial_probes;
			size_type probes;
			size_type rand_probes; /// used when there might be an attack
			size_type last_modified;
			/// the existence bit set is a factor of BITS_SIZE+1 less than the extent
			_Segment* clusters;///a.k.a. pages
			_ElPair* keys;

			size_type overflow;
			size_type overflow_elements;
			overflow_stats stats;
			_Mapper key_mapper;
			_H hf;
			_E eq_f;
			float mf;
			size_type buckets;
			size_type removed;
			_Allocator allocator;
			_K empty_key;
			size_type logarithmic;
            size_type collisions;
			typename _Allocator::template rebind<_Segment>::other get_segment_allocator() {
				return typename _Allocator::template rebind<_Segment>::other(allocator) ;
			}
			typename _Allocator::template rebind<_ElPair>::other get_el_allocator(){
				return typename _Allocator::template rebind<_ElPair>::other(allocator) ;
			}
			typename _Allocator::template rebind<_V>::other get_value_allocator(){
				return typename _Allocator::template rebind<_V>::other(allocator) ;
			}
			size_type capacity() const {
			    return get_data_size();
			}

			/// the minimum load factor
			float load_factor() const{
			    if(!elements) return 0;
				return (float)((double)elements/(double)bucket_count());
			}
			float collision_factor() const{
				return (float)((double)collisions/(double)bucket_count());
			}

			/// there are a variable ammount of buckets there are at most this much
			///
			size_type bucket_count() const {
                if(!elements) return 0;
				return get_data_size();
			}
			/// the size of a bucket can be calculated based on the
			/// hash value of its first occupant
			/// mainly to satisfy stl conventions
			size_type bucket_size ( size_type n ) const{
				size_type pos = n;
				if (!overflows_(pos)) {
                    if (exists_(pos) && map_key(get_key(pos)) == n)
                        return 1;
                    else return 0;
                }
				size_type m = pos + probes;
				size_type r = 0;
				for(; pos < m;++pos){
					if(!exists_(pos)){
					}else if(map_key(get_key(pos)) == n){
						++r;
					}
				}
				size_type e = end();
				for(pos=get_o_start(); pos < e; ){
					if(!exists_(pos)){
					}else if(map_key(get_key(pos)) == n){
						++r;
					}
					++pos;
				}
				return r;
			}

			float max_load_factor() const {
				return mf;
			}

			void max_load_factor ( float z ){
				mf = z;
			}

			/// total data size, never less than than size()
			size_type get_data_size() const {
				return get_extent()+initial_probes+overflow;
			}

			/// the overflow start
			size_type get_o_start() const {
				return get_extent()+initial_probes;
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

			const _ElPair &get_pair(size_type pos) const {
				return keys[pos];
			}

			const _K & get_key(size_type pos) const {
				return keys[pos].first;
			}

			const _V & get_value(size_type pos) const {
				return keys[pos].second;
			}


			_ElPair& get_pair(size_type pos) {
				return keys[pos];
			}

			_K & get_key(size_type pos) {
				return keys[pos].first;
			}

			_V & get_value(size_type pos) {
				return keys[pos].second;
			}

			void set_segment_key(size_type pos, const _K &k) {
				keys[pos].first = k;
			}

			void destroy_segment_value(size_type pos){
				keys[pos].second = _V();
			}

			_V* create_segment_value(size_type pos) {
				_V* r = &(keys[pos].second);
				return r;
			}

			void set_segment_value(size_type pos, const _V &v) {
				keys[pos].second = v;
			}

			void set_exists(size_type pos, bool f){
				last_modified = pos;
				get_segment(pos).set_exists(get_segment_index(pos),f);
			}

			void set_overflows(size_type pos, bool f){
				get_segment(pos).set_overflows(get_segment_index(pos),f);
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
            inline size_type hash_key(const _K& k) const {
                return (size_type)_H()(k);
            }

			inline size_type map_key(const _K& k) const {
				size_type h = (size_type)_H()(k);
				return map_hash(h);
			}

			inline size_type map_hash(size_type h) const {
				return key_mapper(h);
			}

			inline size_type map_rand_key(const _K& k) const {
				size_type h = (size_type)_H()(k);
				if(this->rand_probes)
                    return map_hash(randomize(h));
				return map_hash(h); //
			}

			inline size_type map_rand_key(const _K& k, size_type origin) const {
				if(this->rand_probes)
                  return map_hash(randomize(origin));
				return origin; //
			}

			size_type get_e_size() const {
				return (size_type) (get_data_size()/config.BITS_SIZE)+1;
			}

			void free_data(){

				if(clusters) {
					size_type esize = get_e_size();
					for(size_type e = 0; e < get_data_size(); ++e){
                        get_el_allocator().destroy(&keys[e]);
                    }
                    get_el_allocator().deallocate(keys,get_data_size());
					for(size_type c = 0; c < esize; ++c){
						get_segment_allocator().destroy(&clusters[c]);
					}
					get_segment_allocator().deallocate(clusters,get_e_size());
				}

				clusters = nullptr;
			}

			double get_resize_factor() const {
				return key_mapper.resize_factor();
			}

			size_type get_probes() const {
				return this->probes;
			}

			void set_rand_probes(){
				this->rand_probes = this->probes;
			}

			void set_rand_probes(size_type rand_probes){
				this->rand_probes = rand_probes;
			}

			/// clears all data and resize the new data vector to the parameter
			void resize_clear(size_type new_extent){
				/// inverse of factor used to determine overflow list
				/// when overflow list is full rehash starts
				free_data();

				key_mapper = _Mapper(new_extent,config);

				mf = 1.0;
				assert(config.MAX_OVERFLOW_FACTOR > 0);
				if(is_logarithmic()){
                    probes = config.log2(new_extent)*logarithmic;
                    overflow = config.log2(new_extent)*logarithmic;

				}else{
					probes = config.PROBES;
					if (new_extent*sizeof(_ElPair) < 8*8*1024*1024 ) {
                        //probes+=0;
						//overflow = std::max<size_type>(config.PROBES, new_extent / (config.MAX_OVERFLOW_FACTOR/16));
						probes = config.log2(new_extent);
                        overflow = config.log2(new_extent)*8;

					} else {
						overflow = std::max<size_type>(config.PROBES, new_extent / (config.MAX_OVERFLOW_FACTOR));

					}
				}

				rand_probes = 0;

				initial_probes = probes;
				//std::cout << "rehash with overflow:" << overflow  << std::endl;
				elements = 0;
				removed = 0;
				collisions = 0;
				empty_key = _K();
				overflow_elements = get_o_start();
				size_type esize = get_e_size();
				keys = get_el_allocator().allocate(get_data_size());
				clusters = get_segment_allocator().allocate(esize);
				_KeySegment ks;
                _ElPair element;
				for(size_type e = 0; e < get_data_size(); ++e){
					get_el_allocator().construct(&keys[e],element);
				}
				for(size_type c = 0; c < esize; ++c){
					get_segment_allocator().construct(&clusters[c],ks);
				}
				set_exists(get_data_size(),true);
				buckets = 0;

			};

			void clear(){
				size_type esize = get_e_size();
				for(size_type c = 0; c < esize; ++c){
					clusters[c].clear();
				}
				_ElPair element;
				for(size_type e = 0; e < get_data_size(); ++e){
					keys[e] = element;
				}
				set_exists(get_data_size(),true);
				collisions = 0;
				elements = 0;
				removed = 0;
			}

			hash_kernel(const key_compare& compare,const allocator_type& allocator)
			:	clusters(nullptr), eq_f(compare), mf(1.0f), allocator(allocator),logarithmic(config.LOGARITHMIC)
			{
				resize_clear(config.MIN_EXTENT);
			}

			hash_kernel() : clusters(nullptr), mf(1.0f),logarithmic(config.LOGARITHMIC)
			{
				resize_clear(config.MIN_EXTENT);
			}

			hash_kernel(const hash_kernel& right) : clusters(nullptr), mf(1.0f),logarithmic(config.LOGARITHMIC)
			{
				*this = right;
			}

			~hash_kernel(){
				free_data();
			}
			inline size_type get_extent() const {
				return key_mapper.extent;
			}
			void set_logarithmic(size_type loga){
				logarithmic = loga;
			}
			size_type get_logarithmic() const {
				return this->logarithmic;
			}
			bool is_logarithmic() const {
				return this->logarithmic > 0;
			}
			hash_kernel& operator=(const hash_kernel& right){
				config = right.config;
				key_mapper = right.key_mapper;
				free_data();
				buckets = right.buckets;
				removed = right.removed;
				mf = right.mf;
				elements = right.elements;
				collisions = right.collisions;
				size_type esize = get_e_size();
				clusters = get_segment_allocator().allocate(esize);
				keys = right.keys;
				std::copy(clusters, right.clusters, right.clusters+esize);
				return *this;
			}
			inline bool raw_equal_key(size_type pos,const _K& k) const {
				const _K& l = get_key(pos); ///.key(get_segment_index(pos));
				return eq_f(l, k) ;
			}
			inline bool segment_equal_key_exists(size_type pos,const _K& k) const {
				_Bt index = get_segment_index(pos);
				const _Segment& s = get_segment(pos);
				return  eq_f(get_key(pos), k) && s.is_exists(index) ;

			}

			inline bool equal_key(size_type pos,const _K& k) const {
				const _K& l = get_key(pos);
				return eq_f(l, k) ;
			}

			inline size_type randomize(size_type v) const {
				return key_mapper.randomize(v);
			}

            inline size_type hash_probe_incr(size_type base, unsigned int i) const {
                //if(sizeof(_K) > sizeof(unsigned long long)){
                //    return base + i*i + 1;
                //}else{
                    return base + i + 1;
                //}
            }

			_V* subscript_rest(const _K& k, size_type origin)
			RABBIT_NOINLINE_ {
				size_type pos = map_rand_key(k);
				size_type base = pos;
				for(unsigned int i =0; i < probes && pos < get_extent();++i){
					_Bt si = get_segment_index(pos);
					_Segment& s = get_segment(pos);
					if(!s.is_exists(si)){
						s.toggle_exists(si);
						set_segment_key(pos,k);

						++collisions;
						++elements;
						set_overflows(origin, true);
						return create_segment_value(pos);
					}
					pos = hash_probe_incr(base,i);
				}

				size_type at_empty = end();

				if(overflow_elements < end()){
					if(!exists_(overflow_elements)){
						at_empty = overflow_elements++;

					}
				}else if (removed){
					size_type e = end();
					for(pos=get_o_start(); pos < e; ){
						if(!exists_(pos) ) {

							at_empty = pos; break;
						}
						++pos;
					}
				}

				pos = at_empty;
				if(pos != end()){
					set_overflows(origin, true);
					set_exists(pos, true);
					set_segment_key(pos, k);
					size_type os = (overflow_elements - (get_extent()+initial_probes));
					if(os == 1){
                        stats.start_elements = elements;
                        //std::cout << "overflow start: hash table size " << elements << " elements in over flow:" << os << std::endl;
					}

                    if(overflow_elements == end() && stats.start_elements){
                        stats.end_elements = elements;
                        size_type saved = stats.end_elements - stats.start_elements - os;
                        double percent_saved = (100.0*((double)saved/(double)elements));

                       // std::cout << "overflow end: hash table size " << elements << " elements in over flow:" << os << " saved : " << saved <<
                       // std::endl <<  " percent saved " << std::setprecision(4) << percent_saved <<
                       // std::endl;
                    }

					++elements;
					return create_segment_value(pos);

				}
				return nullptr;
			}
			_V* subscript(const _K& k){
				size_type pos = map_key(k);
				_Bt si = get_segment_index(pos);
				_Segment& s = get_segment(pos);

				bool key_exists = s.is_exists(si);
				bool key_overflows = s.is_overflows(si);
				if(!key_exists && !key_overflows){
					s.toggle_exists(si);
					set_segment_key(pos,k);
					++elements;
					return create_segment_value(pos);
				}else if(key_exists && equal_key(pos,k)){
					return &(get_value(pos));
				}
				size_type h = pos;
				if(key_overflows){
					pos = find_rest(k,h);
					if(pos != end()){
						return &(get_value(pos));
					}
				}
				return subscript_rest(k,h);
			}
			size_type erase_rest(const _K& k, size_type origin)
			RABBIT_NOINLINE_ /// this function must never be inlined
			{
				size_type pos = find_rest(k,origin);

				if(pos != (*this).end()){
					set_exists(pos, false);
					++removed;
					set_segment_key(pos,  empty_key);
					destroy_segment_value(pos);
					--elements;
					return 1;
				}
				return 0;
			}
			size_type erase(const _K& k){

				size_type pos = map_key(k);

				_Bt si = get_segment_index(pos);
				_Segment& s = get_segment(pos);
				if(s.is_exists(si) && equal_key(pos,k)){ ///get_segment(pos).exists == ALL_BITS_SET ||
					set_segment_key(pos,  empty_key);
					s.toggle_exists(si);
					destroy_segment_value(pos);
					--elements;
					++removed;
					return 1;
				}
				if(!s.is_overflows(si)){
					return 0;
				}else
					return erase_rest(k, pos);

			}
			/// not used (could be used where hash table must actually shrink too)
			bool is_small() const {
				return (get_extent() > (config.MIN_EXTENT << 3)) && (elements < get_extent()/8);
			}

			size_type count(const _K& k) const {
				size_type pos =(*this).find(k);
				if(pos == (*this).end()){
					return 0;
				}else return 1;
			}
			const _V& at(const _K& k) const {
				size_type pos = find(k);
				if(pos != (*this).end()){
					return get_value(pos);
				}
				throw std::exception();
			}
			_V& direct(size_type pos) {
				return get_value(pos);
			}
			_V& at(const _K& k) {
				size_type pos = find(k);
				if(pos != (*this).end()){
					return get_value(pos);
				}
				throw std::exception();
			}

			bool get(const _K& k, _V& v) const {
				size_type pos = find(k);
				if(pos != (*this).end()){
					v = get_value(pos);
					return true;
				}
				return false;
			}

            /// probabilistic check if key with given hash exists
            /// false indicates the key definitely will not be found
            /// else well have to do a full find

            bool could_have(size_type origin){
                size_type pos = map_hash(origin);
                _Bt index = get_segment_index(origin);
				const _Segment& s = get_segment(origin);
                return (s.is_exists(index) || s.is_overflows(index));
            }

			size_type find_rest_not_empty(const _K& k, size_type origin) const
			RABBIT_NOINLINE_
			{

				/// randomization step for attack mitigation
				size_type pos = map_rand_key(k,origin);
                size_type base = pos;
				for(unsigned int i = 0; i < probes && pos < get_extent();){
					if(equal_key(pos,k)) return pos;
					pos = hash_probe_incr(base,i);
					++i;
				}
				_Bt index = get_segment_index(origin);
				const _Segment& s = get_segment(origin);
				if(!s.is_overflows(index)){
					return end();

				}

				for(pos=get_o_start(); pos < overflow_elements; ){
					if(equal_key(pos,k)) return pos;
					++pos;
				}

				return end();
			}
			size_type find_rest(const _K& k, size_type origin) const
			RABBIT_NOINLINE_
			{
				/// randomization step for attack mitigation
				size_type pos = map_rand_key(k);
                size_type base = pos;
				for(unsigned int i =0; i < probes && pos < get_extent();){//
					_Bt si = get_segment_index(pos);
					if(segment_equal_key_exists(pos,k)){
						return pos;
					}
                    pos = hash_probe_incr(base,i);
					++i;
				}

				for(pos=get_o_start(); pos < overflow_elements; ){
					if(equal_key(pos,k) && exists_(pos) ) return pos;
					++pos;
				}

				return end();
			}
			size_type find_empty(const _K& k, const size_type& unmapped) const
			RABBIT_NOINLINE_
			{
                size_type pos = map_hash(unmapped);
                _Bt index = get_segment_index(pos);
                const _Segment& s = get_segment(pos);
                if(s.is_exists(index) && equal_key(pos,k) ){ ///get_segment(pos).exists == ALL_BITS_SET ||
                    return pos;
                }
                if(!s.is_overflows(index)){
                    return end();
                }
                return find_rest(k, pos);
			}

			inline size_type find_non_empty(const _K& k,const size_type& unmapped)  const {
				size_type pos = map_hash(unmapped);
				if(equal_key(pos,k)) return pos;
				return find_rest_not_empty(k, pos);
			}

			inline size_type find(const _K& k,const size_type& unmapped) const {

				bool is_empty = eq_f(empty_key,k); // && sizeof(_K) <= sizeof(size_type);
				if(is_empty){
                    return find_empty(k, unmapped);
				}else{
					return find_non_empty(k,unmapped);
				}
			}

			size_type find(const _K& k) const {

				size_type pos = hash_key(k);
				return find(k,pos);
			}

			size_type begin() const {
				if(!elements)
					return end();

				size_type pos = 0;
				_Bt index = 0;

				const _Bt bits_size = config.BITS_SIZE;
				size_type e = end();
				while(pos < e){
					const _Segment &seg = get_segment(pos);
					index = get_segment_index(pos);
					if(seg.exists == 0){
						pos += bits_size;
					}else{
						if(seg.is_exists(index))
							break;
						++pos;
					}
				}
				return pos ;
			}
			size_type end() const {

				return get_data_size();
			}
			size_type size() const {
				return elements;
			}
			size_type get_collisions() const {
                return collisions;
			}
			typedef std::shared_ptr<hash_kernel> ptr;
		}; /// hash_kernel
		public:
        struct const_iterator;
        struct iterator {
            typedef hash_kernel* kernel_ptr;
            const basic_unordered_map* h;
            size_type pos;
            friend struct const_iterator;
        protected:
            _Bt index;
            _Bt exists;
            _Bt bsize;
            kernel_ptr get_kernel() const {
				return h->current.get();
            }
            kernel_ptr get_kernel() {
				return h->current.get();
            }

            void increment() {
				++pos;
                ++index;
                if (index == bsize) {
                    auto k = get_kernel();
                    const _Segment& s = k->get_segment(pos);
                    exists = s.exists;
                    index = k->get_segment_index(pos);
                }

            }
        public:
            iterator() : h(nullptr), pos(0) {
            }

            iterator(const basic_unordered_map* h, size_type pos,  _Bt exists, _Bt index, _Bt bsize)
            :  pos(pos), h(h), exists(exists), index(index), bsize(bsize) {

            }

            iterator(const iterator& r) {
                (*this) = r;
            }

            //~iterator() {
            //}

            iterator& operator=(const iterator& r) {
                pos = r.pos;
                h = r.h;
                exists = r.exists;
                index = r.index;
                bsize = r.bsize;
                return (*this);
            }
            inline iterator& operator++() {
                do {
                    increment();
                } while ((exists & (((_Bt)1) << index)) == (_Bt)0);
                return (*this);
            }
            iterator operator++(int) {
                iterator t = (*this);
                ++(*this);
                return t;
            }
			const _ElPair& operator*() const {
				return get_kernel()->get_pair((*this).pos);
			}
			inline _ElPair& operator*() {
			    return get_kernel()->get_pair((*this).pos);
			}
			inline _ElPair* operator->() const {
			    _ElPair* ret = &(get_kernel()->get_pair(pos));
				return ret;
			}
			inline const _ElPair *operator->() {
                _ElPair* ret = &(get_kernel()->get_pair(pos));
				return ret;
			}
            inline bool operator==(const iterator& r) const {

                return (pos == r.pos);
            }
            bool operator!=(const iterator& r) const {

                return (pos != r.pos);
            }

            size_type get_pos() const {
                return pos;
            }

        };

        struct const_iterator {
        private:
            typedef hash_kernel* kernel_ptr;
            const basic_unordered_map* h;
            _Bt index;
            _Bt exists;
			void check_pointer() const {
				return;
				if(h!=nullptr && h->pcurrent != h->current.get()){
					std::cout << "invalid cache pointer: not equal to actual" << std::endl;
				}
			}
            inline kernel_ptr get_kernel() const {
				check_pointer();
                return h->pcurrent;
            }
            inline kernel_ptr get_kernel() {
				check_pointer();
				return const_cast<basic_unordered_map*>(h)->pcurrent; // current.get();
            }

            void increment() {
				++pos;
                ++index;
                auto k = get_kernel();
                if (index == k->config.BITS_SIZE) {
                    const _Segment& s = k->get_segment(pos);
                    exists = s.exists;
                    index = k->get_segment_index(pos);
                }

            }
        public:
            size_type pos;

            const_iterator() : h(nullptr){

            }
            //~const_iterator() {

            //}
            const_iterator
            (   const basic_unordered_map* h,  size_type pos,  _Bt exists, _Bt index)
            :  pos(pos), h(h), exists(exists), index(index){

            }
            const_iterator(const iterator& r) : h(nullptr){
                (*this) = r;
            }

            const_iterator& operator=(const iterator& r) {
                pos = r.pos;
                h = r.h;
                index = r.index;
                exists = r.exists;
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
                return (*this);
            }
            const_iterator operator++(int) {
                const_iterator t = (*this);
                ++(*this);
                return t;
            }
            const _ElPair& operator*() const {
			    return get_kernel()->get_pair(pos);
			}
			const _ElPair *operator->() const {
			  	_ElPair* ret = &(get_kernel()->get_pair(pos));
				return ret;
			}

            inline bool operator==(const const_iterator& r) const {

                return (pos == r.pos);
            }
            bool operator!=(const const_iterator& r) const {

                return (pos != r.pos);
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

		void rehash(){
			size_type to = current->key_mapper.next_size();
			rehash(to);
		}
		void set_current(typename hash_kernel::ptr c){
			current = c;
			pcurrent = c.get();
		}


		typename hash_kernel::ptr current;
		typedef std::vector<typename hash_kernel::ptr> kernel_stack;
		hash_kernel* pcurrent;
		inline void create_current(){
			if(current==nullptr)
				set_current(std::allocate_shared<hash_kernel>(alloc,key_c,alloc));
		}

		iterator from_pos_empty(size_type pos) const {
			return iterator(this, pos, 0, 0, 0);
		}

		iterator from_pos(size_type pos) const {
            const _Segment& s = pcurrent->get_segment(pos);
            _Bt index = pcurrent->get_segment_index(pos);
            _Bt bsize = pcurrent->config.BITS_SIZE;
            return iterator(this,pos,s.exists,index,bsize);
		}

	public:
		float load_factor() const{
			if(current==nullptr) return 0;
			return current->load_factor();
		}
		size_type bucket_count() const {
			if(current==nullptr) return 0;
			return current->bucket_count();
		}
		size_type bucket_size ( size_type n ) const{
			if(current==nullptr) return 0;
			return current->bucket_size ( n );
		}
		float max_load_factor() const {
			if(current==nullptr) return 1;
			return current->max_load_factor();
		}

		void max_load_factor ( float z ){
			create_current();
			current->max_load_factor(z);
		}
		bool empty() const {
			if(current==nullptr) return true;
			return current->size() == 0;
		}
		void reserve(size_type atleast){
			create_current();
			rehash((size_type)((double)atleast*current->get_resize_factor()));
		}
		void resize(size_type atleast){
			create_current();
			rehash(current->key_mapper.nearest_larger(atleast));
		}
		/// called when we dont want pure stl semantics
		void rehash(size_type to_){
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
			try{
               rehashed->set_logarithmic(current->get_logarithmic());
				rehashed->resize_clear(new_extent);
				rehashed->mf = (*this).current->mf;
				//std::cout << " load factor " << current->load_factor() << " for " << current->size() << " elements and collision factor " << current->collision_factor() << std::endl;
				//std::cout << " capacity " << current->capacity() << std::endl;
				if(current->load_factor() < 0.3){
					//std::cout << "possible attack/bad hash detected : using random probes : " << current->get_probes() << std::endl;
					nrand_probes = 1;
					rehashed->set_rand_probes(nrand_probes);
				}
				using namespace std;
				while(true){
					iterator e = end();
					size_type ctr = 0;
					bool rerehashed = false;
					//_K k;
					for(iterator i = begin();i != e;++i){
						//std::swap(k,(*i).first);
						_V* v = reh->subscript((*i).first);
						if(v != nullptr){
							*v = i->second;
							/// a cheap check to illuminate subtle bugs during development
							if(++ctr != rehashed->elements){
								cout << "iterations " << ctr << " elements " << rehashed->elements << " extent " << rehashed->get_extent() << endl;
								cout << "inside rehash " << rehashed->get_extent() << endl;
								cout << "new " << rehashed->elements << " current size:"  << current->elements << endl;
								throw bad_alloc();
							}
						}else{
						    //std::cout << "rehashing in rehash " << ctr << " of " << current->elements << std::endl;
						    rerehashed = true;
                            new_extent = rehashed->key_mapper.next_size();
							rehashed = std::allocate_shared<hash_kernel>(alloc);
							rehashed->resize_clear(new_extent);
							rehashed->mf = (*this).current->mf;
							rehashed->set_rand_probes(nrand_probes);
                            reh = rehashed.get();

                           // i = begin(); // start over
                            //ctr = 0;
							break;

						}
					}
					if(rehashed->elements == current->elements){
						break;
					}else if(!rerehashed){
					    cout << "hash error: unequal key count - retry rehash " << endl;
                        cout << "iterations " << ctr << " elements " << rehashed->elements << " extent " << rehashed->get_extent() << endl;
                        cout << "new " << rehashed->elements << " current size:"  << current->elements << endl;
                        throw bad_alloc();
					}else{
						//cout << "re-rehash iterations " << ctr << " elements " << rehashed->elements << " extent " << rehashed->get_extent() << endl;

						//rehashed->resize_clear(rehashed->get_extent());
						//break;
					}

				}

			}catch(std::bad_alloc &e){
				std::cout << "bad allocation: rehash failed in temp phase :" << new_extent << std::endl;
				size_t t = 0;
				std::cin >> t;
				throw e;
			}
			set_current(rehashed);

		}
		void clear(){
			if(current!=nullptr)
				current->clear();
			pcurrent = nullptr;
			current = nullptr;
			///set_current(std::allocate_shared<hash_kernel>(alloc));
		}

		void clear(const key_compare& compare,const allocator_type& allocator){
			set_current(std::allocate_shared<hash_kernel>(allocator,compare, allocator));
		}

		basic_unordered_map() :current(nullptr),pcurrent(nullptr){
			//
		}

		basic_unordered_map(const key_compare& compare,const allocator_type& allocator) : key_c(compare),alloc(allocator),pcurrent(nullptr){

		}

		basic_unordered_map(const basic_unordered_map& right) {
			*this = right;
		}

		~basic_unordered_map(){

		}

		void swap(basic_unordered_map& with){
			typename hash_kernel::ptr t = with.current;
			with.set_current(this->current);
			this->set_current(t);
		}

		void move(basic_unordered_map& from){
			(*this).set_current(from.current);
			from.set_current(nullptr);
		}

		basic_unordered_map& operator=(const basic_unordered_map& right){
			(*this).set_current(std::allocate_shared<hash_kernel>(alloc));
			(*this).reserve(right.size());
			const_iterator e = right.end();
			for(const_iterator c = right.begin(); c!=e;++c){
				(*this)[(*c).first] = (*c).second;
			}

			return *this;
		}

		hasher hash_function() const {
			return (this->current->hf);
		}

		key_equal key_eq() const {
			if(current!=nullptr)
				return (this->current->eq_f);
			return key_equal();
		}
		iterator insert(const _K& k,const _V& v){
			create_current();
			(*this)[k] = v;
			return from_pos(current->last_modified);
		}

		iterator insert(const std::pair<_K,_V>& p){

			return iterator(this, insert(p.first, p.second));
		}
		/// generic template copy
		template<class _Iter>
		iterator insert(_Iter start, _Iter _afterLast){
			create_current();
			for(_Iter i = start; i != _afterLast; ++i){
				insert((*i).first, (*i).second);
			}
			return from_pos(current->last_modified);
		}
		/// fast getter that doesnt use iterators and doesnt change the table without letting you know
		bool get(const _K& k, _V& v) const {
			if(current!=nullptr)
				return (*this).current->get(k,v);
			return false;
		}
		/// throws a exception when value could not match the key
		const _V& at(const _K& k) const {
			if(current == nullptr) throw std::exception();
			return (*this).current->at(k);
		}
		_V& at(const _K& k) {
			create_current();
			return (*this).current->at(k);
		}

        bool error(const _K& k){
            _V *rv = current->subscript(k);
            return rv==nullptr;
        }

		_V& operator[](const _K& k){
			create_current();
			_V *rv = current->subscript(k);
			while(rv == nullptr){
				this->rehash();
				rv = current->subscript(k);
			}
			return *rv;
		}
		size_type erase(const _K& k){
			if(current==nullptr) return size_type();
			//if(current->is_small()){
			//	rehash(1);
			//}
			return current->erase(k);
		}
		size_type erase(iterator i){
			return erase((*i).first);
		}
		size_type erase(const_iterator i){
			return erase((*i).first);
		}
		size_type count(const _K& k) const {
			if(current == nullptr)return size_type();
			return current->count(k);
		}
		iterator find(const _K& k) const {
			if(current == nullptr) return from_pos_empty(size_type());
			return from_pos(current->find(k));
		}
		iterator begin() const {
			if(current==nullptr) return from_pos_empty(size_type());
			return from_pos(current->begin());
		}
		iterator end() const {
			if(current==nullptr) return from_pos_empty(size_type());
			return from_pos(current->end());
        }
		const_iterator cbegin() const {
			return begin();
		}
		const_iterator cend() const {
			return end();
		}
		size_type size() const {
			if(current==nullptr)return size_type();
			return current->size();
		}
		void set_logarithmic(size_type logarithmic){
			create_current();
			this->current->set_logarithmic(logarithmic);
		}
	};
/// the stl compatible unordered map interface
template
<	class _Kty
,	class _Ty
,	class _Hasher = rabbit_hash<_Kty>
,	class _Keyeq = std::equal_to<_Kty>
,	class _Alloc = std::allocator<std::pair<const _Kty, _Ty> >
,	class _Traits = default_traits
>
class unordered_map :  public basic_unordered_map<_Kty, _Ty, _Hasher, _Keyeq, _Alloc, _Traits>
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
	(	size_type _Buckets
	,	const hasher& _Hasharg
	,	const _Keyeq& _Keyeqarg
	)
	:	_Base(key_compare(_Hasharg, _Keyeqarg), allocator_type())
	{	// construct empty map from hasher and equality comparator
		this->rehash(_Buckets);
	}

	unordered_map
	(	size_type _Buckets
	,	const hasher& _Hasharg
	,	const _Keyeq& _Keyeqarg
	,	const allocator_type& a
	)
	: _Base(key_compare(_Hasharg, _Keyeqarg), a)
	{	// construct empty map from hasher and equality comparator
		this->rehash(_Buckets);
	}

	template<class _Iter>
	unordered_map
	(	_Iter _First
	,	_Iter _Last
	)
	: _Base(key_compare(), allocator_type())
	{	// construct map from sequence, defaults
		_Base::insert(_First, _Last);
	}

	template<class _Iter>
	unordered_map
	(	_Iter _First
	,	_Iter _Last
	,	size_type _Buckets
	)
	: _Base(key_compare(), allocator_type())
	{	// construct map from sequence, ignore initial size
		this->rehash(_Buckets);
		_Base::insert(_First, _Last);
	}

	template<class _Iter>
	unordered_map
	(	_Iter _First
	,	_Iter _Last
	,	size_type _Buckets
	,	const hasher& _Hasharg
	)
	:	_Base(key_compare(_Hasharg), allocator_type())
	{
		this->rehash(_Buckets);
		_Base::insert(_First, _Last);
	}

	template<class _Iter>
	unordered_map
	(	_Iter _First
	,	_Iter _Last
	,	size_type _Buckets
	,	const hasher& _Hasharg
	,	const _Keyeq& _Keyeqarg
	)
	: _Base(key_compare(_Hasharg, _Keyeqarg), allocator_type())
	{
		this->rehash(_Buckets);
		_Base::insert(_First, _Last);
	}

	template<class _Iter>
	unordered_map
	(	_Iter _First
	,	_Iter _Last
	,	size_type _Buckets
	,	const hasher& _Hasharg
	,	const _Keyeq& _Keyeqarg
	,	const allocator_type& _Al
	)
	:	_Base(key_compare(_Hasharg, _Keyeqarg), _Al)
	{
		this->rehash(_Buckets);
		_Base::insert(_First, _Last);
	}

	_Myt& operator=(const _Myt& _Right){	// assign by copying _Right
		_Base::operator=(_Right);
		return (*this);
	}

	unordered_map(_Myt&& from)
	{
		_Base::move(from);
	}

	unordered_map(_Myt&& from, const allocator_type& _Al)
	:	_Base(key_compare(), _Al)
	{	// construct map by moving _Right, allocator
		_Base::move(from);
	}

	_Myt& operator=(_Myt&& from){	// assign by moving _Right
		_Base::move(from);
		return (*this);
	}
	const mapped_type& at(const key_type& k) const {
		return _Base::at(k);
	}

	mapped_type& at(const key_type& k) {
		return _Base::at(k);
	}

	mapped_type& operator[](const key_type& k){
		// find element matching _Keyval or insert with default mapped
		return _Base::operator[](k);
	}

	// find element matching _Keyval or insert with default mapped
	mapped_type& operator[](key_type&& k){
		return (*this)[k];
	}

	void swap(_Myt& _Right){	// exchange contents with non-movable _Right
		_Base::swap(_Right);
	}
};


template
<	class _Kty
,	class _Ty
,	class _Hasher = rabbit_hash<_Kty>
,	class _Keyeq = std::equal_to<_Kty>
,	class _Alloc = std::allocator<std::pair<const _Kty, _Ty> >
,	class _Traits = sparse_traits
>
class sparse_unordered_map :  public unordered_map<_Kty, _Ty, _Hasher, _Keyeq, _Alloc, _Traits>
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
	sparse_unordered_map(){
	}
	~sparse_unordered_map(){
	}
};

}; // rab-bit

#endif ///  _RABBIT_H_CEP_20150303_
