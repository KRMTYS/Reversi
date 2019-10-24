INCDIR   := include
SRCDIR   := src
BUILDDIR := build

CC       := gcc
CFLAGS   := -Wall -Wextra -Wpedantic -std=c11 -I$(INCDIR)
DEBUG    ?= no

ifeq ($(DEBUG),yes)
	CFLAGS += -O0 -g
	TYPE   := debug
else
	CFLAGS += -O2
	TYPE   := release
endif

ifeq ($(OS),Windows_NT)
	EXT := .exe
	RM  := cmd.exe /C del
else
	EXT :=
	RM  := rm -rf
endif

TARGET   := $(BUILDDIR)/$(TYPE)/reversi$(EXT)

SRCS := $(wildcard $(SRCDIR)/*.c)
OBJS := $(addprefix $(BUILDDIR)/$(TYPE)/,$(SRCS:.c=.o))
DEPS := $(OBJS:.o=.d)

.PHONY: all clean

all: $(TARGET)

-include $(DEPS)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILDDIR)/$(TYPE)/%.o: %.c
	@mkdir -p $(BUILDDIR)/$(TYPE)/$(SRCDIR)
	$(CC) $(CFLAGS) -c -MMD -MP $^ -o $@

clean:
	$(RM) $(BUILDDIR)
