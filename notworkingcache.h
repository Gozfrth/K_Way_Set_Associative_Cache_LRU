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
#include<cstring>
#include<vector>

#include <SFML/Graphics.hpp>

using namespace std;

template<typename T>
class Kway{
	public:
		//constructor
		Kway(int max_size, int k, int block_size, bool graph = false); 

		void PutData(T* key){return KImpl_->PutData(key);};

		void* GetData(T* key){return KImpl_->GetData(key);};

		int size(){return KImpl_->size();}; // maybe call size for all sets and return their sum.

		int hit_count(){return KImpl_->hit_count();};

		int miss_count(){return KImpl_->miss_count();};

		double miss_ratio(){return KImpl_->miss_ratio();};

		double AMAT(){return KImpl_->AMAT();};

		void display_all(){return KImpl_->display_all();};

		int num_sets(){return KImpl_->num_sets();};

		int num_lines(){return KImpl_->num_lines();};

	private:
		struct KImpl;
		unique_ptr<KImpl> KImpl_;

		//		Kway(const Kway&); // new object being created with contents of another object
		//		void operator=(const Kway&);
};
template<typename T>
Kway<T>::Kway(int max_size, int k, int block_size, bool graph) : KImpl_(new KImpl(max_size, k, block_size, graph)) {}

template<typename T>
class Kway<T>::KImpl{
	public:
		//actual implementation
		KImpl(int max_size, int k, int block_size, bool graph = false) : max_size_(max_size), lines_(k), size_(0), hit_count_(0), miss_count_(0), block_size_(block_size), display_graph_(graph) {
			num_sets_ = ceil(max_size / (k *block_size));
			for (int i = 0; i < num_sets_; i++) {
				sets.push_back(new Set(lines_, block_size));
			};

			//Line incase k is not a power of 2 (which shouldnt happen)
			/*int small_lines = ((max_size/block_size)%k); //probably (sizeof(U)*block_size)
			  if(small_lines > 0){
			  num_sets_++;
			  sets.push_back(new Set(small_lines, block_size));
			  }*/
		};

		int getIndex(T* addr){
			cout<<"In getIndex yo\n";
			return (reinterpret_cast<uintptr_t>(addr) / block_size_) % num_sets_;
		};

		void PutData(T* key){
			int set_index = getIndex(key);
			cout<<"Set Index ="<<set_index<<endl;
			cout<<"Num Sets="<<num_sets_<<endl;
			cout<<"Set 0 ka hit_count= "<<sets[0]->set_hit_count()<<endl;
			sets[set_index]->PutData(key); //Find out which set the key belongs to
			cout<<"In PutData yo\n";
		};

		void* GetData(T* key){
			int set_index = getIndex(key);
			return sets[set_index]->GetData(key);
		};

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
			int sum = miss_count() + hit_count();
			if(sum > 0){
				miss_ratio_ = (double)(miss_count()/(sum));
				return miss_ratio_;
			}else{
				return 0;
			}
		};

		double AMAT(){
			return (double)(hit_count()*CAT + miss_count()*MMAT)/1000000000;

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
		int block_size_;
		struct Set;

		double miss_ratio_; //miss_count- / hit_count_
		int max_size_;
		int size_;
		int lines_;
		int num_sets_;
		int hit_count_;
		int miss_count_;

		bool display_graph_;

		vector<Set*> sets;
};


template<typename T>
class Kway<T>::KImpl::Set{
	public:

		struct CacheLine{
			CacheLine (int block_size): data(block_size/sizeof(T)), prev(NULL), next(NULL){}

			bool valid;
			int tag;
			vector<T> data;
			CacheLine* prev;
			CacheLine* next;
		};

		Set(int max_size, int block_size): set_max_size_(max_size), blk_size_(block_size), size_(0), head_(NULL), tail_(NULL), hit_count_(0), miss_count_(0){
		};

