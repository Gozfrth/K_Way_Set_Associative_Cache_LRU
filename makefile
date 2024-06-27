default:
	g++ test.cpp -g -lgtest -lgtest_main -pthread -o test
	g++ randoo.cpp -g -lgtest -lgtest_main -pthread -o randoo 
	streamlit run HOME.py
