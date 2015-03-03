#ifndef _RABBIT_H_CEP_20150303_
#define  _RABBIT_H_CEP_20150303_

#include <stdio.h>
#include <iostream>
#include <vector>
#include <chrono>

/// the rab-bit hash
/// probably the worlds simplest working hashtable - only kiddingk
/// it uses linear probing for the first level of fallback

namespace rabbit{
	template <typename _K, typename _V>
	class unordered_map{
	public:
		typedef _K key_type;
		typedef _V value_type;

		typedef unsigned int _Bt; /// exists ebucket type
		
		typedef std::pair<_K,_V> _ElPair;
	struct iterator{
		const unordered_map* h;
		size_t pos;
		
		iterator(){
			
		}
		iterator(const unordered_map* h, size_t pos): h(h),pos(pos){
			
		}
		iterator(const iterator& r){
			(*this) = r;
		}
		iterator& operator=(const iterator& r){
			h = r.h;
			pos = r.pos;
			return (*this);
		}
		iterator& operator++(){
			++pos;
			/// todo optimize with 32-bit or 64-bit zero counting
			while(pos < h->get_data_size() && !h->exists_(pos)){
				++pos;
			}
			
			return (*this);
		}
		iterator operator++(int){
			return (*this);
		}
		_ElPair& operator*() const {
			return const_cast<unordered_map*>(h)->data[pos];
		}
		const _ElPair& operator*() {
			return h->data[pos];
		}
		bool operator==(const iterator& r) const {
			return h==r.h&&pos == r.pos;
		}
		bool operator!=(const iterator& r) const {
			return (h!=r.h)||(pos != r.pos);
		}
	};
	protected:
		static const _Bt CHAR_BITS = 8;
		static const _Bt EXTRA = 32;
		/// inverse of factor used to determine overflow list
		/// when overflow list is full rehash starts
		static const size_t R_OVERFLOW = 16384; 
		
		static const size_t MIN_EXTENT = 17;
		static const _Bt BITS_SIZE = (sizeof(_Bt) * CHAR_BITS);
		double get_min_backoff() const {
			return 2;
		}
		double get_max_backoff() const {
			return 8;
		}
		
		/// the existence bit set type
		typedef std::vector<_Bt> _Exists;
		
		/// the vector that will contain the mapping pairs
		typedef std::vector<_ElPair> _Data;
		
		/// the existence bit set is a factor of BITS_SIZE less than the extent
		_Exists exists;
		
		/// data being used
		_Data data;
		size_t extent;

		size_t elements;
		double backoff;
		size_t key2pos(const _K& k) const {
			size_t r = 0;
			size_t h = (size_t)(k);
			
			r = h % extent;						
			
			return r;
		}
		
		size_t get_data_size() const {
			return extent+extent/R_OVERFLOW;
		}
		
		void set_exists(size_t pos, bool f){
			size_t bucket = pos / BITS_SIZE;
			
			/// lifted directly from Bit Twiddling Hacks
			/// https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetTable
			/// TODO: table wont go over 1<<32
			_Bt m = ((_Bt)pos) % BITS_SIZE; 
			m = (_Bt)1 << m;// the bit mask
			_Bt& w = exists[bucket]; // the word to modify:  
			/// the branching way if (f) w |= m; else w &= ~m; 
			// TODO: not superscalar: add to a traits;
			/// w ^= (-f ^ w) & m;
			// TODO: OR, for superscalar CPUs: also to traits;
			w = (w & ~m) | (-f & m);
		
		}
		
		/// fast generic bit counting
		/// lifted (again) directly from Bit Twiddling Hacks
		/// https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetTable
		template<typename IntT>
		unsigned int count_bits
		(	IntT v	// count bits set in this ((8<=n<=128)-bit value)
		) const {
			/// A generalization of the best bit counting method to integers of 
			/// bit-widths upto 128 (parameterized by type T) is this:
			unsigned int c; // store the total here
			/// ~(IntT)0 is the bitwise maximum of IntT which would be n consecutive true bits
			v = v - ((v >> 1) & (IntT)~(IntT)0/3);                           		// temp
			
			v = (v & (IntT)~(IntT)0/15*3) + ((v >> 2) & (IntT)~(IntT)0/15*3);     	// temp
			v = (v + (v >> 4)) & (IntT)~(IntT)0/255*15;                      		// temp MIN 8 BITS
			c = (IntT)(v * ((IntT)~(IntT)0/255)) >> (sizeof(IntT) - 1) * CHAR_BITS; // count
			return c;
		}
		/// count on bits in the given bucket
		/// each bucket 
		size_t count_bucket(size_t b) const {
			return count_bits<_Bt>(exists[b]);
		}
		bool exists_(size_t pos) const {
			size_t bucket = pos / BITS_SIZE;
			size_t bit = pos % BITS_SIZE;
			_Bt v = exists[bucket];
			return ((v >> bit) & 1) != 0;
		}
		
		void resize_clear(size_t new_extent){
			extent = new_extent;
			elements = 0;
			size_t esize = (get_data_size()/BITS_SIZE)+1;
			
			exists.clear();
			data.clear();
			
			exists.resize(esize);
			data.resize(get_data_size());
		};
		
