#  Grafx2 - The Ultimate 256-color bitmap paint program
#  
#  Copyright 2008 Peter Gordon
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

# Overridable defaults
  prefix = /usr/local
  exec_prefix = $(prefix)
  bindir = $(exec_prefix)/bin
  datarootdir = $(prefix)/share
  datadir = $(datarootdir)

  STRIP = strip

# Windows specific
ifdef COMSPEC
  DELCOMMAND = rm -f
  MKDIR = mkdir -p
  RMDIR = rmdir
  CP = cp
  BIN = grafx2.exe
  CFGBIN = gfxcfg.exe
  COPT = -W -Wall -Wdeclaration-after-statement -O -g -ggdb `sdl-config --cflags` $(TTFCOPT) $(JOYCOPT)
  LOPT = `sdl-config --libs` -lSDL_image $(TTFLOPT) -lpng
  CC = gcc
  OBJDIR = obj/win32
  # Resources (icon)
  WINDRES = windres.exe
  OBJRES = $(OBJDIR)/winres.o
  CFGOBJRES = $(OBJDIR)/wincfgres.o
  PLATFORM = win32
  PLATFORMFILES = SDL.dll SDL_image.dll libpng13.dll zlib1.dll gfx2cfg.ico gfx2.ico $(TTFLIBS) #some misc files we have to add to the release archive under windows.
  ZIP = zip
