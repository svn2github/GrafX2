/*  Grafx2 - The Ultimate 256-color bitmap paint program

    Copyright 2008 Yves Rizoud
    Copyright 2008 Franck Charlet
    Copyright 2007 Adrien Destugues
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
#include <SDL.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include "struct.h"
#include "sdlscreen.h"
#include "global.h"
#include "erreurs.h"
#include "boutons.h"
#include "moteur.h"
#include "divers.h"
#include "clavier.h"
#include "sdlscreen.h"
#include "windows.h"
#include "palette.h"
#include "input.h"

word Palette_Compter_nb_couleurs_utilisees(dword* usage)
{
  int Nombre_De_Pixels=0;
  Uint8* Pixel_Courant=Principal_Ecran;
  Uint8 Couleur;
  word Nombre_Couleurs=0;
  int i;

  for (i=0;i<256;i++) usage[i]=0;

  //Calcul du nombre de pixels dans l'image
  Nombre_De_Pixels=Principal_Hauteur_image*Principal_Largeur_image;

  // On parcourt l'�cran courant pour compter les utilisations des couleurs
  for(i=0;i<Nombre_De_Pixels;i++)
  {
    Couleur=*Pixel_Courant; //on lit la couleur dans l'�cran

    usage[Couleur]++; //Un point de plus pour cette couleur

    // On passe au pixel suivant
    Pixel_Courant++;
  }

  //On va maintenant compter dans la table les couleurs utilis�es:
  for(i=0;i<256;i++)
  {
    if (usage[i]!=0)
        Nombre_Couleurs++;
  }

  return Nombre_Couleurs;
}

void Set_palette(T_Palette palette)
{
  register int i;
  SDL_Color PaletteSDL[256];
  for(i=0;i<256;i++)
  {
    PaletteSDL[i].r=(palette[i].R=Palette_Scale_Component(palette[i].R));
    PaletteSDL[i].g=(palette[i].G=Palette_Scale_Component(palette[i].G));
    PaletteSDL[i].b=(palette[i].B=Palette_Scale_Component(palette[i].B));
  }
  SDL_SetPalette(Ecran_SDL, SDL_PHYSPAL | SDL_LOGPAL, PaletteSDL,0,256);
}

void Set_color(byte Couleur, byte Rouge, byte Vert, byte Bleu)
{
  SDL_Color comp;
  comp.r=Rouge;
  comp.g=Vert;
  comp.b=Bleu;
  SDL_SetPalette(Ecran_SDL, SDL_LOGPAL, &comp, Couleur, 1);
}

void Attendre_fin_de_click(void)
{
    // On d�sactive tous les raccourcis clavier

    while(Mouse_K) if(!Get_input()) Wait_VBL();
}

void Effacer_image_courante_Stencil(byte Couleur, byte * Pochoir)
//Effacer l'image courante avec une certaine couleur en mode Stencil
{
  int Nombre_De_Pixels=0; //ECX
  //al=Couleur
  //edi=Ecran
  byte* Pixel_Courant=Ecran; //dl
  int i;

  Nombre_De_Pixels=Principal_Hauteur_image*Principal_Largeur_image;

  for(i=0;i<Nombre_De_Pixels;i++)
  {
    if (Pochoir[*Pixel_Courant]==0)
      *Pixel_Courant=Couleur;
    Pixel_Courant++;
  }
}

void Effacer_image_courante(byte Couleur)
// Effacer l'image courante avec une certaine couleur
{
  memset(
    Principal_Ecran ,
    Couleur ,
    Principal_Largeur_image * Principal_Hauteur_image
  );
}

void Sensibilite_souris(__attribute__((unused)) word x,__attribute__((unused)) word y)
{

}

void Initialiser_chrono(dword Delai)
// D�marrer le chrono
{
  Chrono_delay = Delai;
  Chrono_cmp = SDL_GetTicks()/55;
  return;
}

void Wait_VBL(void)
// Attente de VBL. Pour avoir des scrollbars qui ont une vitesse raisonnable par exemple.
// SDL ne sait pas faire, alors on simule un timer qui a une fr�quence de 100Hz,
// sans charger inutilement le CPU par du busy-wait (on n'est pas � 10ms pr�s)
{
  const int Delai = 10;

  Uint32 debut;
  debut = SDL_GetTicks();
  // Premi�re attente : le compl�ment de "Delai" millisecondes
  SDL_Delay(Delai - (debut % Delai));
  // Si �a ne suffit pas, on compl�te par des attentes successives de "1ms".
  // (Remarque, Windows arrondit g�n�ralement aux 10ms sup�rieures)
  while (SDL_GetTicks() / Delai <= debut / Delai)
  {
    SDL_Delay(1);
  }
}

void Pixel_dans_brosse             (word x,word y,byte Couleur)
{
  *(Brosse+y*Brosse_Largeur+x)=Couleur;
}

byte Lit_pixel_dans_brosse         (word x,word y)
{
  return *(Brosse + y * Brosse_Largeur + x);
}


byte Lit_pixel_dans_ecran_courant  (word x,word y)
{
        return *(Principal_Ecran+y*Principal_Largeur_image+x);
}

void Pixel_dans_ecran_courant      (word x,word y,byte Couleur)
{
    byte* dest=(x+y*Principal_Largeur_image+Principal_Ecran);
    *dest=Couleur;
}

void Remplacer_une_couleur(byte Ancienne_couleur, byte Nouvelle_couleur)
{
  byte* edi;

  // pour chaque pixel :
  for(edi = Principal_Ecran;edi < Principal_Ecran + Principal_Hauteur_image * Principal_Largeur_image;edi++)
    if (*edi == Ancienne_couleur)
      *edi = Nouvelle_couleur;
  UpdateRect(0,0,0,0); // On peut TOUT a jour
  // C'est pas un probl�me car il n'y a pas de preview
}

void Ellipse_Calculer_limites(short Rayon_horizontal,short Rayon_vertical)
{
  Ellipse_Rayon_horizontal_au_carre =
    Rayon_horizontal * Rayon_horizontal;
  Ellipse_Rayon_vertical_au_carre =
    Rayon_vertical * Rayon_vertical;
  Ellipse_Limite = Ellipse_Rayon_horizontal_au_carre * Ellipse_Rayon_vertical_au_carre;
}

byte Pixel_dans_ellipse(void)
{
  qword ediesi = Ellipse_Curseur_X * Ellipse_Curseur_X * Ellipse_Rayon_vertical_au_carre +
        Ellipse_Curseur_Y * Ellipse_Curseur_Y * Ellipse_Rayon_horizontal_au_carre;
  if((ediesi) <= Ellipse_Limite) return 255;

        return 0;
}

byte Pixel_dans_cercle(void)
{
        if(Cercle_Curseur_X * Cercle_Curseur_X +
            Cercle_Curseur_Y * Cercle_Curseur_Y <= Cercle_Limite)
                return 255;
        return 0;
}

void Copier_une_partie_d_image_dans_une_autre(byte * Source,word S_Pos_X,word S_Pos_Y,word width,word height,word Largeur_source,byte * dest,word D_Pos_X,word D_Pos_Y,word Largeur_destination)
{
        // ESI = adresse de la source en (S_Pox_X,S_Pos_Y)
        byte* esi = Source + S_Pos_Y * Largeur_source + S_Pos_X;

        // EDI = adresse de la destination (D_Pos_X,D_Pos_Y)
        byte* edi = dest + D_Pos_Y * Largeur_destination + D_Pos_X;

        int line;

        // Pour chaque ligne
        for (line=0;line < height; line++)
        {
                memcpy(edi,esi,width);

                // Passe � la ligne suivante
                esi+=Largeur_source;
                edi+=Largeur_destination;
        }


}

byte Lit_pixel_dans_ecran_brouillon(word x,word y)
{
  return *(Brouillon_Ecran+y*Brouillon_Largeur_image+x);
}

void Rotate_90_deg_LOWLEVEL(byte * Source,byte * dest)
{
  byte* esi;
  byte* edi;
  word dx,bx,cx;

  //ESI = Point haut droit de la source
  byte* Debut_de_colonne = Source + Brosse_Largeur - 1;
  edi = dest;

  // Largeur de la source = hauteur de la destination
  dx = bx = Brosse_Largeur;

  // Pour chaque ligne
  for(dx = Brosse_Largeur;dx>0;dx--)
  {
    esi = Debut_de_colonne;
    // Pout chaque colonne
    for(cx=Brosse_Hauteur;cx>0;cx--)
    {
      *edi = *esi;
      esi+=Brosse_Largeur;
      edi++;
    }

    Debut_de_colonne--;
  }
}

// Remplacer une couleur par une autre dans un buffer

void Remap_general_LOWLEVEL(byte * Table_conv,byte * Buffer,short width,short height,short Largeur_buffer)
{
  int dx,cx;

  // Pour chaque ligne
  for(dx=height;dx>0;dx--)
  {
    // Pour chaque pixel
    for(cx=width;cx>0;cx--)
    {
    *Buffer = Table_conv[*Buffer];
    Buffer++;
    }
    Buffer += Largeur_buffer-width;
  }
}

void Copier_image_dans_brosse(short Debut_X,short Debut_Y,short Brosse_Largeur,short Brosse_Hauteur,word image_width)
{
    byte* Src=Debut_Y*image_width+Debut_X+Principal_Ecran; //Adr d�part image (ESI)
    byte* Dest=Brosse; //Adr dest brosse (EDI)
    int dx;

  for (dx=Brosse_Hauteur;dx!=0;dx--)
  //Pour chaque ligne
  {

    // On fait une copie de la ligne
    memcpy(Dest,Src,Brosse_Largeur);

    // On passe � la ligne suivante
    Src+=image_width;
    Dest+=Brosse_Largeur;
   }

}

byte Lit_pixel_dans_ecran_feedback (word x,word y)
{
  return *(FX_Feedback_Ecran+y*Principal_Largeur_image+x);
}

dword Round_div(dword Numerateur,dword Diviseur)
{
        return Numerateur/Diviseur;
}

byte Effet_Trame(word x,word y)
{
  return Trame[x % Trame_Largeur][y % Trame_Hauteur];
}

void Set_mouse_position(void)
{
    SDL_WarpMouse(
        Mouse_X*Pixel_width,
        Mouse_Y*Pixel_height
    );
}

void Remplacer_toutes_les_couleurs_dans_limites(byte * Table_de_remplacement)
{
  int line;
  int counter;
  byte* Adresse;

  byte Ancien;

  // Pour chaque ligne :
  for(line = Limite_Haut;line <= Limite_Bas; line++)
  {
    // Pour chaque pixel sur la ligne :
    for (counter = Limite_Gauche;counter <= Limite_Droite;counter ++)
    {
      Adresse = Principal_Ecran+line*Principal_Largeur_image+counter;
      Ancien=*Adresse;
      *Adresse = Table_de_remplacement[Ancien];
    }
  }
}

byte Lit_pixel_dans_ecran_backup (word x,word y)
{
  return *(Ecran_backup + x + Principal_Largeur_image * y);
}

void Palette_256_to_64(T_Palette palette)
{
  int i;
  for(i=0;i<256;i++)
  {
    palette[i].R = palette[i].R >> 2;
    palette[i].G = palette[i].G >> 2;
    palette[i].B = palette[i].B >> 2;
  }
}

void Palette_64_to_256(T_Palette palette)
{
  int i;
  for(i=0;i<256;i++)
  {
    palette[i].R = (palette[i].R << 2)|(palette[i].R >> 4);
    palette[i].G = (palette[i].G << 2)|(palette[i].G >> 4);
    palette[i].B = (palette[i].B << 2)|(palette[i].B >> 4);
  }
}

byte Effet_Colorize_interpole  (word x,word y,byte Couleur)
{
  // Facteur_A = 256*(100-Colorize_Opacite)/100
  // Facteur_B = 256*(    Colorize_Opacite)/100
  //
  // (Couleur_dessous*Facteur_A+Couleur*facteur_B)/256
  //

  // On place dans ESI 3*Couleur_dessous ( = position de cette couleur dans la
  // palette des teintes) et dans EDI, 3*Couleur.
  byte Bleu_dessous=Principal_Palette[*(FX_Feedback_Ecran + y * Principal_Largeur_image + x)].B;
  byte Bleu=Principal_Palette[Couleur].B;
  byte Vert_dessous=Principal_Palette[*(FX_Feedback_Ecran + y * Principal_Largeur_image + x)].G;
  byte Vert=Principal_Palette[Couleur].G;
  byte Rouge_dessous=Principal_Palette[*(FX_Feedback_Ecran + y * Principal_Largeur_image + x)].R;
  byte Rouge=Principal_Palette[Couleur].R;

  // On r�cup�re les 3 composantes RVB

  // Bleu
  Bleu = (Table_de_multiplication_par_Facteur_B[Bleu]
    + Table_de_multiplication_par_Facteur_A[Bleu_dessous]) / 256;
  Vert = (Table_de_multiplication_par_Facteur_B[Vert]
    + Table_de_multiplication_par_Facteur_A[Vert_dessous]) / 256;
  Rouge = (Table_de_multiplication_par_Facteur_B[Rouge]
    + Table_de_multiplication_par_Facteur_A[Rouge_dessous]) / 256;
  return Meilleure_couleur(Rouge,Vert,Bleu);

}

byte Effet_Colorize_additif    (word x,word y,byte Couleur)
{
  byte Bleu_dessous=Principal_Palette[*(FX_Feedback_Ecran + y * Principal_Largeur_image + x)].B;
  byte Vert_dessous=Principal_Palette[*(FX_Feedback_Ecran + y * Principal_Largeur_image + x)].G;
  byte Rouge_dessous=Principal_Palette[*(FX_Feedback_Ecran + y * Principal_Largeur_image + x)].R;
  byte Bleu=Principal_Palette[Couleur].B;
  byte Vert=Principal_Palette[Couleur].G;
  byte Rouge=Principal_Palette[Couleur].R;

  return Meilleure_couleur(
    Rouge>Rouge_dessous?Rouge:Rouge_dessous,
    Vert>Vert_dessous?Vert:Vert_dessous,
    Bleu>Bleu_dessous?Bleu:Bleu_dessous);
}

byte Effet_Colorize_soustractif(word x,word y,byte Couleur)
{
  byte Bleu_dessous=Principal_Palette[*(FX_Feedback_Ecran + y * Principal_Largeur_image + x)].B;
  byte Vert_dessous=Principal_Palette[*(FX_Feedback_Ecran + y * Principal_Largeur_image + x)].G;
  byte Rouge_dessous=Principal_Palette[*(FX_Feedback_Ecran + y * Principal_Largeur_image + x)].R;
  byte Bleu=Principal_Palette[Couleur].B;
  byte Vert=Principal_Palette[Couleur].G;
  byte Rouge=Principal_Palette[Couleur].R;

  return Meilleure_couleur(
    Rouge<Rouge_dessous?Rouge:Rouge_dessous,
    Vert<Vert_dessous?Vert:Vert_dessous,
    Bleu<Bleu_dessous?Bleu:Bleu_dessous);
}

void Tester_chrono(void)
{
  if((SDL_GetTicks()/55)-Chrono_delay>Chrono_cmp) Etat_chrono=1;
}

// Effectue une inversion de la brosse selon une droite horizontale
void Flip_Y_LOWLEVEL(void)
{
  // ESI pointe sur la partie haute de la brosse
  // EDI sur la partie basse
  byte* ESI = Brosse ;
  byte* EDI = Brosse + (Brosse_Hauteur - 1) *Brosse_Largeur;
  byte tmp;
  word cx;

  while(ESI < EDI)
  {
    // Il faut inverser les lignes point�es par ESI et
    // EDI ("Brosse_Largeur" octets en tout)

    for(cx = Brosse_Largeur;cx>0;cx--)
    {
      tmp = *ESI;
      *ESI = *EDI;
      *EDI = tmp;
      ESI++;
      EDI++;
    }

    // On change de ligne :
    // ESI pointe d�j� sur le d�but de la ligne suivante
    // EDI pointe sur la fin de la ligne en cours, il
    // doit pointer sur le d�but de la pr�c�dente...
    EDI -= 2 * Brosse_Largeur; // On recule de 2 lignes
  }
}

// Effectue une inversion de la brosse selon une droite verticale
void Flip_X_LOWLEVEL(void)
{
  // ESI pointe sur la partie gauche et EDI sur la partie
  // droite
  byte* ESI = Brosse;
  byte* EDI = Brosse + Brosse_Largeur - 1;

  byte* Debut_Ligne;
  byte* Fin_Ligne;
  byte tmp;
  word cx;

  while(ESI<EDI)
  {
    Debut_Ligne = ESI;
    Fin_Ligne = EDI;

    // On �change par colonnes
    for(cx=Brosse_Hauteur;cx>0;cx--)
    {
      tmp=*ESI;
      *ESI=*EDI;
      *EDI=tmp;
      EDI+=Brosse_Largeur;
      ESI+=Brosse_Largeur;
    }

    // On change de colonne
    // ESI > colonne suivante
    // EDI > colonne pr�c�dente
    ESI = Debut_Ligne + 1;
    EDI = Fin_Ligne - 1;
  }
}

// Faire une rotation de 180� de la brosse
void Rotate_180_deg_LOWLEVEL(void)
{
  // ESI pointe sur la partie sup�rieure de la brosse
  // EDI pointe sur la partie basse
  byte* ESI = Brosse;
  byte* EDI = Brosse + Brosse_Hauteur*Brosse_Largeur - 1;
  // EDI pointe sur le dernier pixel de la derniere ligne
  byte tmp;
  word cx;

  while(ESI < EDI)
  {
    // On �change les deux lignes point�es par EDI et
    // ESI (Brosse_Largeur octets)
    // En m�me temps, on �change les pixels, donc EDI
    // pointe sur la FIN de sa ligne

    for(cx=Brosse_Largeur;cx>0;cx--)
    {
      tmp = *ESI;
      *ESI = *EDI;
      *EDI = tmp;

      EDI--; // Attention ici on recule !
      ESI++;
    }
  }
}

void Tempo_jauge(byte Vitesse)
//Boucle d'attente pour faire bouger les scrollbars � une vitesse correcte
{
  Uint32 end;
  byte MouseK_Original = Mouse_K;
  end = SDL_GetTicks() + Vitesse*10;
  do
  {
    if (!Get_input()) Wait_VBL();
  } while (Mouse_K == MouseK_Original && SDL_GetTicks()<end);
}

void Scroll_picture(short x_offset,short y_offset)
{
  byte* esi = Ecran_backup; //Source de la copie
  byte* edi = Principal_Ecran + y_offset * Principal_Largeur_image + x_offset;
  const word ax = Principal_Largeur_image - x_offset; // Nombre de pixels � copier � droite
  word dx;
  for(dx = Principal_Hauteur_image - y_offset;dx>0;dx--)
  {
  // Pour chaque ligne
    memcpy(edi,esi,ax);
    memcpy(edi - x_offset,esi+ax,x_offset);

    // On passe � la ligne suivante
    edi += Principal_Largeur_image;
    esi += Principal_Largeur_image;
  }

  // On vient de faire le traitement pour otutes les lignes au-dessous de y_offset
  // Maintenant on traite celles au dessus
  edi = x_offset + Principal_Ecran;
  for(dx = y_offset;dx>0;dx--)
  {
    memcpy(edi,esi,ax);
    memcpy(edi - x_offset,esi+ax,x_offset);

    edi += Principal_Largeur_image;
    esi += Principal_Largeur_image;
  }

  UpdateRect(0,0,0,0);
}

void Zoomer_une_ligne(byte* Ligne_originale, byte* Ligne_zoomee,
        word factor, word width
)
{
        byte color;
        word x;

        // Pour chaque pixel
        for(x=0;x<width;x++){
                color = *Ligne_originale;

                memset(Ligne_zoomee,color,factor);
                Ligne_zoomee+=factor;

                Ligne_originale++;
        }
}

/*############################################################################*/

