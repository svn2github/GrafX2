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

void Pixel_Wide (word X,word Y,byte Couleur)
/* Affiche un pixel de la Couleur aux coords X;Y � l'�cran */
{
  *(Ecran + X * 2 + Y * 2 * Largeur_ecran)=Couleur;
  *(Ecran + X * 2 + Y * 2 * Largeur_ecran + 1)=Couleur;
}

byte Lit_Pixel_Wide (word X,word Y)
/* On retourne la couleur du pixel aux coords donn�es */
{
  return *( Ecran + Y * 2 * Largeur_ecran + X * 2);
}

void Block_Wide (word Debut_X,word Debut_Y,word Largeur,word Hauteur,byte Couleur)
/* On affiche un rectangle de la couleur donn�e */
{
  SDL_Rect rectangle;
  rectangle.x=Debut_X*2;
  rectangle.y=Debut_Y;
  rectangle.w=Largeur*2;
  rectangle.h=Hauteur;
  SDL_FillRect(Ecran_SDL,&rectangle,Couleur);
}

void Afficher_partie_de_l_ecran_Wide (word Largeur,word Hauteur,word Largeur_image)
/* Afficher une partie de l'image telle quelle sur l'�cran */
{
  byte* Dest=Ecran; //On va se mettre en 0,0 dans l'�cran (Dest)
  byte* Src=Principal_Decalage_Y*Largeur_image+Principal_Decalage_X+Principal_Ecran; //Coords de d�part ds la source (Src)
  int dx;
  int dy;

  for(dx=Hauteur;dx!=0;dx--)
  // Pour chaque ligne
  {
    // On fait une copie de la ligne
    for (dy=Largeur;dy>0;dy--)
    {
      *(Dest+1)=*Dest=*Src;
      Src++;
      Dest+=2;
    }

    // On passe � la ligne suivante
    Src+=Largeur_image-Largeur;
    Dest+=(Largeur_ecran - Largeur)*2;
  }
  //UpdateRect(0,0,Largeur,Hauteur);
}

void Pixel_Preview_Normal_Wide (word X,word Y,byte Couleur)
/* Affichage d'un pixel dans l'�cran, par rapport au d�calage de l'image 
 * dans l'�cran, en mode normal (pas en mode loupe)
 * Note: si on modifie cette proc�dure, il faudra penser � faire �galement 
 * la modif dans la proc�dure Pixel_Preview_Loupe_SDL. */
{
//  if(X-Principal_Decalage_X >= 0 && Y - Principal_Decalage_Y >= 0)
  Pixel_Wide(X-Principal_Decalage_X,Y-Principal_Decalage_Y,Couleur);
}

void Pixel_Preview_Loupe_Wide  (word X,word Y,byte Couleur)
{
  // Affiche le pixel dans la partie non zoom�e
  Pixel_Wide(X-Principal_Decalage_X,Y-Principal_Decalage_Y,Couleur);
  
  // Regarde si on doit aussi l'afficher dans la partie zoom�e
  if (Y >= Limite_Haut_Zoom && Y <= Limite_visible_Bas_Zoom
          && X >= Limite_Gauche_Zoom && X <= Limite_visible_Droite_Zoom)
  {
    // On est dedans
    int hauteur;
    int Y_Zoom = Table_mul_facteur_zoom[Y-Loupe_Decalage_Y];

    if (Menu_Ordonnee - Y_Zoom < Loupe_Facteur)
      // On ne doit dessiner qu'un morceau du pixel
      // sinon on d�passe sur le menu
      hauteur = Menu_Ordonnee - Y_Zoom;
    else
      hauteur = Loupe_Facteur;

    Block_Wide(
      Table_mul_facteur_zoom[X-Loupe_Decalage_X]+Principal_X_Zoom, 
      Y_Zoom, Loupe_Facteur, hauteur, Couleur
      );
  }
}

void Ligne_horizontale_XOR_Wide(word Pos_X,word Pos_Y,word Largeur)
{
  //On calcule la valeur initiale de Dest:
  byte* Dest=Pos_Y*2*Largeur_ecran+Pos_X*2+Ecran;

  int ecx;

  for (ecx=0;ecx<Largeur*2;ecx+=2)
    *(Dest+ecx+1)=*(Dest+ecx)=~*(Dest+ecx);
}

void Ligne_verticale_XOR_Wide(word Pos_X,word Pos_Y,word Hauteur)
{
  int i;
  byte color;
  byte *Dest=Ecran+Pos_X*2+Pos_Y*Largeur_ecran*2;
  for (i=Hauteur;i>0;i--)
  {
    color=~*Dest;
    *Dest=color;
    *(Dest+1)=color;
    Dest+=Largeur_ecran*2;
  }
}

