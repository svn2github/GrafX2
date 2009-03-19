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

#include <string.h>
#include <stdlib.h>
#include <SDL.h>
#include "global.h"
#include "sdlscreen.h"
#include "divers.h"
#include "pxwide.h" // for Afficher_une_ligne_transparente_a_l_ecran_Wide()

#define ZOOMX 2
#define ZOOMY 2

void Pixel_Double (word X,word Y,byte Couleur)
/* Affiche un pixel de la Couleur aux coords X;Y � l'�cran */
{
  *(Ecran + X * 2 + Y * 4 * Largeur_ecran)=Couleur;
  *(Ecran + X * 2 + Y * 4 * Largeur_ecran + 1)=Couleur;
  *(Ecran + X * 2 + (Y * 4 + 2 )* Largeur_ecran)=Couleur;
  *(Ecran + X * 2 + (Y * 4 + 2 )* Largeur_ecran + 1)=Couleur;
}

byte Lit_Pixel_Double (word X,word Y)
/* On retourne la couleur du pixel aux coords donn�es */
{
  return *( Ecran + Y * 4 * Largeur_ecran + X * 2);
}

void Block_Double (word Debut_X,word Debut_Y,word width,word height,byte Couleur)
/* On affiche un rectangle de la couleur donn�e */
{
  SDL_Rect rectangle;
  rectangle.x=Debut_X*2;
  rectangle.y=Debut_Y*2;
  rectangle.w=width*2;
  rectangle.h=height*2;
  SDL_FillRect(Ecran_SDL,&rectangle,Couleur);
}

void Afficher_partie_de_l_ecran_Double (word width,word height,word image_width)
/* Afficher une partie de l'image telle quelle sur l'�cran */
{
  byte* Dest=Ecran; //On va se mettre en 0,0 dans l'�cran (Dest)
  byte* Src=Principal_Decalage_Y*image_width+Principal_Decalage_X+Principal_Ecran; //Coords de d�part ds la source (Src)
  int y;
  int dy;

  for(y=height;y!=0;y--)
  // Pour chaque ligne
  {
    // On fait une copie de la ligne
    for (dy=width;dy>0;dy--)
    {
      *(Dest+1)=*Dest=*Src;
      Src++;
      Dest+=2;
    }
    // On double la ligne qu'on vient de copier
    memcpy(Dest-width*ZOOMX+Largeur_ecran*ZOOMX,Dest-width*ZOOMX,width*ZOOMX);
    
    // On passe � la ligne suivante
    Src+=image_width-width;
    Dest+=Largeur_ecran*4 - width*2;
  }
  //UpdateRect(0,0,width,height);
}

void Pixel_Preview_Normal_Double (word X,word Y,byte Couleur)
/* Affichage d'un pixel dans l'�cran, par rapport au d�calage de l'image 
 * dans l'�cran, en mode normal (pas en mode loupe)
 * Note: si on modifie cette proc�dure, il faudra penser � faire �galement 
 * la modif dans la proc�dure Pixel_Preview_Loupe_SDL. */
{
//  if(X-Principal_Decalage_X >= 0 && Y - Principal_Decalage_Y >= 0)
  Pixel_Double(X-Principal_Decalage_X,Y-Principal_Decalage_Y,Couleur);
}

void Pixel_Preview_Loupe_Double  (word X,word Y,byte Couleur)
{
  // Affiche le pixel dans la partie non zoom�e
  Pixel_Double(X-Principal_Decalage_X,Y-Principal_Decalage_Y,Couleur);
  
  // Regarde si on doit aussi l'afficher dans la partie zoom�e
  if (Y >= Limite_Haut_Zoom && Y <= Limite_visible_Bas_Zoom
          && X >= Limite_Gauche_Zoom && X <= Limite_visible_Droite_Zoom)
  {
    // On est dedans
    int height;
    int Y_Zoom = Table_mul_facteur_zoom[Y-Loupe_Decalage_Y];

    if (Menu_Ordonnee - Y_Zoom < Loupe_Facteur)
      // On ne doit dessiner qu'un morceau du pixel
      // sinon on d�passe sur le menu
      height = Menu_Ordonnee - Y_Zoom;
    else
      height = Loupe_Facteur;

    Block_Double(
      Table_mul_facteur_zoom[X-Loupe_Decalage_X]+Principal_X_Zoom, 
      Y_Zoom, Loupe_Facteur, height, Couleur
      );
  }
}