else

  PLATFORM = $(shell uname)

  #AmigaOS4 specific
  ifeq ($(PLATFORM),AmigaOS)
    DELCOMMAND = rm -rf
    MKDIR = mkdir -p
    RMDIR = rmdir
    CP = cp
    BIN = grafx2
    CFGBIN = gfxcfg
    COPT = -Wall -c -gstabs -mcrt=newlib `sdl-config --cflags` -D__USE_INLINE__ $(TTFCOPT)
    LOPT = `sdl-config --libs` -lSDL_image -lpng -ljpeg -lz $(TTFLOPT) -lft2
    CC = gcc
    OBJDIR = obj/amiga
    ZIP = lha
    ZIPOPT = a
  else

  #AROS specific
  ifeq ($(PLATFORM),AROS)
    DELCOMMAND = rm -rf
    MKDIR = mkdir -p
    RMDIR = rmdir
    CP = cp
    BIN = grafx2
    CFGBIN = gfxcfg
    COPT = -Wall -g `sdl-config --cflags` $(TTFCOPT)
    LOPT = -lSDL_image `sdl-config --libs` -lpng -ljpeg -lz $(TTFLOPT) -lfreetype2shared
    CC = gcc
    OBJDIR = obj/aros
    ZIP = lha
    ZIPOPT = a
  else

  #MorphOS specific
  ifeq ($(PLATFORM),MorphOS) 
    DELCOMMAND = rm -rf
    MKDIR = mkdir -p
    RMDIR = rmdir
    CP = cp
    BIN = grafx2
    CFGBIN = gfxcfg
    COPT = -Wall -gstabs -c `sdl-config --cflags` $(TTFCOPT)
    LOPT = -lSDL_image `sdl-config --libs` -lpng -ljpeg -lz $(TTFLOPT)
    CC = gcc
    OBJDIR = obj/morphos
    ZIP = lha
    ZIPOPT = a
  else

  #BeOS specific
  ifeq ($(PLATFORM),BeOS)
    DELCOMMAND = rm -rf
    MKDIR = mkdir -p
    RMDIR = rmdir
    CP = cp
    BIN = grafx2
    CFGBIN = gfxcfg
    COPT = -W -Wall -c -g `sdl-config --cflags` $(TTFCOPT) -I/boot/home/config/include
    LOPT = `sdl-config --libs` -lSDL_image -lpng -ljpeg -lz $(TTFLOPT)
    CC = gcc
    OBJDIR = obj/beos
    ZIP = zip
  else

  #Haiku specific
  ifeq ($(PLATFORM),Haiku)
    DELCOMMAND = rm -rf
    MKDIR = mkdir -p
    RMDIR = rmdir
    CP = cp
    BIN = grafx2
    CFGBIN = gfxcfg
    COPT = -W -Wall -c -g `sdl-config --cflags` $(TTFCOPT) -I/boot/common/include
    LOPT = `sdl-config --libs` -lSDL_image -lpng -ljpeg -lz $(TTFLOPT)
    CC = gcc
    OBJDIR = obj/haiku
    ZIP = zip
  else

  #SkyOS specific
  ifeq ($(PLATFORM),skyos)
    DELCOMMAND = rm -rf
    MKDIR = mkdir -p
    RMDIR = rmdir
    CP = cp
    BIN = grafx2
    CFGBIN = gfxcfg
    COPT = -W -Wall -Wdeclaration-after-statement -c -g `sdl-config --cflags` $(TTFCOPT)
    LOPT = `sdl-config --libs` -lSDL_image -lpng -ljpeg -lz $(TTFLOPT)
    CC = gcc
    OBJDIR = obj/skyos
    ZIP = zip
  else
  
    # Amiga classic
    
    $(shell version >PIPE:gfx2ver)
    AMIGA=$(shell sed -e s/\(Kickstart\).*/\1/g <PIPE:gfx2ver)
    
    ifeq ($(AMIGA),Kickstart)
      DELCOMMAND = Delete -r
      MKDIR = Makedir
      RMDIR = Delete
      CP = copy
      ZIP = zip
      PLATFORMFILS = gfx2.png
      BIN = grafx2
      CFGBIN = gfxcfg
      COPT = -c99
      LOPT =
      CC = Logiciels:vbcc/bin/vc
      OBJDIR = obj/Amiga68k
      # No strip command with vbcc, do some dummy thingsto make Make happy
      STRIP = echo
    else
  
    # Linux and FreeBSD specific (default rules)
    DELCOMMAND = rm -rf
    MKDIR = mkdir -p
    RMDIR = rmdir
    CP = cp
    ZIP = zip
    PLATFORMFILES = gfx2.png

    ifdef WIN32CROSS
      #cross compile a Win32 executable
      CC = i586-mingw32msvc-gcc
      BIN = grafx2.exe
      CFGBIN = gfxcfg.exe
      COPT = -W -Wall -Wdeclaration-after-statement -O -g -ggdb -Dmain=SDL_main `/usr/local/cross-tools/i386-mingw32/bin/sdl-config --cflags` $(TTFCOPT)
      LOPT = -mwindows -lmingw32 -lSDLmain -lSDL -lshlwapi `/usr/local/cross-tools/i386-mingw32/bin/sdl-config --libs` -lSDL_image $(TTFLOPT)
      OBJDIR = obj/win32
      PLATFORM = win32
    else

      ifdef GP2XCROSS
        #cross compile an exec for the gp2x
        CC = /opt/open2x/gcc-4.1.1-glibc-2.3.6/arm-open2x-linux/bin/arm-open2x-linux-gcc
        BIN = grafx2.gpe
        CFGBIN = gfxcfg.gpe
        COPT = -W -Wall -Wdeclaration-after-statement -pedantic -std=c99 -static -c -g -O3 `/opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/sdl-config --cflags` $(TTFCOPT) -D__GP2X__
        LOPT = -static -lSDL_image `/opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/sdl-config --static-libs` -ljpeg -lpng -lz -lm $(TTFLOPT)
        OBJDIR = obj/gp2x
        NOTTF = 1
        PLATFORM = gp2x
        STRIP = /opt/open2x/gcc-4.1.1-glibc-2.3.6/arm-open2x-linux/bin/arm-open2x-linux-strip
      else

        # Compiles a regular linux exectutable for the native platform
        BIN = grafx2
        CFGBIN = gfxcfg
        COPT = -W -Wall -Wdeclaration-after-statement -pedantic -std=c99 -c -g `sdl-config --cflags` $(TTFCOPT)
        LOPT = `sdl-config --libs` -lSDL_image $(TTFLOPT)
        CC = gcc
        OBJDIR = obj/unix
        X11LOPT = -lX11
      endif
    endif
  endif
  endif
