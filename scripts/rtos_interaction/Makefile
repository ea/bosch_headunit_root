CC=arm-linux-gnueabi-gcc
CFLAGS= --sysroot=${SYSROOT} -B ${SYSROOT}
LDLIBS = -ldl
DEPS = dynload.h osal_types.h util.h
OBJ = util.o dynload.o
bins = rtos_message_testing  rtos_registry_testing rtos_device_testing trace_hooking_demo 



all: $(bins) inject.so

inject.so : inject.c
	$(CC) $(CFLAGS) -o $@ $^ -shared -fPIC

$(bins) : %: %.c ${OBJ}
	$(CC) $(CFLAGS) -o $@  $^ $(LDLIBS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

deploy: $(bins) inject.so
	scp $^ root@172.17.0.1:/tmp/

clean:
	rm $(bins) *.o *.so

