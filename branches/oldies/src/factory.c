/* vim:expandtab:ts=2 sw=2:
*/
/*  Grafx2 - The Ultimate 256-color bitmap paint program

    Copyright 2009 Adrien Destugues

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

/*! \file factory.c
 *  \brief Brush factory - generates brush from lua scripts
 *
 * The brush factory allows you to generate brushes with Lua code.
 */

#include <math.h>

#include "brush.h"
#include "buttons.h"
#include "engine.h"
#include "errors.h"
#include "filesel.h" // Get_item_by_index
#include "global.h"
#include "graph.h"
#include "io.h"     // find_last_slash
#include "misc.h"
#include "pages.h"  // Backup()
#include "readline.h"
#include "sdlscreen.h"
#include "windows.h"
#include "palette.h"
#include "input.h" // Is_shortcut()
#include "help.h" // Window_help()

/// Lua scripts bound to shortcut keys.
char * Bound_script[10];

#ifdef __ENABLE_LUA__

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <float.h> // for DBL_MAX
#include <unistd.h> // chdir()
#include <limits.h> //for INT_MIN

///
/// Number of characters for name in fileselector.
/// Window is adjusted according to it.
#define NAME_WIDTH 34
/// Number of characters for the description block
#define DESC_WIDTH ((NAME_WIDTH+2)*8/6)
/// Position of fileselector top, in window space
#define FILESEL_Y 18

// Work data that can be used during a script
static byte * Brush_backup = NULL;
static word Brush_backup_width;
static word Brush_backup_height;
static byte Palette_has_changed;
static byte Brush_was_altered;
static byte Original_fore_color;
static byte Original_back_color;

/// Helper function to clamp a double to 0-255 range
static inline byte clamp_byte(double value)
{
  if (value<0.0)
    return 0;
  else if (value>255.0)
    return 255;
  else return (byte)value;
}

///
/// This macro reads a Lua argument into a double or integral lvalue.
/// If argument is invalid, it will break the caller and raise a verbose message.
/// This macro uses 2 existing symbols: L for the context, and nb_args=lua_gettop(L)
/// @param index     Index of the argument to check, starting at 1.
/// @param func_name The name of the lua callback, to display a message in case of error.
/// @param dest      Destination lvalue. Can be a double, or any integral type. Conversion will "floor".
/// @param min_value Check for minimum value. Pass a double, or if you don't care, -DBL_MAX.
/// @param max_value Check for maximum value. Pass a double, or if you don't care, DBL_MAX.
#define LUA_ARG_NUMBER(index, func_name, dest, min_value, max_value) \
do { \
  double value; \
  if (nb_args < (index)) return luaL_error(L, "%s: Argument %d is missing.", func_name, (index)); \
  if (!lua_isnumber(L, (index))) return luaL_error(L, "%s: Argument %d is not a number.", func_name, (index)); \
  value = lua_tonumber(L, (index)); \
  if ((min_value) != -DBL_MAX && value<(min_value)) return luaL_error(L, "%s: Argument %d was too small, it had value of %f and minimum should be %f.", func_name, (index), value, (double)(min_value)); \
  if ((max_value) != DBL_MAX && value>(max_value)) return luaL_error(L, "%s: Argument %d was too big, it had value of %f and maximum should be %f.", func_name, (index), value, (double)(max_value)); \
  dest = value; \
} while(0)

///
/// This macro reads a Lua argument into a string.
/// If argument is invalid, it will break the caller and raise a verbose message.
/// This macro uses 2 existing symbols: L for the context, and nb_args=lua_gettop(L)
/// @param index     Index of the argument to check, starting at 1.
/// @param func_name The name of the lua callback, to display a message in case of error.
/// @param dest      Destination string pointer, ideally a const char *.
#define LUA_ARG_STRING(index, func_name, dest) \
do { \
  if (nb_args < (index)) return luaL_error(L, "%s: Argument %d is missing.", func_name, index); \
  if (!lua_isstring(L, (index))) return luaL_error(L, "%s: Argument %d is not a string.", func_name, index); \
  dest = lua_tostring(L, (index)); \
} while (0)

///
/// This macro checks that a Lua argument is a function.
/// If argument is invalid, it will break the caller and raise a verbose message.
/// This macro uses 2 existing symbols: L for the context, and nb_args=lua_gettop(L)
/// @param index     Index of the argument to check, starting at 1.
/// @param func_name The name of the lua callback, to display a message in case of error.
#define LUA_ARG_FUNCTION(index, func_name) \
do { \
  if (nb_args < (index)) return luaL_error(L, "%s: Argument %d is missing.", func_name, index); \
  if (!lua_isfunction(L, (index))) return luaL_error(L, "%s: Argument %d is not a function.", func_name, index); \
} while (0)

/// Check if 'num' arguments were provided exactly
#define LUA_ARG_LIMIT(num, func_name) \
do { \
  if (nb_args != (num)) \
    return luaL_error(L, "%s: Expected %d arguments, but found %d.", func_name, (num), nb_args); \
} while(0)

// Updates the screen colors after a running screen has modified the palette.
void Update_colors_during_script(void)
{
  if (Palette_has_changed)
  {
    Set_palette(Main_palette);
    Compute_optimal_menu_colors(Main_palette);
    Display_menu();
    Palette_has_changed=0;
  }
}


// Wrapper functions to call C from Lua

int L_SetBrushSize(lua_State* L)
{
  int w;
  int h;
  int nb_args=lua_gettop(L);
  
  LUA_ARG_LIMIT (2, "setbrushsize");
  LUA_ARG_NUMBER(1, "setbrushsize", w, 1, 10000);
  LUA_ARG_NUMBER(2, "setbrushsize", h, 1, 10000);
  
  if (Realloc_brush(w, h))
  {
    return luaL_error(L, "setbrushsize: Resize failed");
  }
  Brush_was_altered=1;
  // Fill with Back_color
  memset(Brush,Back_color,(long)Brush_width*Brush_height);
  // Center the handle
  Brush_offset_X=(Brush_width>>1);
  Brush_offset_Y=(Brush_height>>1);
  return 0;
}

int L_GetBrushSize(lua_State* L)
{
  lua_pushinteger(L, Brush_width);  
  lua_pushinteger(L, Brush_height); 
  return 2;
}

int L_GetBrushTransparentColor(lua_State* L)
{
  lua_pushinteger(L, Back_color);
  return 1;
}

int L_PutBrushPixel(lua_State* L)
{
  int x;
  int y;
  uint8_t c;
  int nb_args=lua_gettop(L);
  
  LUA_ARG_LIMIT (3, "putbrushpixel");
  LUA_ARG_NUMBER(1, "putbrushpixel", x, INT_MIN, INT_MAX);
  LUA_ARG_NUMBER(2, "putbrushpixel", y, INT_MIN, INT_MAX);
  LUA_ARG_NUMBER(3, "putbrushpixel", c, INT_MIN, INT_MAX);

  Brush_was_altered=1;
  
  if (x<0 || y<0 || x>=Brush_width || y>=Brush_height)
  ;
  else
  {
    Pixel_in_brush(x, y, c);
  }
  return 0; // no values returned for lua
}

