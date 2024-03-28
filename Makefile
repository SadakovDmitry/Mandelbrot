TARGET = mondelbrot
TARGET+ = mondelbrot+
TARGET++ = mondelbrot++

SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, %.o, $(SRC))
FLAGS = -fsanitize=address -Wall -Wextra


.PHONY : clean
clean :
		rm *.o $(TARGET)
		rm *.o $(TARGET+)

comp :
	g++ -O3 -c Mondelbrot.cpp -o mondelbrot.o
	g++ mondelbrot.o -o $(TARGET) -lsfml-graphics -lsfml-window -lsfml-system -fpermissive

comp+ :
	g++ -O3 -c Mondelbrot+.cpp -o mondelbrot+.o
	g++ mondelbrot+.o -o $(TARGET+) -lsfml-graphics -lsfml-window -lsfml-system -fpermissive

comp++ :
	g++ -O3 -c Mondelbrot++.cpp -o mondelbrot++.o
	g++ mondelbrot++.o -o $(TARGET++) -lsfml-graphics -lsfml-window -lsfml-system -fpermissive

run : $(TARGET)
		./$(TARGET)

run+ : $(TARGET+)
		./$(TARGET+)

run++ : $(TARGET++)
		./$(TARGET++)