#if defined(__WIN32__)
    #define _WIN32_WINNT 0x0500
    #include <windows.h>
#elif defined(__macosx__) || defined(__FreeBSD__)
    #include <sys/sysctl.h>
#elif defined(__BEOS__) || defined(__HAIKU__)
    // sysinfo not implemented
#elif defined(__AROS__) || defined(__amigaos4__) || defined(__MORPHOS__)
    #include <proto/exec.h>
#elif defined(__SKYOS__)
    #include <skyos/sysinfo.h>
#else
    #include <sys/sysinfo.h> // sysinfo() for free RAM
#endif

// Indique quelle est la m�moire disponible
unsigned long Memoire_libre(void)
{
    // Memory is no longer relevant. If there is ANY problem or doubt here,
    // you can simply return 10*1024*1024 (10Mb), to make the "Pages"something
    // memory allocation functions happy.

    // However, it is still a good idea to make a proper function if you can...
    // If Grafx2 thinks the memory is full, weird things may happen. And if memory
    // ever becomes full and you're still saying there are 10MB free here, the
    // program will crash without saving any picture backup ! You've been warned...
    #if defined(__WIN32__)
        MEMORYSTATUS mstt;
        mstt.dwLength = sizeof(MEMORYSTATUS);
        GlobalMemoryStatus(&mstt);
        return mstt.dwAvailPhys;
    #elif defined(__macosx__) || defined(__FreeBSD__)
        int mib[2];
        int maxmem;
        size_t len;

        mib[0] = CTL_HW;
        mib[1] = HW_USERMEM;
        len = sizeof(maxmem);
        sysctl(mib,2,&maxmem,&len,NULL,0);
        return maxmem;
    #elif defined(__BEOS__) || defined(__HAIKU__) || defined(__SKYOS__) || defined(__amigaos4__)
        // No <sys/sysctl.h> on BeOS or Haiku
        // AvailMem is misleading on os4 (os4 caches stuff in memory that you can still allocate)
        #warning "There is missing code there for your platform ! please check and correct :)"
        return 10*1024*1024;
    #elif defined(__AROS__) || defined(__MORPHOS__)
        return AvailMem(MEMF_ANY);
    #else
        struct sysinfo info;
        sysinfo(&info);
        return info.freeram*info.mem_unit;
    #endif
}



