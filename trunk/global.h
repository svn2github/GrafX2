/*  Grafx2 - The Ultimate 256-color bitmap paint program

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

// Dans MAIN on declare les variables
// Dans les autres fichiers, on ne fait que les r�f�rencer
#ifdef VARIABLES_GLOBALES
  #define GFX2_GLOBAL
#else
  #define GFX2_GLOBAL extern
#endif

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//        Ce fichier contient les d�claration des variables globales        //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


// -- Section des variables de CONFIGURATION ---------------------------------

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <SDL.h>
#include "struct.h"

GFX2_GLOBAL T_Config Config;

  // Tableau des touches sp�ciales
GFX2_GLOBAL word Config_Touche[NB_TOUCHES_SPECIALES][2];


struct S_Mode_video
{
  short  Largeur;
  short  Hauteur;
  byte   Mode;
  word   Fullscreen;
  byte   Etat; // 0:Cool 1:OK ; 2:Bof ; 3:Naze ; si on rajoute +128 => incompatible
};
GFX2_GLOBAL struct S_Mode_video Mode_video[MAX_MODES_VIDEO];
GFX2_GLOBAL int  Nb_modes_video; // Nombre de modes r�ellement recens�s dans Mode_video[]


  // Palette par d�faut

GFX2_GLOBAL T_Palette Palette_defaut;

  // Couleurs du menu

GFX2_GLOBAL byte CM_Noir;
GFX2_GLOBAL byte CM_Fonce;
GFX2_GLOBAL byte CM_Clair;
GFX2_GLOBAL byte CM_Blanc;
GFX2_GLOBAL byte CM_Trans;
GFX2_GLOBAL Composantes Coul_menu_pref[4];

  // Etat des entr�es

GFX2_GLOBAL word Mouse_X; // Abscisse de la souris
GFX2_GLOBAL word Mouse_Y; // Ordonn�e de la souris
GFX2_GLOBAL byte Mouse_K; // Etat des boutons de la souris (tient comte des boutons appuy�s simultan�ments

#define Mouse_K_Unique (Mouse_K==0?0:(Mouse_K&1?1:(Mouse_K&2?2:0))) // Etat des boutons de la souris (un seul bouton � la fois, on regarde d'abord le 1, puis le 2, ...)

GFX2_GLOBAL dword Touche; // Touche tap�e
GFX2_GLOBAL dword Touche_ANSI; // Caract�re tap�
GFX2_GLOBAL Uint8* Etat_Du_Clavier;  // Scancode de la touche en cours et etat des touches de ctrl
// Modificateurs pour Touche
// (Name conflict with windows.h)
#ifdef MOD_SHIFT
  #undef MOD_SHIFT
#endif
#ifdef MOD_CTRL
  #undef MOD_CTRL
#endif
#ifdef MOD_ALT
  #undef MOD_ALT
#endif
#define MOD_SHIFT 0x1000
#define MOD_CTRL  0x2000
#define MOD_ALT   0x4000

GFX2_GLOBAL byte Quit_demande; // !=0 lorsque l'utilisateur demande la fermeture de fen�tre.

GFX2_GLOBAL byte Mouse_Facteur_de_correction_X;
GFX2_GLOBAL byte Mouse_Facteur_de_correction_Y;

GFX2_GLOBAL byte Autoriser_changement_de_couleur_pendant_operation;

  // Donn�es sur le curseur

GFX2_GLOBAL byte Forme_curseur;
GFX2_GLOBAL byte Forme_curseur_avant_fenetre; // Forme du curseur avant l'ouverture d'une fen�tre
GFX2_GLOBAL byte Cacher_curseur;
GFX2_GLOBAL byte Curseur_dans_menu;           // Bool�en "Le curseur se trouve dans le menu"
GFX2_GLOBAL byte Curseur_dans_menu_precedent; // Bool�en "Le curseur se trouvait pr�c�demment dans le menu"
GFX2_GLOBAL word Curseur_Decalage_X[NB_SPRITES_CURSEUR]; // Coordonn�es X du point sensible de curseurs en sprite
GFX2_GLOBAL word Curseur_Decalage_Y[NB_SPRITES_CURSEUR]; // Coordonn�es Y du point sensible de curseurs en sprite
GFX2_GLOBAL byte SPRITE_CURSEUR[NB_SPRITES_CURSEUR][HAUTEUR_SPRITE_CURSEUR][LARGEUR_SPRITE_CURSEUR]; // Dessins des sprites de curseur
GFX2_GLOBAL byte FOND_CURSEUR[HAUTEUR_SPRITE_CURSEUR][LARGEUR_SPRITE_CURSEUR]; // Contenu du dessous du curseur

  // Donn�es sur le pinceau

GFX2_GLOBAL byte  Pinceau_Forme;
GFX2_GLOBAL byte  Pinceau_Forme_avant_fill;
GFX2_GLOBAL byte  Pinceau_Forme_avant_pipette;
GFX2_GLOBAL byte  Pinceau_Forme_avant_lasso;
GFX2_GLOBAL byte  Cacher_pinceau;
GFX2_GLOBAL short Pinceau_X;
GFX2_GLOBAL short Pinceau_Y;
GFX2_GLOBAL byte  SPRITE_PINCEAU [NB_SPRITES_PINCEAU][HAUTEUR_PINCEAU][LARGEUR_PINCEAU];
GFX2_GLOBAL word  Pinceau_predefini_Largeur[NB_SPRITES_PINCEAU];
GFX2_GLOBAL word  Pinceau_predefini_Hauteur[NB_SPRITES_PINCEAU];
GFX2_GLOBAL byte  Pinceau_Type[NB_SPRITES_PINCEAU];
GFX2_GLOBAL word  Pinceau_predefini_Decalage_X[NB_SPRITES_PINCEAU];
GFX2_GLOBAL word  Pinceau_predefini_Decalage_Y[NB_SPRITES_PINCEAU];
GFX2_GLOBAL byte * Pinceau_Sprite;
GFX2_GLOBAL short Pinceau_Largeur;
GFX2_GLOBAL short Pinceau_Hauteur;
GFX2_GLOBAL short Pinceau_Decalage_X;
GFX2_GLOBAL short Pinceau_Decalage_Y;

  // Commandes graphiques

GFX2_GLOBAL int Pixel_ratio;
GFX2_GLOBAL int Pixel_width;
GFX2_GLOBAL int Pixel_height;
GFX2_GLOBAL fonction_afficheur Pixel;          // Affiche un point � l'�cran
GFX2_GLOBAL fonction_afficheur Pixel_dans_menu;// Affiche un point dans le menu (ou pas si le menu est invisible)
GFX2_GLOBAL fonction_lecteur   Lit_pixel;      // Teste la couleur d'un pixel dans l'�cran
GFX2_GLOBAL fonction_display   Display_screen; // Affiche rapidement tout l'�cran (en faisant attention de ne pas effacer le menu)
GFX2_GLOBAL fonction_block     Block;          // Affiche rapidement un bloc � l'�cran
GFX2_GLOBAL fonction_afficheur Pixel_Preview_Normal; // Affiche un point de l'image � l'�cran en mode normal (pas en mode loupe)
GFX2_GLOBAL fonction_afficheur Pixel_Preview_Loupe;  // Affiche un point de l'image � l'�cran en mode loupe
GFX2_GLOBAL fonction_afficheur Pixel_Preview;        // Affiche un point de l'image � l'�cran en fonction de l'�tat du mode loupe
GFX2_GLOBAL fonction_Ligne_XOR Ligne_horizontale_XOR;// Affiche une ligne horizontale en XOR (pour placer la loupe)
GFX2_GLOBAL fonction_Ligne_XOR Ligne_verticale_XOR;  // Affiche une ligne verticale en XOR (pour placer la loupe)
GFX2_GLOBAL fonction_display_brush_Color Display_brush_Color; // Affiche une partie de la brosse en couleur
GFX2_GLOBAL fonction_display_brush_Mono  Display_brush_Mono;  // Affiche une partie de la brosse en monochrome
GFX2_GLOBAL fonction_display_brush_Color Clear_brush;         // Efface la partie de la brosse affich�e � l'�cran
GFX2_GLOBAL fonction_remap     Remap_screen;   // Remappe une partie de l'�cran avec les nouvelles couleurs du menu
GFX2_GLOBAL fonction_procsline Afficher_ligne;  // Afficher une ligne
GFX2_GLOBAL fonction_procsline Afficher_ligne_fast;  // Afficher une ligne talle quelle (sans la doubler en mode wide)
GFX2_GLOBAL fonction_procsline Lire_ligne;      // Afficher ou lire une ligne
GFX2_GLOBAL fonction_display_zoom Display_zoomed_screen; // Affiche rapidement toute la partie zoom�e � l'�cran (en faisant attention de ne pas effacer le menu)
GFX2_GLOBAL fonction_display_brush_Color_zoom Display_brush_Color_zoom;
GFX2_GLOBAL fonction_display_brush_Mono_zoom  Display_brush_Mono_zoom;
GFX2_GLOBAL fonction_display_brush_Color_zoom Clear_brush_zoom;
GFX2_GLOBAL fonction_affiche_brosse Affiche_brosse;
  // Donn�es sur les dimensions de l'�cran

GFX2_GLOBAL int   Resize_Largeur;      // \__ Positionn�es lorsque l'utilisateur tire
GFX2_GLOBAL int   Resize_Hauteur;      // /      un bord de la fen�tre.
GFX2_GLOBAL int   Resolution_actuelle; // R�solution graphique courante
GFX2_GLOBAL short Ecran_original_X;    // |_ Dimensions de l'�cran d'origine de
GFX2_GLOBAL short Ecran_original_Y;    // |  l'image qui vient d'�tre charg�e.
GFX2_GLOBAL short Largeur_ecran;       // Largeur de l'�cran
GFX2_GLOBAL short Hauteur_ecran;       // Hauteur de l'�cran
GFX2_GLOBAL short Limite_Haut;         // |
GFX2_GLOBAL short Limite_Bas;          // |_ Limites dans lesquelles
GFX2_GLOBAL short Limite_Gauche;       // |  on peut �crire
GFX2_GLOBAL short Limite_Droite;       // |
GFX2_GLOBAL short Limite_visible_Bas;    // |_ Derniers points visibles
GFX2_GLOBAL short Limite_visible_Droite; // |  "� l'image"

GFX2_GLOBAL short Limite_Haut_Zoom;         // |
GFX2_GLOBAL short Limite_Bas_Zoom;          // |_ Limites dans lesquelles on peut
GFX2_GLOBAL short Limite_Gauche_Zoom;       // |  �crire dans la partie zoom�e
GFX2_GLOBAL short Limite_Droite_Zoom;       // |
GFX2_GLOBAL short Limite_visible_Bas_Zoom;    // |_ Derniers points visibles "�
GFX2_GLOBAL short Limite_visible_Droite_Zoom; // |  l'image" dans la partie zoom�e

GFX2_GLOBAL byte * Buffer_de_ligne_horizontale; // Buffer d'affichage de lignes

  // Donn�es sur l'image actuelle:

GFX2_GLOBAL byte *    Principal_Ecran;   // Ecran virtuel courant
GFX2_GLOBAL T_Palette Principal_Palette; // Palette de l'�cran en cours

GFX2_GLOBAL byte  Principal_Image_modifiee; // L'image courante a �t� modifi�e
GFX2_GLOBAL short Principal_Largeur_image;  // Largeur de l'image dans laquelle l'utilisateur d�sire travailler
GFX2_GLOBAL short Principal_Hauteur_image;  // Hauteur de l'image dans laquelle l'utilisateur d�sire travailler
GFX2_GLOBAL short Principal_Decalage_X;     // D�calage en X de l'�cran par rapport au d�but de l'image
GFX2_GLOBAL short Principal_Decalage_Y;     // D�calage en Y de l'�cran par rapport au d�but de l'image
GFX2_GLOBAL short Ancien_Principal_Decalage_X;
GFX2_GLOBAL short Ancien_Principal_Decalage_Y;

GFX2_GLOBAL char  Principal_Repertoire_fichier[1024]; // |_ Nom complet =
GFX2_GLOBAL char  Principal_Nom_fichier[256];         // |  Repertoire_fichier+"\"+Nom_fichier
GFX2_GLOBAL byte  Principal_Format_fichier;          // Format auquel il faut lire et �crire le fichier
GFX2_GLOBAL byte  Principal_Format;               // Format du fileselect
GFX2_GLOBAL short Principal_File_list_Position; // D�but de la partie affich�e dans la liste de fichiers
GFX2_GLOBAL short Principal_File_list_Decalage; // D�calage de la barre de s�lection dans le fileselector
GFX2_GLOBAL char  Principal_Repertoire_courant[1024]; // R�pertoire actuel sur disque
GFX2_GLOBAL char  Principal_Commentaire[TAILLE_COMMENTAIRE+1]; // Commentaire de l'image

GFX2_GLOBAL short Principal_Split; // Position en X du bord gauche du split de la loupe
GFX2_GLOBAL short Principal_X_Zoom; // (Menu_Facteur_X) + Position en X du bord droit du split de la loupe
GFX2_GLOBAL float Principal_Proportion_split; // Proportion de la zone non-zoom�e par rapport � l'�cran

  // Donn�es sur le brouillon:

GFX2_GLOBAL byte *    Brouillon_Ecran;   // Ecran virtuel brouillon
GFX2_GLOBAL T_Palette Brouillon_Palette; // Palette de l'�cran de brouillon

GFX2_GLOBAL byte  Brouillon_Image_modifiee; // Le brouillon a �t� modifi�
GFX2_GLOBAL short Brouillon_Largeur_image;  // Largeur du brouillon dans laquelle l'utilisateur d�sire travailler
GFX2_GLOBAL short Brouillon_Hauteur_image;  // Hauteur du brouillon dans laquelle l'utilisateur d�sire travailler
GFX2_GLOBAL short Brouillon_Decalage_X;     // D�calage en X du brouillon par rapport au d�but de l'image
GFX2_GLOBAL short Brouillon_Decalage_Y;     // D�calage en Y du brouillon par rapport au d�but de l'image
GFX2_GLOBAL short Ancien_Brouillon_Decalage_X;
GFX2_GLOBAL short Ancien_Brouillon_Decalage_Y;

GFX2_GLOBAL char  Brouillon_Repertoire_fichier[TAILLE_CHEMIN_FICHIER]; // |_ Nom complet =
GFX2_GLOBAL char  Brouillon_Nom_fichier[TAILLE_CHEMIN_FICHIER];        // |  Repertoire_fichier+"\"+Nom_fichier
GFX2_GLOBAL byte  Brouillon_Format_fichier;          // Format auquel il faut lire et �crire le fichier
GFX2_GLOBAL byte  Brouillon_Format;               // Format du fileselect
GFX2_GLOBAL short Brouillon_File_list_Position; // D�but de la partie affich�e dans la liste de fichiers
GFX2_GLOBAL short Brouillon_File_list_Decalage; // D�calage de la barre de s�lection dans le fileselector
GFX2_GLOBAL char  Brouillon_Repertoire_courant[TAILLE_CHEMIN_FICHIER]; // R�pertoire actuel sur disque
GFX2_GLOBAL char  Brouillon_Commentaire[TAILLE_COMMENTAIRE+1]; // Commentaire de l'image

GFX2_GLOBAL short Brouillon_Split; // Position en X du bord gauche du split de la loupe
GFX2_GLOBAL short Brouillon_X_Zoom; // (Menu_Facteur_X) + Position en X du bord droit du split de la loupe
GFX2_GLOBAL float Brouillon_Proportion_split; // Proportion de la zone non-zoom�e par rapport � l'�cran

GFX2_GLOBAL byte  Brouillon_Loupe_Mode;      // On est en mode loupe dans le brouillon
GFX2_GLOBAL word  Brouillon_Loupe_Facteur;   // Facteur de zoom dans le brouillon
GFX2_GLOBAL word  Brouillon_Loupe_Hauteur;   // Largeur de la fen�tre de zoom dans le brouillon
GFX2_GLOBAL word  Brouillon_Loupe_Largeur;   // Hauteur de la fen�tre de zoom dans le brouillon
GFX2_GLOBAL short Brouillon_Loupe_Decalage_X;// Decalage horizontal de la fen�tre de zoom dans le brouillon
GFX2_GLOBAL short Brouillon_Loupe_Decalage_Y;// Decalage vertical   de la fen�tre de zoom dans le brouillon

GFX2_GLOBAL byte Masque_copie_couleurs[256]; // Tableau des couleurs � copier vers le brouillon

  // Sauvegarde de l'image:

GFX2_GLOBAL byte * Ecran_backup;     // Sauvegarde de l'�cran virtuel courant
GFX2_GLOBAL S_Liste_de_pages * Principal_Backups; // Liste des pages de backup de la page principale
GFX2_GLOBAL S_Liste_de_pages * Brouillon_Backups; // Liste des pages de backup de la page de brouillon


  // Donn�es sur la brosse:

GFX2_GLOBAL byte * Brosse;          // Sprite de la brosse
GFX2_GLOBAL word Brosse_Decalage_X; // Centre horizontal de la brosse
GFX2_GLOBAL word Brosse_Decalage_Y; // Centre vertical de la brosse
GFX2_GLOBAL word Brosse_Largeur;    // Largeur de la brosse
GFX2_GLOBAL word Brosse_Hauteur;    // Hauteur de la brosse

GFX2_GLOBAL char  Brosse_Repertoire_fichier[TAILLE_CHEMIN_FICHIER];// |
GFX2_GLOBAL char  Brosse_Nom_fichier[TAILLE_CHEMIN_FICHIER];       // |
GFX2_GLOBAL byte  Brosse_Format_fichier;                           // |  Infos sur le
GFX2_GLOBAL byte  Brosse_Format;                                   // |_ s�lecteur de
GFX2_GLOBAL short Brosse_File_list_Position;                       // |  fichiers de la
GFX2_GLOBAL short Brosse_File_list_Decalage;                       // |  brosse.
GFX2_GLOBAL char  Brosse_Repertoire_courant[256];                  // |
GFX2_GLOBAL char  Brosse_Commentaire[TAILLE_COMMENTAIRE+1];        // |

GFX2_GLOBAL byte  Brosse_Centre_rotation_defini; // |  Infos sur le
GFX2_GLOBAL short Brosse_Centre_rotation_X;      // |- centre de rotation
GFX2_GLOBAL short Brosse_Centre_rotation_Y;      // |  de la brosse

  // Donn�es sur le menu

GFX2_GLOBAL byte  Menu_visible;        // Le menu est actif � l'�cran
GFX2_GLOBAL word  Menu_Ordonnee;       // Ordonn�e o� commence le menu
GFX2_GLOBAL word  Menu_Ordonnee_Texte; // Ordonn�e o� commence le texte dans le menu
GFX2_GLOBAL byte  Menu_Facteur_X;      // Facteur de grossissement du menu en X
GFX2_GLOBAL byte  Menu_Facteur_Y;      // Facteur de grossissement du menu en Y
GFX2_GLOBAL word  Menu_Taille_couleur; // Taille d'une couleur de la palette du menu


  // Donn�es sur la fen�tre de menu

GFX2_GLOBAL byte Fenetre; // Nombre de fenetres empil�es. 0 si pas de fenetre ouverte.

GFX2_GLOBAL byte Menu_visible_avant_fenetre;  // Le menu �tait visible avant d'ouvir une fen�tre
GFX2_GLOBAL word Menu_Ordonnee_avant_fenetre; // Ordonn�e du menu avant d'ouvrir une fen�tre
GFX2_GLOBAL byte Cacher_pinceau_avant_fenetre;// Le pinceau �tatit d�j� cach� avant l'ouverture de la fenetre?

GFX2_GLOBAL word Pile_Fenetre_Pos_X[8];   // Position du bord gauche de la fen�tre dans l'�cran
#define Fenetre_Pos_X Pile_Fenetre_Pos_X[Fenetre-1]

GFX2_GLOBAL word Pile_Fenetre_Pos_Y[8];   // Position du bord haut   de la fen�tre dans l'�cran
#define Fenetre_Pos_Y Pile_Fenetre_Pos_Y[Fenetre-1]

GFX2_GLOBAL word Pile_Fenetre_Largeur[8]; // Largeur de la fen�tre
#define Fenetre_Largeur Pile_Fenetre_Largeur[Fenetre-1]

GFX2_GLOBAL word Pile_Fenetre_Hauteur[8]; // Hauteur de la fen�tre
#define Fenetre_Hauteur Pile_Fenetre_Hauteur[Fenetre-1]

GFX2_GLOBAL word Pile_Nb_boutons_fenetre[8];
#define Nb_boutons_fenetre Pile_Nb_boutons_fenetre[Fenetre-1]

GFX2_GLOBAL T_Bouton_normal   * Pile_Fenetre_Liste_boutons_normal[8];
#define Fenetre_Liste_boutons_normal Pile_Fenetre_Liste_boutons_normal[Fenetre-1]

GFX2_GLOBAL T_Bouton_palette  * Pile_Fenetre_Liste_boutons_palette[8];
#define Fenetre_Liste_boutons_palette Pile_Fenetre_Liste_boutons_palette[Fenetre-1]

GFX2_GLOBAL T_Bouton_scroller * Pile_Fenetre_Liste_boutons_scroller[8];
#define Fenetre_Liste_boutons_scroller Pile_Fenetre_Liste_boutons_scroller[Fenetre-1]

GFX2_GLOBAL T_Bouton_special  * Pile_Fenetre_Liste_boutons_special[8];
#define Fenetre_Liste_boutons_special Pile_Fenetre_Liste_boutons_special[Fenetre-1]

GFX2_GLOBAL T_Bouton_dropdown  * Pile_Fenetre_Liste_boutons_dropdown[8];
#define Fenetre_Liste_boutons_dropdown Pile_Fenetre_Liste_boutons_dropdown[Fenetre-1]


GFX2_GLOBAL int Pile_Fenetre_Attribut1[8];
#define Fenetre_Attribut1 Pile_Fenetre_Attribut1[Fenetre-1]

// Cette variable sert � stocker 2 informations:
// - Sur le click d'un scroller, la position active (0-n)
// - Sur le click d'une palette, le numero de couleur (0-255).
GFX2_GLOBAL int Pile_Fenetre_Attribut2[8];
#define Fenetre_Attribut2 Pile_Fenetre_Attribut2[Fenetre-1]




// D�finition des boutons ////////////////////////////////////////////////////

GFX2_GLOBAL struct
{
  // Informations sur l'aspect du bouton (graphisme):
  word            Decalage_X;        // D�calage par rapport � la gauche du menu
  word            Decalage_Y;        // D�calage par rapport au haut du menu
  word            Largeur;           // Largeur du bouton
  word            Hauteur;           // Hauteur du bouton
  byte            Enfonce;           // Le bouton est enfonc�
  byte            Forme;             // Forme du bouton

  // Information sur les clicks de la souris:
  fonction_action Gauche;            // Action d�clench�e par un click gauche sur le bouton
  fonction_action Droite;            // Action d�clench�e par un click droit  sur le bouton
  word            Raccourci_gauche[2];  // Raccourci clavier �quivalent � un click gauche sur le bouton
  word            Raccourci_droite[2];  // Raccourci clavier �quivalent � un click droit  sur le bouton

  // Informations sur le d�senclenchement du bouton g�r� par le moteur:
  fonction_action Desenclencher;     // Action appel�e lors du d�senclenchement du bouton
  byte            Famille;           // Ensemble de boutons auquel celui-ci appartient

} Bouton[NB_BOUTONS];



// Informations sur les diff�rents modes de dessin

GFX2_GLOBAL fonction_effet Fonction_effet;

GFX2_GLOBAL byte * FX_Feedback_Ecran;

GFX2_GLOBAL byte Exclude_color[256]; // Couleurs � exclure pour Meilleure_couleur

  // Mode smear:

GFX2_GLOBAL byte  Smear_Mode;    // Le mode smear est enclench�
GFX2_GLOBAL byte  Smear_Debut;   // On vient juste de commencer une op�ration en Smear
GFX2_GLOBAL byte * Smear_Brosse; // Sprite de la brosse de Smear
GFX2_GLOBAL word  Smear_Brosse_Largeur; // Largeur de la brosse de Smear
GFX2_GLOBAL word  Smear_Brosse_Hauteur; // Hauteur de la brosse de Smear
GFX2_GLOBAL short Smear_Min_X,Smear_Max_X,Smear_Min_Y,Smear_Max_Y; // Bornes de la Brosse du smear

  // Mode shade:

GFX2_GLOBAL T_Shade        Shade_Liste[8]; // Listes de shade
GFX2_GLOBAL byte           Shade_Actuel;   // Num�ro du shade en cours
GFX2_GLOBAL byte *         Shade_Table;    // Table de conversion de shade en cours
GFX2_GLOBAL byte           Shade_Table_gauche[256]; // Table de conversion de shade pour un clic gauche
GFX2_GLOBAL byte           Shade_Table_droite[256]; // Table de conversion de shade pour un clic droit
GFX2_GLOBAL byte           Shade_Mode;     // Le mode shade est enclench�

GFX2_GLOBAL byte           Quick_shade_Mode; // Le mode quick-shade est enclench�
GFX2_GLOBAL byte           Quick_shade_Step; // Pas du mode quick-shade
GFX2_GLOBAL byte           Quick_shade_Loop; // Normal / Loop / No sat.

  // Mode stencil:

GFX2_GLOBAL byte Stencil_Mode;  // Le mode stencil est enclench�
GFX2_GLOBAL byte Stencil[256];  // Tableau des couleurs prot�g�es

  // Mode grille:

GFX2_GLOBAL byte  Snap_Mode;       // Le mode grille est enclench�
GFX2_GLOBAL word Snap_Largeur;    // Largeur entre 2 points de la grille
GFX2_GLOBAL word Snap_Hauteur;    // Hauteur entre 2 points de la grille
GFX2_GLOBAL word Snap_Decalage_X; // Position en X du point le + � gauche
GFX2_GLOBAL word Snap_Decalage_Y; // Position en Y du point le + en haut

  // Mode trame:

GFX2_GLOBAL byte  Trame_Mode;    // Le mode Trame est enclench�
GFX2_GLOBAL byte  Trame[16][16]; // Sprite de la trame
GFX2_GLOBAL word  TRAME_PREDEFINIE[12][16]; // Trames pr�s�finies (compact�es sur 16*16 bits)
GFX2_GLOBAL short Trame_Largeur; // Largeur de la trame
GFX2_GLOBAL short Trame_Hauteur; // Hauteur de la trame

  // Mode colorize:

GFX2_GLOBAL byte Colorize_Mode;          // Le mode Colorize est enclench�
GFX2_GLOBAL byte Colorize_Opacite;       // Intensit� du Colorize
GFX2_GLOBAL byte Colorize_Mode_en_cours; // Le type de Colorize en cours (0-2)
GFX2_GLOBAL word Table_de_multiplication_par_Facteur_A[256];
GFX2_GLOBAL word Table_de_multiplication_par_Facteur_B[256];

  // Mode smooth:

GFX2_GLOBAL byte Smooth_Mode;          // Le mode Smooth est enclench�
GFX2_GLOBAL byte Smooth_Matrice[3][3]; // La matrice du Smooth actuel

  // Mode Tiling:

GFX2_GLOBAL byte  Tiling_Mode;       // Le mode Tiling est enclench�
GFX2_GLOBAL short Tiling_Decalage_X; // D�calage du tiling en X
GFX2_GLOBAL short Tiling_Decalage_Y; // D�calage du tiling en Y

  // Mode Mask

GFX2_GLOBAL byte Mask_Mode;  // Le mode Masque est enclench�
GFX2_GLOBAL byte Mask_table[256];  // Tableau des couleurs constituant le masque

  // Mode loupe:

GFX2_GLOBAL byte  Loupe_Mode;
GFX2_GLOBAL word  Loupe_Facteur;
GFX2_GLOBAL word  Loupe_Hauteur;
GFX2_GLOBAL word  Loupe_Largeur;
GFX2_GLOBAL short Loupe_Decalage_X;
GFX2_GLOBAL short Loupe_Decalage_Y;
GFX2_GLOBAL word  * Table_mul_facteur_zoom;
GFX2_GLOBAL word  TABLE_ZOOM[NB_FACTEURS_DE_ZOOM][512];

#ifdef VARIABLES_GLOBALES
  word FACTEUR_ZOOM[NB_FACTEURS_DE_ZOOM]={2,3,4,5,6,8,10,12,14,16,18,20};
#else
  extern word FACTEUR_ZOOM[NB_FACTEURS_DE_ZOOM];
#endif

  // Donn�es sur les ellipses et les cercles:

GFX2_GLOBAL long  Ellipse_Curseur_X;
GFX2_GLOBAL long  Ellipse_Curseur_Y;
GFX2_GLOBAL long  Ellipse_Rayon_vertical_au_carre;
GFX2_GLOBAL long  Ellipse_Rayon_horizontal_au_carre;
//GFX2_GLOBAL long  Ellipse_Limite_High;
//GFX2_GLOBAL long  Ellipse_Limite_Low;
GFX2_GLOBAL uint64_t Ellipse_Limite;
GFX2_GLOBAL long  Cercle_Curseur_X;
GFX2_GLOBAL long  Cercle_Curseur_Y;
GFX2_GLOBAL long  Cercle_Limite;

  // Donn�es sur les d�grad�s:

GFX2_GLOBAL short Degrade_Borne_Inferieure;   // Plus petite couleur englob�e par le d�grad�
GFX2_GLOBAL short Degrade_Borne_Superieure;   // Plus grande couleur englob�e par le d�grad�
GFX2_GLOBAL int   Degrade_Inverse;            // Bool�en "Le d�grad� est en r�alit� invers�"
GFX2_GLOBAL long  Degrade_Intervalle_bornes;  // = Abs(Degrade_Borne_Inferieure-Degrade_Borne_Superieure)+1
GFX2_GLOBAL long  Degrade_Intervalle_total;   // Valeur maximum des indices pass�s � la fonction de d�grad� (!!! >0 !!!)
GFX2_GLOBAL long  Degrade_Melange_aleatoire;  // Facteur de m�lange (1-256+) du d�grad�
GFX2_GLOBAL fonction_degrade Traiter_degrade; // Fonction de traitement du d�grad�, varie selon la m�thode choisie par l'utilisateur.
GFX2_GLOBAL fonction_afficheur Traiter_pixel_de_degrade; // Redirection de l'affichage

GFX2_GLOBAL T_Degrade_Tableau Degrade_Tableau[16]; // Donn�es de tous les d�grad�s
GFX2_GLOBAL byte Degrade_Courant;             // Indice du tableau correspondant au d�grad� courant



  // Donn�es sur le Spray:

GFX2_GLOBAL byte  Spray_Mode;            // Mode Mono(1) ou Multicolore(0)
GFX2_GLOBAL short Spray_Size;            // Diam�tre du spray en pixels
GFX2_GLOBAL byte  Spray_Delay;           // D�lai en VBLs entre 2 "pschiitt"
GFX2_GLOBAL byte  Spray_Mono_flow;       // Nombre de pixels qui sortent en m�me temps en mono
GFX2_GLOBAL byte  Spray_Multi_flow[256]; // Idem pour chaque couleur


  // Donn�es diverses sur le programme:

GFX2_GLOBAL byte Sortir_du_programme;
GFX2_GLOBAL char Repertoire_initial[256];          // R�pertoire � partir duquel � �t� lanc� le programme
GFX2_GLOBAL char Repertoire_des_donnees[256];      // R�pertoire contenant les fichiers lus (interface graphique, etc)
GFX2_GLOBAL char Repertoire_de_configuration[256]; // R�pertoire contenant les fichiers .ini et .cfg
GFX2_GLOBAL byte Fore_color;
GFX2_GLOBAL byte Back_color;
GFX2_GLOBAL byte Mode_de_dessin_en_cours;
GFX2_GLOBAL byte Courbe_en_cours;
GFX2_GLOBAL byte Ligne_en_cours;
GFX2_GLOBAL byte Couleur_debut_palette;
GFX2_GLOBAL byte Un_fichier_a_ete_passe_en_parametre;
GFX2_GLOBAL byte Une_resolution_a_ete_passee_en_parametre; // utilis�e uniquement si la variable pr�c�dente est � 1

// Les diff�rents sprites:

GFX2_GLOBAL byte BLOCK_MENU[HAUTEUR_MENU][LARGEUR_MENU];
GFX2_GLOBAL byte SPRITE_MENU[NB_SPRITES_MENU][HAUTEUR_SPRITE_MENU][LARGEUR_SPRITE_MENU];
GFX2_GLOBAL byte SPRITE_EFFET[NB_SPRITES_EFFETS][HAUTEUR_SPRITE_MENU][LARGEUR_SPRITE_MENU];

GFX2_GLOBAL byte * Logo_GrafX2;

GFX2_GLOBAL byte Fonte_systeme[256*8*8];
GFX2_GLOBAL byte Fonte_fun    [256*8*8];
GFX2_GLOBAL byte Fonte_help_norm [256][6][8];
GFX2_GLOBAL byte Fonte_help_bold [256][6][8];
// 12
// 34
GFX2_GLOBAL byte Fonte_help_t1 [64][6][8];
GFX2_GLOBAL byte Fonte_help_t2 [64][6][8];
GFX2_GLOBAL byte Fonte_help_t3 [64][6][8];
GFX2_GLOBAL byte Fonte_help_t4 [64][6][8];
GFX2_GLOBAL byte * Fonte;

  // Les donn�es de l'aide:

GFX2_GLOBAL byte Section_d_aide_en_cours;  // Indice de la table d'aide en cours de consultation
GFX2_GLOBAL word Position_d_aide_en_cours; // Num�ro de la ligne d'aide en cours de consultation

  // Donn�es sur les op�rations

GFX2_GLOBAL word Operation_avant_interruption; // N� de l'op�ration en cours avant l'utilisation d'une interruption
GFX2_GLOBAL word Operation_en_cours;           // N� de l'op�ration en cours
GFX2_GLOBAL word Operation_Pile[TAILLE_PILE_OPERATIONS]; // Pile simplifi�e
GFX2_GLOBAL byte Operation_Taille_pile;    // Taille effective de la pile (0=vide)
GFX2_GLOBAL byte Operation_dans_loupe;     // Indique si l'op�ration a commenc� dans la partie Zoom�e ou non

GFX2_GLOBAL short Pipette_Couleur;
GFX2_GLOBAL short Pipette_X;
GFX2_GLOBAL short Pipette_Y;


#ifdef VARIABLES_GLOBALES
  byte CURSEUR_D_OPERATION[NB_OPERATIONS]=
  {
    FORME_CURSEUR_CIBLE            , // Dessin � la main continu
    FORME_CURSEUR_CIBLE            , // Dessin � la main discontinu
    FORME_CURSEUR_CIBLE            , // Dessin � la main point par point
    FORME_CURSEUR_CIBLE            , // Contour fill
    FORME_CURSEUR_CIBLE            , // Lignes
    FORME_CURSEUR_CIBLE            , // Lignes reli�es
    FORME_CURSEUR_CIBLE            , // Lignes centr�es
    FORME_CURSEUR_CIBLE_XOR        , // Rectangle vide
    FORME_CURSEUR_CIBLE_XOR        , // Rectangle plein
    FORME_CURSEUR_CIBLE            , // Cercles vides
    FORME_CURSEUR_CIBLE            , // Cercles pleins
    FORME_CURSEUR_CIBLE            , // Ellipses vides
    FORME_CURSEUR_CIBLE            , // Ellipses pleines
    FORME_CURSEUR_CIBLE            , // Fill
    FORME_CURSEUR_CIBLE            , // Remplacer
    FORME_CURSEUR_CIBLE_XOR        , // Prise de brosse rectangulaire
    FORME_CURSEUR_CIBLE            , // Prise d'une brosse multiforme
    FORME_CURSEUR_CIBLE_PIPETTE    , // R�cup�ration d'une couleur
    FORME_CURSEUR_RECTANGLE_XOR    , // Positionnement de la fen�tre de loupe
    FORME_CURSEUR_CIBLE            , // Courbe � 3 points
    FORME_CURSEUR_CIBLE            , // Courbe � 4 points
    FORME_CURSEUR_CIBLE            , // Spray
    FORME_CURSEUR_CIBLE            , // Polygone
    FORME_CURSEUR_CIBLE            , // Polyforme
    FORME_CURSEUR_CIBLE            , // Polyfill
    FORME_CURSEUR_CIBLE            , // Polyforme rempli
    FORME_CURSEUR_MULTIDIRECTIONNEL, // Scroll
    FORME_CURSEUR_CIBLE            , // Cercles d�grad�s
    FORME_CURSEUR_CIBLE            , // Ellipses d�grad�es
    FORME_CURSEUR_ROTATE_XOR       , // Faire tourner brosse
    FORME_CURSEUR_CIBLE_XOR        , // Etirer brosse
    FORME_CURSEUR_CIBLE            , // Deformer brosse
    FORME_CURSEUR_CIBLE_XOR        , // Rectangle degrade
  };
#else
  extern byte CURSEUR_D_OPERATION[NB_OPERATIONS];
#endif


  // Proc�dures � appeler: Op�ration,Mouse_K,Etat de la pile

GFX2_GLOBAL struct
{
  byte Effacer_curseur; // Bool�en "il faut effacer le curseur pour l'op�ra."
  fonction_action Action;                                   // Action appel�e
} Operation[NB_OPERATIONS][3][TAILLE_PILE_OPERATIONS];



// Informations sur les lecteurs et autres images
GFX2_GLOBAL byte SPRITE_DRIVE[NB_SPRITES_DRIVES][HAUTEUR_SPRITE_DRIVE][LARGEUR_SPRITE_DRIVE];


// -- Section des informations sur les formats de fichiers ------------------

  // Comptage du nb d'�l�ments dans la liste:
GFX2_GLOBAL short Liste_Nb_elements;
GFX2_GLOBAL short Liste_Nb_fichiers;
GFX2_GLOBAL short Liste_Nb_repertoires;
  // T�te de la liste cha�n�e:
GFX2_GLOBAL Element_de_liste_de_fileselect * Liste_du_fileselect;

// ------------------- Inititialisation des formats connus -------------------

void Rien_du_tout(void);

GFX2_GLOBAL signed char Erreur_fichier; // 0: op�ration I/O OK
                                   // 1: Erreur d�s le d�but de l'op�ration
                                   // 2: Erreur durant l'op�ration => donn�es modifi�es
                                   //-1: Interruption du chargement d'une preview

GFX2_GLOBAL int Ligne_INI;

GFX2_GLOBAL fonction_afficheur Pixel_de_chargement;
GFX2_GLOBAL fonction_lecteur   Lit_pixel_de_sauvegarde;

/********************
 * Sp�cifique � SDL *
 ********************/
GFX2_GLOBAL SDL_Surface * Ecran_SDL;
GFX2_GLOBAL SDL_Joystick* joystick;

#define TOUCHE_AUCUNE          0
#define TOUCHE_MOUSEMIDDLE     (SDLK_LAST+1)
#define TOUCHE_MOUSEWHEELUP    (SDLK_LAST+2)
#define TOUCHE_MOUSEWHEELDOWN  (SDLK_LAST+3)
#define TOUCHE_BUTTON          (SDLK_LAST+4)

#ifdef __gp2x__
  #define TOUCHE_ESC (TOUCHE_BUTTON+GP2X_BUTTON_X)
#else
  #define TOUCHE_ESC SDLK_ESCAPE
#endif

#endif
