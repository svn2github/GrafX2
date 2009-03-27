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
#ifndef _CONST_H_
#define _CONST_H_



// D�claration des constantes ////////////////////////////////////////////////

#define M_2PI 6.28318530717958647692528676656

#define PERCENTAGE_VERSION     "98.0%" // Libell� du pourcentage de la version �
#define VERSION1                  2     // |
#define VERSION2                  0     // |_ Num�ro de version d�coup� en
#define BETA1                     98    // |  plusieurs parties => 2.0 �95.5%
#define BETA2                     0     // |  (utilis� pour le fichier de config)
#define ALPHA_BETA                "�"   // Type de la version "�" ou "�"
#define MAX_VIDEO_MODES           100   // Nombre de modes vid�o maxi
#define NB_BUTTONS                38    // Nombre de boutons � g�rer
#define NB_SHORTCUTS                134   // Nombre de combinaisons de touches
#define NB_SPECIAL_SHORTCUTS      72    // Nombre de touches sp�ciales
#define NB_OPERATIONS             33    // Nombre d'op�rations g�r�es par le moteur
#define NB_ZOOM_FACTORS       12    // Nombre de facteurs de zoom
#define MENU_WIDTH              254   // Largeur du menu en taille r�elle
#define MENU_HEIGHT              44    // Hauteur du menu en taille r�elle
#define NB_CURSOR_SPRITES        8     // Nombre de sprites de curseur
#define CURSOR_SPRITE_WIDTH    15    // Largeur d'un sprite de curseur en pixels
#define CURSOR_SPRITE_HEIGHT    15    // Hauteur d'un sprite de curseur en pixels
#define NB_EFFECTS_SPRITES         9     // Nombre de sprites d'effets
#define NB_MENU_SPRITES           18    // Nombre de sprites de menu
#define MENU_SPRITE_WIDTH       14    // Largeur d'un sprite de menu en pixels
#define MENU_SPRITE_HEIGHT       14    // Hauteur d'un sprite de menu en pixels
#define PAINTBRUSH_WIDTH           16    // Largeur d'un sprite de pinceau pr�d�fini
#define PAINTBRUSH_HEIGHT           16    // Hauteur d'un sprite de pinceau pr�d�fini
#define MAX_PAINTBRUSH_SIZE       127   // Taille maxi des pinceaux
#define NB_ICON_SPRITES         7     // Nombre de sprites de drives
#define ICON_SPRITE_WIDTH      8     // Largeur d'un sprite de drive en pixels
#define ICON_SPRITE_HEIGHT      8     // Hauteur d'un sprite de drive en pixels
#define NB_PAINTBRUSH_SPRITES        48    // Nombre de sprites de pinceau
#define NB_PRESET_SIEVE     12    // Nombre de trames pr�d�finies
#define OPERATION_STACK_SIZE    16    // Nombre maximum d'�l�ments utilis�s par les op�rations
#define MAX_DISPLAYABLE_PATH          37    // Taille maximum affichable du r�pertoire courant dans les fen�tres du fileselect
#define COMMENT_SIZE        32    // Taille maxi des commentaires pour le PKM
#define NB_MAX_PAGES_UNDO         99    // Nombre maximal de pages d'undo
#define DEFAULT_ZOOM_FACTOR 4    // Facteur de zoom initial
#define MAX_PATH_CHARACTERS     260   // Le nombre de caract�res maxi pour un nom de fichier avec chemin complet
#define NB_BOOKMARKS               4    // Nombre de boutons "signet" dans l'ecran Save/Load
// Caract�res pr�sents dans les deux fontes
#define RIGHT_TRIANGLE_CHARACTER  16
#define LEFT_TRIANGLE_CHARACTER 17
#define ELLIPSIS_CHARACTER      '�'

// On impose � l'allocation dynamique des pages de backups de conserver un
// minimum de 256 Ko pour que le reste du programme puisse continuer �
// fonctionner.
#define MINIMAL_MEMORY_TO_RESERVE (256*1024)

#define LEFT_SIDE                  1     // Indique une direction (ou click) � gauche
#define RIGHT_SIDE                  2     // Indique une direction (ou click) � droite