int L_GetBrushPixel(lua_State* L)
{
  int x;
  int y;
  uint8_t c;
  int nb_args=lua_gettop(L);
  
  LUA_ARG_LIMIT (2, "getbrushpixel");
  LUA_ARG_NUMBER(1, "getbrushpixel", x, INT_MIN, INT_MAX);
  LUA_ARG_NUMBER(2, "getbrushpixel", y, INT_MIN, INT_MAX);

  if (x<0 || y<0 || x>=Brush_width || y>=Brush_height)
  {
    c = Back_color; // Return 'transparent'
  }
  else
  {
    c = Read_pixel_from_brush(x, y);
  }
  lua_pushinteger(L, c);
  return 1;
}

int L_GetBrushBackupPixel(lua_State* L)
{
  int x;
  int y;
  uint8_t c;
  int nb_args=lua_gettop(L);
  
  LUA_ARG_LIMIT (2, "getbrushbackuppixel");
  LUA_ARG_NUMBER(1, "getbrushbackuppixel", x, INT_MIN, INT_MAX);
  LUA_ARG_NUMBER(2, "getbrushbackuppixel", y, INT_MIN, INT_MAX);
  
  if (x<0 || y<0 || x>=Brush_backup_width || y>=Brush_backup_height)
  {
    c = Back_color; // Return 'transparent'
  }
  else
  {
    c = *(Brush_backup + y * Brush_backup_width + x);
  }
  lua_pushinteger(L, c);
  return 1;
}

int L_SetPictureSize(lua_State* L)
{
  
  int w;
  int h;
  int nb_args=lua_gettop(L);
  int i;
  
  LUA_ARG_LIMIT (2, "setpicturesize");
  LUA_ARG_NUMBER(1, "setpicturesize", w, 1, 9999);
  LUA_ARG_NUMBER(2, "setpicturesize", h, 1, 9999);
    
  Backup_in_place(w, h);
  // part of Resize_image() : the pixel copy part.
  for (i=0; i<Main_backups->Pages->Nb_layers; i++)
  {
    Copy_part_of_image_to_another(
      Main_backups->Pages->Next->Image[i],0,0,Min(Main_backups->Pages->Next->Width,Main_image_width),
      Min(Main_backups->Pages->Next->Height,Main_image_height),Main_backups->Pages->Next->Width,
      Main_backups->Pages->Image[i],0,0,Main_image_width);
  }
  Redraw_layered_image();
  
  return 0;
}

int L_GetPictureSize(lua_State* L)
{
  lua_pushinteger(L, Main_image_width);
  lua_pushinteger(L, Main_image_height);
  return 2;
}

int L_ClearPicture(lua_State* L)
{
  int c;
  int nb_args=lua_gettop(L);
  
  LUA_ARG_LIMIT (1, "clearpicture");
  LUA_ARG_NUMBER(1, "clearpicture", c, INT_MIN, INT_MAX);
  
  if (Stencil_mode && Config.Clear_with_stencil)
    Clear_current_image_with_stencil(c,Stencil);
  else
    Clear_current_image(c);
  Redraw_layered_image();
  
  return 0; // no values returned for lua
}

int L_PutPicturePixel(lua_State* L)
{
  int x;
  int y;
  int c;
  int nb_args=lua_gettop(L);
  
  LUA_ARG_LIMIT (3, "putpicturepixel");
  LUA_ARG_NUMBER(1, "putpicturepixel", x, INT_MIN, INT_MAX);
  LUA_ARG_NUMBER(2, "putpicturepixel", y, INT_MIN, INT_MAX);
  LUA_ARG_NUMBER(3, "putpicturepixel", c, INT_MIN, INT_MAX);
  
  // Bound check
  if (x<0 || y<0 || x>=Main_image_width || y>=Main_image_height)
  {
    // Silently ignored
    return 0;
  }
  Pixel_in_current_screen(x, y, c, 0);
  return 0; // no values returned for lua
}

int L_GetPicturePixel(lua_State* L)
{
  int x;
  int y;
  int nb_args=lua_gettop(L);
  
  LUA_ARG_LIMIT (2, "getpicturepixel");
  LUA_ARG_NUMBER(1, "getpicturepixel", x, INT_MIN, INT_MAX);
  LUA_ARG_NUMBER(2, "getpicturepixel", y, INT_MIN, INT_MAX);
  
  // Bound check
  if (x<0 || y<0 || x>=Main_image_width || y>=Main_image_height)
  {
    // Silently return the image's transparent color
    lua_pushinteger(L, Main_backups->Pages->Transparent_color);
    return 1;
  }
  lua_pushinteger(L, Read_pixel_from_current_screen(x,y));
  return 1;
}

int L_GetBackupPixel(lua_State* L)
{
  int x;
  int y;
  int nb_args=lua_gettop(L);
  
  LUA_ARG_LIMIT (2, "getbackuppixel");
  LUA_ARG_NUMBER(1, "getbackuppixel", x, INT_MIN, INT_MAX);
  LUA_ARG_NUMBER(2, "getbackuppixel", y, INT_MIN, INT_MAX);
  
  // Bound check
  if (x<0 || y<0 || x>=Main_backups->Pages->Next->Width || y>=Main_backups->Pages->Next->Height)
  {
    // Silently return the image's transparent color
    lua_pushinteger(L, Main_backups->Pages->Next->Transparent_color);
    return 1;
  }
  // Can't use Read_pixel_from_backup_screen(), because in a Lua script
  // the "backup" can use a different screen dimension.
  lua_pushinteger(L, *(Screen_backup + x + Main_backups->Pages->Next->Width * y));
  
  return 1;
}

int L_GetLayerPixel(lua_State* L)
{
  int x;
  int y;
  int nb_args=lua_gettop(L);
  
  LUA_ARG_LIMIT (2, "getlayerpixel");
  LUA_ARG_NUMBER(1, "getlayerpixel", x, INT_MIN, INT_MAX);
  LUA_ARG_NUMBER(2, "getlayerpixel", y, INT_MIN, INT_MAX);
  
  // Bound check
  if (x<0 || y<0 || x>=Main_image_width || y>=Main_image_height)
  {
    // Silently return the image's transparent color
    lua_pushinteger(L, Main_backups->Pages->Transparent_color);
    return 1;
  }
  lua_pushinteger(L, Read_pixel_from_current_layer(x,y));
  return 1;
}

// Spare

int L_GetSparePictureSize(lua_State* L)
{
  lua_pushinteger(L, Spare_image_width);  
  lua_pushinteger(L, Spare_image_height); 
  return 2;
}

