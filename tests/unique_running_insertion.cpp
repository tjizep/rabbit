/* Measuring running insertion times of unordered associative containers
 * without duplicate elements.
 *
 * Copyright 2013 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#include <algorithm>
#include <array>
#include <chrono>
#include <numeric>
#include <rabbit/unordered_set>
static std::chrono::high_resolution_clock::time_point measure_start,measure_pause;

template<typename F>
static double measure(F f)
{
  using namespace std::chrono;

  static const int              num_trials=10;
  static const milliseconds     min_time_per_trial(200);
  std::array<double,num_trials> trials;

  for(int i=0;i<num_trials;++i){
    int                               runs=0;
    high_resolution_clock::time_point t2;

    measure_start=high_resolution_clock::now();
    do{
      f();
      ++runs;
      t2=high_resolution_clock::now();
    }while(t2-measure_start<min_time_per_trial);
    trials[i]=duration_cast<duration<double>>(t2-measure_start).count()/runs;
  }

  std::sort(trials.begin(),trials.end());
  return std::accumulate(
    trials.begin()+2,trials.end()-2,0.0)/(trials.size()-4);
}

static void pause_timing()
{
  measure_pause=std::chrono::high_resolution_clock::now();
}

static void resume_timing()
{
  measure_start+=std::chrono::high_resolution_clock::now()-measure_pause;
}

#include <boost/bind.hpp>
#include <iostream>
#include <random>

struct rand_seq
{
  rand_seq(unsigned int):gen(34862){}
  unsigned int operator()(){return dist(gen);}

private:
  std::uniform_int_distribution<unsigned int> dist;
  std::mt19937                                gen;
};

template<typename Container>
void reserve(Container& s,unsigned int n)
{
  s.rehash((unsigned int)((double)n/s.max_load_factor()));
}

template<typename Container>
struct running_insertion
{
  typedef unsigned int result_type;

  unsigned int operator()(unsigned int n)const
  {
    unsigned int res=0;
    {
      Container s;
      rand_seq  rnd(n);
      while(n--)s.insert(rnd());
      res=( unsigned int)s.size();
      pause_timing();
    }
    resume_timing();
    return res;
  }
};

template<typename Container>
struct norehash_running_insertion
{
  typedef unsigned int result_type;

  unsigned int operator()(unsigned int n)const
  {
    unsigned int res=0;
    {
      Container s;
      rand_seq  rnd(n);
      reserve(s,n);
      while(n--)s.insert(rnd());
      res=( unsigned int)s.size();
      pause_timing();
    }
    resume_timing();
    return res;
  }
};

template<
  template<typename> class Tester,
  typename Container1,typename Container2,typename Container3>
void test(
  const char* title,
  const char* name1,const char* name2,const char* name3)
{
  unsigned int n0=10000,n1=3000000,dn=500;
  double       fdn=1.05;

  std::cout<<title<<":"<<std::endl;
  std::cout<<name1<<";"<<name2<<";"<<name3<<std::endl;

  for(unsigned int n=n0;n<=n1;n+=dn,dn=(unsigned int)(dn*fdn)){
    double t;
    unsigned int m=Tester<Container1>()(n);

    t=measure(boost::bind(Tester<Container1>(),n));
    std::cout<<m<<";"<<(t/m)*10E6;

    t=measure(boost::bind(Tester<Container2>(),n));
    std::cout<<";"<<(t/m)*10E6;
 
    t=measure(boost::bind(Tester<Container3>(),n));
    std::cout<<";"<<(t/m)*10E6<<std::endl;
  }
}

#include <boost/unordered_set.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <unordered_set>

int unique_running_insertion()
{
  using namespace boost::multi_index;

  /* some stdlibs provide the discussed but finally rejected std::identity */
  using boost::multi_index::identity; 

  typedef std::unordered_set<unsigned int>        container_t1;
  typedef rabbit::unordered_set<unsigned int>        container_t2;
  typedef boost::unordered_set<unsigned int>      container_t3;
  /// typedef boost::multi_index_container<unsigned int,indexed_by<hashed_unique<identity<unsigned int> >>> container_t3;

  test<
    norehash_running_insertion,
    container_t1,
    container_t2,    
	container_t3>
  (
    "No-rehash runnning insertion",
    "std::unordered_set",
    "rabbit::unordered_set" ,    
	"boost::unordered_set"
  );

  test<
    running_insertion,
    container_t1,
    container_t2,   
	container_t3>
  (
    "Runnning insertion",
    "std::unordered_set",
    "rabbit::unordered_set",   
	"boost::unordered_set"
  );
  return 0;
}
