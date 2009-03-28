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

//////////////////////////////////////////////////////////////////////////////
///@file global.h
/// This file contains all global variables.
/// They are prefixed by ::GFX2_GLOBAL so they are extern when needed.
//////////////////////////////////////////////////////////////////////////////
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <SDL.h>
#include "struct.h"

// MAIN declares the variables,
// other files only have an extern definition.
#ifdef GLOBAL_VARIABLES
  /// Magic prefix to make all declarations extern, except when included by main.c.
  #define GFX2_GLOBAL
#else
  #define GFX2_GLOBAL extern
#endif

// -- CONFIGURATION variables

/// Current configuration.
GFX2_GLOBAL T_Config Config;

/// Array of special keys.
GFX2_GLOBAL word Config_Key[NB_SPECIAL_SHORTCUTS][2];

/// A video mode (resolution) usable by Grafx2.
typedef struct
{
  short  Width; ///< Screen width
  short  Height; ///< Screen height
  byte   Mode; ///< Unused (used to be Mode-X, SVGA, etc)
  word   Fullscreen; ///< 0 for window, 1 for fullscreen
  byte   State; ///< How good is the mode supported. 0:Good (white) 1:OK (light) 2:So-so (dark) 4:User-disabled (black); +128 => System doesn't support it at all.
} T_Video_mode;

/// Array of all video modes supported by your platform. Actually filled up to ::Nb_video_modes, excluded.
GFX2_GLOBAL T_Video_mode Video_mode[MAX_VIDEO_MODES];

/// Actual number of video modes in ::Video_mode.
GFX2_GLOBAL int  Nb_video_modes;

/// A default 256-color palette.
GFX2_GLOBAL T_Palette Default_palette;

// Menu colors

GFX2_GLOBAL byte MC_Black; ///< Index of color to use as "black" in the GUI menus.
GFX2_GLOBAL byte MC_Dark; ///< Index of color to use as "dark grey" in the GUI menus.
GFX2_GLOBAL byte MC_Light; ///< Index of color to use as "light grey" in the GUI menus.
GFX2_GLOBAL byte MC_White; ///< Index of color to use as "white" in the GUI menus.
GFX2_GLOBAL byte MC_Trans; ///< Index of color to use as "transparent" while loading the GUI file.

/// Favorite menu colors (RGB values).
GFX2_GLOBAL T_Components Fav_menu_colors[4];

// Input state
GFX2_GLOBAL word Mouse_X; ///< Current mouse cursor position.
GFX2_GLOBAL word Mouse_Y; ///< Current mouse cursor position.
GFX2_GLOBAL byte Mouse_K; ///< Current mouse buttons state. Bitfield: 1 for RMB, 2 for LMB.

/// Helper macro to take only one button when both are pressed (LMB has priority)
#define Mouse_K_unique (Mouse_K==0?0:(Mouse_K&1?1:(Mouse_K&2?2:0)))

/// Last key pressed, 0 if none. Set by the latest call to ::Get_input()
GFX2_GLOBAL dword Key;

///
/// Last character typed, converted to ANSI character set (Windows-1252).
/// This is mostly used when the user enters text (filename, etc).
GFX2_GLOBAL dword Key_ANSI;

// Keyboard modifiers
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
/// Key modifier for SHIFT key. Used as mask in ::Key, for example.
#define MOD_SHIFT 0x1000
/// Key modifier for CONTROL key. Used as mask in ::Key, for example.
#define MOD_CTRL  0x2000
/// Key modifier for ALT key. Used as mask in ::Key, for example.
#define MOD_ALT   0x4000

/// Boolean set to true when the OS/window manager requests the application to close. ie: [X] button
GFX2_GLOBAL byte Quit_is_required;

/// Divisor for the mouse coordinates recieved. No target currently needs it, should be left at 1.
GFX2_GLOBAL byte Mouse_fix_factor_X;
/// Divisor for the mouse coordinates recieved. No target currently needs it, should be left at 1.
GFX2_GLOBAL byte Mouse_fix_factor_Y;

