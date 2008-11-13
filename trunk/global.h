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
#ifdef VARIABLES_GLOBALES
  #define GLOBAL
#else
  #define GLOBAL extern
#endif

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//        Ce fichier contient les d�claration des variables globales        //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


// -- Section des variables de CONFIGURATION ---------------------------------

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <SDL/SDL.h>
#include "struct.h"

GLOBAL struct S_Config
{
  byte Fonte;
  int  Lire_les_fichiers_caches;
  int  Lire_les_repertoires_caches;
  int  Lire_les_repertoires_systemes;
  byte Afficher_limites_image;
  byte Curseur;
  byte Maximize_preview;
  byte Auto_set_res;
  byte Coords_rel;
  byte Backup;
  byte Adjust_brush_pick;
  byte Auto_save;
  byte Nb_pages_Undo;
  byte Indice_Sensibilite_souris_X;
  byte Indice_Sensibilite_souris_Y;
  byte Mouse_Facteur_de_correction_X;
  byte Mouse_Facteur_de_correction_Y;
  byte Valeur_tempo_jauge_gauche;
  byte Valeur_tempo_jauge_droite;
  long Chrono_delay;
  struct Composantes Coul_menu_pref[4];
  int  Nb_max_de_vertex_par_polygon;
  byte Clear_palette;
  byte Set_resolution_according_to;
  byte Ratio;
  byte Fast_zoom;
  byte Find_file_fast;
  byte Couleurs_separees;
  byte FX_Feedback;
  byte Safety_colors;
  byte Opening_message;
  byte Clear_with_stencil;
  byte Auto_discontinuous;
  byte Taille_ecran_dans_GIF;
  byte Auto_nb_used;
  byte Resolution_par_defaut;
} Config;

  // Tableau des touches sp�ciales
GLOBAL word Config_Touche[NB_TOUCHES_SPECIALES];


struct S_Mode_video
{
  short  Largeur;
  short  Hauteur;
  byte   Mode;
  word   Fullscreen;
  byte   Etat; // 0:Cool 1:OK ; 2:Bof ; 3:Naze ; si on rajoute +128 => incompatible
};
GLOBAL struct S_Mode_video Mode_video[MAX_MODES_VIDEO];
GLOBAL int  Nb_modes_video; // Nombre de modes r�ellement recens�s dans Mode_video[]


  // Palette par d�faut

GLOBAL T_Palette Palette_defaut;

  // Couleurs du menu

GLOBAL byte CM_Noir;
GLOBAL byte CM_Fonce;
GLOBAL byte CM_Clair;
GLOBAL byte CM_Blanc;
GLOBAL byte CM_Trans;
GLOBAL struct Composantes Coul_menu_pref[4];

  // Etat des entr�es

GLOBAL word Mouse_X; // Abscisse de la souris
GLOBAL word Mouse_Y; // Ordonn�e de la souris
GLOBAL byte Mouse_K; // Etat des boutons de la souris
GLOBAL dword Touche; // Touche tap�e
GLOBAL dword Touche_ANSI; // Caract�re tap�
GLOBAL Uint8* Etat_Du_Clavier;  // Scancode de la touche en cours et etat des touches de ctrl
// Modificateurs pour Touche
#define MOD_SHIFT 0x1000
#define MOD_CTRL  0x2000
#define MOD_ALT   0x4000
GLOBAL byte Quit_demande; // !=0 lorsque l'utilisateur demande la fermeture de fen�tre.

GLOBAL byte Mouse_Facteur_de_correction_X;
GLOBAL byte Mouse_Facteur_de_correction_Y;

GLOBAL byte Autoriser_changement_de_couleur_pendant_operation;

  // Donn�es sur le curseur

GLOBAL byte Forme_curseur;
GLOBAL byte Forme_curseur_avant_fenetre; // Forme du curseur avant l'ouverture d'une fen�tre
GLOBAL byte Forcer_affichage_curseur;    // Forcer l'affichage du curseur au prochain Get_input();
GLOBAL byte Cacher_curseur;
GLOBAL byte Curseur_dans_menu;           // Bool�en "Le curseur se trouve dans le menu"
GLOBAL byte Curseur_dans_menu_precedent; // Bool�en "Le curseur se trouvait pr�c�demment dans le menu"
GLOBAL word Curseur_Decalage_X[NB_SPRITES_CURSEUR]; // Coordonn�es X du point sensible de curseurs en sprite
GLOBAL word Curseur_Decalage_Y[NB_SPRITES_CURSEUR]; // Coordonn�es Y du point sensible de curseurs en sprite
GLOBAL byte SPRITE_CURSEUR[NB_SPRITES_CURSEUR][HAUTEUR_SPRITE_CURSEUR][LARGEUR_SPRITE_CURSEUR]; // Dessins des sprites de curseur
GLOBAL byte FOND_CURSEUR[HAUTEUR_SPRITE_CURSEUR][LARGEUR_SPRITE_CURSEUR]; // Contenu du dessous du curseur

  // Donn�es sur le pinceau

