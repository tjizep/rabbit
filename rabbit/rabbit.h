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

/// the rab-bit hash
/// probably the worlds simplest working hashtable - only kiddingk
/// it uses linear probing for the first level of fallback and then a overflow area or secondary hash

#ifdef _MSC_VER
#define RABBIT_NOINLINE_  _declspec(noinline)
#else
#define RABBIT_NOINLINE_
#endif
namespace rabbit{
	template <class _Config>
	struct _ModMapper{
		typedef _Config config_type;
		typedef typename config_type::size_type size_type;
		size_type extent;
		double backoff;
		_Config config;
		_ModMapper(){
		}
		_ModMapper(size_type new_extent, const _Config& config){
			this->backoff = get_max_backoff();
			this->config = config;
			
			this->extent = new_extent;
			
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
		/// Truncated Linear Backoff in Rehasing after collisions	
		/// growth factor is calculated as a binary exponential 
		/// backoff (yes, analogous to the one used in network congestion control)
		/// in evidence of hash collisions the the growth factor is exponentialy 
		/// decreased as memory becomes a scarce resource.
		/// a factor between get_min_backoff() and get_max_backoff() is returned by this function
		double recalc_growth_factor(size_type elements)  {
			return 1.80;
			double growth_factor = backoff;
			bool linear = true;
			if(linear){
				double d = 0.81;				
				if(backoff - d > get_min_backoff()){
					backoff -= d ;					
				}else backoff = get_min_backoff();				
			}else{								
				double backof_factor = 0.502;
				backoff = get_min_backoff() + (( backoff - get_min_backoff() ) * backof_factor);
				
			}
			
			return growth_factor ;
		}
		inline size_type next_size(){
			double r = recalc_growth_factor(this->extent) * this->extent;
			assert(r > (double)extent);
			return (size_type)r;
		}
	};
	
	template <class _Config>		
	struct _BinMapper{
		typedef typename _Config::size_type size_type;
		typedef _Config config_type;
		size_type extent;
		size_type extent1;
		size_type extent2;
		size_type primary_bits;	
		_Config config;
		_BinMapper(){
		}
		_BinMapper(size_type new_extent,const _Config& config){
			this->config = config;		
			this->extent = ((size_type)1) << this->config.log2(new_extent);
			
			this->extent1 = this->extent-1;	
			this->extent2 = this->config.log2(new_extent);								
			this->primary_bits = extent2;
			
		}
		
