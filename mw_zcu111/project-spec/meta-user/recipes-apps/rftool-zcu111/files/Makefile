APP = rftool-zcu111

# Add any other object files to this list below
SRCS=$(wildcard *.c)
APP_OBJS = $(SRCS:.c=.o)

all: $(APP)

$(APP): $(APP_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(APP_OBJS) $(LDLIBS) -lsysfs -lrfdc -lmetal -lpthread -lconfig -lm

clean:
	-rm -f $(APP) *.elf *.gdb *.o


