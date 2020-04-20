all: SrcMainSerial

SrcMainSerial: SrcMainSerial.cpp sha1.h sha1.cpp
	g++ -g -Wall -std=c++11 SrcMainSerial.cpp sha1.h -o SrcMainSerial

clean: 
	rm -f *.o SrcMainSerial