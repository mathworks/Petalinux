APP = rftool

# Add any other object files to this list below
SRCS=$(wildcard *.c)
APP_OBJS = $(SRCS:.c=.o)
CFLAGS += -DXPS_BOARD_ZCU216

all: $(APP)

$(APP): $(APP_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(APP_OBJS) $(LDLIBS) -lrfdc -lrfclk -lmetal -lpthread

clean:
	-rm -f $(APP) *.elf *.gdb *.o


