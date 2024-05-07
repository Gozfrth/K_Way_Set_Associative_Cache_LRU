#ifndef K_WAY_CACHE
#define K_WAY_CACHE


#include<iostream>
#include<mutex>
#include<unordered_map>

using namespace std;
template<typename T, U>
class Kway{
	public:
		Kway(int max_size, int k);

		void put(T key, U value);

		pair<U, bool> retrieve(T key);

		bool remove(T key);

		void evict_all();

		int size(); // maybe call size for all sets and return their sum.

		int hit_count();

		int miss_count();

		int num_sets();

		int num_lines();

	private:
		struct KImpl;
		unique_ptr<Set> KImpl_;

		Kway(const Kway&); // new object being created with contents of another object
		void operator=(const Kway&);
}

template<typename T, typename U>
class Kway::KImpl{

	KImpl(int max_size, int k):lines_(k), sets_(ceil(max_size / (k * sizeof(U))), size_(0), hit_count_(0), miss_count_(0){
	}

	void put(T key, U value);

	pair<U, bool> retrieve(T key);

	bool remove(T key);

	void evict_set();

	int set_size(); // maybe call size for all sets and return their sum.

	int set_hit_count();

	int set_miss_count();

	struct Set;
	unique_ptr<Imple> set_;

	unordered_set<T, Set*>;
}


template<typename T, typename U>
class Kway::Kimpl::Set{
	struct Node{
		Node (U data): data(data), prev(NULL), next(NULL){}

		U data;
		Node* prev, next;
		unordered_set<T, Set*>::iterator map_it;
	}

	Set(int max_size): set_max_size_(max_size), size_(0), head_(NULL), tail_(NULL), hit_count_(0), miss_count_(0){
}
	void put(T key, U value);

	pair<U, bool> retrieve(T key);
	
	bool remove(T key);

	void evict_set();

	int curr_size(){return size_};

	int set_size() {return set_max_size_};

	int set_hit_count(){return hit_count_};

	int set_miss_count(){return miss_count_};

	void bunp_to_front(Node* node);

	unordered_set<T, Node*> set_hashmap_;
	Node* head_;
	Node* tail_;

	int size_;
	int max_size_;

	int hit_count_;
	int miss_count_;

	mutex mutex_;
}




int main(){
}









#endif