/// 
/// This boolean is true when the current operation allows changing the
/// foreground or background color.
GFX2_GLOBAL byte Allow_color_change_during_operation;

// Mouse cursor data

/// Current mouse cursor. Value is in enum ::CURSOR_SHAPES
GFX2_GLOBAL byte Cursor_shape;
/// Backup of ::Cursor_shape, used while a window is open (and needs a different cursor)
GFX2_GLOBAL byte Cursor_shape_before_window;
/// Boolean, means the cursor should not be drawn. It's togglable by the user.
GFX2_GLOBAL byte Cursor_hidden;
/// Boolean, means the cursor is currently hovering over a menu GUI element.
GFX2_GLOBAL byte Cursor_in_menu;
/// Boolean, means the cursor was hovering over a menu GUI element.
GFX2_GLOBAL byte Cursor_in_menu_previous;
/// X coordinate of the mouse cursor's "hot spot". It is < ::CURSOR_SPRITE_WIDTH
GFX2_GLOBAL word Cursor_offset_X[NB_CURSOR_SPRITES];
/// Y coordinate of the mouse cursor's "hot spot". It is < ::CURSOR_SPRITE_HEIGHT
GFX2_GLOBAL word Cursor_offset_Y[NB_CURSOR_SPRITES];
/// Graphic resources for the mouse cursor.
GFX2_GLOBAL byte GFX_cursor_sprite[NB_CURSOR_SPRITES][CURSOR_SPRITE_HEIGHT][CURSOR_SPRITE_WIDTH];
/// Storage for the graphics under the mouse cursor. Used by ::Hide_cursor and ::Display_cursor
GFX2_GLOBAL byte CURSOR_BACKGROUND[CURSOR_SPRITE_HEIGHT][CURSOR_SPRITE_WIDTH];

// Paintbrush data

/// Active paintbrush. It's an index in enum ::PAINTBRUSH_SHAPES
GFX2_GLOBAL byte  Paintbrush_shape;
/// Backup of ::Paintbrush_shape, before fill operation
GFX2_GLOBAL byte  Paintbrush_shape_before_fill;
/// Backup of ::Paintbrush_shape, before color picker operation
GFX2_GLOBAL byte  Paintbrush_shape_before_colorpicker;
/// Backup of ::Paintbrush_shaper, before lasso operation
GFX2_GLOBAL byte  Paintbrush_shape_before_lasso;
/// Boolean, true when the preview paintbrush shouldn't be drawn.
GFX2_GLOBAL byte  Paintbrush_hidden;
/// Cordinate of the preview paintbrush in image space.
GFX2_GLOBAL short Paintbrush_X;
/// Cordinate of the preview paintbrush in image space.
GFX2_GLOBAL short Paintbrush_Y;
/// Graphic resources for the preset paintbrushes.
GFX2_GLOBAL byte  GFX_paintbrush_sprite [NB_PAINTBRUSH_SPRITES][PAINTBRUSH_HEIGHT][PAINTBRUSH_WIDTH];
/// Width of the preset paintbrushes.
GFX2_GLOBAL word  Preset_paintbrush_width[NB_PAINTBRUSH_SPRITES];
/// Height of the preset paintbrushes.
GFX2_GLOBAL word  Preset_paintbrush_height[NB_PAINTBRUSH_SPRITES];
/// Type of the preset paintbrush: index in enum PAINTBRUSH_SHAPES
GFX2_GLOBAL byte  Paintbrush_type[NB_PAINTBRUSH_SPRITES];
/// Brush handle for the preset brushes. Generally ::Preset_paintbrush_width[]/2
GFX2_GLOBAL word  Preset_paintbrush_offset_X[NB_PAINTBRUSH_SPRITES];
/// Brush handle for the preset brushes. Generally ::Preset_paintbrush_height[]/2
GFX2_GLOBAL word  Preset_paintbrush_offset_Y[NB_PAINTBRUSH_SPRITES];
/// Pixel data of the current brush
GFX2_GLOBAL byte * Paintbrush_sprite;
/// Current paintbrush's width
GFX2_GLOBAL short Paintbrush_width;
/// Current paintbrush's height
GFX2_GLOBAL short Paintbrush_height;
/// Position of current paintbrush's handle
GFX2_GLOBAL short Paintbrush_offset_X;
/// Position of current paintbrush's handle
GFX2_GLOBAL short Paintbrush_offset_Y;

