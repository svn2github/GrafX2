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
#ifdef GLOBAL_VARIABLES
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
GFX2_GLOBAL word Config_Key[NB_SPECIAL_SHORTCUTS][2];


typedef struct
{
  short  Width;
  short  Height;
  byte   Mode;
  word   Fullscreen;
  byte   State; // 0:Cool 1:OK ; 2:Bof ; 3:Naze ; si on rajoute +128 => incompatible
} T_Video_mode;
GFX2_GLOBAL T_Video_mode Video_mode[MAX_VIDEO_MODES];
GFX2_GLOBAL int  Nb_video_modes; // Nombre de modes r�ellement recens�s dans Video_mode[]


  // Palette par d�faut

GFX2_GLOBAL T_Palette Default_palette;

  // Couleurs du menu

GFX2_GLOBAL byte MC_Black;
GFX2_GLOBAL byte MC_Dark;
GFX2_GLOBAL byte MC_Light;
GFX2_GLOBAL byte MC_White;
GFX2_GLOBAL byte MC_Trans;
GFX2_GLOBAL T_Components Fav_menu_colors[4];

  // State des entr�es

GFX2_GLOBAL word Mouse_X; // Abscisse de la souris
GFX2_GLOBAL word Mouse_Y; // Ordonn�e de la souris
GFX2_GLOBAL byte Mouse_K; // State des boutons de la souris (tient comte des boutons appuy�s simultan�ments

#define Mouse_K_unique (Mouse_K==0?0:(Mouse_K&1?1:(Mouse_K&2?2:0))) // State des boutons de la souris (un seul bouton � la fois, on regarde d'abord le 1, puis le 2, ...)

GFX2_GLOBAL dword Key; // Key tap�e
GFX2_GLOBAL dword Key_ANSI; // Caract�re tap�
GFX2_GLOBAL Uint8* Keyboard_state;  // Scancode de la touche en cours et etat des touches de ctrl
// Modificateurs pour Key
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

GFX2_GLOBAL byte Quit_is_required; // !=0 lorsque l'utilisateur demande la fermeture de fen�tre.

GFX2_GLOBAL byte Mouse_fix_factor_X;
GFX2_GLOBAL byte Mouse_fix_factor_Y;

GFX2_GLOBAL byte Allow_color_change_during_operation;

  // Donn�es sur le curseur

GFX2_GLOBAL byte Cursor_shape;
GFX2_GLOBAL byte Cursor_shape_before_window; // Forme du curseur avant l'ouverture d'une fen�tre
GFX2_GLOBAL byte Cursor_hidden;
GFX2_GLOBAL byte Cursor_in_menu;           // Bool�en "Le curseur se trouve dans le menu"
GFX2_GLOBAL byte Cursor_in_menu_precedent; // Bool�en "Le curseur se trouvait pr�c�demment dans le menu"
GFX2_GLOBAL word Cursor_offset_X[NB_CURSOR_SPRITES]; // Coordonn�es X du point sensible de curseurs en sprite
GFX2_GLOBAL word Cursor_offset_Y[NB_CURSOR_SPRITES]; // Coordonn�es Y du point sensible de curseurs en sprite
GFX2_GLOBAL byte GFX_cursor_sprite[NB_CURSOR_SPRITES][CURSOR_SPRITE_HEIGHT][CURSOR_SPRITE_WIDTH]; // Dessins des sprites de curseur
GFX2_GLOBAL byte CURSOR_BACKGROUND[CURSOR_SPRITE_HEIGHT][CURSOR_SPRITE_WIDTH]; // Contenu du dessous du curseur

  // Donn�es sur le pinceau

GFX2_GLOBAL byte  Paintbrush_shape;
GFX2_GLOBAL byte  Paintbrush_shape_before_fill;
GFX2_GLOBAL byte  Paintbrush_shape_before_colorpicker;
GFX2_GLOBAL byte  Paintbrush_shape_before_lasso;
GFX2_GLOBAL byte  Paintbrush_hidden;
GFX2_GLOBAL short Paintbrush_X;
GFX2_GLOBAL short Paintbrush_Y;
GFX2_GLOBAL byte  GFX_paintbrush_sprite [NB_PAINTBRUSH_SPRITES][PAINTBRUSH_HEIGHT][PAINTBRUSH_WIDTH];
GFX2_GLOBAL word  Preset_paintbrush_width[NB_PAINTBRUSH_SPRITES];
GFX2_GLOBAL word  Preset_paintbrush_height[NB_PAINTBRUSH_SPRITES];
GFX2_GLOBAL byte  Paintbrush_type[NB_PAINTBRUSH_SPRITES];
GFX2_GLOBAL word  Preset_paintbrush_offset_X[NB_PAINTBRUSH_SPRITES];
GFX2_GLOBAL word  Preset_paintbrush_offset_Y[NB_PAINTBRUSH_SPRITES];
GFX2_GLOBAL byte * Paintbrush_sprite;
GFX2_GLOBAL short Paintbrush_width;
GFX2_GLOBAL short Paintbrush_height;
GFX2_GLOBAL short Paintbrush_offset_X;
GFX2_GLOBAL short Paintbrush_offset_Y;

  // Commandes graphiques

