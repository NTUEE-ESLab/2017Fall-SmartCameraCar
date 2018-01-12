#The following four macros should be defined.
TARGET2=yolo_test
OBJECT2=yolo_test.o
CC=gcc
CC=gcc
CXX=g++
LD_FLAGS= `pkg-config --cflags --libs opencv`
C_FLAGS=
# end of user configuration
all: $(TARGET2)
$(TARGET2) : $(OBJECT2)
	$(CXX) $^ -o $@ $(LD_FLAGS)
%.o : %.c
%.o : %.c
	$(CC) $(C_FLAGS) -c $<
%.o : %.cpp
	$(CXX) $(C_FLAGS) -c $<
.PHONY: clean
clean :
	-rm -f $(TARGET2) *.o
