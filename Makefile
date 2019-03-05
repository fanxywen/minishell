myShell:prompt.cpp var.cpp commandLine.cpp pipe.cpp main.cpp
	g++ -o myShell -pedantic -std=gnu++11 -Wall -Werror -ggdb3 prompt.cpp var.cpp commandLine.cpp pipe.cpp main.cpp
clean:
	rm *~