GFX2_GLOBAL int Pixel_ratio;
GFX2_GLOBAL int Pixel_width;
GFX2_GLOBAL int Pixel_height;
GFX2_GLOBAL Func_pixel Pixel;          // Affiche un point � l'�cran
GFX2_GLOBAL Func_pixel Pixel_in_menu;// Affiche un point dans le menu (ou pas si le menu est invisible)
GFX2_GLOBAL Func_read   Read_pixel;      // Teste la couleur d'un pixel dans l'�cran
GFX2_GLOBAL Func_display   Display_screen; // Affiche rapidement tout l'�cran (en faisant attention de ne pas effacer le menu)
GFX2_GLOBAL Func_block     Block;          // Affiche rapidement un bloc � l'�cran
GFX2_GLOBAL Func_pixel Pixel_preview_normal; // Affiche un point de l'image � l'�cran en mode normal (pas en mode loupe)
GFX2_GLOBAL Func_pixel Pixel_preview_magnifier;  // Affiche un point de l'image � l'�cran en mode loupe
GFX2_GLOBAL Func_pixel Pixel_preview;        // Affiche un point de l'image � l'�cran en fonction de l'�tat du mode loupe
GFX2_GLOBAL Func_line_XOR Horizontal_XOR_line;// Affiche une ligne horizontale en XOR (pour placer la loupe)
GFX2_GLOBAL Func_line_XOR Vertical_XOR_line;  // Affiche une ligne verticale en XOR (pour placer la loupe)
GFX2_GLOBAL Func_display_brush_color Display_brush_color; // Affiche une partie de la brosse en couleur
GFX2_GLOBAL Func_display_brush_mono  Display_brush_mono;  // Affiche une partie de la brosse en monochrome
GFX2_GLOBAL Func_display_brush_color Clear_brush;         // Efface la partie de la brosse affich�e � l'�cran
GFX2_GLOBAL Func_remap     Remap_screen;   // Remappe une partie de l'�cran avec les nouvelles couleurs du menu
GFX2_GLOBAL Func_procsline Display_line;  // Afficher une ligne
GFX2_GLOBAL Func_procsline Display_line_fast;  // Afficher une ligne talle quelle (sans la doubler en mode wide)
GFX2_GLOBAL Func_procsline Read_line;      // Afficher ou lire une ligne
GFX2_GLOBAL Func_display_zoom Display_zoomed_screen; // Affiche rapidement toute la partie zoom�e � l'�cran (en faisant attention de ne pas effacer le menu)
GFX2_GLOBAL Func_display_brush_color_zoom Display_brush_color_zoom;
GFX2_GLOBAL Func_display_brush_mono_zoom  Display_brush_mono_zoom;
GFX2_GLOBAL Func_display_brush_color_zoom Clear_brush_scaled;
GFX2_GLOBAL Func_draw_brush Display_brush;
  // Donn�es sur les dimensions de l'�cran

GFX2_GLOBAL int   Resize_width;      // \__ Positionn�es lorsque l'utilisateur tire
GFX2_GLOBAL int   Resize_height;      // /      un bord de la fen�tre.
GFX2_GLOBAL int   Current_resolution; // R�solution graphique courante
GFX2_GLOBAL short Original_screen_X;    // |_ Dimensions de l'�cran d'origine de
GFX2_GLOBAL short Original_screen_Y;    // |  l'image qui vient d'�tre charg�e.
GFX2_GLOBAL short Screen_width;       // Largeur de l'�cran
GFX2_GLOBAL short Screen_height;       // Hauteur de l'�cran
GFX2_GLOBAL short Limit_top;         // |
GFX2_GLOBAL short Limit_bottom;          // |_ Limites dans lesquelles
GFX2_GLOBAL short Limit_left;       // |  on peut �crire
GFX2_GLOBAL short Limit_right;       // |
GFX2_GLOBAL short Limit_visible_bottom;    // |_ Derniers points visibles
GFX2_GLOBAL short Limit_visible_right; // |  "� l'image"

