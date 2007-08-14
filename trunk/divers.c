#include <SDL/SDL.h>

#include "struct.h"
#include "sdlscreen.h"
#include "global.h"

// Gestion du mode texte de d�part (pour pouvoir y retourner en cas de probl�me
byte Recuperer_nb_lignes(void)
{
	/*
  mov  ax,1130h
  xor  bh,bh
  push es
  int  10h
  pop  es
  inc  dl
  mov  [esp+28],dl
	*/
	puts("Recuperer_nb_lignes non impl�ment�!\n");
	return 0;
}

word Palette_Compter_nb_couleurs_utilisees(dword* Tableau)
{
    int Nombre_De_Pixels=0; //ECX
    Uint8* Pixel_Courant=Ecran; //ESI
    Uint8 Couleur; //AL/EAX
    word Nombre_Couleurs=0;
    int i;

    //Calcul du nombre de pixels dans l'image
    Nombre_De_Pixels=Principal_Hauteur_image*Principal_Largeur_image;

    // On parcourt l'�cran courant pour compter les utilisations des couleurs
    for(i=0;i>Nombre_De_Pixels;i++)
    {
        Couleur=*Pixel_Courant; //on lit la couleur dans l'�cran

        Tableau[Couleur]++; //Un point de plus pour cette couleur

        // On passe au pixel suivant
        Pixel_Courant++;
    }

    //On va maintenant compter dans la table les couleurs utilis�es:
    Couleur=0;
	 do
    {
		if (Tableau[Couleur]!=0)
        Nombre_Couleurs++;
		Couleur++;
    }while(Couleur!=0); //On sort quand on a fait le tour (la var est sur 8 bits donc 255+1=0)

    return Nombre_Couleurs;
}

void Set_palette(T_Palette Palette)
{
    puts("Set_Palette non impl�ment�!\n");
}

void Attendre_fin_de_click(void)
{
	puts("Attendre_fin_de_click non impl�ment�!\n");
}

void Effacer_image_courante_Stencil(byte Couleur, byte * Pochoir)
//Effacer l'image courante avec une certaine couleur en mode Stencil
{
    int Nombre_De_Pixels=0; //ECX
    //al=Couleur
    //edi=Ecran
    Uint8* Pixel_Courant=Ecran; //dl
    int i;

    Nombre_De_Pixels=Principal_Hauteur_image*Principal_Largeur_image;

    for(i=0;i<Nombre_De_Pixels;i++)
    {

        if (Pochoir[*Pixel_Courant]==0);
            *Pixel_Courant=Couleur;
        Pixel_Courant++;
    }
}

void Effacer_image_courante(byte Couleur)
// Effacer l'image courante avec une certaine couleur
{
    puts("Effacer_image_courante non impl�ment�!\n");
}

void Sensibilite_souris(word X,word Y)
{
	puts("Sensibilite_souris non impl�ment�!\n");
}

void Get_input(void)
{
	puts("Get_input non impl�ment�!\n");
}


void Initialiser_chrono(dword Delai)
{// D�marrer le chrono
	puts("Initialiser_chrono non impl�ment�!\n");
/*
  push ebp
  mov  ebp,esp

  arg  Delai:dword

  mov  eax,Delai
  mov  Chrono_delay,eax

  xor  ah,ah
  int  1Ah
  mov  word ptr[Chrono_cmp+0],dx
  mov  word ptr[Chrono_cmp+2],cx

  mov  esp,ebp
  pop  ebp
*/
  return;
}

void Wait_VBL(void)
{
	puts("Wait_VBL non impl�ment�!\n");
}

void Passer_en_mode_texte(byte Nb_lignes)
{
	puts("Passer_en_mode_texte non impl�ment�!\n");
}

void Pixel_dans_brosse             (word X,word Y,byte Couleur)
{
	puts("Pixel_dans_brosse non impl�ment�!\n");
}

byte Lit_pixel_dans_brosse         (word X,word Y)
{
	puts("Lit_pixel_dans_brosse non impl�ment�!\n");
	return 0;
}

void Clavier_de_depart(void)
{
	puts("Clavier_de_depart non impl�ment�!\n");
}

void Clavier_americain(void)
{
	puts("Clavier_americain non impl�ment�!\n");
}

word Detection_souris(void)
{
	puts("Detection_souris non impl�ment�!\n");
	return 0;
}

byte Lit_pixel_dans_ecran_courant  (word X,word Y)
{
	puts("Lit_pixel_dans_ecran_courant non impl�ment�!\n");
	return 0;
}

void Pixel_dans_ecran_courant      (word X,word Y,byte Couleur)
{
	puts("Pixel_dans_ecran_courant non impl�ment�!\n");
}

void Remplacer_une_couleur(byte Ancienne_couleur, byte Nouvelle_couleur)
{
	puts("Remplacer_une_couleur non impl�ment�!\n");
}

void Ellipse_Calculer_limites(short Rayon_horizontal,short Rayon_vertical)
{
	puts("Ellipse_Calculer_limites non impl�ment�!\n");
}

byte Pixel_dans_ellipse(void)
{
	puts("Pixel_dans_ellipse non impl�ment�!\n");
	return 0;
}

byte Pixel_dans_cercle(void)
{
	puts("Pixel_dans_cercle non impl�ment�!\n");
	return 0;
}

