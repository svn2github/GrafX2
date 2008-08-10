#include "op_c.h"
#include "stdio.h"

#include "graph.h"

/*************************************************************************************
 * Diffusion d'erreur avec l'algorithme de Floyd-Steinberg pour conversion 24b > 256c
 * Pour chaque pixel de la source :
 *  1) On fait une recherche dans la palette pour trouver la couleurs plus proche
 *  2) On calcule la diff�rence avec la vraie couleur
 *  3) On r�partit cette diff�rence entre les pixels autours avec les coefs:
 *  	- 7/16 pour le pixel de droite
 *  	- 3/16 pour le pixel en bas � gauche
 *  	- 5/16 pour le pixel en dessous
 *  	- 1/16 pour le pixel en bas � droite
 *  Cette diff�rence est appliqu�e directement � la source, et sera prise en compte
 *  lors du traitement des pixels suivants (on ne touche pas aux pixels qui sont au
 *  dessus ou � gauche de celui en cours !)
 ************************************************************************************/

void OPASM_DitherFS_6123(
  Bitmap256            Destination,  // Pointeur sur le 1er pixel de la ligne
  Bitmap24B            Source,       // Idem mais sur la source
  int                  Largeur,      // Largeur � traiter, =(largeur_image-2), (>0)
  struct Composantes * Palette,      // Palette de l'image destination
  byte *               TableC,       // Table de conversion 24b->8b
  byte                 ReducR,       // 8-Nb_bits_rouges
  byte                 ReducV,       // 8-Nb_bits_verts
  byte                 ReducB,       // 8-Nb_bits_bleus
  byte                 NbbV,         // Nb_bits_verts
  byte                 NbbB)         // Nb_bits_bleus
{
	byte DSFRouge,DSFVert,DSFBleu=0,DSFRougeAD,DSFVertAD,DSFBleuAD;
	int VarA;
	Bitmap24B PixelCourant, Cible;

	// Pour chaque pixel sur la largeur
	for(VarA=0;VarA<Largeur;VarA++)
	{
		PixelCourant = Source + VarA;
		// On regarde la meilleure couleur d'apr�s la table de conversion 24b>8b
		DSFRougeAD = PixelCourant->R - ReducR;
		DSFVertAD = PixelCourant->V - ReducV;
		DSFBleuAD = PixelCourant->B - ReducB;

		*Destination = *(TableC  + DSFRougeAD*256*256 + DSFVertAD*256 + DSFBleu);

		// On calcule l'erreur
		DSFRouge = PixelCourant->R - Palette[*Destination].R;
		DSFVert = PixelCourant->V - Palette[*Destination].V;
		DSFBleu = PixelCourant->B - Palette[*Destination].B;

		// On diffuse l'erreur sur le pixel de droite (6)
		DSFRougeAD = (DSFRouge*7)/16;
		DSFVertAD = (DSFVert*7)/16;
		DSFBleuAD = (DSFBleu*7)/16;

		// Diffusion de l'erreur dans la source
		Cible = PixelCourant + 1; // "Pixel � droite"
		Cible->R = Max(Cible->R + DSFRougeAD,255);
		Cible->V = Max(Cible->V + DSFVertAD,255);
		Cible->B = Max(Cible->B + DSFBleuAD,255);

		// On diffuse sur le pixel en bas � gauche (1)
		DSFRougeAD = (DSFRouge*3)/16;
		DSFVertAD = (DSFVert*3)/16;
		DSFBleuAD = (DSFBleu*3)/16;

		Cible = PixelCourant + Largeur; // "Pixel en bas � gauche"
		Cible->R = Max(Cible->R + DSFRougeAD,255);
		Cible->V = Max(Cible->V + DSFVertAD,255);
		Cible->B = Max(Cible->B + DSFBleuAD,255);

		// On diffuse sur le pixel en dessous (2)
		DSFRougeAD = (DSFRouge*5)/16;
		DSFVertAD = (DSFVert*5)/16;
		DSFBleuAD = (DSFBleu*5)/16;

		Cible ++; // "Pixel en dessous"
		Cible->R = Max(Cible->R + DSFRougeAD,255);
		Cible->V = Max(Cible->V + DSFVertAD,255);
		Cible->B = Max(Cible->B + DSFBleuAD,255);

		// On diffuse sur le pixel en dessous (2)
		DSFRougeAD = (DSFRouge*1)/16;
		DSFVertAD = (DSFVert*1)/16;
		DSFBleuAD = (DSFBleu*1)/16;

		Cible ++; // "Pixel en bas � droite"
		Cible->R = Max(Cible->R + DSFRougeAD,255);
		Cible->V = Max(Cible->V + DSFVertAD,255);
		Cible->B = Max(Cible->B + DSFBleuAD,255);

		Destination++;
	}

}
  
