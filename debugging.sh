make && g++ -std=c++11 -o bin/debugging.exec -Iinc -Llib debugging.cpp -lclinterp && ./bin/debugging.exec #valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./bin/debugging.exec
