#ifndef K_WAY_CACHE
#define K_WAY_CACHE


#include<iostream>
#include<mutex>
#include<unordered_map>

using namespace std;
template<typename T, U>
class Kway{
	public:
		//constructor
		Kway(int max_size, int k){
			KImpl_ = new KImpl(max_size);
		}

		void put(T key, U value){KImpl_->put(key, value);};

		pair<U, bool> retrieve(T key){KImpl_->retrieve(key);};

		bool remove(T key){KImpl_->remove(key)};

		void evict_set(int set_ind){KImpl_->evict_set(set_ind)};
		
		void evict_all(){KImpl_->evict_all()};

		int size(){KImpl_->size()}; // maybe call size for all sets and return their sum.

		int hit_count(){KImpl_->hit_count()};

		int miss_count(){KImpl_->miss_count()};

		int num_sets(){KImpl_->num_sets()};

		int num_lines(){KImpl_->num_lines()};

	private:

		struct KImpl;
		unique_ptr<KImpl> KImpl_;

		Kway(const Kway&); // new object being created with contents of another object
		void operator=(const Kway&);
}

template<typename T, typename U>
class Kway<T, U>::KImpl{
	//actual implementation

	KImpl(int max_size, int k) : max_size_(max_size), lines_(k), size_(0), hit_count_(0), miss_count_(0) {
		num_sets_ = ceil(max_size / (k * sizeof(U)));
		for (int i = 0; i < num_sets_; i++) {
			sets.push_back(new Set(lines_));
		}
	}

	void put(T key, U value){
		size_t hash_value = hash<key>{}(key);
		size_t set_index = hash_value % num_sets_;
		sets[set_index]->put(key, value);
	};

	pair<U, bool> retrieve(T key){
		size_t hash_value = hash<key>{}(key);
		size_t set_index = hash_value % num_sets_;
		return sets[set_index]->retrieve(key);
	};

	bool remove(T key){
		size_t hash_value = hash<key>{}(key);
		size_t set_index = hash_value % num_sets_;
		return sets[set_index]->remove(key);
	};

	void evict_set(int set_ind){
		if(set_ind < num_sets_){
			sets[set_ind]->evict_all();
		}else{
			throw out_of_range("Set index out of range");	
		}
	};

	void evict_all(){
		for(Set* s: sets){
			s->evict_all();
		}
	}

	int size(){
		int sum=0;
		for(Set* s: sets){
			sum+= s->set_size();
		}
		return sum;
	}; // maybe call size for all sets and return their sum.

	int set_hit_count(){return hit_count_};

	int set_miss_count(){return hit_miss_};

	struct Set;
	unique_ptr<Set> set_;	

	int max_size_;
	int lines_;
	int sets_;
	int hit_count_;
	int miss_count_;

	vector<Set*> sets;
	unordered_map<T, Set*> set_hashmap_;
}


template<typename T, typename U>
class Kway<T, U>::Kimpl::Set{
	struct Node{
		Node (U data): data(data), prev(NULL), next(NULL){}

		U data;
		Node* prev, next;
		unordered_map<T, Set*>::iterator map_it;
	}

	Set(int max_size): set_max_size_(max_size), size_(0), head_(NULL), tail_(NULL), hit_count_(0), miss_count_(0){
	}
	void put(T key, U value){
		lock_guard<mutex> lock(mutex_);
		
		Node* newNode = new Node(value);

		auto it = set_hashmap_.insert(pair<key, newNode>);

		if(!it.second){
			delete it.first->second;
			it.first->second = newNode;
			newNode->map_it = it;
			moveFront(newNode);
			return;
		}

		if(head_ == NULL){
			head_ = tail_ = newNode;
		}else{
			head_->prev = newNode;
			newNode->next = head_;
			head_ = newNode;
		}
		size_++;
		if(size_ > set_max_size_){
			tail_ = tail->prev;
			set_hashmap_.erase(tail->next->map_it);
			delete tail_->next;
			tail_->next = NULL;
			size--;
		}
	};

	pair<U, bool> retrieve(T key);

	bool remove(T key);

	void evict_set();

	int curr_size(){return size_};

	int set_size() {return set_max_size_};

	int set_hit_count(){return hit_count_};

	int set_miss_count(){return miss_count_};

	void bunp_to_front(Node* node);

	unordered_map<T, Node*> set_hashmap_;
	Node* head_;
	Node* tail_;

	int size_;
	int max_size_;

	int hit_count_;
	int miss_count_;

	mutex mutex_;
}

template<typename T, typename U>
class KWay<T, U>::KImpl : put()

template<typename T, typename U>
template<typename T, typename U>
template<typename T, typename U>






int main(){
}









#endif