void OPASM_DitherFS_623(
  Bitmap256            Destination,  // Pointeur sur le 1er pixel de la ligne
  Bitmap24B            Source,       // Idem mais sur la source
  int                  Largeur,      // Largeur � traiter, =(largeur_image-2), (>0)
  struct Composantes * Palette,      // Palette de l'image destination
  byte *               TableC,       // Table de conversion 24b->8b
  byte                 ReducR,       // 8-Nb_bits_rouges
  byte                 ReducV,       // 8-Nb_bits_verts
  byte                 ReducB,       // 8-Nb_bits_bleus
  byte                 NbbV,         // Nb_bits_verts
  byte                 NbbB)         // Nb_bits_bleus
{
	puts("OPASM_DitherFS_623 non impl�ment�!");
}

void OPASM_DitherFS_12(
  Bitmap256            Destination,  // Pointeur sur le 1er pixel de la ligne
  Bitmap24B            Source,       // Idem mais sur la source
  int                  Largeur,      // Largeur � traiter, =(largeur_image-2), (>0)
  struct Composantes * Palette,      // Palette de l'image destination
  byte *               TableC,       // Table de conversion 24b->8b
  byte                 ReducR,       // 8-Nb_bits_rouges
  byte                 ReducV,       // 8-Nb_bits_verts
  byte                 ReducB,       // 8-Nb_bits_bleus
  byte                 NbbV,         // Nb_bits_verts
  byte                 NbbB)         // Nb_bits_bleus
{
	puts("OPASM_DitherFS_12 non impl�ment�!");
}

void OPASM_DitherFS_6(
  Bitmap256            Destination,  // Pointeur sur le 1er pixel de la ligne
  Bitmap24B            Source,       // Idem mais sur la source
  int                  Largeur,      // Largeur � traiter, =(largeur_image-1), (>0)
  struct Composantes * Palette,      // Palette de l'image destination
  byte *               TableC,       // Table de conversion 24b->8b
  byte                 ReducR,       // 8-Nb_bits_rouges
  byte                 ReducV,       // 8-Nb_bits_verts
  byte                 ReducB,       // 8-Nb_bits_bleus
  byte                 NbbV,         // Nb_bits_verts
  byte                 NbbB)         // Nb_bits_bleus
{
	puts("OPASM_DitherFS_6 non impl�ment�!");
}

  void OPASM_DitherFS(
  Bitmap256 Destination,  // Pointeur sur le pixel
  Bitmap24B Source,       // Idem mais sur la source
  byte *    TableC,       // Table de conversion 24b->8b
  byte      ReducR,       // 8-Nb_bits_rouges
  byte      ReducV,       // 8-Nb_bits_verts
  byte      ReducB,       // 8-Nb_bits_bleus
  byte      NbbV,         // Nb_bits_verts
  byte      NbbB)         // Nb_bits_bleus
{
	puts("OPASM_DitherFS non impl�ment�!");
}

