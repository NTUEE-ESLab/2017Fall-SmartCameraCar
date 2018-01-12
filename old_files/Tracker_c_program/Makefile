#The following four macros should be defined.
TARGET1=opencvTrackTest
OBJECT1=opencvTrackTest.o
TARGET2=TrackerReturnNum
OBJECT2=TrackerReturnNum.o
TARGET3=TrackerReturnNum_localver
OBJECT3=TrackerReturnNum_localver.o
CC=gcc
CC=gcc
CXX=g++
LD_FLAGS= `pkg-config --cflags --libs opencv`
C_FLAGS=
# end of user configuration
all: $(TARGET1) $(TARGET2) $(TARGET3)
$(TARGET1) : $(OBJECT1)
	$(CXX) $^ -o $@ $(LD_FLAGS)
$(TARGET2) : $(OBJECT2)
	$(CXX) $^ -o $@ $(LD_FLAGS)
$(TARGET3) : $(OBJECT3)
	$(CXX) $^ -o $@ $(LD_FLAGS)
%.o : %.c
%.o : %.c
	$(CC) $(C_FLAGS) -c $<
%.o : %.cpp
	$(CXX) $(C_FLAGS) -c $<
.PHONY: clean
clean :
	-rm -f $(TARGET1) $(TARGET2) $(TARGET3) *.o