		inline size_type operator()(size_type h) const {		

			return (h+(h>>this->primary_bits)) & this->extent1; 

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
		unsigned long operator()(const _Ht& k) const{	
			return std::hash<_Ht>(k);		
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
		unsigned long operator()(const unsigned long& k) const{	
			return k;		
		};
	};
	template<>
	struct rabbit_hash<unsigned int>{	
		unsigned long operator()(const unsigned int& k) const{	
			return (unsigned long)k;		
		};
	};
	template<>
	struct rabbit_hash<int>{	
		unsigned long operator()(const int& k) const{	
			return k;		
		};
	};
	template<>
	struct rabbit_hash<unsigned long long>{	
		unsigned long operator()(const unsigned long long& k) const{	
			return (unsigned long)k;		
		};
	};
	template<>
	struct rabbit_hash<long long>{	
		unsigned long operator()(const long long& k) const{	
			return (unsigned long)k;		
		};
	};
	
	class basic_config{
	public:
		typedef unsigned long long _Bt; /// exists ebucket type - not using vector<bool> - interface does not support bit bucketing
		/// if even more speed is desired but you'r willing to live with a 4 billion key limit then
		/// typedef unsigned long size_type;
		typedef size_t size_type;

		size_type log2(size_type n){
			size_type r = 0; 
			while (n >>= 1) 
			{
				r++;
			}
			return r;
		}
		_Bt CHAR_BITS ;					
		_Bt BITS_SIZE ;
		_Bt BITS_SIZE1 ;			
		_Bt ALL_BITS_SET ;
		/// maximum probes per access
		size_type PROBES; /// a value of 32 gives a little more speed but much larger table size(> twice the size in some cases)			
		size_type BITS_LOG2_SIZE;
		/// this distributes the h values which are powers of 2 a little to avoid primary clustering when there is no
		///	hash randomizer available 
			
		size_type MIN_EXTENT;
		size_type MAX_OVERFLOW ; 
			
		basic_config(const basic_config& right){
			*this = right;
		}
			
		basic_config& operator=(const basic_config& right){
			CHAR_BITS = right.CHAR_BITS;						
			BITS_SIZE = right.BITS_SIZE;
			BITS_SIZE1 = right.BITS_SIZE1;
			BITS_LOG2_SIZE = right.BITS_LOG2_SIZE;
			ALL_BITS_SET = right.ALL_BITS_SET;
			PROBES = right.PROBES; /// a value of 32 gives a little more speed but much larger table size(> twice the size in some cases)
				
			MIN_EXTENT = right.MIN_EXTENT;
			MAX_OVERFLOW = right.MAX_OVERFLOW; 
			return *this;
		}
			
		basic_config(){
			CHAR_BITS = 8;						
			BITS_SIZE = (sizeof(_Bt) * CHAR_BITS);
			BITS_SIZE1 = BITS_SIZE-1;
			BITS_LOG2_SIZE = (size_type) log2((size_type)BITS_SIZE);
			ALL_BITS_SET = ~(_Bt)0;				
			PROBES = 32;							
			MIN_EXTENT = 16; /// start size of the hash table
			MAX_OVERFLOW = 512; //BITS_SIZE*8/sizeof(_Bt); 
			
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
	typedef basic_traits<_BinMapper<basic_config> > default_traits;
	typedef basic_traits<_ModMapper<basic_config> > sparse_traits;

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
		typedef _ElPair value_type;
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
		
		struct _PairSegment{
		private:
			_ElPair data[sizeof(_Bt)*8];								
		private:		
			void set_bit(_Bt& w, _Bt index, bool f){
				
				#ifdef _MSC_VER
				#pragma warning(disable:4804)
				#endif				
				_Bt m = (_Bt)1ul << index;// the bit mask				
				w ^= (-f ^ w) & m;				
			}
		public:
			_Bt exists;
			

			const _ElPair pair(_Bt ix) const {
				return data[ix];				
			}
			
			_ElPair& pair(_Bt ix) {
				return data[ix];
			}
			
			const _K &key(_Bt ix) const {
				return data[ix].first;
			}
			
			_K &key(_Bt ix) {
				return data[ix].first;
			}
			
			const _V &value(_Bt ix) const {
				return data[ix].second;
			}
			
			_V &value(_Bt ix) {
				return data[ix].second;
			}
			
					
			bool is_exists(_Bt bit) const {								
				_Bt r = ((exists >> bit) & (_Bt)1ul);
				return r!=0;
			}
						
			void set_exists(_Bt index, bool f){				
				set_bit(exists,index,f);
			}
			
			
			_PairSegment(){
				exists = 0;
				
			}
			
		} ;

		struct _KeySegment{
		
		public:
			_Bt exists;			
			_Bt overflows;			
		private:
			_K keys[sizeof(_Bt)*8];
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
			
			inline const _K &key(_Bt ix) const {
				return keys[ix];
			}
			
			inline _K &key(_Bt ix) {
				return keys[ix];
			}
		
			inline bool all_exists() const {
				 return (exists == ~(_Bt)0);
			}
			inline bool none_exists() const {
				 return (exists == (_Bt)0);
			}
			inline bool is_exists(_Bt bit) const {												
				return ((exists >> bit) & (_Bt)1ul);
			}
									
			void set_exists(_Bt index, bool f){				
				set_bit(exists,index,f);
			}
			bool is_overflows(_Bt bit) const {												
				return ((overflows >> bit) & (_Bt)1ul);
			}
									
			void set_overflows(_Bt index, bool f){				
				set_bit(overflows,index,f);
			}
			
			_KeySegment(){
				exists = 0;
				overflows = 0;
			}
			
		} ;
		//typedef _PairSegment _Segment;
		typedef _KeySegment _Segment;
		/// the vector that will contain the segmented mapping pairs and flags
		
		typedef std::vector<_Segment, _Allocator> _Segments;
		typedef std::vector<_K, _Allocator> _Keys;
		typedef std::vector<_V, _Allocator> _Values;
		
		struct hash_kernel{
			
			/// settings configuration
			rabbit_config config;
			size_type elements;
			size_type probes;
			size_type last_modified;
			/// the existence bit set is a factor of BITS_SIZE+1 less than the extent
			_Segment* clusters;///a.k.a. pages
			//_Keys keys;
			///_Values values;
			_V* values;

			size_type overflow;
			size_type overflow_elements;
			_Mapper key_mapper;
			_H hf;
			_E eq_f;
			float mf;
			size_type buckets;		
			size_type removed;
			_Allocator allocator;

			typename _Allocator::template rebind<_Segment>::other get_segment_allocator() {
				return typename _Allocator::template rebind<_Segment>::other(allocator) ;
			}
			typename _Allocator::template rebind<_V>::other get_value_allocator(){
				return typename _Allocator::template rebind<_V>::other(allocator) ;
			}
			/// the minimum load factor
			float load_factor() const{
				return (float)((double)elements/(double)bucket_count());
			}

			/// there are a variable ammount of buckets there are at most this much
			///
			size_type bucket_count() const {
				
				return get_data_size();
			}
			/// the size of a bucket can be calculated based on the 
			/// hash value of its first occupant
			/// mainly to satisfy stl conventions
			size_type bucket_size ( size_type n ) const{
				size_type pos = n;
				if(!overflows_(pos) && exists_(pos)){
					return 1;
				}
				size_type m = pos + probes;
				size_type r = 0;				
				for(; pos < m;++pos){
					if(!exists_(pos)){				
					}else if(map_key(get_segment_key(pos)) == n){
						++r;						
					}					
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
				return get_extent()+config.PROBES+overflow;
			}
			
			/// the overflow start
			size_type get_o_start() const {
				return get_extent()+config.PROBES;
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
			
			_ElPair get_segment_pair(size_type pos) const {
				_ElPair r = std::make_pair(get_segment_key(pos),get_segment_value(pos));
				return r;
				//return clusters[get_segment_number(pos)].pair(get_segment_index(pos));
			}
			
			const _K & get_segment_key(size_type pos) const {
				return get_segment(pos).key(get_segment_index(pos));
				//return keys[pos];
			}
			
			const _V & get_segment_value(size_type pos) const {
				//return get_segment(pos).value(get_segment_index(pos));
				return values[pos];
			}
			
			_ElPair rr;
			_ElPair& get_segment_pair(size_type pos) {
				//return clusters[get_segment_number(pos)].pair(get_segment_index(pos));
				rr = std::make_pair(get_segment_key(pos),get_segment_value(pos));
				return rr;
			}			

			_K & get_segment_key(size_type pos) {
				return get_segment(pos).key(get_segment_index(pos));
				//return keys[pos];
			}
			_V & get_segment_value(size_type pos) {
				//return get_segment(pos).value(get_segment_index(pos));
				return values[pos];
			}
			void set_segment_key(size_type pos, const _K &k) {
				get_segment(pos).key(get_segment_index(pos)) = k;
				//keys[pos] = k;
			}
			void destroy_segment_value(size_type pos){
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
				//get_segment(pos).value(get_segment_index(pos)) = v;
				values[pos] = v;
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

			inline size_type map_key(const _K& k) const {
				size_type h = (size_type)_H()(k);
				return key_mapper(h);
			}

			size_type get_e_size() const {
				return (size_type) (get_data_size()/config.BITS_SIZE)+1;
			}

			void free_data(){									
				if(values) {
					
					for(size_type pos = 0; pos < get_data_size(); ++pos){
						if(exists_(pos)){
							get_value_allocator().destroy(&values[pos]);
						}
					}
					get_value_allocator().deallocate(values,get_data_size());				
				}
				if(clusters) {
					size_type esize = get_e_size();
					for(size_type c = 0; c < esize; ++c){
						get_segment_allocator().destroy(&clusters[c]);
					}
					get_segment_allocator().deallocate(clusters,get_e_size());
				}
				values = nullptr;
				clusters = nullptr;
			}
			double get_resize_factor() const {
				return key_mapper.resize_factor();
			}
			/// clears all data and resize the new data vector to the parameter
			void resize_clear(size_type new_extent){
				/// inverse of factor used to determine overflow list
				/// when overflow list is full rehash starts
				free_data();

				key_mapper = _Mapper(new_extent,config); 
				
				mf = 1.0;
				assert(config.MAX_OVERFLOW > 0);
				overflow = std::min<size_type>(get_extent()/config.MAX_OVERFLOW+16,config.MAX_OVERFLOW);
				assert(overflow > 0);
				elements = 0;
				removed = 0;
				probes = config.PROBES; /// start probes
				overflow_elements = get_o_start();

				size_type esize = get_e_size();				
				
				clusters = get_segment_allocator().allocate(esize);				
				values = get_value_allocator().allocate(get_data_size());
			
				for(size_type c = 0; c < esize; ++c){
					get_segment_allocator().construct(&clusters[c]);
				}
				set_exists(get_data_size(),true);
				buckets = 0;

			};
		
			void clear(){				
				resize_clear(config.MIN_EXTENT);
			}

			hash_kernel(const key_compare& compare,const allocator_type& allocator) 
			:	clusters(nullptr), values(nullptr), eq_f(compare), mf(1.0f), allocator(allocator){
				clear();
			}

			hash_kernel() : clusters(nullptr), values(nullptr), mf(1.0f){
				clear();
			}
			
			hash_kernel(const hash_kernel& right) : clusters(nullptr), values(nullptr), mf(1.0f) {
				*this = right;
			}
			
			~hash_kernel(){
				free_data();
			}
			inline size_type get_extent() const {
				return key_mapper.extent;
			}
			
			hash_kernel& operator=(const hash_kernel& right){
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
				
				std::copy(values, right.values, right.values+right.get_data_size());
				std::copy(clusters, right.clusters, right.clusters+esize);
				
				return *this;
			}
			inline bool raw_equal_key(size_type pos,const _K& k) const {
				const _K& l = get_segment_key(pos); ///.key(get_segment_index(pos));
				return eq_f(l, k) ;
			}
			inline bool segment_equal_key(size_type pos,const _K& k) const {
				_Bt index = get_segment_index(pos);
				const _Segment& s = get_segment(pos);
				return eq_f(s.key(index), k) ;
			}
			inline bool segment_equal_key_exists(size_type pos,const _K& k) const {
				_Bt index = get_segment_index(pos);
				const _Segment& s = get_segment(pos);				
				return  eq_f(s.key(index), k) && s.is_exists(index) ;
			}
			inline bool erase_segment_equal_key_exists(size_type pos,const _K& k) {
				_Bt index = get_segment_index(pos);
				const _Segment& s = get_segment(pos);				
				if(eq_f(s.key(index), k) && s.is_exists(index)){
					s.set_exists(index, false);
					return true;
				}
				return false;
			}
			
			bool equal_key(size_type pos,const _K& k) const {				
				const _K& l = get_segment_key(pos);
				return eq_f(l, k) ;
			}

			/// when all inputs to this function is unique relative to current hash map(i.e. they dont exist in the hashmap)
			/// and there where no erasures. for maximum fillrate in rehash
			_V* unique_subscript_rest(const _K& k, size_type pos){
				
				size_type h = pos;
				++pos;
				size_type start = 0;					
				for(unsigned int i =0; i < probes;++i){

					if(!exists_(pos)){
						set_exists(pos, true);												
						set_segment_key(pos, k);
						set_overflows(h, true);
						++elements;						
						last_modified = pos;	
						return create_segment_value(pos);
					}
					++pos;				
				}

				
				if(overflow_elements < end()){
					pos = overflow_elements++;
					probes = config.PROBES + config.log2(overflow_elements)/4;
					if(!exists_(pos)){
						set_overflows(h,true);
						set_exists(pos, true);												
						set_segment_key(pos, k);
						++elements;						
						last_modified = pos;	
						return create_segment_value(pos);
					}
				};
				
				return nullptr;
			}
			_V* unique_subscript(const _K& k){
			
				/// eventualy an out of memory (bad_allocation) exception will occur
				size_type pos = map_key(k);
				_Bt si = get_segment_index(pos);
				_Segment &s = clusters[pos >> config.BITS_LOG2_SIZE];/// get_segment(pos)
								
				if(!s.is_overflows(si) && !s.is_exists(si)){
					s.set_exists(si, true);					
					s.key(si) = k;
					++elements;
					
					return create_segment_value(pos);
				}
				
				return unique_subscript_rest(k, pos);
			}
			_V* subscript_rest(const _K& k, size_type pos,size_type h){
				pos = h;				
				++pos;
				for(unsigned int i =0; i < probes;++i){
					_Bt si = get_segment_index(pos);
					_Segment& s = get_segment(pos);
					if(!s.is_exists(si)){
						s.set_exists(si, true);
						s.key(si)=k;						
						++elements;
						set_overflows(h, true);
						return create_segment_value(pos);
					}
					++pos;
				}

				size_type at_empty = end();				
				
				if(overflow_elements < end()){
					if(!exists_(overflow_elements)){
						at_empty = overflow_elements++;		
						probes = config.PROBES + config.log2(overflow_elements)/4;						
					}
				}else if (removed){
					size_type e = end();					
					for(pos=get_o_start(); pos < e; ){		
						if(!exists_(pos) ) at_empty = pos; break;
						++pos;						
					}
				}
				
				pos = at_empty;		
				if(pos != end()){
					set_overflows(h, true);
					set_exists(pos, true);					
					set_segment_key(pos, k);
					
					++elements;
					return create_segment_value(pos);
					
				}
				return nullptr;
			}
			_V* subscript(const _K& k){
				/// eventualy an out of memory (bad_allocation) exception will occur
				size_type pos = map_key(k);				
				_Bt si = get_segment_index(pos);
				_Segment& s = get_segment(pos);
				bool key_exists = s.is_exists(si);
				bool key_overflow = s.is_overflows(si);
				if(!key_overflow && !key_exists){
					s.set_exists(si, true);
					s.key(si)=k;						
					++elements;
					return create_segment_value(pos);
				}else if(key_exists && eq_f(s.key(si),k)){
					return &(get_segment_value(pos));
				}
				
				size_type h = pos;
				if(key_overflow){
					pos = find_rest(k,pos);
					if(pos != end()){
						return &(get_segment_value(pos));
					}
				}
				return subscript_rest(k,pos,h);
			}
			bool erase(const _K& k){
				
				size_type pos = map_key(k);	
				
				_Bt si = get_segment_index(pos);
				_Segment& s = get_segment(pos);
				bool key_exists = s.is_exists(si);
				bool key_overflow = s.is_overflows(si);

				if(key_exists && eq_f(s.key(si),k)){ ///get_segment(pos).exists == ALL_BITS_SET || 
					set_exists(pos, false);										
					//set_segment_key(pos,  _K());
					//set_segment_value(pos, _V());			
					destroy_segment_value(pos);
					--elements;														
					if(!elements) removed = 0;
					return true;
				}				
				if(!key_overflow){
					return false;
				}
				size_type h = pos;
				
				pos = find_rest(k,pos);

				if(pos != (*this).end()){					
					set_exists(pos, false);
					++removed;					
					//set_segment_key(pos,  _K());
					//set_segment_value(pos, _V());			
					destroy_segment_value(pos);
					--elements;									
					if(pos >= get_o_start()){
						size_type c = get_o_start();
						for(; c < overflow_elements; ++c){		
							if(h == map_key(get_segment_key(c))){
								break;
							}
						}						
					}
					if(!elements) removed = 0;					
					return true;
				}
				return false;
				
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

			bool get(const _K& k, _V& v) const {
				size_type pos = find(k);			
				if(pos != (*this).end()){
					v = get_segment_value(pos);
					return true;
				}
				return false;
			}
			
			size_type find_rest(const _K& k, size_type pos) const {
				size_type h = pos;
				++pos;
				for(unsigned int i =0; i < probes;){
					_Bt si = get_segment_index(pos);
					if(si+8 < config.BITS_SIZE){
						const _Segment& s = get_segment(pos);
						
						if(eq_f(s.key(  si), k) && s.is_exists(si))
							return pos;
						if(eq_f(s.key(++si), k) && s.is_exists(si))
							return pos+1;
						if(eq_f(s.key(++si), k) && s.is_exists(si))
							return pos+2;
						if(eq_f(s.key(++si), k) && s.is_exists(si))
							return pos+3;
						if(eq_f(s.key(++si), k) && s.is_exists(si))
							return pos+4;
						if(eq_f(s.key(++si), k) && s.is_exists(si))
							return pos+5;
						if(eq_f(s.key(++si), k) && s.is_exists(si))
							return pos+6;
						if(eq_f(s.key(++si), k) && s.is_exists(si))
							return pos+7;
					
						i+=8;
						pos+=8;
						
					}else{
				
						if(segment_equal_key_exists(pos,k)){
							return pos;
						}
						
						++pos;
						i+=1;
					}
					
				}
				
				//size_type e = ((overflow_elements/8)+1)*8;
				//if(get_data_size() < e) e = get_data_size(); ///overflow is divisible by 8
				for(pos=get_o_start(); pos < overflow_elements; ){		
					if(equal_key(pos,k) && exists_(pos) ) return pos;											
					++pos;
					if(equal_key(pos,k) && exists_(pos) ) return pos;						
					++pos;
					if(equal_key(pos,k) && exists_(pos) ) return pos;						
					++pos;
					if(equal_key(pos,k) && exists_(pos) ) return pos;						
					++pos;
					if(equal_key(pos,k) && exists_(pos) ) return pos;						
					++pos;
					if(equal_key(pos,k) && exists_(pos) ) return pos;						
					++pos;
					if(equal_key(pos,k) && exists_(pos) ) return pos;						
					++pos;
					if(equal_key(pos,k) && exists_(pos) ) return pos;						
					++pos;
					
				}
				
			
			
				return end();
			}
			size_type find(const _K& k,size_type& pos) const {				
				
				pos = map_key(k);			
				_Bt index = get_segment_index(pos);
				const _Segment& s = get_segment(pos);				
				;
				if(eq_f(s.key(index), k) && s.is_exists(index) ){ ///get_segment(pos).exists == ALL_BITS_SET || 
					return pos;
				}				
				if(!s.is_overflows(index)){
					return end();
				}
				return find_rest(k,pos);
			}
			size_type find(const _K& k) const {				
				
				size_type pos;
				return find(k,pos);
			}
			
			size_type begin() const {
				if(elements==0)
					return end();
				size_type pos = 0;
				while(!exists_(pos)){ 
					++pos;

				}
				return pos ;
			}
			size_type end() const {
				return get_data_size();
			}
			size_type size() const {
				return elements;
			}
			typedef std::shared_ptr<hash_kernel> ptr;
		}; /// hash_kernel
		public:
		struct iterator{
		
			const basic_unordered_map* h;
			hash_kernel * hc;
			size_type pos;
		private:
			_Bt index;
			_Bt exists;
			_Bt bsize;
			void set_index(){
				const _Segment& s = hc->get_segment(pos);
				exists = s.exists;
				index = hc->get_segment_index(pos);
				bsize =  hc->config.BITS_SIZE;
			}
			void check_index(){
				
			}
			void increment(){
				++pos;
				++index;				
				if(index == bsize){
					set_index();	
				}
				
			}
		public:
			iterator(){
				
			}
			iterator(const basic_unordered_map* h, size_type pos): h(h),pos(pos){
				 hc = h->current.get();
				 set_index();
			}
			iterator(const iterator& r){
				(*this) = r;
			}
			iterator& operator=(const iterator& r){
				h = r.h;
				pos = r.pos;
				hc = r.hc;
				set_index();
				return (*this);
			}
			iterator& operator++(){
				increment();
				while( ( exists & (((_Bt)1)<<index) ) == (_Bt)0){
					increment();
				}
				
				return (*this);
			}
			iterator operator++(int){
				return (*this);
			}
			inline _V& get_value(){
				return const_cast<hash_kernel *>(hc)->get_segment_value((*this).pos);
			}
			inline const _V& get_value() const {
				return hc->get_segment_value((*this).pos);
			}
			inline _K& get_key() {
				return const_cast<hash_kernel *>(hc)->get_segment_key((*this).pos);
			}
			inline const _K& get_key() const {
				return hc->get_segment_key((*this).pos);
			}
			const _ElPair operator*() const {
				return const_cast<hash_kernel *>(hc)->get_segment_pair((*this).pos);
			}
			inline _ElPair& operator*() {
				return hc->get_segment_pair((*this).pos);
			}
			inline _ElPair* operator->() const {
				return &(const_cast<hash_kernel *>(hc)->get_segment_pair((*this).pos));
			}
			inline const _ElPair* operator->() {
				return &(hc->get_segment_pair((*this).pos));
			}
			inline bool operator==(const iterator& r) const {
				return (pos == r.pos);
			}
			bool operator!=(const iterator& r) const {
				return (pos != r.pos);
			}
		};
		
		struct const_iterator{
		private:
			const basic_unordered_map* h;
			const hash_kernel * hc;
			_Bt index;
			_Bt exists;
			void set_index(){
				const _Segment& s = hc->get_segment(pos);
				exists = s.exists;
				index = hc->get_segment_index(pos);
			}
			void check_index(){
				
			}
			void increment(){
				++pos;
				++index;				
				if(index == hc->config.BITS_SIZE){
					set_index();	
				}
				
			}
		public:
			size_type pos;
			
			const_iterator(){
				
			}
			const_iterator(const basic_unordered_map* h, size_type pos): h(h),pos(pos){
				 hc = h->current.get();
				 set_index();
			}
			const_iterator(const iterator& r){
				(*this) = r;
			}

			const_iterator& operator=(const iterator& r){
				h = r.h;
				pos = r.pos;
				hc = r.hc;
				set_index();
				return (*this);
			}

			const_iterator& operator=(const const_iterator& r){
				h = r.h;
				pos = r.pos;
				hc = r.hc;
				index = r.index;
				return (*this);
			}

			const_iterator& operator++(){								
				increment();
				while( (exists & (((_Bt)1)<<index)) == (_Bt)0){
					increment();
				}
				
				
				return (*this);
			}
			const_iterator operator++(int){
				return (*this);
			}
			const _ElPair& operator*() const {
				return const_cast<basic_unordered_map*>(h)->current->get_segment_pair(pos);
			}
			const _ElPair* operator->() const {
				return &(const_cast<basic_unordered_map*>(h)->current->get_segment_pair(pos));
			}
			
			bool operator==(const const_iterator& r) const {
				return (pos == r.pos);
			}
			bool operator!=(const const_iterator& r) const {
				return (pos != r.pos);
			}
		};

	protected:
		/// the default config for each hash instance
		rabbit_config default_config;

		
		double recalc_growth_factor(size_type elements)  {
			return 1.8;			
		}
		
		void rehash(){
			size_type to = current->key_mapper.next_size();
			rehash(to);
		}
		void set_current(typename hash_kernel::ptr c){
			current = c;			
		}
		
		typename hash_kernel::ptr current;
	public:
		float load_factor() const{
			return current->load_factor();
		}
		size_type bucket_count() const {
			return current->bucket_count();
		}
		size_type bucket_size ( size_type n ) const{
			return current->bucket_size ( n );
		}
		float max_load_factor() const {
			return current->max_load_factor();
		}
	
		void max_load_factor ( float z ){
			current->max_load_factor(z);
		}
		bool empty() const {
			return (*this).elements == 0;
		}
		void reserve(size_type atleast){
			
			rehash((size_type)((double)atleast*current->get_resize_factor()));
		}
		void resize(size_type atleast){
			
			rehash((size_type)((double)atleast*current->get_resize_factor()));
		}
		void rehash(size_type to_){			
			rabbit_config config;
			size_type to = std::max<size_type>(to_, config.MIN_EXTENT);
			/// can cause oom e because of recursive rehash'es
			
			typename hash_kernel::ptr rehashed = std::make_shared<hash_kernel>();
			size_type extent = current->get_extent();
			size_type new_extent = to;
			try{
				rehashed->resize_clear(new_extent);						
				rehashed->mf = (*this).current->mf;
				using namespace std;
				while(true){
					iterator e = end();
					size_type ctr = 0;
					for(iterator i = begin();i != e;++i){						
						_V* v = rehashed->unique_subscript(i.get_key());	
						if(v != nullptr){
							*v = i.get_value();
							/// a cheap check to illuminate subtle bugs during development
							if(++ctr != rehashed->elements){
								cout << "iterations " << ctr << " elements " << rehashed->elements << " extent " << rehashed->get_extent() << endl;
								cout << "inside rehash " << rehashed->get_extent() << endl;
								cout << "new " << rehashed->elements << " current size:"  << current->elements << endl;		
								throw bad_alloc();
							}
						}else{							
							
							rehashed = make_shared<hash_kernel>();
							new_extent = (size_type)(new_extent * recalc_growth_factor(rehashed->elements)) + 1;
							rehashed->resize_clear(new_extent);				
							rehashed->mf = (*this).current->mf;
							break;
						}																		
					}
					if(rehashed->elements == current->elements){
						break;						
					}else{						
						rehashed->resize_clear(rehashed->get_extent());		
					}
					
				}
				
			}catch(std::bad_alloc &e){
				std::cout << "rehash failed in temp phase :" << new_extent << std::endl;
				size_t t = 0;
				std::cin >> t;
				throw e;
			}			
			set_current(rehashed);	
			
		}
		void clear(){
			
			set_current(std::make_shared<hash_kernel>());
		}
		
		void clear(const key_compare& compare,const allocator_type& allocator){
			set_current(std::make_shared<hash_kernel>(compare, allocator));
		}

		basic_unordered_map() {
			clear();
		}

		basic_unordered_map(const key_compare& compare,const allocator_type& allocator) {
			clear(compare,allocator);
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
			(*this).current = from.current;
			from.current = nullptr;
		}
		
		basic_unordered_map& operator=(const basic_unordered_map& right){
			(*this).clear();
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
			return (this->current->eq_f);
		}
		iterator insert(const _K& k,const _V& v){
			(*this)[k] = v;
			return iterator(this, current->last_modified);
		}
		
		iterator insert(const std::pair<_K,_V>& p){
			return iterator(this, insert(p.first, p.second));
		}
		/// generic template copy
		template<class _Iter>
		iterator insert(_Iter start, _Iter _afterLast){
			for(_Iter i = start; i != _afterLast; ++i){
				insert((*i).first, (*i).second);
			}
			return iterator(this, current->last_modified);
		}
		/// fast getter that doesnt use iterators and doesnt change the table without letting you know
		bool get(const _K& k, _V& v) const {
			return (*this).current->get(k,v);
		}

		_V& operator[](const _K& k){
			_V *rv = current->subscript(k);
			while(rv == nullptr){
				this->rehash();
				rv = current->subscript(k);
			}			
			return *rv;
		}
		bool erase(const _K& k){
			//if(current->is_small()){
			//	rehash(1);
			//}
			return current->erase(k);
		}
		
		bool erase(iterator i){
			return erase((*i).first);
		}
		
		bool erase(const_iterator i){
			return erase((*i).first);
		}
		
		size_type count(const _K& k) const {
			return current->count(k);
		}
		
		iterator find(const _K& k) const {			
			return iterator(this, current->find(k));
		}
		
		iterator begin() const {
			return iterator(this, current->begin());
		}
		iterator end() const {
			return iterator(this, current->end());
		}
		const_iterator cbegin() const {
			return const_iterator(this, current->begin());
		}
		const_iterator cend() const {
			return const_iterator(this, current->end());
		}
		size_type size() const {
			return current->size();
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

/// the unordered set
template <typename _K, typename _H = rabbit_hash<_K>>
class unordered_set : public basic_unordered_map<_K,char,_H>{
protected:
	typedef basic_unordered_map<_K,char,_H> _Container;
public:
		
		
	unordered_set(){
	}

	~unordered_set(){
	}
		
	void insert(const _K& k){
		_Container::insert(k,'0');
	}
		
}; /// unordered set
}; // rab-bit

#endif ///  _RABBIT_H_CEP_20150303_