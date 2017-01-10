#include <numeric>
#include <stdlib.h>
#include <stdio.h>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <string>
#include <sstream>

#include <functional>
#ifdef _HAS_GOOGLE_HASH_
#include <sparsehash/internal/sparseconfig.h>
#endif

#include <rabbit/unordered_map>
#include <rabbit/unordered_set>

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
double get_proc_mem_use(const double MB = 1024.0*1024.0) {


	PROCESS_MEMORY_COUNTERS memCounter;
	bool result = (GetProcessMemoryInfo(GetCurrentProcess(),
		&memCounter,
		sizeof(memCounter)) != 0);
	if (result) {
		return memCounter.WorkingSetSize / MB;
	}

	return 0.0;
}

#else
double get_proc_mem_use(const double MB = 1024.0*1024.0) {
	return 0.0;
}
#endif
namespace conversion {
	void to_t(long long inp, std::string& out) {
#ifdef _MSC_VER
		out = std::to_string(inp);
#else
		out = dynamic_cast< std::ostringstream & >((std::ostringstream() << std::dec << inp)).str();
#endif

	}
	template<typename _In>
	void to_t(_In in, std::string& out) {
		out = std::to_string(in);
	}

    template<typename _In>
	void to_t(_In in, long long& out) {
		out = in;
	}
    template<typename _In>
	void to_t(_In in, long& out) {
		out = (long)in;
	}

    template<typename _In>
	void to_t(_In in, int& out) {
		out = (int)in;
	}
    template<typename _In>
	void to_t(_In in, unsigned long long& out) {
		out = (unsigned long long)in;
	}

    template<typename _In>
	void to_t(_In in, unsigned long& out) {
		out = (unsigned long)in;
	}

	void to_t(const std::string& in, unsigned long& out) {
		out = std::stoul(in);
	}

	void to_t(const std::string& in, long& out) {
		out = std::stol(in);
	}

	void to_t(const std::string& in, long long& out) {
		out = std::stoll(in);
	}

	void to_t(const std::string& in, unsigned long long& out) {
		out = std::stoull(in);
	}

	template<typename _In>
    void to_t(_In in, unsigned int& out) {
		out = (unsigned int)in;
	}

    template<typename _In>
	void to_t(_In in, double& out) {
		out = (double)in;
	}

    template<typename _In>
	void to_t(_In in, float& out) {
		out = (float)in;
	}
};
static const int64_t SEED = 13;
static std::mt19937_64 generator(SEED);


template< class _T,class _V>
class tester {
public:
	typedef _V _ValueType;

	typedef _T _InputField;
	typedef std::vector<_InputField> _Script;
	void gen_random(size_t count, _Script& script) {
		double start = get_proc_mem_use();
		//std::minstd_rand rd;
		std::mt19937 gen(6);

		std::uniform_int_distribution<_ValueType> dis(std::numeric_limits<_ValueType>::min(), std::numeric_limits<_ValueType>::max());
		/// script creation is not benched
		_InputField v;
		for (size_t r = 0; r < count; ++r) {
			conversion::to_t(dis(gen), v);//dis(gen)
			script.push_back(v);
		}
		printf("memory used by script: %.4g MB\n", get_proc_mem_use() - start);
	}

