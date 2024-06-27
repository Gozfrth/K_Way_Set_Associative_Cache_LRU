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
		CacheTest() : kway_cache_1(128, 2, 16), kway_cache_2(128, 1, 16), kway_cache_3(16, 1, 16), kway_cache_4(128, 4, 16), kway_cache_5(128, 4), kway_cache_large(ONE_MB, 4, 16, true, "cache_simulation"), kway_cache_small(4, 1){
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
	ASSERT_NE(nullptr, kway_cache_3.GetData(&i)); //in cache
	ASSERT_EQ(nullptr, kway_cache_3.GetData((&i)+1)); //not in cache
}

// TEST_2
TEST_F(CacheTest, Structure){
	//FULLY ASSOCIATIVE (k= size of cache)
	//

	//how to define?
	//for kway_cache_1, maybe add 32 elements and check whether 1st element is always there, then add one more, check if 1st one is still there? If not then correct

	int i;
	int arr[33];
	for(int i =0; i<33; i++){
		arr[i] = (rand()%ONE_MB); //random initialization
	}
	for(i=0; i<32; i++){
		kway_cache_1.PutData(&arr[i]);
	};

	//Asserting that elements must not be evicted.
	for(i=0; i<32; i++){
		ASSERT_NE(nullptr, kway_cache_1.GetData(&arr[i]));
	};

	kway_cache_1.PutData(&arr[i]);//for first eviction
	ASSERT_EQ(nullptr, kway_cache_1.GetData(&arr[0]));//0 should be evicted (since it was the first item in cache)


	//DIRECT-MAPPED (k=1)

	//for kway_cache_2, add one element, then another element that falls into the same set. The first element must be evicted by now. If yes, correct. Else, no

	//Since there are 32 sets, assuming it is a basic mod hash gunction, for eviction to occur, an item with key 33 needs to be inserted. 
	//NOTE:
	//If the sets follow a different hashing policy, this would not be a good test and may need to be changed.

	kway_cache_2.PutData(&arr[5]);
	ASSERT_NE(nullptr, kway_cache_2.GetData(&arr[5]));//33 must be inserted

	ASSERT_EQ(nullptr, kway_cache_2.GetData(&arr[0])); //1 should be evicted
};

// TEST_3
TEST_F(CacheTest, SmallSize){
	int i[2];
	kway_cache_small.PutData(&i[0]);
	ASSERT_NE(nullptr, kway_cache_small.GetData(&i[0]));
	kway_cache_small.PutData(&i[1]);
	ASSERT_EQ(nullptr, kway_cache_small.GetData(&i[0]));
}

// TEST_4
TEST_F(CacheTest, LargeSize){
	kway_cache_large.initGraph("Large");
	int arr[1000000], i;

        for(i =0; i<1000000; i++){
                arr[i] = (rand()%ONE_MB); //random initialization
        }

        for(i=0; i<1000000; i++){
                kway_cache_large.PutData(&arr[i]);
        }
        for(i=750000; i<1000000; i++){
                ASSERT_NE(nullptr, kway_cache_large.GetData(&arr[i]));
        }
	kway_cache_large.terminateGraph();

}
// TEST_5
TEST_F(CacheTest, CacheLiterals){
	//CACHE LITERALS
	//checking hit_count and miss_count. Also AMAT?

	int i;
	int arr[64];
	for(int i =0; i<64; i++){
		arr[i] = (rand()%ONE_MB); //random initialization
	}
	for(i=0; i<32; i++){	
		kway_cache_4.PutData(&arr[i]);
	}// 8 misses, 24 hits

	for(i=32; i<64; i++){
		kway_cache_4.GetData(&arr[i]);
	}//8 misses, 24 hits

	ASSERT_EQ(16, kway_cache_4.miss_count());
	ASSERT_EQ(48, kway_cache_4.hit_count());
	// EXPECT_EQ((double)(32*1000)/1e9, kway_cache_4.AMAT()); //maybe better to use EXPECT_GT

	/*
	for(i=0; i<64; i++){
		if(kway_cache_4.GetData(i%32)==nullptr){
			cout<<i%32<<endl;
		};
	}//64 hits

	ASSERT_EQ(64, kway_cache_4.hit_count());
	EXPECT_GT((double)(32*1000)/1e9, kway_cache_4.AMAT()); //maybe better to use EXPECT_GT
	*/
}


int threadArr[1000];

void ThreadTesterFunction(Kway<int>* cache){
	for(int i=0; i<1000; i++){
		cache->PutData(&threadArr[i]);
	}
	for(int i=0; i<1000; i++){
		cache->GetData(&threadArr[i]);
	}
};

// TEST_6
TEST_F(CacheTest, ThreadSafety){
	const int num_threads = 4;
	int i;

	thread threads[num_threads]; //arraty of thread objects
	pid_t pid = fork();

	if(pid == -1){
		FAIL()<<"Failed to process new fork";
	}else if(pid == 0){
		for(i=0; i<num_threads; i++){
			threads[i] = thread(ThreadTesterFunction, &kway_cache_5);
		}

		for(int i=0; i<num_threads; i++){
			threads[i].join();
		}
	} else {
		int status;
		waitpid(pid, &status, 0);
		if(WIFSIGNALED(status)){
			if(WTERMSIG(status) == SIGSEGV){
				FAIL()<<"Process terminated due to segmentation fault";
			}else{
				FAIL()<<"Process terminated by unxpected signal";
			}
		}else{
			SUCCEED();
		}
	}
	//THREAD SAFETY
}

int main(int argc, char **argv){
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
