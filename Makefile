all: SrcMainSerial

SrcMainSerial: SrcMainSerial.cpp sha1.h sha1.cpp
	g++ -g -Wall -std=c++11 SrcMainSerial.cpp sha1.h -o SrcMainSerial


sha-test: main_sha.cpp sha1.h sha1.cpp sha1_parallel.h sha1_parallel.cpp

clean: 
	rm -f *.o SrcMainSerial