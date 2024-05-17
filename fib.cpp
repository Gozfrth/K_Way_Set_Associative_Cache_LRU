#include <iostream>
#include <time.h>
#include <thread>
#include <bits/stdc++.h>
#include "cache.h"
#include <SFML/Graphics.hpp>

using namespace std;

const int rows = 50;
const int cols = 120;
const int cellSize = 12;
const int delay = 40;
const bool buttoned = true;
bool done = false;

sf::Font font;


unordered_map<int, double> missRatioData;
unordered_map<int, double> AMATData;

int fibonacci(int n){
	if(n<=1)
		return n;
	return fibonacci(n-1) + fibonacci(n-2);
}


Kway<int, int> cache(400000, 4);

void pushMissRatio(){
	int time=0;
	while(!done){
		double missRatio = cache.miss_ratio();

		missRatioData[time] = missRatio;

		++time;
		cout<<"1"<<endl;
		std::this_thread::sleep_for(std::chrono::nanoseconds(25));
	}
}

void pushAMAT(){
	int time=0;
	while(!done){
		double amat = cache.AMAT();

		AMATData[time] = amat;

		++time;

		std::this_thread::sleep_for(std::chrono::nanoseconds(25));
	}
}

int actual(int n){
	if(n<=1)
		return n;
	if(cache.GetData(n) != nullptr)	
		return *(int*)cache.GetData(n);
	cache.PutData(n, actual(n-1) + actual(n-2));
	return *(int*)cache.GetData(n);
}
int fibonacci_cached(int n){
	int x = actual(n);
	done = true;
	return x;
}

int main(){


	int fib=0, n=43;
	clock_t start1 = clock();
	fib = fibonacci(n);
	clock_t end1 = clock();
	cout<<"Fibonacci without caching: "<<fib<<"\n Time taken: "<<((double)(end1-start1)/CLOCKS_PER_SEC)<<"s\n\n"<<endl;


	thread AMATThread(pushAMAT);
	thread missRatioThread(pushMissRatio);
	thread cacl_threaded_one(fibonacci_cached, n);
	clock_t start2 = clock();
	missRatioThread.join();
	AMATThread.join();
	cacl_threaded_one.join();
	clock_t end2 = clock();
	cout<<"Fibonacci with caching: "<<fib<<"\n Time taken: "<<((double)(end2-start2)/CLOCKS_PER_SEC)<<"s\n\n"<<endl;
	cout<<"1\n";
	
	sf::RenderWindow window(sf::VideoMode(800, 800), "Cache Performance Plot");
	sf::Image image;
	image.create(window.getSize().x, window.getSize().y, sf::Color::Black);

	sf::Texture texture;
	sf::Sprite sprite;

	int denser = missRatioData.size() / 800;
	if(denser == 0){
		cout<<missRatioData.size()<<endl;
		cout<<"Less info"<<endl;
		exit(1);
	}
	while(window.isOpen()){
		sf::Event sfmlEvent;
		while(window.pollEvent(sfmlEvent)){
			if(sfmlEvent.type == sf::Event::Closed){
				window.close();
			}
		}

		for(int i = 0; i<800*denser; i++){
			float x = 0;
			int iter = 0;
			while(iter < denser){
				x +=(float) missRatioData[i*denser + iter];
			}
			x /= denser;
			image.setPixel(i, x*800, {10, 75, 200});

		}
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		window.draw(sprite);

		window.display();
	}
	return 1;
}
