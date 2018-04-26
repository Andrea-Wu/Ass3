TARGET1 = server
TARGET2 = test_client
OBJECTS1 = server.o
OBJECTS2 = libnetfiles.o util.o test_client.o
FLAGS = -g -fsanitize=address -lm

all: $(TARGET1) $(TARGET2)

$(TARGET1): $(OBJECTS1)
	gcc $(FLAGS) -pthread -o $@ $^

$(TARGET2): $(OBJECTS2)
	gcc $(FLAGS) -o $@ $^

clean:
	rm -f $(TARGET1) $(TARGET2) $(OBJECTS1) $(OBJECTS2)

%.o: %.c
	gcc $(FLAGS) -c $<

libnetfiles.o: libnetfiles.h
util.o: util.h
test_client.o: libnetfiles.h util.h

