#include <stdlib.h>
#include <stdio.h>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <string>
#include <sstream>

#define _HAS_GOOGLE_HASH_
#ifdef _MSC_VER
#endif
#include <functional>
#include <sparsehash/internal/sparseconfig.h>

#include <rabbit/unordered_map>

#define _HAS_STD_HASH_
#ifdef _HAS_GOOGLE_HASH_
#include <sparsehash/type_traits.h>
#include <sparsehash/dense_hash_map>
#include <sparsehash/sparse_hash_map>
#endif
#include <rabbit/unordered_map>

#ifdef _WIN32
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
namespace conversion{
	void to_t(long long inp, std::string& out){
		#ifdef _MSC_VER
		out = std::to_string(inp);
		#else
		out = dynamic_cast< std::ostringstream & >( ( std::ostringstream() << std::dec << inp ) ).str();
		#endif

	}

	void to_t(long long in, long long& out){
		out = in;
	}

	void to_t(long long in, long& out){
		out = (long)in;
	}

	void to_t(long long in, int& out){
		out = (int)in;
	}

	void to_t(long long in, unsigned long long& out){
		out = (unsigned long long)in;
	}

	void to_t(long long in, unsigned long& out){
		out = (unsigned long)in;
	}

	void to_t(long long in, unsigned int& out){
		out = (unsigned int)in;
	}

	void to_t(long long in, double& out){
		out = (double)in;
	}
	void to_t(long long in, float& out){
		out = (float)in;
	}
};
static const int64_t SEED = 0;
static std::mt19937_64 generator(SEED);
template< typename _T>
class tester{
public:
    typedef long long _ValueType;

	template<typename _MapT>
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
	typedef _T _InputField;
	typedef std::vector<_InputField> _Script;



	void gen_random(size_t count, _Script& script){
		double start = get_proc_mem_use();
		std::minstd_rand rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<long long> dis(1ll<<8, 1ll<<63);
		/// script creation is not benched
		_InputField v;
		for(size_t r = 0; r < count;++r){
			conversion::to_t(dis(gen),v);
			script.push_back(v);
		}
		printf("memory used by script: %.4g MB\n", get_proc_mem_use()-start);
	}

