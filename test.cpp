//TODO:

//STRUCTURE OF CACHE----
//test with k=1, k=number of sets. (boundary test cases) -- testing if definition of structure of the cache is correct.

//CACHE OPERATIONS
//insert data and check if it is retrievable, insert more data than size and check LRU, remove and verify no longer retrievable.

//CACHE LITERALS
//insert data, access multiple times (check if cache_hits_ increases), (access data not in cache and verify miss increments)

//THREAD SAFETY
//test concurrent access to cache from multiple threads... gotta figure out how to
//verufy thread safety and consistency

//Test with corner cases / large values that are more likely to cause bugs. 
//Test with very large size of cache


#include <iostream>
#include <gtest/gtest.h>
#include <cstdlib>
#include <ctime>
#include <thread>
#include "cache.h"

using namespace std;

class CacheTest : public testing::Test{
	protected:
		CacheTest() : kway_cache_1(128, 32), kway_cache_2(128, 1), kway_cache_3(4, 1), kway_cache_4(128, 3), kway_cache_5(128, 3){
		}

		~CacheTest() override {
		}

		void SetUp() override {
			//code here called right after constructor.
		}

		void TearDown() override {
			//codfe called right after each test (before destructor)
		}
		Kway<int, int> kway_cache_1; //fully associative
		Kway<int, int> kway_cache_2; //direct mappoing
		Kway<int, int> kway_cache_3; //cache size 1
		Kway<int, int> kway_cache_4; //3 way setassoc
		Kway<int, int> kway_cache_5;
};

// TEST_1
TEST_F(CacheTest, CacheOperations){
	//CACHE OPERATIONS
	//Since only 2 functions were mentioned in requirements, ig this is it?
	kway_cache_3.PutData(0, 0);
	ASSERT_NE(nullptr, kway_cache_3.GetData(0)); //in cache
	ASSERT_EQ(nullptr, kway_cache_3.GetData(1)); //not in cache
}

// TEST_2
TEST_F(CacheTest, CacheLiterals){
	//CACHE LITERALS
	//checking hit_count and miss_count. Also AMAT?
	int i;

	for(i=0; i<32; i++){	
		kway_cache_4.PutData(i, i);
	}

	for(i=32; i<64; i++){
		kway_cache_4.GetData(i);
	}//32 misses
	
	ASSERT_EQ(32, kway_cache_4.miss_count());
	ASSERT_EQ(0, kway_cache_4.hit_count());
	EXPECT_EQ((double)(32*1000)/1e9, kway_cache_4.AMAT()); //maybe better to use EXPECT_GT
	
	for(i=0; i<64; i++){
		if(kway_cache_4.GetData(i%32)==nullptr){
			cout<<i%32<<endl;
		};
	}//64 hits
	
	ASSERT_EQ(64, kway_cache_4.hit_count());
	EXPECT_GT((double)(32*1000)/1e9, kway_cache_4.AMAT()); //maybe better to use EXPECT_GT
}

// TEST_3
TEST_F(CacheTest, Structure){
	//FULLY ASSOCIATIVE (k= size of cache)

	//how to define?
	//for kway_cache_1, maybe add 32 elements and check whether 1st element is always there, then add one more, check if 1st one is still there? If not then correct

	int i;
	for(i=0; i<=31*31; i+=31){
		kway_cache_1.PutData(i, i);
	};

	//Asserting that elements must not be evicted.
	for(i=0; i<=31*31; i+=31){
		ASSERT_NE(nullptr, kway_cache_1.GetData(i));
	};

	kway_cache_1.PutData(32, 32);//for first eviction
	ASSERT_EQ(nullptr, kway_cache_1.GetData(0));//0 should be evicted (since it was the first item in cache)


	//DIRECT-MAPPED (k=1)

	//for kway_cache_2, add one element, then another element that falls into the same set. The first element must be evicted by now. If yes, correct. Else, no
	kway_cache_2.PutData(1, 1);
	ASSERT_NE(nullptr, kway_cache_2.GetData(1));

	//Since there are 32 sets, assuming it is a basic mod hash gunction, for eviction to occur, an item with key 33 needs to be inserted. 
	//NOTE:
	//If the sets follow a different hashing policy, this would not be a good test and may need to be changed.

	kway_cache_2.PutData(33, 33);
	ASSERT_NE(nullptr, kway_cache_2.GetData(33));//33 must be inserted

	ASSERT_EQ(nullptr, kway_cache_2.GetData(1)); //1 should be evicted
};



void ThreadTesterFunction(Kway<int, int>* cache){
	int i;
	for(i=0; i<32; i++){
		cache->PutData(i, i);
	}
	for(i=0; i<32; i++){
		cache->GetData(i);
	}
};

// TEST_4
TEST_F(CacheTest, ThreadSafety){
	const int num_threads = 4;
	int i;

	thread threads[num_threads]; //arraty of thread objects
	
	for(i=0; i<num_threads; i++){
		threads[i] = thread(ThreadTesterFunction, &kway_cache_5);
	}

	for(int i=0; i<num_threads; i++){
		threads[i].join();
	}

	ASSERT_EQ(128, kway_cache_5.hit_count());
	ASSERT_EQ(0, kway_cache_5.miss_count());
	//THREAD SAFETY
}

int main(int argc, char **argv){
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
