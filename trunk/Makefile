#  Grafx2 - The Ultimate 256-color bitmap paint program
#  
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
COPT = -Wall -O -g -ggdb -Dmain=SDL_main
LOPT = -mwindows -lmingw32 -lSDLmain -lSDL -lshlwapi
CC = gcc
else
# Linux specific
DELCOMMAND = rm -rf
ifdef WIN32CROSS
#cross compile a Win32 executable
CC = i586-mingw32msvc-gcc
BIN = grafx2.exe
COPT = -Wall -O -g -ggdb -Dmain=SDL_main
LOPT = -mwindows -lmingw32 -lSDLmain -lSDL -lshlwapi
else
BIN = grafx2
COPT = -Wall -c -g
LOPT = -lSDL -o $(BIN)
CC = gcc
endif
endif

.PHONY : all debug release clean depend zip

OBJ = main.o init.o graph.o sdlscreen.o divers.o special.o boutons.o palette.o aide.o operatio.o pages.o loadsave.o readline.o moteur.o files.o op_c.o linux.o op_asm.o readini.o saveini.o shade.o clavier.o io.o

OBJDIR = obj/

all : $(BIN)

debug : $(BIN)

release : $(BIN)
	strip $(BIN)

# A raw source archive
zip :
	tar cvzf gfx2-src.tgz *.c *.h Makefile Makefile.dep gfx2.dat gfx2.ini gfx2.cfg doc/gpl-2.0.txt \
    cfg_new/*.c cfg_new/*.h cfg_new/Makefile cfg_new/8pxfont.png 

# A release zip archive
ziprelease:
	zip grafx2-beta-svn`svnversion`.zip $(BIN) gfx2.dat gfx2.ico doc/gpl-2.0.txt SDL.dll gfx2.cfg

$(BIN) : $(OBJ)
	$(CC) $(OBJ) -o $(BIN) $(LOPT)
	
%.o :
	$(CC) $(COPT) -c $*.c -o $*.o

depend :
	$(CC) -MM $(OBJ:.o=.c) > Makefile.dep

clean :
	$(DELCOMMAND) *.o
	$(DELCOMMAND) $(BIN)

include Makefile.dep