GLOBAL byte  Pinceau_Forme;
GLOBAL byte  Pinceau_Forme_avant_fill;
GLOBAL byte  Pinceau_Forme_avant_pipette;
GLOBAL byte  Pinceau_Forme_avant_lasso;
GLOBAL byte  Cacher_pinceau;
GLOBAL short Pinceau_X;
GLOBAL short Pinceau_Y;
GLOBAL byte  SPRITE_PINCEAU [NB_SPRITES_PINCEAU][HAUTEUR_PINCEAU][LARGEUR_PINCEAU];
GLOBAL word  Pinceau_predefini_Largeur[NB_SPRITES_PINCEAU];
GLOBAL word  Pinceau_predefini_Hauteur[NB_SPRITES_PINCEAU];
GLOBAL byte  Pinceau_Type[NB_SPRITES_PINCEAU];
GLOBAL word  Pinceau_predefini_Decalage_X[NB_SPRITES_PINCEAU];
GLOBAL word  Pinceau_predefini_Decalage_Y[NB_SPRITES_PINCEAU];
GLOBAL byte * Pinceau_Sprite;
GLOBAL short Pinceau_Largeur;
GLOBAL short Pinceau_Hauteur;
GLOBAL short Pinceau_Decalage_X;
GLOBAL short Pinceau_Decalage_Y;

  // Commandes graphiques

GLOBAL fonction_afficheur Pixel;          // Affiche un point � l'�cran
GLOBAL fonction_afficheur Pixel_dans_menu;// Affiche un point dans le menu (ou pas si le menu est invisible)
GLOBAL fonction_lecteur   Lit_pixel;      // Teste la couleur d'un pixel dans l'�cran
GLOBAL fonction_display   Display_screen; // Affiche rapidement tout l'�cran (en faisant attention de ne pas effacer le menu)
GLOBAL fonction_block     Block;          // Affiche rapidement un bloc � l'�cran
GLOBAL fonction_afficheur Pixel_Preview_Normal; // Affiche un point de l'image � l'�cran en mode normal (pas en mode loupe)
GLOBAL fonction_afficheur Pixel_Preview_Loupe;  // Affiche un point de l'image � l'�cran en mode loupe
GLOBAL fonction_afficheur Pixel_Preview;        // Affiche un point de l'image � l'�cran en fonction de l'�tat du mode loupe
GLOBAL fonction_Ligne_XOR Ligne_horizontale_XOR;// Affiche une ligne horizontale en XOR (pour placer la loupe)
GLOBAL fonction_Ligne_XOR Ligne_verticale_XOR;  // Affiche une ligne verticale en XOR (pour placer la loupe)
GLOBAL fonction_display_brush_Color Display_brush_Color; // Affiche une partie de la brosse en couleur
GLOBAL fonction_display_brush_Mono  Display_brush_Mono;  // Affiche une partie de la brosse en monochrome
GLOBAL fonction_display_brush_Color Clear_brush;         // Efface la partie de la brosse affich�e � l'�cran
GLOBAL fonction_remap     Remap_screen;   // Remappe une partie de l'�cran avec les nouvelles couleurs du menu
GLOBAL fonction_procsline Afficher_ligne;  // Afficher une ligne
GLOBAL fonction_procsline Lire_ligne;      // Afficher ou lire une ligne
GLOBAL fonction_display_zoom Display_zoomed_screen; // Affiche rapidement toute la partie zoom�e � l'�cran (en faisant attention de ne pas effacer le menu)
GLOBAL fonction_display_brush_Color_zoom Display_brush_Color_zoom;
GLOBAL fonction_display_brush_Mono_zoom  Display_brush_Mono_zoom;
GLOBAL fonction_display_brush_Color_zoom Clear_brush_zoom;

  // Donn�es sur les dimensions de l'�cran

