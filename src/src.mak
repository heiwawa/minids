
CC:= gcc

INCLUDE+=
CFLAGS+=
LDFLAGS+=
LIBS+=

SRCS:= $(wildcard *.c)
OBJS:= $(patsubst %.c,%.o, $(SRCS))

all:$(OBJS)

%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $< $(LDFLAGS) $(LIBS)

%.d:%.c %.h
	@set -e; rm -f $@; \
	$(CC) -MM  $< > $@.$$$$; \
	sed 's,/($*/)/.o[ :]*,/1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(SRCS:.c=.d)

clean:
	rm -f *.o *.d

.PHONY:all clean