GFX2_GLOBAL short Limit_top_zoom;         // |
GFX2_GLOBAL short Limit_bottom_zoom;          // |_ Limites dans lesquelles on peut
GFX2_GLOBAL short Limit_left_zoom;       // |  �crire dans la partie zoom�e
GFX2_GLOBAL short Limit_right_zoom;       // |
GFX2_GLOBAL short Limit_visible_bottom_zoom;    // |_ Derniers points visibles "�
GFX2_GLOBAL short Limit_visible_right_zoom; // |  l'image" dans la partie zoom�e

GFX2_GLOBAL byte * Horizontal_line_buffer; // buffer d'affichage de lignes

  // Donn�es sur l'image actuelle:

GFX2_GLOBAL byte *    Main_screen;   // Screen virtuel courant
GFX2_GLOBAL T_Palette Main_palette; // Palette de l'�cran en cours

GFX2_GLOBAL byte  Main_image_is_modified; // L'image courante a �t� modifi�e
GFX2_GLOBAL short Main_image_width;  // Largeur de l'image dans laquelle l'utilisateur d�sire travailler
GFX2_GLOBAL short Main_image_height;  // Hauteur de l'image dans laquelle l'utilisateur d�sire travailler
GFX2_GLOBAL short Main_offset_X;     // D�calage en X de l'�cran par rapport au d�but de l'image
GFX2_GLOBAL short Main_offset_Y;     // D�calage en Y de l'�cran par rapport au d�but de l'image
GFX2_GLOBAL short Old_main_offset_X;
GFX2_GLOBAL short Old_main_offset_Y;

GFX2_GLOBAL char  Main_file_directory[1024]; // |_ Nom complet =
GFX2_GLOBAL char  Main_filename[256];         // |  File_directory+"\"+Filename
GFX2_GLOBAL byte  Main_fileformat;          // Format auquel il faut lire et �crire le fichier
GFX2_GLOBAL byte  Main_format;               // Format du fileselect
GFX2_GLOBAL short Main_fileselector_position; // D�but de la partie affich�e dans la liste de fichiers
GFX2_GLOBAL short Main_fileselector_offset; // D�calage de la barre de s�lection dans le fileselector
GFX2_GLOBAL char  Main_current_directory[1024]; // R�pertoire actuel sur disque
GFX2_GLOBAL char  Main_comment[COMMENT_SIZE+1]; // Commentaire de l'image

GFX2_GLOBAL short Main_separator_position; // Position en X du bord gauche du split de la loupe
GFX2_GLOBAL short Main_X_zoom; // (Menu_factor_X) + Position en X du bord droit du split de la loupe
GFX2_GLOBAL float Main_separator_proportion; // Proportion de la zone non-zoom�e par rapport � l'�cran

  // Donn�es sur le brouillon:

GFX2_GLOBAL byte *    Spare_screen;   // Screen virtuel brouillon
GFX2_GLOBAL T_Palette Spare_palette; // Palette de l'�cran de brouillon

GFX2_GLOBAL byte  Spare_image_is_modified; // Le brouillon a �t� modifi�
GFX2_GLOBAL short Spare_image_width;  // Largeur du brouillon dans laquelle l'utilisateur d�sire travailler
GFX2_GLOBAL short Spare_image_height;  // Hauteur du brouillon dans laquelle l'utilisateur d�sire travailler
GFX2_GLOBAL short Spare_offset_X;     // D�calage en X du brouillon par rapport au d�but de l'image
GFX2_GLOBAL short Spare_offset_Y;     // D�calage en Y du brouillon par rapport au d�but de l'image
GFX2_GLOBAL short Old_spare_offset_X;
GFX2_GLOBAL short Old_spare_offset_Y;

GFX2_GLOBAL char  Spare_file_directory[MAX_PATH_CHARACTERS]; // |_ Nom complet =
GFX2_GLOBAL char  Spare_filename[MAX_PATH_CHARACTERS];        // |  File_directory+"\"+Filename
GFX2_GLOBAL byte  Spare_fileformat;          // Format auquel il faut lire et �crire le fichier
GFX2_GLOBAL byte  Spare_format;               // Format du fileselect
GFX2_GLOBAL short Spare_fileselector_position; // D�but de la partie affich�e dans la liste de fichiers
GFX2_GLOBAL short Spare_fileselector_offset; // D�calage de la barre de s�lection dans le fileselector
GFX2_GLOBAL char  Spare_current_directory[MAX_PATH_CHARACTERS]; // R�pertoire actuel sur disque
GFX2_GLOBAL char  Spare_comment[COMMENT_SIZE+1]; // Commentaire de l'image

