CLIENTSRC=client_utils client_ui
SERVERSRC=server_utils
SHAREDSRC=sys_utils main

LIBS=-lpanel -lncurses
FLAGS=-Wall -std=c99 -I "./header" -D "_XOPEN_SOURCE"

CC=gcc
LD=gcc

CFLAGS=$(FLAGS)
LDFLAGS=$(FLAGS) $(LIBS)

CLIENTOBJS=$(addprefix object/, $(addsuffix .o, $(CLIENTSRC)))
SERVEROBJS=$(addprefix object/, $(addsuffix .o, $(SERVERSRC)))
SHAREDOBJS=$(addprefix object/, $(addsuffix .o, $(SHAREDSRC)))

all: client server
server: $(SHAREDOBJS) $(SERVEROBJS) object/server.o
	$(LD) $^ $(LDFLAGS) -o $@
client: $(SHAREDOBJS) $(CLIENTOBJS) object/client.o
	$(LD) $^ $(LDFLAGS) -o $@

object/%.o: source/%.c header/%.h
	$(CC) -c $< $(CFLAGS) -o $@
clean:
	rm -f server client object/*.o