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



#include "cache.h"
#include <iostream>
#include <chrono>
#include <thread>
//Write unit tests for this instead.

using namespace std;

int main(){
	//128 bytes cache -> 4 byte int -> 4 lines in a set(k) -> 8 sets
	// (max_size, k, block_size)
	Kway<int> kway_cache(1024, 128, 8);

	vector<int>arr(4000);
	for(int i=0; i<4000; i++){
		arr[i] = i;
	}

	//PUTTING DATA
	cout<<"----PUTTING DATA----\n";

	for(int i=0; i<4000; i++){
		// kway_cache.display_all();
		kway_cache.PutData(&arr[i]);
		cout<<"YO"<<i;
		cout<<"\n\n\n";
	}		
	// kway_cache.display_all();

	cout<<"Current size of cache (bytes) :"<<kway_cache.size()<<""<<endl;	
	cout<<"Number of lines in each Set :"<<kway_cache.num_lines()<<""<<endl;
	cout<<"Number of sets: "<<kway_cache.num_sets()<<""<<endl;
	cout<<"Cache Miss Count: "<<kway_cache.miss_count()<<""<<endl;
	cout<<"Cache Hit Count: "<<kway_cache.hit_count()<<""<<endl;

	//GETTING DATA
	/*cout<<"\n\n----GETTING DATA----\n";
	for(int i=0; i<10; i++){
		if(auto x = kway_cache.GetData(i)){
			cout<<*(int*)x<<endl;
		}
	}*/

	// kway_cache.display_all();

	//cout<<kway_cache.GetData(1)<<endl;
	/*cout<<"Removing key 1: "<<kway_cache.remove(1)<<endl;
	cout<<"Current Size: "<<kway_cache.size()<<" lines x "<<sizeof(int)<<" bytes"<<endl;	

	if(auto x = kway_cache.GetData(1)){
		cout<<*(int*)x<<endl;
	};

	kway_cache.remove(2);
	kway_cache.remove(3);
	*/cout<<"AVERAGE MEMORY ACCESS TIME: "<<kway_cache.AMAT()<<"seconds"<<endl;

	return 1;
}