#define SEPARATOR_WIDTH       6     // [ ۱�� ]
#define INITIAL_SEPARATOR_PROPORTION          0.3   // proportion de la zone non-zoom�e en largeur par rapport � l'�cran
#define NB_ZOOMED_PIXELS_MIN      4     // Nombre minimal de pixels zoom�s en largeur (Note: En dessous de 4, on ne peut plus scroller!)

#if defined(__MORPHOS__) || defined(__amigaos4__) || defined(__amigaos__)
   #define PARENT_DIR "/"
#else
   #define PARENT_DIR ".."
#endif

  // Les diff�rents formats de fichiers:

#ifndef __no_pnglib__
#define NB_KNOWN_FORMATS         13    // Nombre de formats connus (devrait �tre la valeur maximale de NB_FORMATS_LOAD et NB_FORMATS_SAVE, mais plus g�n�ralement: Card({NB_FORMATS_LOAD} UNION {NB_FORMATS_SAVE}))
#define NB_FORMATS_LOAD           13    // Nombre de formats que l'on sait charger
#define NB_FORMATS_SAVE           13    // Nombre de formats que l'on sait sauver
#else
// Without pnglib
#define NB_KNOWN_FORMATS         12    // Nombre de formats connus (devrait �tre la valeur maximale de NB_FORMATS_LOAD et NB_FORMATS_SAVE, mais plus g�n�ralement: Card({NB_FORMATS_LOAD} UNION {NB_FORMATS_SAVE}))
#define NB_FORMATS_LOAD           12    // Nombre de formats que l'on sait charger
#define NB_FORMATS_SAVE           12    // Nombre de formats que l'on sait sauver
#endif

enum FILE_FORMATS
{
  FORMAT_PKM=1,      // |
  FORMAT_LBM,        // |
  FORMAT_GIF,        // |    Il faudra penser � r�ordonner
  FORMAT_BMP,        // |  les donn�es sur les formats dans
  FORMAT_PCX,        // |  GLOBAL.H si on modifie ces cons-
  FORMAT_IMG,        // |_ tantes.
  FORMAT_SCx,        // |
  FORMAT_PI1,        // |    Elles repr�sentent l'indice o�
  FORMAT_PC1,        // |  l'on doit aller piocher ces
  FORMAT_CEL,        // |  donn�es.
  FORMAT_KCF,        // |
  FORMAT_PAL,        // |
  FORMAT_PNG         // |
};

#define DEFAULT_FILEFORMAT    FORMAT_GIF // Format par d�faut (ah bon? oh!)

  // Les diff�rentes erreurs:

enum ERROR_CODES
{
  // 0 = Flash rouge de l'�cran, erreur non critique
  ERROR_GUI_MISSING=1,          // Le fichier gfx2gui.gif est absent
  ERROR_GUI_CORRUPTED,          // Mauvais fichier gfx2gui.gif
  ERROR_INI_MISSING,            // Le fichier gfx2def.ini est absent
  ERROR_CFG_MISSING,            // Le fichier GFX2.CFG est absent
  ERROR_CFG_CORRUPTED,          // Mauvais fichier GFX2.CFG
  ERROR_CFG_OLD,            // Ancienne version du fichier GFX2.CFG
  ERROR_MEMORY,               // Plus de m�moire
  ERROR_COMMAND_LINE,        // Error sur la ligne de commande
  ERROR_MOUSE_DRIVER,         // Pas de driver souris install�
  ERROR_FORBIDDEN_MODE,         // Mode demand� sur la ligne de commande interdit (coch� en noir)
  ERROR_SAVING_CFG,        // Error en �criture pour GFX2.CFG
  ERROR_MISSING_DIRECTORY,    // Le r�pertoire de lancement n'existe plus
  ERROR_INI_CORRUPTED,          // Le fichier GFX2.INI est corrompu
  ERROR_SAVING_INI,        // Le fichier GFX2.INI n'est pas inscriptible
  ERROR_SORRY_SORRY_SORRY      // On le refera plus, promis (erreur d'allocation de page qui ne devrait JAMAIS se produire)
};

  // Les diff�rents types de pixels

enum PIXEL_RATIO
{
    PIXEL_SIMPLE,
    PIXEL_WIDE,
    PIXEL_TALL,
    PIXEL_DOUBLE
};

  // Les diff�rentes cat�gories de bouton:

enum FAMILY_OF_BUTTONS
{
  FAMILY_TOOL=1,          // Outils de dessin (exemple : Freehand draw)
  FAMILY_INTERRUPTION,     // Op�ration �ph�m�re (exemple : changement de brosse) > Interruption de l'op�ration courante pour faire autre chose, puis on revient
  FAMILY_INSTANT,       // Pif paf (exemple : changement de couleur) > �a sera fini d�s qu'on lache le bouton, pas d'utilisation de la pile d'op�rations
  FAMILY_TOOLBAR,          // Cache/Montre la barre d'outils
  FAMILY_EFFECTS            // Effets
};

  // Les diff�rentes formes de bouton:

enum BUTTON_SHAPES
{
  BUTTON_SHAPE_NO_FRAME,            // Ex: la palette
  BUTTON_SHAPE_RECTANGLE,             // Ex: la plupart
  BUTTON_SHAPE_TRIANGLE_TOP_LEFT,  // Ex: Rectangles vides
  BUTTON_SHAPE_TRIANGLE_BOTTOM_RIGHT    // Ex: Rectangles pleins
};

  // Les diff�rentes formes de curseur:

enum CURSOR_SHAPES
{
  CURSOR_SHAPE_ARROW,
  CURSOR_SHAPE_TARGET,              // Utilise le pinceau
  CURSOR_SHAPE_COLORPICKER,      // Utilise le pinceau
  CURSOR_SHAPE_HOURGLASS,
  CURSOR_SHAPE_MULTIDIRECTIONNAL,
  CURSOR_SHAPE_HORIZONTAL,
  CURSOR_SHAPE_THIN_TARGET,         // Utilise le pinceau
  CURSOR_SHAPE_THIN_COLORPICKER, // Utilise le pinceau
  CURSOR_SHAPE_XOR_TARGET,
  CURSOR_SHAPE_XOR_RECTANGLE,
  CURSOR_SHAPE_XOR_ROTATION
};

  // Les diff�rentes formes de pinceaux (les types de pinceaux doivent �tre au d�but)

enum PAINTBRUSH_SHAPES
{
  PAINTBRUSH_SHAPE_ROUND,
  PAINTBRUSH_SHAPE_SQUARE,
  PAINTBRUSH_SHAPE_HORIZONTAL_BAR,
  PAINTBRUSH_SHAPE_VERTICAL_BAR,
  PAINTBRUSH_SHAPE_SLASH,
  PAINTBRUSH_SHAPE_ANTISLASH,
  PAINTBRUSH_SHAPE_RANDOM,
  PAINTBRUSH_SHAPE_CROSS,
  PAINTBRUSH_SHAPE_PLUS,
  PAINTBRUSH_SHAPE_DIAMOND,
  PAINTBRUSH_SHAPE_SIEVE_ROUND,
  PAINTBRUSH_SHAPE_SIEVE_SQUARE,
  PAINTBRUSH_SHAPE_MISC,// Ce doit �tre le dernier des types de pinceaux, comme �a il indique le nombre de types de pinceaux (-1)
  PAINTBRUSH_SHAPE_POINT, // Utilis� pour r�duire de pinceau � 1 point dans certaines op�rations
  PAINTBRUSH_SHAPE_COLOR_BRUSH,
  PAINTBRUSH_SHAPE_MONO_BRUSH
};

  // Les diff�rents �tats de bouton:

#define BUTTON_RELEASED 0
#define BUTTON_PRESSED 1

  // Les diff�rents modes de Shade

enum SHADE_MODES
{
  SHADE_MODE_NORMAL,
  SHADE_MODE_LOOP,
  SHADE_MODE_NOSAT
};

  // Les diff�rents chunks du fichier .CFG

enum CHUNKS_CFG
{
  CHUNK_KEYS            = 0,
  CHUNK_VIDEO_MODES        = 1,
  CHUNK_SHADE              = 2,
  CHUNK_MASK             = 3,
  CHUNK_STENCIL            = 4,
  CHUNK_GRADIENTS           = 5,
  CHUNK_SMOOTH             = 6,
  CHUNK_EXCLUDE_COLORS     = 7,
  CHUNK_QUICK_SHADE        = 8,
  CHUNK_GRID             = 9,
  CHUNK_MAX 
};

  // Les diff�rents types de lecteurs:

