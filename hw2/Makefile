INC=/lib/modules/$(shell uname -r)/build/arch/x86/include

all: treplay trctl

treplay: treplay.c
	gcc -Wall -Werror -I$(INC)/generated/uapi -I$(INC)/uapi treplay.c -o treplay.o

trctl: trctl.c
	gcc -Wall -Werror -I$(INC)/generated/uapi -I$(INC)/uapi trctl.c -o trctl.o
