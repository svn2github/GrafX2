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
#include <SDL/SDL.h>
#include "global.h"
#include "sdlscreen.h"
#include "divers.h"
#include "erreurs.h"
#include "graph.h"

// Mise � jour minimaliste en nombre de pixels
#define METHODE_UPDATE_MULTI_RECTANGLE 1
// Mise � jour interm�diaire, par rectangle inclusif.
#define METHODE_UPDATE_PAR_CUMUL       2
// Mise � jour totale, pour les plate-formes qui imposent un Vsync � chaque mise � jour �cran.
#define METHODE_UPDATE_PLEINE_PAGE     3

// METHODE_UPDATE peut �tre fix� depuis le makefile, sinon c'est ici:
#ifndef METHODE_UPDATE
  #ifdef __macosx__
    #define METHODE_UPDATE     METHODE_UPDATE_PLEINE_PAGE
  #else
    #define METHODE_UPDATE     METHODE_UPDATE_MULTI_RECTANGLE
  #endif
#endif


void Pixel_SDL (word X,word Y,byte Couleur)
/* Affiche un pixel de la Couleur aux coords X;Y � l'�cran */
{
    *(Ecran + X + Y * Largeur_ecran)=Couleur;
}

byte Lit_Pixel_SDL (word X,word Y)
/* On retourne la couleur du pixel aux coords donn�es */
{
        return *( Ecran + Y * Largeur_ecran + X );
}

void Afficher_partie_de_l_ecran_SDL       (word Largeur,word Hauteur,word Largeur_image)
/* Afficher une partie de l'image telle quelle sur l'�cran */
{
    byte* Dest=Ecran; //On va se mettre en 0,0 dans l'�cran (EDI)
    byte* Src=Principal_Decalage_Y*Largeur_image+Principal_Decalage_X+Principal_Ecran; //Coords de d�part ds la source (ESI)
    int dx;

  for(dx=Hauteur;dx!=0;dx--)
  // Pour chaque ligne
  {
    // On fait une copie de la ligne
    memcpy(Dest,Src,Largeur);

    // On passe � la ligne suivante
    Src+=Largeur_image;
    Dest+=Largeur_ecran;
  }
  UpdateRect(0,0,Largeur,Hauteur);
}

void Block_SDL (word Debut_X,word Debut_Y,word Largeur,word Hauteur,byte Couleur)
/* On affiche un rectangle de la couleur donn�e */
{
        SDL_Rect rectangle;
        rectangle.x=Debut_X;
        rectangle.y=Debut_Y;
        rectangle.w=Largeur;
        rectangle.h=Hauteur;
        SDL_FillRect(Ecran_SDL,&rectangle,Couleur);
//        UpdateRect(Debut_X,Debut_Y,Largeur,Hauteur);
}

void Pixel_Preview_Normal_SDL (word X,word Y,byte Couleur)
/* Affichage d'un pixel dans l'�cran, par rapport au d�calage de l'image 
 * dans l'�cran, en mode normal (pas en mode loupe)
 * Note: si on modifie cette proc�dure, il faudra penser � faire �galement 
 * la modif dans la proc�dure Pixel_Preview_Loupe_SDL. */
{
//      if(X-Principal_Decalage_X >= 0 && Y - Principal_Decalage_Y >= 0)
                Pixel_SDL(X-Principal_Decalage_X,Y-Principal_Decalage_Y,Couleur);
}

void Pixel_Preview_Loupe_SDL  (word X,word Y,byte Couleur)
{
        // Affiche le pixel dans la partie non zoom�e
        Pixel_SDL(X-Principal_Decalage_X,Y-Principal_Decalage_Y,Couleur);
        
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

		Block_SDL(
                        Table_mul_facteur_zoom[X-Loupe_Decalage_X] 
                                + Principal_X_Zoom, 
                        Y_Zoom, Loupe_Facteur, hauteur, Couleur
                );
        }
}

void Ligne_horizontale_XOR_SDL(word Pos_X,word Pos_Y,word Largeur)
{
  //On calcule la valeur initiale de EDI:
  byte* edi=Pos_Y*Largeur_ecran+Pos_X+Ecran;

  int ecx;

  for (ecx=0;ecx<Largeur;ecx++)
        *(edi+ecx)=~*(edi+ecx);
}

void Ligne_verticale_XOR_SDL  (word Pos_X,word Pos_Y,word Hauteur)
{
    int i;
    byte color;
    for (i=Pos_Y;i<Pos_Y+Hauteur;i++)
    {
        color=*(Ecran+Pos_X+i*Largeur_ecran);
        *(Ecran+Pos_X+i*Largeur_ecran)=~color;
    }
}

