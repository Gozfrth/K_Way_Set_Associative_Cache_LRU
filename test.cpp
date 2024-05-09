#include "nincompoop.h"

int main(){
	Kway<int, int> kway_cache(128, 3);
	
	for(int i=0; i<10; i++){
		kway_cache.PutData(i, i);
	}
	
	cout<<""<<kway_cache.num_lines()<<""<<endl;
	cout<<""<<kway_cache.num_sets()<<""<<endl;
	cout<<""<<kway_cache.miss_count()<<""<<endl;
	cout<<""<<kway_cache.hit_count()<<""<<endl;

	for(int i=0; i<10; i++){
		cout<<*kway_cache.GetData(i);
	}
	cout<<*kway_cache.GetData(1)<<endl;
	kway_cache.remove(1);
	cout<<*kway_cache.GetData(1)<<endl;
	kway_cache.remove(1);
	kway_cache.remove(1);
	
	return 1;
}
