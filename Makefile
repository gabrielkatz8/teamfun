all: SrcMainSerial

SrcMainSerial: SrcMainSerial.cpp sha1.h sha1.cpp
	g++ -g -Wall -std=c++11 SrcMainSerial.cpp sha1.h -o SrcMainSerial

ParallelPass: ParallelPass.cpp sha1.h sha1.cpp
	g++ -g -Wall -std=c++11 ParallelPass.cpp sha1.h -o ParallelPass -fopenmp

ParallelSym: ParallelSym.cpp sha1.h sha1.cpp
	g++ -g -Wall -std=c++11 ParallelSym.cpp sha1.h -o ParallelSym -fopenmp

clean: 
	rm -f *.o SrcMainSerial ParallelPass ParallelSym