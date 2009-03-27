/*  Grafx2 - The Ultimate 256-color bitmap paint program

    Copyright 2008 Yves Rizoud
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
#ifndef _STRUCT_H_
#define _STRUCT_H_

#if defined(__BEOS__)
    #include <inttypes.h>
#else
    #include <stdint.h>
#endif

#include "const.h"

// D�claration des types de base /////////////////////////////////////////////

#define byte  uint8_t
#define word  uint16_t
#define dword uint32_t
#define qword uint64_t

typedef void (* Func_action)    (void);
typedef void (* Func_pixel) (word,word,byte);
typedef byte (* Func_read)   (word,word);
typedef void (* Func_clear)  (byte);
typedef void (* Func_display)   (word,word,word);
typedef byte (* Func_effect)     (word,word,byte);
typedef void (* Func_block)     (word,word,word,word,byte);
typedef void (* Func_line_XOR) (word,word,word);
typedef void (* Func_display_brush_color) (word,word,word,word,word,word,byte,word);
typedef void (* Func_display_brush_mono)  (word,word,word,word,word,word,byte,byte,word);
typedef void (* Func_gradient)   (long,short,short);
typedef void (* Func_remap)     (word,word,word,word,byte *);
typedef void (* Func_procsline) (word,word,word,byte *);
typedef void (* Func_display_zoom) (word,word,word,byte *);
typedef void (* Func_display_brush_color_zoom) (word,word,word,word,word,word,byte,word,byte *);
typedef void (* Func_display_brush_mono_zoom)  (word,word,word,word,word,word,byte,byte,word,byte *);
typedef void (* Func_draw_brush) (byte *,word,word,word,word,word,word,byte,word);

typedef struct
{
  byte R;
  byte G;
  byte B;
}__attribute__ ((__packed__)) T_Components, T_Palette[256];

typedef struct T_Normal_button
{
  short Number;
  word Pos_X;
  word Pos_Y;
  word Width;
  word Height;
  byte Clickable;
  byte Repeatable;
  word Shortcut;
  struct T_Normal_button * Next;
} T_Normal_button;

typedef struct T_Palette_button
{
  short Number;
  word Pos_X;
  word Pos_Y;
  struct T_Palette_button * Next;
} T_Palette_button;

typedef struct T_Scroller_button
{
  short Number;
  word Pos_X;
  word Pos_Y;
  word Height;
  word Nb_elements;
  word Nb_visibles;
  word Position;
  word Cursor_height;
  struct T_Scroller_button * Next;
} T_Scroller_button;

typedef struct T_Special_button
{
  short Number;
  word Pos_X;
  word Pos_Y;
  word Width;
  word Height;
  struct T_Special_button * Next;
} T_Special_button;

typedef struct T_Dropdown_choice
{
  short Number;
  const char * Label;
  struct T_Dropdown_choice * Next;
} T_Dropdown_choice;

typedef struct T_Dropdown_button
{
  short Number;
  word Pos_X;
  word Pos_Y;
  word Width;
  word Height;
  byte Display_choice;  // The selected item's label is printed in the dropdown area
  byte Display_centered; // Center labels (otherwise, align left)
  byte Display_arrow; // Display a "down" arrow box in top right
  byte Active_button; // Mouse button: LEFT_SIDE || RIGHT_SIDE || (LEFT_SIDE|RIGHT_SIDE)
  word Dropdown_width; // 0 for "same as control"
  T_Dropdown_choice * First_item;
  struct T_Dropdown_button * Next;
} T_Dropdown_button;

// D�claration du type d'�l�ment qu'on va m�moriser dans la liste:
typedef struct T_Fileselector_item
{
  char Short_name[19]; // Le nom tel qu'affich� dans le fileselector
  char Full_name[256]; // Le nom du fichier ou du r�pertoire
  byte Type;    // Type d'�l�ment : 0 = Fichier, 1 = R�pertoire, 2 = Lecteur

  // donn�es de cha�nage de la liste
  struct T_Fileselector_item * Next;
  struct T_Fileselector_item * Previous;
} T_Fileselector_item;

typedef struct {
  char Line_type;
  char * Text;
  int Line_parameter;
} T_Help_table;

// D�claration d'une section d'aide:
typedef struct
{
  const T_Help_table* Help_table; // Pointeur sur le d�but de la table d'aide
  word Length;
} T_Help_section;

// D�claration d'une info sur un d�grad�
typedef struct
{
  byte Start;     // Premi�re couleur du d�grad�
  byte End;       // Derni�re couleur du d�grad�
  dword Inverse;   // "Le d�grad� va de End � Start" //INT
  dword Mix;   // Valeur de m�lange du d�grad� (0-255) //LONG
  dword Technique; // Technique � utiliser (0-2) //INT
} __attribute__((__packed__)) T_Gradient_array;

// D�claration d'une info de shade
typedef struct
{
  word List[512]; // Liste de couleurs
  byte Step;        // Pas d'incr�mentation/d�cr�mentation
  byte Mode;       // Mode d'utilisation (Normal/Boucle/Non-satur�)
} T_Shade;



// Structure des donn�es dans le fichier de config.

typedef struct
{
  byte State;
  word Width;
  word Height;
} __attribute__((__packed__)) T_Config_video_mode;

typedef struct
{
  char Signature[3];
  byte Version1;
  byte Version2;
  byte Beta1;
  byte Beta2;
} __attribute__((__packed__)) T_Config_header;

typedef struct
{
  byte Number;
  word Size;
} __attribute__((__packed__)) T_Config_chunk;

typedef struct
{
  word Number;
  word Key;
  word Key2;
} __attribute__((__packed__)) T_Config_shortcut_info;

typedef struct
{
  byte Font;
  int  Show_hidden_files;
  int  Show_hidden_directories;
//  int  Show_system_directories;
  byte Display_image_limits;
  byte Cursor;
  byte Maximize_preview;
  byte Auto_set_res;
  byte Coords_rel;
  byte Backup;
  byte Adjust_brush_pick;
  byte Auto_save;
  byte Max_undo_pages;
  byte Mouse_sensitivity_index_x;
  byte Mouse_sensitivity_index_y;
  byte Mouse_fix_factor_X;
  byte Mouse_fix_factor_Y;
  byte Mouse_merge_movement;
  byte Delay_left_click_on_slider;
  byte Delay_right_click_on_slider;
  long Timer_delay;
  T_Components Fav_menu_colors[4];
  int  Nb_max_vertices_per_polygon;
  byte Clear_palette;
  byte Set_resolution_according_to;
  byte Ratio;
  byte Fast_zoom;
  byte Find_file_fast;
  byte Couleurs_separees;
  word Palette_cells_X;
  word Palette_cells_Y;
  byte FX_Feedback;
  byte Safety_colors;
  byte Opening_message;
  byte Clear_with_stencil;
  byte Auto_discontinuous;
  byte Screen_size_in_GIF;
  byte Auto_nb_used;
  byte Default_resolution;
  char *Bookmark_directory[NB_BOOKMARKS]; // independant malloc of adaptive size
  char Bookmark_label[NB_BOOKMARKS][8+1];
} T_Config;

// Structures utilis�es pour les descriptions de pages et de liste de pages.
// Lorsqu'on g�rera les animations, il faudra aussi des listes de listes de
// pages.

// Ces structures sont manipul�es � travers des fonctions de gestion du
// backup dans "graph.c".

typedef struct
{
  byte *    Image;   // Bitmap de l'image
  int       Width; // Largeur du bitmap
  int       Height; // Hauteur du bitmap
  T_Palette Palette; // Palette de l'image

  char      Comment[COMMENT_SIZE+1]; // Commentaire de l'image

  char      File_directory[MAX_PATH_CHARACTERS]; // |_ Nom complet =
  char      Filename[MAX_PATH_CHARACTERS];        // |  File_directory+"\"+Nom_fichier
  byte      File_format;          // Format auquel il faut lire et �crire le fichier

/*
  short     X_offset; // D�calage en X de l'�cran par rapport au d�but de l'image
  short     Y_offset; // D�calage en Y de l'�cran par rapport au d�but de l'image
  short     old_offset_x; // Le m�me avant le passage en mode loupe
  short     old_offset_y; // Le m�me avant le passage en mode loupe

  short     Split; // Position en X du bord gauche du split de la loupe
  short     X_zoom; // (Menu_factor_X) + Position en X du bord droit du split de la loupe
  float     Separator_proportion; // Proportion de la zone non-zoom�e par rapport � l'�cran

  byte      Main_magnifier_mode;       // On est en mode loupe
  word      Main_magnifier_factor;    // Facteur de zoom
  word      Main_magnifier_height;    // Largeur de la fen�tre de zoom
  word      Main_magnifier_width;    // Hauteur de la fen�tre de zoom
  short     Main_magnifier_offset_X; // Offset horizontal de la fen�tre de zoom
  short     Main_magnifier_offset_Y; // Offset vertical   de la fen�tre de zoom
*/
} T_Page;

typedef struct
{
  int      List_size;      // Nb de T_Page dans le vecteur "Pages"
  int      Nb_pages_allocated; // Nb de T_Page d�signant des pages allou�es
  T_Page * Pages;             // Liste de pages (List_size �l�ments)
} T_List_of_pages;



#endif