	void gen_random_narrow(size_t count, _Script& script){
		double start = get_proc_mem_use();
		/// script creation is not benched
		_InputField v;
        _Script random_ints(count);
        std::iota(random_ints.begin(), random_ints.end(), 0);
        std::shuffle(random_ints.begin(), random_ints.end(),generator);
        //std::random_shuffle(random_ints.begin(), random_ints.end());
		///for(size_t r = 0; r < count;++r){
		///	conversion::to_t(r,v);
		///	script.push_back(v);
		///}
		//std::random_shuffle(script.begin(), script.end());
		script.swap(random_ints);
		printf("memory used by script: %.4g MB\n", get_proc_mem_use()-start);
	}
	template<typename _MapT>
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
	template<typename _MapT>
	void bench_hash(_MapT& h,const _Script& script){
		/// create a list of random numbers and add to test script
		double mem_start = get_proc_mem_use();
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		typename _MapT::size_type count = (typename _MapT::size_type)script.size();

		typename _MapT::size_type s = count/10;

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
	template<typename _MapT>
	void bench_hash_simple(_MapT& h,const _Script& script){
		/// create a list of random numbers and add to test script
		double mem_start = get_proc_mem_use();
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		size_t count = script.size();

		size_t s = count/10;
		for(size_t j = 0; j < count; ++j){

			h[script[j]] = (typename _MapT::mapped_type)j+1;
			if(j % s == 0){
				//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

				//printf("%ld: %ld in hash, bench total %.4g secs, %.4g MB\n",(long)j,(long)h.size(),(double)(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count())/(1000000.0),get_proc_mem_use()-mem_start);
			}
		}

		printf("%ld in hash, writes total %.4g secs, %.4g MB\n",(long)h.size(),(double)(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count())/(1000000.0),get_proc_mem_use()-mem_start);
		//int t;
		//std::cin >> t;
		std::chrono::steady_clock::time_point start_read = std::chrono::steady_clock::now();
		/// check what is
		for(size_t k = 0; k < count; ++k){
			if(h.count(script[k]) == 0){
				printf("ERROR: could not find %ld\n",(long int)k);
			};
			if(k % s == 0){
				//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

				//printf("%ld: bench read %.4g secs\n",(long)k,(double)(std::chrono::duration_cast<std::chrono::microseconds>(end - start_read).count())/(1000000.0),get_proc_mem_use()-mem_start);
			}
			if(false){
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
		}

		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		printf("time total %.4g secs read %.4g secs. mem used %.4g MB\n",(double)(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count())/(1000000.0),(double)(std::chrono::duration_cast<std::chrono::microseconds>(end - start_read).count())/(1000000.0),get_proc_mem_use()-mem_start);

	}
};
template<typename _T>
void test_dense_hash(typename tester<_T>::_Script& script,size_t ts){
#ifdef _HAS_GOOGLE_HASH_
	printf("google dense hash test\n");
	typedef ::google::dense_hash_map<_T,typename tester<_T>::_ValueType> _Map; //
	_Map h;
	_T c,c1;
	conversion::to_t(-1l,c);
	conversion::to_t(-2l,c1);
	h.set_deleted_key(c);
	h.set_empty_key(c1);
	tester<_T> t;
	t.bench_hash_simple(h,script);
#endif
}
template<typename _T>
void test_sparse_hash(typename tester<_T>::_Script& script,size_t ts){
#ifdef _HAS_GOOGLE_HASH_
	printf("google sparse hash test\n");
	typedef ::google::sparse_hash_map<_T,typename tester<_T>::_ValueType> _Map;
	_Map h;
	tester<_T> t;
	t.bench_hash_simple(h,script);
#endif
}

template<typename _T>
void test_rabbit_hash(typename tester<_T>::_Script& script,size_t ts){
	printf("rabbit hash test\n");
	typedef rabbit::unordered_map<_T,typename tester<_T>::_ValueType> _Map;
	_Map h;

	tester<_T> t;
    h.set_logarithmic(1);
	t.bench_hash_simple(h,script);

}
template<typename _T>
void test_rabbit_sparse_hash(typename tester<_T>::_Script& script,size_t ts){
	printf("rabbit sparse hash test\n");
	typedef rabbit::unordered_map<_T,typename tester<_T>::_ValueType> _Map;
	_Map h;
    h.set_logarithmic(4);
	tester<_T> t;

	t.bench_hash_simple(h,script);

}
template<typename T>
void test_rabbit_hash_erase(size_t ts){
	printf("rabbit hash test\n");
	typedef rabbit::unordered_map<T,typename tester<T>::_ValueType> _Map;
	_Map h;

	typename tester<T>::_Script script;
	tester<T> t;
	t.gen_random(ts, script);
	t.bench_hash(h,script);
	t.erase_test(h,script);

}

template<typename T>
void test_std_hash(typename tester<T>::_Script& script,size_t ts){
#ifdef _HAS_STD_HASH_
	printf("std hash test\n");
	typedef std::unordered_map<T,typename tester<T>::_ValueType> _Map;
	_Map h;

	tester<T> t;

	t.bench_hash_simple(h,script);
#endif
}

extern int google_times(int iters);
//extern int unique_running_insertion();
//extern int unique_scattered_lookup();
void more_tests(){
	//unique_scattered_lookup();
	//unique_running_insertion();
}
void test_random(size_t ts){
    typedef long long _K;
    //typedef std::string _K;

    tester<_K>::_Script script;
    tester<_K> t;
    t.gen_random_narrow(ts, script);

    test_rabbit_hash<_K>(script,ts);
    test_rabbit_sparse_hash<_K>(script,ts);
    //test_rabbit_hash_erase<_K>(ts/10);

    test_dense_hash<_K>(script,ts);

}
int main(int argc, char **argv)
{
#ifdef _MSC_VER
	::Sleep(1000);
#endif
	size_t ts = 40000000;
    test_random(ts);
    //google_times(ts);
	//more_tests();
	return 0;
}
