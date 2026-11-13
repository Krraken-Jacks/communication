include ../shared.mk


SOURCES:=$(wildcard *.c) \
$(wildcard comms/*.c) \
		 $(wildcard comms/core/*.c)
CFLAGS+=-Werror -Icomms -Icomms/core 

include ../end.mk