int L_GetSpareLayerPixel(lua_State* L)
{
  int x;
  int y;
  int nb_args=lua_gettop(L);
  
  LUA_ARG_LIMIT (2, "getsparelayerpixel");
  LUA_ARG_NUMBER(1, "getsparelayerpixel", x, INT_MIN, INT_MAX);
  LUA_ARG_NUMBER(2, "getsparelayerpixel", y, INT_MIN, INT_MAX);
  
  // Bound check
  if (x<0 || y<0 || x>=Spare_image_width || y>=Spare_image_height)
  {
    // Silently return the image's transparent color
    lua_pushinteger(L, Spare_backups->Pages->Transparent_color);
    return 1;
  }
  lua_pushinteger(L, *(Spare_backups->Pages->Image[Spare_current_layer] + y*Spare_image_width + x));
  return 1;
}

int L_GetSparePicturePixel(lua_State* L)
{
  int x;
  int y;
  int nb_args=lua_gettop(L);
  
  LUA_ARG_LIMIT (2, "getsparepicturepixel");
  LUA_ARG_NUMBER(1, "getsparepicturepixel", x, INT_MIN, INT_MAX);
  LUA_ARG_NUMBER(2, "getsparepicturepixel", y, INT_MIN, INT_MAX);
  
  // Some bound checking is done by the function itself, here's the rest.
  if (x<0 || y<0)
  {
    // Silently return the image's transparent color
    lua_pushinteger(L, Spare_backups->Pages->Transparent_color);
    return 1;
  }
  lua_pushinteger(L, Read_pixel_from_spare_screen(x,y));
  return 1;
}

int L_GetSpareColor(lua_State* L)
{
  byte c;
  int nb_args=lua_gettop(L);
  
  LUA_ARG_LIMIT (1, "getsparecolor");
  LUA_ARG_NUMBER(1, "getsparecolor", c, INT_MIN, INT_MAX);

  lua_pushinteger(L, Spare_palette[c].R);
  lua_pushinteger(L, Spare_palette[c].G);
  lua_pushinteger(L, Spare_palette[c].B);
  return 3;
}

int L_GetSpareTransColor(lua_State* L)
{
  lua_pushinteger(L, Spare_backups->Pages->Transparent_color);
  return 1;
}



int L_SetColor(lua_State* L)
{
  byte c;
  double r, g, b;
  int nb_args=lua_gettop(L);
    
  LUA_ARG_LIMIT (4, "setcolor");
  LUA_ARG_NUMBER(1, "setcolor", c, INT_MIN, INT_MAX);
  LUA_ARG_NUMBER(2, "setcolor", r, INT_MIN, INT_MAX);
  LUA_ARG_NUMBER(3, "setcolor", g, INT_MIN, INT_MAX);
  LUA_ARG_NUMBER(4, "setcolor", b, INT_MIN, INT_MAX);
  
    
  Main_palette[c].R=Round_palette_component(clamp_byte(r));
  Main_palette[c].G=Round_palette_component(clamp_byte(g));
  Main_palette[c].B=Round_palette_component(clamp_byte(b));
  // Set_color(c, r, g, b); Not needed. Update screen when script is finished
  Palette_has_changed=1;
  return 0;
}

int L_GetColor(lua_State* L)
{
  byte c;
  int nb_args=lua_gettop(L);
  
  LUA_ARG_LIMIT (1, "getcolor");
  LUA_ARG_NUMBER(1, "getcolor", c, INT_MIN, INT_MAX);

  lua_pushinteger(L, Main_palette[c].R);
  lua_pushinteger(L, Main_palette[c].G);
  lua_pushinteger(L, Main_palette[c].B);
  return 3;
}

int L_GetBackupColor(lua_State* L)
{
  byte c;
  int nb_args=lua_gettop(L);
  
  LUA_ARG_LIMIT (1, "getbackupcolor");
  LUA_ARG_NUMBER(1, "getbackupcolor", c, INT_MIN, INT_MAX);

  lua_pushinteger(L, Main_backups->Pages->Next->Palette[c].R);
  lua_pushinteger(L, Main_backups->Pages->Next->Palette[c].G);
  lua_pushinteger(L, Main_backups->Pages->Next->Palette[c].B);
  return 3;
}

int L_MatchColor(lua_State* L)
{
  double r, g, b;
  int c;
  int nb_args=lua_gettop(L);

  LUA_ARG_LIMIT (3, "matchcolor");
  LUA_ARG_NUMBER(1, "matchcolor", r, -DBL_MAX, DBL_MAX);
  LUA_ARG_NUMBER(2, "matchcolor", g, -DBL_MAX, DBL_MAX);
  LUA_ARG_NUMBER(3, "matchcolor", b, -DBL_MAX, DBL_MAX);
  
  c = Best_color_nonexcluded(clamp_byte(r),clamp_byte(g),clamp_byte(b));
  lua_pushinteger(L, c);
  return 1;
}

int L_GetForeColor(lua_State* L)
{
  lua_pushinteger(L, Fore_color);
  return 1;
}

int L_GetBackColor(lua_State* L)
{
  lua_pushinteger(L, Back_color);
  return 1;
}

int L_GetTransColor(lua_State* L)
{
  lua_pushinteger(L, Main_backups->Pages->Transparent_color);
  return 1;
}

int L_SetForeColor(lua_State* L)
{
  byte c;
  int nb_args=lua_gettop(L);
  
  LUA_ARG_LIMIT (1, "setforecolor");
  LUA_ARG_NUMBER(1, "setforecolor", c, -DBL_MAX, DBL_MAX);

  Fore_color = c;

  return 0;
}

int L_SetBackColor(lua_State* L)
{
  byte c;
  int nb_args=lua_gettop(L);
  
  LUA_ARG_LIMIT (1, "setbackcolor");
  LUA_ARG_NUMBER(1, "setbackcolor", c, -DBL_MAX, DBL_MAX);

  Back_color = c;

  return 0;
}


