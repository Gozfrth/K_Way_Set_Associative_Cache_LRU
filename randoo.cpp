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
	Kway<int, int> kway_cache(1024, 4);

	//PUTTING DATA
	cout<<"----PUTTING DATA----\n";

	for(int i=0; i<4098; i++){
		kway_cache.PutData(i, i);
	}		

	cout<<"Current size of cache (bytes) :"<<kway_cache.size()<<""<<endl;	
	cout<<"Number of lines in each Set :"<<kway_cache.num_lines()<<""<<endl;
	cout<<"Number of sets: "<<kway_cache.num_sets()<<""<<endl;
	cout<<"Cache Miss Count: "<<kway_cache.miss_count()<<""<<endl;
	cout<<"Cache Hit Count: "<<kway_cache.hit_count()<<""<<endl;

	//GETTING DATA
	cout<<"\n\n----GETTING DATA----\n";
	for(int i=0; i<4098; i++){
		if(auto x = kway_cache.GetData(i)){
		}
	}

	kway_cache.display_all();

	cout<<"Current Size: "<<kway_cache.size()<<" lines x "<<sizeof(int)<<" bytes"<<endl;	


	cout<<"AVERAGE MEMORY ACCESS TIME: "<<kway_cache.AMAT()<<" nanoseconds"<<endl;

	return 1;
}
