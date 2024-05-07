#ifndef K_WAY_CACHE
#define K_WAY_CACHE


#include<iostream>
#include<mutex>
#include<unordered_map>
using namespace std;

template<typename U, typename V>
class Kway{
	public:
		Kway(int max_size, int k);

		void put(T key, U val);	

		int hit_count();

		int miss_count();
		
		pair<U, bool> retrieve(T key);

		bool remove(T key);
		
		int num_lines();

		int num_sets();
	private:

}