endif
endif
endif
endif
endif
endif

#TrueType is optional: make NOTTF=1 to disable support and dependencies.
ifeq ($(NOTTF),1)
  TTFCOPT = -DNOTTF=1
  TTFLOPT =
  TTFLIBS =
  TTFLABEL = -nottf
else
  TTFCOPT = 
  TTFLOPT = -L/usr/local/lib -lSDL_ttf $(X11LOPT)
  TTFLIBS = libfreetype-6.dll SDL_ttf.dll
  TTFLABEL = 
endif

#To disable Joystick emulation of cursor, make NOTTF=1 (for input.o)
#This can be necessary to test keyboard cursor code, because an existing
#joystick will keep reporting a contradicting position.
ifeq ($(NOJOY),1)
  JOYCOPT = -DNO_JOYCURSOR
else
  JOYCOPT =
endif

.PHONY : all debug release clean depend zip version force install uninstall

OBJ = $(OBJDIR)/main.o $(OBJDIR)/init.o $(OBJDIR)/graph.o $(OBJDIR)/sdlscreen.o  $(OBJDIR)/divers.o $(OBJDIR)/special.o $(OBJDIR)/boutons.o $(OBJDIR)/palette.o $(OBJDIR)/aide.o $(OBJDIR)/operatio.o $(OBJDIR)/pages.o $(OBJDIR)/loadsave.o $(OBJDIR)/readline.o $(OBJDIR)/moteur.o $(OBJDIR)/files.o $(OBJDIR)/op_c.o $(OBJDIR)/readini.o $(OBJDIR)/saveini.o $(OBJDIR)/shade.o $(OBJDIR)/clavier.o $(OBJDIR)/io.o $(OBJDIR)/version.o $(OBJDIR)/texte.o $(OBJDIR)/SFont.o $(OBJDIR)/setup.o $(OBJDIR)/pxsimple.o $(OBJDIR)/pxtall.o $(OBJDIR)/pxwide.o $(OBJDIR)/pxdouble.o $(OBJDIR)/windows.o $(OBJDIR)/brush.o $(OBJDIR)/realpath.o $(OBJDIR)/mountlist.o $(OBJDIR)/input.o $(OBJDIR)/hotkeys.o
CFGOBJ = $(OBJDIR)/gfxcfg.o $(OBJDIR)/SFont.o $(OBJDIR)/clavier.o $(OBJDIR)/io.o $(OBJDIR)/setup.o  $(OBJDIR)/hotkeys.o

all : $(BIN) $(CFGBIN)

debug : $(BIN)

release : $(BIN) $(CFGBIN)
	$(STRIP) $(BIN)
	$(STRIP) $(CFGBIN)

# A release zip archive
ziprelease: version $(BIN) $(BINCFG) release
	tar cvzf src-svn`svnversion | sed 's/:/-/'`.tgz *.c *.h Makefile Makefile.dep gfx2.ico gfx2cfg.ico
	$(ZIP) $(ZIPOPT) grafx2-svn`svnversion | sed 's/:/-/'`$(TTFLABEL)-$(PLATFORM).$(ZIP) $(BIN) $(CFGBIN) gfx2def.ini gfx2gui.gif gfx2.gif gfx2cfg.gif doc/gpl-2.0.txt fonts/8pxfont.png doc/README-zlib1.txt doc/README-SDL.txt doc/README-SDL_image.txt doc/README-SDL_ttf.txt fonts/Tuffy.ttf src-svn`svnversion | sed 's/:/-/'`.tgz $(PLATFORMFILES)
	$(DELCOMMAND) src-svn`svnversion | sed 's/:/-/'`.tgz
	tar cvzf grafx2-svn`svnversion | sed 's/:/-/'`$(TTFLABEL)-src.tgz *.c *.h Makefile Makefile.dep gfx2def.ini gfx2gui.gif gfx2.ico gfx2cfg.ico gfx2.gif gfx2cfg.gif doc/gpl-2.0.txt fonts/8pxfont.png fonts/Tuffy.ttf

