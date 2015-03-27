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
#include <memory>
/// the rab-bit hash
/// probably the worlds simplest working hashtable - only kiddingk
/// it uses linear probing for the first level of fallback

#ifdef _MSC_VER
#define RABBIT_NOINLINE_  _declspec(noinline)
#else
#define RABBIT_NOINLINE_
#endif
namespace rabbit{

	template <typename _K, typename _V, typename _H = std::hash<_K>, typename _E = std::equal_to<_K>, typename _Allocator = std::allocator<_K> >
	class unordered_map{
	public:
		typedef _K key_type;
		
		typedef _V mapped_type;

		typedef unsigned int _Bt; /// exists ebucket type
		
		typedef unsigned int size_type;

		typedef std::pair<_K,_V> _ElPair;
		typedef _ElPair value_type;

	protected:
		static const _Bt CHAR_BITS = 8;
		
		
		
		static const size_type MIN_EXTENT = 64;
		static const _Bt BITS_SIZE = (sizeof(_Bt) * CHAR_BITS);
		/// the existence bit set type
		typedef std::vector<_Bt,_Allocator> _Exists;
		
		/// the vector that will contain the mapping pairs
		typedef std::vector<_ElPair,_Allocator> _Data;
		
		struct hash_state{
			/// maximum probes per bucket
			static const _Bt PROBES = 16;
			
			/// the existence bit set is a factor of BITS_SIZE less than the extent
			_Exists exists;
			_Exists erased;
			const _Bt* _exists;
			/// data being used
			_Data data;
			size_type extent;

			size_type elements;
			size_type overflow;
			_H hf;
			_E eq_f;
			float mf;
			size_type buckets;
			mutable size_type min_element;
			size_type removed;