		void move_front(CacheLine* node){
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

		void Tempo(T* key){
			cout<<"What dis"<<endl;
			return;
		};

		void PutData(T* key){
			cout<<"Yep";
			lock_guard<mutex> lock(mutex_);

			int tag = reinterpret_cast<uintptr_t>(key); //TAG
			CacheLine* tagFinder = head_;
			while(tagFinder!=NULL){
				if(tagFinder->tag == tag){
					break;
				}
				tagFinder = tagFinder->next;
			}

			//Finds matching tag ----> Maybe if flag is true, do something and return, else create new Line and evict old;
			
			if(tagFinder != NULL){
				hit_count_++;
				move_front(tagFinder);
				int blockOffset = reinterpret_cast<uintptr_t>(key)/sizeof(T)%(blk_size_/sizeof(T)); // Finds offset (in steps of block_size and not bytes...)
			}
			
			miss_count_++;
			CacheLine* newCacheLine = new CacheLine(blk_size_);

			int intAddress = reinterpret_cast<uintptr_t>(key);
		
			int blockOffset = intAddress/sizeof(T)%(blk_size_/sizeof(T));

			int lineStart = intAddress - sizeof(T)*blockOffset;

			memcpy(&newCacheLine->data, reinterpret_cast<T*>(lineStart), blk_size_);
		};/*
			if(head_ == NULL){
				head_ = tail_ = newCacheLine;
			}else{
				head_->prev = newCacheLine;
				newCacheLine->next = head_;
				head_ = newCacheLine;
			}
			++size_;
			if(size_ > set_max_size_){
				tail_ = tail_->prev;
				delete tail_->next;
				tail_->next = NULL;
				size_--;
			}
		};*/
		void placeHolder(){
			return;
		};
		/*void PutData(T* key){
		  cout<<"Yep";
		//update AMAT at the end of each access. Could be more efficient if calculated later but requirements specify to do so.

		//traverse till you find a matching tag?. If tag present, access index within it, else, create new line and evict using lru
		lock_guard<mutex> lock(mutex_);

		int tag = reinterpret_cast<uintptr_t>(key); //TAG

		CacheLine* tagFinder = head_;
		while(tagFinder!=NULL){
		if(tagFinder->tag == tag){
		break;
		}
		tagFinder = tagFinder->next;
		}

		//Finds matching tag ----> Maybe if flag is true, do something and return, else create new Line and evict old;

		if(tagFinder != NULL){
		hit_count_++;
		move_front(tagFinder);
		int blockOffset = reinterpret_cast<uintptr_t>(key)/sizeof(T)%(blk_size_/sizeof(T)); // Finds offset (in steps of block_size and not bytes...)
		}

		miss_count_++;
		CacheLine* newCacheLine = new CacheLine(blk_size_);

		int intAddress = reinterpret_cast<uintptr_t>(key);

		int blockOffset = intAddress/sizeof(T)%(blk_size_/sizeof(T));

		int lineStart = intAddress - sizeof(T)*blockOffset;

		memcpy(&newCacheLine->data, reinterpret_cast<T*>(lineStart), blk_size_);

		if(head_ == NULL){
		head_ = tail_ = newCacheLine;
		}else{
		head_->prev = newCacheLine;
		newCacheLine->next = head_;
		head_ = newCacheLine;
		}
		++size_;
		if(size_ > set_max_size_){
		tail_ = tail_->prev;
		delete tail_->next;
		tail_->next = NULL;
		size_--;
		}

		};*/

		void* GetData(T key){
			//traverse till you find a matching tag?. If tag present, access index within it, else, create new line and evict using lru
			lock_guard<mutex> lock(mutex_);

			int tag = reinterpret_cast<uintptr_t>(key); //TAG

			CacheLine* tagFinder = head_;
			while(tagFinder!=NULL){
				if(tagFinder->tag == tag){
					break;
				}
				tagFinder = tagFinder->next;
			}

			//Finds matching tag ----> Maybe if flag is true, do something and return, else create new Line and evict old;

			if(tagFinder != NULL){
				hit_count_++;
				move_front(tagFinder);
				int blockOffset = reinterpret_cast<uintptr_t>(key)/sizeof(T)%(blk_size_/sizeof(T)); // Finds offset (in steps of block_size and not bytes...)

				return &(tagFinder->data[blockOffset*sizeof(T)]);

				//Find the offset and
			}

			// else
			PutData_(key);
			return nullptr;
		};

		//bool remove(){
		//if(set_hashmap_.find(key) == set_hashmap_.end()){
		//	return false;
		//}
		//size_--;
		//CacheLine* temp_node = set_hashmap_.at(key);

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
		//};

		int curr_size(){return size_;};

		// int set_size() {return set_max_size_;};

		int set_hit_count(){return hit_count_;};

		int set_miss_count(){return miss_count_;};


		void display_set(){
			//Dunno if viable cuz random access?...
			CacheLine* temp = head_;
			if(temp == NULL){
				cout<<" Empty ";
			}
			while(temp != NULL){
				for(int i=0; i<blk_size_; i+= sizeof(T)){
					cout<<" "<<*(T*)(&temp->data[i]);
				}
				temp = temp->next;
			}
			return;
		};

	private:
		CacheLine* head_;
		CacheLine* tail_;

		int size_;
		int set_max_size_;

		int blk_size_;
		int hit_count_;
		int miss_count_;

		mutex mutex_;
};



// template<typename T, typename U>
// class KWay<T, U>::KImpl : put()

// template<typename T, typename U>
// template<typename T, typename U>
// template<typename T, typename U>
