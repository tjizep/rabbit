#include <stdlib.h>
#include <stdio.h>
#include <random>
#include <unordered_map>
#include <map>
#include <string>
#include <sstream>
#ifdef _MSC_VER
#define _HAS_GOOGLE_HASH_
#endif
#define _HAS_STD_HASH_
#ifdef _HAS_GOOGLE_HASH_
#include <sparsehash/type_traits.h>
#include <sparsehash/dense_hash_map>
#include <sparsehash/sparse_hash_map>
#endif
#include <rabbit/unordered_map>
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <Psapi.h>
double get_proc_mem_use(const double MB = 1024.0*1024.0){
	
	PROCESS_MEMORY_COUNTERS memCounter;
	bool result = (GetProcessMemoryInfo(GetCurrentProcess(),
                                   &memCounter,
                                   sizeof( memCounter )) != 0);
	if(result){
		return memCounter.WorkingSetSize/MB;
	}
	return 0.0;
}
#else
double get_proc_mem_use(const double MB = 1024.0*1024.0){
	return 0.0;
}
#endif
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
	
	void to_t(long long inp, std::string& out){
		#ifdef _MSC_VER
		out = std::to_string(inp);
		#else
		out = dynamic_cast< std::ostringstream & >( ( std::ostringstream() << std::dec << inp ) ).str();
		#endif
		
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
		std::uniform_int_distribution<long long> dis(1<<24, 1ll<<32);
		/// script creation is not benched
		_InputField v;
		for(size_t r = 0; r < count;++r){
			to_t(dis(gen),v);
			script.push_back(v);
		}
		printf("memory used by script: %.4g MB\n", get_proc_mem_use());
	}
	void erase_test(_MapT &h,const _Script& script){
		double mem_start = get_proc_mem_use();
		std::chrono::steady_clock::time_point start_erase = std::chrono::steady_clock::now();
		size_t count = script.size();
		size_t s = count / 10;
		size_t hs = h.size();
		long errors = 0;
		for(size_t k = 0; k < count/2; ++k){
			
			if(h.count(script[k]) != 0){
				if(!h.erase(script[k])){
					printf("ERROR: could not erase %ld\n",(long int)k);
					++errors;
				};				
				if(h.count(script[k]) != 0){
					printf("ERROR: erase not reported %ld\n",(long int)k);				
				}
				if(k % s == 0){
					std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		
					printf("%ld: %ld in hash, test total %.4g secs\n",(long)k,(long)h.size(),(double)(std::chrono::duration_cast<std::chrono::microseconds>(end - start_erase).count())/(1000000.0));
				}
			}
		}
		
		for(size_t k = 0; k < count/2; ++k){
			if(h.count(script[k]) != 0){
				printf("ERROR: could find %ld\n",(long int)k);
				++errors;
			};
		}
		for(size_t k = 0; k < count/2; ++k){
			h[script[k]] = (typename _MapT::mapped_type)k+1;
		}
		if(h.size() != hs){
			++errors;
			printf("ERROR: container invalid size %ld != %ld\n",(long int)h.size(),(long int)hs);
		}
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		
		printf("erase test total %.4g secs. mem used %.4g MB : %ld errors\n",(double)(std::chrono::duration_cast<std::chrono::microseconds>(end - start_erase).count())/(1000000.0),get_proc_mem_use()-mem_start,errors);
		
		
	}
	void bench_hash(_MapT& h,const _Script& script){
		/// create a list of random numbers and add to test script
		double mem_start = get_proc_mem_use();
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		size_t count = script.size();
	
		size_t s = count/10;
		for(size_t j = 0; j < count; ++j){
			
			h[script[j]] = (typename _MapT::mapped_type)j+1;
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
			auto f = h.find(script[k]);
			
			if(f==h.end() || (*f).second != (typename _MapT::mapped_type)k+1){
				if(f==h.end()){					
					printf("ERROR: counted data does not exist %ld\n",(long int)k);
				}else{
					auto n2v = (*f).second-1;
					f = h.find(script[n2v]);
					if(f==h.end() || (*f).second != n2v+1){
						printf("ERROR: could not iterator find %ld\n",(long int)k);
					}					
				}
			}
		}	
		
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		
		printf("time total %.4g secs read %.4g secs. mem used %.4g MB\n",(double)(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count())/(1000000.0),(double)(std::chrono::duration_cast<std::chrono::microseconds>(end - start_read).count())/(1000000.0),get_proc_mem_use()-mem_start);
		
		
		
	}
};

void test_dense_hash_long(size_t ts){
#ifdef _HAS_GOOGLE_HASH_
	printf("google dense hash test\n");
	typedef ::google::dense_hash_map<long,long> _Map;
	_Map h;
	h.set_deleted_key(0l);
	h.set_empty_key(1l);
	tester<_Map>::_Script script;
	tester<_Map> t;
	t.gen_random(ts, script);
	t.bench_hash(h,script);	
#endif
}
template<typename T>
void test_sparse_hash(size_t ts){
#ifdef _HAS_GOOGLE_HASH_
	printf("google sparse hash test\n");
	typedef ::google::sparse_hash_map<T,long> _Map;
	_Map h;
	tester<_Map>::_Script script;
	tester<_Map> t;
	t.gen_random(ts, script);
	t.bench_hash(h,script);	
#endif
}

template<typename T>
void test_rabbit_hash(size_t ts){
	printf("rabbit hash test\n");
	typedef rabbit::unordered_map<T,long> _Map;
	_Map h;
	
	typename tester<_Map>::_Script script;
	tester<_Map> t;
	t.gen_random(ts, script);
	t.bench_hash(h,script);
	
}
template<typename T>
void test_rabbit_hash_erase(size_t ts){
	printf("rabbit hash test\n");
	typedef rabbit::unordered_map<T,long> _Map;
	_Map h;
	
	typename tester<_Map>::_Script script;
	tester<_Map> t;
	t.gen_random(ts, script);
	t.bench_hash(h,script);	
	t.erase_test(h,script);
	
}

template<typename T>
void test_std_hash(size_t ts){
#ifdef _HAS_STD_HASH_
	printf("std hash test\n");
	typedef std::unordered_map<T,long> _Map;
	_Map h;
	
	typename tester<_Map>::_Script script;
	tester<_Map> t;
	t.gen_random(ts, script);
	t.bench_hash(h,script);
#endif
}
#ifdef _MSC_VER
extern int google_times(int iters);
extern int unique_running_insertion();
extern int unique_scattered_lookup();
void more_tests(){
	unique_scattered_lookup();
	unique_running_insertion();
}
#else
 void more_tests(){
 }
#endif
int main(int argc, char **argv)
{
	size_t ts = 10000000;
	google_times(ts);
	if(false){
		//typedef std::string _K;
		typedef unsigned long _K;
		test_dense_hash_long(ts);
		//test_sparse_hash<_K>(ts);


		///test_std_hash<_K>(ts);

	
		test_rabbit_hash<_K>(ts);
		test_rabbit_hash_erase<_K>(ts/10);
	}
	//more_tests();
	return 0;
}
