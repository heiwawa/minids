
SUB_DIRS:=$(shell ls -l | grep ^d | awk '{print $$9}')
CUR_SOURCE:=${wildcard *.c}
CUR_OBJS:=${patsubst %.c, %.o, $(CUR_SOURCE)}

LDFLAGS+=-L..
LIBS+=-lminids-$(VER)

all:$(SUB_DIRS) $(CUR_OBJS)

$(CUR_OBJS):%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@
	$(CC) $(CFLAGS) $@ -o ${patsubst %.o,%.out, $@} $(LDFLAGS) $(LIBS)

$(SUB_DIRS):ECHO
	make -C $@
 
ECHO:
	@echo $(SUBDIRS)

clean:
	rm -f *.o *.out

distclean: clean

.PHONY:all ECHO clean distclean
