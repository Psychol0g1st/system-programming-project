CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS =
OBJFILES = include/helper/helper.o include/init/init.o include/info/info.o include/data/data.o include/bmp/bmp.o include/pid/pid.o include/communication/communication.o main.o
TARGET = chart

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET) *~