// Transformer un nombre (entier naturel) en cha�ne
void Num2str(dword number,char * Chaine,byte nb_char)
{
  int Indice;

  for (Indice=nb_char-1;Indice>=0;Indice--)
  {
    Chaine[Indice]=(number%10)+'0';
    number/=10;
    if (number==0)
      for (Indice--;Indice>=0;Indice--)
        Chaine[Indice]=' ';
  }
  Chaine[nb_char]='\0';
}

// Transformer une cha�ne en un entier naturel (renvoie -1 si ch. invalide)
int Str2num(char * Chaine)
{
  int value=0;

  for (;*Chaine;Chaine++)
  {
    if ( (*Chaine>='0') && (*Chaine<='9') )
      value=(value*10)+(*Chaine-'0');
    else
      return -1;
  }
  return value;
}


// Arrondir un nombre r�el � la valeur enti�re la plus proche
short Round(float value)
{
  short Temp=value;

  if (value>=0)
    { if ((value-Temp)>= 0.5) Temp++; }
  else
    { if ((value-Temp)<=-0.5) Temp--; }

  return Temp;
}

// Arrondir le r�sultat d'une division � la valeur enti�re sup�rieure
short Round_div_max(short Numerateur,short Diviseur)
{
  if (!(Numerateur % Diviseur))
    return (Numerateur/Diviseur);
  else
    return (Numerateur/Diviseur)+1;
}

// Retourne le minimum entre deux nombres
int Min(int a,int b)
{
  return (a<b)?a:b;
}

// Retourne le maximum entre deux nombres
int Max(int a,int b)
{
  return (a>b)?a:b;
}



// Fonction retournant le libell� d'une mode (ex: " 320x200")
char * Libelle_mode(int mode)
{
  static char Chaine[24];
  if (! Mode_video[mode].Fullscreen)
    return "window";
  sprintf(Chaine, "%dx%d", Mode_video[mode].Width, Mode_video[mode].Height);

  return Chaine;
}

// Trouve un mode video � partir d'une chaine: soit "window",
// soit de la forme "320x200"
// Renvoie -1 si la chaine n'est pas convertible
int Conversion_argument_mode(const char *Argument)
{
  // Je suis paresseux alors je vais juste tester les libell�s
  int Indice_mode;
  for (Indice_mode=0; Indice_mode<Nb_modes_video; Indice_mode++)
    // Attention les vieilles fonctions de lecture .ini mettent tout en MAJUSCULE.
    if (!strcasecmp(Libelle_mode(Indice_mode), Argument))
      return Indice_mode;

  return -1;
}