void Display_brush_Color_Wide(word Pos_X,word Pos_Y,word Decalage_X,word Decalage_Y,word Largeur,word Hauteur,byte Couleur_de_transparence,word Largeur_brosse)
{
  // Dest = Position � l'�cran
  byte* Dest = Ecran + Pos_Y * 2 * Largeur_ecran + Pos_X * 2;
  // Src = Position dans la brosse
  byte* Src = Brosse + Decalage_Y * Largeur_brosse + Decalage_X;

  word DX,CX;

  // Pour chaque ligne
  for(DX = Hauteur;DX > 0; DX--)
  {
    // Pour chaque pixel
    for(CX = Largeur;CX > 0; CX--)
    {
      // On v�rifie que ce n'est pas la transparence
      if(*Src != Couleur_de_transparence)
      {
        *(Dest+1) = *Dest = *Src;
      }

      // Pixel suivant
      Src++;
      Dest+=2;
    }

    // On passe � la ligne suivante
    Dest = Dest + (Largeur_ecran - Largeur)*2;
    Src = Src + Largeur_brosse - Largeur;
  }
  UpdateRect(Pos_X,Pos_Y,Largeur,Hauteur);
}

void Display_brush_Mono_Wide(word Pos_X, word Pos_Y,
        word Decalage_X, word Decalage_Y, word Largeur, word Hauteur,
        byte Couleur_de_transparence, byte Couleur, word Largeur_brosse)
/* On affiche la brosse en monochrome */
{
  byte* Dest=Pos_Y*2*Largeur_ecran+Pos_X*2+Ecran; // Dest = adr Destination � 
      // l'�cran
  byte* Src=Largeur_brosse*Decalage_Y+Decalage_X+Brosse; // Src = adr ds 
      // la brosse
  int dx,cx;

  for(dx=Hauteur;dx!=0;dx--)
  //Pour chaque ligne
  {
    for(cx=Largeur;cx!=0;cx--)
    //Pour chaque pixel
    {
      if (*Src!=Couleur_de_transparence)
        *(Dest+1)=*Dest=Couleur;

      // On passe au pixel suivant
      Src++;
      Dest+=2;
    }

    // On passe � la ligne suivante
    Src+=Largeur_brosse-Largeur;
    Dest+=(Largeur_ecran-Largeur)*2;
  }
  UpdateRect(Pos_X,Pos_Y,Largeur,Hauteur);
}

void Clear_brush_Wide(word Pos_X,word Pos_Y,__attribute__((unused)) word Decalage_X,__attribute__((unused)) word Decalage_Y,word Largeur,word Hauteur,__attribute__((unused))byte Couleur_de_transparence,word Largeur_image)
{
  byte* Dest=Ecran+Pos_X*2+Pos_Y*2*Largeur_ecran; //On va se mettre en 0,0 dans l'�cran (Dest)
  byte* Src = ( Pos_Y + Principal_Decalage_Y ) * Largeur_image + Pos_X + Principal_Decalage_X + Principal_Ecran; //Coords de d�part ds la source (Src)
  int dx;
  int cx;

  for(dx=Hauteur;dx!=0;dx--)
  // Pour chaque ligne
  {
    for(cx=Largeur;cx!=0;cx--)
    //Pour chaque pixel
    {
      *(Dest+1)=*Dest=*Src;

      // On passe au pixel suivant
      Src++;
      Dest+=2;
    }

    // On passe � la ligne suivante
    Src+=Largeur_image-Largeur;
    Dest+=(Largeur_ecran-Largeur)*2;
  }
  UpdateRect(Pos_X,Pos_Y,Largeur,Hauteur);
}

// Affiche une brosse (arbitraire) � l'�cran
void Affiche_brosse_Wide(byte * B, word Pos_X,word Pos_Y,word Decalage_X,word Decalage_Y,word Largeur,word Hauteur,byte Couleur_de_transparence,word Largeur_brosse)
{
  // Dest = Position � l'�cran
  byte* Dest = Ecran + Pos_Y * 2 * Largeur_ecran + Pos_X * 2;
  // Src = Position dans la brosse
  byte* Src = B + Decalage_Y * Largeur_brosse + Decalage_X;
  
  word DX,CX;
  
  // Pour chaque ligne
  for(DX = Hauteur;DX > 0; DX--)
  {
    // Pour chaque pixel
    for(CX = Largeur;CX > 0; CX--)
    {
      // On v�rifie que ce n'est pas la transparence
      if(*Src != Couleur_de_transparence)
      {
        *(Dest+1) = *Dest = *Src;
      }

      // Pixel suivant
      Src++; Dest+=2;
    }

    // On passe � la ligne suivante
    Dest = Dest + (Largeur_ecran - Largeur)*2;
    Src = Src + Largeur_brosse - Largeur;
  }
}

