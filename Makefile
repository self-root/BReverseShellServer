cpp := -std=c++2a

server: main.o server.o clientconnection.o
	g++ $(cpp) main.o server.o clientconnection.o -o server -lpthread

main.o: main.cpp
	g++ $(cpp) -c main.cpp

server.o: server.cpp server.hpp clientconnection.hpp 
	g++ $(cpp) -c server.cpp

clientconnection.o: clientconnection.cpp clientconnection.hpp utils/utility.h
	g++ $(cpp) -c clientconnection.cpp

clean:
	rm -f *.o server