// Graphic commands

/// On the screen, draw a point.
GFX2_GLOBAL Func_pixel Pixel;
/// On screen, draw a point in the menu (do nothing is menu is hidden).
GFX2_GLOBAL Func_pixel Pixel_in_menu;
/// Test a pixel color from screen.
GFX2_GLOBAL Func_read Read_pixel;
/// Redraw all screen, without overwriting the menu.
GFX2_GLOBAL Func_display Display_screen;
/// Draw a rectangle on screen.
GFX2_GLOBAL Func_block Block;
/// Draw a point from the image to screen (no zoom).
GFX2_GLOBAL Func_pixel Pixel_preview_normal;
/// Draw a point from the image to screen (magnified part).
GFX2_GLOBAL Func_pixel Pixel_preview_magnifier;
/// Draw a point from the image to screen (zoomed if needed).
GFX2_GLOBAL Func_pixel Pixel_preview;
/// Draw a horizontal XOR line on screen.
GFX2_GLOBAL Func_line_XOR Horizontal_XOR_line;
/// Draw a vertical XOR line on screen.
GFX2_GLOBAL Func_line_XOR Vertical_XOR_line;
/// Display part of the brush on screen, color mode.
GFX2_GLOBAL Func_display_brush_color Display_brush_color;
/// Display part of the brush on screen, monochrome mode.
GFX2_GLOBAL Func_display_brush_mono  Display_brush_mono;
/// Clear the brush currently displayed on screen, redrawing the image instead.
GFX2_GLOBAL Func_display_brush_color Clear_brush;
/// Remap part of the screen after the menu colors have changed.
GFX2_GLOBAL Func_remap     Remap_screen;
/// Draw a line on screen.
GFX2_GLOBAL Func_procsline Display_line;
/// Draw a line on screen, without doubling it if using wide pixels. (to be used when the line is already doubled in the input buffer)
GFX2_GLOBAL Func_procsline Display_line_fast;
/// Read a line of pixels from screen.
GFX2_GLOBAL Func_procsline Read_line;
/// Redraw all magnified part on screen, without overwriting the menu.
GFX2_GLOBAL Func_display_zoom Display_zoomed_screen;
/// Display part of the brush on the magnified part of screen, color mode.
GFX2_GLOBAL Func_display_brush_color_zoom Display_brush_color_zoom;
/// Display part of the brush on the magnified part of screen, monochrome mode.
GFX2_GLOBAL Func_display_brush_mono_zoom  Display_brush_mono_zoom;
/// Clear the brush currently displayed on the magnified part of screen, redrawing the image instead.
GFX2_GLOBAL Func_display_brush_color_zoom Clear_brush_scaled;
/// Draw an arbitrary brush on screen (not the current brush)
GFX2_GLOBAL Func_draw_brush Display_brush;

// Screen data

/// Requested window width. This is set when the user resizes the window.
GFX2_GLOBAL int   Resize_width;
/// Requested window height. This is set when the user resizes the window.
GFX2_GLOBAL int   Resize_height;
/// Current video mode. Index in ::Video_mode
GFX2_GLOBAL int   Current_resolution;
/// After loading an image, this holds the "original screen width", if the file format supported it.
GFX2_GLOBAL short Original_screen_X;
/// After loading an image, this holds the "original screen height", if the file format supported it.
GFX2_GLOBAL short Original_screen_Y;
///
/// Current screen (or window) width, in pixels.
/// Note that this takes ::Pixel_ratio into account.
GFX2_GLOBAL short Screen_width;
///
/// Current screen (or window) height, in pixels.
/// Note that this takes ::Pixel_ratio into account.
GFX2_GLOBAL short Screen_height;
/// Coordinate (in image space) of the topmost visible pixel.
GFX2_GLOBAL short Limit_top;
///
/// Coordinate (in image space) of the lowest visible pixel.
/// This can be larger than the image height, if the screen is bigger than image.
GFX2_GLOBAL short Limit_bottom;
/// Coordinate (in image space) of the leftmost visible pixel.
GFX2_GLOBAL short Limit_left;
///
/// Coordinate (in image space) of the rightmost visible pixel.
/// This can be larger than the image width, if the screen is bigger than image.
GFX2_GLOBAL short Limit_right;
///
/// Coordinate (in image space) of the lowest visible pixel, limited by the
/// image height. Compare with ::Limit_bottom, which is not clipped.
GFX2_GLOBAL short Limit_visible_bottom;
///
/// Coordinate (in image space) of the rightmost visible pixel, limited by the
/// image width. Compare with ::Limit_right, which is not clipped.
GFX2_GLOBAL short Limit_visible_right;