int L_InputBox(lua_State* L)
{
  const int max_settings = 9;
  const int args_per_setting = 5;
  double min_value[max_settings];
  double max_value[max_settings];
  double decimal_places[max_settings];
  double current_value[max_settings];
  const char * label[max_settings];
  unsigned short control[max_settings*3+1]; // Each value has at most 3 widgets.
  enum CONTROL_TYPE {
    CONTROL_OK          = 0x0100,
    CONTROL_CANCEL      = 0x0200,
    CONTROL_INPUT       = 0x0300,
    CONTROL_INPUT_MINUS = 0x0400,
    CONTROL_INPUT_PLUS  = 0x0500,
    CONTROL_CHECKBOX    = 0x0600,
    CONTROL_VALUE_MASK  = 0x00FF,
    CONTROL_TYPE_MASK   = 0xFF00
  };
  const char * window_caption;
  int caption_length;
  int nb_settings;
  int nb_args;
  unsigned int max_label_length;
  int setting;
  short clicked_button;
  char  str[40];
  short close_window = 0;
  
  nb_args = lua_gettop (L);
  
  
  if (nb_args < 6)
  {
    return luaL_error(L, "inputbox: Less than 6 arguments");
  }
  if ((nb_args - 1) % args_per_setting)
  {
    return luaL_error(L, "inputbox: Wrong number of arguments");
  }
  nb_settings = (nb_args-1)/args_per_setting;
  if (nb_settings > max_settings)
  {
    return luaL_error(L, "inputbox: Too many settings, limit reached");
  }
  
  max_label_length=4; // Minimum size to account for OK / Cancel buttons
  
  // First argument is window caption
  LUA_ARG_STRING(1, "inputbox", window_caption);
  caption_length = strlen(window_caption);
  if ( caption_length > 14)
    max_label_length = caption_length - 10;
  
  for (setting=0; setting<nb_settings; setting++)
  {
    LUA_ARG_STRING(setting*args_per_setting+2, "inputbox", label[setting]);
    if (strlen(label[setting]) > max_label_length)
      max_label_length = strlen(label[setting]);
    
    LUA_ARG_NUMBER(setting*args_per_setting+3, "inputbox", current_value[setting], -DBL_MAX, DBL_MAX);
    LUA_ARG_NUMBER(setting*args_per_setting+4, "inputbox", min_value[setting], -DBL_MAX, DBL_MAX);
    /*if (min_value[setting] < -999999999999999.0)
      min_value[setting] = -999999999999999.0;*/
    LUA_ARG_NUMBER(setting*args_per_setting+5, "inputbox", max_value[setting], -DBL_MAX, DBL_MAX);
    /*if (max_value[setting] > 999999999999999.0)
      max_value[setting] = 999999999999999.0;*/
    LUA_ARG_NUMBER(setting*args_per_setting+6, "inputbox", decimal_places[setting], -15.0, 15.0);
    if (decimal_places[setting]>15)
        decimal_places[setting]=15;
    if (min_value[setting]!=0 || max_value[setting]!=1)
      if (decimal_places[setting]<0)
        decimal_places[setting]=0;
    // Keep current value in range
    if (decimal_places[setting]>=0)
      current_value[setting] = Fround(current_value[setting], decimal_places[setting]);

    if (current_value[setting] < min_value[setting])
      current_value[setting] = min_value[setting];
    else if (current_value[setting] > max_value[setting])
      current_value[setting] = max_value[setting];
  }
  // Max is 25 to keep window under 320 pixel wide
  if (max_label_length>25)
    max_label_length=25;

  Update_colors_during_script();
  Open_window(115+max_label_length*8,44+nb_settings*17,window_caption);

  // Normally this index is unused, but this initialization avoids
  // any weird behavior if it was used by mistake.
  control[0]=0;
  
  // OK
  Window_set_normal_button( 7, 25 + 17 * nb_settings, 51,14,"OK" , 0,1,SDLK_RETURN);
  control[Window_nb_buttons] = CONTROL_OK;

  // Cancel
  Window_set_normal_button( 64, 25 + 17 * nb_settings, 51,14,"Cancel" , 0,1,KEY_ESC);
  control[Window_nb_buttons] = CONTROL_CANCEL;
  
  for (setting=0; setting<nb_settings; setting++)
  {
    Print_in_window_limited(12,22+setting*17,label[setting],max_label_length,MC_Black,MC_Light);
    if (min_value[setting]==0 && max_value[setting]==1 && decimal_places[setting]<=0)
    {
      // Checkbox or Radio button
      byte outline = decimal_places[setting]==0 ? 2 : 0;
      Window_set_normal_button(12+max_label_length*8+44-outline, 21+setting*17-outline, 12+outline*2,10+outline*2,current_value[setting]?"X":"", 0,1,KEY_NONE);
      control[Window_nb_buttons] = CONTROL_CHECKBOX | setting;
    }
    else
    {
      // - Button
      Window_set_repeatable_button(12+max_label_length*8+5, 20+setting*17, 13,11,"-" , 0,1,KEY_NONE);
      control[Window_nb_buttons] = CONTROL_INPUT_MINUS | setting;

      // Numeric input field
      Window_set_input_button(12+max_label_length*8+21, 20+setting*17,7);
      // Print editable value
      Sprint_double(str,current_value[setting],decimal_places[setting],7);
      Print_in_window_limited(12+max_label_length*8+23, 22+setting*17, str, 7,MC_Black,MC_Light);
      //
      control[Window_nb_buttons] = CONTROL_INPUT | setting;

      // + Button
      Window_set_repeatable_button(12+max_label_length*8+84, 20+setting*17, 13,11,"+" , 0,1,KEY_NONE);
      control[Window_nb_buttons] = CONTROL_INPUT_PLUS | setting;
    }
  }
  
  // Draw outlines for series of radio buttons
  for (setting=0; setting<nb_settings; setting++)
  {
    if (min_value[setting]==0 && max_value[setting]==1 && decimal_places[setting]<0)
    {
      word first_setting=setting;
      for (;setting+1<nb_settings; setting++)
        if (min_value[setting+1]!=0 || max_value[setting+1]!=1 || decimal_places[setting+1]!=decimal_places[first_setting])
          break;
      
      Window_display_frame_in(12+max_label_length*8+41,18+first_setting*17,18,(setting-first_setting+1)*17-1);
    }
  }
  
  Update_window_area(0,0,Window_width, Window_height);
  Cursor_shape=CURSOR_SHAPE_ARROW;
  Display_cursor();

  while (!close_window)
  {
    clicked_button=Window_clicked_button();
    if (clicked_button>0)
    {
      setting = control[clicked_button] & (CONTROL_VALUE_MASK);
      
      switch (control[clicked_button] & CONTROL_TYPE_MASK)
      {
        case CONTROL_OK:
          close_window = CONTROL_OK;
          break;
          
        case CONTROL_CANCEL:
          close_window = CONTROL_CANCEL;
          break;
          
        case CONTROL_INPUT:

          Sprint_double(str,current_value[setting],decimal_places[setting],0);
          Readline_ex(12+max_label_length*8+23, 22+setting*17,str,7,40,INPUT_TYPE_DECIMAL,decimal_places[setting]);
          current_value[setting]=atof(str);

          if (current_value[setting] < min_value[setting])
            current_value[setting] = min_value[setting];
          else if (current_value[setting] > max_value[setting])
            current_value[setting] = max_value[setting];
          // Print editable value
          Sprint_double(str,current_value[setting],decimal_places[setting],7);
          Print_in_window_limited(12+max_label_length*8+23, 22+setting*17, str, 7,MC_Black,MC_Light);
          //
          Display_cursor();
          
          break;
          
        case CONTROL_INPUT_MINUS:
          if (current_value[setting] > min_value[setting])
          {
            current_value[setting]--;
            if (current_value[setting] < min_value[setting])
              current_value[setting] = min_value[setting];
              
            Hide_cursor();
            // Print editable value
            Sprint_double(str,current_value[setting],decimal_places[setting],7);
            Print_in_window_limited(12+max_label_length*8+23, 22+setting*17, str, 7,MC_Black,MC_Light);
            //
            Display_cursor();
          }
          break;
          
        case CONTROL_INPUT_PLUS:
          if (current_value[setting] < max_value[setting])
          {
            current_value[setting]++;
            if (current_value[setting] > max_value[setting])
              current_value[setting] = max_value[setting];
            
            Hide_cursor();
            // Print editable value
            Sprint_double(str,current_value[setting],decimal_places[setting],7);
            Print_in_window_limited(12+max_label_length*8+23, 22+setting*17, str, 7,MC_Black,MC_Light);
            //
            Display_cursor();
          }
          break;
          
        case CONTROL_CHECKBOX:
          if (decimal_places[setting]==0 || current_value[setting]==0.0)
          {
            current_value[setting] = (current_value[setting]==0.0);
            Hide_cursor();
            Print_in_window(12+max_label_length*8+46, 22+setting*17, current_value[setting]?"X":" ",MC_Black,MC_Light);
            // Uncheck other buttons of same family
            if (decimal_places[setting]<0)
            {
              byte button;
              for (button=3; button<=Window_nb_buttons; button++)
              {
                if (button != clicked_button && control[button] & CONTROL_CHECKBOX)
                {
                  byte other_setting = control[button] & (CONTROL_VALUE_MASK);
                  if (decimal_places[other_setting] == decimal_places[setting])
                  {
                    // Same family: unset and uncheck
                    current_value[other_setting]=0.0;
                    Print_in_window(12+max_label_length*8+46, 22+other_setting*17, " ",MC_Black,MC_Light);
                  }
                }
              }
            }
            Display_cursor();
          }
          break;
      }
    }
  }
  
  Close_window();
  Cursor_shape=CURSOR_SHAPE_HOURGLASS;
  Display_cursor();
  
  // Return values:

  // One boolean to tell if user pressed ok or cancel
  lua_pushboolean(L, (close_window == CONTROL_OK));
  
  // One value per control
  for (setting=0; setting<nb_settings; setting++)
    lua_pushnumber(L, current_value[setting]);
    
  return 1 + nb_settings;
}

