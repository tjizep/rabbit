#pragma once
#include <algorithm>
#include <string>
#include <cstring>
#include <rabbit/unordered_map>

namespace rabbit {

	class int_string {
	public:
		typedef unsigned char size_type;
		typedef unsigned long long string_int;
	private:
		static const size_t D = 3;
		size_type size;
		size_t hash_val;
		string_int ints[D];
		const string_int * end() const {
			return (&ints[D]);
		}
		const string_int * begin() const {
			return (&ints[0]);
		}
		void copy_chars(string_int* dest, const char * src, size_type len) {
			size_type lm = std::min<size_type>(len, sizeof(string_int));
			char * data = (char *)(dest);
			*dest = string_int();
			for (size_type i = size_type(); i < lm; ++i) {
				data[i] = src[i];
			}
		}

		void copy_string(const char * source, size_t l) {
			size_type isize = sizeof(string_int);
			size = (size_type)std::min<size_t>(D*isize-1, l);
			size_type remaining = size;
			string_int * d = ints;
			const char * s = source;
			while (d < end()) {
				size_type todo = std::min<size_type>(remaining, isize);
				copy_chars(d, s, todo);
				s += todo;
				++d;
				remaining -= todo;
			}
			hash_val = fnv_1a_bytes();
		}

		void copy_string(const std::string& s) {
			copy_string(s.c_str(), s.size());
		}

	public:
		int_string()
		:	size(size_type())
		,   hash_val(size_t()){
			for (int i = 0; i < D; ++i)
				ints[i] = string_int();
		}
		int_string(const std::string& s) {
			copy_string(s);
		}
		int_string(const char * s) {
			copy_string(s, std::strlen(s));
		}
		int_string(const int_string& s) {
			*this = s;

		}
		int_string& operator=(const std::string& s) {
			copy_string(s);
			assert(size < sizeof(string_int) * D);
			return *this;
		}
		int_string& operator=(const int_string& s) {
			string_int * d = ints;
			const string_int * rd = s.ints;
			while (d < end()) {
				*d = *rd;
				++d;
				++rd;
			}
			size = s.size;
			hash_val = s.hash_val;
			return *this;
		}
		bool operator==(const int_string& s) const {
			if (size != s.size) return  false;
			const string_int * d = ints;
			const string_int * rd = s.ints;
			while (d < end()) {
				if (*d != *rd)
					return false;
				++d;
				++rd;
			}
			return true;
		}
		bool operator!=(const int_string& s) const {
			return !(*this == s);
		}
		bool operator<(const int_string& s) const {
			const string_int * d = ints;
			const string_int * rd = s.ints;
			while (d < end()) {
				if (*d != *rd)
					return *d < *rd;
				++d;
				++rd;
			}
			return size < s.size;
		}
		const char * c_str() const {
			return (const char *)&ints[0];
		}
		std::string to_string() const {
			return std::string(this->c_str(), size);
		}

		size_t fnv_1a_bytes(const unsigned char *bytes, size_t count) const {
		    const unsigned long long FNV64prime = 0x00000100000001B3ull;
			const unsigned long long FNV64basis = 0xCBF29CE484222325ull;
			size_t r = FNV64basis;
			for (size_t a = 0; a < count; ++a){
				r ^= (size_t)bytes[a]; // folding of one byte at a time
				r *= FNV64prime;
			}
			return r;
		}

		size_t fnv_1a_bytes()const{
            size_t r = size;
			const char * s = c_str();
			return fnv_1a_bytes((const unsigned char *)s,r);
		}

		size_t hash() const {
		    if(hash_val) return hash_val;
		    //return fnv_1a();
			return fnv_1a_bytes();
		}
        size_t bad_hash() const {
			size_t r = size;
			const string_int * i = begin();
			while (i < end()) {
				r += *i;
				++i;
			}
			return r;
		}
	};
	template<>
	struct rabbit_hash <int_string>{
		size_t operator()(const int_string& k) const {
			return k.bad_hash();
		};
	};
};
namespace std {
	template<>
	struct hash <rabbit::int_string> {
		size_t operator()(const rabbit::int_string& k) const {
			return k.bad_hash();
		};
	};
}
