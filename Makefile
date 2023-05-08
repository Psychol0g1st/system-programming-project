CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -fopenmp
OBJFILES = main.c include/helper/helper.c include/init/init.c include/info/info.c include/data/data.c include/bmp/bmp.c include/pid/pid.c include/communication/communication.c 
TARGET = chart

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET) *~