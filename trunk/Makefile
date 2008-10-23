#  Grafx2 - The Ultimate 256-color bitmap paint program
#  
#  Copyright 2008 Yves Rizoud
#  Copyright 2007 Adrien Destugues
#  Copyright 1996-2001 Sunset Design (Guillaume Dorme & Karl Maritaud)
#  
#  Grafx2 is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; version 2
#  of the License.
#  
#  Grafx2 is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with Grafx2; if not, see <http://www.gnu.org/licenses/> or
#  write to the Free Software Foundation, Inc.,
#  59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

# Windows specific
ifdef COMSPEC
  DELCOMMAND = del
  MKDIR = mkdir /s
  BIN = grafx2.exe
  CFGBIN = gfxcfg.exe
  COPT = -W -Wall -O -g -ggdb `sdl-config --cflags`
  LOPT = `sdl-config --libs`
  CC = gcc
  OBJDIR = obj/win32
else

  PLATFORM = $(shell uname)

  #OS4 specific
  ifeq ($(PLATFORM),AmigaOS)
    DELCOMMAND = rm -rf
    MKDIR = mkdir -p
    BIN = grafx2
    CFGBIN = gfxcfg
    COPT = -Wall -c -gstabs -mcrt=newlib `sdl-config --cflags`
    LOPT = `sdl-config --libs` -lpng -ljpeg -lz
    CC = gcc
    OBJDIR = obj/amiga
  else

    # Linux specific
    DELCOMMAND = rm -rf
    MKDIR = mkdir -p
    ifdef WIN32CROSS
      #cross compile a Win32 executable
      CC = i586-mingw32msvc-gcc
      BIN = grafx2.exe
      CFGBIN = gfxcfg.exe
      COPT = -W -Wall -O -g -ggdb -Dmain=SDL_main `/usr/local/cross-tools/i386-mingw32/bin/sdl-config --cflags`
      LOPT = -mwindows -lmingw32 -lSDLmain -lSDL -lshlwapi `/usr/local/cross-tools/i386-mingw32/bin/sdl-config --libs`
      OBJDIR = obj/win32
    else
      BIN = grafx2
      CFGBIN = gfxcfg
      COPT = -W -Wall -c -g `sdl-config --cflags`
      LOPT = `sdl-config --libs`
      CC = gcc
      OBJDIR = obj/unix
    endif
  endif
endif

.PHONY : all debug release clean depend zip version force

OBJ = $(OBJDIR)/main.o $(OBJDIR)/init.o $(OBJDIR)/graph.o \
 $(OBJDIR)/sdlscreen.o  $(OBJDIR)/divers.o $(OBJDIR)/special.o \
 $(OBJDIR)/boutons.o $(OBJDIR)/palette.o $(OBJDIR)/aide.o $(OBJDIR)/operatio.o \
 $(OBJDIR)/pages.o $(OBJDIR)/loadsave.o $(OBJDIR)/readline.o $(OBJDIR)/moteur.o\
 $(OBJDIR)/files.o $(OBJDIR)/op_c.o $(OBJDIR)/linux.o $(OBJDIR)/readini.o \
 $(OBJDIR)/saveini.o $(OBJDIR)/shade.o $(OBJDIR)/clavier.o $(OBJDIR)/io.o \
 $(OBJDIR)/version.o
CFGOBJ = $(OBJDIR)/gfxcfg.o $(OBJDIR)/SFont.o $(OBJDIR)/clavier.o $(OBJDIR)/io.o

all : $(BIN) $(CFGBIN)

debug : $(BIN)

release : $(BIN) $(CFGBIN)
	strip $(BIN)
	strip $(CFGBIN)

# A release zip archive
ziprelease: version $(BIN) $(BINCFG) release
	tar cvzf src-svn`svnversion`.tgz *.c *.h Makefile Makefile.dep
	zip grafx2-svn`svnversion`-win32.zip $(BIN) $(CFGBIN) gfx2.dat gfx2.ico doc/gpl-2.0.txt SDL.dll 8pxfont.png SDL_image.dll src-svn`svnversion`.tgz
	$(DELCOMMAND) src-svn`svnversion`.tgz
	tar cvzf grafx2-svn`svnversion`-src.tgz *.c *.h Makefile Makefile.dep gfx2.dat gfx2.ico doc/gpl-2.0.txt 8pxfont.png

$(BIN) : $(OBJ)
	$(CC) $(OBJ) -o $(BIN) $(LOPT)

$(CFGBIN) : $(CFGOBJ)
	$(CC) $(CFGOBJ) -o $(CFGBIN) $(LOPT) -lSDL_image

# SVN revision number
version.c :
	echo "char SVNRevision[]=\"`svnversion`\";" > version.c

version.o : version.c
	$(CC) $(COPT) -c $*.c -o $*.o
	
version : delversion version.c version.o

delversion :
	$(DELCOMMAND) version.c

$(OBJDIR)/%.o : $(OBJDIR)
	$(CC) $(COPT) -c $*.c -o $(OBJDIR)/$*.o

$(OBJDIR) :
	$(MKDIR) $(OBJDIR)

depend :
	$(CC) -MM *.c | sed 's:^[^ ]:$(OBJDIR)/&:' > Makefile.dep


clean :
	$(DELCOMMAND) $(OBJ) $(CFGOBJ)
	$(DELCOMMAND) $(BIN) $(CFGBIN)

test :

-include Makefile.dep