GLOBAL int   Resize_Largeur;      // \__ Positionn�es lorsque l'utilisateur tire
GLOBAL int   Resize_Hauteur;      // /      un bord de la fen�tre.
GLOBAL int   Resolution_actuelle; // R�solution graphique courante
GLOBAL short Ecran_original_X;    // |_ Dimensions de l'�cran d'origine de
GLOBAL short Ecran_original_Y;    // |  l'image qui vient d'�tre charg�e.
GLOBAL short Largeur_ecran;       // Largeur de l'�cran
GLOBAL short Hauteur_ecran;       // Hauteur de l'�cran
GLOBAL byte Plein_ecran;	  // Indique si on est en mode plein �cran ou fen�tr�
GLOBAL short Limite_Haut;         // |
GLOBAL short Limite_Bas;          // |_ Limites dans lesquelles
GLOBAL short Limite_Gauche;       // |  on peut �crire
GLOBAL short Limite_Droite;       // |
GLOBAL short Limite_visible_Bas;    // |_ Derniers points visibles
GLOBAL short Limite_visible_Droite; // |  "� l'image"

GLOBAL short Limite_Haut_Zoom;         // |
GLOBAL short Limite_Bas_Zoom;          // |_ Limites dans lesquelles on peut
GLOBAL short Limite_Gauche_Zoom;       // |  �crire dans la partie zoom�e
GLOBAL short Limite_Droite_Zoom;       // |
GLOBAL short Limite_visible_Bas_Zoom;    // |_ Derniers points visibles "�
GLOBAL short Limite_visible_Droite_Zoom; // |  l'image" dans la partie zoom�e

GLOBAL byte * Buffer_de_ligne_horizontale; // Buffer d'affichage de lignes

  // Donn�es sur l'image actuelle:

GLOBAL byte *    Principal_Ecran;   // Ecran virtuel courant
GLOBAL T_Palette Principal_Palette; // Palette de l'�cran en cours

GLOBAL byte  Principal_Image_modifiee; // L'image courante a �t� modifi�e
GLOBAL short Principal_Largeur_image;  // Largeur de l'image dans laquelle l'utilisateur d�sire travailler
GLOBAL short Principal_Hauteur_image;  // Hauteur de l'image dans laquelle l'utilisateur d�sire travailler
GLOBAL short Principal_Decalage_X;     // D�calage en X de l'�cran par rapport au d�but de l'image
GLOBAL short Principal_Decalage_Y;     // D�calage en Y de l'�cran par rapport au d�but de l'image
GLOBAL short Ancien_Principal_Decalage_X;
GLOBAL short Ancien_Principal_Decalage_Y;

GLOBAL char  Principal_Repertoire_fichier[1024]; // |_ Nom complet =
GLOBAL char  Principal_Nom_fichier[256];         // |  Repertoire_fichier+"\"+Nom_fichier
GLOBAL byte  Principal_Format_fichier;          // Format auquel il faut lire et �crire le fichier
GLOBAL byte  Principal_Format;               // Format du fileselect
GLOBAL short Principal_File_list_Position; // D�but de la partie affich�e dans la liste de fichiers
GLOBAL short Principal_File_list_Decalage; // D�calage de la barre de s�lection dans le fileselector
GLOBAL char  Principal_Repertoire_courant[1024]; // R�pertoire actuel sur disque
GLOBAL char  Principal_Commentaire[TAILLE_COMMENTAIRE+1]; // Commentaire de l'image

GLOBAL short Principal_Split; // Position en X du bord gauche du split de la loupe
GLOBAL short Principal_X_Zoom; // (Menu_Facteur_X) + Position en X du bord droit du split de la loupe
GLOBAL float Principal_Proportion_split; // Proportion de la zone non-zoom�e par rapport � l'�cran

  // Donn�es sur le brouillon:

GLOBAL byte *    Brouillon_Ecran;   // Ecran virtuel brouillon
GLOBAL T_Palette Brouillon_Palette; // Palette de l'�cran de brouillon

GLOBAL byte  Brouillon_Image_modifiee; // Le brouillon a �t� modifi�
GLOBAL short Brouillon_Largeur_image;  // Largeur du brouillon dans laquelle l'utilisateur d�sire travailler
GLOBAL short Brouillon_Hauteur_image;  // Hauteur du brouillon dans laquelle l'utilisateur d�sire travailler
GLOBAL short Brouillon_Decalage_X;     // D�calage en X du brouillon par rapport au d�but de l'image
GLOBAL short Brouillon_Decalage_Y;     // D�calage en Y du brouillon par rapport au d�but de l'image
GLOBAL short Ancien_Brouillon_Decalage_X;
GLOBAL short Ancien_Brouillon_Decalage_Y;