GFX2_GLOBAL short Spare_separator_position; // Position en X du bord gauche du split de la loupe
GFX2_GLOBAL short Spare_X_zoom; // (Menu_factor_X) + Position en X du bord droit du split de la loupe
GFX2_GLOBAL float Spare_separator_proportion; // Proportion de la zone non-zoom�e par rapport � l'�cran

GFX2_GLOBAL byte  Spare_magnifier_mode;      // On est en mode loupe dans le brouillon
GFX2_GLOBAL word  Spare_magnifier_factor;   // Facteur de zoom dans le brouillon
GFX2_GLOBAL word  Spare_magnifier_height;   // Largeur de la fen�tre de zoom dans le brouillon
GFX2_GLOBAL word  Spare_magnifier_width;   // Hauteur de la fen�tre de zoom dans le brouillon
GFX2_GLOBAL short Spare_magnifier_offset_X;// Offset horizontal de la fen�tre de zoom dans le brouillon
GFX2_GLOBAL short Spare_magnifier_offset_Y;// Offset vertical   de la fen�tre de zoom dans le brouillon

GFX2_GLOBAL byte Mask_color_to_copy[256]; // Tableau des couleurs � copier vers le brouillon

  // Sauvegarde de l'image:

GFX2_GLOBAL byte * Screen_backup;     // Sauvegarde de l'�cran virtuel courant
GFX2_GLOBAL T_List_of_pages * Main_backups; // Liste des pages de backup de la page principale
GFX2_GLOBAL T_List_of_pages * Spare_backups; // Liste des pages de backup de la page de brouillon


  // Donn�es sur la brosse:

GFX2_GLOBAL byte * Brush;          // Sprite de la brosse
GFX2_GLOBAL word Brush_offset_X; // Centre horizontal de la brosse
GFX2_GLOBAL word Brush_offset_Y; // Centre vertical de la brosse
GFX2_GLOBAL word Brush_width;    // Largeur de la brosse
GFX2_GLOBAL word Brush_height;    // Hauteur de la brosse

GFX2_GLOBAL char  Brush_file_directory[MAX_PATH_CHARACTERS];// |
GFX2_GLOBAL char  Brush_filename[MAX_PATH_CHARACTERS];       // |
GFX2_GLOBAL byte  Brush_fileformat;                           // |  Infos sur le
GFX2_GLOBAL byte  Brush_format;                                   // |_ s�lecteur de
GFX2_GLOBAL short Brush_fileselector_position;                       // |  fichiers de la
GFX2_GLOBAL short Brush_fileselector_offset;                       // |  brosse.
GFX2_GLOBAL char  Brush_current_directory[256];                  // |
GFX2_GLOBAL char  Brush_comment[COMMENT_SIZE+1];        // |

GFX2_GLOBAL byte  Brush_rotation_center_is_defined; // |  Infos sur le
GFX2_GLOBAL short Brush_rotation_center_X;      // |- centre de rotation
GFX2_GLOBAL short Brush_rotation_center_Y;      // |  de la brosse

  // Donn�es sur le menu

GFX2_GLOBAL byte  Menu_is_visible;        // Le menu est actif � l'�cran
GFX2_GLOBAL word  Menu_Y;       // Ordonn�e o� commence le menu
GFX2_GLOBAL word  Menu_status_Y; // Ordonn�e o� commence le texte dans le menu
GFX2_GLOBAL byte  Menu_factor_X;      // Facteur de grossissement du menu en X
GFX2_GLOBAL byte  Menu_factor_Y;      // Facteur de grossissement du menu en Y
GFX2_GLOBAL word  Menu_palette_cell_width; // Taille d'une couleur de la palette du menu


  // Donn�es sur la fen�tre de menu

GFX2_GLOBAL byte Window; // Nombre de fenetres empil�es. 0 si pas de fenetre ouverte.

GFX2_GLOBAL byte Menu_is_visible_before_window;  // Le menu �tait visible avant d'ouvir une fen�tre
GFX2_GLOBAL word Menu_Y_before_window; // Ordonn�e du menu avant d'ouvrir une fen�tre
GFX2_GLOBAL byte Paintbrush_hidden_before_window;// Le pinceau �tatit d�j� cach� avant l'ouverture de la fenetre?

GFX2_GLOBAL word Window_stack_pos_X[8];   // Position du bord gauche de la fen�tre dans l'�cran
#define Window_pos_X Window_stack_pos_X[Window-1]

GFX2_GLOBAL word Window_stack_pos_Y[8];   // Position du bord haut   de la fen�tre dans l'�cran
#define Window_pos_Y Window_stack_pos_Y[Window-1]

