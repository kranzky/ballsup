# vim: noexpandtab filetype=make

TARGET = ballsup

DEPDIR := .d
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

CC = cc
CFLAGS = -g -std=c11 -pedantic-errors -Wall $(DEPFLAGS) -I. $(shell sdl2-config --cflags)

LINKER = $(CC) -o
LFLAGS = -g -Wall -I. -lm $(shell sdl2-config --libs) -lSDL2_Image -lSDL2_TTF

SRCDIR = src
OBJDIR = obj
BINDIR = bin

SOURCES := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm = rm -f

POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

$(shell mkdir -p $(DEPDIR) >/dev/null)

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(LINKER) $@ $(LFLAGS) $(OBJECTS)

$(OBJDIR)/%.o : $(SRCDIR)/%.c
$(OBJDIR)/%.o : $(SRCDIR)/%.c $(DEPDIR)/%.d
	$(CC) $(CFLAGS) -c $< -o $@
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

.PHONEY: clean
clean:
	$(rm) $(wildcard $(DEPDIR)/*.d)
	$(rm) $(OBJECTS)
	$(rm) $(BINDIR)/$(TARGET)

include $(wildcard $(DEPDIR)/*.d)
