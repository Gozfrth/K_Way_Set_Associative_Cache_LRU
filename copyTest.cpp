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
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <csignal>

#include "cache.h"

#define ONE_MB 1048576

using namespace std;

class CacheTest : public testing::Test{
	protected:
		// kway_cache (max_size, k, block_size) max and block size in bytes
		CacheTest() : kway_cache_1(128, 2, 16), kway_cache_2(128, 1, 16), kway_cache_3(16, 1, 16), kway_cache_4(128, 3), kway_cache_5(128, 4), kway_cache_large(ONE_MB, 4, 16), kway_cache_small(4, 1){
		}

		~CacheTest() override {
		}

		void SetUp() override {
			//code here called right after constructor.
		}

		void TearDown() override {
			//codfe called right after each test (before destructor)
		}
		Kway<int> kway_cache_1; //fully associative
		Kway<int> kway_cache_2; //direct mappoing
		Kway<int> kway_cache_3; //cache size 1 
		Kway<int> kway_cache_4; //3 way setassoc
		Kway<int> kway_cache_5; //Used for thread safety test
		Kway<int> kway_cache_large; //Usef for LargeSize test
		Kway<int> kway_cache_small;
};

// TEST_1
TEST_F(CacheTest, CacheOperations){
	//CACHE OPERATIONS
	//Since only 2 functions were mentioned in requirements, ig this is it?
	int i=0;
	kway_cache_3.PutData(&i);
	cout<<"After Inserting"<<endl;
	ASSERT_NE(nullptr, kway_cache_3.GetData(&i)); //in cache
	cout<<"After hit\n";
	ASSERT_EQ(nullptr, kway_cache_3.GetData((&i)+1)); //not in cache
	cout<<"After miss\n";
}


// TEST_2

// TEST_3

// TEST_4

// TEST_5

int main(int argc, char **argv){
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