GFX2_GLOBAL word Window_stack_width[8]; // Largeur de la fen�tre
#define Window_width Window_stack_width[Window-1]

GFX2_GLOBAL word Window_stack_height[8]; // Hauteur de la fen�tre
#define Window_height Window_stack_height[Window-1]

GFX2_GLOBAL word Window_stack_nb_buttons[8];
#define Window_nb_buttons Window_stack_nb_buttons[Window-1]

GFX2_GLOBAL T_Normal_button   * Window_stack_normal_button_list[8];
#define Window_normal_button_list Window_stack_normal_button_list[Window-1]

GFX2_GLOBAL T_Palette_button  * Window_stack_palette_button_list[8];
#define Window_palette_button_list Window_stack_palette_button_list[Window-1]

GFX2_GLOBAL T_Scroller_button * Window_stack_scroller_button_list[8];
#define Window_scroller_button_list Window_stack_scroller_button_list[Window-1]

GFX2_GLOBAL T_Special_button  * Window_stack_special_button_list[8];
#define Window_special_button_list Window_stack_special_button_list[Window-1]

GFX2_GLOBAL T_Dropdown_button  * Window_stack_dropdown_button_list[8];
#define Window_dropdown_button_list Window_stack_dropdown_button_list[Window-1]


GFX2_GLOBAL int Window_stack_attribute1[8];
#define Window_attribute1 Window_stack_attribute1[Window-1]

// Cette variable sert � stocker 2 informations:
// - Sur le click d'un scroller, la position active (0-n)
// - Sur le click d'une palette, le numero de couleur (0-255).
GFX2_GLOBAL int Window_stack_attribute2[8];
#define Window_attribute2 Window_stack_attribute2[Window-1]




// D�finition des boutons ////////////////////////////////////////////////////

GFX2_GLOBAL struct
{
  // Informations sur l'aspect du bouton (graphisme):
  word            X_offset;        // D�calage par rapport � la gauche du menu
  word            Y_offset;        // D�calage par rapport au haut du menu
  word            Width;           // Largeur du bouton
  word            Height;           // Hauteur du bouton
  byte            Pressed;           // Le bouton est enfonc�
  byte            Shape;             // Forme du bouton

  // Information sur les clicks de la souris:
  Func_action Gauche;            // Action d�clench�e par un click gauche sur le bouton
  Func_action Droite;            // Action d�clench�e par un click droit  sur le bouton
  word            Left_shortcut[2];  // Shortcut clavier �quivalent � un click gauche sur le bouton
  word            Right_shortcut[2];  // Shortcut clavier �quivalent � un click droit  sur le bouton

  // Informations sur le d�senclenchement du bouton g�r� par le moteur:
  Func_action Desenclencher;     // Action appel�e lors du d�senclenchement du bouton
  byte            Famille;           // Ensemble de boutons auquel celui-ci appartient

} Button[NB_BUTTONS];



// Informations sur les diff�rents modes de dessin

GFX2_GLOBAL Func_effect Effect_function;

GFX2_GLOBAL byte * FX_feedback_screen;

GFX2_GLOBAL byte Exclude_color[256]; // Couleurs � exclure pour Best_color

  // Mode smear:

GFX2_GLOBAL byte  Smear_mode;    // Le mode smear est enclench�
GFX2_GLOBAL byte  Smear_start;   // On vient juste de commencer une op�ration en Smear
GFX2_GLOBAL byte * Smear_brush; // Sprite de la brosse de Smear
GFX2_GLOBAL word  Smear_brush_width; // Largeur de la brosse de Smear
GFX2_GLOBAL word  Smear_brush_height; // Hauteur de la brosse de Smear
GFX2_GLOBAL short Smear_min_X,Smear_max_X,Smear_min_Y,Smear_max_Y; // Bornes de la Brush du smear

  // Mode shade:

GFX2_GLOBAL T_Shade        Shade_list[8]; // Listes de shade
GFX2_GLOBAL byte           Shade_current;   // Num�ro du shade en cours
GFX2_GLOBAL byte *         Shade_table;    // Table de conversion de shade en cours
GFX2_GLOBAL byte           Shade_table_left[256]; // Table de conversion de shade pour un clic gauche
GFX2_GLOBAL byte           Shade_table_right[256]; // Table de conversion de shade pour un clic droit
GFX2_GLOBAL byte           Shade_mode;     // Le mode shade est enclench�

GFX2_GLOBAL byte           Quick_shade_mode; // Le mode quick-shade est enclench�
GFX2_GLOBAL byte           Quick_shade_step; // Pas du mode quick-shade
GFX2_GLOBAL byte           Quick_shade_loop; // Normal / Loop / No sat.

  // Mode stencil:

GFX2_GLOBAL byte Stencil_mode;  // Le mode stencil est enclench�
GFX2_GLOBAL byte Stencil[256];  // Tableau des couleurs prot�g�es

  // Mode grille:

GFX2_GLOBAL byte  Snap_mode;       // Le mode grille est enclench�
GFX2_GLOBAL word Snap_width;    // Largeur entre 2 points de la grille
GFX2_GLOBAL word Snap_height;    // Hauteur entre 2 points de la grille
GFX2_GLOBAL word Snap_offset_X; // Position en X du point le + � gauche
GFX2_GLOBAL word Snap_offset_Y; // Position en Y du point le + en haut

  // Mode trame:

GFX2_GLOBAL byte  Sieve_mode;    // Le mode Sieve est enclench�
GFX2_GLOBAL byte  Sieve[16][16]; // Sprite de la trame
GFX2_GLOBAL word  GFX_sieve_pattern[12][16]; // Trames pr�s�finies (compact�es sur 16*16 bits)
GFX2_GLOBAL short Sieve_width; // Largeur de la trame
GFX2_GLOBAL short Sieve_height; // Hauteur de la trame

  // Mode colorize:

GFX2_GLOBAL byte Colorize_mode;          // Le mode Colorize est enclench�
GFX2_GLOBAL byte Colorize_opacity;       // Intensit� du Colorize
GFX2_GLOBAL byte Colorize_current_mode; // Le type de Colorize en cours (0-2)
GFX2_GLOBAL word Facteur_A_table[256];
GFX2_GLOBAL word Facteur_B_table[256];

  // Mode smooth:

GFX2_GLOBAL byte Smooth_mode;          // Le mode Smooth est enclench�
GFX2_GLOBAL byte Smooth_matrix[3][3]; // La matrice du Smooth actuel

  // Mode Tiling:

GFX2_GLOBAL byte  Tiling_mode;       // Le mode Tiling est enclench�
GFX2_GLOBAL short Tiling_offset_X; // D�calage du tiling en X
GFX2_GLOBAL short Tiling_offset_Y; // D�calage du tiling en Y

  // Mode Mask

GFX2_GLOBAL byte Mask_mode;  // Le mode Masque est enclench�
GFX2_GLOBAL byte Mask_table[256];  // Tableau des couleurs constituant le masque

  // Mode loupe:

GFX2_GLOBAL byte  Main_magnifier_mode;
GFX2_GLOBAL word  Main_magnifier_factor;
GFX2_GLOBAL word  Main_magnifier_height;
GFX2_GLOBAL word  Main_magnifier_width;
GFX2_GLOBAL short Main_magnifier_offset_X;
GFX2_GLOBAL short Main_magnifier_offset_Y;
GFX2_GLOBAL word  * Zoom_factor_table;
GFX2_GLOBAL word  Magnify_table[NB_ZOOM_FACTORS][512];

#ifdef GLOBAL_VARIABLES
  word ZOOM_FACTOR[NB_ZOOM_FACTORS]={2,3,4,5,6,8,10,12,14,16,18,20};
#else
  extern word ZOOM_FACTOR[NB_ZOOM_FACTORS];
#endif

  // Donn�es sur les ellipses et les cercles:

GFX2_GLOBAL long  Ellipse_cursor_X;
GFX2_GLOBAL long  Ellipse_cursor_Y;
GFX2_GLOBAL long  Ellipse_vertical_radius_squared;
GFX2_GLOBAL long  Ellipse_horizontal_radius_squared;
GFX2_GLOBAL qword Ellipse_limit;
GFX2_GLOBAL long  Circle_cursor_X;
GFX2_GLOBAL long  Circle_cursor_Y;
GFX2_GLOBAL long  Circle_limit;

  // Donn�es sur les d�grad�s:

GFX2_GLOBAL short Gradient_lower_bound;   // Plus petite couleur englob�e par le d�grad�
GFX2_GLOBAL short Gradient_upper_bound;   // Plus grande couleur englob�e par le d�grad�
GFX2_GLOBAL int   Gradient_is_inverted;            // Bool�en "Le d�grad� est en r�alit� invers�"
GFX2_GLOBAL long  Gradient_bounds_range;  // = Abs(Gradient_lower_bound-Gradient_upper_bound)+1
GFX2_GLOBAL long  Gradient_total_range;   // Valeur maximum des indices pass�s � la fonction de d�grad� (!!! >0 !!!)
GFX2_GLOBAL long  Gradient_random_factor;  // Facteur de m�lange (1-256+) du d�grad�
GFX2_GLOBAL Func_gradient Gradient_function; // Fonction de traitement du d�grad�, varie selon la m�thode choisie par l'utilisateur.
GFX2_GLOBAL Func_pixel Gradient_pixel; // Redirection de l'affichage

