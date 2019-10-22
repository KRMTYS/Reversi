CC       := gcc
CFLAGS    = -Wall -Wextra -Wpedantic -std=c11 -I$(INCDIR)
TARGET    = $(BUILDDIR)/reversi

INCDIR   := include
SRCDIR   := src
BUILDDIR := build

ifeq ($(OS),Windows_NT)
	EXT := .exe
	RM  := cmd.exe /C del
else
	EXT :=
	RM  := rm -rf
endif

SRCS := $(wildcard $(SRCDIR)/*.c)
OBJS := $(addprefix $(BUILDDIR)/,$(SRCS:.c=.o))
DEPS := $(OBJS:.o=.d)

.PHONY: all clean

-include $(DEPS)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILDDIR)/$(SRCDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)/$(SRCDIR)
	$(CC) $(CFLAGS) -c -MMD $? -o $@

clean:
	$(RM) $(BUILDDIR)
