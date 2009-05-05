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
    along with Grafx2; if not, see <http://www.gnu.org/licenses/>
*/

//////////////////////////////////////////////////////////////////////////////
///@file input.h
/// Functions for mouse, keyboard and joystick input.
/// Joystick input is used to emulate mouse on platforms that don't have a
/// pointing device, ie: the GP2X.
//////////////////////////////////////////////////////////////////////////////

///
/// This is the keyboard/mouse/joystick input polling function.
/// Returns 1 if a significant changed occurred, such as a mouse button pressed
/// or depressed, or a new keypress was in the keyboard buffer.
/// The latest input variables are held in ::Key, ::Key_ANSI, ::Mouse_X, ::Mouse_Y, ::Mouse_K.
/// Note that ::Key and ::Key_ANSI are not persistent, they will be reset to 0
/// on subsequent calls to ::Get_input().
int  Get_input(void);

/// Returns true if the keycode has been set as a keyboard shortcut for the function.
int Is_shortcut(word Key, word function);
