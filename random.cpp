#include "cache.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <time.h>

using namespace std;

int main(int argc, char*argv[]){
	srand(time(0));
	std::map<std::string, std::string> args;
	for (int i = 1; i < argc; i += 2) {
		if (i + 1 < argc) {
			args[argv[i]] = argv[i + 1];
		}
	}
        int max_size = stoi(args["--max_size"]);
        int k = stoi(args["--k"]);
        int block_size = stoi(args["--block_size"]);
        int step = stoi(args["--step_size"]);
	int iterations = stoi(args["--iterations"]);
	bool display = false;

	if(args["--display"] == "True"){
		display = true;
	}

	//128 bytes cache -> 4 byte int -> 4 lines in a set(k) -> 8 sets
	// (max_size, k, block_size)
	Kway<int> kway_cache(max_size, k, block_size, true, step); 
	kway_cache.initGraph("random");
	vector<int>arr(4000);
	for(int i=0; i<4000; i++){
		arr[i] = i;
	}

	int randInt;
	for(int i=0; i<iterations; i++){
		randInt = rand()%4000;
		kway_cache.PutData(&arr[randInt]);
		if(display){
			cout<<"\n\nRandom access index: "<<randInt<<"\n";
			kway_cache.display_all();
		}
	}
	// kway_cache.display_all();

	cout<<"Current size of cache (bytes) :"<<kway_cache.size()<<""<<endl;	
	cout<<"Number of lines in each Set :"<<kway_cache.num_lines()<<""<<endl;
	cout<<"Number of sets: "<<kway_cache.num_sets()<<""<<endl;
	cout<<"Cache Miss Count: "<<kway_cache.miss_count()<<""<<endl;
	cout<<"Cache Hit Count: "<<kway_cache.hit_count()<<""<<endl;
	cout<<"Cache Miss Ratio: "<<kway_cache.miss_ratio()<<""<<endl;


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
	cout<<"\n\n\n\n";
	return 1;
}
