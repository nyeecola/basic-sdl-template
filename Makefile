all: lint compile run

lint:
	cppcheck . --enable=warning,performance,portability --error-exitcode=5

compile:
	mkdir -p build
	g++ main.cpp -O0 -o build/main.exe -lSDL2 -lSDL2_ttf -lm -Wall -Wextra -pedantic -std=c++11

clean:
	rm -rf build

run:
	./build/main.exe
