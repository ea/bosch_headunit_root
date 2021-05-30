CC=arm-linux-gnueabi-gcc
CFLAGS= --sysroot=${SYSROOT} -B ${SYSROOT}
LDLIBS = -ldl
DEPS = dynload.h osal_types.h util.h
OBJ = util.o dynload.o
bins = rtos_message_testing  rtos_registry_testing rtos_device_testing

all: $(bins)

$(bins) : %: %.c ${OBJ}
	$(CC) $(CFLAGS) -o $@  $^ $(LDLIBS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

deploy:
	scp $(bins) root@172.17.0.1:/tmp/

clean:
	rm $(bins) *.o