void OPASM_DitherFS_2(
  Bitmap256            Destination,  // Pointeur sur le 1er pixel de la colonne
  Bitmap24B            Source,       // Idem mais sur la source
  int                  Hauteur,      // Hauteur � traiter, =(hauteur_image-1), (>0)
  struct Composantes * Palette,      // Palette de l'image destination
  byte *               TableC,       // Table de conversion 24b->8b
  byte                 ReducR,       // 8-Nb_bits_rouges
  byte                 ReducV,       // 8-Nb_bits_verts
  byte                 ReducB,       // 8-Nb_bits_bleus
  byte                 NbbV,         // Nb_bits_verts
  byte                 NbbB)         // Nb_bits_bleus
{
	puts("OPASM_DitherFS_2 non impl�ment�!");
}

  void OPASM_Split_cluster_Rouge(
  int * tableO, // Table d'occurences
  int   rmin,   // rmin << rdec
  int   vmin,   // vmin << vdec
  int   bmin,   // bmin << bdec
  int   rmax,   // rmax << rdec
  int   vmax,   // vmin << vdec
  int   bmax,   // bmin << bdec
  int   rinc,   // Incr�mentation sur les rouges 1 << rdec
  int   vinc,   // Incr�mentation sur les verts  1 << vdec
  int   binc,   // Incr�mentation sur les bleus  1 << bdec
  int   limite, // Nombre d'occurences minimales
  int   rdec,   // rdec
  int * rouge)  // Valeur du rouge atteignant la limite
{
	puts("OPASM_Split_cluster_Rouge non impl�ment�!");
}

void OPASM_Split_cluster_Vert(
  int * tableO, // Table d'occurences
  int   rmin,   // rmin << rdec
  int   vmin,   // vmin << vdec
  int   bmin,   // bmin << bdec
  int   rmax,   // rmax << rdec
  int   vmax,   // vmin << vdec
  int   bmax,   // bmin << bdec
  int   rinc,   // Incr�mentation sur les rouges 1 << rdec
  int   vinc,   // Incr�mentation sur les verts  1 << vdec
  int   binc,   // Incr�mentation sur les bleus  1 << bdec
  int   limite, // Nombre d'occurences minimales
  int   vdec,   // vdec
  int * vert)   // Valeur du vert atteignant la limite
{
	puts("OPASM_Split_cluster_Vert non impl�ment�!");
}

void OPASM_Split_cluster_Bleu(
  int * tableO, // Table d'occurences
  int   rmin,   // rmin << rdec
  int   vmin,   // vmin << vdec
  int   bmin,   // bmin << bdec
  int   rmax,   // rmax << rdec
  int   vmax,   // vmin << vdec
  int   bmax,   // bmin << bdec
  int   rinc,   // Incr�mentation sur les rouges 1 << rdec
  int   vinc,   // Incr�mentation sur les verts  1 << vdec
  int   binc,   // Incr�mentation sur les bleus  1 << bdec
  int   limite, // Nombre d'occurences minimales
  int   bdec,   // bdec
  int * bleu)   // Valeur du bleu atteignant la limite
{
	puts("OPASM_Split_cluster_Bleu non impl�ment�!");
}

  void OPASM_Compter_occurences(
  int *     Destination,  // Pointeur sur la table d'occurences
  Bitmap24B Source,       // Pointeur sur l'image
  int       Taille,       // Nombre de pixels dans l'image
  byte      ReducR,       // 8-Nb_bits_rouges
  byte      ReducV,       // 8-Nb_bits_verts
  byte      ReducB,       // 8-Nb_bits_bleus
  byte      NbbV,         // Nb_bits_verts
  byte      NbbB)         // Nb_bits_bleus
{
	puts("OPASM_Compter_occurences non impl�ment�!");
}

void OPASM_Analyser_cluster(
  int * TableO, // Table d'occurences
  int * rmin,   // rmin << rdec
  int * vmin,   // vmin << vdec
  int * bmin,   // bmin << bdec
  int * rmax,   // rmax << rdec
  int * vmax,   // vmax << vdec
  int * bmax,   // bmax << bdec
  int   rdec,   // rdec
  int   vdec,   // vdec
  int   bdec,   // bdec
  int   rinc,   // Incr�mentation sur les rouges 1 << rdec
  int   vinc,   // Incr�mentation sur les verts  1 << vdec
  int   binc,   // Incr�mentation sur les bleus  1 << bdec
  int * Nbocc)  // Nombre d'occurences
{
	puts("OPASM_Analyser_cluster non impl�ment�!");
}
