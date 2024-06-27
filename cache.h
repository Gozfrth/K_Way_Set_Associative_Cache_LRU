//Strong references from  https://github.com/lamerman/cpp-lru-cache, https://github.com/ekg/lru_cache

// Assumptions
// 1. Assume that we are developing this cache to store integers or chars only.
// You can choose any data type to store this.
// 2. Assume that the data is always available in Main Memory.
// 3. Cache Access Time = 10 ns and Main Memory access time = 1 microsec-
// ond.

// TODO:
// Clean up implementation
// Find bugs / handle cases where k isnt power of 2 or variable block_size?
// Add AMAT into the csv file

#pragma once
#define CAT 10
// CAT- Cache Access time (10 nanoseconds)
#define MMAT 1000
// MMAT- Main Memory Access Time (1 microsecond = 1000 nanoseconds)
#define PLOT_STEP 100
// PLOT_STEP- Interval at which data points are plotted in csv file

#include<iostream>
#include<mutex>
#include<memory>
#include<utility>
#include<cmath>
#include<unordered_map>
#include<cstring>
#include<vector>
#include<fstream>

#include <SFML/Graphics.hpp>

using namespace std;

template<typename T>
class Kway{
	public:
		//constructor default block_size = 1 element
		Kway(int max_size, int k, int block_size = sizeof(T), bool plot = false, string fName = ""); 

		void logCounts(ofstream& logFile, int step=10){return KImpl_->logCounts(logFile, step);};
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

		void initGraph(string fName){return KImpl_->initGraph(fName);};

		void terminateGraph(){return KImpl_->terminateGraph();};

	private:
		struct KImpl;
		unique_ptr<KImpl> KImpl_;

		//		Kway(const Kway&); // new object being created with contents of another object
		//		void operator=(const Kway&);
};
template<typename T>
Kway<T>::Kway(int max_size, int k, int block_size, bool plot, string fName) : KImpl_(new KImpl(max_size, k, block_size, plot, fName)) {}

template<typename T>
class Kway<T>::KImpl{
	public:
		//actual implementation
		KImpl(int max_size, int k, int block_size, bool plot = false, string fName = "") : max_size_(max_size), lines_(k), size_(0), hit_count_(0), miss_count_(0), block_size_(block_size), plot_(plot), fName_(fName) {
			num_sets_ = ceil(max_size / (k *block_size));
			for (int i = 0; i < num_sets_; i++) {
				sets.push_back(new Set(lines_, block_size, num_sets_));
			};

		};

		int getIndex(T* addr){
			return (reinterpret_cast<uintptr_t>(addr) / block_size_) % num_sets_;
		};

		void logCounts(ofstream& logFile, int step) {
			logFile << step << "," << hit_count() << "," << miss_count() << endl;
			logFile.flush(); // Flush the buffer to ensure data is written immediately
			if (logFile.fail()) {
				cerr << "Error writing to log file!" << endl;
			}
		}

		void PutData(T* key){
			int set_index = getIndex(key);
			sets[set_index]->PutData(key); //Find out which set the key belongs to
			step_++;
			if(plot_ == true && step_%PLOT_STEP == 0){
				logCounts(logFile_, step_);
			}
		};

		void* GetData(T* key){
			int set_index = getIndex(key);
			return sets[set_index]->GetData(key);
			step_++;
			if(plot_ == true && step_%PLOT_STEP == 0){
				logCounts(logFile_, step_);
			}
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

		void initGraph(string fName){
			logFile_.open(fName + ".csv");
			logFile_<<"Step,HitCount,MissCount"<<endl;
		};

		void terminateGraph(){
			logFile_.close();
		};

	private:
		int block_size_;
		struct Set;
		ofstream logFile_;
		string fName_;

		double miss_ratio_; //miss_count- / hit_count_
		int max_size_;
		int size_;
		int lines_;
		int num_sets_;
		int hit_count_;
		int miss_count_;
		bool plot_;
		int step_ = 0;

		vector<Set*> sets;
};


template<typename T>
class Kway<T>::KImpl::Set{
	public:

