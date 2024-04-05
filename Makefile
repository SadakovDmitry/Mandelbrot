TARGET      = mandelbrot
TARGET+     = mandelbrot+
TARGET++    = mandelbrot++
TARGET256++ = mandelbrot256++
OPT = -O0
TESTING_MODE = -DTESTING_MODE
#TESTING_MODE =

SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, %.o, $(SRC))
FLAGS = -fsanitize=address -Wall -Wextra -O3 -mavx -mavx2


.PHONY : clean
clean :
		rm *.o $(TARGET)
		rm *.o $(TARGET+)
		rm *.o $(TARGET++)
		rm *.o $(TARGET256++)
		rm accuracy.txt


comp      :
	@g++ $(OPT) $(TESTING_MODE) -c Mandelbrot.cpp -o mandelbrot.o
	@g++ mandelbrot.o -o $(TARGET) -lsfml-graphics -lsfml-window -lsfml-system -fpermissive

comp+     :
	@g++ $(OPT) $(TESTING_MODE) -c Mandelbrot+.cpp -o mandelbrot+.o
	@g++ mandelbrot+.o -o $(TARGET+) -lsfml-graphics -lsfml-window -lsfml-system -fpermissive

comp128++ :
	@g++ $(OPT) $(TESTING_MODE) -mavx -mavx2 -c Mandelbrot++.cpp -o mandelbrot++.o
	@g++ mandelbrot++.o -o $(TARGET++) -lsfml-graphics -lsfml-window -lsfml-system -fpermissive

comp256++ :
	g++ $(OPT) $(TESTING_MODE) -mavx -mavx2 -c Mandelbrot256++.cpp -o mandelbrot256++.o
	g++ mandelbrot256++.o -o $(TARGET256++) -lsfml-graphics -lsfml-window -lsfml-system -fpermissive

run      : $(TARGET)
	@./$(TARGET)

run+     : $(TARGET+)
	@./$(TARGET+)

run128++ : $(TARGET++)
	@./$(TARGET++)

run256++ : $(TARGET256++)
	./$(TARGET256++)

tests    :
	@./testing.sh mandelbrot256++ comp256++ M_256++:
	@./testing.sh mandelbrot++ comp128++ M_128++:
	@./testing.sh mandelbrot+ comp+ M_+:
	@./testing.sh mandelbrot comp M_0:



