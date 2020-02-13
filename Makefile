# vim: noexpandtab filetype=make

TARGET = ballsup

DEPDIR := .d
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

CC = clang
CFLAGS = -g -std=c11 -pedantic-errors -Wall $(DEPFLAGS) -I. -I../raylib/src

LINKER = $(CC) -o
LFLAGS = -g -Wall -I. -lm -mmacosx-version-min=10.9 -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL -L../raylib/src -lraylib

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
	export MACOSX_DEPLOYMENT_TARGET=10.9
	clang -mmacosx-version-min=10.9 -framework CoreVideo -framework IOKit -framework Cocoa -framework OpenGL ../raylib/src/libraylib.a src/main.c -o osx/BallsUp.app/Contents/MacOS/ballsup
	rm -fR osx/BallsUp.app/Contents/MacOS/res
	cp -R res osx/BallsUp.app/Contents/MacOS/res
	/System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/LaunchServices.framework/Versions/A/Support/lsregister -f osx/BallsUp.app
	otool -l osx/BallsUp.app/Contents/MacOS/ballsup

dmg: osx
	hdiutil create -size 32m -fs HFS+ -volname "BallsUp" ballsup_writeable.dmg
	hdiutil attach ballsup_writeable.dmg
	cp -R osx/BallsUp.app /Volumes/BallsUp
	hdiutil detach /Volumes/BallsUp
	hdiutil convert ballsup_writeable.dmg -format UDZO -o osx/BallsUp.dmg
	rm ballsup_writeable.dmg

include $(wildcard $(DEPDIR)/*.d)
