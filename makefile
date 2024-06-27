default:
	g++ copyTest.cpp -g -lgtest -lgtest_main -pthread -o test
	g++ randoo.cpp -g -lgtest -lgtest_main -pthread -o randoo 