			float load_factor() const{
				return (float)((double)elements/(double)bucket_count());
			}
			size_type bucket_count() const {
				return get_data_size();
			}
			size_type bucket_size ( size_type n ) const{
				size_type pos = n;
				size_type m = std::min<size_type>(extent, pos + PROBES);
				size_type r = 0;				
				for(; pos < m;++pos){
					if(!exists_(pos)){				
					}else{
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
			
			size_type key2pos(const _K& k) const {
				size_type r = 0;
				size_type h = hf(k);
				
				r = h % extent;						
				
				return r;
			}
			
			size_type get_data_size() const {
				return extent+overflow;
			}
			void set_bucket_bit(_Exists & bits, size_type pos, bool f){
				size_type bucket = pos / BITS_SIZE;
				
				/// lifted directly from Bit Twiddling Hacks
				/// https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetTable
				/// TODO: table wont go over 1<<32
				_Bt m = ((_Bt)pos) % BITS_SIZE; 
				m = (_Bt)1ul << m;// the bit mask
				_Bt& w = bits[bucket]; // the word to modify:  
				/// the branching way 		
				/// if (f) w |= m; else w &= ~m; 
				// TODO: not superscalar: add to a traits;
				///w ^= (-f ^ w) & m;
				// TODO: OR, for superscalar CPUs: also to traits;
				w = (w & ~m) | (-f & m);
			}

			void set_exists(size_type pos, bool f){
				set_bucket_bit(exists, pos, f);				
			}

			void set_erased(size_type pos, bool f){
				set_bucket_bit(erased, pos, f);
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
			size_type count_bucket(size_type b) const {
				return count_bits<_Bt>(exists[b]);
			}
			inline bool is_bit(const _Exists& bits, size_type pos) const {				
				_Bt bit = (_Bt)(pos & (BITS_SIZE-1));
				_Bt v = bits[(pos / BITS_SIZE)];
				_Bt r = ((v >> bit) & (_Bt)1ul);
				return r != 0;
			}
			
			inline bool exists_(size_type pos) const {
				return is_bit(exists, pos);
			}

			inline bool erased_(size_type pos) const {
				return is_bit(erased, pos);
			}
			
			void resize_clear(size_type new_extent){
				/// inverse of factor used to determine overflow list
				/// when overflow list is full rehash starts
				const size_type MIN_OVERFLOW = 64; 
				extent = new_extent;
				mf = 1.0;
				overflow = std::min<size_type>(extent/1024,MIN_OVERFLOW);
				elements = 0;
				removed = 0;
				size_type esize = (get_data_size()/BITS_SIZE)+1;
				
				exists.clear();
				erased.clear();
				data.clear();
				
				erased.resize(esize);
				exists.resize(esize);
				_exists = &exists[0];
				data.resize(get_data_size());
				min_element = get_data_size();
				buckets = 0;

			};
			void clear(){				
				resize_clear(MIN_EXTENT);
			}
			
			hash_state() : mf(1.0f){
				clear();
			}
			
			hash_state(const hash_state& right) : mf(1.0f) {
				*this = right;
			}
			
			~hash_state(){
				
			}
			size_type get_extent() const {
				return extent;
			}
			
			hash_state& operator=(const hash_state& right){
				extent = right.extent;
				exists = right.exists;
				erased = right.erased;
				buckets = right.buckets;
				removed = right.removed;
				mf = right.mf;
				_exists = &exists[0];
				data = right.data;
				min_element = right.min_element ;
				elements = right.elements;				
				return *this;
			}
			inline bool raw_equal_key(size_type pos,const _K& k) const {
				return eq_f(data[pos].first, k) ;
			}
			bool equal_key(size_type pos,const _K& k) const {
				if(removed)
					return raw_equal_key(pos,k) && !erased_(pos);
				else
					return raw_equal_key(pos,k) ;
			}
			_V* subscript(const _K& k){
				
				size_type pos = 0;
				size_type epos = 0;
				/// eventualy an out of memory (bad_allocation) exception will occur
				pos = key2pos(k);
				epos = pos;
				if(!exists_(pos)){
					set_exists(pos, true);
					data[pos].first = k;
					data[pos].second = _V();					
					++elements;
					return &(data[pos].second);
				}else if(equal_key(pos,k)){
					return &(data[pos].second);
				}
				
				size_type f = 0;
				size_type m = std::min<size_t>(extent, pos + PROBES);
				++pos;
				
				for(; pos < m;++pos){
					if(!exists_(pos)){
						set_exists(pos, true);
						data[pos].first = k;
						data[pos].second = _V();						
						++elements;
						return &(data[pos].second);					
					}else if(equal_key(pos,k)){
						return &(data[pos].second);
					}
				}
										
				/// this is quite slow so only calc when reaching overflow
				if(load_factor() > max_load_factor()){				
					return nullptr;
				}	
				size_type last_empty = 0;
				for(pos = extent; pos < get_data_size();++pos){
					if(!exists_(pos)){
						last_empty = pos;
					}else if(equal_key(pos,k)){							
						return &(data[pos].second);
					}
				};
				/// the key was not found now opurtunistically unerase it
				if((*this).removed){
					pos = epos;
					if(erased_(pos)){
						set_erased(pos,false); /// un erase the element
						--removed;
						set_exists(pos, true);
						data[pos].first = k;
						data[pos].second = _V();					
						++elements;
						return &(data[pos].second);
					}
				}
				if(last_empty > 0){
					pos = last_empty;
					if(!exists_(pos)){
						set_exists(pos,true);
						data[pos].first = k;
						data[pos].second = _V();
						++elements;
						return &(data[pos].second);
					
					}else{

					}
				}
				return nullptr;
			}
			bool erase(const _K& k){
				
				size_type pos = (*this).find(k);		
				if(pos != (*this).end()){
					set_erased(pos, true);
					++removed;
					data[pos].first = _K();
					data[pos].second = _V();						
					--elements;									
					return true;
				}
				return false;
				
			}

			bool is_small() const {
				return (extent > (MIN_EXTENT << 3)) && (elements < extent/8);
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
					v = data[pos].second;
					return true;
				}
				return false;
			}
			
			size_type find(const _K& k) const {
				if(!elements) return end();
				size_type pos = key2pos(k);
				if(exists_(pos) && equal_key(pos,k)){
					return pos;
				}
				size_type m = std::min<size_t>(extent, pos + PROBES);
				++pos;
				for(; pos < m;++pos){
					if(exists_(pos) && equal_key(pos,k)){
						return pos;
					}
				}
				size_type e = get_data_size();
				for(pos = extent; pos < e;++pos){
					
					if(exists_(pos) && equal_key(pos,k)){
						return pos;
					}
				}
				return end();
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
			typedef std::shared_ptr<hash_state> ptr;
		}; /// hash_state
		public:
		struct iterator{
			const unordered_map* h;
			size_type pos;
			
			iterator(){
				
			}
			iterator(const unordered_map* h, size_type pos): h(h),pos(pos){
				
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
				while(pos < h->current->get_data_size() && (!h->current->exists_(pos) || h->current->erased_(pos))){
					++pos;
				}
				
				return (*this);
			}
			iterator operator++(int){
				return (*this);
			}
			_ElPair& operator*() const {
				return const_cast<unordered_map*>(h)->current->data[pos];
			}
			const _ElPair& operator*() {
				return h->current->data[pos];
			}
			_ElPair* operator->() const {
				return &(const_cast<unordered_map*>(h)->current->data[pos]);
			}
			const _ElPair* operator->() {
				return &(h->current->data[pos]);
			}
			bool operator==(const iterator& r) const {
				return h==r.h&&pos == r.pos;
			}
			bool operator!=(const iterator& r) const {
				return (h!=r.h)||(pos != r.pos);
			}
		};
		typedef iterator const_iterator;
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
		double recalc_growth_factor(size_type elements)  {
			
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
		
		void rehash(){
			size_type to = (size_t)(current->bucket_count() * recalc_growth_factor(current->elements)) + 1;			
			rehash(to);
		}
		void set_current(typename hash_state::ptr c){
			current = c;
			pcurrent = current.get();
		}
		typename hash_state::ptr current;
		hash_state* pcurrent;
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
			rehash(atleast*2);
		}
		void rehash(size_type to_){
			size_type to = std::max<size_type>(to_, MIN_EXTENT);
			/// can cause oom e because of recursive rehash'es
			_Data temp;
			
			typename hash_state::ptr rehashed = std::make_shared<hash_state>();
			size_type extent = current->get_extent();
			size_type new_extent = to;
			try{
				rehashed->resize_clear(new_extent);						
				rehashed->mf = (*this).current->mf;
				while(true){
					iterator e = end();
					size_type ctr = 0;
					for(iterator i = begin();i != e;++i){					
						_V* v = rehashed->subscript((*i).first);
						v = rehashed->subscript((*i).first);
						if(v != nullptr){
							*v = (*i).second;
						}else{							
							rehashed = std::make_shared<hash_state>();
							new_extent = (size_type)(new_extent * recalc_growth_factor(rehashed->elements)) + 1;
							rehashed->resize_clear(new_extent);				
							rehashed->mf = (*this).current->mf;
							break;
						}												
					}
					if(rehashed->elements == current->elements){
						break;						
					}else{
						rehashed->resize_clear(rehashed->extent);		
					}
					
				}
				
			}catch(std::bad_alloc &e){
				///printf("rehash failed in temp phase\n");
				throw e;
			}			
			set_current(rehashed);	
			
		}
		void clear(){
			backoff = get_max_backoff();
			set_current(std::make_shared<hash_state>());
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
		
		bool get(const _K& k, _V& v) const {
			return (*this).pcurrent->get(k,v);
		}

		_V& operator[](const _K& k){
			_V *rv = pcurrent->subscript(k);
			while(rv == nullptr){
				this->rehash();
				rv = pcurrent->subscript(k);
			}			
			return *rv;
		}
		bool erase(const _K& k){
			//if(current->is_small()){
			//	rehash(1);
			//}
			return pcurrent->erase(k);
		}
		
		size_type count(const _K& k) const {
			return current->count(k);
		}
		
		iterator find(const _K& k) const {			
			return iterator(this, pcurrent->find(k));
		}
		
		iterator begin() const {
			return iterator(this, pcurrent->begin());
		}
		iterator end() const {
			return iterator(this, pcurrent->end());
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

	}; /// unordered set
}; // rab-bit

#endif ///  _RABBIT_H_CEP_20150303_