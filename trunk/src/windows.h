/* vim:expandtab:ts=2 sw=2:
*/
/*  Grafx2 - The Ultimate 256-color bitmap paint program

    Copyright 2007-2008 Adrien Destugues
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
    along with Grafx2; if not, see <http://www.gnu.org/licenses/>
*/

//////////////////////////////////////////////////////////////////////////////
///@file windows.h
/// Graphical interface management functions (windows, menu, cursor)
//////////////////////////////////////////////////////////////////////////////

#ifndef __WINDOWS_H_
#define __WINDOWS_H_

#include "struct.h"

#define ToWinX(x) (((x)*Menu_factor_X)+Window_pos_X)
#define ToWinY(y) (((y)*Menu_factor_Y)+Window_pos_Y)
#define ToWinL(l) ((l)*Menu_factor_X)
#define ToWinH(h) ((h)*Menu_factor_Y)

#define Update_window_area(x,y,w,h) Update_rect(Window_pos_X+(x)*Menu_factor_X,Window_pos_Y+(y)*Menu_factor_Y,(w)*Menu_factor_X,(h)*Menu_factor_Y);

void Display_cursor(void);
void Hide_cursor(void);

void Remap_screen_after_menu_colors_change(void);
void Compute_optimal_menu_colors(T_Components * palette);
void Remap_menu_sprites();

void Position_screen_according_to_zoom(void);
void Compute_separator_data(void);
void Compute_magnifier_data(void);
void Clip_magnifier_offsets(short *x_offset, short *y_offset);
void Compute_limits(void);
void Compute_paintbrush_coordinates(void);

void Pixel_in_menu(word bar, word x, word y, byte color);
void Pixel_in_menu_and_skin(word bar, word x, word y, byte color);
void Pixel_in_window(word x,word y,byte color);
void Set_fore_color(byte color);
void Set_back_color(byte color);
void Frame_menu_color(byte id);
void Display_menu_palette(void);
void Display_menu(void);
void Display_layerbar(void);
void Reposition_palette(void);
void Change_palette_cells(void);
int Pick_color_in_palette(void);
word Palette_cells_X(void);
word Palette_cells_Y(void);

void Print_general(short x,short y,const char * str,byte text_color,byte background_color);
void Print_in_window(short x,short y,const char * str,byte text_color,byte background_color);
void Print_in_window_limited(short x,short y,const char * str,byte size,byte text_color,byte background_color);
void Print_char_in_window(short x_pos,short y_pos,const unsigned char c,byte text_color,byte background_color);
void Print_in_menu(const char * str, short position);
void Print_coordinates(void);
void Print_filename(void);
void Print_counter(short x,short y,const char * str,byte text_color,byte background_color);

byte Confirmation_box(char * message);
void Warning_message(char * message);
void Verbose_message(const char * caption, const char * message);
int Requester_window(char* message, int initial_value);

void Display_image_limits(void);
void Display_all_screen(void);
void Window_rectangle(word x_pos,word y_pos,word width,word height,byte color);
void Window_display_frame_generic(word x_pos,word y_pos,word width,word height,
                                    byte color_tl,byte color_br,byte color_s,byte color_tlc,byte color_brc);
void Window_display_frame_mono(word x_pos,word y_pos,word width,word height,byte color);
void Window_display_frame_in(word x_pos,word y_pos,word width,word height);
void Window_display_frame_out(word x_pos,word y_pos,word width,word height);
void Window_display_frame(word x_pos,word y_pos,word width,word height);

void Display_sprite_in_menu(int btn_number,char sprite_number);
void Display_paintbrush_in_menu(void);
void Display_paintbrush_in_window(word x,word y,int number);

void Draw_thingumajig(word x,word y, byte color, short direction);
void Display_grad_block_in_window(word x_pos,word y_pos,word block_start,word block_end);
void Window_display_icon_sprite(word x_pos,word y_pos,byte type);

byte Best_color(byte red,byte green,byte blue);
byte Best_color_nonexcluded(byte red,byte green,byte blue);
byte Best_color_perceptual(byte r,byte g,byte b);
byte Best_color_perceptual_except(byte r,byte g,byte b, byte except);
byte Best_color_perceptual_weighted(byte r,byte g,byte b, float weight);

void Horizontal_XOR_line_zoom(short x_pos, short y_pos, short width);
void Vertical_XOR_line_zoom(short x_pos, short y_pos, short height);

void Change_magnifier_factor(byte factor_index, byte point_at_mouse);

/// Width of one layer button, in pixels before scaling
extern word Layer_button_width;

/// Copy viewport settings and offsets from the Main to the Spare.
void Copy_view_to_spare(void);

/// Definition of a toolbar button
typedef struct
{
  // Button aspect
  word            X_offset;         ///< Position relative to menu's left
  word            Y_offset;         ///< Position relative to menu's top
  word            Width;            ///< Button's active width
  word            Height;           ///< Button's active heigth
  byte            Pressed;          ///< Button is currently pressed
  byte            Shape;            ///< Shape, listed in enum ::BUTTON_SHAPES
  signed char     Icon;             ///< Which icon to display: Either the one from the toolbar (-1) or one of ::MENU_SPRITE

  // Triggers on mouse/keyboard
  Func_action     Left_action;      ///< Action triggered by a left mouseclick on the button
  Func_action     Right_action;     ///< Action triggered by a right mouseclick on the button
  word            Left_shortcut[2]; ///< Keyboard shortcut for a left mouseclick
  word            Right_shortcut[2];///< Keyboard shortcut for a right mouseclick
  byte            Left_instant;     ///< Will not wait for mouse release before triggering action
  byte            Right_instant;    ///< Will not wait for mouse release before triggering action
  const char *    Tooltip;          ///< Text in status bar when button is hovered

  // Data used when the button is unselected
  Func_action     Unselect_action;  ///< Action triggered by unselecting the button
  byte            Family;           ///< enum ::FAMILY_OF_BUTTONS.

} T_Toolbar_button;

extern T_Toolbar_button Buttons_Pool[NB_BUTTONS];

#endif