/// Coordinate (in image space) of the pixel at the top of the magnified view.
GFX2_GLOBAL short Limit_top_zoom;
///
/// Coordinate (in image space) of the pixel at the bottom of the magnified view.
/// This can be larger than the image height, if the screen is bigger than image.
GFX2_GLOBAL short Limit_bottom_zoom;
/// Coordinate (in image space) of the pixel at the left of the magnified view.
GFX2_GLOBAL short Limit_left_zoom;
///
/// Coordinate (in image space) of the pixel at the right of the magnified view.
/// This can be larger than the image width, if the screen is bigger than image.
GFX2_GLOBAL short Limit_right_zoom;
///
/// Coordinate (in image space) of the lowest visible pixel, limited by the
/// image height. Compare with ::Limit_bottom, which is not clipped.
GFX2_GLOBAL short Limit_visible_bottom_zoom;
/// Coordinate (in image space) of the rightmost visible pixel.
/// This can be larger than the image width, if the screen is bigger than image.
GFX2_GLOBAL short Limit_visible_right_zoom;

/// Buffer of pixels, used when drawing something to screen.
GFX2_GLOBAL byte * Horizontal_line_buffer;

/// Current pixel ratio. Index in enum ::PIXEL_RATIO
GFX2_GLOBAL int Pixel_ratio;
/// Current width of pixels, according to ::Pixel_ratio
GFX2_GLOBAL int Pixel_width;
/// Current height of pixels, according to ::Pixel_ratio
GFX2_GLOBAL int Pixel_height;


// Current image data

/// Pointer to the pixel data of the main image
GFX2_GLOBAL byte *    Main_screen;
/// Palette of the main image
GFX2_GLOBAL T_Palette Main_palette;
/// Boolean, means the image has been modified since last save.
GFX2_GLOBAL byte  Main_image_is_modified;
/// Width in pixels of the main image.
GFX2_GLOBAL short Main_image_width;
/// Height in pixels of the main image.
GFX2_GLOBAL short Main_image_height;
/// X position (in image space) of the pixel to display in the top left corner of screen.
GFX2_GLOBAL short Main_offset_X;
/// Y position (in image space) of the pixel to display in the top left corner of screen.
GFX2_GLOBAL short Main_offset_Y;
/// Backup of ::Main_offset_X, used to store it while the magnifier is open.
GFX2_GLOBAL short Old_main_offset_X;
/// Backup of ::Main_offset_Y, used to store it while the magnifier is open.
GFX2_GLOBAL short Old_main_offset_Y;
/// Name of the directory that holds the image currently edited.
GFX2_GLOBAL char  Main_file_directory[1024];
/// Filename (without directory) of the image currently edited.
GFX2_GLOBAL char  Main_filename[256];
/// File format of the image currently edited. It's a value of enum ::FILE_FORMATS
GFX2_GLOBAL byte  Main_fileformat;
///
/// Fileselector "filter" format, for the current image.
/// (The spare page has its own separate settings)
/// It's 0 for "*.*", or a value of enum ::FILE_FORMATS
GFX2_GLOBAL byte  Main_format;
/// Index of the first file/entry to display in the fileselector.
GFX2_GLOBAL short Main_fileselector_position;
///
/// Position of the "highlight" bar in the fileselector. 10 Files can be visible,
/// So it's a number in the [0-9] range.
GFX2_GLOBAL short Main_fileselector_offset;
/// Current directory for the fileselector.
GFX2_GLOBAL char  Main_current_directory[1024];
/// Main image's file comment (some image formats support text strings).
GFX2_GLOBAL char  Main_comment[COMMENT_SIZE+1];
/// X position (in screen coordinates) of the separator between normal and magnified views.
GFX2_GLOBAL short Main_separator_position;
/// X position (in screen coordinates) of the first pixel of the magnified view.
GFX2_GLOBAL short Main_X_zoom;
/// Proportion of the non-magnified part of the screen. 
GFX2_GLOBAL float Main_separator_proportion;
/// Boolean, true if the main image has the magnifier active.
GFX2_GLOBAL byte  Main_magnifier_mode;
/// Zoom factor used in the magnifier (main image).
GFX2_GLOBAL word  Main_magnifier_factor;
/// Height of the magnified view for the main image.
GFX2_GLOBAL word  Main_magnifier_height;
/// Width of the magnified view for the main image.
GFX2_GLOBAL word  Main_magnifier_width;
/// X position (in image space) of the pixel to display in the top left corner of the magnified view.
GFX2_GLOBAL short Main_magnifier_offset_X;
/// Y position (in image space) of the pixel to display in the top left corner of the magnified view.
GFX2_GLOBAL short Main_magnifier_offset_Y;