GLOBAL char  Brouillon_Repertoire_fichier[TAILLE_CHEMIN_FICHIER]; // |_ Nom complet =
GLOBAL char  Brouillon_Nom_fichier[TAILLE_CHEMIN_FICHIER];        // |  Repertoire_fichier+"\"+Nom_fichier
GLOBAL byte  Brouillon_Format_fichier;          // Format auquel il faut lire et �crire le fichier
GLOBAL byte  Brouillon_Format;               // Format du fileselect
GLOBAL short Brouillon_File_list_Position; // D�but de la partie affich�e dans la liste de fichiers
GLOBAL short Brouillon_File_list_Decalage; // D�calage de la barre de s�lection dans le fileselector
GLOBAL char  Brouillon_Repertoire_courant[TAILLE_CHEMIN_FICHIER]; // R�pertoire actuel sur disque
GLOBAL char  Brouillon_Commentaire[TAILLE_COMMENTAIRE+1]; // Commentaire de l'image

GLOBAL short Brouillon_Split; // Position en X du bord gauche du split de la loupe
GLOBAL short Brouillon_X_Zoom; // (Menu_Facteur_X) + Position en X du bord droit du split de la loupe
GLOBAL float Brouillon_Proportion_split; // Proportion de la zone non-zoom�e par rapport � l'�cran

GLOBAL byte  Brouillon_Loupe_Mode;      // On est en mode loupe dans le brouillon
GLOBAL word  Brouillon_Loupe_Facteur;   // Facteur de zoom dans le brouillon
GLOBAL word  Brouillon_Loupe_Hauteur;   // Largeur de la fen�tre de zoom dans le brouillon
GLOBAL word  Brouillon_Loupe_Largeur;   // Hauteur de la fen�tre de zoom dans le brouillon
GLOBAL short Brouillon_Loupe_Decalage_X;// Decalage horizontal de la fen�tre de zoom dans le brouillon
GLOBAL short Brouillon_Loupe_Decalage_Y;// Decalage vertical   de la fen�tre de zoom dans le brouillon

GLOBAL byte Masque_copie_couleurs[256]; // Tableau des couleurs � copier vers le brouillon

  // Sauvegarde de l'image:

GLOBAL byte * Ecran_backup;     // Sauvegarde de l'�cran virtuel courant
GLOBAL S_Liste_de_pages * Principal_Backups; // Liste des pages de backup de la page principale
GLOBAL S_Liste_de_pages * Brouillon_Backups; // Liste des pages de backup de la page de brouillon


  // Donn�es sur la brosse:

GLOBAL byte * Brosse;          // Sprite de la brosse
GLOBAL word Brosse_Decalage_X; // Centre horizontal de la brosse
GLOBAL word Brosse_Decalage_Y; // Centre vertical de la brosse
GLOBAL word Brosse_Largeur;    // Largeur de la brosse
GLOBAL word Brosse_Hauteur;    // Hauteur de la brosse

GLOBAL char  Brosse_Repertoire_fichier[TAILLE_CHEMIN_FICHIER];// |
GLOBAL char  Brosse_Nom_fichier[TAILLE_CHEMIN_FICHIER];       // |
GLOBAL byte  Brosse_Format_fichier;                           // |  Infos sur le
GLOBAL byte  Brosse_Format;                                   // |_ s�lecteur de
GLOBAL short Brosse_File_list_Position;                       // |  fichiers de la
GLOBAL short Brosse_File_list_Decalage;                       // |  brosse.
GLOBAL char  Brosse_Repertoire_courant[256];                  // |
GLOBAL char  Brosse_Commentaire[TAILLE_COMMENTAIRE+1];        // |

GLOBAL byte  Brosse_Centre_rotation_defini; // |  Infos sur le
GLOBAL short Brosse_Centre_rotation_X;      // |- centre de rotation
GLOBAL short Brosse_Centre_rotation_Y;      // |  de la brosse

  // Donn�es sur le menu

GLOBAL byte  Menu_visible;        // Le menu est actif � l'�cran
GLOBAL word  Menu_Ordonnee;       // Ordonn�e o� commence le menu
GLOBAL word  Menu_Ordonnee_Texte; // Ordonn�e o� commence le texte dans le menu
GLOBAL word  Menu_Facteur_X;      // Facteur de grossissement du menu en X
GLOBAL word  Menu_Facteur_Y;      // Facteur de grossissement du menu en Y
GLOBAL word  Menu_Taille_couleur; // Taille d'une couleur de la palette du menu


  // Donn�es sur la fen�tre de menu

GLOBAL byte Fenetre; // Nombre de fenetres empil�es. 0 si pas de fenetre ouverte.