void Remap_screen_Wide(word Pos_X,word Pos_Y,word Largeur,word Hauteur,byte * Table_de_conversion)
{
  // Dest = coords a l'�cran
  byte* Dest = Ecran + Pos_Y * 2 * Largeur_ecran + Pos_X * 2;
  int dx,cx;

  // Pour chaque ligne
  for(dx=Hauteur;dx>0;dx--)
  {
    // Pour chaque pixel
    for(cx=Largeur;cx>0;cx--)
    {
      *(Dest+1) = *Dest = Table_de_conversion[*Dest];
      Dest +=2;
    }

    Dest = Dest + (Largeur_ecran - Largeur)*2;
  }

  UpdateRect(Pos_X,Pos_Y,Largeur,Hauteur);
}

void Afficher_une_ligne_ecran_fast_Wide(word Pos_X,word Pos_Y,word Largeur,byte * Ligne)
/* On affiche toute une ligne de pixels telle quelle. */
/* Utilis�e si le buffer contient d�ja des pixel doubl�s. */
{
  memcpy(Ecran+Pos_X*2+Pos_Y*2*Largeur_ecran,Ligne,Largeur*2);
}

void Afficher_une_ligne_ecran_Wide(word Pos_X,word Pos_Y,word Largeur,byte * Ligne)
/* On affiche une ligne de pixels en les doublant. */
{
  int dx;
  byte *Dest;
  Dest=Ecran+Pos_X*2+Pos_Y*2*Largeur_ecran;
  for(dx=Largeur;dx>0;dx--)
  {
    *Dest=*Ligne;
    Dest++;
    *Dest=*Ligne;
    Dest++;
    Ligne++;
  }
}
void Afficher_une_ligne_transparente_mono_a_l_ecran_Wide(
        word Pos_X, word Pos_Y, word Largeur, byte* Ligne, 
        byte Couleur_transparence, byte Couleur)
// Affiche une ligne � l'�cran avec une couleur + transparence.
// Utilis� par les brosses en mode zoom
{
  byte* Dest = Ecran+ Pos_Y*2 * Largeur_ecran + Pos_X*2;
  int Compteur;
  // Pour chaque pixel
  for(Compteur=0;Compteur<Largeur;Compteur++)
  {
    if (Couleur_transparence!=*Ligne)
    {
      *Dest = Couleur;
      *(Dest+1) = Couleur;
    }
    Ligne ++; // Pixel suivant
    Dest+=2;
  }
}

void Lire_une_ligne_ecran_Wide(word Pos_X,word Pos_Y,word Largeur,byte * Ligne)
{
  memcpy(Ligne,Largeur_ecran * 2 * Pos_Y + Pos_X * 2 + Ecran,Largeur*2);
}

void Afficher_partie_de_l_ecran_zoomee_Wide(
        word Largeur, // Largeur non zoom�e
        word Hauteur, // Hauteur zoom�e
        word Largeur_image,byte * Buffer)
{
  byte* Src = Principal_Ecran + Loupe_Decalage_Y * Largeur_image 
                      + Loupe_Decalage_X;
  int EDX = 0; // Ligne en cours de traitement

  // Pour chaque ligne � zoomer
  while(1)
  {
    int CX;
    
    // On �clate la ligne
    Zoomer_une_ligne(Src,Buffer,Loupe_Facteur*2,Largeur);
    // On l'affiche Facteur fois, sur des lignes cons�cutives
    CX = Loupe_Facteur;
    // Pour chaque ligne
    do{
      // On affiche la ligne zoom�e
      Afficher_une_ligne_ecran_fast_Wide(
        Principal_X_Zoom, EDX, Largeur*Loupe_Facteur,
        Buffer
      );
      // On passe � la suivante
      EDX++;
      if(EDX==Hauteur)
      {
        UpdateRect(Principal_X_Zoom,0,
          Largeur*Loupe_Facteur,Hauteur);
        return;
      }
      CX--;
    }while (CX > 0);
    Src += Largeur_image;
  }
// ATTENTION on n'arrive jamais ici !
}

