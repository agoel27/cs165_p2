#
# File          : Makefile
# Description   : Build file for CS165 project 2, break it
#                 


# Environment Setup
#LIBDIRS=-L. -L/opt/local/lib
#INCLUDES=-I. -I/opt/local/include
#CC=gcc 
CC=clang
CFLAGS=-c $(INCLUDES) -g -Wall -m32 -fno-stack-protector
# CFLAGS=-c $(INCLUDES) -g -Wall 
LINK=clang -g
LDFLAGS=$(LIBDIRS) -m32 
# LDFLAGS=$(LIBDIRS)
AR=ar rc
RANLIB=ranlib

# Suffix rules
.c.o :
	${CC} ${CFLAGS} $< -o $@

#
# Setup builds

TARGETS=cs165-p2
#LIBS=-lcrypto -lm
LIBS=

#
# Project Protections

p2 : $(TARGETS)

cs165-p2 : cs165-user.o
	$(LINK) $(LDFLAGS) cs165-user.o $(LIBS) -o $@

clean:
	rm -f *.o *~ $(TARGETS)

BASENAME=p2
tar: 
	tar cvfz $(BASENAME).tgz -C ..\
	    $(BASENAME)/Makefile \
            $(BASENAME)/cs165-user.c \
	    $(BASENAME)/cs165-user.h \

basic: 
	tar cvfz $(BASENAME).tgz -C ..\
	    $(BASENAME)/Makefile \
            $(BASENAME)/cs165-user.c \
	    $(BASENAME)/cs165-user.h \
	    $(BASENAME)/test.disclosure \
	    $(BASENAME)/test.overflow \
	    $(BASENAME)/test.confusion \
	    $(BASENAME)/test.temporal \
	    $(BASENAME)/report.pdf \

extra: 
	tar cvfz $(BASENAME).tgz -C ..\
	    $(BASENAME)/Makefile \
            $(BASENAME)/cs165-user.c \
	    $(BASENAME)/cs165-user.h \
	    $(BASENAME)/test.disclosure \
	    $(BASENAME)/test.overflow \
	    $(BASENAME)/test.confusion \
	    $(BASENAME)/test.temporal \
	    $(BASENAME)/test.extra \
	    $(BASENAME)/report.pdf \

