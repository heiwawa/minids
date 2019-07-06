
CC:= gcc

INCLUDE+=../src
CFLAGS+=-I$(INCLUDE)
LDFLAGS+=
LIBS+=

SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o, $(SRCS))
TARGET:=$(patsubst %.c,%.out, $(SRCS))

all:$(TARGET)

%.out:%.o
	$(CC) $(CFLAGS) -o $@ $< ../src/$(patsubst test_%.o,%.o, $<) $(LDFLAGS) $(LIBS)

%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $< $(LDFLAGS) $(LIBS)

%.d:%.c
	@set -e; rm -f $@; \
	$(CC) $(CFLAGS) -MM $< > $@.$$$$; \
	sed -i 's/$$/ ..\/src\/$(patsubst test_%.c,%.c, $<)/' $@.$$$$; \
	sed 's,/($*/)/.o[ :]*,/1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(SRCS:.c=.d)

clean:
	rm -f *.o *.d *.out

.PHONY:all clean
