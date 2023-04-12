.PHONY: all clean

all: compile run_taylor run_python_taylor run_all_taylor

compile:
	g++ -std=c++14 -o run taylor.cpp -isystem /usr/include/eigen3

run_taylor: compile
	./run

run_python_taylor:
	python3 taylor.py

run_all_taylor:
	python3 all-taylor.py

clean:
	rm -f run
