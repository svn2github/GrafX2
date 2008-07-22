#include <SDL/SDL.h>
#include "global.h"
#include "sdlscreen.h"
#include "divers.h"
#include "erreurs.h"

void Pixel_SDL (word X,word Y,byte Couleur)
/* Affiche un pixel de la Couleur aux coords X;Y � l'�cran */
{
    *(((Uint8 *)Ecran_SDL->pixels) + Y * Ecran_SDL->pitch + X)=Couleur;
    //SDL_UpdateRect(Ecran_SDL,X,Y,0,0);
}

byte Lit_Pixel_SDL            (word X,word Y)
/* On retourne la couleur du pixel aux coords donn�es */
{
	Uint8 * p = ((Uint8 *)Ecran_SDL->pixels) + Y * Ecran_SDL -> pitch + X * Ecran_SDL -> format -> BytesPerPixel;
	return *p;
}

void Effacer_Tout_l_Ecran_SDL (byte Couleur)
{
	UNIMPLEMENTED
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
  SDL_UpdateRect(Ecran_SDL,0,0,Largeur,Hauteur);
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
	SDL_UpdateRect(Ecran_SDL,Debut_X,Debut_Y,Largeur,Hauteur);
}

void Block_SDL_Fast (word Debut_X,word Debut_Y,word Largeur,word Hauteur,byte Couleur)
/* On affiche un rectangle de la couleur donn�e */
{
	SDL_Rect rectangle;
	rectangle.x=Debut_X;
	rectangle.y=Debut_Y;
	rectangle.w=Largeur;
	rectangle.h=Hauteur;
	SDL_FillRect(Ecran_SDL,&rectangle,Couleur);
}

void Pixel_Preview_Normal_SDL (word X,word Y,byte Couleur)
/* Affichage d'un pixel dans l'�cran, par rapport au d�calage de l'image 
 * dans l'�cran, en mode normal (pas en mode loupe)
 * Note: si on modifie cette proc�dure, il faudra penser � faire �galement 
 * la modif dans la proc�dure Pixel_Preview_Loupe_SDL. */
{
    Pixel_SDL(X-Principal_Decalage_X,Y-Principal_Decalage_Y,Couleur);
}

void Pixel_Preview_Loupe_SDL  (word X,word Y,byte Couleur)
{
	UNTESTED
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

		Block_SDL_Fast(
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

  //On met dans ECX le nombre de pixels � traiter:
  int ecx;

  for (ecx=0;ecx<Largeur;ecx++)
	*(edi+ecx)=!*(edi+ecx);
  SDL_UpdateRect(Ecran_SDL,Pos_X,Pos_Y,Largeur,1);
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
    SDL_UpdateRect(Ecran_SDL,Pos_X,Pos_Y,1,Hauteur);
}

void Display_brush_Color_SDL  (word Pos_X,word Pos_Y,word Decalage_X,word Decalage_Y,word Largeur,word Hauteur,byte Couleur_de_transparence,word Largeur_brosse)
{
	UNIMPLEMENTED
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
    SDL_UpdateRect(Ecran_SDL,Pos_X,Pos_Y,Largeur,Hauteur);
}

void Clear_brush_SDL (word Pos_X,word Pos_Y,word Decalage_X,word Decalage_Y,word Largeur,word Hauteur,byte Couleur_de_transparence,word Largeur_image)
{
    byte* Dest=Ecran+Pos_X+Pos_Y*Largeur_ecran; //On va se mettre en 0,0 dans l'�cran (EDI)
    byte* Src=(Pos_Y+Principal_Decalage_Y)*Largeur_image+Pos_X+Principal_Decalage_X+Principal_Ecran; //Coords de d�part ds la source (ESI)
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
  SDL_UpdateRect(Ecran_SDL,Pos_X,Pos_Y,Largeur,Hauteur);
}

void Remap_screen_SDL (word Pos_X,word Pos_Y,word Largeur,word Hauteur,byte * Table_de_conversion)
{
	UNIMPLEMENTED
}

void Afficher_une_ligne_ecran_SDL (word Pos_X,word Pos_Y,word Largeur,byte * Ligne)
/* On affiche toute une ligne de pixels. Utilis� pour les textes. */
{
    memcpy(Ecran+Pos_X+Pos_Y*Largeur_ecran,Ligne,Largeur);
    SDL_UpdateRect(Ecran_SDL,Pos_X,Pos_Y,Largeur,1);
}

void Afficher_une_ligne_transparente_mono_a_l_ecran_SDL(
	word Pos_X, word Pos_Y, word Largeur, byte* Ligne, 
	byte Couleur_transparence, byte Couleur)
// Affiche une ligne � l'�cran avec une couleur + transparence.
// Utilis� par les brosses en mode zoom
{
	// Pour chaque pixel
	do
	{
		if (Couleur_transparence!=*Ligne)
			*Ligne = Couleur;
		Ligne ++; // Pixel suivant
		Largeur --;
	}
	while (Largeur!=0);

	UNTESTED
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
    while(1){
    	// On �clate la ligne
	Zoomer_une_ligne(ESI,Buffer,Loupe_Facteur,Largeur);
	// On l'affiche Facteur fois, sur des lignes cons�cutives
    	int CX = Loupe_Facteur;
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
			return;
		CX--;
	}while (CX > 0);
	ESI += Largeur_image;
    }
}

void Display_brush_Color_zoom_SDL (word Pos_X,word Pos_Y,word Decalage_X,word Decalage_Y,word Largeur,word Pos_Y_Fin,byte Couleur_de_transparence,word Largeur_brosse,byte * Buffer)
{
    UNIMPLEMENTED
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
	UNTESTED
	byte* ESI = Brosse + Decalage_Y * Largeur_brosse + Decalage_X;
	int DX=Pos_Y;

	//Pour chaque ligne � zoomer :
	while(1)
	{
		// ESI = Ligne originale
		// On �clate la ligne
		Zoomer_une_ligne(ESI,Buffer,Loupe_Facteur,Largeur);

		// On affiche la ligne Facteur fois � l'�cran (sur des
		// lignes cons�cutives)
		int BX = Loupe_Facteur;

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
				return;
			}
			BX --;
		}
		while (BX > 0);
		
		// Passage � la ligne suivante dans la brosse aussi
		ESI+=Largeur_brosse;
	}
	SDL_UpdateRect(Ecran_SDL, Pos_X, Pos_Y,
		Largeur * Loupe_Facteur, Pos_Y_Fin - Pos_Y
	);
}

void Clear_brush_zoom_SDL        (word Pos_X,word Pos_Y,word Decalage_X,word Decalage_Y,word Largeur,word Pos_Y_Fin,byte Couleur_de_transparence,word Largeur_image,byte * Buffer)
{
	UNIMPLEMENTED
}

void Set_Mode_SDL()
/* On r�gle la r�solution de l'�cran */
{
	Ecran_SDL=SDL_SetVideoMode(Largeur_ecran,Hauteur_ecran,8,SDL_HWSURFACE/*|SDL_FULLSCREEN*/);
	Ecran=Ecran_SDL->pixels;
}