	void gen_random_narrow(size_t count, _Script& script) {
		double start = get_proc_mem_use();
		/// script creation is not benched
		_InputField v;
		for (size_t r = 0; r < count; ++r) {
			conversion::to_t(r, v);
			script.push_back(v);
		}
		std::shuffle(script.begin(), script.end(), generator);
		printf("memory used by script: %.4g MB\n", get_proc_mem_use() - start);
	}
	void gen_random_narrowest(size_t count, _Script& script) {
		double start = get_proc_mem_use();
		/// script creation is not benched
		_InputField v;
		for (size_t r = 0; r < count; ++r) {
			conversion::to_t(r, v);
			script.push_back(v);
		}
		std::random_shuffle(script.begin(), script.end());
		printf("memory used by script: %.4g MB\n", get_proc_mem_use() - start);
	}
	void gen_seq(size_t count, _Script& script) {
		double start = get_proc_mem_use();
		/// script creation is not benched
		_InputField v;
		for (size_t r = 0; r < count; ++r) {
			conversion::to_t(r, v);
			script.push_back(v);
		}
		printf("memory used by script: %.4g MB\n", get_proc_mem_use() - start);
	}
	template<typename _MapT>
	void erase_test(_MapT &h, const _Script& script) {
		double mem_start = get_proc_mem_use();
		std::chrono::steady_clock::time_point start_erase = std::chrono::steady_clock::now();
		size_t count = script.size();
		size_t s = count / 10;
		size_t hs = h.size();
		long errors = 0;
		for (size_t k = 0; k < count / 2; ++k) {
            auto f = h.find(script[k]);
			if (f != h.end() && f->second == k+1) {
				if (!h.erase(script[k])) {
					printf("ERROR: could not erase %ld\n", (long int)k);
					++errors;
				};
				if (h.count(script[k]) != 0) {
					printf("ERROR: erase not reported %ld\n", (long int)k);
				}
				if (k % s == 0) {
					std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

					printf("%ld: %ld in hash, test total %.4g secs\n", (long)k, (long)h.size(), (double)(std::chrono::duration_cast<std::chrono::microseconds>(end - start_erase).count()) / (1000000.0));
				}
			}
		}

		for (size_t k = 0; k < count / 2; ++k) {
			if (h.count(script[k]) != 0) {
				printf("ERROR: could find %ld\n", (long int)k);
				++errors;
			};
		}
		for (size_t k = 0; k < count / 2; ++k) {
			h[script[k]] = (typename _MapT::mapped_type)k + 1;
		}
		if (h.size() != hs) {
			++errors;
			printf("ERROR: container invalid size %ld != %ld\n", (long int)h.size(), (long int)hs);
		}
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		printf("erase test total %.4g secs. mem used %.4g MB : %ld errors\n", (double)(std::chrono::duration_cast<std::chrono::microseconds>(end - start_erase).count()) / (1000000.0), get_proc_mem_use() - mem_start, errors);


	}
	template<typename _MapT>
	void bench_hash(_MapT& h, const _Script& script) {
		/// create a list of random numbers and add to test script
		double mem_start = get_proc_mem_use();
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		typename _MapT::size_type count = (typename _MapT::size_type)script.size();

		typename _MapT::size_type s = count / 10;

		for (size_t j = 0; j < count; ++j) {
			h[script[j]] = j;
			if (j % s == 0) {
				std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

				printf("%ld: %ld in hash, bench total %.4g secs\n", (long)j, (long)h.size(), (double)(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) / (1000000.0));
			}
		}
		std::chrono::steady_clock::time_point start_read = std::chrono::steady_clock::now();
		/// check what is
		for (size_t k = 0; k < count; ++k) {
			if (h.count(script[k]) == 0) {
				printf("ERROR: could not find %ld\n", (long int)k);
			};
			auto f = h.find(script[k]);

			if (f == h.end() || (*f).second != k) {
				if (f == h.end()) {
					printf("ERROR: counted data does not exist %ld\n", (long int)k);
				}
				else {
					auto n2v = script[k];
					f = h.find(script[n2v]);
					if (f == h.end() || (*f).second != n2v) {
						printf("ERROR: could not iterator find %ld\n", (long int)k);
					}
				}
			}
		}

		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		printf("time total %.4g secs read %.4g secs. mem used %.4g MB\n", (double)(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) / (1000000.0), (double)(std::chrono::duration_cast<std::chrono::microseconds>(end - start_read).count()) / (1000000.0), get_proc_mem_use() - mem_start);



	}
	template<typename _MapT>
	void bench_hash_simple(_MapT& h, const _Script& script) {
		/// create a list of random numbers and add to test script
		double mem_start = get_proc_mem_use();
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		size_t count = script.size();

		size_t s = count / 10;
		_V value;
		for (size_t j = 0; j < count; ++j) {

			conversion::to_t(script[j],value);
			h[script[j]] = value;
			if (j % s == 0) {
				//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

				//printf("%ld: %ld in hash, bench total %.4g secs, %.4g MB\n",(long)j,(long)h.size(),(double)(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count())/(1000000.0),get_proc_mem_use()-mem_start);
			}
		}

		printf("%ld in hash, writes total %.4g secs, %.4g MB\n", (long)h.size(), (double)(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count()) / (1000000.0), get_proc_mem_use() - mem_start);
		//int t;
		//std::cin >> t;
		std::chrono::steady_clock::time_point start_read = std::chrono::steady_clock::now();
		/// check what is
		for (size_t k = 0; k < count; ++k) {
            auto f = h.find(script[k]);
            conversion::to_t(script[k],value);
			if (f == h.end() || f->second != value) {
				printf("ERROR: could not find %ld\n", (long int)k);
			}

			if (k % s == 0) {
				//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

				//printf("%ld: bench read %.4g secs\n",(long)k,(double)(std::chrono::duration_cast<std::chrono::microseconds>(end - start_read).count())/(1000000.0),get_proc_mem_use()-mem_start);
			}
        }

		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		printf("time total %.4g secs read %.4g secs. mem used %.4g MB\n", (double)(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) / (1000000.0), (double)(std::chrono::duration_cast<std::chrono::microseconds>(end - start_read).count()) / (1000000.0), get_proc_mem_use() - mem_start);

	}
};
template<typename _T,typename _V>
void test_dense_hash(typename tester<_T,_V>::_Script& script, size_t ts) {
#ifdef _HAS_GOOGLE_HASH_
	printf("google dense hash test\n");
	typedef ::google::dense_hash_map<_T, typename tester<_T,_V>::_ValueType> _Map; //
	_Map h;
	_T c, c1;
	conversion::to_t(-1l, c);
	conversion::to_t(-2l, c1);
	h.set_deleted_key(c);
	h.set_empty_key(c1);
	tester<_T,_V> t;
	t.bench_hash_simple(h, script);
#endif
}

