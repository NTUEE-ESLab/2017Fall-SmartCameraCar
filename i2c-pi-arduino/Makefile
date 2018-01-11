#The following macros should be defined.
TARGET1=i2c-pi-arduino
OBJECT1=i2c-pi-arduino.o
TARGET2=hardstop
OBJECT2=hardstop.o
TARGET3=forward
OBJECT3=forward.o
TARGET4=backward
OBJECT4=backward.o
TARGET5=right
OBJECT5=right.o
TARGET6=left
OBJECT6=left.o
#TARGET1=i2c-pi-Arduino_Asynchronous_GPIO
#OBJECT1=i2c-pi-Arduino_Asynchronous_GPIO.o
CC=gcc
CXX=g++
LD=g++
#LD_FLAGS += -lrt
#LD_FLAGS += -lwiringPi
#LD_FLAGS += -lncurses -pthread
#C_FLAGS= -g -O0
C_FLAGS=
# end of user configuration
#
ALL : $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) $(TARGET5) $(TARGET6)
$(TARGET1) : $(OBJECT1)
	$(LD)  $^ $(LD_FLAGS) -o $@
$(TARGET2) : $(OBJECT2)
	$(LD)  $^ $(LD_FLAGS) -o $@
$(TARGET3) : $(OBJECT3)
	$(LD)  $^ $(LD_FLAGS) -o $@
$(TARGET4) : $(OBJECT4)
	$(LD)  $^ $(LD_FLAGS) -o $@
$(TARGET5) : $(OBJECT5)
	$(LD)  $^ $(LD_FLAGS) -o $@
$(TARGET6) : $(OBJECT6)
	$(LD)  $^ $(LD_FLAGS) -o $@
%.o : %.c
	$(CC) $(C_FLAGS) -c $<
%.o : %.cpp
	$(CXX) $(C_FLAGS) -c $<
.PHONY: clean
clean :
	-rm -f $(OBJECT1) $(OBJECT2)
