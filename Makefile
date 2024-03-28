TARGET = mandelbrot
TARGET+ = mandelbrot+
TARGET++ = mandelbrot++
TARGET256++ = mandelbrot256++

SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, %.o, $(SRC))
FLAGS = -fsanitize=address -Wall -Wextra -O3 -mavx -mavx2


.PHONY : clean
clean :
		rm *.o $(TARGET)
		rm *.o $(TARGET+)
		rm *.o $(TARGET++)
		rm *.o $(TARGET256++)

comp :
	g++ -O3 -c Mandelbrot.cpp -o mandelbrot.o
	g++ mandelbrot.o -o $(TARGET) -lsfml-graphics -lsfml-window -lsfml-system -fpermissive

comp+ :
	g++ -O3 -c Mandelbrot+.cpp -o mandelbrot+.o
	g++ mandelbrot+.o -o $(TARGET+) -lsfml-graphics -lsfml-window -lsfml-system -fpermissive

comp++ :
	g++ -O3 -mavx -mavx2 -c Mandelbrot++.cpp -o mandelbrot++.o
	g++ mandelbrot++.o -o $(TARGET++) -lsfml-graphics -lsfml-window -lsfml-system -fpermissive

comp256++ :
	g++ -O3 -mavx -mavx2 -c Mandelbrot256++.cpp -o mandelbrot256++.o
	g++ mandelbrot256++.o -o $(TARGET256++) -lsfml-graphics -lsfml-window -lsfml-system -fpermissive

run : $(TARGET)
		./$(TARGET)

run+ : $(TARGET+)
		./$(TARGET+)

run++ : $(TARGET++)
		./$(TARGET++)

run256++ : $(TARGET256++)
		./$(TARGET256++)