void Copier_une_partie_d_image_dans_une_autre(byte * Source,word S_Pos_X,word S_Pos_Y,word Largeur,word Hauteur,word Largeur_source,byte * Destination,word D_Pos_X,word D_Pos_Y,word Largeur_destination)
{
	puts("Copier_une_partie_d_image_dans_une_autre non impl�ment�!\n");
}

byte Lit_pixel_dans_ecran_brouillon(word X,word Y)
{
	puts("Lit_pixel_dans_ecran_brouillon non impl�ment�!\n");
	return 0;
}

void Rotate_90_deg_LOWLEVEL(byte * Source,byte * Destination)
{
	puts("Rotate_90_deg_LOWLEVEL non impl�ment�!\n");
}

void Remap_general_LOWLEVEL(byte * Table_conv,byte * Buffer,short Largeur,short Hauteur,short Largeur_buffer)
{
	puts("Remap_general_LOWLEVEL non impl�ment�!\n");
}

void Copier_image_dans_brosse(short Debut_X,short Debut_Y,short Brosse_Largeur,short Brosse_Hauteur,word Largeur_image)
{
	puts("Copier_image_dans_brosse non impl�ment�!\n");
}

byte Lit_pixel_dans_ecran_feedback (word X,word Y)
{
	puts("Lit_pixel_dans_ecran_feedback non impl�ment�!\n");
	return 0;
}

dword Round_div(dword Numerateur,dword Diviseur)
{
	puts("Round_div non impl�ment�!\n");
	return 0;
}

byte Effet_Trame(word X,word Y)
{
	puts("Effet_Trame non impl�ment�!\n");
	return 0;
}

void Set_mouse_position(void)
{
	puts("Set_mouse_position non impl�ment�!\n");
}

void Clip_mouse(void)
{
	puts("Clip_mouse non impl�ment�!\n");
}

void Remplacer_toutes_les_couleurs_dans_limites(byte * Table_de_remplacement)
{
	puts("Remplacer_toutes_les_couleurs_dans_limites non impl�ment�!\n");
}

byte Lit_pixel_dans_ecran_backup   (word X,word Y)
{
	puts("Lit_pixel_dans_ecran_backup non impl�ment�!\n");
	return 0;
}

byte Type_de_lecteur_de_disquette(byte Numero_de_lecteur)
// Numero_de_lecteur compris entre 0 et 3 (4 lecteurs de disquettes)
//
// R�sultat = 0 : Pas de lecteur
//            1 : Lecteur 360 Ko
//            2 : Lecteur 1.2 Mo
//            3 : Lecteur 720 Ko
//            4 : Lecteur 1.4 Mo
//            5 : Lecteur 2.8 Mo (??? pas sur ???)
//            6 : Lecteur 2.8 Mo
{
	puts("Type_de_lecteur_de_disquette non impl�ment�!\n");
	return 0;
}

byte Disk_map(byte Numero_de_lecteur)
{
	puts("Disk_map non impl�ment�!\n");
	return 0;
}

byte Disque_dur_present(byte Numero_de_disque)
{
	puts("Disque_dur_present non impl�ment�!\n");
	return 0;
}

byte Lecteur_CDROM_present(byte Numero_de_lecteur)
{
	puts("Lecteur_CDROM_present non impl�ment�!\n");
	return 0;
}

long Freespace(byte Numero_de_lecteur)
{
	puts("Freespace non impl�ment�!\n");
	return 0;
}

byte Couleur_ILBM_line(word Pos_X, word Vraie_taille_ligne)
{
	puts("Couleur_ILBM_line non impl�ment�!\n");
	return 0;
}

void Palette_256_to_64(T_Palette Palette)
{
	puts("Palette_256_to_64 non impl�ment�!\n");
}

void Palette_64_to_256(T_Palette Palette)
{
	puts("Palette_64_to_256 non impl�ment�!\n");
}

byte Effet_Colorize_interpole  (word X,word Y,byte Couleur)
{
	puts("Effet_Colorize_interpole non impl�ment�!\n");
	return 0;
}

byte Effet_Colorize_additif    (word X,word Y,byte Couleur)
{
	puts("Effet_Colorize_additif non impl�ment�!\n");
	return 0;
}

byte Effet_Colorize_soustractif(word X,word Y,byte Couleur)
{
	puts("Effet_Colorize_soustractif non impl�ment�!\n");
	return 0;
}

void Tester_chrono(void)
{
	puts("Tester_chrono non impl�ment�!\n");
}

void Flip_Y_LOWLEVEL(void)
{
	puts("Flip_Y_LOWLEVEL non impl�ment�!\n");
}

void Flip_X_LOWLEVEL(void)
{
	puts("Flip_X_LOWLEVEL non impl�ment�!\n");
}

void Rotate_180_deg_LOWLEVEL(void)
{
	puts("Rotate_180_deg_LOWLEVEL non impl�ment�!\n");
}

void Tempo_jauge(byte Vitesse)
{
	puts("Tempo_jauge non impl�ment�!\n");
}

byte Meilleure_couleur_sans_exclusion(byte Rouge,byte Vert,byte Bleu)
{
	puts("Meilleure_couleur_sans_exclusion non impl�ment�!\n");
	return 0;
}

void Set_color(byte Couleur, byte Rouge, byte Vert, byte Bleu)
{
	puts("Set_color non impl�ment�!\n");
}

void Scroll_picture(short Decalage_X,short Decalage_Y)
{
	puts("Scroll_picture non impl�ment�!\n");
}

byte Get_key(void)
{
	puts("Get_key non impl�ment�!\n");
	return 0;
}