GLOBAL byte Menu_visible_avant_fenetre;  // Le menu �tait visible avant d'ouvir une fen�tre
GLOBAL word Menu_Ordonnee_avant_fenetre; // Ordonn�e du menu avant d'ouvrir une fen�tre
GLOBAL byte Cacher_pinceau_avant_fenetre;// Le pinceau �tatit d�j� cach� avant l'ouverture de la fenetre?

GLOBAL word Pile_Fenetre_Pos_X[8];   // Position du bord gauche de la fen�tre dans l'�cran
#define Fenetre_Pos_X Pile_Fenetre_Pos_X[Fenetre-1]

GLOBAL word Pile_Fenetre_Pos_Y[8];   // Position du bord haut   de la fen�tre dans l'�cran
#define Fenetre_Pos_Y Pile_Fenetre_Pos_Y[Fenetre-1]

GLOBAL word Pile_Fenetre_Largeur[8]; // Largeur de la fen�tre
#define Fenetre_Largeur Pile_Fenetre_Largeur[Fenetre-1]

GLOBAL word Pile_Fenetre_Hauteur[8]; // Hauteur de la fen�tre
#define Fenetre_Hauteur Pile_Fenetre_Hauteur[Fenetre-1]

GLOBAL word Pile_Nb_boutons_fenetre[8];
#define Nb_boutons_fenetre Pile_Nb_boutons_fenetre[Fenetre-1]

GLOBAL struct Fenetre_Bouton_normal   * Pile_Fenetre_Liste_boutons_normal[8];
#define Fenetre_Liste_boutons_normal Pile_Fenetre_Liste_boutons_normal[Fenetre-1]

GLOBAL struct Fenetre_Bouton_palette  * Pile_Fenetre_Liste_boutons_palette[8];
#define Fenetre_Liste_boutons_palette Pile_Fenetre_Liste_boutons_palette[Fenetre-1]

GLOBAL struct Fenetre_Bouton_scroller * Pile_Fenetre_Liste_boutons_scroller[8];
#define Fenetre_Liste_boutons_scroller Pile_Fenetre_Liste_boutons_scroller[Fenetre-1]

GLOBAL struct Fenetre_Bouton_special  * Pile_Fenetre_Liste_boutons_special[8];
#define Fenetre_Liste_boutons_special Pile_Fenetre_Liste_boutons_special[Fenetre-1]

GLOBAL int Pile_Fenetre_Attribut1[8];
#define Fenetre_Attribut1 Pile_Fenetre_Attribut1[Fenetre-1]

GLOBAL int Pile_Fenetre_Attribut2[8];
#define Fenetre_Attribut2 Pile_Fenetre_Attribut2[Fenetre-1]




// D�finition des boutons ////////////////////////////////////////////////////

GLOBAL struct
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
  word            Raccourci_gauche;  // Raccourci clavier �quivalent � un click gauche sur le bouton
  word            Raccourci_droite;  // Raccourci clavier �quivalent � un click droit  sur le bouton

  // Informations sur le d�senclenchement du bouton g�r� par le moteur:
  fonction_action Desenclencher;     // Action appel�e lors du d�senclenchement du bouton
  byte            Famille;           // Ensemble de boutons auquel celui-ci appartient

} Bouton[NB_BOUTONS];



// Informations sur les diff�rents modes de dessin

GLOBAL fonction_effet Fonction_effet;

GLOBAL byte * FX_Feedback_Ecran;

GLOBAL byte Exclude_color[256]; // Couleurs � exclure pour Meilleure_couleur

  // Mode smear:

GLOBAL byte  Smear_Mode;    // Le mode smear est enclench�
GLOBAL byte  Smear_Debut;   // On vient juste de commencer une op�ration en Smear
GLOBAL byte * Smear_Brosse; // Sprite de la brosse de Smear
GLOBAL word  Smear_Brosse_Largeur; // Largeur de la brosse de Smear
GLOBAL word  Smear_Brosse_Hauteur; // Hauteur de la brosse de Smear
GLOBAL short Smear_Min_X,Smear_Max_X,Smear_Min_Y,Smear_Max_Y; // Bornes de la Brosse du smear

  // Mode shade:

GLOBAL struct T_Shade Shade_Liste[8]; // Listes de shade
GLOBAL byte           Shade_Actuel;   // Num�ro du shade en cours
GLOBAL byte *         Shade_Table;    // Table de conversion de shade en cours
GLOBAL byte           Shade_Table_gauche[256]; // Table de conversion de shade pour un clic gauche
GLOBAL byte           Shade_Table_droite[256]; // Table de conversion de shade pour un clic droit
GLOBAL byte           Shade_Mode;     // Le mode shade est enclench�

GLOBAL byte           Quick_shade_Mode; // Le mode quick-shade est enclench�
GLOBAL byte           Quick_shade_Step; // Pas du mode quick-shade
GLOBAL byte           Quick_shade_Loop; // Normal / Loop / No sat.

  // Mode stencil:

GLOBAL byte Stencil_Mode;  // Le mode stencil est enclench�
GLOBAL byte Stencil[256];  // Tableau des couleurs prot�g�es

  // Mode grille:

GLOBAL byte  Snap_Mode;       // Le mode grille est enclench�
GLOBAL word Snap_Largeur;    // Largeur entre 2 points de la grille
GLOBAL word Snap_Hauteur;    // Hauteur entre 2 points de la grille
GLOBAL word Snap_Decalage_X; // Position en X du point le + � gauche
GLOBAL word Snap_Decalage_Y; // Position en Y du point le + en haut

  // Mode trame:

GLOBAL byte  Trame_Mode;    // Le mode Trame est enclench�
GLOBAL byte  Trame[16][16]; // Sprite de la trame
GLOBAL word  TRAME_PREDEFINIE[12][16]; // Trames pr�s�finies (compact�es sur 16*16 bits)
GLOBAL short Trame_Largeur; // Largeur de la trame
GLOBAL short Trame_Hauteur; // Hauteur de la trame

  // Mode colorize:

GLOBAL byte Colorize_Mode;          // Le mode Colorize est enclench�
GLOBAL byte Colorize_Opacite;       // Intensit� du Colorize
GLOBAL byte Colorize_Mode_en_cours; // Le type de Colorize en cours (0-2)
GLOBAL word Table_de_multiplication_par_Facteur_A[64];
GLOBAL word Table_de_multiplication_par_Facteur_B[64];

  // Mode smooth:

GLOBAL byte Smooth_Mode;          // Le mode Smooth est enclench�
GLOBAL byte Smooth_Matrice[3][3]; // La matrice du Smooth actuel

  // Mode Tiling:

GLOBAL byte  Tiling_Mode;       // Le mode Tiling est enclench�
GLOBAL short Tiling_Decalage_X; // D�calage du tiling en X
GLOBAL short Tiling_Decalage_Y; // D�calage du tiling en Y

  // Mode Mask

GLOBAL byte Mask_Mode;  // Le mode Masque est enclench�
GLOBAL byte Mask_table[256];  // Tableau des couleurs constituant le masque

  // Mode loupe:

GLOBAL byte  Loupe_Mode;
GLOBAL word  Loupe_Facteur;
GLOBAL word  Loupe_Hauteur;
GLOBAL word  Loupe_Largeur;
GLOBAL short Loupe_Decalage_X;
GLOBAL short Loupe_Decalage_Y;
GLOBAL word  * Table_mul_facteur_zoom;
GLOBAL word  TABLE_ZOOM[NB_FACTEURS_DE_ZOOM][512];

#ifdef VARIABLES_GLOBALES
  word FACTEUR_ZOOM[NB_FACTEURS_DE_ZOOM]={2,3,4,5,6,8,10,12,14,16,18,20};
#else
  extern word FACTEUR_ZOOM[NB_FACTEURS_DE_ZOOM];
#endif

  // Donn�es sur les ellipses et les cercles:

GLOBAL long  Ellipse_Curseur_X;
GLOBAL long  Ellipse_Curseur_Y;
GLOBAL long  Ellipse_Rayon_vertical_au_carre;
GLOBAL long  Ellipse_Rayon_horizontal_au_carre;
//GLOBAL long  Ellipse_Limite_High;
//GLOBAL long  Ellipse_Limite_Low;
GLOBAL uint64_t Ellipse_Limite;
GLOBAL long  Cercle_Curseur_X;
GLOBAL long  Cercle_Curseur_Y;
GLOBAL long  Cercle_Limite;

  // Donn�es sur les d�grad�s:

GLOBAL short Degrade_Borne_Inferieure;   // Plus petite couleur englob�e par le d�grad�
GLOBAL short Degrade_Borne_Superieure;   // Plus grande couleur englob�e par le d�grad�
GLOBAL int   Degrade_Inverse;            // Bool�en "Le d�grad� est en r�alit� invers�"
GLOBAL long  Degrade_Intervalle_bornes;  // = Abs(Degrade_Borne_Inferieure-Degrade_Borne_Superieure)+1
GLOBAL long  Degrade_Intervalle_total;   // Valeur maximum des indices pass�s � la fonction de d�grad� (!!! >0 !!!)
GLOBAL long  Degrade_Melange_aleatoire;  // Facteur de m�lange (1-256+) du d�grad�
GLOBAL fonction_degrade Traiter_degrade; // Fonction de traitement du d�grad�, varie selon la m�thode choisie par l'utilisateur.
GLOBAL fonction_afficheur Traiter_pixel_de_degrade; // Redirection de l'affichage