void Ligne_horizontale_XOR_Double(word Pos_X,word Pos_Y,word width)
{
  //On calcule la valeur initiale de Dest:
  byte* Dest=Pos_Y*4*Largeur_ecran+Pos_X*2+Ecran;

  int X;

  for (X=0;X<width*2;X+=2)
    *(Dest+X+2*Largeur_ecran+1)=*(Dest+X+2*Largeur_ecran)=*(Dest+X+1)=*(Dest+X)=~*(Dest+X);
}

void Ligne_verticale_XOR_Double(word Pos_X,word Pos_Y,word height)
{
  int i;
  byte *Dest=Ecran+Pos_X*2+Pos_Y*Largeur_ecran*4;
  for (i=height;i>0;i--)
  {
    *Dest=*(Dest+1)=*(Dest+Largeur_ecran*2)=*(Dest+Largeur_ecran*2+1)=~*Dest;
    Dest+=Largeur_ecran*4;
  }
}

void Display_brush_Color_Double(word Pos_X,word Pos_Y,word x_offset,word y_offset,word width,word height,byte Couleur_de_transparence,word Largeur_brosse)
{
  // Dest = Position � l'�cran
  byte* Dest = Ecran + Pos_Y * 4 * Largeur_ecran + Pos_X * 2;
  // Src = Position dans la brosse
  byte* Src = Brosse + y_offset * Largeur_brosse + x_offset;

  word x,y;

  // Pour chaque ligne
  for(y = height;y > 0; y--)
  {
    // Pour chaque pixel
    for(x = width;x > 0; x--)
    {
      // On v�rifie que ce n'est pas la transparence
      if(*Src != Couleur_de_transparence)
      {
        *(Dest+Largeur_ecran*2+1) = *(Dest+Largeur_ecran*2) = *(Dest+1) = *Dest = *Src;
      }

      // Pixel suivant
      Src++;
      Dest+=2;
    }

    // On passe � la ligne suivante
    Dest = Dest + Largeur_ecran*4 - width*2;
    Src = Src + Largeur_brosse - width;
  }
  UpdateRect(Pos_X,Pos_Y,width,height);
}

void Display_brush_Mono_Double(word Pos_X, word Pos_Y,
        word x_offset, word y_offset, word width, word height,
        byte Couleur_de_transparence, byte Couleur, word Largeur_brosse)
/* On affiche la brosse en monochrome */
{
  byte* Dest=Pos_Y*4*Largeur_ecran+Pos_X*2+Ecran; // Dest = adr destination � 
      // l'�cran
  byte* Src=Largeur_brosse*y_offset+x_offset+Brosse; // Src = adr ds 
      // la brosse
  int x,y;

  for(y=height;y!=0;y--)
  //Pour chaque ligne
  {
    for(x=width;x!=0;x--)
    //Pour chaque pixel
    {
      if (*Src!=Couleur_de_transparence)
        *(Dest+Largeur_ecran*2+1)=*(Dest+Largeur_ecran*2)=*(Dest+1)=*Dest=Couleur;

      // On passe au pixel suivant
      Src++;
      Dest+=2;
    }

    // On passe � la ligne suivante
    Src+=Largeur_brosse-width;
    Dest+=Largeur_ecran*4-width*2;
  }
  UpdateRect(Pos_X,Pos_Y,width,height);
}

void Clear_brush_Double(word Pos_X,word Pos_Y,__attribute__((unused)) word x_offset,__attribute__((unused)) word y_offset,word width,word height,__attribute__((unused))byte Couleur_de_transparence,word image_width)
{
  byte* Dest=Ecran+Pos_X*2+Pos_Y*4*Largeur_ecran; //On va se mettre en 0,0 dans l'�cran (Dest)
  byte* Src = ( Pos_Y + Principal_Decalage_Y ) * image_width + Pos_X + Principal_Decalage_X + Principal_Ecran; //Coords de d�part ds la source (Src)
  int y;
  int x;

  for(y=height;y!=0;y--)
  // Pour chaque ligne
  {
    for(x=width;x!=0;x--)
    //Pour chaque pixel
    {
      *(Dest+Largeur_ecran*2+1)=*(Dest+Largeur_ecran*2)=*(Dest+1)=*Dest=*Src;

      // On passe au pixel suivant
      Src++;
      Dest+=2;
    }

    // On passe � la ligne suivante
    Src+=image_width-width;
    Dest+=Largeur_ecran*4-width*2;
  }
  UpdateRect(Pos_X,Pos_Y,width,height);
}

