/*  Grafx2 - The Ultimate 256-color bitmap paint program

    Copyright 2008 Yves Rizoud
    Copyright 2008 Franck Charlet
    Copyright 2008 Adrien Destugues
    Copyright 1996-2001 Sunset Design (Guillaume Dorme & Karl Maritaud)

    Grafx2 is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; version 2
    of the License.

    Grafx2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Grafx2; if not, see <http://www.gnu.org/licenses/> or
    write to the Free Software Foundation, Inc.,
    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

// Pour d�sactiver le support TrueType, d�finir NOTTF
// To disable TrueType support, define NOTTF

#include <string.h>
#include <stdlib.h>
#include <ctype.h> // tolower()

// TrueType
#ifndef NOTTF
#ifdef __macosx__
  #include <SDL_ttf/SDL_ttf.h>
#else
  #include <SDL_ttf.h>
#endif

#ifdef __linux__
#ifdef __macosx__
  #include <Carbon/Carbon.h>
  #import <corefoundation/corefoundation.h>
  #import <sys/param.h>
#else
  #include <X11/Xlib.h>
#endif
#endif
#endif

#include <SDL_image.h>
// SFont
#include "SFont.h"

#include "struct.h"
#include "global.h"
#include "sdlscreen.h"
#include "io.h"
#include "files.h"

typedef struct T_FONTE
{
  char * Nom;
  int    EstTrueType;
  int    EstImage;
  char   Libelle[22];
  
  // Liste chain�e simple
  struct T_FONTE * Suivante;
  struct T_FONTE * Precedente;
} T_FONTE;
// Liste chain�e des polices de texte
T_FONTE * Liste_fontes_debut;
int Fonte_nombre;

// Inspir� par Allegro
#define EXTID(a,b,c) ((((a)&255)<<16) | (((b)&255)<<8) | (((c)&255)))
#define EXTID4(a,b,c,d) ((((a)&255)<<24) | (((b)&255)<<16) | (((c)&255)<<8) | (((d)&255)))

int Compare_fontes(T_FONTE * Fonte1, T_FONTE * Fonte2)
{
  if (Fonte1->EstImage && !Fonte2->EstImage)
    return -1;
  if (Fonte2->EstImage && !Fonte1->EstImage)
    return 1;
  return strcmp(Fonte1->Libelle, Fonte2->Libelle);
}

// Ajout d'une fonte � la liste.
void Ajout_fonte(const char *Nom)
{
  char * Nom_fonte;
  T_FONTE * Fonte;
  int Taille=strlen(Nom)+1;
  int Indice;
  
  // D�termination du type:

#ifdef __macosx__

  if (Taille < 6) return;
  
  char strFontName[512];
  CFStringRef CFSFontName;// = CFSTR(Nom);

  CFSFontName = CFStringCreateWithBytes(NULL, (UInt8 *) Nom, Taille - 1, kCFStringEncodingASCII, false);
  // Fix some funny names
  CFStringGetCString(CFSFontName, strFontName, 512, kCFStringEncodingASCII);

  // Now we have a printable font name, use it
  Nom = strFontName;

#else
  if (Taille<5 ||
      Nom[Taille-5]!='.')
    return;
#endif

  Fonte = (T_FONTE *)malloc(sizeof(T_FONTE));

  switch (EXTID(tolower(Nom[Taille-4]), tolower(Nom[Taille-3]), tolower(Nom[Taille-2])))
  {
    case EXTID('t','t','f'):
    case EXTID('f','o','n'):
    case EXTID('o','t','f'):
    case EXTID('p','f','b'):
      Fonte->EstTrueType = 1;
      Fonte->EstImage = 0;
      break;
    case EXTID('b','m','p'):
    case EXTID('g','i','f'):
    case EXTID('j','p','g'):
    case EXTID('l','b','m'):
    case EXTID('p','c','x'):
    case EXTID('p','n','g'):
    case EXTID('t','g','a'):
    case EXTID('t','i','f'):
    case EXTID('x','c','f'):
    case EXTID('x','p','m'):
    case EXTID('.','x','v'):
      Fonte->EstTrueType = 0;
      Fonte->EstImage = 1;
      break;
    default:
      #ifdef __macosx__
         if(strcasecmp(&Nom[Taille-6], "dfont") == 0)
         {
           Fonte->EstTrueType = 1;
           Fonte->EstImage = 0;
         }
         else
         {
  	       return;
         }
      #else
         return;
      #endif
  }

  Fonte->Nom = (char *)malloc(Taille);
  strcpy(Fonte->Nom, Nom);
  // Libelle
  strcpy(Fonte->Libelle, "                   ");
  if (Fonte->EstTrueType)
    Fonte->Libelle[17]=Fonte->Libelle[18]='T'; // Logo TT
  Nom_fonte=Position_dernier_slash(Fonte->Nom);
  if (Nom_fonte==NULL)
    Nom_fonte=Fonte->Nom;
  else
    Nom_fonte++;
  for (Indice=0; Indice < 17 && Nom_fonte[Indice]!='\0' && Nom_fonte[Indice]!='.'; Indice++)
    Fonte->Libelle[Indice]=Nom_fonte[Indice];

  // Gestion Liste
  Fonte->Suivante = NULL;
  Fonte->Precedente = NULL;
  if (Liste_fontes_debut==NULL)
  {
    // Premiere (liste vide)
    Liste_fontes_debut = Fonte;
    Fonte_nombre++;
  }
  else
  {
    int Compare;
    Compare = Compare_fontes(Fonte, Liste_fontes_debut);
    if (Compare<=0)
    {
      if (Compare==0 && !strcmp(Fonte->Nom, Liste_fontes_debut->Nom))
      {
        // Doublon
        free(Fonte->Nom);
        free(Fonte);
        return;
      }
      // Avant la premiere
      Fonte->Suivante=Liste_fontes_debut;
      Liste_fontes_debut=Fonte;
      Fonte_nombre++;
    }
    else
    {
      T_FONTE *Fonte_cherchee;
      Fonte_cherchee=Liste_fontes_debut;
      while (Fonte_cherchee->Suivante && (Compare=Compare_fontes(Fonte, Fonte_cherchee->Suivante))>0)
        Fonte_cherchee=Fonte_cherchee->Suivante;
      // Apr�s Fonte_cherchee
      if (Compare==0 && strcmp(Fonte->Nom, Fonte_cherchee->Suivante->Nom)==0)
      {
        // Doublon
        free(Fonte->Nom);
        free(Fonte);
        return;
      }
      Fonte->Suivante=Fonte_cherchee->Suivante;
      Fonte_cherchee->Suivante=Fonte;
      Fonte_nombre++;
    }
  }
}


// Trouve le nom d'une fonte par son num�ro
char * Nom_fonte(int Indice)
{
  T_FONTE *Fonte = Liste_fontes_debut;
  if (Indice<0 ||Indice>=Fonte_nombre)
    return "";
  while (Indice--)
    Fonte = Fonte->Suivante;
  return Fonte->Nom;
}


// Trouve le libell� d'affichage d'une fonte par son num�ro
// Renvoie un pointeur sur un buffer statique de 20 caracteres.
char * Libelle_fonte(int Indice)
{
  T_FONTE *Fonte;
  static char Libelle[20];
  
  strcpy(Libelle, "                   ");
  
  // Recherche de la fonte
  Fonte = Liste_fontes_debut;
  if (Indice<0 ||Indice>=Fonte_nombre)
    return Libelle;
  while (Indice--)
    Fonte = Fonte->Suivante;
  
  // Libell�
  strcpy(Libelle, Fonte->Libelle);
  return Libelle;
}


// V�rifie si une fonte donn�e est TrueType
int TrueType_fonte(int Indice)
{
  T_FONTE *Fonte = Liste_fontes_debut;
  if (Indice<0 ||Indice>=Fonte_nombre)
    return 0;
  while (Indice--)
    Fonte = Fonte->Suivante;
  return Fonte->EstTrueType;
}



// Initialisation � faire une fois au d�but du programme
void Initialisation_Texte(void)
{
  char Nom_repertoire[TAILLE_CHEMIN_FICHIER];
  #ifndef NOTTF
  // Initialisation de TTF
  TTF_Init();
  #endif
  
  // Initialisation des fontes
  Liste_fontes_debut = NULL;
  Fonte_nombre=0;
  // Parcours du r�pertoire "fonts"
  strcpy(Nom_repertoire, Repertoire_des_donnees);
  strcat(Nom_repertoire, "fonts");
  for_each_file(Nom_repertoire, Ajout_fonte);
  
  #ifdef __WIN32__
    // Parcours du r�pertoire systeme windows "fonts"
    #ifndef NOTTF
    {
      char * WindowsPath=getenv("windir");
      if (WindowsPath)
      {
        sprintf(Nom_repertoire, "%s\\FONTS", WindowsPath);
        for_each_file(Nom_repertoire, Ajout_fonte);
      }
    }
    #endif
  #elif defined(__macosx__)
    // R�cup�ration de la liste des fonts avec fontconfig
    #ifndef NOTTF


      int i,number;
      char home_dir[MAXPATHLEN];
      char *font_path_list[3] = {
      	 "/System/Library/Fonts",
      	 "/Library/Fonts"
      };
      number = 3;
      // Make sure we also search into the user's fonts directory
      CFURLRef url = (CFURLRef) CFCopyHomeDirectoryURLForUser(NULL);
      CFURLGetFileSystemRepresentation(url, true, (UInt8 *) home_dir, MAXPATHLEN);
      strcat(home_dir, "/Library/Fonts");
      font_path_list[2] = home_dir;

      for(i=0;i<number;i++)
         for_each_file(*(font_path_list+i),Ajout_fonte);

      CFRelease(url);
    #endif

  #elif defined(__linux__)
    #ifndef NOTTF
       #define USE_XLIB
    
       #ifdef USE_XLIB
       {
	int i,number;
	Display* dpy = XOpenDisplay(NULL);
	char** font_path_list = XGetFontPath(dpy,&number);

	for(i=0;i<number;i++)
	    for_each_file(*(font_path_list+i),Ajout_fonte);

	XFreeFontPath(font_path_list);
       }
       #endif
    #endif
  #elif defined(__amigaos4__)
    #ifndef NOTTF
      for_each_file( "FONTS:_TrueType", Ajout_fonte );
    #endif
  #elif defined(__BEOS__) || defined(__HAIKU__)
    #ifndef NOTTF
      for_each_file("/etc/fonts/ttfonts", Ajout_fonte);
    #endif

  #elif defined(__SKYOS__)
    #ifndef NOTTF
      for_each_file("/boot/system/fonts", Ajout_fonte);
    #endif

  #endif
}

// Informe si texte.c a �t� compil� avec l'option de support TrueType ou pas.
int Support_TrueType()
{
  #ifdef NOTTF
  return 0;
  #else
  return 1;
  #endif
}

  
#ifndef NOTTF
byte *Rendu_Texte_TTF(const char *Chaine, int Numero_fonte, int Taille, int AntiAlias, int Bold, int Italic, int *Largeur, int *Hauteur)
{
 TTF_Font *Fonte;
  SDL_Surface * TexteColore;
  SDL_Surface * Texte8Bit;
  byte * BrosseRetour;
  int Indice;
  int Style;
  
  SDL_Color Couleur_Avant;
  SDL_Color Couleur_Arriere;

  // Chargement de la fonte
  Fonte=TTF_OpenFont(Nom_fonte(Numero_fonte), Taille);
  if (!Fonte)
  {
    return NULL;
  }
  // Style
  Style=0;
  if (Italic)
    Style|=TTF_STYLE_ITALIC;
  if (Bold)
    Style|=TTF_STYLE_BOLD;
  TTF_SetFontStyle(Fonte, Style);
  // Couleurs
  if (AntiAlias)
  {
    Couleur_Avant = Conversion_couleur_SDL(Fore_color);
    Couleur_Arriere = Conversion_couleur_SDL(Back_color);
  }
  else
  {
    Couleur_Avant = Conversion_couleur_SDL(CM_Blanc);
    Couleur_Arriere = Conversion_couleur_SDL(CM_Noir);
  }

  
  // Rendu du texte: cr�e une surface SDL RGB 24bits
  if (AntiAlias)
    TexteColore=TTF_RenderText_Shaded(Fonte, Chaine, Couleur_Avant, Couleur_Arriere );
  else
    TexteColore=TTF_RenderText_Solid(Fonte, Chaine, Couleur_Avant);
  if (!TexteColore)
  {
    TTF_CloseFont(Fonte);
    return NULL;
  }
  
  Texte8Bit=SDL_DisplayFormat(TexteColore);

  SDL_FreeSurface(TexteColore);
    
  BrosseRetour=Surface_en_bytefield(Texte8Bit, NULL);
  if (!BrosseRetour)
  {
    SDL_FreeSurface(TexteColore);
    SDL_FreeSurface(Texte8Bit);
    TTF_CloseFont(Fonte);
    return NULL;
  }
  if (!AntiAlias)
  {
    // Mappage des couleurs
    for (Indice=0; Indice < Texte8Bit->w * Texte8Bit->h; Indice++)
    {
      if (*(BrosseRetour+Indice) == CM_Noir)
      *(BrosseRetour+Indice)=Back_color;
      else if (*(BrosseRetour+Indice) == CM_Blanc)
      *(BrosseRetour+Indice)=Fore_color;
    }
  }
  *Largeur=Texte8Bit->w;
  *Hauteur=Texte8Bit->h;
  SDL_FreeSurface(Texte8Bit);
  TTF_CloseFont(Fonte);
  return BrosseRetour;
}
#endif


byte *Rendu_Texte_SFont(const char *Chaine, int Numero_fonte, int *Largeur, int *Hauteur)
{
  SFont_Font *Fonte;
  SDL_Surface * TexteColore;
  SDL_Surface * Texte8Bit;
  SDL_Surface *Surface_fonte;
  byte * BrosseRetour;

  // Chargement de la fonte
  Surface_fonte=IMG_Load(Nom_fonte(Numero_fonte));
  if (!Surface_fonte)
    return NULL;
  Fonte=SFont_InitFont(Surface_fonte);
  if (!Fonte)
  {
    return NULL;
  }
  
  // Calcul des dimensions
  *Hauteur=SFont_TextHeight(Fonte);
  *Largeur=SFont_TextWidth(Fonte, Chaine);
  // Allocation d'une surface SDL
  TexteColore=SDL_CreateRGBSurface(SDL_SWSURFACE, *Largeur, *Hauteur, 24, 0, 0, 0, 0);
  // Rendu du texte
  SFont_Write(TexteColore, Fonte, 0, 0, Chaine);
  if (!TexteColore)
  {
    SFont_FreeFont(Fonte);
    return NULL;
  }
  
  Texte8Bit=SDL_DisplayFormat(TexteColore);
  SDL_FreeSurface(TexteColore);
    
  BrosseRetour=Surface_en_bytefield(Texte8Bit, NULL);
  if (!BrosseRetour)
  {
    SDL_FreeSurface(TexteColore);
    SDL_FreeSurface(Texte8Bit);
    SFont_FreeFont(Fonte);
    return NULL;
  }
  SDL_FreeSurface(Texte8Bit);
  SFont_FreeFont(Fonte);

  return BrosseRetour;
}


// Cr�e une brosse � partir des param�tres de texte demand�s.
// Si cela r�ussit, la fonction place les dimensions dans Largeur et Hauteur, 
// et retourne l'adresse du bloc d'octets.
byte *Rendu_Texte(const char *Chaine, int Numero_fonte, int Taille, int AntiAlias, int Bold, int Italic, int *Largeur, int *Hauteur)
{
  T_FONTE *Fonte = Liste_fontes_debut;
  int Indice=Numero_fonte;
  
  // Verification type de la fonte
  if (Numero_fonte<0 ||Numero_fonte>=Fonte_nombre)
    return NULL;
    
  while (Indice--)
    Fonte = Fonte->Suivante;
  if (Fonte->EstTrueType)
  {
  #ifndef NOTTF 
    return Rendu_Texte_TTF(Chaine, Numero_fonte, Taille, AntiAlias, Bold, Italic, Largeur, Hauteur);
  #else
    return NULL;
  #endif
  }
  else
  {
    return Rendu_Texte_SFont(Chaine, Numero_fonte, Largeur, Hauteur);
  }
}