int L_SelectBox(lua_State* L)
{
  const int max_settings = 10;
  const char * label[max_settings];

  const char * window_caption;
  unsigned int caption_length;
  int nb_args;
  
  unsigned int max_label_length;
  int button;
  int nb_buttons;
  short clicked_button;
  //char  str[40];
  //short close_window = 0;
  
  nb_args = lua_gettop (L);
  
  if (nb_args < 2)
  {
    return luaL_error(L, "selectbox: Less than 2 arguments");
  }
  if ((nb_args - 1) % 2)
  {
    return luaL_error(L, "selectbox: Wrong number of arguments");
  }
  
  
  nb_buttons=(nb_args-1) /2;
  
  max_label_length=4; // Minimum size
  
  // First argument is window caption
  LUA_ARG_STRING(1, "selectbox", window_caption);
  caption_length = strlen(window_caption);
  if ( caption_length > max_label_length)
    max_label_length = caption_length;
  
  for (button=0; button<nb_buttons; button++)
  {
    LUA_ARG_STRING(button*2+2, "selectbox", label[button]);
    if (strlen(label[button]) > max_label_length)
      max_label_length = strlen(label[button]);
    LUA_ARG_FUNCTION(button*2+3, "selectbox");
  }
  // Max is 25 to keep window under 320 pixel wide
  if (max_label_length>25)
    max_label_length=25;

  Update_colors_during_script();
  Open_window(28+max_label_length*8,26+nb_buttons*17,window_caption);

  for (button=0; button<nb_buttons; button++)
  {
    Window_set_normal_button( 7, 22 + 17 * button, 14+max_label_length*8,14,label[button], 0,1,KEY_NONE);
  }

  Update_window_area(0,0,Window_width, Window_height);
  Cursor_shape=CURSOR_SHAPE_ARROW;
  Display_cursor();
 
  Key=0;
  do
  {
    clicked_button=Window_clicked_button();
  } while (clicked_button<1 && Key != KEY_ESC);
    
  Close_window();
  Cursor_shape=CURSOR_SHAPE_HOURGLASS;
  Display_cursor();

  if (clicked_button)
  {
    // Push the function on top of the stack
    lua_pushvalue(L, 1+2*clicked_button);
    // Call it
    lua_call(L, 0, 0);
  }
  // No return value:    
  return 0;
}

int L_MessageBox(lua_State* L)
{
  const char * caption;
  const char * message;
  int nb_args = lua_gettop (L);
  
  if (nb_args == 1)
  {
    caption = "Script message";
    LUA_ARG_STRING(1, "messagebox", message);
  }
  else if (nb_args == 2)
  {
    LUA_ARG_STRING(1, "messagebox", caption);
    LUA_ARG_STRING(2, "messagebox", message);
  }
  else
  {
    return luaL_error(L, "messagebox: Needs one or two arguments.");
  }

  Update_colors_during_script();
  Verbose_message(caption, message);
  return 0;
}

int L_Wait(lua_State* L)
{
  float delay;
  Uint32 end;
  int nb_args=lua_gettop(L);

  LUA_ARG_LIMIT (1, "wait");
  LUA_ARG_NUMBER(1, "wait", delay, 0.0, 10.0);

  // Simple 'yield'
  if (delay == 0.0)
  {
    Get_input(0);
    return 0;
  }
  // Wait specified time
  end = SDL_GetTicks()+(int)(delay*1000.0);
  
  do
  {
    Get_input(20);
  } while (SDL_GetTicks()<end);
  return 0;
}

int L_WaitBreak(lua_State* L)
{
  float delay;
  Uint32 end;
  int nb_args=lua_gettop(L);
  
  LUA_ARG_LIMIT (1, "waitbreak");
  LUA_ARG_NUMBER(1, "waitbreak", delay, 0.0, DBL_MAX);

  // Simple 'yield'
  if (delay == 0.0)
  {
    Get_input(0);
    lua_pushinteger(L, (Key == KEY_ESC));
    return 1;
  }
  // Wait specified time
  end = SDL_GetTicks()+(int)(delay*1000.0);
  
  do
  {
    Get_input(20);
    if (Key == KEY_ESC)
    {
      lua_pushinteger(L, 1);
      return 1;
    }
  } while (SDL_GetTicks()<end);

  lua_pushinteger(L, 0);
  return 1;
}

int L_WaitInput(lua_State* L)
{
  float delay;
  Uint32 end;
  int nb_args=lua_gettop(L);
  int moved;
  
  /*word key;
  word button;
  word mouse_x;
  word mouse_y;*/
  
  LUA_ARG_LIMIT (1, "waitinput");
  LUA_ARG_NUMBER(1, "waitinput", delay, 0.0, DBL_MAX);

  // Simple 'yield'
  if (delay == 0.0)
  {
    moved=Get_input(0);
  }
  else
  {
    // Wait specified time
    end = SDL_GetTicks()+(int)(delay*1000.0);
  
    moved=0;
    do
    {
      moved=Get_input(20);
    } while (!moved && SDL_GetTicks()<end);
  }
  
  lua_pushboolean(L, moved ? 1 : 0);
  lua_pushinteger(L, (Key == KEY_ESC) ? SDLK_ESCAPE : Key);
  lua_pushinteger(L, Mouse_X);
  lua_pushinteger(L, Mouse_Y);
  lua_pushinteger(L, Mouse_K);
  
  // The event arguments are in the stack.
  // Need to reset "Key" here, so that a key event will not be
  // created again before an actual key repeat occurs.
  Key=0;
  
  return 5;
}