// Affiche une brosse (arbitraire) � l'�cran
void Affiche_brosse_Double(byte * brush, word Pos_X,word Pos_Y,word x_offset,word y_offset,word width,word height,byte Couleur_de_transparence,word Largeur_brosse)
{
  // Dest = Position � l'�cran
  byte* Dest = Ecran + Pos_Y * 4 * Largeur_ecran + Pos_X * 2;
  // Src = Position dans la brosse
  byte* Src = brush + y_offset * Largeur_brosse + x_offset;
  
  word x,y;
  
  // Pour chaque ligne
  for(y = height;y > 0; y--)
  {
    // Pour chaque pixel
    for(x = width;x > 0; x--)
    {
      // On v�rifie que ce n'est pas la transparence
      if(*Src != Couleur_de_transparence)
      {
        *(Dest+Largeur_ecran*2+1)=*(Dest+Largeur_ecran*2)=*(Dest+1)=*Dest=*Src;
      }

      // Pixel suivant
      Src++; Dest+=2;
    }

    // On passe � la ligne suivante
    Dest = Dest + Largeur_ecran*4 - width*2;
    Src = Src + Largeur_brosse - width;
  }
}

void Remap_screen_Double(word Pos_X,word Pos_Y,word width,word height,byte * Table_de_conversion)
{
  // Dest = coords a l'�cran
  byte* Dest = Ecran + Pos_Y * 4 * Largeur_ecran + Pos_X * 2;
  int x,y;

  // Pour chaque ligne
  for(y=height;y>0;y--)
  {
    // Pour chaque pixel
    for(x=width;x>0;x--)
    {
      *(Dest+Largeur_ecran*2+1)=*(Dest+Largeur_ecran*2)=*(Dest+1)=*Dest=
        Table_de_conversion[*Dest];
      Dest +=2;
    }

    Dest = Dest + Largeur_ecran*4 - width*2;
  }

  UpdateRect(Pos_X,Pos_Y,width,height);
}

void Afficher_une_ligne_ecran_fast_Double(word Pos_X,word Pos_Y,word width,byte * line)
/* On affiche toute une ligne de pixels telle quelle. */
/* Utilis�e si le buffer contient d�ja des pixel doubl�s. */
{
  memcpy(Ecran+Pos_X*2+Pos_Y*4*Largeur_ecran,line,width*2);
  memcpy(Ecran+Pos_X*2+(Pos_Y*4+2)*Largeur_ecran,line,width*2);
}

void Afficher_une_ligne_ecran_Double(word Pos_X,word Pos_Y,word width,byte * line)
/* On affiche une ligne de pixels en les doublant. */
{
  int x;
  byte *Dest;
  Dest=Ecran+Pos_X*2+Pos_Y*4*Largeur_ecran;
  for(x=width;x>0;x--)
  {
    *(Dest+Largeur_ecran*2+1)=*(Dest+Largeur_ecran*2)=*(Dest+1)=*Dest=*line;
    Dest+=2;
    line++;
  }
}
void Afficher_une_ligne_transparente_mono_a_l_ecran_Double(
        word Pos_X, word Pos_Y, word width, byte* line, 
        byte Couleur_transparence, byte Couleur)
// Affiche une ligne � l'�cran avec une couleur + transparence.
// Utilis� par les brosses en mode zoom
{
  byte* Dest = Ecran+ Pos_Y*ZOOMX*Largeur_ecran + Pos_X*ZOOMX;
  int x;
  // Pour chaque pixel
  for(x=0;x<width;x++)
  {
    if (Couleur_transparence!=*line)
    {
      *(Dest+1)=*Dest=Couleur;
    }
    line ++; // Pixel suivant
    Dest+=2;
  }
}

void Lire_une_ligne_ecran_Double(word Pos_X,word Pos_Y,word width,byte * line)
{
  memcpy(line,Largeur_ecran * 4 * Pos_Y + Pos_X * 2 + Ecran,width*2);
}

void Afficher_partie_de_l_ecran_zoomee_Double(
        word width, // width non zoom�e
        word height, // height zoom�e
        word image_width,byte * Buffer)
{
  byte* Src = Principal_Ecran + Loupe_Decalage_Y * image_width 
                      + Loupe_Decalage_X;
  int y = 0; // Ligne en cours de traitement

  // Pour chaque ligne � zoomer
  while(1)
  {
    int x;
    
    // On �clate la ligne
    Zoomer_une_ligne(Src,Buffer,Loupe_Facteur*ZOOMX,width);
    // On l'affiche Facteur fois, sur des lignes cons�cutives
    x = Loupe_Facteur/**ZOOMY*/;
    // Pour chaque ligne
    do{
      // On affiche la ligne zoom�e
      Afficher_une_ligne_ecran_fast_Double(
        Principal_X_Zoom, y, width*Loupe_Facteur,
        Buffer
      );
      // On passe � la suivante
      y++;
      if(y==height/**ZOOMY*/)
      {
        UpdateRect(Principal_X_Zoom,0,
          width*Loupe_Facteur,height);
        return;
      }
      x--;
    }while (x > 0);
    Src += image_width;
  }
// ATTENTION on n'arrive jamais ici !
}

