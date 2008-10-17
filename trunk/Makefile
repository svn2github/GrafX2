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
  BIN = grafx2.exe
  CFGBIN = gfxcfg.exe
  COPT = -Wall -O -g -ggdb `sdl-config --cflags`
  LOPT = `sdl-config --libs`
  CC = gcc
else
  # Linux specific
  DELCOMMAND = rm -rf
  ifdef WIN32CROSS
    #cross compile a Win32 executable
    CC = i586-mingw32msvc-gcc
    BIN = grafx2.exe
    CFGBIN = gfxcfg.exe
    COPT = -Wall -O -g -ggdb -Dmain=SDL_main `/usr/local/cross-tools/i386-mingw32/bin/sdl-config --cflags`
    LOPT = -mwindows -lmingw32 -lSDLmain -lSDL -lshlwapi `/usr/local/cross-tools/i386-mingw32/bin/sdl-config --libs`
  else
    BIN = grafx2
    CFGBIN = gfxcfg
    COPT = -Wall -c -g `sdl-config --cflags`
    LOPT = `sdl-config --libs`
    CC = gcc
  endif
endif

.PHONY : all debug release clean depend zip version force

OBJ = main.o init.o graph.o sdlscreen.o divers.o special.o boutons.o palette.o \
      aide.o operatio.o pages.o loadsave.o readline.o moteur.o files.o op_c.o \
      linux.o op_asm.o readini.o saveini.o shade.o clavier.o io.o version.o
CFGOBJ = gfxcfg.o SFont.o clavier.o io.o

OBJDIR = obj/

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

%.o :
	$(CC) $(COPT) -c $*.c -o $*.o

depend :
	$(CC) -MM $(OBJ:.o=.c) $(CFGOBJ:.o=.c) > Makefile.dep

clean :
	$(DELCOMMAND) *.o
	$(DELCOMMAND) $(BIN)
	$(DELCOMMAND) $(CFGBIN)

test :

include Makefile.dep
