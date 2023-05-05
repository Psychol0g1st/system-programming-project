CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -fopenmp
OBJFILES = include/helper/helper.o include/init/init.o include/info/info.o include/data/data.o include/bmp/bmp.o include/pid/pid.o include/communication/communication.o main.o
TARGET = chart

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) -fopenmp $(CFLAGS) -o $(TARGET) $(OBJFILES)

clean:
	rm -f $(OBJFILES) $(TARGET) *~