void Afficher_une_ligne_transparente_a_l_ecran_Wide(word Pos_X,word Pos_Y,word Largeur,byte* Ligne,byte Couleur_transparence)
{
  byte* Src = Ligne;
  byte* Dest = Ecran + Pos_Y * 2 * Largeur_ecran + Pos_X * 2;

  word cx;

  // Pour chaque pixel de la ligne
  for(cx = Largeur;cx > 0;cx--)
  {
    if(*Src!=Couleur_transparence)
    {
      *Dest = *Src;
      *(Dest+1) = *Src;
    }
    Src++;
    Dest+=2;
  }
}

// Affiche une partie de la brosse couleur zoom�e
void Display_brush_Color_zoom_Wide(word Pos_X,word Pos_Y,
        word Decalage_X,word Decalage_Y,
        word Largeur, // Largeur non zoom�e
        word Pos_Y_Fin,byte Couleur_de_transparence,
        word Largeur_brosse, // Largeur r�elle de la brosse
        byte * Buffer)
{
  byte* Src = Brosse+Decalage_Y*Largeur_brosse + Decalage_X;
  word DX = Pos_Y;
  byte bx;

  // Pour chaque ligne
  while(1)
  {
    Zoomer_une_ligne(Src,Buffer,Loupe_Facteur,Largeur);
    // On affiche facteur fois la ligne zoom�e
    for(bx=Loupe_Facteur;bx>0;bx--)
    {
      Afficher_une_ligne_transparente_a_l_ecran_Wide(Pos_X,DX,Largeur*Loupe_Facteur,Buffer,Couleur_de_transparence);
      DX++;
      if(DX==Pos_Y_Fin)
      {
        return;
      }
    }
    Src += Largeur_brosse;
  }
  // ATTENTION zone jamais atteinte
}

void Display_brush_Mono_zoom_Wide(word Pos_X, word Pos_Y,
        word Decalage_X, word Decalage_Y, 
        word Largeur, // Largeur non zoom�e 
        word Pos_Y_Fin,
        byte Couleur_de_transparence, byte Couleur, 
        word Largeur_brosse, // Largeur r�elle de la brosse
        byte * Buffer
)

{
  byte* Src = Brosse + Decalage_Y * Largeur_brosse + Decalage_X;
  int DX=Pos_Y;

  //Pour chaque ligne � zoomer :
  while(1)
  {
    int BX;
    // Src = Ligne originale
    // On �clate la ligne
    Zoomer_une_ligne(Src,Buffer,Loupe_Facteur,Largeur);

    // On affiche la ligne Facteur fois � l'�cran (sur des
    // lignes cons�cutives)
    BX = Loupe_Facteur;

    // Pour chaque ligne �cran
    do
    {
      // On affiche la ligne zoom�e
      Afficher_une_ligne_transparente_mono_a_l_ecran_Wide(
        Pos_X, DX, Largeur * Loupe_Facteur, 
        Buffer, Couleur_de_transparence, Couleur
      );
      // On passe � la ligne suivante
      DX++;
      // On v�rifie qu'on est pas � la ligne finale
      if(DX == Pos_Y_Fin)
      {
        UpdateRect( Pos_X, Pos_Y,
          Largeur * Loupe_Facteur, Pos_Y_Fin - Pos_Y );
        return;
      }
      BX --;
    }
    while (BX > 0);
    
    // Passage � la ligne suivante dans la brosse aussi
    Src+=Largeur_brosse;
  }
}

void Clear_brush_zoom_Wide(word Pos_X,word Pos_Y,word Decalage_X,word Decalage_Y,word Largeur,word Pos_Y_Fin,__attribute__((unused)) byte Couleur_de_transparence,word Largeur_image,byte * Buffer)
{
  // En fait on va recopier l'image non zoom�e dans la partie zoom�e !
  byte* Src = Principal_Ecran + Decalage_Y * Largeur_image + Decalage_X;
  int DX = Pos_Y;
  int bx;

  // Pour chaque ligne � zoomer
  while(1){
    Zoomer_une_ligne(Src,Buffer,Loupe_Facteur*2,Largeur);

    bx=Loupe_Facteur;

    // Pour chaque ligne
    do{
      Afficher_une_ligne_ecran_fast_Wide(Pos_X,DX,
        Largeur * Loupe_Facteur,Buffer);

      // Ligne suivante
      DX++;
      if(DX==Pos_Y_Fin)
      {
        UpdateRect(Pos_X,Pos_Y,
          Largeur*Loupe_Facteur,Pos_Y_Fin-Pos_Y);
        return;
      }
      bx--;
    }while(bx!=0);

    Src+= Largeur_image;
  }
}