template<typename _T,typename _V>
void test_sparse_hash(typename tester<_T,_V>::_Script& script, size_t ts) {
#ifdef _HAS_GOOGLE_HASH_
	printf("google sparse hash test\n");
	typedef ::google::sparse_hash_map<_T, typename tester<_T,_V>::_ValueType> _Map;
	_Map h;
	tester<_T,_V> t;
	t.bench_hash_simple(h, script);
#endif
}

template<typename _T,typename _V>
void test_rabbit_hash(typename tester<_T,_V>::_Script& script, size_t ts) {
	printf("rabbit hash test\n");
	typedef rabbit::unordered_map<_T, typename tester<_T,_V>::_ValueType, std::hash<_T> > _Map;
	_Map h;
    //h.set_logarithmic(1);
	tester<_T,_V> t;

	t.bench_hash_simple(h, script);

}
template<typename _T, typename _V>
void test_rabbit_sparse_hash(typename tester<_T,_V>::_Script& script, size_t ts) {
	printf("rabbit sparse hash test\n");
	typedef rabbit::sparse_unordered_map<_T, typename tester<_T,_V>::_ValueType> _Map;
	_Map h;
	tester<_T,_V> t;
	t.bench_hash_simple(h, script);

}
template<typename T,typename _V>
void test_rabbit_hash_erase(size_t ts) {
	printf("rabbit hash test\n");
	typedef rabbit::unordered_map<T, typename tester<T,_V>::_ValueType, std::hash<T> > _Map;
	_Map h;
    typename tester<T,_V>::_Script script;
	tester<T,_V> t;
	t.gen_random(ts, script);
	t.bench_hash(h, script);
	t.erase_test(h, script);

}

template<typename T,typename _V>
void test_std_hash(typename tester<T,_V>::_Script& script, size_t ts) {
#ifdef _HAS_STD_HASH_
	printf("std hash test\n");
	typedef std::unordered_map<T, typename tester<T,_V>::_ValueType> _Map;
	_Map h;

	tester<T,_V> t;

	t.bench_hash_simple(h, script);
#endif
}

extern int google_times(int iters);
//extern int unique_running_insertion();
//extern int unique_scattered_lookup();
void more_tests() {
	//unique_scattered_lookup();
	//unique_running_insertion();
}
void test_random(size_t ts) {
	//typedef std::string _K;
	typedef unsigned long  _K;
	typedef unsigned long _V;
	//typedef std::string _K;

	tester<_K,_V>::_Script script;
	tester<_K,_V> t;
	t.gen_random(ts, script);
    //t.gen_seq(ts, script);
	//t.gen_random_narrowest(ts, script);
	//t.gen_random_narrow(ts, script);

	test_rabbit_hash<_K,_V>(script, ts);
	//test_rabbit_sparse_hash<_K,_V>(script, ts);
	//test_rabbit_hash_erase<_K>(ts/10);

	test_dense_hash<_K,_V>(script, ts);
	//test_sparse_hash<_K,_V>(script, ts);
	//test_std_hash<_K,_V>(script, ts);

}
int main(int argc, char **argv)
{
#ifdef _MSC_VER
	::Sleep(1000);
#endif
	size_t ts = 10000000;
	test_random(ts);
	//google_times(ts);
	//more_tests();
	return 0;
}
