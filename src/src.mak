
SUB_DIRS:=$(shell ls -l | grep ^d | awk '{print $$9}')
CUR_SOURCE:=${wildcard *.c}
CUR_OBJS:=${patsubst %.c, %.o, $(CUR_SOURCE)}

LIBS+=

all:$(SUB_DIRS) $(CUR_OBJS)

$(CUR_OBJS):%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@ $(LDFLAGS) $(LIBS)

$(SUB_DIRS):ECHO
	make -C $@

ECHO:
	@echo $(SUBDIRS)

clean:
	rm -f *.o

distclean: clean

.PHONY:all ECHO clean distclean