GFX2_GLOBAL T_Gradient_array Gradient_array[16]; // Donn�es de tous les d�grad�s
GFX2_GLOBAL byte Current_gradient;             // index du tableau correspondant au d�grad� courant



  // Donn�es sur le Spray:

GFX2_GLOBAL byte  Airbrush_mode;            // Mode Mono(1) ou Multicolore(0)
GFX2_GLOBAL short Airbrush_size;            // Diam�tre du spray en pixels
GFX2_GLOBAL byte  Airbrush_delay;           // D�lai en VBLs entre 2 "pschiitt"
GFX2_GLOBAL byte  Airbrush_mono_flow;       // Nombre de pixels qui sortent en m�me temps en mono
GFX2_GLOBAL byte  Airbrush_multi_flow[256]; // Idem pour chaque couleur


  // Donn�es diverses sur le programme:

GFX2_GLOBAL byte Quitting;
GFX2_GLOBAL char Initial_directory[256];          // R�pertoire � partir duquel � �t� lanc� le programme
GFX2_GLOBAL char Repertoire_des_donnees[256];      // R�pertoire contenant les fichiers lus (interface graphique, etc)
GFX2_GLOBAL char Config_directory[256]; // R�pertoire contenant les fichiers .ini et .cfg
GFX2_GLOBAL byte Fore_color;
GFX2_GLOBAL byte Back_color;
GFX2_GLOBAL byte Selected_operation;
GFX2_GLOBAL byte Selected_curve_mode;
GFX2_GLOBAL byte Selected_line_mode;
GFX2_GLOBAL byte First_color_in_palette;
GFX2_GLOBAL byte File_in_command_line;
GFX2_GLOBAL byte Resolution_in_command_line; // utilis�e uniquement si la variable pr�c�dente est � 1

// Les diff�rents sprites:

GFX2_GLOBAL byte GFX_menu_block[MENU_HEIGHT][MENU_WIDTH];
GFX2_GLOBAL byte GFX_menu_sprite[NB_MENU_SPRITES][MENU_SPRITE_HEIGHT][MENU_SPRITE_WIDTH];
GFX2_GLOBAL byte GFX_effect_sprite[NB_EFFECTS_SPRITES][MENU_SPRITE_HEIGHT][MENU_SPRITE_WIDTH];

GFX2_GLOBAL byte * GFX_logo_grafx2;

GFX2_GLOBAL byte GFX_system_font[256*8*8];
GFX2_GLOBAL byte GFX_fun_font    [256*8*8];
GFX2_GLOBAL byte GFX_help_font_norm [256][6][8];
GFX2_GLOBAL byte GFX_bold_font [256][6][8];
// 12
// 34
GFX2_GLOBAL byte GFX_help_font_t1 [64][6][8];
GFX2_GLOBAL byte GFX_help_font_t2 [64][6][8];
GFX2_GLOBAL byte GFX_help_font_t3 [64][6][8];
GFX2_GLOBAL byte GFX_help_font_t4 [64][6][8];
GFX2_GLOBAL byte * Font;

  // Les donn�es de l'aide:

GFX2_GLOBAL byte Current_help_section;  // index de la table d'aide en cours de consultation
GFX2_GLOBAL word Help_position; // Num�ro de la ligne d'aide en cours de consultation

  // Donn�es sur les op�rations

GFX2_GLOBAL word Operation_before_interrupt; // N� de l'op�ration en cours avant l'utilisation d'une interruption
GFX2_GLOBAL word Current_operation;           // N� de l'op�ration en cours
GFX2_GLOBAL word Operation_stack[OPERATION_STACK_SIZE]; // Pile simplifi�e
GFX2_GLOBAL byte Operation_stack_size;    // Taille effective de la pile (0=vide)
GFX2_GLOBAL byte Operation_in_magnifier;     // Indique si l'op�ration a commenc� dans la partie Zoom�e ou non

GFX2_GLOBAL short Colorpicker_color;
GFX2_GLOBAL short Colorpicker_X;
GFX2_GLOBAL short Colorpicker_Y;


