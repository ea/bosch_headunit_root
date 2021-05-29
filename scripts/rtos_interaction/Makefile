CC=arm-linux-gnueabi-gcc
LDFLAGS = -ldl
DEPS = dynload.h osal_types.h util.h

.PHONY: default
default: rtos_message_testing  rtos_registry_testing rtos_iosc_testing;

rtos_message_testing: rtos_message_testing.c util.c dynload.c
	$(CC) --sysroot=${SYSROOT} -B ${SYSROOT} -o rtos_message_testing rtos_message_testing.c util.c dynload.c $(LDFLAGS) 

rtos_registry_testing: rtos_registry_testing.c util.c dynload.c
	$(CC) --sysroot=${SYSROOT} -B ${SYSROOT} -o rtos_registry_testing rtos_registry_testing.c util.c dynload.c $(LDFLAGS) 

rtos_iosc_testing: rtos_iosc_testing.c util.c dynload.c
	$(CC) --sysroot=${SYSROOT} -B ${SYSROOT} -o rtos_iosc_testing rtos_iosc_testing.c util.c dynload.c $(LDFLAGS) 



clean:
	rm rtos_message_testing rtos_registry_testing rtos_iosc_testing

#arm-linux-gnueabi-gcc   --sysroot=/home/ea/xterra/lx001/ -B /home/ea/xterra/lx001/ -shared -fPIC -o inject.so ld_pre.c
#arm-linux-gnueabi-gcc   --sysroot=/home/ea/xterra/lx001/ -B /home/ea/xterra/lx001/ ~/rtos_message_testing.c -o ~/rtos_message_testing -ldl