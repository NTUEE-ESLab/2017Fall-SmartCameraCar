#The following macros should be defined.
TARGET1=server
OBJECT1=server.o
TARGET2=client
OBJECT2=client.o

CC=gcc
CXX=g++
LD=g++
LD_FLAGS += -lboost_system
#LD_FLAGS += -lwiringPi
#LD_FLAGS += -lncurses -pthread
#C_FLAGS= -g -O0
C_FLAGS=
# end of user configuration
#
ALL : $(TARGET1) $(TARGET2)
$(TARGET1) : $(OBJECT1)
	$(LD)  $^ $(LD_FLAGS) -o $@
$(TARGET2) : $(OBJECT2)
	$(LD)  $^ $(LD_FLAGS) -o $@
%.o : %.c
	$(CC) $(C_FLAGS) -c $<
%.o : %.cpp
	$(CXX) $(C_FLAGS) -c $<
.PHONY: clean
clean :
	-rm -f $(TARGET1) $(TARGET2) *.o
