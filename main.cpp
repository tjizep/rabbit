#include <stdio.h>
#include <stdlib.h>

#include <random>
#include <unordered_map>
#include <map>

#include "rabbit.h"
template< typename _MapT>
class tester{
public:
	void test_hash(int count){
		
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	
		_MapT h;
		typename _MapT::iterator i = h.begin();
		if(i == h.end()){
			printf("the array is empty\n");
		}
		
		h.insert(std::make_pair(0, 0));
		i = h.begin();
		++i;
		if(i!=h.end()){
			printf("error in iterator\n");
		}
		
		for(int j = 0; j < count; ++j){
			h[j] = j+1;
		}
		
		for(int k = 0; k < count; ++k){
			if(h.count(k) == 0){
				printf("could not find %ld\n",(long int)k);
			};
		}
		
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	
		printf("test too %.4g secs\n",(double)(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count())/(1000000.0));
		int r = h.size();
		std::cin >> r;
		//printf("the hash size is %ld \n",h.size());
		
	}
	typedef typename _MapT::key_type _InputField;
	typedef std::vector<_InputField> _Script;
	void gen_random_ll(size_t count, _Script& script){
		std::minstd_rand rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(1<<24, 1<<31);
		/// script creation is not benched
		for(size_t r = 0; r < count;++r){
			script.push_back(dis(gen));
		}
	}
	void to_t(int in, std::string& out){
		out.clear();
		out.reserve(20);
		char outbuf[20];
		::ltoa(in, outbuf, 10);
		out = outbuf;
		
	}
	
	void to_t(int in, long long& out){
		out = in;
	}
	
	void to_t(int in, long& out){
		out = in;
	}
	
	void to_t(int in, int& out){
		out = in;
	}
	
	void to_t(int in, unsigned long long& out){
		out = in;
	}
	
	void to_t(int in, unsigned long& out){
		out = in;
	}
	
	void to_t(int in, unsigned int& out){
		out = in;
	}
	
	void to_t(int in, double& out){
		out = in;
	}
	void to_t(int in, float& out){
		out = in;
	}

	void gen_random(size_t count, _Script& script){
		std::minstd_rand rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(1<<24, 1<<31);
		/// script creation is not benched
		_InputField v;
		for(size_t r = 0; r < count;++r){
			to_t(dis(gen),v);
			script.push_back(v);
		}
	}
	void erase_test(_MapT &h,const _Script& script){
		for(size_t k = 0; k < count/2; ++k){
			if(h.count(script[k]) != 0){
				if(!h.erase(script[k])){
					printf("ERROR: could not erase %ld\n",(long int)k);
				};
				if(h.count(script[k]) != 0){
					printf("ERROR: erase not reported %ld\n",(long int)k);
				}
			}
		}
		
		for(size_t k = 0; k < count/2; ++k){
			if(h.count(script[k]) != 0){
				printf("ERROR: could find %ld\n",(long int)k);
			};
		}
	}
	void bench_hash(const _Script& script){
		/// create a list of random numbers and add to test script
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		size_t count = script.size();
		_MapT h;
		size_t s = count/30;
		for(size_t j = 0; j < count; ++j){
			h[script[j]] = j+1;
			if(j % s == 0){
				std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	
				printf("%ld: %ld in hash, bench total %.4g secs\n",(long)j,(long)h.size(),(double)(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count())/(1000000.0));
			}
		}
		std::chrono::steady_clock::time_point start_read = std::chrono::steady_clock::now();
		/// check what is
		for(size_t k = 0; k < count; ++k){
			if(h.count(script[k]) == 0){
				printf("ERROR: could not find %ld\n",(long int)k);
			};
		}	
		
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	
		printf("bench total %.4g secs read %.4g secs\n",(double)(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count())/(1000000.0),(double)(std::chrono::duration_cast<std::chrono::microseconds>(end - start_read).count())/(1000000.0));
		int r = h.size();
		std::cin >> r;
		//printf("the hash size is %ld \n",h.size());
		
	}
};
int main(int argc, char **argv)
{
	typedef rabbit::unordered_map<std::string,long> _Map;
	tester<_Map>::_Script script;
	tester<_Map> t;
	t.gen_random(10000000, script);
	t.bench_hash(script);
	
	return 0;
}
