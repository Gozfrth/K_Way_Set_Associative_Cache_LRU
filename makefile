default:
	g++ test.cpp -lgtest -lgtest_main -pthread -o test
	g++ fib.cpp -lsfml-graphics -lsfml-window -lsfml-system -o fib_sfml