void Display_brush_Color_SDL  (word Pos_X,word Pos_Y,word Decalage_X,word Decalage_Y,word Largeur,word Hauteur,byte Couleur_de_transparence,word Largeur_brosse)
{
        // EDI = Position � l'�cran
        byte* EDI = Ecran + Pos_Y * Largeur_ecran + Pos_X;
        // ESI = Position dans la brosse
        byte* ESI = Brosse + Decalage_Y * Largeur_brosse + Decalage_X;

        word DX,CX;

        // Pour chaque ligne
        for(DX = Hauteur;DX > 0; DX--)
        {
                // Pour chaque pixel
                for(CX = Largeur;CX > 0; CX--)
                {
                        // On v�rifie que ce n'est pas la transparence
                        if(*ESI != Couleur_de_transparence)
                        {
                                *EDI = *ESI;
                        }

                        // Pixel suivant
                        ESI++; EDI++;
                }

                // On passe � la ligne suivante
                EDI = EDI + Largeur_ecran - Largeur;
                ESI = ESI + Largeur_brosse - Largeur;
        }
        UpdateRect(Pos_X,Pos_Y,Largeur,Hauteur);
}

void Display_brush_Mono_SDL (word Pos_X, word Pos_Y,
        word Decalage_X, word Decalage_Y, word Largeur, word Hauteur,
        byte Couleur_de_transparence, byte Couleur, word Largeur_brosse)
/* On affiche la brosse en monochrome */
{
    byte* Dest=Pos_Y*Largeur_ecran+Pos_X+Ecran; // EDI = adr destination � 
        // l'�cran
    byte* Src=Largeur_brosse*Decalage_Y+Decalage_X+Brosse; // ESI = adr ds 
        // la brosse
    int dx,cx;

    for(dx=Hauteur;dx!=0;dx--)
    //Pour chaque ligne
    {
        for(cx=Largeur;cx!=0;cx--)
        //Pour chaque pixel
        {
            if (*Src!=Couleur_de_transparence)
                *Dest=Couleur;

            // On passe au pixel suivant
            Src++;
            Dest++;
        }

        // On passe � la ligne suivante
        Src+=Largeur_brosse-Largeur;
        Dest+=Largeur_ecran-Largeur;
    }
    UpdateRect(Pos_X,Pos_Y,Largeur,Hauteur);
}

void Clear_brush_SDL (word Pos_X,word Pos_Y,word Decalage_X,word Decalage_Y,word Largeur,word Hauteur,byte Couleur_de_transparence,word Largeur_image)
{
    byte* Dest=Ecran+Pos_X+Pos_Y*Largeur_ecran; //On va se mettre en 0,0 dans l'�cran (EDI)
    byte* Src = ( Pos_Y + Principal_Decalage_Y ) * Largeur_image + Pos_X + Principal_Decalage_X + Principal_Ecran; //Coords de d�part ds la source (ESI)
    int dx;

  for(dx=Hauteur;dx!=0;dx--)
  // Pour chaque ligne
  {
    // On fait une copie de la ligne
    memcpy(Dest,Src,Largeur);

    // On passe � la ligne suivante
    Src+=Largeur_image;
    Dest+=Largeur_ecran;
  }
  UpdateRect(Pos_X,Pos_Y,Largeur,Hauteur);
}

void Remap_screen_SDL (word Pos_X,word Pos_Y,word Largeur,word Hauteur,byte * Table_de_conversion)
{
	// EDI = coords a l'�cran
	byte* EDI = Ecran + Pos_Y * Largeur_ecran + Pos_X;
	int dx,cx;

	// Pour chaque ligne
	for(dx=Hauteur;dx>0;dx--)
	{
		// Pour chaque pixel
		for(cx=Largeur;cx>0;cx--)
		{
			*EDI = Table_de_conversion[*EDI];
			EDI ++;
		}

		EDI = EDI + Largeur_ecran - Largeur;
	}

	UpdateRect(Pos_X,Pos_Y,Largeur,Hauteur);
}

void Afficher_une_ligne_ecran_SDL (word Pos_X,word Pos_Y,word Largeur,byte * Ligne)
/* On affiche toute une ligne de pixels. Utilis� pour les textes. */
{
    memcpy(Ecran+Pos_X+Pos_Y*Largeur_ecran,Ligne,Largeur);
    //UpdateRect(Pos_X,Pos_Y,Largeur,1);
}

void Afficher_une_ligne_transparente_mono_a_l_ecran_SDL(
        word Pos_X, word Pos_Y, word Largeur, byte* Ligne, 
        byte Couleur_transparence, byte Couleur)