#ifdef GLOBAL_VARIABLES
  byte CURSOR_FOR_OPERATION[NB_OPERATIONS]=
  {
    CURSOR_SHAPE_TARGET            , // Dessin � la main continu
    CURSOR_SHAPE_TARGET            , // Dessin � la main discontinu
    CURSOR_SHAPE_TARGET            , // Dessin � la main point par point
    CURSOR_SHAPE_TARGET            , // Contour fill
    CURSOR_SHAPE_TARGET            , // Lignes
    CURSOR_SHAPE_TARGET            , // Lignes reli�es
    CURSOR_SHAPE_TARGET            , // Lignes centr�es
    CURSOR_SHAPE_XOR_TARGET        , // Rectangle vide
    CURSOR_SHAPE_XOR_TARGET        , // Rectangle plein
    CURSOR_SHAPE_TARGET            , // Cercles vides
    CURSOR_SHAPE_TARGET            , // Cercles pleins
    CURSOR_SHAPE_TARGET            , // Ellipses vides
    CURSOR_SHAPE_TARGET            , // Ellipses pleines
    CURSOR_SHAPE_TARGET            , // Fill
    CURSOR_SHAPE_TARGET            , // Replace
    CURSOR_SHAPE_XOR_TARGET        , // Prise de brosse rectangulaire
    CURSOR_SHAPE_TARGET            , // Prise d'une brosse multiforme
    CURSOR_SHAPE_COLORPICKER    , // R�cup�ration d'une couleur
    CURSOR_SHAPE_XOR_RECTANGLE    , // Positionnement de la fen�tre de loupe
    CURSOR_SHAPE_TARGET            , // Courbe � 3 points
    CURSOR_SHAPE_TARGET            , // Courbe � 4 points
    CURSOR_SHAPE_TARGET            , // Spray
    CURSOR_SHAPE_TARGET            , // Polygone
    CURSOR_SHAPE_TARGET            , // Polyforme
    CURSOR_SHAPE_TARGET            , // Polyfill
    CURSOR_SHAPE_TARGET            , // Polyforme rempli
    CURSOR_SHAPE_MULTIDIRECTIONNAL, // Scroll
    CURSOR_SHAPE_TARGET            , // Cercles d�grad�s
    CURSOR_SHAPE_TARGET            , // Ellipses d�grad�es
    CURSOR_SHAPE_XOR_ROTATION       , // Faire tourner brosse
    CURSOR_SHAPE_XOR_TARGET        , // Etirer brosse
    CURSOR_SHAPE_TARGET            , // Deformer brosse
    CURSOR_SHAPE_XOR_TARGET        , // Rectangle degrade
  };
#else
  extern byte CURSOR_FOR_OPERATION[NB_OPERATIONS];
#endif


  // Proc�dures � appeler: Op�ration,Mouse_K,State de la pile

GFX2_GLOBAL struct
{
  byte Hide_cursor; // Bool�en "il faut effacer le curseur pour l'op�ra."
  Func_action Action;                                   // Action appel�e
} Operation[NB_OPERATIONS][3][OPERATION_STACK_SIZE];



// Informations sur les lecteurs et autres images
GFX2_GLOBAL byte GFX_icon_sprite[NB_ICON_SPRITES][ICON_SPRITE_HEIGHT][ICON_SPRITE_WIDTH];


// -- Section des informations sur les formats de fichiers ------------------

  // Comptage du nb d'�l�ments dans la liste:
GFX2_GLOBAL short Filelist_nb_elements;
GFX2_GLOBAL short Filelist_nb_files;
GFX2_GLOBAL short Filelist_nb_directories;
  // T�te de la liste cha�n�e:
GFX2_GLOBAL T_Fileselector_item * Filelist;

// ------------------- Inititialisation des formats connus -------------------

void Do_nothing(void);

GFX2_GLOBAL signed char File_error; // 0: op�ration I/O OK
                                   // 1: Error d�s le d�but de l'op�ration
                                   // 2: Error durant l'op�ration => donn�es modifi�es
                                   //-1: Interruption du chargement d'une preview

GFX2_GLOBAL int Line_number_in_INI_file;

GFX2_GLOBAL Func_pixel Pixel_load_function;
GFX2_GLOBAL Func_read   Read_pixel_old;

/********************
 * Sp�cifique � SDL *
 ********************/
GFX2_GLOBAL SDL_Surface * Screen_SDL;
GFX2_GLOBAL SDL_Joystick* Joystick;

#define KEY_NONE          0
#define KEY_MOUSEMIDDLE     (SDLK_LAST+1)
#define KEY_MOUSEWHEELUP    (SDLK_LAST+2)
#define KEY_MOUSEWHEELDOWN  (SDLK_LAST+3)
#define KEY_JOYBUTTON          (SDLK_LAST+4)

#ifdef __gp2x__
  #define KEY_ESC (KEY_JOYBUTTON+GP2X_BUTTON_X)
#else
  #define KEY_ESC SDLK_ESCAPE
#endif

#endif
