#include <iostream>
#include <time.h>
#include "cache.h"


using namespace std;

int fibonacci(int n){
	if(n<=1)
		return n;
	return fibonacci(n-1) + fibonacci(n-2);
}


Kway<int, int> cache(400000, 4);
int fibonacci_cached(int n){
	if(n<=1)
		return n;
	if(cache.GetData(n) != nullptr)	
		return *(int*)cache.GetData(n);
	cache.PutData(n, fibonacci_cached(n-1) + fibonacci_cached(n-2));
	return *(int*)cache.GetData(n);
}

int main(){

	int fib=0;
	int n=45;
	clock_t start1 = clock();
	fib = fibonacci(n);
	clock_t end1 = clock();
	cout<<"Fibonacci without caching: "<<fib<<"\n Time taken: "<<((double)(end1-start1)/CLOCKS_PER_SEC)<<"s\n\n"<<endl;
	
	clock_t start2 = clock();
	fib = fibonacci_cached(n);
	clock_t end2 = clock();
	cout<<"Fibonacci with caching: "<<fib<<"\n Time taken: "<<((double)(end2-start2)/CLOCKS_PER_SEC)<<"s\n\n"<<endl;
	
	return 1;
}
