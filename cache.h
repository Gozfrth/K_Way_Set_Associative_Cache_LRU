// TODO:
// Add time variables (10ns access time and miss_penalty, etc) 
// Clean up implementation
// line 133
// Theres bound to be errors since I havent tested this once also.


#ifndef K_WAY_CACHE
#define K_WAY_CACHE


#include<iostream>
#include<mutex>
#include<unordered_map>
#include<memory>
#include<cmath>
#include<vector>
using namespace std;

template<typename T, typename U>
class Kway{
	public:
		//constructor
		Kway(int max_size, int k); 

		void PutData(T key, U value){return KImpl_->PutData(key, value);};

		void* GetData(T key){return KImpl_->GetData(key);};

		bool remove(T key){return KImpl_->remove(key);};

		void evict_set(int set_ind){return KImpl_->evict_set(set_ind);};

		void evict_all(){return KImpl_->evict_all();};

		int size(){return KImpl_->size();}; // maybe call size for all sets and return their sum.

		int hit_count(){return KImpl_->hit_count();};

		int miss_count(){return KImpl_->miss_count();};

		int num_sets(){return KImpl_->num_sets();};

		int num_lines(){return KImpl_->num_lines();};

	private:
		struct KImpl;
		unique_ptr<KImpl> KImpl_;

		Kway(const Kway&); // new object being created with contents of another object
		void operator=(const Kway&);
};
template<typename T, typename U>
Kway<T, U>::Kway(int max_size, int k) : KImpl_(new KImpl(max_size, k)) {}

template<typename T, typename U>
class Kway<T, U>::KImpl{
	//actual implementation
	public:
		KImpl(int max_size, int k) : max_size_(max_size), lines_(k), size_(0), hit_count_(0), miss_count_(0) {
			num_sets_ = ceil(max_size / (k * sizeof(U)));
			for (int i = 0; i < num_sets_; i++) {
				sets.push_back(new Set(lines_));
			}
		}

		void PutData(T key, U value){
			size_t hash_value = hash<T>{}(key);
			size_t set_index = hash_value % num_sets_;
			sets[set_index]->PutData(key, value);
		};

		void* GetData(T key){
			size_t hash_value = hash<T>{}(key);
			size_t set_index = hash_value % num_sets_;
			return sets[set_index]->GetData(key);
		};

		bool remove(T key){
			size_t hash_value = hash<T>{}(key);
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

		int hit_count(){return hit_count_;};

		int miss_count(){return miss_count_;};

		int num_sets(){return num_sets_;};

		int num_lines(){return max_size_;};

	private:
		struct Set;

		int max_size_;
		int size_;
		int lines_;
		int num_sets_;
		int hit_count_;
		int miss_count_;

		vector<Set*> sets;
		unordered_map<T, Set*> set_hashmap_;
};


template<typename T, typename U>
class Kway<T, U>::KImpl::Set{
	struct Node{
		Node (U data): data(data), prev(NULL), next(NULL){}

		U data;
		Node* prev;
		Node* next;
		typename unordered_map<T, Set*>::iterator map_it; //compiler told to add typename but idk why this works
	};
	Set(int max_size): set_max_size_(max_size), size_(0), head_(NULL), tail_(NULL), hit_count_(0), miss_count_(0){
	};
	void PutData(T key, U value){
		lock_guard<mutex> lock(mutex_);

		Node* newNode = new Node(value);

		auto it = set_hashmap_.insert(make_pair(key, newNode));

		if(!it.second){
			delete it.first->second;
			it.first->second = newNode;
			newNode->map_it = it;
			move_front(newNode);
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
			tail_ = tail_->prev;
			set_hashmap_.erase(tail_->next->map_it);
			delete tail_->next;
			tail_->next = NULL;
			size--;
		}
	};

	void* GetData(T key){
		lock_guard<mutex> lock(mutex_);

		if(set_hashmap_.find(key) == set_hashmap_.end()){
			miss_count_++;
			return nullptr;
		}else{
			hit_count_++;
			Node* temp_node = set_hashmap_.at(key);
			moveFront(temp_node);

			return &(temp_node->data);
		}
		return nullptr;
	};

	bool remove(T key){
		lock_guard<mutex> lock(mutex_);

		if(set_hashmap_.find(key) == set_hashmap_.end()){
			return false;
		}
		size_--;
		Node* temp_node = set_hashmap_.at(key);

		if(head_ == temp_node){
			head_ = temp_node->next;
		}
		if(tail_ == temp_node){
			tail_ = temp_node->prev;
		}
		if(temp_node->next){
			temp_node->next->prev = temp_node->prev;
		}if(temp_node->prev){
			temp_node->prev->next = temp_node->next;
		}
		set_hashmap_.erase(temp_node->map_it);
		delete temp_node;
		return true;
	};

	void evict_set(){
		lock_guard<mutex> lock(mutex_);

		size_ = 0;
		head_ = tail_ = NULL;
		Node* temp_node = head_, ptr;
		while(temp_node){
			ptr = temp_node;
			set_hashmap_.erase(ptr->map_it);
			temp_node = temp_node->next;
			delete ptr;
		}

	};

	int curr_size(){return size_;};

	int set_size() {return set_max_size_;};

	int set_hit_count(){return hit_count_;};

	int set_miss_count(){return miss_count_;};

	void move_front(Node* node){
		if(node == head_){
			return;
		}

		if(node->next){
			node->next->prev = node->prev;
		}else{
			tail_ = tail_->prev; 
		}

		node->prev->next = node->next;
		node->next = head_;
		node->prev = NULL;
		head_->prev = node;
		head_ = node;
	};

	unordered_map<T, Node*> set_hashmap_;
	Node* head_;
	Node* tail_;

	int size_;
	int set_max_size_;

	int hit_count_;
	int miss_count_;

	mutex mutex_;
};

// template<typename T, typename U>
// class KWay<T, U>::KImpl : put()

// template<typename T, typename U>
// template<typename T, typename U>
// template<typename T, typename U>

#endif