		struct CacheLine{
			CacheLine (int block_size, int tag_): data(block_size/sizeof(T)), tag(tag_), prev(NULL), next(NULL){}

			bool valid;
			int tag;
			vector<T> data;
			CacheLine* prev;
			CacheLine* next;
		};

		Set(int max_size, int block_size, int num_sets_): set_max_size_(max_size), blk_size_(block_size), num_sets(num_sets_), size_(0), head_(nullptr), tail_(nullptr), hit_count_(0), miss_count_(0){
		};

		void move_front(CacheLine* node) {
			if (head_ == node) {
				return;
			}

			if (node == tail_) {
				tail_ = node->prev;
			}

			if (node->prev) {
				node->prev->next = node->next;
			}

			if (node->next) {
				node->next->prev = node->prev;
			}

			node->next = head_;
			node->prev = nullptr;

			if (head_) {
				head_->prev = node;
			}

			head_ = node;

			if (tail_ == nullptr) {
				tail_ = node;
			}
		}

		void Tempo(T* key){
			cout<<"What dis"<<endl;
			return;
		};

		void PutData(T* key) {
			lock_guard<mutex> lock(mutex_);

			int tag = (reinterpret_cast<uintptr_t>(key) / (blk_size_ * num_sets)); // TAG
			CacheLine* tagFinder = head_;

			while (tagFinder != nullptr) {
				if (tagFinder->tag == tag) {
					// Tag Found
					break;
				}
				tagFinder = tagFinder->next;
			}

			// Finds matching tag ----> Maybe if flag is true, do something and return, else create new Line and evict old;
			if (tagFinder != nullptr) {
				hit_count_++;
				move_front(tagFinder);
				return;
			}

			miss_count_++;

			int intAddress = reinterpret_cast<uintptr_t>(key);
			int blockOffset = (intAddress / sizeof(T)) % (blk_size_ / sizeof(T));

			CacheLine* newCacheLine = new CacheLine(blk_size_, tag);
			memcpy(newCacheLine->data.data(), key - blockOffset, blk_size_);

			newCacheLine->next = head_;
			if (head_ != nullptr) {
				head_->prev = newCacheLine;
			}
			head_ = newCacheLine;

			if (tail_ == nullptr) {
				tail_ = newCacheLine;
			}

			size_++;
			if (size_ > set_max_size_) {
				if (tail_ != nullptr) {
					CacheLine* oldTail = tail_;
					tail_ = tail_->prev;
					if(tail_ != nullptr) {
						tail_->next = nullptr;
					}
					delete oldTail;
				}
				size_--;
			}
		};

		void placeHolder(){
			return;
		};

		void* GetData(T* key){
			//traverse till you find a matching tag?. If tag present, access index within it, else, create new line and evict using lru
			lock_guard<mutex> lock(mutex_);

			int tag = (reinterpret_cast<uintptr_t>(key) / (blk_size_ * num_sets)); // TAG

			CacheLine* tagFinder = head_;
			while(tagFinder!=nullptr){
				if(tagFinder->tag == tag){
					// Tag Found
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

			miss_count_++;

			int intAddress = reinterpret_cast<uintptr_t>(key);
			int blockOffset = (intAddress / sizeof(T)) % (blk_size_ / sizeof(T));

			CacheLine* newCacheLine = new CacheLine(blk_size_, tag);
			memcpy(newCacheLine->data.data(), key - blockOffset, blk_size_);

			newCacheLine->next = head_;
			if (head_ != nullptr) {
				head_->prev = newCacheLine;
			}
			head_ = newCacheLine;

			if (tail_ == nullptr) {
				tail_ = newCacheLine;
			}

			size_++;
			if (size_ > set_max_size_) {
				if (tail_ != nullptr) {
					CacheLine* oldTail = tail_;
					tail_ = tail_->prev;
					if (tail_ != nullptr) {
						tail_->next = nullptr;
					}
					delete oldTail;
				}
				size_--;
			}
			return nullptr;
		};

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
		int num_sets;

		mutex mutex_;
};