enum ICON_TYPES
{
  ICON_FLOPPY_3_5,   // 0: Diskette 3"�
  ICON_FLOPPY_5_25,  // 1: Diskette 5"�
  ICON_HDD,          // 2: HDD
  ICON_CDROM,        // 3: CD-ROM
  ICON_NETWORK,      // 4: Logique (r�seau?)
};

  // Les diff�rents boutons:

enum BUTTON_NUMBERS
{
  BUTTON_PAINTBRUSHES,
  BUTTON_ADJUST,
  BUTTON_DRAW,
  BUTTON_CURVES,
  BUTTON_LINES,
  BUTTON_AIRBRUSH,
  BUTTON_FLOODFILL,
  BUTTON_POLYGONS,
  BUTTON_POLYFILL,
  BUTTON_RECTANGLES,
  BUTTON_FILLRECT,
  BUTTON_CIRCLES,
  BUTTON_FILLCIRC,
  BUTTON_GRADRECT,
  BUTTON_GRADMENU,
  BUTTON_SPHERES,
  BUTTON_BRUSH,
  BUTTON_POLYBRUSH,
  BUTTON_BRUSH_EFFECTS,
  BUTTON_EFFECTS,
  BUTTON_TEXT,
  BUTTON_MAGNIFIER,
  BUTTON_COLORPICKER,
  BUTTON_RESOL,
  BUTTON_PAGE,
  BUTTON_SAVE,
  BUTTON_LOAD,
  BUTTON_SETTINGS,
  BUTTON_CLEAR,
  BUTTON_HELP,
  BUTTON_UNDO,
  BUTTON_KILL,
  BUTTON_QUIT,
  BUTTON_PALETTE,
  BUTTON_PAL_LEFT,
  BUTTON_PAL_RIGHT,
  BUTTON_CHOOSE_COL,
  BUTTON_HIDE
};

  // Les actions des touches sp�ciales

enum SPECIAL_ACTIONS
{
  SPECIAL_MOUSE_UP,                
  SPECIAL_MOUSE_DOWN,              
  SPECIAL_MOUSE_LEFT,              
  SPECIAL_MOUSE_RIGHT,             
  SPECIAL_CLICK_LEFT,               
  SPECIAL_CLICK_RIGHT,             
  SPECIAL_NEXT_FORECOLOR,         
  SPECIAL_PREVIOUS_FORECOLOR,      
  SPECIAL_NEXT_BACKCOLOR,          
  SPECIAL_PREVIOUS_BACKCOLOR,      
  SPECIAL_SMALLER_PAINTBRUSH,         
  SPECIAL_BIGGER_PAINTBRUSH,          
  SPECIAL_NEXT_USER_FORECOLOR,     
  SPECIAL_PREVIOUS_USER_FORECOLOR, 
  SPECIAL_NEXT_USER_BACKCOLOR,     
  SPECIAL_PREVIOUS_USER_BACKCOLOR, 
  SPECIAL_SCROLL_UP,
  SPECIAL_SCROLL_DOWN,
  SPECIAL_SCROLL_LEFT,
  SPECIAL_SCROLL_RIGHT,
  SPECIAL_SCROLL_UP_FAST,
  SPECIAL_SCROLL_DOWN_FAST,
  SPECIAL_SCROLL_LEFT_FAST,
  SPECIAL_SCROLL_RIGHT_FAST,
  SPECIAL_SCROLL_UP_SLOW,
  SPECIAL_SCROLL_DOWN_SLOW,
  SPECIAL_SCROLL_LEFT_SLOW,
  SPECIAL_SCROLL_RIGHT_SLOW,
  SPECIAL_SHOW_HIDE_CURSOR,
  SPECIAL_DOT_PAINTBRUSH,
  SPECIAL_CONTINUOUS_DRAW,
  SPECIAL_FLIP_X,
  SPECIAL_FLIP_Y,
  SPECIAL_ROTATE_90,
  SPECIAL_ROTATE_180,
  SPECIAL_STRETCH,
  SPECIAL_DISTORT,
  SPECIAL_OUTLINE,
  SPECIAL_NIBBLE,
  SPECIAL_GET_BRUSH_COLORS,
  SPECIAL_RECOLORIZE_BRUSH,
  SPECIAL_ROTATE_ANY_ANGLE,
  SPECIAL_LOAD_BRUSH,
  SPECIAL_SAVE_BRUSH,
  SPECIAL_INVERT_SIEVE,
  SPECIAL_ZOOM_IN,
  SPECIAL_ZOOM_OUT,
  SPECIAL_CENTER_ATTACHMENT,
  SPECIAL_TOP_LEFT_ATTACHMENT,
  SPECIAL_TOP_RIGHT_ATTACHMENT,
  SPECIAL_BOTTOM_LEFT_ATTACHMENT,
  SPECIAL_BOTTOM_RIGHT_ATTACHMENT,
  SPECIAL_EXCLUDE_COLORS_MENU,
  SPECIAL_SHADE_MODE,       // |
  SPECIAL_SHADE_MENU,       // |
  SPECIAL_QUICK_SHADE_MODE, // |
  SPECIAL_QUICK_SHADE_MENU, // |
  SPECIAL_STENCIL_MODE,     // |
  SPECIAL_STENCIL_MENU,     // |
  SPECIAL_MASK_MODE,        // |  Il faut que le premier effet soit
  SPECIAL_MASK_MENU,        // |  SPECIAL_SHADE_MODE, et que le
  SPECIAL_GRID_MODE,        // |  dernier soit SPECIAL_TILING_MENU,
  SPECIAL_GRID_MENU,        // |_ et que seuls des effets soient
  SPECIAL_SIEVE_MODE,       // |  d�finis entre ces deux l� car
  SPECIAL_SIEVE_MENU,       // |  des tests sur cet intervalle sont
  SPECIAL_COLORIZE_MODE,    // |  faits dans le moteur.
  SPECIAL_COLORIZE_MENU,    // |
  SPECIAL_SMOOTH_MODE,      // |
  SPECIAL_SMOOTH_MENU,      // |
  SPECIAL_SMEAR_MODE,       // |
  SPECIAL_TILING_MODE,      // |
  SPECIAL_TILING_MENU       // |
};

  // D�finition des op�rations:

enum OPERATIONS
{
  OPERATION_CONTINUOUS_DRAW,     // Dessin � la main continu
  OPERATION_DISCONTINUOUS_DRAW,  // Dessin � la main discontinu
  OPERATION_POINT_DRAW,       // Dessin � la main point par point
  OPERATION_FILLED_CONTOUR,     // Contour rempli
  OPERATION_LINE,              // Lignes
  OPERATION_K_LIGNE,            // Lignes reli�es
  OPERATION_CENTERED_LINES,    // Lignes concentriques
  OPERATION_EMPTY_RECTANGLE,     // Rectangle vide
  OPERATION_FILLED_RECTANGLE,    // Rectangle plein
  OPERATION_EMPTY_CIRCLE,        // Cercle vide
  OPERATION_FILLED_CIRCLE,       // Cercle plein
  OPERATION_EMPTY_ELLIPSE,       // Ellipse vide
  OPERATION_FILLED_ELLIPSE,     // Ellipse pleine
  OPERATION_FILL,               // Fill
  OPERATION_REPLACE,          // Replace couleur
  OPERATION_GRAB_BRUSH,       // Prise de brosse rectangulaire
  OPERATION_POLYBRUSH,         // Prise d'une brosse multiforme
  OPERATION_COLORPICK,            // R�cup�ration d'une couleur
  OPERATION_MAGNIFY,              // Positionnement de la fen�tre de loupe
  OPERATION_3_POINTS_CURVE,    // Courbe � 3 points
  OPERATION_4_POINTS_CURVE,    // Courbe � 4 points
  OPERATION_AIRBRUSH,              // Spray
  OPERATION_POLYGON,           // Polygone
  OPERATION_POLYFORM,           // Polyforme
  OPERATION_POLYFILL,           // Polyfill
  OPERATION_FILLED_POLYFORM,    // Polyforme rempli
  OPERATION_SCROLL,             // Scroll
  OPERATION_GRAD_CIRCLE,     // Cercles d�grad�s
  OPERATION_GRAD_ELLIPSE,   // Ellipses d�grad�es
  OPERATION_ROTATE_BRUSH,     // Faire tourner brosse
  OPERATION_STRETCH_BRUSH,      // Etirer brosse
  OPERATION_DISTORT_BRUSH,    // Deformer brosse
  OPERATION_GRAD_RECTANGLE,  // Rectangle d�grad�

  OPERATION_NONE
};



#endif
