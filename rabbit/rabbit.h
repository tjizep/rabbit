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
	template <typename _Config>
	struct _ModMapper{
		typedef typename _Config::size_type size_type;
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
		
		/// Truncated Linear Backoff in Rehasing after collisions	
		/// growth factor is calculated as a binary exponential 
		/// backoff (yes, analogous to the one used in network congestion control)
		/// in evidence of hash collisions the the growth factor is exponentialy 
		/// decreased as memory becomes a scarce resource.
		/// a factor between get_min_backoff() and get_max_backoff() is returned by this function
		double recalc_growth_factor(size_type elements)  {
			return 1.9;
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
	
	template <typename _Config>		
	struct _BinMapper{
		typedef typename _Config::size_type size_type;
		size_type extent;
		size_type extent1;
		size_type extent2;
		size_type primary_bits;
		double backoff;
		_Config config;
		_BinMapper(){
		}
		_BinMapper(size_type new_extent,const _Config& config){
			this->config = config;
			this->backoff = get_max_backoff();
			this->extent = 1 << this->config.log2(new_extent);
			
			this->extent1 = this->extent-1;	
			this->extent2 = this->config.log2(new_extent);								
			this->primary_bits = extent2;
			
		}
		
		inline size_type operator()(size_type h) const {										
			return (h+(h>>this->primary_bits)) & this->extent1;///
		}
		
		double get_min_backoff() const {
			return 2;
		}
		double get_max_backoff() const {
			return 8;
		}
		
		/// Truncated Linear Backoff in Rehasing after collisions	
		/// growth factor is calculated as a binary exponential 
		/// backoff (yes, analogous to the one used in network congestion control)
		/// in evidence of hash collisions the the growth factor is exponentialy 
		/// decreased as memory becomes a scarce resource.
		/// a factor between get_min_backoff() and get_max_backoff() is returned by this function
		double recalc_growth_factor(size_type elements)  {
			return 2;
			double growth_factor = backoff;
			bool linear = true;
			if(linear){
				double d = 0.81;				
				if(backoff - d > get_min_backoff()){
					backoff -= d ;					
				}								
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
	struct default_traits{
		typedef unsigned short _Bt; /// exists ebucket type - not using vector<bool> - interface does not support bit bucketing
		typedef unsigned long _Size_Type;
		typedef _Size_Type size_type;
		class rabbit_config{
		public:
			typedef _Size_Type size_type;
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
			/// maximum probes per bucket
			size_type PROBES; /// a value of 32 gives a little more speed but much larger table size(> twice the size in some cases)
			size_type BITS_LOG2_SIZE;
			/// this distributes the h values which are powers of 2 a little to avoid primary clustering when there is no
			///	hash randomizer available 
			
			size_type MIN_EXTENT;
			size_type MIN_OVERFLOW ; 
			
			rabbit_config(const rabbit_config& right){
				*this = right;
			}
			
			rabbit_config& operator=(const rabbit_config& right){
				CHAR_BITS = right.CHAR_BITS;						
				BITS_SIZE = right.BITS_SIZE;
				BITS_SIZE1 = right.BITS_SIZE1;
				BITS_LOG2_SIZE = right.BITS_LOG2_SIZE;
				ALL_BITS_SET = right.ALL_BITS_SET;
				PROBES = right.PROBES; /// a value of 32 gives a little more speed but much larger table size(> twice the size in some cases)
				MIN_EXTENT = right.MIN_EXTENT;
				MIN_OVERFLOW = right.MIN_OVERFLOW; 
				return *this;
			}
			
			rabbit_config(){
				CHAR_BITS = 8;						
				BITS_SIZE = (sizeof(_Bt) * CHAR_BITS);
				BITS_SIZE1 = BITS_SIZE-1;
				BITS_LOG2_SIZE = log2(BITS_SIZE);
				ALL_BITS_SET = ~(_Bt)0;
				PROBES = 256; /// a value of 32 gives a little more speed but much larger table size(> twice the size in some cases)								
				MIN_EXTENT = BITS_SIZE; /// start size of the hash table
				MIN_OVERFLOW = BITS_SIZE*4; 
			}
		};
		typedef _BinMapper<rabbit_config> _Mapper;
	};
	
	template <typename _K, typename _V, typename _H = rabbit_hash<_K>, typename _E = std::equal_to<_K>, typename _Allocator = std::allocator<_K>, typename _Traits = default_traits >
	class unordered_map {
	public:
		typedef _K key_type;
		
		typedef _V mapped_type;

		typedef std::pair<_K,_V> _ElPair;
		typedef _ElPair value_type;
		typedef typename _Traits::_Bt _Bt; /// exists ebucket type - not using vector<bool> - interface does not support bit bucketing
		typedef typename _Traits::size_type size_type;
		typedef typename _Traits::rabbit_config rabbit_config;
		typedef typename _Traits::_Mapper _Mapper;
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
			_Bt erased;		

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
			
			bool is_erased(_Bt bit) const {								
				_Bt r = ((erased >> bit) & (_Bt)1ul);
				return r!=0;
			}
			
			bool is_exists(_Bt bit) const {								
				_Bt r = ((exists >> bit) & (_Bt)1ul);
				return r!=0;
			}
						
			void set_exists(_Bt index, bool f){				
				set_bit(exists,index,f);
			}
			
			void set_erased(_Bt index, bool f){				
				set_bit(erased,index,f);
			}
			
			_PairSegment(){
				exists = 0;
				erased = 0;
			}
			
		} ;

		struct _KeySegment{
		public:
			_Bt exists;			
			_Bt erased;
		private:
			_K keys[sizeof(_Bt)*8];
		private:
			void set_bit(_Bt& w, _Bt index, bool f){
				
				#ifdef _MSC_VER
				#pragma warning(disable:4804)
				#endif								
				_Bt m = (_Bt)1ul << index;// the bit mask				
				w ^= (-f ^ w) & m;
				//w = (w & ~m) | (-f & m);
			}
		public:
			
			inline const _K &key(_Bt ix) const {
				return keys[ix];
			}
			
			inline _K &key(_Bt ix) {
				return keys[ix];
			}
						
			inline bool is_erased(_Bt bit) const {								
				_Bt r = ((erased >> bit) & (_Bt)1ul);
				return r!=0;
			}
			
			inline bool all_erased() const {
				 return (erased == ~(_Bt)0);
			}

			bool is_exists(_Bt bit) const {												
				return ((exists >> bit) & (_Bt)1ul);
			}
									
			void set_exists(_Bt index, bool f){				
				set_bit(exists,index,f);
			}
			
			void set_erased(_Bt index, bool f){				
				set_bit(erased,index,f);
			}
			
			_KeySegment(){
				exists = 0;
				erased = 0;
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

			/// the existence and erased bit sets are a factor of BITS_SIZE+1 less than the extent
			_Segments clusters;///a.k.a. pages
			//_Keys keys;

			_Values values;
			
						
			size_type elements;
			size_type overflow;
			_Mapper key_mapper;
			_H hf;
			_E eq_f;
			float mf;
			size_type buckets;
			mutable size_type min_element;
			size_type removed;

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
				size_type m = std::min<size_type>(get_extent(), pos + config.PROBES);
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
				return get_extent()+overflow;
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
			void set_segment_value(size_type pos, const _V &v) {
				//get_segment(pos).value(get_segment_index(pos)) = v;
				values[pos] = v;
			}
			void set_segment_bit(_Bt& w, _Bt index, bool f){
				
				#ifdef _MSC_VER
				#pragma warning(disable:4804)
				#endif				
								
				_Bt m = (_Bt)1ul << index;// the bit mask
				
				// TODO: not superscalar: add to a traits;
				w ^= (-f ^ w) & m;
				// TODO: OR, for superscalar CPUs: also to traits;
				//w = (w & ~m) | (-f & m);
			}
			
			void set_exists(size_type pos, bool f){
				set_segment_bit(get_segment(pos).exists, get_segment_index(pos), f);
			}

			void set_erased(size_type pos, bool f){
				set_segment_bit(get_segment(pos).erased, get_segment_index(pos), f);				
			}
			
			inline bool exists_(size_type pos) const {
				return get_segment(pos).is_exists(get_segment_index(pos));
			}

			inline bool erased_(size_type pos) const {
				
				const _Segment &s = get_segment(pos);
				/// works because the likelyhood of a lot of erased elements are low
				return  s.erased && s.is_erased(get_segment_index(pos));
			}
			inline size_type map_key(const _K& k) const {
				size_type h = (size_type)_H()(k);
				return key_mapper(h);
			}
			/// clears all data and resize the new data vector to the parameter
			void resize_clear(size_type new_extent){
				/// inverse of factor used to determine overflow list
				/// when overflow list is full rehash starts
				
				key_mapper = _Mapper(new_extent,config); 
				
				mf = 1.0;
				assert(config.MIN_OVERFLOW > 0);
				overflow = config.MIN_OVERFLOW; //std::min<size_type>(get_extent()/1024,config.MIN_OVERFLOW);
				if(overflow < config.MIN_OVERFLOW){
					overflow = config.MIN_OVERFLOW;
				}
				assert(overflow > 0);
				elements = 0;
				removed = 0;
				size_type esize = (get_data_size()/config.BITS_SIZE)+1;
			
				values.clear();
				clusters.clear();

				values.resize(get_data_size());
				
				clusters.resize(esize);
							
				min_element = get_data_size();
				buckets = 0;

			};
		
			void clear(){				
				resize_clear(config.MIN_EXTENT);
			}
			
			hash_kernel() : mf(1.0f){
				clear();
			}
			
			hash_kernel(const hash_kernel& right) : mf(1.0f) {
				*this = right;
			}
			
			~hash_kernel(){
				
			}
			inline size_type get_extent() const {
				return key_mapper.extent;
			}
			
			hash_kernel& operator=(const hash_kernel& right){
				config = right.config;				
				key_mapper = right.key_mapper;
				values = right.values;
				clusters = right.clusters;
				buckets = right.buckets;
				removed = right.removed;
				mf = right.mf;				
				min_element = right.min_element ;
				elements = right.elements;				
				return *this;
			}
			inline bool raw_equal_key(size_type pos,const _K& k) const {
				const _K& l = get_segment_key(pos); ///.key(get_segment_index(pos));
				return eq_f(l, k) ;
			}
			inline bool segment_equal_key(size_type pos,const _K& k) const {
				_Bt index = get_segment_index(pos);
				const _Segment& s = get_segment(pos);
				const _K& l = get_segment_key(pos);
				return eq_f(l, k) && !s.is_erased(index);
			}
			inline bool segment_equal_key_exists(size_type pos,const _K& k) const {
				_Bt index = get_segment_index(pos);
				const _Segment& s = get_segment(pos);
				const _K& l = get_segment_key(pos);
				return eq_f(l, k) && s.is_exists(index) && !s.is_erased(index) ;
			}
			
			bool equal_key(size_type pos,const _K& k) const {				
				_Bt index = get_segment_index(pos);
				const _Segment& s = get_segment(pos);
				const _K& l = get_segment_key(pos);
				return eq_f(l, k) && !s.is_erased(index);
			}

			/// when all inputs to this function is unique relative to current hash map(i.e. they dont exist in the hashmap)
			/// and there where no erasures. for maximum fillrate in rehash
			_V* unique_subscript_rest(const _K& k, size_type pos){
				size_type epos = pos;	
				size_type m = std::min<size_type>(get_extent(), pos + config.PROBES);				
				++pos;				
				for(; pos < m;){
					/*if(get_segment(pos).exists == config.ALL_BITS_SET){						
						pos += (config.BITS_SIZE - (pos & (config.BITS_SIZE1)));
					}else{*/
						if(!exists_(pos)){
							set_exists(pos, true);													
							set_segment_key(pos, k);
							++elements;
							return &(get_segment_value(pos));
						}
						++pos;
					//}
										
				}

				for(pos = get_extent(); pos < get_data_size();++pos){
					if(!exists_(pos)){
						set_exists(pos, true);												
						set_segment_key(pos, k);
						++elements;
						return &(get_segment_value(pos));
					}
				};
				
				return nullptr;
			}
			_V* unique_subscript(const _K& k){
				//if(removed) return subscript(k);

				/// eventualy an out of memory (bad_allocation) exception will occur
				size_type pos = map_key(k);
				_Segment &s = clusters[pos >> config.BITS_LOG2_SIZE];/// get_segment(pos)
				_Bt si = get_segment_index(pos);
				
				if(!s.is_exists(si)){
					s.set_exists(si, true);					
					set_segment_key(pos, k);
					++elements;
					return &(get_segment_value(pos));
				}
				return unique_subscript_rest(k, pos);
			}
			_V* subscript_rest(const _K& k, size_type pos){
				size_type epos = 0;
				epos = pos;
				
				size_type f = 0;
				size_type m = std::min<size_type>(get_extent(), pos + config.PROBES);
				
				++pos;
				
				for(; pos < m;){
					/*if(get_segment(pos).all_erased()){						
						pos += (config.BITS_SIZE - (pos & (config.BITS_SIZE-1)));
					}else{*/
						if(get_segment(pos).exists == 0 || !exists_(pos)){
							set_exists(pos, true);
							set_erased(pos, false);							
							set_segment_key(pos, k);
							set_segment_value(pos, _V());						
							++elements;
							return &(get_segment_value(pos));					
						}else if(!erased_(pos)){
							if (raw_equal_key(pos,k)){
								return &(get_segment_value(pos));
							}
						}
						++pos;
					//}
				}

				
										
				/// this is quite slow so only calc when reaching overflow
				
				size_type last_empty = 0;
				if(elements){
					if(load_factor() > max_load_factor()){				
						return nullptr;
					}		
					for(pos = get_extent(); pos < get_data_size();){
						if(get_segment(pos).exists == 0){
							pos += (config.BITS_SIZE - (pos & (config.BITS_SIZE-1)));
						}else{
							if(!exists_(pos)){
								last_empty = pos;						
							}else if(equal_key(pos,k)){							
								return &(get_segment_value(pos));
							}
							++pos;
						}
					};
				}else{
					last_empty = get_data_size()-1;
				}
				
				/// the key was not found now opurtunistically unerase it
				/// this could be more aggressive to avoid potential rehashes

				if((*this).removed){
					pos = epos;
					if(erased_(pos)){
						set_erased(pos,false); /// un erase the element
						--removed;
						set_exists(pos, true);						
						set_segment_key(pos, k);						
						set_segment_value(pos, _V());
						++elements;
						return &(get_segment_value(pos));
					}
				}
				if(last_empty > 0){
					pos = last_empty;
					if(!exists_(pos)){
						set_exists(pos,true);
						set_erased(pos,false);						
						set_segment_key(pos, k);
						set_segment_value(pos, _V());
						++elements;
						return &(get_segment_value(pos));
					
					}
				}
				return nullptr;
			}
			_V* subscript(const _K& k){
				/// eventualy an out of memory (bad_allocation) exception will occur
				size_type pos = map_key(k);
				_Segment &s = get_segment(pos);
				_Bt si = get_segment_index(pos);
				if(!s.is_exists(si)){
					s.set_exists(si,true);
					set_segment_key(pos, k);					
					set_segment_value(pos,_V());					
					++elements;
					return &(get_segment_value(pos));
				}else if(raw_equal_key(pos,k)){
					return &(get_segment_value(pos));
				}
				return subscript_rest(k, pos);
			}
			bool erase(const _K& k){
				
				size_type pos = (*this).find(k);		
				if(pos != (*this).end()){
					set_erased(pos, true);
					++removed;					
					set_segment_key(pos,  _K());
					set_segment_value(pos, _V());						
					--elements;									
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
				
				size_type m = std::min<size_type>(get_extent(), pos + config.PROBES);
				++pos;
				for(; pos < m;){
					if(get_segment(pos).exists == 0){
						pos += (config.BITS_SIZE - (pos & (config.BITS_SIZE-1)));
					}else if(equal_key(pos,k) && exists_(pos)){
						return pos;
					}else{
						++pos;
					}
				}
				size_type e = get_data_size();
				for(pos = get_extent(); pos < e;++pos){					
					if(exists_(pos) && equal_key(pos,k)){
						return pos;						
					}
				}
				return end();
			}
			size_type find(const _K& k) const {				
				
				size_type pos = map_key(k);				
				if(segment_equal_key_exists(pos,k)){ ///get_segment(pos).exists == ALL_BITS_SET || 
					return pos;
				}				
				if(!elements) return end();
				return find_rest(k,pos);
			}
			
			size_type begin() const {
				if(elements==0)
					return end();
				size_type pos = 0;
				while(!exists_(pos) || erased_(pos)){ 
					++pos;
					min_element = pos;
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
			const unordered_map* h;
			hash_kernel * hc;
			size_type pos;
			
			iterator(){
				
			}
			iterator(const unordered_map* h, size_type pos): h(h),pos(pos){
				 hc = h->current.get();
			}
			iterator(const iterator& r){
				(*this) = r;
			}
			iterator& operator=(const iterator& r){
				h = r.h;
				pos = r.pos;
				hc = r.hc;
				return (*this);
			}
			iterator& operator++(){
				++pos;
				/// todo optimize with 32-bit or 64-bit zero counting
				
				while(pos < hc->get_data_size() && (!hc->exists_(pos) || hc->erased_(pos))){
					++pos;
				}
				
				return (*this);
			}
			iterator operator++(int){
				return (*this);
			}
			const _ElPair operator*() const {
				return const_cast<hash_kernel *>(hc)->get_segment_pair((*this).pos);
			}
			_ElPair& operator*() {
				return hc->get_segment_pair((*this).pos);
			}
			_ElPair* operator->() const {
				return &(const_cast<hash_kernel *>(hc)->get_segment_pair((*this).pos));
			}
			const _ElPair* operator->() {
				return &(hc->get_segment_pair((*this).pos));
			}
			bool operator==(const iterator& r) const {
				return h==r.h&&pos == r.pos;
			}
			bool operator!=(const iterator& r) const {
				return (h!=r.h)||(pos != r.pos);
			}
		};
		
		struct const_iterator{
			const unordered_map* h;
			size_type pos;
			
			const_iterator(){
				
			}
			const_iterator(const unordered_map* h, size_type pos): h(h),pos(pos){
				
			}
			const_iterator(const iterator& r){
				(*this) = r;
			}

			const_iterator& operator=(const iterator& r){
				h = r.h;
				pos = r.pos;
				return (*this);
			}

			const_iterator& operator=(const const_iterator& r){
				h = r.h;
				pos = r.pos;
				return (*this);
			}

			const_iterator& operator++(){
				++pos;
				/// todo optimize with 32-bit or 64-bit zero counting
				while(pos < h->current->get_data_size() && (!h->current->exists_(pos) || h->current->erased_(pos))){
					++pos;
				}
				
				return (*this);
			}
			const_iterator operator++(int){
				return (*this);
			}
			const _ElPair& operator*() const {
				return const_cast<unordered_map*>(h)->current->get_segment_pair(pos);
			}
			const _ElPair* operator->() const {
				return &(const_cast<unordered_map*>(h)->current->get_segment_pair(pos));
			}
			
			bool operator==(const const_iterator& r) const {
				return h==r.h&&pos == r.pos;
			}
			bool operator!=(const const_iterator& r) const {
				return (h!=r.h)||(pos != r.pos);
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
			
			rehash((size_type)((double)atleast*1.3));
		}
		void resize(size_type atleast){
			
			rehash((size_type)((double)atleast*1.3));
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
						_V* v = rehashed->unique_subscript((*i).first);	
						if(v != nullptr){
							*v = (*i).second;
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
		
		unordered_map() {
			clear();
		}
		
		unordered_map(const unordered_map& right) {			
			*this = right;
		}
		
		~unordered_map(){
			
		}
		
		unordered_map& operator=(const unordered_map& right){
			(*this).clear();
			(*this).reserve(right.size());
			const_iterator e = right.end();
			for(const_iterator c = right.begin(); c!=e;++c){
				(*this)[(*c).first] = (*c).second;
			}
			
			return *this;
		}
		
		void insert(const _K& k,const _V& v){
			(*this)[k] = v;
		}
		
		void insert(const std::pair<_K,_V>& p){
			insert(p.first, p.second);
		}

		/// fast getter that doesnt use iterators
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
		size_type size() const {
			return current->size();
		}
	};
	/// the unordered set
	template <typename _K, typename _H = std::hash<_K>>
	class unordered_set{
	protected:
		typedef unordered_map<_K,char,_H> _Container;
	public:
		typedef typename _Container::size_type size_type;
	private:		 
		 _Container container;		 
	public:
		struct iterator{
			
			typename _Container::iterator pos;
			
			iterator(){
				
			}
			iterator(typename _Container::iterator pos): pos(pos){
				
			}
			iterator(const iterator& r){
				(*this) = r;
			}
			iterator& operator=(const iterator& r){
				pos = r.pos;
				return (*this);
			}
			iterator& operator++(){
				++pos;								
				return (*this);
			}
			iterator operator++(int){
				return (*this);
			}
			_K& operator*() const {
				return (*pos).first;
			}
			const _K& operator*() {
				return (*pos).first;
			}
			bool operator==(const iterator& r) const {
				return pos == r.pos;
			}
			bool operator!=(const iterator& r) const {
				return (pos != r.pos);
			}
		};

		struct const_iterator{
			
			typename _Container::const_iterator pos;
			
			const_iterator(){
				
			}
			const_iterator(typename _Container::iterator pos): pos(pos){
				
			}
			const_iterator(const const_iterator& r){
				(*this) = r;
			}
			const_iterator(const iterator& r){
				(*this) = r;
			}
			const_iterator& operator=(const iterator& r){
				pos = r.pos;
				return (*this);
			}
			const_iterator& operator=(const const_iterator& r){
				pos = r.pos;
				return (*this);
			}
			const_iterator& operator++(){
				++pos;								
				return (*this);
			}
			const_iterator operator++(int){
				return (*this);
			}
			const _K& operator*() const {
				return (*pos).first;
			}
			bool operator==(const const_iterator& r) const {
				return pos == r.pos;
			}
			bool operator!=(const const_iterator& r) const {
				return (pos != r.pos);
			}
		};
		
		
		unordered_set(){
		}

		~unordered_set(){
		}
		void rehash(size_type n){		
			container.rehash(n);
		}
		float load_factor() const{
			return container.load_factor() ;
		}

		size_type bucket_count() const {
			container.bucket_count() ;
		}
		size_type bucket_size ( size_type n ) const{
			return container.bucket_size ( n );
		}
		float max_load_factor() const {
			return container.max_load_factor();
		}
	
		void max_load_factor ( float z ){
			container.max_load_factor(z);
		}
		void clear(){
			container.clear();
		}
		
		unordered_set& operator=(const unordered_set& right){
			container = right.container;
			return *this;
		}
		
		void insert(const _K& k){
			container.insert(k,'0');
		}
		
		void erase(const _K& k){
			container.erase(k);
		}
		
		void erase(iterator i){
			container.erase((*i).first);
		}
		
		void erase(const_iterator i){
			container.erase((*i).first);
		}
		
		size_type size() const {
			return container.size();
		}

		bool empty() const {
			return container.empty();
		}
		iterator end() const {
			return container.end();
		}
		iterator begin() const {
			return container.begin();
		}
		iterator find(const _K& k) const {
			return container.find(k);
		}
		
		size_type count(const _K& k) const{
			return container.count(k);
		}
	}; /// unordered set
}; // rab-bit

#endif ///  _RABBIT_H_CEP_20150303_