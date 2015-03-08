#ifndef _RABBIT_H_CEP_20150303_
#define  _RABBIT_H_CEP_20150303_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <memory>
/// the rab-bit hash
/// probably the worlds simplest working hashtable - only kiddingk
/// it uses linear probing for the first level of fallback

namespace rabbit{
	template <typename _K, typename _V, typename _H = std::hash<_K>>
	class unordered_map{
	public:
		typedef _K key_type;
		
		typedef _V mapped_type;

		typedef unsigned int _Bt; /// exists ebucket type
		
		typedef std::pair<_K,_V> _ElPair;
		typedef _ElPair value_type;

	protected:
		static const _Bt CHAR_BITS = 8;
		static const _Bt PROBES = 64;
		/// inverse of factor used to determine overflow list
		/// when overflow list is full rehash starts
		static const size_t R_OVERFLOW = 128; 
		
		static const size_t MIN_EXTENT = 17;
		static const _Bt BITS_SIZE = (sizeof(_Bt) * CHAR_BITS);
		/// the existence bit set type
		typedef std::vector<_Bt> _Exists;
		
		/// the vector that will contain the mapping pairs
		typedef std::vector<_ElPair> _Data;
		
		struct hash_state{
			/// the existence bit set is a factor of BITS_SIZE less than the extent
			_Exists exists;
			const _Bt* _exists;
			/// data being used
			_Data data;
			size_t extent;

			size_t elements;
			size_t overflow;
			_H hf;
			
			size_t key2pos(const _K& k) const {
				size_t r = 0;
				size_t h = hf(k);
				
				r = h % extent;						
				
				return r;
			}
			
