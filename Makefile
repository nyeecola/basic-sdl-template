all: compile lint run

lint:
	cppcheck . --enable=warning,performance,portability

compile:
	mkdir -p build
	g++ src/main.cpp -O2 -o build/main.exe -lSDL2 -lSDL2_ttf -lSDL2_image -lm -Wall -Wextra -pedantic -std=c++11 -g

clean:
	rm -rf build

run:
	./build/main.exe