		/// Truncated Exponential Backoff in Rehasing after collisions	
		/// growth factor is calculated as a binary exponential 
		/// backoff (yes, analogous to the one used in network congestion control)
		/// in evidence of hash collisions the the growth factor is exponentialy 
		/// decreased as memory becomes a scarce resource.
		/// a factor between get_min_backoff() and get_max_backoff() is returned by this function
		double recalc_growth_factor(size_t ext)  {
			double factor = backoff;
			/// a very slow backoff seems very important
			backoff = get_min_backoff() + (( backoff - get_min_backoff() ) * 0.97);
			
			return factor ;
		}
		
		void rehash(){
			/// can cause oom e because of recursive rehash'es
			_Data temp;
			/// size_t old_extent = extent;
			size_t new_extent = extent * recalc_growth_factor(extent) + 1;
			try{
				///temp.reserve(elements);
				///std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
				
				for(iterator i = begin();i != end();++i){
					temp.push_back((*i));
				}
				///std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	
				///printf("rehash scan time %.4g secs\n",(double)(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count())/(1000000.0));
			}catch(std::bad_alloc &e){
				printf("rehash failed in temp phase\n");
				exit(-1);
			}
			try{
				resize_clear(new_extent);
				
			}catch(std::bad_alloc &e){
				
				printf("rehash failed in resize phase\n");
			
				exit(-1);
			}
			
			try{
				for(typename _Data::iterator d = temp.begin(); d != temp.end();++d){
					(*this)[(*d).first] = (*d).second;/// rehash recursion can happen here
				}
					
			}catch(std::bad_alloc &e){
				printf("rehash failed in hash phase\n");
			
				exit(-1);
			}
			
		}
	public:
		
		void clear(){
			backoff = get_max_backoff();
			resize_clear(MIN_EXTENT);
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
			extent = right.extent;
			exists = right.exists;
			data = right.data;
			elements = right.elements;
			backoff = right.backoff;
			return *this;
		}
		
		void insert(const _K& k,const _V& v){
			(*this)[k] = v;
		}
		
		void insert(const std::pair<_K,_V>& p){
			insert(p.first,p.second);
		}
		
		_V& operator[](const _K& k){
			size_t pos = 0;
			while(true){
				/// eventualy an out of memory exception will occur
				pos = key2pos(k);
				size_t m = std::min<size_t>(extent, pos + EXTRA);
				for(; pos < m;++pos){
					if(!exists_(pos)){
						set_exists(pos, true);
						data[pos].first = k;
						data[pos].second = _V();
						++elements;
						return data[pos].second;
					}else if(data[pos].first == k){
						return data[pos].second;
					}
				}
				
				for(pos = extent; pos < get_data_size();++pos){
					if(!exists_(pos)){
						break;
					}
					if(data[pos].first == k){
						
						return data[pos].second;
					}
				};
				if(pos < get_data_size()){
					set_exists(pos,true);
					data[pos].first = k;
					data[pos].second = _V();
					++elements;
					return data[pos].second;
				}
				rehash();
			}
			data[pos].second = _V();
			return data[pos].second;
		}
		bool erase(const _K& k){
			size_t pos = key2pos(k);			
			size_t m = std::min<size_t>(extent, pos + EXTRA);
			for(; pos < m;++pos){
				if(!exists_(pos)){
					return false;
				}else if(data[pos].first == k){
					set_exists(pos, false);
					--elements;
					size_t j = pos+1;
					for(; j < m-1;++j){
						if(!exists_(j)){
							break;
						}
						set_exists(j, exists_(j+1));
						data[j] = data[j+1];						
					}
					set_exists(j, false);
					return true;
				}
			}
			m = get_data_size();
			for(pos = extent; pos < m;++pos){
				if(!exists_(pos)){
					break;
				}
				if(data[pos].first == k){
					size_t j = pos;
					for(; j < m-1;++j){
						if(!exists_(j)){
							break;
						}
						set_exists(j, exists_(j+1));
						data[j] = data[j+1];
						
					}
					set_exists(j, false);
					--elements;						
					return true;
				}				
			};
			return false;
		}
		size_t count(const _K& k) const {
			size_t pos = key2pos(k);
			size_t m = std::min<size_t>(extent, pos + EXTRA);
			for(; pos < m;++pos){
				if(!exists_(pos)) 
					return 0;
				else if(data[pos].first == k){
					return 1;
				}
			}
			
			for(pos = extent; pos < get_data_size();++pos){
				if(!exists_(pos)){
					break;
				}
				if(data[pos].first == k){
					return 1;
				}
			};
			return 0;
		}
		
		iterator find(const _K& k) const {
			
			size_t pos = key2pos(k);
			size_t m = std::min<size_t>(extent, pos + EXTRA);
			for(; pos < m;++pos){
				if(!exists_(pos)){
					return end();
				}else if(data[pos].first == k){
					return iterator(this,pos);
				}
			}
			
			for(pos = extent; pos < get_data_size();++pos){
				if(!exists_(pos)){
					break;
				}
				if(data[pos].first == k){
					return iterator(this,pos);
				}
			}
			return end();
		}
		
		iterator begin() const {
			if(elements==0)
				return end();
			size_t start = 0;
			/// TODO: optimize with bit counting hacks
			while(start < get_data_size()){
				if(exists_(start))
					break;
				++start;
			}
			return iterator(this,start);
		}
		iterator end() const {
			return iterator(this,get_data_size());
		}
		size_t size() const {
			return elements;
		}
	};
}; // rab-bit

#endif ///  _RABBIT_H_CEP_20150303_