			size_t get_data_size() const {
				return extent+overflow;
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
			inline bool exists_(size_t pos) const {
				size_t bucket = (pos / BITS_SIZE);
				size_t bit = pos & (BITS_SIZE-1);
				_Bt v = exists[bucket];
				return ((v >> bit) & 1) != 0;
			}
			
			void resize_clear(size_t new_extent){
				extent = new_extent;
				overflow = std::max<size_t>(extent/1024,R_OVERFLOW);
				elements = 0;

				size_t esize = (get_data_size()/BITS_SIZE)+1;
				
				exists.clear();
				data.clear();
				
				exists.resize(esize);
				_exists = &exists[0];
				data.resize(get_data_size());
			};
			void clear(){				
				resize_clear(MIN_EXTENT);
			}
			
			hash_state() {
				clear();
			}
			
			hash_state(const hash_state& right) {
				*this = right;
			}
			
			~hash_state(){
				
			}
			size_t get_extent() const {
				return extent;
			}
			hash_state& operator=(const hash_state& right){
				extent = right.extent;
				exists = right.exists;
				_exists = &exists[0];
				data = right.data;
				elements = right.elements;				
				return *this;
			}
			
			
			_V* subscript(const _K& k){
				size_t pos = 0;
				
				/// eventualy an out of memory exception will occur
				pos = key2pos(k);
				/// this hoist provides performance improvement for rehashes specifically
				/// probably also for new hash tables
				if(!exists_(pos)){
					set_exists(pos, true);
					data[pos].first = k;
					data[pos].second = _V();
					++elements;
					return &(data[pos].second);
				}

				size_t m = std::min<size_t>(extent, pos + PROBES);
				++pos;
				for(; pos < m;++pos){
					if(!exists_(pos)){
						set_exists(pos, true);
						data[pos].first = k;
						data[pos].second = _V();
						++elements;
						return &(data[pos].second);
					}else if(data[pos].first == k){
						return &(data[pos].second);
					}
				}
					
				for(pos = extent; pos < get_data_size();++pos){
					if(!exists_(pos)){
						break;
					}
					if(data[pos].first == k){
							
						return &(data[pos].second);
					}
				};
				if(pos < get_data_size()){
					set_exists(pos,true);
					data[pos].first = k;
					data[pos].second = _V();
					++elements;
					return &(data[pos].second);
					
				}
					
				return nullptr;
			}
			bool erase(const _K& k){
				size_t pos = key2pos(k);			
				size_t m = std::min<size_t>(extent, pos + PROBES);
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
				if(!exists_(pos)) 
					return 0;
				else if(data[pos].first == k){
					return 1;
				}
				size_t m = std::min<size_t>(extent, pos + PROBES);
				++pos;
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
			
			size_t find(const _K& k) const {
				
				size_t pos = key2pos(k);
				size_t m = std::min<size_t>(extent, pos + PROBES);
				for(; pos < m;++pos){
					if(!exists_(pos)){
						return end();
					}else if(data[pos].first == k){
						return pos;
					}
				}
				
				for(pos = extent; pos < get_data_size();++pos){
					if(!exists_(pos)){
						break;
					}
					if(data[pos].first == k){
						return pos;
					}
				}
				return end();
			}
			
			size_t begin() const {
				if(elements==0)
					return end();
				size_t start = 0;
				/// TODO: optimize with bit counting hacks
				while(start < get_data_size()){
					if(exists_(start))
						break;
					++start;
				}
				return start;
			}
			size_t end() const {
				return get_data_size();
			}
			size_t size() const {
				return elements;
			}
			typedef std::shared_ptr<hash_state> ptr;
		}; /// hash_state
		public:
		struct iterator{
			typename hash_state::ptr h;
			size_t pos;
			
			iterator(){
				
			}
			iterator(typename hash_state::ptr h, size_t pos): h(h),pos(pos){
				
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
		double backoff;
		
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
		double recalc_growth_factor(size_t elements)  {
					
			double growth_factor = backoff;
			bool linear = true;
			if(linear){
				double d = 0.65;				
				if(backoff - d > get_min_backoff()){
					backoff -= d ;					
				}								
			}else{								
				double backof_factor = 0.502;
				backoff = get_min_backoff() + (( backoff - get_min_backoff() ) * backof_factor);
				
			}
			
			return growth_factor ;
		}
		
		void rehash(){
			/// can cause oom e because of recursive rehash'es
			_Data temp;
			/// size_t old_extent = extent;
			typename hash_state::ptr rehashed = std::make_shared<hash_state>();
			size_t extent = current->get_extent();
			size_t new_extent = (size_t)(extent * recalc_growth_factor(current->elements)) + 1;
			try{
				rehashed->resize_clear(new_extent);		
				while(true){
					iterator e = end();
					for(iterator i = begin();i != e;++i){
						_V *v = rehashed->subscript((*i).first);
						if(v != nullptr){
							*v = (*i).second;/// rehash recursion can happen here	
						}else{							
							rehashed = std::make_shared<hash_state>();
							new_extent = (size_t)(new_extent * recalc_growth_factor(rehashed->elements)) + 1;
							rehashed->resize_clear(new_extent);				
							continue;
						}
						
					}
					break;
				}
				
			}catch(std::bad_alloc &e){
				///printf("rehash failed in temp phase\n");
				throw e;
			}					
			current = rehashed;	
		}
		typename hash_state::ptr current;
	public:
		
		void clear(){
			backoff = get_max_backoff();
			current = std::make_shared<hash_state>();
		}
		
		unordered_map() {
			clear();
		}
		
		unordered_map(const unordered_map& right) {
			clear();
			*this = right;
		}
		
		~unordered_map(){
			
		}
		
		unordered_map& operator=(const unordered_map& right){
			(*current) = (*right.current);
			return *this;
		}
		
		void insert(const _K& k,const _V& v){
			(*this)[k] = v;
		}
		
		void insert(const std::pair<_K,_V>& p){
			(*this)[p.first] = p.second;
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
			return current->erase(k);
		}
		
		size_t count(const _K& k) const {
			return current->count(k);
		}
		
		iterator find(const _K& k) const {			
			return iterator(current, current->find(k));
		}
		
		iterator begin() const {
			return iterator(current, current->begin());
		}
		iterator end() const {
			return iterator(current, current->end());
		}
		size_t size() const {
			return current->size();
		}
	};
}; // rab-bit

#endif ///  _RABBIT_H_CEP_20150303_