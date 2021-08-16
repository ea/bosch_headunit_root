CC=arm-linux-gnueabi-gcc
CFLAGS= --sysroot=${SYSROOT} -B ${SYSROOT}
LDLIBS = -ldl
DEPS = dynload.h osal_types.h util.h
OBJ = util.o dynload.o
bins = osal_dir_read  osal_device_read rtos_message_register rtos_registry_testing kds_dev_read kds_dev_write trace_hooking_demo 



all: $(bins) inject.so

inject.so : inject.c
	$(CC) $(CFLAGS) -o $@ $^ util.o -shared -fPIC -ldl

$(bins) : %: %.c ${OBJ}
	$(CC) $(CFLAGS) -o $@  $^ $(LDLIBS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

deploy: $(bins) inject.so strace
	scp $^ root@172.17.0.1:/tmp/

clean:
	rm $(bins) *.o *.so