GLOBAL struct T_Degrade_Tableau Degrade_Tableau[16]; // Donn�es de tous les d�grad�s
GLOBAL byte Degrade_Courant;             // Indice du tableau correspondant au d�grad� courant



  // Donn�es sur le Spray:

GLOBAL byte  Spray_Mode;            // Mode Mono(1) ou Multicolore(0)
GLOBAL short Spray_Size;            // Diam�tre du spray en pixels
GLOBAL byte  Spray_Delay;           // D�lai en VBLs entre 2 "pschiitt"
GLOBAL byte  Spray_Mono_flow;       // Nombre de pixels qui sortent en m�me temps en mono
GLOBAL byte  Spray_Multi_flow[256]; // Idem pour chaque couleur


  // Donn�es diverses sur le programme:

GLOBAL byte Sortir_du_programme;
GLOBAL char Repertoire_du_programme[256]; // R�pertoire dans lequel se trouve le programme
GLOBAL char Repertoire_initial[256];      // R�pertoire � partir duquel � �t� lanc� le programme
GLOBAL byte Fore_color;
GLOBAL byte Back_color;
GLOBAL byte Mode_de_dessin_en_cours;
GLOBAL byte Courbe_en_cours;
GLOBAL byte Ligne_en_cours;
GLOBAL byte Couleur_debut_palette;
GLOBAL byte Un_fichier_a_ete_passe_en_parametre;
GLOBAL byte Une_resolution_a_ete_passee_en_parametre; // utilis�e uniquement si la variable pr�c�dente est � 1

  // Variables concernant l'OBJ DIVERS

GLOBAL word INPUT_Nouveau_Mouse_X;
GLOBAL word INPUT_Nouveau_Mouse_Y;
GLOBAL byte INPUT_Nouveau_Mouse_K;
GLOBAL byte INPUT_Keyb_mode;

  // Les diff�rents sprites:

GLOBAL byte BLOCK_MENU[HAUTEUR_MENU][LARGEUR_MENU];
GLOBAL byte SPRITE_MENU[NB_SPRITES_MENU][HAUTEUR_SPRITE_MENU][LARGEUR_SPRITE_MENU];
GLOBAL byte SPRITE_EFFET[NB_SPRITES_EFFETS][HAUTEUR_SPRITE_MENU][LARGEUR_SPRITE_MENU];

GLOBAL byte * Logo_GrafX2;

GLOBAL byte Fonte_systeme[256*8*8];
GLOBAL byte Fonte_fun    [256*8*8];
GLOBAL byte Fonte_help   [315][6][8];
GLOBAL byte * Fonte;

  // Les donn�es de l'aide:

GLOBAL byte Section_d_aide_en_cours;  // Indice de la table d'aide en cours de consultation
GLOBAL word Position_d_aide_en_cours; // Num�ro de la ligne d'aide en cours de consultation

  // Donn�es sur les op�rations

GLOBAL word Operation_avant_interruption; // N� de l'op�ration en cours avant l'utilisation d'une interruption
GLOBAL word Operation_en_cours;           // N� de l'op�ration en cours
GLOBAL word Operation_Pile[TAILLE_PILE_OPERATIONS]; // Pile simplifi�e
GLOBAL byte Operation_Taille_pile;    // Taille effective de la pile (0=vide)
GLOBAL byte Operation_dans_loupe;     // Indique si l'op�ration a commenc� dans la partie Zoom�e ou non

GLOBAL short Pipette_Couleur;
GLOBAL short Pipette_X;
GLOBAL short Pipette_Y;