int L_UpdateScreen(lua_State* L)
{
  int nb_args=lua_gettop(L);
  
  LUA_ARG_LIMIT (0, "updatescreen");
  
  Update_colors_during_script();
  Hide_cursor();
  Display_all_screen();
  Display_cursor();
  //Flush_update();

  return 0;
}


int L_StatusMessage(lua_State* L)
{
	const char* msg;
	int nb_args = lua_gettop(L);
	LUA_ARG_LIMIT(1,"statusmessage");

	LUA_ARG_STRING(1, "statusmessage", msg);
	Print_in_menu(msg,0);
	return 0;
}


int L_FinalizePicture(lua_State* L)
{
  int nb_args=lua_gettop(L);
  
  LUA_ARG_LIMIT (0, "finalizepicture");
  
  Update_colors_during_script();
  End_of_modification();
  Backup();
  
  return 0;
}

// Handlers for window internals
T_Fileselector Scripts_selector;

// Callback to display a skin name in the list
void Draw_script_name(word x, word y, word index, byte highlighted)
{
  T_Fileselector_item * current_item;

  if (Scripts_selector.Nb_elements)
  {
    byte fg, bg;
    
    current_item = Get_item_by_index(&Scripts_selector, index);

    if (current_item->Type==1) // Directories
    {
      fg=(highlighted)?MC_Black:MC_Dark;
      bg=(highlighted)?MC_Dark:MC_Light;
    }
    else // Files
    {
      fg=MC_Black;
      bg=(highlighted)?MC_Dark:MC_Light;
    }

    Print_in_window(x, y, current_item->Short_name, fg,bg);

    Update_window_area(x,y,NAME_WIDTH*8,8);
  }
}

///
/// Displays first lines of comments from a lua script in the window.
void Draw_script_information(T_Fileselector_item * script_item)
{
  FILE *script_file;
  char full_name[MAX_PATH_CHARACTERS];
  char text_block[3][DESC_WIDTH+1];
  int x, y;
  int i;
  
  // Blank the target area
  Window_rectangle(7, FILESEL_Y + 89, DESC_WIDTH*6+2, 4*8, MC_Black);

  if (script_item && script_item->Full_name && script_item->Full_name[0]!='\0')
  {
    strcpy(full_name, Data_directory);
    strcat(full_name, "scripts/");
    strcat(full_name, script_item->Full_name);
    
    
    x=0;
    y=0;
    text_block[0][0] = text_block[1][0] = text_block[2][0] = '\0';
    // Start reading
    script_file = fopen(full_name, "r");
    if (script_file != NULL)
    {
      int c;
      c = fgetc(script_file);
      while (c != EOF && y<3)
      {
        if (c == '\n')
        {
          if (x<2)
            break; // Carriage return without comment: Stopping
          y++;
          x=0;
        }
        else if (x==0 || x==1)
        {
          if (c != '-')
            break; // Non-comment line was encountered. Stopping.       
          x++;
        }
        else
        {
          if (x < DESC_WIDTH+2)
          {
            // Adding character
            text_block[y][x-2] = (c<32 || c>255) ? ' ' : c;
            text_block[y][x-1] = '\0';
          }
          x++;
        }
        // Read next
        c = fgetc(script_file);
      }
      fclose(script_file);
    }
    
    Print_help(8, FILESEL_Y + 89   , text_block[0], 'N', 0, 0);
    Print_help(8, FILESEL_Y + 89+ 8, text_block[1], 'N', 0, 0);
    Print_help(8, FILESEL_Y + 89+16, text_block[2], 'N', 0, 0);
  
    // Display a line with the keyboard shortcut
    Print_help(8, FILESEL_Y + 89+24, "Key:", 'N', 0, 0);
    for (i=0; i<10; i++)
      if (Bound_script[i]!=NULL && !strcmp(Bound_script[i], script_item->Full_name))
        break;
  
    if (i<10)
    {
      const char *shortcut;    
      shortcut=Keyboard_shortcut_value(SPECIAL_RUN_SCRIPT_1+i);
      Print_help(8+4*6, FILESEL_Y + 89+24, shortcut, 'K', 0, strlen(shortcut));
    }
    else
    {
      Print_help(8+4*6, FILESEL_Y + 89+24, "None", 'K', 0, 4);
    }
  }
  

  
  Update_window_area(8, FILESEL_Y + 89, DESC_WIDTH*6+2, 4*8);
    
}

// Add a script to the list
void Add_script(const char *name, byte is_file, byte is_directory, byte is_hidden)
{
  const char * file_name;
  int len;

  file_name=Find_last_slash(name)+1;

  if (is_file)
{
    // Only files ending in ".lua"
    len=strlen(file_name);
    if (len<=4 || strcasecmp(file_name+len-4, ".lua"))
      return;
    // Hidden
    //if (is_hidden && !Config.Show_hidden_files)
    //  return;

    Add_element_to_list(&Scripts_selector, file_name, Format_filename(file_name, NAME_WIDTH+1, 0), 0, ICON_NONE);
  }
  else if (is_directory)
  {
    // Ignore current directory
    if ( !strcmp(file_name, "."))
      return;
    // Hidden
    //if (is_hidden && !Config.Show_hidden_directories)
    //  return;
    
    Add_element_to_list(&Scripts_selector, file_name, Format_filename(file_name, NAME_WIDTH+1, 1), 1, ICON_NONE);
    }
}

void Highlight_script(T_Fileselector *selector, T_List_button *list, const char *selected_file)
    {
  short index;

  index=Find_file_in_fileselector(selector, selected_file);
  Locate_list_item(list, selector, index);
    }

static char Last_run_script[MAX_PATH_CHARACTERS]="";

