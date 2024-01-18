CC=g++
INCLUDE_DIRS = -I./ -I../
CFALGS= -Wall -Wl,--stack,8388608 -g $(INCLUDE_DIRS)

SOURCES = test.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = mjtest

all:$(EXECUTABLE)

$(EXECUTABLE):$(OBJECTS)
	$(CC) $(CFALGS) $(OBJECTS) -o $@

%.o:%.cpp
	$(CC) $(CFALGS) -c $< -o $@

clean:
	del