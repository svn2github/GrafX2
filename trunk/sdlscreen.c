#include <SDL/SDL.h>
#include "global.h"
#include "sdlscreen.h"

void Pixel_SDL (word X,word Y,byte Couleur)
/* Affiche un pixel de la Couleur aux coords X;Y � l'�cran */
{
    *(((Uint8 *)Ecran_SDL->pixels) + Y * Ecran_SDL->pitch + X)=Couleur;
//		SDL_UpdateRect(Ecran_SDL,X,Y,0,0);
}

static inline void Pixel_SDL_Fast(word X, word Y,byte Couleur)
/* Idem mais on ne rafraichit pas (plus rapide, mais �a ne s'affiche pas tout de suite) */
{
	*(((byte *)Ecran_SDL->pixels) + Y * Ecran_SDL->pitch +X)=Couleur;
}

byte Lit_Pixel_SDL            (word X,word Y)
/* On retourne la couleur du pixel aux coords donn�es */
{
	Uint8 * p = ((Uint8 *)Ecran_SDL->pixels) + Y * Ecran_SDL -> pitch + X * Ecran_SDL -> format -> BytesPerPixel;
	return *p;
}

void Effacer_Tout_l_Ecran_SDL (byte Couleur)
{
	puts("Effacer_Tout_l_Ecran_SDL non impl�ment�!\n");
}

void Afficher_partie_de_l_ecran_SDL       (word Largeur,word Hauteur,word Largeur_image)
{
	puts("Afficher_partie_de_l_ecran_SDL non impl�ment�!\n");
}

void Block_SDL                (word Debut_X,word Debut_Y,word Largeur,word Hauteur,byte Couleur)
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

void Pixel_Preview_Normal_SDL (word X,word Y,byte Couleur)
{
	puts("Pixel_Preview_Normal_SDL non impl�ment�!\n");
}

void Pixel_Preview_Loupe_SDL  (word X,word Y,byte Couleur)
{
	puts("Pixel_Preview_Loupe_SDL non impl�ment�!\n");
}

void Ligne_horizontale_XOR_SDL(word Pos_X,word Pos_Y,word Largeur)
{
	puts("Ligne_horizontale_XOR_SDL non impl�ment�!\n");
}

void Ligne_verticale_XOR_SDL  (word Pos_X,word Pos_Y,word Hauteur)
{
	puts("Ligne_verticale_XOR_SDL non impl�ment�!\n");
}

void Display_brush_Color_SDL  (word Pos_X,word Pos_Y,word Decalage_X,word Decalage_Y,word Largeur,word Hauteur,byte Couleur_de_transparence,word Largeur_brosse)
{
	puts("Display_brush_Color_SDL non impl�ment�!\n");
}

void Display_brush_Mono_SDL   (word Pos_X,word Pos_Y,word Decalage_X,word Decalage_Y,word Largeur,word Hauteur,byte Couleur_de_transparence,byte Couleur,word Largeur_brosse)
/* On affiche la brosse en monochrome */
{
	int i,j;
	for(i=0;i<Hauteur;i++)
	{
		for(j=0;j<Largeur;j++)
		{
			if (*(Brosse+Decalage_X+Brosse_Largeur*Decalage_Y)==Couleur_de_transparence)
				Pixel_SDL_Fast(Pos_X+j,Pos_Y+i,Couleur);
		}
	}
	SDL_UpdateRect(Ecran_SDL,Pos_X,Pos_Y,Largeur,Hauteur);
}

void Clear_brush_SDL          (word Pos_X,word Pos_Y,word Decalage_X,word Decalage_Y,word Largeur,word Hauteur,byte Couleur_de_transparence,word Largeur_image)
{
	puts("Clear_brush_SDL non impl�ment�!\n");
}

void Remap_screen_SDL         (word Pos_X,word Pos_Y,word Largeur,word Hauteur,byte * Table_de_conversion)
{
	puts("Remap_screen_SDL non impl�ment�!\n");
}

void Afficher_une_ligne_ecran_SDL     (word Pos_X,word Pos_Y,word Largeur,byte * Ligne)
/* On affiche toute une lignei de pixels. Utilis� pour les textes. */
{
	int i;
	for(i=0;i<Largeur;i++)
	{
		Pixel_SDL_Fast(Pos_X+i,Pos_Y,*(Ligne+i));
	}
	SDL_UpdateRect(Ecran_SDL,Pos_X,Pos_Y,Largeur,1);
} 
 
void Lire_une_ligne_ecran_SDL         (word Pos_X,word Pos_Y,word Largeur,byte * Ligne)
{
	puts("Lire_une_ligne_ecran_SDL non impl�ment�!\n");
}
  
void Afficher_partie_de_l_ecran_zoomee_SDL(word Largeur,word Hauteur,word Largeur_image,byte * Buffer)
{
	puts("Afficher_partie_de_l_ecran_zoomee_SDL non impl�ment�!\n");
}
  
void Display_brush_Color_zoom_SDL(word Pos_X,word Pos_Y,word Decalage_X,word Decalage_Y,word Largeur,word Pos_Y_Fin,byte Couleur_de_transparence,word Largeur_brosse,byte * Buffer)
{
	puts("Display_brush_Color_zoom_SDL non impl�ment�!\n");
}
  
void Display_brush_Mono_zoom_SDL (word Pos_X,word Pos_Y,word Decalage_X,word Decalage_Y,word Largeur,word Pos_Y_Fin,byte Couleur_de_transparence,byte Couleur,word Largeur_brosse,byte * Buffer)
{
	puts("Display_brush_Mono_zoom_SDL non impl�ment�!\n");
}
  
void Clear_brush_zoom_SDL        (word Pos_X,word Pos_Y,word Decalage_X,word Decalage_Y,word Largeur,word Pos_Y_Fin,byte Couleur_de_transparence,word Largeur_image,byte * Buffer)
{
	puts("Clear_brush_zoom_SDL non impl�ment�!\n");
}
  
void Set_Mode_SDL()
/* On r�gle lar�solution de l'�cran */
{
	Ecran_SDL=SDL_SetVideoMode(Largeur_ecran,Hauteur_ecran,8,SDL_SWSURFACE);
}