#ifdef VARIABLES_GLOBALES
  byte CURSEUR_D_OPERATION[NB_OPERATIONS]=
  {
    FORME_CURSEUR_CIBLE            , // Dessin � la main continu
    FORME_CURSEUR_CIBLE            , // Dessin � la main discontinu
    FORME_CURSEUR_CIBLE            , // Dessin � la main point par point
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

GLOBAL struct
{
  byte Effacer_curseur; // Bool�en "il faut effacer le curseur pour l'op�ra."
  fonction_action Action;                                   // Action appel�e
} Operation[NB_OPERATIONS][3][TAILLE_PILE_OPERATIONS];



// Informations sur les lecteurs

GLOBAL byte Nb_drives;
GLOBAL struct T_Drive Drive[26];
GLOBAL byte SPRITE_DRIVE[NB_SPRITES_DRIVES][HAUTEUR_SPRITE_DRIVE][LARGEUR_SPRITE_DRIVE];


// -- Section des informations sur les formats de fichiers ------------------

  // Comptage du nb d'�l�ments dans la liste:
GLOBAL short Liste_Nb_elements;
GLOBAL short Liste_Nb_fichiers;
GLOBAL short Liste_Nb_repertoires;
  // T�te de la liste cha�n�e:
GLOBAL struct Element_de_liste_de_fileselect * Liste_du_fileselect;

// ------------------- Inititialisation des formats connus -------------------

#include "loadsave.h"
void Rien_du_tout(void);

#ifdef VARIABLES_GLOBALES
  // Extension du format
  char Format_Extension[NB_FORMATS_CONNUS][4]=
  {
    "pkm", // PKM
    "lbm", // LBM
    "gif", // GIF
    "bmp", // BMP
    "pcx", // PCX
    "img", // IMG
    "sc?", // SCx
    "pi1", // PI1
    "pc1", // PC1
    "cel", // CEL
    "kcf", // KCF
    "pal"  // PAL
  };

  // Fonction � appeler pour v�rifier la signature du fichier
  fonction_action Format_Test[NB_FORMATS_LOAD]=
  {
    Test_PKM, // PKM
    Test_LBM, // LBM
    Test_GIF, // GIF
    Test_BMP, // BMP
    Test_PCX, // PCX
    Test_IMG, // IMG
    Test_SCx, // SCx
    Test_PI1, // PI1
    Test_PC1, // PC1
    Test_CEL, // CEL
    Test_KCF, // KCF
    Test_PAL  // PAL
  };

  // Fonction � appeler pour charger l'image
  fonction_action Format_Load[NB_FORMATS_LOAD]=
  {
    Load_PKM, // PKM
    Load_LBM, // LBM
    Load_GIF, // GIF
    Load_BMP, // BMP
    Load_PCX, // PCX
    Load_IMG, // IMG
    Load_SCx, // SCx
    Load_PI1, // PI1
    Load_PC1, // PC1
    Load_CEL, // CEL
    Load_KCF, // KCF
    Load_PAL  // PAL
  };

  // Fonction � appeler pour sauvegarder l'image
  fonction_action Format_Save[NB_FORMATS_SAVE]=
  {
    Save_PKM, // PKM
    Save_LBM, // LBM
    Save_GIF, // GIF
    Save_BMP, // BMP
    Save_PCX, // PCX
    Save_IMG, // IMG
    Save_SCx, // SCx
    Save_PI1, // PI1
    Save_PC1, // PC1
    Save_CEL, // CEL
    Save_KCF, // KCF
    Save_PAL  // PAL
  };

  // indique si l'on doit consid�rer que l'image n'est plus modifi�e
  byte Format_Backup_done[NB_FORMATS_CONNUS]=
  {
    1, // PKM
    1, // LBM
    1, // GIF
    1, // BMP
    1, // PCX
    1, // IMG
    1, // SCx
    1, // PI1
    1, // PC1
    1, // CEL
    0, // KCF
    0  // PAL
  };

  // Le format de fichier autorise un commentaire
  byte Format_Commentaire[NB_FORMATS_CONNUS]=
  {
    1, // PKM
    0, // LBM
    0, // GIF
    0, // BMP
    0, // PCX
    0, // IMG
    0, // SCx
    0, // PI1
    0, // PC1
    0, // CEL
    0, // KCF
    0  // PAL
  };
#else
  extern char Format_Extension[NB_FORMATS_CONNUS][4];
  extern fonction_action Format_Load[NB_FORMATS_LOAD];
  extern fonction_action Format_Save[NB_FORMATS_SAVE];
  extern fonction_action Format_Test[NB_FORMATS_LOAD];
  extern byte Format_Backup_done[NB_FORMATS_CONNUS];
  extern byte Format_Commentaire[NB_FORMATS_CONNUS];
#endif

GLOBAL signed char Erreur_fichier; // 0: op�ration I/O OK
                                   // 1: Erreur d�s le d�but de l'op�ration
                                   // 2: Erreur durant l'op�ration => donn�es modifi�es
                                   //-1: Interruption du chargement d'une preview

GLOBAL int Ligne_INI;

GLOBAL fonction_afficheur Pixel_de_chargement;
GLOBAL fonction_lecteur   Lit_pixel_de_sauvegarde;

/********************
 * Sp�cifique � SDL *
 ********************/
GLOBAL SDL_Surface * Ecran_SDL;
#endif
