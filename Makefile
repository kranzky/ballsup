# vim: noexpandtab filetype=make

TARGET = ballsup

DEPDIR := .d
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

CC = cc
CFLAGS = -g -std=c11 -pedantic-errors -Wall $(DEPFLAGS) -I. $(shell pkg-config raylib --cflags)

LINKER = $(CC) -o
LFLAGS = -g -Wall -I. -lm $(shell pkg-config raylib --libs)

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

osx:
	clang -mmacosx-version-min=10.4.0 -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT /usr/local/opt/raylib/lib/libraylib.a -framework OpenGL src/main.c -o osx/BallsUp.app/Contents/MacOS/ballsup
	/System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/LaunchServices.framework/Versions/A/Support/lsregister -f osx/BallsUp.app

include $(wildcard $(DEPDIR)/*.d)