// Before: Cursor hidden
// After: Cursor shown
void Run_script(const char *script_subdirectory, const char *script_filename)
{
  lua_State* L;
  const char* message;
  byte  old_cursor_shape=Cursor_shape;
  char scriptdir[MAX_PATH_CHARACTERS];

  strcpy(scriptdir, Data_directory);
  strcat(scriptdir, "scripts/");

  // Some scripts are slow
  Cursor_shape=CURSOR_SHAPE_HOURGLASS;
  Display_cursor();
  Flush_update();
  
  chdir(scriptdir);

  if (script_subdirectory && script_subdirectory[0]!='\0')
  {
    sprintf(Last_run_script, "%s%s%s", script_subdirectory, PATH_SEPARATOR, script_filename);
  }
  else
  {
    strcpy(Last_run_script, script_filename);
  }
  

  L = lua_open();
  putenv("LUA_PATH=libs\\?.lua");

  lua_register(L,"putbrushpixel",L_PutBrushPixel);
  lua_register(L,"getbrushpixel",L_GetBrushPixel);
  lua_register(L,"getbrushbackuppixel",L_GetBrushBackupPixel);
  lua_register(L,"putpicturepixel",L_PutPicturePixel);
  lua_register(L,"getpicturepixel",L_GetPicturePixel);
  lua_register(L,"getlayerpixel",L_GetLayerPixel);
  lua_register(L,"getbackuppixel",L_GetBackupPixel);
  lua_register(L,"setbrushsize",L_SetBrushSize);
  lua_register(L,"setpicturesize",L_SetPictureSize);
  lua_register(L,"getbrushsize",L_GetBrushSize);
  lua_register(L,"getpicturesize",L_GetPictureSize);
  lua_register(L,"setcolor",L_SetColor);
  lua_register(L,"getcolor",L_GetColor);
  lua_register(L,"getbackupcolor",L_GetBackupColor);
  lua_register(L,"matchcolor",L_MatchColor);
  lua_register(L,"getbrushtransparentcolor",L_GetBrushTransparentColor);
  lua_register(L,"inputbox",L_InputBox);
  lua_register(L,"messagebox",L_MessageBox);
  lua_register(L,"statusmessage",L_StatusMessage);
  lua_register(L,"selectbox",L_SelectBox);
  lua_register(L,"getforecolor",L_GetForeColor);
  lua_register(L,"getbackcolor",L_GetBackColor);
  lua_register(L,"setforecolor",L_SetForeColor);
  lua_register(L,"setbackcolor",L_SetBackColor);
  lua_register(L,"gettranscolor",L_GetTransColor);
  lua_register(L,"getsparepicturesize ",L_GetSparePictureSize);
  lua_register(L,"getsparelayerpixel ",L_GetSpareLayerPixel);
  lua_register(L,"getsparepicturepixel",L_GetSparePicturePixel);
  lua_register(L,"getsparecolor",L_GetSpareColor);
  lua_register(L,"getsparetranscolor",L_GetSpareTransColor);
  lua_register(L,"clearpicture",L_ClearPicture);
  lua_register(L,"wait",L_Wait);
  lua_register(L,"waitbreak",L_WaitBreak);
  lua_register(L,"waitinput",L_WaitInput);
  lua_register(L,"updatescreen",L_UpdateScreen);
  lua_register(L,"finalizepicture",L_FinalizePicture);
  
  // Load all standard libraries
  luaL_openlibs(L);

  /*
  luaopen_base(L);
  //luaopen_package(L); // crashes on Windows, for unknown reason
  luaopen_table(L);
  //luaopen_io(L); // crashes on Windows, for unknown reason
  //luaopen_os(L);
  luaopen_string(L);
  luaopen_math(L);
  //luaopen_debug(L);
  */

  // TODO The script may modify the picture, so we do a backup here.
  // If the script is only touching the brush, this isn't needed...
  // The backup also allows the script to read from it to make something
  // like a feedback off effect (convolution matrix comes to mind).
  Backup();

  Palette_has_changed=0;
  Brush_was_altered=0;
  Original_back_color=Back_color;
  Original_fore_color=Fore_color;

  // Backup the brush
  Brush_backup=(byte *)malloc(((long)Brush_height)*Brush_width);
  Brush_backup_width = Brush_width;
  Brush_backup_height = Brush_height;
  
  if (Brush_backup == NULL)
  {
    Verbose_message("Error!", "Out of memory!");
  }
  else 
  {
    memcpy(Brush_backup, Brush, ((long)Brush_height)*Brush_width);
  
    if (luaL_loadfile(L,Last_run_script) != 0)
    {
      int stack_size;
      stack_size= lua_gettop(L);
      if (stack_size>0 && (message = lua_tostring(L, stack_size))!=NULL)
        Verbose_message("Error!", message);
      else
        Warning_message("Unknown error loading script!");
    }
    else if (lua_pcall(L, 0, 0, 0) != 0)
    {
      int stack_size;
      stack_size= lua_gettop(L);
      
      Update_colors_during_script();
      
      if (stack_size>0 && (message = lua_tostring(L, stack_size))!=NULL)
        Verbose_message("Error running script", message);
      else
        Warning_message("Unknown error running script!");
    }
  }
  // Cleanup
  free(Brush_backup);
  Brush_backup=NULL;
  Update_colors_during_script();
  End_of_modification();

  lua_close(L);
  
  if (Brush_was_altered)
    Change_paintbrush_shape(PAINTBRUSH_SHAPE_COLOR_BRUSH);

  Hide_cursor();
  Display_all_screen();
  // Update changed pen colors.
  if (Back_color!=Original_back_color || Fore_color!=Original_fore_color)
  {
    // This is done at end of script, in case somebody would use the
    // functions in a custom window.
    if (Back_color!=Original_back_color)
    {
      byte new_color = Back_color;
      Back_color = Original_back_color;
      Set_back_color(new_color);
    }
    if (Fore_color!=Original_fore_color)
    {
      byte new_color = Fore_color;
      Fore_color = Original_fore_color;
      Set_fore_color(new_color);
    }
    
  }
  Cursor_shape=old_cursor_shape;
  Display_cursor();
}

void Run_numbered_script(byte index)
{

  if (index>=10)
    return;
  if (Bound_script[index]==NULL)
    return;
    
  Hide_cursor();
  Run_script(NULL, Bound_script[index]);
}               

void Repeat_script(void)
{

  if (Last_run_script==NULL || Last_run_script[0]=='\0')
  {
    Warning_message("No script to repeat.");
    return;
  }

  Hide_cursor();
  Run_script(NULL, Last_run_script);
}

void Set_script_shortcut(T_Fileselector_item * script_item)
{
  int i;
  char full_name[MAX_PATH_CHARACTERS];
  
  if (script_item && script_item->Full_name && script_item->Full_name[0]!='\0')
  {
    strcpy(full_name, Data_directory);
    strcat(full_name, "scripts/");
    strcat(full_name, script_item->Full_name);
    
    // Find if it already has a shortcut
    for (i=0; i<10; i++)
      if (Bound_script[i]!=NULL && !strcmp(Bound_script[i], script_item->Full_name))
        break;
    if (i<10)
    {
      // Existing shortcut
    }
    else
    {
      // Try to find a "free" one.
      for (i=0; i<10; i++)
        if (Bound_script[i]==NULL
          || !Has_shortcut(SPECIAL_RUN_SCRIPT_1+i)
          || !File_exists(full_name))
          break;
      if (i<10)
      {
        free(Bound_script[i]);
        Bound_script[i]=strdup(script_item->Full_name);
      }
      else
      {
        Warning_message("Already 10 scripts have shortcuts.");
        return;
      }
    }
    Window_set_shortcut(SPECIAL_RUN_SCRIPT_1+i);
    if (!Has_shortcut(SPECIAL_RUN_SCRIPT_1+i))
    {
      // User cancelled or deleted all shortcuts
      free(Bound_script[i]);
      Bound_script[i]=NULL;
    }
    // Refresh display
    Hide_cursor();
    Draw_script_information(script_item);
    Display_cursor();
  }
}