// Spare page data

/// Pointer to the pixel data of the spare page
GFX2_GLOBAL byte *    Spare_screen;
/// Palette of the spare page
GFX2_GLOBAL T_Palette Spare_palette;
/// Boolean, means the spare page has been modified since last save.
GFX2_GLOBAL byte  Spare_image_is_modified;
/// Width in pixels of the spare image.
GFX2_GLOBAL short Spare_image_width;
/// Height in pixels of the spare image.
GFX2_GLOBAL short Spare_image_height;
/// X position (in image space) of the pixel to display in the top left corner of screen.
GFX2_GLOBAL short Spare_offset_X;
/// Y position (in image space) of the pixel to display in the top left corner of screen.
GFX2_GLOBAL short Spare_offset_Y;
/// Backup of ::Main_offset_X, used to store it while the magnifier is open.
GFX2_GLOBAL short Old_spare_offset_X;
/// Backup of ::Main_offset_Y, used to store it while the magnifier is open.
GFX2_GLOBAL short Old_spare_offset_Y;
/// Name of the directory that holds the image currently edited as spare page.
GFX2_GLOBAL char  Spare_file_directory[MAX_PATH_CHARACTERS];
/// Filename (without directory) of the image currently edited as spare page.
GFX2_GLOBAL char  Spare_filename[MAX_PATH_CHARACTERS];
/// File format of the image currently edited as spare page. It's a value of enum ::FILE_FORMATS
GFX2_GLOBAL byte  Spare_fileformat;
///
/// Fileselector "filter" format, for the spare page.
/// (The main image has its own separate settings)
/// It's 0 for "*.*", or a value of enum ::FILE_FORMAT
GFX2_GLOBAL byte  Spare_format;
/// Index of the first file/entry to display in the fileselector.
GFX2_GLOBAL short Spare_fileselector_position;
///
/// Position of the "highlight" bar in the fileselector. 10 Files can be visible,
/// So it's a number in the [0-9] range.
GFX2_GLOBAL short Spare_fileselector_offset;
/// Current directory for the fileselector.
GFX2_GLOBAL char  Spare_current_directory[MAX_PATH_CHARACTERS];
/// Spare page's file comment  (some image formats support text strings).
GFX2_GLOBAL char  Spare_comment[COMMENT_SIZE+1];
/// X position (in screen coordinates) of the separator between normal and magnified views.
GFX2_GLOBAL short Spare_separator_position;
/// X position (in screen coordinates) of the first pixel of the magnified view.
GFX2_GLOBAL short Spare_X_zoom;
/// Proportion of the non-magnified part of the screen. 
GFX2_GLOBAL float Spare_separator_proportion;
/// Boolean, true if the main image has the magnifier active.
GFX2_GLOBAL byte  Spare_magnifier_mode;
/// Zoom factor used in the magnifier (spare page).
GFX2_GLOBAL word  Spare_magnifier_factor;
/// Width of the magnified view for the spare page.
GFX2_GLOBAL word  Spare_magnifier_height;
/// Height of the magnified view for the spare page.
GFX2_GLOBAL word  Spare_magnifier_width;
/// X position (in image space) of the pixel to display in the top left corner of the magnified view.
GFX2_GLOBAL short Spare_magnifier_offset_X;
/// Y position (in image space) of the pixel to display in the top left corner of the magnified view.
GFX2_GLOBAL short Spare_magnifier_offset_Y;

