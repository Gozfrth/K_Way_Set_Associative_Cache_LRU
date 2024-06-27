#include "cache.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <string>

using namespace std;

int main(int argc, char*argv[]){
	if(argc != 4){
		cout<<"Erroro"<<endl;
		exit(1);
	}
	int max_size = stoi(argv[1]);
	int k = stoi(argv[2]);
	int block_size = stoi(argv[3]);
	//128 bytes cache -> 4 byte int -> 4 lines in a set(k) -> 8 sets
	// (max_size, k, block_size)
	Kway<int> kway_cache(max_size, k, block_size, true); 
	kway_cache.initGraph("pyexec");
	vector<int>arr(4000);
	for(int i=0; i<4000; i++){
		arr[i] = i;
	}

	//PUTTING DATA
	cout<<"----PUTTING DATA----\n";

	for(int i=0; i<4000; i++){
		// kway_cache.display_all();
		kway_cache.PutData(&arr[i]);
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
	*/
	cout<<"AVERAGE MEMORY ACCESS TIME: "<<kway_cache.AMAT()<<"seconds"<<endl;
	
	kway_cache.terminateGraph();
	return 1;
}