// Affiche une partie de la brosse couleur zoom�e
void Display_brush_Color_zoom_Double(word Pos_X,word Pos_Y,
        word x_offset,word y_offset,
        word width, // width non zoom�e
        word Pos_Y_Fin,byte Couleur_de_transparence,
        word Largeur_brosse, // width r�elle de la brosse
        byte * Buffer)
{
  byte* Src = Brosse+y_offset*Largeur_brosse + x_offset;
  word y = Pos_Y;
  byte bx;

  // Pour chaque ligne
  while(1)
  {
    Zoomer_une_ligne(Src,Buffer,Loupe_Facteur,width);
    // On affiche facteur fois la ligne zoom�e
    for(bx=Loupe_Facteur;bx>0;bx--)
    {
      Afficher_une_ligne_transparente_a_l_ecran_Wide(Pos_X,y*ZOOMX,width*Loupe_Facteur,Buffer,Couleur_de_transparence);
      // TODO: pas clair ici
      memcpy(Ecran + (y*ZOOMY+1)*ZOOMX*Largeur_ecran + Pos_X*ZOOMX, Ecran + y*ZOOMX*ZOOMY*Largeur_ecran + Pos_X*ZOOMX, width*ZOOMX*Loupe_Facteur);
      y++;
      if(y==Pos_Y_Fin)
      {
        return;
      }
    }
    Src += Largeur_brosse;
  }
  // ATTENTION zone jamais atteinte
}

void Display_brush_Mono_zoom_Double(word Pos_X, word Pos_Y,
        word x_offset, word y_offset, 
        word width, // width non zoom�e 
        word Pos_Y_Fin,
        byte Couleur_de_transparence, byte Couleur, 
        word Largeur_brosse, // width r�elle de la brosse
        byte * Buffer
)

{
  byte* Src = Brosse + y_offset * Largeur_brosse + x_offset;
  int y=Pos_Y*ZOOMY;

  //Pour chaque ligne � zoomer :
  while(1)
  {
    int BX;
    // Src = Ligne originale
    // On �clate la ligne
    Zoomer_une_ligne(Src,Buffer,Loupe_Facteur,width);

    // On affiche la ligne Facteur fois � l'�cran (sur des
    // lignes cons�cutives)
    BX = Loupe_Facteur*ZOOMX;

    // Pour chaque ligne �cran
    do
    {
      // On affiche la ligne zoom�e
      Afficher_une_ligne_transparente_mono_a_l_ecran_Double(
        Pos_X, y, width * Loupe_Facteur, 
        Buffer, Couleur_de_transparence, Couleur
      );
      // On passe � la ligne suivante
      y++;
      // On v�rifie qu'on est pas � la ligne finale
      if(y == Pos_Y_Fin*ZOOMX)
      {
        UpdateRect( Pos_X, Pos_Y,
          width * Loupe_Facteur, Pos_Y_Fin - Pos_Y );
        return;
      }
      BX --;
    }
    while (BX > 0);
    
    // Passage � la ligne suivante dans la brosse aussi
    Src+=Largeur_brosse;
  }
}

void Clear_brush_zoom_Double(word Pos_X,word Pos_Y,word x_offset,word y_offset,word width,word Pos_Y_Fin,__attribute__((unused)) byte Couleur_de_transparence,word image_width,byte * Buffer)
{

  // En fait on va recopier l'image non zoom�e dans la partie zoom�e !
  byte* Src = Principal_Ecran + y_offset * image_width + x_offset;
  int y = Pos_Y;
  int bx;

  // Pour chaque ligne � zoomer
  while(1){
    Zoomer_une_ligne(Src,Buffer,Loupe_Facteur*2,width);

    bx=Loupe_Facteur;

    // Pour chaque ligne
    do{
      // TODO a verifier
      Afficher_une_ligne_ecran_fast_Double(Pos_X,y,
        width * Loupe_Facteur,Buffer);

      // Ligne suivante
      y++;
      if(y==Pos_Y_Fin)
      {
        UpdateRect(Pos_X,Pos_Y,
          width*Loupe_Facteur,Pos_Y_Fin-Pos_Y);
        return;
      }
      bx--;
    }while(bx!=0);

    Src+= image_width;
  }
}


