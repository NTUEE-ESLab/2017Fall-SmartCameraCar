#The following four macros should be defined.
TARGET1=Tracker_and_I2C
OBJECT1=Tracker_and_I2C.o
CC=gcc
CC=gcc
CXX=g++
LD_FLAGS= `pkg-config --cflags --libs opencv` -lwiringPi
C_FLAGS=
# end of user configuration
all: $(TARGET1)
$(TARGET1) : $(OBJECT1)
	$(CXX) $^ -o $@ $(LD_FLAGS)
%.o : %.c
	$(CC) $(C_FLAGS) -c $<
%.o : %.cpp
	$(CXX) $(C_FLAGS) -c $<
.PHONY: clean
clean :
	-rm -f $(TARGET1) *.o
