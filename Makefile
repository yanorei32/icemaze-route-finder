CFLAGS = -Wextra -O3 --std=c99 --pedantic
DEST = .
LD = $(CC)
LDFLAGS =
OBJS = main.o list.o
PROGRAM = finder
HDRS = list.h
SRCS = list.c main.c

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(PROGRAM)

$(OBJS):$(SRCS) $(HDRS)

clean:
	rm -f $(OBJS) $(PROGRAM)
