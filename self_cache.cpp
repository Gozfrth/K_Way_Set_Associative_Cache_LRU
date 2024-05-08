#ifndef K_WAY_CACHE
#define K_WAY_CACHE


#include<iostream>
#include<mutex>
#include<unordered_map>
using namespace std;

template<typename T, typename U>
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
		struct KImpl{
			KImpl(int max_size, int k):lines_(k), sets_(ceil(max_size/(k *sizeof(V))), size_(0), hit_count_(0), miss_count_(0){
			}
			void put(T key, U val);

			pair<U, bool> retrieve(T key);

			bool remove(T key);

			int num_lines();

			
		}
		unique_ptr<KImpl> KImpl_;

		Kway(const Kwat&);
		void operator=(const Kway&);
}