// Affiche une ligne � l'�cran avec une couleur + transparence.
// Utilis� par les brosses en mode zoom
{
        byte* Dest = Ecran+ Pos_Y * Largeur_ecran + Pos_X;
        int Compteur;
        // Pour chaque pixel
        for(Compteur=0;Compteur<Largeur;Compteur++)
        {
                if (Couleur_transparence!=*Ligne)
                        *Dest = Couleur;
                Ligne ++; // Pixel suivant
                Dest++;
        }
}

void Lire_une_ligne_ecran_SDL (word Pos_X,word Pos_Y,word Largeur,byte * Ligne)
{
    memcpy(Ligne,Largeur_ecran * Pos_Y + Pos_X + Ecran,Largeur);
}

void Afficher_partie_de_l_ecran_zoomee_SDL(
        word Largeur, // Largeur non zoom�e
        word Hauteur, // Hauteur zoom�e
        word Largeur_image,byte * Buffer)
{
    byte* ESI = Principal_Ecran + Loupe_Decalage_Y * Largeur_image 
                        + Loupe_Decalage_X;
    int EDX = 0; // Ligne en cours de traitement

    // Pour chaque ligne � zoomer
    while(1)
    {
        int CX;
        
        // On �clate la ligne
        Zoomer_une_ligne(ESI,Buffer,Loupe_Facteur,Largeur);
        // On l'affiche Facteur fois, sur des lignes cons�cutives
        CX = Loupe_Facteur;
        // Pour chaque ligne
        do{
                // On affiche la ligne zoom�e
                Afficher_une_ligne_ecran_SDL(
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
        ESI += Largeur_image;
    }
// ATTENTION on n'arrive jamais ici !
}

void Afficher_une_ligne_transparente_a_l_ecran_SDL(word Pos_X,word Pos_Y,word Largeur,byte* Ligne,byte Couleur_transparence)
{
        byte* ESI = Ligne;
        byte* EDI = Ecran + Pos_Y * Largeur_ecran + Pos_X;

        word cx;

        // Pour chaque pixel de la ligne
        for(cx = Largeur;cx > 0;cx--)
        {
                if(*ESI!=Couleur_transparence)
                        *EDI = *ESI;
                ESI++;
                EDI++;
        }
}

// Affiche une partie de la brosse couleur zoom�e
void Display_brush_Color_zoom_SDL (word Pos_X,word Pos_Y,
        word Decalage_X,word Decalage_Y,
        word Largeur, // Largeur non zoom�e
        word Pos_Y_Fin,byte Couleur_de_transparence,
        word Largeur_brosse, // Largeur r�elle de la brosse
        byte * Buffer)
{
        byte* ESI = Brosse+Decalage_Y*Largeur_brosse + Decalage_X;
        word DX = Pos_Y;
        byte bx;

        // Pour chaque ligne
        while(1)
        {
                Zoomer_une_ligne(ESI,Buffer,Loupe_Facteur,Largeur);
                // On affiche facteur fois la ligne zoom�e
                for(bx=Loupe_Facteur;bx>0;bx--)
                {
                        Afficher_une_ligne_transparente_a_l_ecran_SDL(Pos_X,DX,Largeur*Loupe_Facteur,Buffer,Couleur_de_transparence);
                        DX++;
                        if(DX==Pos_Y_Fin)
                        {
                                return;
                        }
                }
                ESI += Largeur_brosse;
                
        }
        // ATTENTION zone jamais atteinte
}

void Display_brush_Mono_zoom_SDL (word Pos_X, word Pos_Y,
        word Decalage_X, word Decalage_Y, 
        word Largeur, // Largeur non zoom�e 
        word Pos_Y_Fin,
        byte Couleur_de_transparence, byte Couleur, 
        word Largeur_brosse, // Largeur r�elle de la brosse
        byte * Buffer
)

{
        byte* ESI = Brosse + Decalage_Y * Largeur_brosse + Decalage_X;
        int DX=Pos_Y;

        //Pour chaque ligne � zoomer :
        while(1)
        {
            int BX;
                // ESI = Ligne originale
                // On �clate la ligne
                Zoomer_une_ligne(ESI,Buffer,Loupe_Facteur,Largeur);

                // On affiche la ligne Facteur fois � l'�cran (sur des
                // lignes cons�cutives)
                BX = Loupe_Facteur;

                // Pour chaque ligne �cran
                do
                {
                        // On affiche la ligne zoom�e
                        Afficher_une_ligne_transparente_mono_a_l_ecran_SDL(
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
                ESI+=Largeur_brosse;
        }
}

void Clear_brush_zoom_SDL (word Pos_X,word Pos_Y,word Decalage_X,word Decalage_Y,word Largeur,word Pos_Y_Fin,__attribute__((unused)) byte Couleur_de_transparence,word Largeur_image,byte * Buffer)
{
        // En fait on va recopier l'image non zoom�e dans la partie zoom�e !
        byte* ESI = Principal_Ecran + Decalage_Y * Largeur_image + Decalage_X;
        int DX = Pos_Y;
        int bx;

        // Pour chaque ligne � zoomer
        while(1){
                Zoomer_une_ligne(ESI,Buffer,Loupe_Facteur,Largeur);

                bx=Loupe_Facteur;

                // Pour chaque ligne
                do{
                        Afficher_une_ligne_ecran_SDL(Pos_X,DX,
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

                ESI+= Largeur_image;
        }
}

void Set_Mode_SDL()
/* On r�gle la r�solution de l'�cran */
{
  Ecran_SDL=SDL_SetVideoMode(Largeur_ecran,Hauteur_ecran,8,SDL_FULLSCREEN*Plein_ecran|SDL_RESIZABLE);
	if(Ecran_SDL != NULL)
	{
    // V�rification du mode obtenu (ce n'est pas toujours celui demand�)
    if (Ecran_SDL->w != Largeur_ecran || Ecran_SDL->h != Hauteur_ecran)
    {
  		DEBUG("Erreur mode video obtenu diff�rent de celui demand� !!",0);
  		Largeur_ecran = Ecran_SDL->w;
  		Hauteur_ecran = Ecran_SDL->h;
    }
    Ecran=Ecran_SDL->pixels;
  }
	else
		DEBUG("Erreur changement de mode video !!",0);

        SDL_ShowCursor(0); // Cache le curseur SDL, on le g�re en soft
}

// Fonction qui filtre les evenements g�n�riques.
void Gere_Evenement_SDL(SDL_Event * event)
{
  // Redimensionnement fenetre
  if (event->type == SDL_VIDEORESIZE )
  {
    Resize_Largeur = event->resize.w;
    Resize_Hauteur = event->resize.h;
  }
  // Fermeture
  if (event->type == SDL_QUIT )
  {
    Quit_demande=1;
  }
}

#if (METHODE_UPDATE == METHODE_UPDATE_PAR_CUMUL)
short Min_X=0;
short Min_Y=0;
short Max_X=10000;
short Max_Y=10000;
#endif

#if (METHODE_UPDATE == METHODE_UPDATE_PLEINE_PAGE)
  int Update_necessaire=0;
#endif

void Flush_update(void)
{
#if (METHODE_UPDATE == METHODE_UPDATE_PLEINE_PAGE)
  // Mise � jour de la totalit� de l'�cran
  if (Update_necessaire)
  {
    SDL_UpdateRect(Ecran_SDL, 0, 0, 0, 0);
    Update_necessaire=0;
  }
#endif
  #if (METHODE_UPDATE == METHODE_UPDATE_PAR_CUMUL)
  if (Min_X>=Max_X || Min_Y>=Max_Y)
  {
    ; // Rien a faire
  }
  else
  {
    SDL_UpdateRect(Ecran_SDL, Max(Min_X,0), Max(Min_Y,0), Min(Largeur_ecran, Max_X-Min_X), Min(Hauteur_ecran, Max_Y-Min_Y));

    Min_X=Min_Y=10000;
    Max_X=Max_Y=0;
  }
    #endif
  
}

void UpdateRect(short X, short Y, unsigned short Largeur, unsigned short Hauteur)
{
  #if (METHODE_UPDATE == METHODE_UPDATE_MULTI_RECTANGLE)
    SDL_UpdateRect(Ecran_SDL, (X), (Y), (Largeur), (Hauteur));
  #endif
  
  #if (METHODE_UPDATE == METHODE_UPDATE_PAR_CUMUL)
  if (Largeur==0 || Hauteur==0)
  {
    Min_X=Min_Y=0;
    Max_X=Max_Y=10000;
  }
  else
  {
    if (X < Min_X)
      Min_X = X;
    if (Y < Min_Y)
      Min_Y = Y;
    if (X+Largeur>Max_X)
      Max_X=X+Largeur;
    if (Y+Hauteur>Max_Y)
      Max_Y=Y+Hauteur;
  }
  #endif
  
  #if (METHODE_UPDATE == METHODE_UPDATE_PLEINE_PAGE)
  Update_necessaire=1;
  #endif
}