$(BIN) : $(OBJ) $(OBJRES)
	$(CC) $(OBJ) $(OBJRES) -o $(BIN) $(LOPT)

$(CFGBIN) : $(CFGOBJ) $(CFGOBJRES)
	$(CC) $(CFGOBJ) $(CFGOBJRES) -o $(CFGBIN) $(LOPT)

# SVN revision number
version.c :
	echo "char SVNRevision[]=\"`svnversion`\";" > version.c

version : delversion version.c $(OBJDIR)/version.o

delversion :
	$(DELCOMMAND) version.c

$(OBJDIR)/%.o :
	$(if $(wildcard $(OBJDIR)),,$(MKDIR) $(OBJDIR))
	$(CC) $(COPT) -c $*.c -o $(OBJDIR)/$*.o

depend :
	$(CC) -MM *.c | sed 's:^[^ ]:$$(OBJDIR)/&:' > Makefile.dep

$(OBJDIR)/winres.o : gfx2.ico
	echo "1 ICON \"gfx2.ico\"" | $(WINDRES) -o $(OBJDIR)/winres.o

$(OBJDIR)/wincfgres.o : gfx2cfg.ico
	echo "1 ICON \"gfx2cfg.ico\"" | $(WINDRES) -o $(OBJDIR)/wincfgres.o

clean :
	$(DELCOMMAND) $(OBJ) $(CFGOBJ) $(OBJDIR)/version.o $(OBJRES) $(CFGOBJRES)
	$(DELCOMMAND) $(BIN) $(CFGBIN)

install : $(BIN) $(CFGBIN)
	echo "#!/bin/sh" > $(bindir)/grafx2
	echo $(datadir)/grafx2/$(BIN) '$$*' >> $(bindir)/grafx2
	chmod 755 $(bindir)/grafx2
	echo "#!/bin/sh" > $(bindir)/gfxcfg
	echo $(datadir)/grafx2/$(CFGBIN) '$$*' >> $(bindir)/gfxcfg
	chmod 755 $(bindir)/gfxcfg
	$(if $(wildcard $(datadir)/grafx2),,$(MKDIR) $(datadir)/grafx2)
	$(CP) $(BIN) $(datadir)/grafx2/
	$(CP) $(CFGBIN) $(datadir)/grafx2/
	$(CP) gfx2def.ini $(datadir)/grafx2/
	$(CP) gfx2gui.gif $(datadir)/grafx2/
	$(CP) gfx2.gif $(datadir)/grafx2/
	$(CP) gfx2cfg.gif $(datadir)/grafx2/
	$(if $(wildcard $(datadir)/grafx2/fonts),,$(MKDIR) $(datadir)/grafx2/fonts)
	cd fonts && $(CP) * $(datadir)/grafx2/fonts/
	@echo Install complete
  
uninstall :
	$(DELCOMMAND) $(bindir)/grafx2
	$(DELCOMMAND) $(bindir)/gfxcfg
	$(DELCOMMAND) $(datadir)/grafx2/$(BIN)
	$(DELCOMMAND) $(datadir)/grafx2/$(CFGBIN)
	$(DELCOMMAND) $(datadir)/grafx2/gfx2def.ini
	$(DELCOMMAND) $(datadir)/grafx2/gfx2gui.gif
	$(DELCOMMAND) $(datadir)/grafx2/gfx2.gif
	$(DELCOMMAND) $(datadir)/grafx2/gfx2cfg.gif
	$(DELCOMMAND) $(datadir)/grafx2/fonts/*
	$(if $(wildcard $(datadir)/grafx2/fonts),,$(RMDIR) $(datadir)/grafx2/fonts)
	$(if $(wildcard $(datadir)/grafx2),,$(RMDIR) $(datadir)/grafx2)
	@echo Uninstall complete

-include Makefile.dep

