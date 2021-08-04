SRCDIR	= src
HDRDIR	= hdr
OBJDIR	= obj
BINDIR	= bin

CC		= gcc
TARGET	= server_select
CFLAGS	= -g -Wall -I$(HDRDIR)

SOURCES	:= $(wildcard $(SRCDIR)/*.c)
HEADERS	:= $(wildcard $(HDRDIR)/*.h)
OBJECTS	:= $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

REMOVE	:= rm -rf

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lpthread

.PHONY: clean
clean:
	$(REMOVE) $(BINDIR)/$(TARGET)
	$(REMOVE) $(OBJECTS)
	@echo "removed $<"

.PHONY: all
all: $(BINDIR)/$(TARGET)
