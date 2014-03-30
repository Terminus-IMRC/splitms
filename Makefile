PROG=splitms
SRCS=splitms.c
OBJS=$(SRCS:%.c=%.c.o)
ALLDEP=$(MAKEFILE_LIST)
TOCLEAN=

all: $(PROG)

BUFFERSIZE?=1000000

CC:=cc
HEADERFLAGS:=-I.
OPTFLAGS:=-pipe -O0 -g
WARNFLAGS:=-ansi -pedantic -Wall -Wextra -W -Wundef -Wshadow -Wcast-qual -Winline -Wno-long-long -fsigned-char
VALFLAGS:=-DBUFFERSIZE=$(BUFFERSIZE)
RM:=rm -r -f
WC:=wc -c -l

COMPILE.c=$(CC) $(HEADERFLAGS) $(OPTFLAGS) $(WARNFLAGS) $(ADDCFLAGS) $(VALFLAGS) $(CFLAGS) -c
LINK.o=$(CC) $(OPTFLAGS) $(WARNFLAGS) $(LINKFLAGS) $(LDFLAGS)

$(PROG): $(OBJS) $(ALLDEP)
	$(LINK.o) $(OUTPUT_OPTION) $(OBJS)

%.c.o: %.c $(ALLDEP)
	$(COMPILE.c) $(OUTPUT_OPTION) $<

run: $(PROG)
	./$(PROG)

.PHONY: line
line:
	$(WC) $(SRCS) $(MAKEFILE_LIST)

.PHONY: clean
clean:
	$(RM) $(PROG) $(OBJS)
	$(RM) $(TOCLEAN)