// Image backups

/// Backup of the current screen, used during drawing when FX feedback is OFF.
GFX2_GLOBAL byte * Screen_backup;
/// List of backup pages for the main image.
GFX2_GLOBAL T_List_of_pages * Main_backups;
/// List of backup pages for the spare page.
GFX2_GLOBAL T_List_of_pages * Spare_backups;

// Brush data

/// Pixel data of the current brush.
GFX2_GLOBAL byte * Brush;
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

GFX2_GLOBAL byte Windows_open; // Nombre de fenetres empil�es. 0 si pas de fenetre ouverte.

GFX2_GLOBAL byte Menu_is_visible_before_window;  // Le menu �tait visible avant d'ouvir une fen�tre
GFX2_GLOBAL word Menu_Y_before_window; // Ordonn�e du menu avant d'ouvrir une fen�tre
GFX2_GLOBAL byte Paintbrush_hidden_before_window;// Le pinceau �tatit d�j� cach� avant l'ouverture de la fenetre?

GFX2_GLOBAL word Window_stack_pos_X[8];   // Position du bord gauche de la fen�tre dans l'�cran
#define Window_pos_X Window_stack_pos_X[Windows_open-1]

GFX2_GLOBAL word Window_stack_pos_Y[8];   // Position du bord haut   de la fen�tre dans l'�cran
#define Window_pos_Y Window_stack_pos_Y[Windows_open-1]

GFX2_GLOBAL word Window_stack_width[8]; // Largeur de la fen�tre
#define Window_width Window_stack_width[Windows_open-1]

GFX2_GLOBAL word Window_stack_height[8]; // Hauteur de la fen�tre
#define Window_height Window_stack_height[Windows_open-1]

GFX2_GLOBAL word Window_stack_nb_buttons[8];
#define Window_nb_buttons Window_stack_nb_buttons[Windows_open-1]

GFX2_GLOBAL T_Normal_button   * Window_stack_normal_button_list[8];
#define Window_normal_button_list Window_stack_normal_button_list[Windows_open-1]

GFX2_GLOBAL T_Palette_button  * Window_stack_palette_button_list[8];
#define Window_palette_button_list Window_stack_palette_button_list[Windows_open-1]

GFX2_GLOBAL T_Scroller_button * Window_stack_scroller_button_list[8];
#define Window_scroller_button_list Window_stack_scroller_button_list[Windows_open-1]

GFX2_GLOBAL T_Special_button  * Window_stack_special_button_list[8];
#define Window_special_button_list Window_stack_special_button_list[Windows_open-1]

GFX2_GLOBAL T_Dropdown_button  * Window_stack_dropdown_button_list[8];
#define Window_dropdown_button_list Window_stack_dropdown_button_list[Windows_open-1]


GFX2_GLOBAL int Window_stack_attribute1[8];
#define Window_attribute1 Window_stack_attribute1[Windows_open-1]

// Cette variable sert � stocker 2 informations:
// - Sur le click d'un scroller, la position active (0-n)
// - Sur le click d'une palette, le numero de couleur (0-255).
GFX2_GLOBAL int Window_stack_attribute2[8];
#define Window_attribute2 Window_stack_attribute2[Windows_open-1]




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
GFX2_GLOBAL byte * Menu_font;

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