void Button_Brush_Factory(void)
{
  static char selected_file[MAX_PATH_CHARACTERS]="";
  static char sub_directory[MAX_PATH_CHARACTERS]="";

  short clicked_button;
  T_List_button* scriptlist;
  T_Scroller_button* scriptscroll;
  T_Special_button* scriptarea;
  char scriptdir[MAX_PATH_CHARACTERS];
  T_Fileselector_item *item;

  // Reinitialize the list
  Free_fileselector_list(&Scripts_selector);
  strcpy(scriptdir, Data_directory);
  strcat(scriptdir, "scripts/");
  strcat(scriptdir, sub_directory);
  // Add each found file to the list
  For_each_directory_entry(scriptdir, Add_script);
  // Sort it
  Sort_list_of_files(&Scripts_selector);
  //

  Open_window(33+8*NAME_WIDTH, 180, "Brush Factory");
  
  Window_set_normal_button(85, 149, 67, 14, "Cancel", 0, 1, KEY_ESC); // 1

  Window_display_frame_in(6, FILESEL_Y - 2, NAME_WIDTH*8+4, 84); // File selector
    // Fileselector
  scriptarea=Window_set_special_button(8, FILESEL_Y + 1, NAME_WIDTH*8, 80); // 2
    // Scroller for the fileselector
  scriptscroll = Window_set_scroller_button(NAME_WIDTH*8+14, FILESEL_Y - 1, 82,
      Scripts_selector.Nb_elements,10, 0); // 3
  scriptlist = Window_set_list_button(scriptarea,scriptscroll,Draw_script_name); // 4

  Window_set_normal_button(10, 149, 67, 14, "Run", 0, 1, SDLK_RETURN); // 5

  Window_display_frame_in(6, FILESEL_Y + 88, DESC_WIDTH*6+4, 4*8+2); // Descr.
  Window_set_special_button(7, FILESEL_Y + 89+24,DESC_WIDTH*6,8); // 6
    
  while (1)
  {
    // Locate selected file in view
    Highlight_script(&Scripts_selector, scriptlist, selected_file);
  // Update the scroller position
  scriptscroll->Position=scriptlist->List_start;
    Window_draw_slider(scriptscroll);
  
  Window_redraw_list(scriptlist);
    Draw_script_information(Get_item_by_index(&Scripts_selector,
    scriptlist->List_start + scriptlist->Cursor_position));
  
  Update_window_area(0, 0, Window_width, Window_height);
  Display_cursor();

    Reset_quicksearch();
  
  do
  {
    clicked_button = Window_clicked_button();
      if (Key==SDLK_BACKSPACE && sub_directory[0]!='\0')
      {
        // Make it select first entry (parent directory)
        scriptlist->List_start=0;
        scriptlist->Cursor_position=0;
        clicked_button=5;
      }
    if (Is_shortcut(Key,0x100+BUTTON_HELP))
      Window_help(BUTTON_BRUSH_EFFECTS, "BRUSH FACTORY");
    else if (Is_shortcut(Key,0x200+BUTTON_BRUSH_EFFECTS))
      clicked_button=1; // Cancel
      // Quicksearch
      if (clicked_button==4)
        Reset_quicksearch();
      else if (clicked_button==0 && Key_ANSI)
        clicked_button=Quicksearch_list(scriptlist, &Scripts_selector);

    switch (clicked_button)
    {
      case 4:
        Hide_cursor();
          Draw_script_information(Get_item_by_index(&Scripts_selector,
          scriptlist->List_start + scriptlist->Cursor_position));
        Display_cursor();
          {
            // Test double-click
            static long time_click = 0;
            static int last_selected_item=-1;
            static long time_previous;
            
            time_previous = time_click;
            time_click = SDL_GetTicks();
            if (scriptlist->List_start + scriptlist->Cursor_position == last_selected_item)
            {
              if (time_click - time_previous < Config.Double_click_speed)
                clicked_button=5;
            }
            else
            {
              last_selected_item=scriptlist->List_start + scriptlist->Cursor_position;
            }
          }
        break;
      
      case 6:
          Set_script_shortcut(Get_item_by_index(&Scripts_selector,
          scriptlist->List_start + scriptlist->Cursor_position));
        break;
        
      default:
        break;
    }
    
  } while (clicked_button != 1 && clicked_button != 5);
  
    // Cancel
    if (clicked_button==1)
      break;
      
    // OK
    if (Scripts_selector.Nb_elements == 0)
  {
      // No items : same as Cancel
      clicked_button=1;
      break;
    }
  
    // Examine selected file
    item = Get_item_by_index(&Scripts_selector,
      scriptlist->List_start + scriptlist->Cursor_position);
    
    if (item->Type==0) // File
    {
      strcpy(selected_file, sub_directory);
      strcat(selected_file, item->Full_name);
      break;
    }
    else if (item->Type==1) // Directory
    {
      if (!strcmp(item->Full_name,PARENT_DIR))
      {
        // Going up one directory
        long len;
        char * slash_pos;

        // Remove trailing slash      
        len=strlen(sub_directory);
        if (len && !strcmp(sub_directory+len-1,PATH_SEPARATOR))
          sub_directory[len-1]='\0';
        
        slash_pos=Find_last_slash(sub_directory);
        if (slash_pos)
        {
          strcpy(selected_file, slash_pos+1);
          *slash_pos='\0';
        }
    else
        {
          strcpy(selected_file, sub_directory);
          sub_directory[0]='\0';
  }
      }
      else
      {
        // Going down one directory
        strcpy(selected_file, PARENT_DIR);
  
        strcat(sub_directory, item->Full_name);
        strcat(sub_directory, PATH_SEPARATOR);
      }

      // No break: going back up to beginning of loop
      
      // Reinitialize the list
      Free_fileselector_list(&Scripts_selector);
      strcpy(scriptdir, Data_directory);
      strcat(scriptdir, "scripts/");
      strcat(scriptdir, sub_directory);
      // Add each found file to the list
      For_each_directory_entry(scriptdir, Add_script);
      // Sort it
      Sort_list_of_files(&Scripts_selector); 
      //
      
      scriptlist->Scroller->Nb_elements=Scripts_selector.Nb_elements;
      Compute_slider_cursor_length(scriptlist->Scroller);
      
      Hide_cursor();
    }
  }
  
  Close_window();
  Unselect_button(BUTTON_BRUSH_EFFECTS);

  if (clicked_button == 5) // Run the script
  {
    Run_script("", selected_file);
  }
  else
  {
    Display_cursor();
  }
}

#else // NOLUA
void Button_Brush_Factory(void)
{
    Verbose_message("Error!", "The brush factory is not available in this build of GrafX2.");
}

#endif
