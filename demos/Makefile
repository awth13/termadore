include ../Makefile.inc

CFLAGS += -I${INCLDIR}

DEMOSRC = $(wildcard *.c)
DEMOS = $(patsubst %.c, %, $(DEMOSRC))

all : $(DEMOS)

.PHONY : clean
clean :
	${RM} ${DEMOS} *.o

${DEMOS} : ${RENDERLIB} ${SIGHANDLER} ${MATHLIB}
