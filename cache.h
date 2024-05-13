//Strong references from  https://github.com/lamerman/cpp-lru-cache, https://github.com/ekg/lru_cache

// Assumptions
// 1. Assume that we are developing this cache to store integers or chars only.
// You can choose any data type to store this.
// 2. Assume that the data is always available in Main Memory.
// 3. Cache Access Time = 10 ns and Main Memory access time = 1 microsec-
// ond.

// TODO:
// WHY DO I GET SEGMENTATION FAULT WHEN I CALL move_front() !???
// ---Figure out why the display function seems bugged (Head isnt always the first element?) The cache itself works, but display seems bugged.
// ---add display function to display all data in a cache set (and the whole cache itself)
// line 237 try remove with commented out code (commented cuz i thought segmentatin fault was cuz of that)
// ---Add discrete time variables (10ns access time and miss_penalty, etc) 
// Clean up implementation
//

#pragma once
#define CAT 10
// CAT- Cache Access time (10 nanoseconds)
#define MMAT 1000
// MMAT- Main Memory Access Time (1 microsecond = 1000 nanoseconds)

#include<iostream>
#include<mutex>
#include<memory>
#include<utility>
#include<cmath>
#include<unordered_map>
#include<vector>
using namespace std;

template<typename T, typename U>
class Kway{
	public:
		//constructor
		Kway(int max_size, int k); 

		void PutData(T key, U value){return KImpl_->PutData(key, value);};

		void*  GetData(T key){return KImpl_->GetData(key);};

		bool remove(T key){return KImpl_->remove(key);};

		void evict_set(int set_ind){return KImpl_->evict_set(set_ind);};

		void evict_all(){return KImpl_->evict_all();};

		int size(){return KImpl_->size();}; // maybe call size for all sets and return their sum.

		int hit_count(){return KImpl_->hit_count();};

		int miss_count(){return KImpl_->miss_count();};

		double miss_ratio(){return KImpl_->miss_ratio();};

		double AMAT(){return KImpl_->AMAT();};

		double total_access_time(){return KImpl_->total_access_time();};

		void display_all(){return KImpl_->display_all();};

		int num_sets(){return KImpl_->num_sets();};

		int num_lines(){return KImpl_->num_lines();};

	private:
		struct KImpl;
		unique_ptr<KImpl> KImpl_;

		//		Kway(const Kway&); // new object being created with contents of another object
		//		void operator=(const Kway&);
};
template<typename T, typename U>
Kway<T, U>::Kway(int max_size, int k) : KImpl_(new KImpl(max_size, k)) {}

template<typename T, typename U>
class Kway<T, U>::KImpl{
	public:
		//actual implementation
		KImpl(int max_size, int k) : max_size_(max_size), lines_(k), size_(0), hit_count_(0), miss_count_(0) {
			num_sets_ = ceil(max_size / (k * sizeof(U)));
			for (int i = 0; i < num_sets_; i++) {
				sets.push_back(new Set(lines_));
			};
			int small_lines = ((max_size/sizeof(U)) %k);
			if(small_lines > 0){
				num_sets_++;
				sets.push_back(new Set(small_lines));
			}
		};

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
				sum+= s->curr_size();
			}
			return sum;
		}; // maybe call size for all sets and return their sum.

		int hit_count(){
			hit_count_ = 0;
			for(Set* s: sets){
				hit_count_+=s->set_hit_count();
			}
			return hit_count_;
		};

		int miss_count(){
			miss_count_ = 0;
			for(Set* s: sets){
				miss_count_+=s->set_miss_count();
			}
			return miss_count_;
		};

		double miss_ratio(){
			miss_ratio_ = (double)(miss_count()/hit_count());
			return miss_ratio_;
		};

		double AMAT(){
			if(hit_count()!= 0)
				return (CAT + miss_ratio()*MMAT)/1e9;
			else 

				return (double)(hit_count()*CAT + miss_count()*MMAT)/1000000000;

		};

		double total_access_time(){
			return (double)(hit_count() * CAT + miss_count()*MMAT)/1e9;
		};

		void display_all(){
			cout<<"\n\nHEAD ----> TAIL";
			for(int i=0; i<num_sets_; i++){
				cout<<"\n------SET-"<<i+1<<"------\n";
				sets[i]->display_set();
			}
			cout<<"\n";
			return;
		};

		int num_sets(){return num_sets_;};

		int num_lines(){return lines_;};
	private:
		struct Set;

		double miss_ratio_; //miss_count- / hit_count_
		int max_size_;
		int size_;
		int lines_;
		int num_sets_;
		int hit_count_;
		int miss_count_;

		vector<Set*> sets;
};


template<typename T, typename U>
class Kway<T, U>::KImpl::Set{
	public:
		struct Node;
		using map_type = std::unordered_map<T, Node*>;

		struct Node{
			Node (U data): data(data), prev(NULL), next(NULL){}

			U data;
			Node* prev;
			Node* next;
			typename map_type::iterator map_it; //compiler told to add typename but idk why this works
		};
		Set(int max_size): set_max_size_(max_size), size_(0), head_(NULL), tail_(NULL), hit_count_(0), miss_count_(0){
		};

		void move_front(Node* node){
			if(head_ == node){
				return;
			}
			head_->prev = node;
			if(node!= tail_){
				node->next->prev = node->prev;
			}else{
				tail_ = tail_->prev; 
			}

			node->prev->next = node->next;
			node->prev = NULL;
			node->next = head_;
			head_ = node;
			return;
		};

		void PutData(T key, U value){
			//update AMAT at the end of each access. Could be more efficient if calculated later but requirements specify to do so.

			lock_guard<mutex> lock(mutex_);

			Node* newNode = new Node(value);

			auto it = set_hashmap_.insert(typename map_type::value_type(key, newNode));

			if(!it.second){
				newNode = it.first->second;

				move_front(newNode);
				return;
			}

			newNode->map_it = it.first;
			if(head_ == NULL){
				head_ = tail_ = newNode;
			}else{
				head_->prev = newNode;
				newNode->next = head_;
				head_ = newNode;
			}
			++size_;
			if(size_ > set_max_size_){
				tail_ = tail_->prev;
				set_hashmap_.erase(tail_->next->map_it);
				delete tail_->next;
				tail_->next = NULL;
				size_--;
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
				move_front(temp_node);		

				return &(temp_node->data);
			}
			return nullptr;
		};

		bool remove(T key){
			lock_guard<mutex> lock(mutex_);
			
			auto it = set_hashmap_.find(key);

			if (it == set_hashmap_.end())
				return false;

			Node* node = it->second;

			move_front(node);


			head_ = head_->next;

			if (head_ != NULL)
				head_->prev = NULL;

			set_hashmap_.erase(node->map_it);
			delete node;

			size_--;

			return true;
			//if(set_hashmap_.find(key) == set_hashmap_.end()){
			//	return false;
			//}
			//size_--;
			//Node* temp_node = set_hashmap_.at(key);

			//if(head_ == temp_node){
			//	head_ = temp_node->next;
			//}
			//if(tail_ == temp_node){
			//	tail_ = temp_node->prev;
			//}
			//if(temp_node->next){
			//	temp_node->next->prev = temp_node->prev;
			//}if(temp_node->prev){
			//	temp_node->prev->next = temp_node->next;
			//}
			//set_hashmap_.erase(temp_node->map_it);
			//delete temp_node;
			//return true;
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


		void display_set(){
			Node* temp = head_;
			if(temp == NULL){
				cout<<" Empty ";
			}
			while(temp != NULL){
				cout<<" "<<temp->data;
				temp = temp->next;
			}
			return;
		};

	private:
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

