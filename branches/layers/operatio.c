/*  Grafx2 - The Ultimate 256-color bitmap paint program

    Copyright 2007 Adrien Destugues
    Copyright 2009 Franck Charlet
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
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "const.h"
#include "struct.h"
#include "global.h"
#include "misc.h"
#include "engine.h"
#include "graph.h"
#include "operatio.h"
#include "buttons.h"
#include "pages.h"
#include "errors.h"
#include "sdlscreen.h"
#include "brush.h"
#include "windows.h"

#if defined(__GP2X__)
    #define M_PI 3.14159265358979323846 
#endif

/// Time (in SDL ticks) when the next airbrush drawing should be done. Also used
/// for discontinuous freehand drawing.
Uint32 Airbrush_next_time;

void Start_operation_stack(word new_operation)
{
  Brush_rotation_center_is_defined=0;

  // On m�morise l'op�ration pr�c�dente si on d�marre une interruption
  switch(new_operation)
  {
    case OPERATION_MAGNIFY         :
    case OPERATION_COLORPICK       :
    case OPERATION_GRAB_BRUSH  :
    case OPERATION_POLYBRUSH    :
    case OPERATION_STRETCH_BRUSH :
    case OPERATION_ROTATE_BRUSH:
      Operation_before_interrupt=Current_operation;
      // On passe � l'operation demand�e
      Current_operation=new_operation;
      break;
    default :
      // On passe � l'operation demand�e
      Current_operation=new_operation;
      Operation_before_interrupt=Current_operation;
  }

  // On sp�cifie si l'op�ration autorise le changement de couleur au clavier
  switch(new_operation)
  {
    case OPERATION_CONTINUOUS_DRAW:
    case OPERATION_DISCONTINUOUS_DRAW:
    case OPERATION_AIRBRUSH:
    case OPERATION_CENTERED_LINES:
      Allow_color_change_during_operation=1;
      break;
    default :
      Allow_color_change_during_operation=0;
  }

  // Et on passe au curseur qui va avec
  Cursor_shape=CURSOR_FOR_OPERATION[new_operation];
  Operation_stack_size=0;
}


void Init_start_operation(void)
{
  Operation_in_magnifier=(Mouse_X>=Main_X_zoom);
  Smear_start=1;
}


void Operation_push(short value)
{
  Operation_stack[++Operation_stack_size]=value;
}


void Operation_pop(short * value)
{
  *value=Operation_stack[Operation_stack_size--];
}


byte Paintbrush_shape_before_operation;
byte Paintbrush_hidden_before_scroll;



short Distance(short x1, short y1, short x2, short y2)
{
  short x2_moins_x1=x2-x1;
  short y2_minus_y1=y2-y1;

  return Round( sqrt( (x2_moins_x1*x2_moins_x1) + (y2_minus_y1*y2_minus_y1) ) );
}


void Display_coords_rel_or_abs(short start_x, short start_y)
{
  char str[6];

  if (Config.Coords_rel)
  {
    if (Menu_is_visible)
    {
      if (Paintbrush_X>start_x)
      {
        Num2str(Paintbrush_X-start_x,str,5);
        str[0]='+';
      }
      else if (Paintbrush_X<start_x)
      {
        Num2str(start_x-Paintbrush_X,str,5);
        str[0]='-';
      }
      else
        strcpy(str,"�   0");
      Print_in_menu(str,2);

      if (Paintbrush_Y>start_y)
      {
        Num2str(Paintbrush_Y-start_y,str,5);
        str[0]='+';
      }
      else if (Paintbrush_Y<start_y)
      {
        Num2str(start_y-Paintbrush_Y,str,5);
        str[0]='-';
      }
      else
        strcpy(str,"�   0");
      Print_in_menu(str,12);
    }
  }
  else
    Print_coordinates();
}

/// Simulates clicking the "Draw" button.
void Return_to_draw_mode(void)
{

  // Comme l'enclenchement du bouton efface le curseur, il faut l'afficher au
  // pr�alable:
  Display_cursor();
  if (Mouse_K)
    Wait_end_of_click();
  // !!! Efface la croix puis affiche le viseur !!!
  Select_button(BUTTON_DRAW,LEFT_SIDE); // D�senclenche au passage le bouton brosse
  if (Config.Auto_discontinuous)
  {
    // On se place en mode Dessin discontinu � la main
    while (Current_operation!=OPERATION_DISCONTINUOUS_DRAW)
      Select_button(BUTTON_DRAW,RIGHT_SIDE);
  }
  // Maintenant, il faut r�effacer le curseur parce qu'il sera raffich� en fin
  // d'appel � cette action:
  Hide_cursor();

  // On passe en brosse couleur:
  Change_paintbrush_shape(PAINTBRUSH_SHAPE_COLOR_BRUSH);

  if ((Config.Coords_rel) && (Menu_is_visible))
    Print_in_menu("X:       Y:",0);
  Print_coordinates();
}

//////////////////////////////////////////////////// OPERATION_CONTINUOUS_DRAW

void Freehand_mode1_1_0(void)
//  Op�ration   : OPERATION_CONTINUOUS_DRAW
//  Click Souris: 1
//  Taille_Pile : 0
//
//  Souris effac�e: Oui
{
  Init_start_operation();
  Backup();
  Shade_table=Shade_table_left;
  // On affiche d�finitivement le pinceau
  Display_paintbrush(Paintbrush_X,Paintbrush_Y,Fore_color,0);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


void Freehand_mode1_1_2(void)
//  Op�ration   : OPERATION_CONTINUOUS_DRAW
//  Click Souris: 1
//  Taille_Pile : 2
//
//  Souris effac�e: Non
{
  short start_x;
  short start_y;

  Operation_pop(&start_y);
  Operation_pop(&start_x);

  if ( (start_y!=Paintbrush_Y) || (start_x!=Paintbrush_X) )
  {
    Hide_cursor();
    Print_coordinates();
    Draw_line_permanent(start_x,start_y,Paintbrush_X,Paintbrush_Y,Fore_color);
    Display_cursor();
  }

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


void Freehand_mode12_0_2(void)
//  Op�ration   : OPERATION_DESSIN_[DIS]CONTINU
//  Click Souris: 0
//  Taille_Pile : 2
//
//  Souris effac�e: Non
{
  Operation_stack_size=0;
  End_of_modification();
}


void Freehand_mode1_2_0(void)
//  Op�ration   : OPERATION_CONTINUOUS_DRAW
//  Click Souris: 2
//  Taille_Pile : 0
//
//  Souris effac�e: Oui
{
  Init_start_operation();
  Backup();
  Shade_table=Shade_table_right;
  // On affiche d�finitivement le pinceau
  Display_paintbrush(Paintbrush_X,Paintbrush_Y,Back_color,0);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


void Freehand_mode1_2_2(void)
//  Op�ration   : OPERATION_CONTINUOUS_DRAW
//  Click Souris: 2
//  Taille_Pile : 2
//
//  Souris effac�e: Non
{
  short start_x;
  short start_y;

  Operation_pop(&start_y);
  Operation_pop(&start_x);

  if ( (start_y!=Paintbrush_Y) || (start_x!=Paintbrush_X) )
  {
    Print_coordinates();
    Hide_cursor();
    Draw_line_permanent(start_x,start_y,Paintbrush_X,Paintbrush_Y,Back_color);
    Display_cursor();
  }

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


///////////////////////////////////////////////// OPERATION_DISCONTINUOUS_DRAW

void Freehand_mode2_1_0(void)
//  Op�ration   : OPERATION_DISCONTINUOUS_DRAW
//  Click Souris: 1
//  Taille_Pile : 0
//
//  Souris effac�e: Oui
{
  Init_start_operation();
  Backup();
  Shade_table=Shade_table_left;
  // On affiche d�finitivement le pinceau
  Display_paintbrush(Paintbrush_X,Paintbrush_Y,Fore_color,0);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Print_coordinates();
  Airbrush_next_time = SDL_GetTicks() + Airbrush_delay*10;
}


void Freehand_mode2_1_2(void)
//  Op�ration   : OPERATION_DISCONTINUOUS_DRAW
//  Click Souris: 1
//  Taille_Pile : 2
//
//  Souris effac�e: Non
{
  short start_x;
  short start_y;

  Operation_pop(&start_y);
  Operation_pop(&start_x);

  if ( (start_x!=Paintbrush_X) || (start_y!=Paintbrush_Y) )
  {
    Print_coordinates();
	if (SDL_GetTicks()>Airbrush_next_time)
	{
		Airbrush_next_time+=Airbrush_delay*10;
    	Hide_cursor();
    	// On affiche d�finitivement le pinceau
    	Display_paintbrush(Paintbrush_X,Paintbrush_Y,Fore_color,0);
    	Display_cursor();
	}
  }

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}

// ----------

void Freehand_mode2_2_0(void)
//  Op�ration   : OPERATION_DISCONTINUOUS_DRAW
//  Click Souris: 2
//  Taille_Pile : 0
//
//  Souris effac�e: Oui
{
  Init_start_operation();
  Backup();
  Shade_table=Shade_table_right;
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Print_coordinates();
	Airbrush_next_time = SDL_GetTicks() + Airbrush_delay*10;
  	// On affiche d�finitivement le pinceau
 	Display_paintbrush(Paintbrush_X,Paintbrush_Y,Back_color,0);
}


void Freehand_mode2_2_2(void)
//  Op�ration   : OPERATION_DISCONTINUOUS_DRAW
//  Click Souris: 2
//  Taille_Pile : 2
//
//  Souris effac�e: Non
{
  short start_x;
  short start_y;

  Operation_pop(&start_y);
  Operation_pop(&start_x);

  if ( (start_x!=Paintbrush_X) || (start_y!=Paintbrush_Y) )
  {
    Print_coordinates();
	if (SDL_GetTicks()>Airbrush_next_time)
	{
		Airbrush_next_time+=Airbrush_delay*10;
    	Hide_cursor();
    	// On affiche d�finitivement le pinceau
    	Display_paintbrush(Paintbrush_X,Paintbrush_Y,Back_color,0);
    	Display_cursor();
	}
  }

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


////////////////////////////////////////////////////// OPERATION_POINT_DRAW

void Freehand_mode3_1_0(void)
//  Op�ration   : OPERATION_POINT_DRAW
//  Click Souris: 1
//  Taille_Pile : 0
//
//  Souris effac�e: Oui
{
  Init_start_operation();
  Backup();
  Shade_table=Shade_table_left;
  // On affiche d�finitivement le pinceau
  Display_paintbrush(Paintbrush_X,Paintbrush_Y,Fore_color,0);
  Operation_push(0);  // On change simplement l'�tat de la pile...
}


void Freehand_Mode3_2_0(void)
//  Op�ration   : OPERATION_POINT_DRAW
//  Click Souris: 2
//  Taille_Pile : 0
//
//  Souris effac�e: Oui
{
  Init_start_operation();
  Backup();
  Shade_table=Shade_table_right;
  // On affiche d�finitivement le pinceau
  Display_paintbrush(Paintbrush_X,Paintbrush_Y,Back_color,0);
  Operation_push(0);  // On change simplement l'�tat de la pile...
}


void Freehand_mode3_0_1(void)
//  Op�ration   : OPERATION_POINT_DRAW
//  Click Souris: 0
//  Taille_Pile : 1
//
//  Souris effac�e: Non
{
  End_of_modification();
  Operation_stack_size--;
}


///////////////////////////////////////////////////////////// OPERATION_LINE

void Line_12_0(void)
// Op�ration   : OPERATION_LINE
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
//  Souris effac�e: Oui

//  D�but du trac� d'une ligne (premier clic)
{
  Init_start_operation();
  Backup();
  Paintbrush_shape_before_operation=Paintbrush_shape;
  Paintbrush_shape=PAINTBRUSH_SHAPE_POINT;

  if (Mouse_K==LEFT_SIDE)
  {
    Shade_table=Shade_table_left;
    Pixel_figure_preview(Paintbrush_X,Paintbrush_Y,Fore_color);
    Update_part_of_screen(Paintbrush_X,Paintbrush_Y,1,1);
    Operation_push(Fore_color);
  }
  else
  {
    Shade_table=Shade_table_right;
    Pixel_figure_preview(Paintbrush_X,Paintbrush_Y,Back_color);
    Update_part_of_screen(Paintbrush_X,Paintbrush_Y,1,1);
    Operation_push(Back_color);
  }

  if ((Config.Coords_rel) && (Menu_is_visible))
    Print_in_menu("X:�   0   Y:�   0",0);

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


void Line_12_5(void)
// Op�ration   : OPERATION_LINE
// Click Souris: 1
// Taille_Pile : 5
//
// Souris effac�e: Non

// Poursuite du trac� d'une ligne (d�placement de la souris en gardant le 
// curseur appuy�)
{
  short start_x;
  short start_y;
  short end_x;
  short end_y;
  
  short cursor_x;
  short cursor_y;

  Operation_pop(&end_y);
  Operation_pop(&end_x);
  Operation_pop(&start_y);
  Operation_pop(&start_x);

  cursor_x = Paintbrush_X;
  cursor_y = Paintbrush_Y;

  // On corrige les coordonn�es de la ligne si la touche shift est appuy�e...
  if(SDL_GetModState() & KMOD_SHIFT)
  {
    Clamp_coordinates_regular_angle(start_x,start_y,&cursor_x,&cursor_y);
  }

  // On vient de bouger
  if ((cursor_x!=end_x) || (cursor_y!=end_y))
  {
      Hide_cursor();

    Display_coords_rel_or_abs(start_x,start_y);

    Hide_line_preview(start_x,start_y,end_x,end_y);
    if (Mouse_K==LEFT_SIDE)
    {
      Pixel_figure_preview (start_x,start_y,Fore_color);
      Draw_line_preview (start_x,start_y,cursor_x,cursor_y,Fore_color);
    }
    else
    {
      Pixel_figure_preview (start_x,start_y,Back_color);
      Draw_line_preview (start_x,start_y,cursor_x,cursor_y,Back_color);
    }

    Operation_push(start_x);
    Operation_push(start_y);
    Operation_push(cursor_x);
    Operation_push(cursor_y);

    Display_cursor();
  }
  else
  {
    Operation_push(start_x);
    Operation_push(start_y);
    Operation_push(end_x);
    Operation_push(end_y);
  }
}


void Line_0_5(void)
// Op�ration   : OPERATION_LINE
// Click Souris: 0
// Taille_Pile : 5
//
// Souris effac�e: Oui

// End du trac� d'une ligne (relachage du bouton)
{
  short start_x;
  short start_y;
  short end_x;
  short end_y;
  short color;

  Operation_pop(&end_y);
  Operation_pop(&end_x);
  Operation_pop(&start_y);
  Operation_pop(&start_x);
  Operation_pop(&color);

  Paintbrush_shape=Paintbrush_shape_before_operation;

  Pixel_figure_preview_auto  (start_x,start_y);
  Hide_line_preview (start_x,start_y,end_x,end_y);
  Display_paintbrush      (start_x,start_y,color,0);
  Draw_line_permanent(start_x,start_y,end_x,end_y,color);

  End_of_modification();
  if ( (Config.Coords_rel) && (Menu_is_visible) )
  {
    Print_in_menu("X:       Y:             ",0);
    Print_coordinates();
  }
}


/////////////////////////////////////////////////////////// OPERATION_K_LIGNE


void K_line_12_0(void)
// Op�ration   : OPERATION_K_LIGNE
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
//  Souris effac�e: Oui
{
  byte color;

  Init_start_operation();
  Backup();
  Shade_table=(Mouse_K==LEFT_SIDE)?Shade_table_left:Shade_table_right;
  Paintbrush_shape_before_operation=Paintbrush_shape;
  Paintbrush_shape=PAINTBRUSH_SHAPE_POINT;

  color=(Mouse_K==LEFT_SIDE)?Fore_color:Back_color;

  // On place temporairement le d�but de la ligne qui ne s'afficherait pas sinon
  Pixel_figure_preview(Paintbrush_X,Paintbrush_Y,color);

  if ((Config.Coords_rel) && (Menu_is_visible))
    Print_in_menu("X:�   0   Y:�   0",0);

  Operation_push(Mouse_K | 0x80);
  Operation_push(color);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  // Taille de pile 6 : phase d'appui, non interruptible
}


void K_line_12_6(void)
// Op�ration   : OPERATION_K_LIGNE
// Click Souris: 1 ou 2 | 0
// Taille_Pile : 6      | 7
//
// Souris effac�e: Non
{
  short start_x;
  short start_y;
  short end_x;
  short end_y;
  short color;

  Operation_pop(&end_y);
  Operation_pop(&end_x);

  if ((Paintbrush_X!=end_x) || (Paintbrush_Y!=end_y))
  {
    Hide_cursor();
    Operation_pop(&start_y);
    Operation_pop(&start_x);
    Operation_pop(&color);

    Display_coords_rel_or_abs(start_x,start_y);

    Hide_line_preview(start_x,start_y,end_x,end_y);
    Pixel_figure_preview (start_x,start_y,color);
    Draw_line_preview (start_x,start_y,Paintbrush_X,Paintbrush_Y,color);

    Operation_push(color);
    Operation_push(start_x);
    Operation_push(start_y);
    Display_cursor();
  }

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


void K_line_0_6(void)
// Op�ration   : OPERATION_K_LIGNE
// Click Souris: 0
// Taille_Pile : 6
//
// Souris effac�e: Oui
{
  short start_x;
  short start_y;
  short end_x;
  short end_y;
  short color;
  short direction;

  Operation_pop(&end_y);
  Operation_pop(&end_x);
  Operation_pop(&start_y);
  Operation_pop(&start_x);
  Operation_pop(&color);
  Operation_pop(&direction);

  if ((Config.Coords_rel) && (Menu_is_visible))
    Print_in_menu("X:�   0   Y:�   0",0);

  Pixel_figure_preview_auto  (start_x,start_y);
  Hide_line_preview (start_x,start_y,end_x,end_y);
  /* Doesn't work if fast moving
  Pixel_figure_preview_xor (start_x,start_y, 0);
  Draw_line_preview_xor (start_x,start_y,end_x,end_y,0);
  */
  Paintbrush_shape=Paintbrush_shape_before_operation;
  if (direction & 0x80)
  {
    Display_paintbrush(start_x,start_y,color,0);
    direction=(direction & 0x7F);
  }
  Draw_line_permanent(start_x,start_y,Paintbrush_X,Paintbrush_Y,color);
  Paintbrush_shape=PAINTBRUSH_SHAPE_POINT;

  Operation_push(direction);
  Operation_push(direction); // Valeur bidon servant de nouvel �tat de pile
  Operation_push(color);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  // Taille de pile 7 : phase de "repos", interruptible (comme Elliot Ness :))
}


void K_line_12_7(void)
// Op�ration   : OPERATION_K_LIGNE
// Click Souris: 1 ou 2
// Taille_Pile : 7
//
// Souris effac�e: Oui
{
  short start_x;
  short start_y;
  short end_x;
  short end_y;
  short color;
  short direction;

  Operation_pop(&end_y);
  Operation_pop(&end_x);
  Operation_pop(&start_y);
  Operation_pop(&start_x);
  Operation_pop(&color);
  Operation_pop(&direction);
  Operation_pop(&direction);

  if (direction==Mouse_K)
  {
    Operation_push(direction);
    Operation_push(color);
    Operation_push(start_x);
    Operation_push(start_y);
    Operation_push(end_x);
    Operation_push(end_y);
    // Taille de pile 6 : phase d'appui, non interruptible
  }
  else
  {
    // La s�rie de ligne est termin�e, il faut donc effacer la derni�re
    // preview de ligne
    Pixel_figure_preview_auto  (start_x,start_y);
    Hide_line_preview (start_x,start_y,end_x,end_y);

    Display_cursor();
    Wait_end_of_click();
    Hide_cursor();
    Paintbrush_shape=Paintbrush_shape_before_operation;

    End_of_modification();
    if ( (Config.Coords_rel) && (Menu_is_visible) )
    {
      Print_in_menu("X:       Y:             ",0);
      Print_coordinates();
    }
  }
}


// ---------------------------------------------------------- OPERATION_MAGNIFY


void Magnifier_12_0(void)

// Op�ration   : 4      (item d'une Loupe)
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui

{

  // On passe en mode loupe
  Main_magnifier_mode=1;

  // La fonction d'affichage dans la partie image est d�sormais un affichage
  // sp�cial loupe.
  Pixel_preview=Pixel_preview_magnifier;

  // On calcule l'origine de la loupe
  Main_magnifier_offset_X=Mouse_X-(Main_magnifier_width>>1);
  Main_magnifier_offset_Y=Mouse_Y-(Main_magnifier_height>>1);

  // Calcul du coin haut_gauche de la fen�tre devant �tre zoom�e DANS L'ECRAN
  if (Main_magnifier_offset_X+Main_magnifier_width>=Limit_right-Main_offset_X)
    Main_magnifier_offset_X=Limit_right-Main_magnifier_width-Main_offset_X+1;
  if (Main_magnifier_offset_Y+Main_magnifier_height>=Limit_bottom-Main_offset_Y)
    Main_magnifier_offset_Y=Limit_bottom-Main_magnifier_height-Main_offset_Y+1;

  // Calcul des coordonn�es absolues de ce coin DANS L'IMAGE
  Main_magnifier_offset_X+=Main_offset_X;
  Main_magnifier_offset_Y+=Main_offset_Y;

  if (Main_magnifier_offset_X<0)
    Main_magnifier_offset_X=0;
  if (Main_magnifier_offset_Y<0)
    Main_magnifier_offset_Y=0;

  // On calcule les bornes visibles dans l'�cran
  Position_screen_according_to_zoom();
  Compute_limits();
  Display_all_screen();

  // Repositionner le curseur en fonction des coordonn�es visibles
  Compute_paintbrush_coordinates();

  // On fait de notre mieux pour restaurer l'ancienne op�ration:
  Start_operation_stack(Operation_before_interrupt);
  Display_cursor();
  Wait_end_of_click();
}

/////////////////////////////////////////////////// OPERATION_RECTANGLE_?????

void Rectangle_12_0(void)
// Op�ration   : OPERATION_EMPTY_RECTANGLE / OPERATION_FILLED_RECTANGLE
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
{
  Init_start_operation();

  if ((Config.Coords_rel) && (Menu_is_visible))
    Print_in_menu("\035:   1   \022:   1",0);
  // On laisse une trace du curseur � l'�cran
  Display_cursor();

  if (Mouse_K==LEFT_SIDE)
  {
    Shade_table=Shade_table_left;
    Operation_push(Fore_color);
  }
  else
  {
    Shade_table=Shade_table_right;
    Operation_push(Back_color);
  }

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


void Rectangle_12_5(void)
// Op�ration   : OPERATION_EMPTY_RECTANGLE / OPERATION_FILLED_RECTANGLE
// Click Souris: 1 ou 2
// Taille_Pile : 5
//
// Souris effac�e: Non
{
  short start_x;
  short start_y;
  short old_x;
  short old_y;
  char  str[5];

  Operation_pop(&old_y);
  Operation_pop(&old_x);

  if ((Paintbrush_X!=old_x) || (Paintbrush_Y!=old_y))
  {
    Operation_pop(&start_y);
    Operation_pop(&start_x);

    if ((Config.Coords_rel) && (Menu_is_visible))
    {
      Num2str(((start_x<Paintbrush_X)?Paintbrush_X-start_x:start_x-Paintbrush_X)+1,str,4);
      Print_in_menu(str,2);
      Num2str(((start_y<Paintbrush_Y)?Paintbrush_Y-start_y:start_y-Paintbrush_Y)+1,str,4);
      Print_in_menu(str,11);
    }
    else
      Print_coordinates();

    Operation_push(start_x);
    Operation_push(start_y);
  }

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


void Empty_rectangle_0_5(void)
// Op�ration   : OPERATION_EMPTY_RECTANGLE
// Click Souris: 0
// Taille_Pile : 5
//
// Souris effac�e: Oui
{
  short old_paintbrush_x;
  short old_paintbrush_y;
  short color;

  // On m�morise la position courante du pinceau:

  old_paintbrush_x=Paintbrush_X;
  old_paintbrush_y=Paintbrush_Y;

  // On lit l'ancienne position du pinceau:

  Operation_pop(&Paintbrush_Y);
  Operation_pop(&Paintbrush_X);
  Operation_pop(&Paintbrush_Y);
  Operation_pop(&Paintbrush_X);

  // On va devoir effacer l'ancien curseur qu'on a laiss� trainer:
  Hide_cursor();

  // On lit la couleur du rectangle:
  Operation_pop(&color);

  // On fait un petit backup de l'image:
  Backup();

  // Et on trace le rectangle:
  Draw_empty_rectangle(Paintbrush_X,Paintbrush_Y,old_paintbrush_x,old_paintbrush_y,color);

  // Enfin, on r�tablit la position du pinceau:
  Paintbrush_X=old_paintbrush_x;
  Paintbrush_Y=old_paintbrush_y;

  End_of_modification();

  if ((Config.Coords_rel) && (Menu_is_visible))
    Print_in_menu("X:       Y:",0);
  Print_coordinates();
}


void Filled_rectangle_0_5(void)
// Op�ration   : OPERATION_FILLED_RECTANGLE
// Click Souris: 0
// Taille_Pile : 5
//
// Souris effac�e: Oui
{
  short old_paintbrush_x;
  short old_paintbrush_y;
  short color;

  // On m�morise la position courante du pinceau:

  old_paintbrush_x=Paintbrush_X;
  old_paintbrush_y=Paintbrush_Y;

  // On lit l'ancienne position du pinceau:

  Operation_pop(&Paintbrush_Y);
  Operation_pop(&Paintbrush_X);
  Operation_pop(&Paintbrush_Y);
  Operation_pop(&Paintbrush_X);

  // On va devoir effacer l'ancien curseur qu'on a laiss� trainer:
  Hide_cursor();

  // On lit la couleur du rectangle:
  Operation_pop(&color);

  // On fait un petit backup de l'image:
  Backup();

  // Et on trace le rectangle:
  Draw_filled_rectangle(Paintbrush_X,Paintbrush_Y,old_paintbrush_x,old_paintbrush_y,color);

  // Enfin, on r�tablit la position du pinceau:
  Paintbrush_X=old_paintbrush_x;
  Paintbrush_Y=old_paintbrush_y;

  End_of_modification();
  if ( (Config.Coords_rel) && (Menu_is_visible) )
  {
    Print_in_menu("X:       Y:",0);
    Print_coordinates();
  }
}


////////////////////////////////////////////////////// OPERATION_CERCLE_?????


void Circle_12_0(void)
//
// Op�ration   : OPERATION_EMPTY_CIRCLE / OPERATION_FILLED_CIRCLE
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
{
  Init_start_operation();
  Backup();

  Paintbrush_shape_before_operation=Paintbrush_shape;
  Paintbrush_shape=PAINTBRUSH_SHAPE_POINT;

  if (Mouse_K==LEFT_SIDE)
  {
    Shade_table=Shade_table_left;
    Operation_push(Fore_color);
  }
  else
  {
    Shade_table=Shade_table_right;
    Operation_push(Back_color);
  }

  if ((Config.Coords_rel) && (Menu_is_visible))
    Print_in_menu("Radius:   0    ",0);

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


void Circle_12_5(void)
//
// Op�ration   : OPERATION_EMPTY_CIRCLE / OPERATION_FILLED_CIRCLE
// Click Souris: 1 ou 2
// Taille_Pile : 5 (color, X_Centre, Y_Centre, X_Tangente, Y_Tangente)
//
// Souris effac�e: Non
//
{
  short tangent_x;
  short tangent_y;
  short center_x;
  short center_y;
  short color;
  short radius;
  char  str[5];

  Operation_pop(&tangent_y);
  Operation_pop(&tangent_x);
  Operation_pop(&center_y);
  Operation_pop(&center_x);
  Operation_pop(&color);

  if ( (tangent_x!=Paintbrush_X) || (tangent_y!=Paintbrush_Y) )
  {
    Hide_cursor();
    if ((Config.Coords_rel) && (Menu_is_visible))
    {
      Num2str(Distance(center_x,center_y,Paintbrush_X,Paintbrush_Y),str,4);
      Print_in_menu(str,7);
    }
    else
      Print_coordinates();

    Circle_limit=((tangent_x-center_x)*(tangent_x-center_x))+
                  ((tangent_y-center_y)*(tangent_y-center_y));
    radius=sqrt(Circle_limit);
    Hide_empty_circle_preview(center_x,center_y,radius);

    Circle_limit=((Paintbrush_X-center_x)*(Paintbrush_X-center_x))+
                  ((Paintbrush_Y-center_y)*(Paintbrush_Y-center_y));
    radius=sqrt(Circle_limit);
    Draw_empty_circle_preview(center_x,center_y,radius,color);

    Display_cursor();
  }

  Operation_push(color);
  Operation_push(center_x);
  Operation_push(center_y);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


void Empty_circle_0_5(void)
//
// Op�ration   : OPERATION_EMPTY_CIRCLE
// Click Souris: 0
// Taille_Pile : 5 (color, X_Centre, Y_Centre, X_Tangente, Y_Tangente)
//
// Souris effac�e: Oui
//
{
  short tangent_x;
  short tangent_y;
  short center_x;
  short center_y;
  short color;
  short radius;

  Operation_pop(&tangent_y);
  Operation_pop(&tangent_x);
  Operation_pop(&center_y);
  Operation_pop(&center_x);
  Operation_pop(&color);

  Circle_limit=((tangent_x-center_x)*(tangent_x-center_x))+
                ((tangent_y-center_y)*(tangent_y-center_y));
  radius=sqrt(Circle_limit);
  Hide_empty_circle_preview(center_x,center_y,radius);

  Paintbrush_shape=Paintbrush_shape_before_operation;

  Draw_empty_circle_permanent(center_x,center_y,radius,color);

  End_of_modification();
  
  if ( (Config.Coords_rel) && (Menu_is_visible) )
  {
    Print_in_menu("X:       Y:",0);
    Print_coordinates();
  }
}


void Filled_circle_0_5(void)
//
// Op�ration   : OPERATION_FILLED_CIRCLE
// Click Souris: 0
// Taille_Pile : 5 (color, X_Centre, Y_Centre, X_Tangente, Y_Tangente)
//
// Souris effac�e: Oui
//
{
  short tangent_x;
  short tangent_y;
  short center_x;
  short center_y;
  short color;
  short radius;

  Operation_pop(&tangent_y);
  Operation_pop(&tangent_x);
  Operation_pop(&center_y);
  Operation_pop(&center_x);
  Operation_pop(&color);

  Circle_limit=((tangent_x-center_x)*(tangent_x-center_x))+
                ((tangent_y-center_y)*(tangent_y-center_y));
  radius=sqrt(Circle_limit);
  Hide_empty_circle_preview(center_x,center_y,radius);

  Paintbrush_shape=Paintbrush_shape_before_operation;

  Draw_filled_circle(center_x,center_y,radius,color);

  End_of_modification();
  if ( (Config.Coords_rel) && (Menu_is_visible) )
  {
    Print_in_menu("X:       Y:",0);
    Print_coordinates();
  }
}


///////////////////////////////////////////////////// OPERATION_ELLIPSE_?????


void Ellipse_12_0(void)
//
// Op�ration   : OPERATION_EMPTY_ELLIPSE / OPERATION_FILLED_ELLIPSE
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
{
  Init_start_operation();
  Backup();

  Paintbrush_shape_before_operation=Paintbrush_shape;
  Paintbrush_shape=PAINTBRUSH_SHAPE_POINT;

  if (Mouse_K==LEFT_SIDE)
  {
    Shade_table=Shade_table_left;
    Operation_push(Fore_color);
  }
  else
  {
    Shade_table=Shade_table_right;
    Operation_push(Back_color);
  }

  if ((Config.Coords_rel) && (Menu_is_visible))
    Print_in_menu("X:�   0   Y:�   0",0);

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


void Ellipse_12_5(void)
//
// Op�ration   : OPERATION_EMPTY_ELLIPSE / OPERATION_FILLED_ELLIPSE
// Click Souris: 1 ou 2
// Taille_Pile : 5 (color, X_Centre, Y_Centre, X_Tangente, Y_Tangente)
//
// Souris effac�e: Non
//
{
  short tangent_x;
  short tangent_y;
  short center_x;
  short center_y;
  short color;
  short horizontal_radius;
  short vertical_radius;

  Operation_pop(&tangent_y);
  Operation_pop(&tangent_x);
  Operation_pop(&center_y);
  Operation_pop(&center_x);
  Operation_pop(&color);

  if ( (tangent_x!=Paintbrush_X) || (tangent_y!=Paintbrush_Y) )
  {
    Hide_cursor();
    Display_coords_rel_or_abs(center_x,center_y);

    horizontal_radius=(tangent_x>center_x)?tangent_x-center_x
                                           :center_x-tangent_x;
    vertical_radius  =(tangent_y>center_y)?tangent_y-center_y
                                           :center_y-tangent_y;
    Hide_empty_ellipse_preview(center_x,center_y,horizontal_radius,vertical_radius);

    horizontal_radius=(Paintbrush_X>center_x)?Paintbrush_X-center_x
                                         :center_x-Paintbrush_X;
    vertical_radius  =(Paintbrush_Y>center_y)?Paintbrush_Y-center_y
                                         :center_y-Paintbrush_Y;
    Draw_empty_ellipse_preview(center_x,center_y,horizontal_radius,vertical_radius,color);

    Display_cursor();
  }

  Operation_push(color);
  Operation_push(center_x);
  Operation_push(center_y);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


void Empty_ellipse_0_5(void)
//
// Op�ration   : OPERATION_EMPTY_ELLIPSE
// Click Souris: 0
// Taille_Pile : 5 (color, X_Centre, Y_Centre, X_Tangente, Y_Tangente)
//
// Souris effac�e: Oui
//
{
  short tangent_x;
  short tangent_y;
  short center_x;
  short center_y;
  short color;
  short horizontal_radius;
  short vertical_radius;

  Operation_pop(&tangent_y);
  Operation_pop(&tangent_x);
  Operation_pop(&center_y);
  Operation_pop(&center_x);
  Operation_pop(&color);

  horizontal_radius=(tangent_x>center_x)?tangent_x-center_x
                                         :center_x-tangent_x;
  vertical_radius  =(tangent_y>center_y)?tangent_y-center_y
                                         :center_y-tangent_y;
  Hide_empty_ellipse_preview(center_x,center_y,horizontal_radius,vertical_radius);

  Paintbrush_shape=Paintbrush_shape_before_operation;

  Draw_empty_ellipse_permanent(center_x,center_y,horizontal_radius,vertical_radius,color);

  End_of_modification();
  
  if ( (Config.Coords_rel) && (Menu_is_visible) )
  {
    Print_in_menu("X:       Y:             ",0);
    Print_coordinates();
  }
}


void Filled_ellipse_0_5(void)
//
// Op�ration   : OPERATION_FILLED_ELLIPSE
// Click Souris: 0
// Taille_Pile : 5 (color, X_Centre, Y_Centre, X_Tangente, Y_Tangente)
//
// Souris effac�e: Oui
//
{
  short tangent_x;
  short tangent_y;
  short center_x;
  short center_y;
  short color;
  short horizontal_radius;
  short vertical_radius;

  Operation_pop(&tangent_y);
  Operation_pop(&tangent_x);
  Operation_pop(&center_y);
  Operation_pop(&center_x);
  Operation_pop(&color);

  horizontal_radius=(tangent_x>center_x)?tangent_x-center_x
                                         :center_x-tangent_x;
  vertical_radius  =(tangent_y>center_y)?tangent_y-center_y
                                         :center_y-tangent_y;
  Hide_empty_ellipse_preview(center_x,center_y,horizontal_radius,vertical_radius);

  Paintbrush_shape=Paintbrush_shape_before_operation;

  Draw_filled_ellipse(center_x,center_y,horizontal_radius,vertical_radius,color);

  End_of_modification();
  if ( (Config.Coords_rel) && (Menu_is_visible) )
  {
    Print_in_menu("X:       Y:             ",0);
    Print_coordinates();
  }
}


////////////////////////////////////////////////////////////// OPERATION_FILL


void Fill_1_0(void)
//
// Op�ration   : OPERATION_FILL
// Click Souris: 1
// Taille_Pile : 0
//
// Souris effac�e: Oui
//
{
  Hide_cursor();
  // Pas besoin d'initialiser le d�but d'op�ration car le Smear n'affecte pas
  // le Fill, et on se fout de savoir si on est dans la partie gauche ou
  // droite de la loupe.
  // On ne s'occupe pas de faire un Backup: c'est "Fill_general" qui s'en charge.
  Shade_table=Shade_table_left;
  Fill_general(Fore_color);
  Display_cursor();
  End_of_modification();
  Wait_end_of_click();
}


void Fill_2_0(void)
//
// Op�ration   : OPERATION_FILL
// Click Souris: 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
//
{
  Hide_cursor();
  // Pas besoin d'initialiser le d�but d'op�ration car le Smear n'affecte pas
  // le Fill, et on se fout de savoir si on est dans la partie gauche ou
  // droite de la loupe.
  // On ne s'occupe pas de faire un Backup: c'est "Fill_general" qui s'en charge.
  Shade_table=Shade_table_right;
  Fill_general(Back_color);
  Display_cursor();
  End_of_modification();
  Wait_end_of_click();
}


///////////////////////////////////////////////////////// OPERATION_REPLACE


void Replace_1_0(void)
//
// Op�ration   : OPERATION_REPLACE
// Click Souris: 1
// Taille_Pile : 0
//
// Souris effac�e: Oui
//
{
  Hide_cursor();
  // Pas besoin d'initialiser le d�but d'op�ration car le Smear n'affecte pas
  // le Replace, et on se fout de savoir si on est dans la partie gauche ou
  // droite de la loupe.
  Backup();
//  Shade_table=Shade_table_left;
  Replace(Fore_color);
  Display_cursor();
  End_of_modification();
  Wait_end_of_click();
}


void Replace_2_0(void)
//
// Op�ration   : OPERATION_REPLACE
// Click Souris: 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
//
{
  Hide_cursor();
  // Pas besoin d'initialiser le d�but d'op�ration car le Smear n'affecte pas
  // le Replace, et on se fout de savoir si on est dans la partie gauche ou
  // droite de la loupe.
  Backup();
//  Shade_table=Shade_table_right;
  Replace(Back_color);
  Display_cursor();
  End_of_modification();
  Wait_end_of_click();
}


/////////////////////////////////////////////////////////// OPERATION_COLORPICK


void Colorpicker_12_0(void)
//
// Op�ration   : OPERATION_COLORPICK
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
//
{
  Init_start_operation();

  if (Mouse_K==LEFT_SIDE)
  {
    Set_fore_color(Colorpicker_color);
  }
  else
  {
    Set_back_color(Colorpicker_color);
  }
  Operation_push(Mouse_K);
}


void Colorpicker_1_1(void)
//
// Op�ration   : OPERATION_COLORPICK
// Click Souris: 1
// Taille_Pile : 1
//
// Souris effac�e: Non
//
{
  char str[4];

  if ( (Paintbrush_X>=0) && (Paintbrush_Y>=0)
    && (Paintbrush_X<Main_image_width)
    && (Paintbrush_Y<Main_image_height) )
    Colorpicker_color=Read_pixel_from_current_screen(Paintbrush_X,Paintbrush_Y);
  else
    Colorpicker_color=0;

  if ( (Colorpicker_X!=Paintbrush_X)
    || (Colorpicker_Y!=Paintbrush_Y) )
  {
    Hide_cursor();
    Colorpicker_X=Paintbrush_X;
    Colorpicker_Y=Paintbrush_Y;

    if (Colorpicker_color!=Fore_color)
    {
      Set_fore_color(Colorpicker_color);
    }

    if (Menu_is_visible)
    {
      Print_coordinates();
      Num2str(Colorpicker_color,str,3);
      Print_in_menu(str,20);
      Print_general(170*Menu_factor_X,Menu_status_Y," ",0,Colorpicker_color);
    }
    Display_cursor();
  }
}


void Colorpicker_2_1(void)
//
// Op�ration   : OPERATION_COLORPICK
// Click Souris: 2
// Taille_Pile : 1
//
// Souris effac�e: Non
//
{
  char str[4];

  if ( (Paintbrush_X>=0) && (Paintbrush_Y>=0)
    && (Paintbrush_X<Main_image_width)
    && (Paintbrush_Y<Main_image_height) )
    Colorpicker_color=Read_pixel_from_current_screen(Paintbrush_X,Paintbrush_Y);
  else
    Colorpicker_color=0;

  if ( (Colorpicker_X!=Paintbrush_X)
    || (Colorpicker_Y!=Paintbrush_Y) )
  {
    Hide_cursor();
    Colorpicker_X=Paintbrush_X;
    Colorpicker_Y=Paintbrush_Y;

    if (Colorpicker_color!=Back_color)
    {
      Set_back_color(Colorpicker_color);
    }

    if (Menu_is_visible)
    {
      Print_coordinates();
      Num2str(Colorpicker_color,str,3);
      Print_in_menu(str,20);
      Print_general(170*Menu_factor_X,Menu_status_Y," ",0,Colorpicker_color);
    }
    Display_cursor();
  }
}



void Colorpicker_0_1(void)
//
// Op�ration   : OPERATION_COLORPICK
// Click Souris: 0
// Taille_Pile : 1
//
// Souris effac�e: Oui
//
{
  short click;

  Operation_pop(&click);
  if (click==LEFT_SIDE)
  {
    Set_fore_color(Colorpicker_color);
  }
  else
  {
    Set_back_color(Colorpicker_color);
  }
  Unselect_button(BUTTON_COLORPICKER);
}


/////////////////////////////////////////////////// OPERATION_4_POINTS_CURVE


void Draw_curve_cross(short x_pos, short y_pos)
{
  short start_x,end_x;
  short start_y,end_y;
  short i,temp;
  //byte  temp2;

  if (x_pos>=Limit_left+3)
    start_x=0;
  else
    start_x=3-(x_pos-Limit_left);

  if (y_pos>=Limit_top+3)
    start_y=0;
  else
    start_y=3-(y_pos-Limit_top);

  if (x_pos<=Limit_visible_right-3)
    end_x=6;
  else
    end_x=3+(Limit_visible_right-x_pos);

  if (y_pos<=Limit_visible_bottom-3)
    end_y=6;
  else
    end_y=3+(Limit_visible_bottom-y_pos);

  if (start_x<=end_x && start_y<=end_y)
  {    
    for (i=start_x; i<=end_x; i++)
    {
      temp=x_pos+i-3;
      Pixel_preview(temp,y_pos,~Read_pixel(temp -Main_offset_X,
                                          y_pos-Main_offset_Y));
    }
    for (i=start_y; i<=end_y; i++)
    {
      temp=y_pos+i-3;
      Pixel_preview(x_pos,temp,~Read_pixel(x_pos-Main_offset_X,
                                          temp -Main_offset_Y));
    }
    Update_part_of_screen(x_pos+start_x-3,y_pos+start_y-3,end_x-start_x+1,end_y-start_y+1);
  }
}


void Curve_34_points_1_0(void)
//
//  Op�ration   : OPERATION_COURBE_?_POINTS
//  Click Souris: 1
//  Taille_Pile : 0
//
//  Souris effac�e: Oui
//
{
  Init_start_operation();
  Backup();
  Shade_table=Shade_table_left;

  Paintbrush_hidden=1;

  Pixel_figure_preview(Paintbrush_X,Paintbrush_Y,Fore_color);
  Update_part_of_screen(Paintbrush_X,Paintbrush_Y,1,1);

  if ((Config.Coords_rel) && (Menu_is_visible))
    Print_in_menu("X:�   0   Y:�   0",0);

  Operation_push(Fore_color);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}

void Curve_34_points_2_0(void)
//
//  Op�ration   : OPERATION_COURBE_?_POINTS
//  Click Souris: 2
//  Taille_Pile : 0
//
//  Souris effac�e: Oui
//
{
  Init_start_operation();
  Backup();
  Shade_table=Shade_table_right;

  Paintbrush_hidden=1;

  Pixel_figure_preview(Paintbrush_X,Paintbrush_Y,Back_color);
  Update_part_of_screen(Paintbrush_X,Paintbrush_Y,1,1);

  if ((Config.Coords_rel) && (Menu_is_visible))
    Print_in_menu("X:�   0   Y:�   0",0);

  Operation_push(Back_color);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


void Curve_34_points_1_5(void)
//
//  Op�ration   : OPERATION_COURBE_?_POINTS
//  Click Souris: 1
//  Taille_Pile : 5
//
//  Souris effac�e: Non
//
{
  short x1,x2,y1,y2;

  Operation_pop(&y2);
  Operation_pop(&x2);
  Operation_pop(&y1);
  Operation_pop(&x1);

  if ( (y2!=Paintbrush_Y) || (x2!=Paintbrush_X) )
  {
    Hide_cursor();
    Display_coords_rel_or_abs(x1,y1);

    Hide_line_preview(x1,y1,x2,y2);
    Pixel_figure_preview (x1,y1,Fore_color);
    Draw_line_preview (x1,y1,Paintbrush_X,Paintbrush_Y,Fore_color);

    Display_cursor();
  }

  Operation_push(x1);
  Operation_push(y1);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}

void Curve_34_points_2_5(void)
//
//  Op�ration   : OPERATION_COURBE_?_POINTS
//  Click Souris: 2
//  Taille_Pile : 5
//
//  Souris effac�e: Non
//
{
  short x1,x2,y1,y2;

  Operation_pop(&y2);
  Operation_pop(&x2);
  Operation_pop(&y1);
  Operation_pop(&x1);

  if ( (y2!=Paintbrush_Y) || (x2!=Paintbrush_X) )
  {
    Hide_cursor();
    Display_coords_rel_or_abs(x1,y1);

    Hide_line_preview(x1,y1,x2,y2);
    Pixel_figure_preview (x1,y1,Back_color);
    Draw_line_preview (x1,y1,Paintbrush_X,Paintbrush_Y,Back_color);

    Display_cursor();
  }

  Operation_push(x1);
  Operation_push(y1);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


byte Cursor_hidden_before_curve;

void Curve_4_points_0_5(void)
//
//  Op�ration   : OPERATION_4_POINTS_CURVE
//  Click Souris: 0
//  Taille_Pile : 5
//
//  Souris effac�e: Oui
//
{
  short x1,y1,x2,y2,x3,y3,x4,y4;
  short third_x,third_y;
  short color;

  Operation_pop(&y4);
  Operation_pop(&x4);
  Operation_pop(&y1);
  Operation_pop(&x1);
  Operation_pop(&color);

  third_x=Round_div(abs(x4-x1),3);
  third_y=Round_div(abs(y4-y1),3);

  if (x1<x4)
  {
    x2=x1+third_x;
    x3=x4-third_x;
  }
  else
  {
    x3=x4+third_x;
    x2=x1-third_x;
  }

  if (y1<y4)
  {
    y2=y1+third_y;
    y3=y4-third_y;
  }
  else
  {
    y3=y4+third_y;
    y2=y1-third_y;
  }

  Hide_line_preview(x1,y1,x4,y4);
  Draw_curve_preview(x1,y1,x2,y2,x3,y3,x4,y4,color);

  // On trace les petites croix montrant les 2 points interm�diares
  Draw_curve_cross(x2,y2);
  Draw_curve_cross(x3,y3);

  Cursor_hidden_before_curve=Cursor_hidden;
  Cursor_hidden=0;

  Operation_push(color);
  Operation_push(x1);
  Operation_push(y1);
  Operation_push(x2);
  Operation_push(y2);
  Operation_push(x3);
  Operation_push(y3);
  Operation_push(x4);
  Operation_push(y4);

  if ( (Config.Coords_rel) && (Menu_is_visible) )
  {
    Print_in_menu("X:       Y:             ",0);
    Print_coordinates();
  }
}


void Curve_4_points_1_9(void)
//
//  Op�ration   : OPERATION_4_POINTS_CURVE
//  Click Souris: 1
//  Taille_Pile : 9
//
//  Souris effac�e: Non
//
{
  short x1,y1,x2,y2,x3,y3,x4,y4;
  short color;
  byte  it_is_x2;

  Operation_pop(&y4);
  Operation_pop(&x4);
  Operation_pop(&y3);
  Operation_pop(&x3);
  Operation_pop(&y2);
  Operation_pop(&x2);
  Operation_pop(&y1);
  Operation_pop(&x1);
  Operation_pop(&color);

  it_is_x2=(Distance(Paintbrush_X,Paintbrush_Y,x2,y2) < Distance(Paintbrush_X,Paintbrush_Y,x3,y3));

  if ( (   it_is_x2  && ( (Paintbrush_X!=x2) || (Paintbrush_Y!=y2) ) )
    || ( (!it_is_x2) && ( (Paintbrush_X!=x3) || (Paintbrush_Y!=y3) ) ) )
  {
    Hide_cursor();
    Print_coordinates();

    Draw_curve_cross(x2,y2);
    Draw_curve_cross(x3,y3);

    Hide_curve_preview(x1,y1,x2,y2,x3,y3,x4,y4,color);

    if (it_is_x2)
    {
      x2=Paintbrush_X;
      y2=Paintbrush_Y;
    }
    else
    {
      x3=Paintbrush_X;
      y3=Paintbrush_Y;
    }

    Draw_curve_preview(x1,y1,x2,y2,x3,y3,x4,y4,color);

    Draw_curve_cross(x2,y2);
    Draw_curve_cross(x3,y3);

    Display_cursor();
  }

  Operation_push(color);
  Operation_push(x1);
  Operation_push(y1);
  Operation_push(x2);
  Operation_push(y2);
  Operation_push(x3);
  Operation_push(y3);
  Operation_push(x4);
  Operation_push(y4);
}


void Curve_4_points_2_9(void)
//
//  Op�ration   : OPERATION_4_POINTS_CURVE
//  Click Souris: 2
//  Taille_Pile : 9
//
//  Souris effac�e: Oui
//
{
  short x1,y1,x2,y2,x3,y3,x4,y4;
  short color;

  Operation_pop(&y4);
  Operation_pop(&x4);
  Operation_pop(&y3);
  Operation_pop(&x3);
  Operation_pop(&y2);
  Operation_pop(&x2);
  Operation_pop(&y1);
  Operation_pop(&x1);
  Operation_pop(&color);

  Hide_cursor();
  
  Draw_curve_cross(x2,y2);
  Draw_curve_cross(x3,y3);

  Paintbrush_hidden=0;
  Cursor_hidden=Cursor_hidden_before_curve;

  Hide_curve_preview(x1,y1,x2,y2,x3,y3,x4,y4,color);
  Draw_curve_permanent(x1,y1,x2,y2,x3,y3,x4,y4,color);

  Display_cursor();
  End_of_modification();
  Wait_end_of_click();
}


/////////////////////////////////////////////////// OPERATION_3_POINTS_CURVE


void Compute_3_point_curve(short x1, short y1, short x4, short y4,
                              short * x2, short * y2, short * x3, short * y3)
{
  float cx,cy; // Centre de (x1,y1) et (x4,y4)
  float bx,by; // Intersect. des dtes ((x1,y1),(x2,y2)) et ((x3,y3),(x4,y4))

  cx=(float)(x1+x4)/2.0;           // P1*--_               L�gende:
  cy=(float)(y1+y4)/2.0;           //   �   \�� P2         -_\|/ : courbe
                                   //   �    \ �*�         * : point important
  bx=cx+((8.0/3.0)*(Paintbrush_X-cx));//   �     |   ��       � : pointill�
  by=cy+((8.0/3.0)*(Paintbrush_Y-cy));//   �     |P    ��  B
                                   // C *�����*���������*  P=Pos. du pinceau
  *x2=Round((bx+x1)/2.0);          //   �     |     ��     C=milieu de [P1,P4]
  *y2=Round((by+y1)/2.0);          //   �     |   ��       B=point tel que
                                   //   �    / �*�         C->B=(8/3) * C->P
  *x3=Round((bx+x4)/2.0);          //   �  _/�� P3         P2=milieu de [P1,B]
  *y3=Round((by+y4)/2.0);          // P4*--                P3=milieu de [P4,B]
}


void Curve_3_points_0_5(void)
//
//  Op�ration   : OPERATION_3_POINTS_CURVE
//  Click Souris: 0
//  Taille_Pile : 5
//
//  Souris effac�e: Oui
//
{
  short x1,y1,x2,y2,x3,y3,x4,y4;
  short color;

  Operation_pop(&y4);
  Operation_pop(&x4);
  Operation_pop(&y1);
  Operation_pop(&x1);
  Operation_pop(&color);

  Compute_3_point_curve(x1,y1,x4,y4,&x2,&y2,&x3,&y3);

  Hide_line_preview(x1,y1,x4,y4);
  Draw_curve_preview(x1,y1,x2,y2,x3,y3,x4,y4,color);

  if ( (Config.Coords_rel) && (Menu_is_visible) )
  {
    Print_in_menu("X:       Y:             ",0);
    Print_coordinates();
  }

  Operation_push(color);
  Operation_push(x1);
  Operation_push(y1);
  Operation_push(x2);
  Operation_push(y2);
  Operation_push(x3);
  Operation_push(y3);
  Operation_push(x4);
  Operation_push(y4);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


void Curve_3_points_0_11(void)
//
//  Op�ration   : OPERATION_3_POINTS_CURVE
//  Click Souris: 0
//  Taille_Pile : 11
//
//  Souris effac�e: Non
//
{
  short x1,y1,x2,y2,x3,y3,x4,y4;
  short old_x,old_y;
  short color;

  Operation_pop(&old_y);
  Operation_pop(&old_x);

  if ( (Paintbrush_X!=old_x) || (Paintbrush_Y!=old_y) )
  {
    Operation_pop(&y4);
    Operation_pop(&x4);
    Operation_pop(&y3);
    Operation_pop(&x3);
    Operation_pop(&y2);
    Operation_pop(&x2);
    Operation_pop(&y1);
    Operation_pop(&x1);
    Operation_pop(&color);

    Hide_cursor();
    Print_coordinates();

    Hide_curve_preview(x1,y1,x2,y2,x3,y3,x4,y4,color);
    Compute_3_point_curve(x1,y1,x4,y4,&x2,&y2,&x3,&y3);
    Draw_curve_preview (x1,y1,x2,y2,x3,y3,x4,y4,color);
    Display_cursor();

    Operation_push(color);
    Operation_push(x1);
    Operation_push(y1);
    Operation_push(x2);
    Operation_push(y2);
    Operation_push(x3);
    Operation_push(y3);
    Operation_push(x4);
    Operation_push(y4);
  }
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


void Curve_3_points_12_11(void)
//
//  Op�ration   : OPERATION_3_POINTS_CURVE
//  Click Souris: 1 ou 2
//  Taille_Pile : 11
//
//  Souris effac�e: Oui
//
{
  short x1,y1,x2,y2,x3,y3,x4,y4;
  short old_x,old_y;
  short color;

  Operation_pop(&old_y);
  Operation_pop(&old_x);
  Operation_pop(&y4);
  Operation_pop(&x4);
  Operation_pop(&y3);
  Operation_pop(&x3);
  Operation_pop(&y2);
  Operation_pop(&x2);
  Operation_pop(&y1);
  Operation_pop(&x1);
  Operation_pop(&color);

  Paintbrush_hidden=0;
  
  Hide_cursor();

  Hide_curve_preview (x1,y1,x2,y2,x3,y3,x4,y4,color);
  Compute_3_point_curve(x1,y1,x4,y4,&x2,&y2,&x3,&y3);
  Draw_curve_permanent(x1,y1,x2,y2,x3,y3,x4,y4,color);

  End_of_modification();
  Display_cursor();
  Wait_end_of_click();
}


///////////////////////////////////////////////////////////// OPERATION_AIRBRUSH

void Airbrush_1_0(void)
//
//  Op�ration   : OPERATION_AIRBRUSH
//  Click Souris: 1
//  Taille_Pile : 0
//
//  Souris effac�e: Non
//
{
  Init_start_operation();
  Backup();
  Shade_table=Shade_table_left;

  Airbrush_next_time = SDL_GetTicks()+Airbrush_delay*10;
  Airbrush(LEFT_SIDE);

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}

void Airbrush_2_0(void)
//
//  Op�ration   : OPERATION_AIRBRUSH
//  Click Souris: 2
//  Taille_Pile : 0
//
//  Souris effac�e: Non
//
{
  Init_start_operation();
  Backup();
  Shade_table=Shade_table_right;
  Airbrush_next_time = SDL_GetTicks()+Airbrush_delay*10;
  Airbrush(RIGHT_SIDE);

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}

void Airbrush_12_2(void)
//
//  Op�ration   : OPERATION_AIRBRUSH
//  Click Souris: 1 ou 2
//  Taille_Pile : 2
//
//  Souris effac�e: Non
//
{
  short old_x,old_y;

  Operation_pop(&old_y);
  Operation_pop(&old_x);

  if ( (Menu_is_visible) && ((Paintbrush_X!=old_x) || (Paintbrush_Y!=old_y)) )
  {
    Hide_cursor();
    Print_coordinates();
    Display_cursor();
  }

  if (SDL_GetTicks()>Airbrush_next_time)
  {
    Airbrush_next_time+=Airbrush_delay*10;
    Airbrush(Mouse_K_unique);
  }

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}

void Airbrush_0_2(void)
//
//  Op�ration   : OPERATION_AIRBRUSH
//  Click Souris: 0
//  Taille_Pile : 2
//
//  Souris effac�e: Non
//
{
  Operation_stack_size-=2;
  End_of_modification();
}


////////////////////////////////////////////////////////// OPERATION_POLYGON


void Polygon_12_0(void)
// Op�ration   : OPERATION_POLYGON
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
{
  byte color;

  Init_start_operation();
  Backup();
  Shade_table=(Mouse_K==LEFT_SIDE)?Shade_table_left:Shade_table_right;
  Paintbrush_shape_before_operation=Paintbrush_shape;
  Paintbrush_shape=PAINTBRUSH_SHAPE_POINT;

  color=(Mouse_K==LEFT_SIDE)?Fore_color:Back_color;

  // On place temporairement le d�but de la ligne qui ne s'afficherait pas sinon
  Pixel_figure_preview(Paintbrush_X,Paintbrush_Y,color);
  Update_part_of_screen(Paintbrush_X,Paintbrush_Y,1,1);

  if ((Config.Coords_rel) && (Menu_is_visible))
    Print_in_menu("X:�   0   Y:�   0",0);

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Operation_push(Mouse_K | 0x80);
  Operation_push(color);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  // Taille de pile 8 : phase d'appui, non interruptible
}



void Polygon_12_9(void)
// Op�ration   : OPERATION_POLYGON
// Click Souris: 1 ou 2
// Taille_Pile : 9
//
// Souris effac�e: Oui
{
  short start_x;
  short start_y;
  short end_x;
  short end_y;
  short color;
  short direction;

  Operation_pop(&end_y);
  Operation_pop(&end_x);
  Operation_pop(&start_y);
  Operation_pop(&start_x);
  Operation_pop(&color);
  Operation_pop(&direction);
  Operation_pop(&direction);

  if (direction==Mouse_K)
  {
    Operation_push(direction);
    Operation_push(color);
    Operation_push(start_x);
    Operation_push(start_y);
    Operation_push(end_x);
    Operation_push(end_y);
    // Taille de pile 8 : phase d'appui, non interruptible
  }
  else
  {
    //   La s�rie de ligne est termin�e, il faut donc effacer la derni�re
    // preview de ligne et relier le dernier point avec le premier
    Pixel_figure_preview_auto  (start_x,start_y);
    Hide_line_preview (start_x,start_y,end_x,end_y);
    Operation_pop(&end_y);
    Operation_pop(&end_x);
    Paintbrush_shape=Paintbrush_shape_before_operation;
    // Le pied aurait �t� de ne pas repasser sur le 1er point de la 1�re ligne
    // mais c'est pas possible :(
    Draw_line_permanent(start_x,start_y,end_x,end_y,color);
    Paintbrush_shape=PAINTBRUSH_SHAPE_POINT;

    Display_cursor();
    End_of_modification();
    Wait_end_of_click();
    Hide_cursor();

    if ( (Config.Coords_rel) && (Menu_is_visible) )
    {
      Print_in_menu("X:       Y:             ",0);
      Print_coordinates();
    }

    Paintbrush_shape=Paintbrush_shape_before_operation;
  }
}


////////////////////////////////////////////////////////// OPERATION_POLYFILL

short * Polyfill_table_of_points;
int Polyfill_number_of_points;

void Polyfill_12_0(void)
// Op�ration   : OPERATION_POLYFILL
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
{
  byte color;

  Init_start_operation();
  Backup();
  Shade_table=(Mouse_K==LEFT_SIDE)?Shade_table_left:Shade_table_right;
  Paintbrush_hidden=1;

  color=(Mouse_K==LEFT_SIDE)?Fore_color:Back_color;

  Polyfill_table_of_points=(short *) malloc((Config.Nb_max_vertices_per_polygon<<1)*sizeof(short));
  Polyfill_table_of_points[0]=Paintbrush_X;
  Polyfill_table_of_points[1]=Paintbrush_Y;
  Polyfill_number_of_points=1;

  // On place temporairement le d�but de la ligne qui ne s'afficherait pas sinon
  Pixel_figure_preview_xor(Paintbrush_X,Paintbrush_Y,0);
  Update_part_of_screen(Paintbrush_X,Paintbrush_Y,1,1);
  
  if ((Config.Coords_rel) && (Menu_is_visible))
    Print_in_menu("X:�   0   Y:�   0",0);

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Operation_push(Mouse_K | 0x80);
  Operation_push(color);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  // Taille de pile 8 : phase d'appui, non interruptible
}


void Polyfill_0_8(void)
// Op�ration   : OPERATION_POLYFILL
// Click Souris: 0
// Taille_Pile : 8
//
// Souris effac�e: Oui
{
  short start_x;
  short start_y;
  short end_x;
  short end_y;
  short color;
  short direction;

  Operation_pop(&end_y);
  Operation_pop(&end_x);
  Operation_pop(&start_y);
  Operation_pop(&start_x);
  Operation_pop(&color);
  Operation_pop(&direction);

  if ((Config.Coords_rel) && (Menu_is_visible))
    Print_in_menu("X:�   0   Y:�   0",0);

  Draw_line_preview_xor(start_x,start_y,end_x,end_y,0);

  if (direction & 0x80)
    direction=(direction & 0x7F);

  Operation_push(direction); // Valeur bidon servant de nouvel �tat de pile
  Operation_push(direction);
  Operation_push(color);

  Draw_line_preview_xor(start_x,start_y,Paintbrush_X,Paintbrush_Y,0);

  if (Polyfill_number_of_points<Config.Nb_max_vertices_per_polygon)
  {
    Polyfill_table_of_points[Polyfill_number_of_points<<1]    =Paintbrush_X;
    Polyfill_table_of_points[(Polyfill_number_of_points<<1)+1]=Paintbrush_Y;
    Polyfill_number_of_points++;

    Operation_push(Paintbrush_X);
    Operation_push(Paintbrush_Y);
    Operation_push(Paintbrush_X);
    Operation_push(Paintbrush_Y);
  }
  else
  {
    Operation_push(start_x);
    Operation_push(start_y);
    Operation_push(Paintbrush_X);
    Operation_push(Paintbrush_Y);
  }
  // Taille de pile 9 : phase de "repos", interruptible (comme Elliot Ness :))
}


void Polyfill_12_8(void)
// Op�ration   : OPERATION_POLYFILL
// Click Souris: 1 ou 2 | 0
// Taille_Pile : 8      | 9
//
// Souris effac�e: Non
{
  short start_x;
  short start_y;
  short end_x;
  short end_y;

  Operation_pop(&end_y);
  Operation_pop(&end_x);

  if ((Paintbrush_X!=end_x) || (Paintbrush_Y!=end_y))
  {
    Hide_cursor();
    Operation_pop(&start_y);
    Operation_pop(&start_x);

    Display_coords_rel_or_abs(start_x,start_y);

    Draw_line_preview_xor(start_x,start_y,end_x,end_y,0);
    Draw_line_preview_xor(start_x,start_y,Paintbrush_X,Paintbrush_Y,0);

    Operation_push(start_x);
    Operation_push(start_y);
    Display_cursor();
  }

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


void Polyfill_12_9(void)
// Op�ration   : OPERATION_POLYFILL
// Click Souris: 1 ou 2
// Taille_Pile : 9
//
// Souris effac�e: Oui
{
  short start_x;
  short start_y;
  short end_x;
  short end_y;
  short color;
  short direction;

  Operation_pop(&end_y);
  Operation_pop(&end_x);
  Operation_pop(&start_y);
  Operation_pop(&start_x);
  Operation_pop(&color);
  Operation_pop(&direction);
  Operation_pop(&direction);

  if (direction==Mouse_K)
  {
    Operation_push(direction);
    Operation_push(color);
    Operation_push(start_x);
    Operation_push(start_y);
    Operation_push(end_x);
    Operation_push(end_y);
    // Taille de pile 8 : phase d'appui, non interruptible
  }
  else
  {
    //   La s�rie de lignes est termin�e, il faut donc effacer la derni�re
    // preview de ligne et relier le dernier point avec le premier
    Hide_cursor();
    Draw_line_preview_xor(start_x,start_y,end_x,end_y,0);
    Operation_pop(&end_y);
    Operation_pop(&end_x);
    Draw_line_preview_xor(start_x,start_y,end_x,end_y,0);

    Display_all_screen();
    Polyfill(Polyfill_number_of_points,Polyfill_table_of_points,color);
    free(Polyfill_table_of_points);

    End_of_modification();
    if ( (Config.Coords_rel) && (Menu_is_visible) )
    {
      Print_in_menu("X:       Y:             ",0);
      Print_coordinates();
    }

    Paintbrush_hidden=0;

    Display_cursor();
    Wait_end_of_click();
  }
}


////////////////////////////////////////////////////////// OPERATION_POLYFORM


void Polyform_12_0(void)
//  Op�ration   : OPERATION_POLYFORM
//  Click Souris: 1 ou 2
//  Taille_Pile : 0
//
//  Souris effac�e: Oui
{
  short color;

  Init_start_operation();
  Backup();
  Shade_table=(Mouse_K==LEFT_SIDE)?Shade_table_left:Shade_table_right;

  color=(Mouse_K==LEFT_SIDE)?Fore_color:Back_color;

  // On place un premier pinceau en (Paintbrush_X,Paintbrush_Y):
  Display_paintbrush(Paintbrush_X,Paintbrush_Y,color,0);
  // Et on affiche un pixel de preview en (Paintbrush_X,Paintbrush_Y):
  Pixel_figure_preview(Paintbrush_X,Paintbrush_Y,color);

  Operation_push(Paintbrush_X); // X Initial
  Operation_push(Paintbrush_Y); // X Initial
  Operation_push(color);   // color de remplissage
  Operation_push(Paintbrush_X); // Start X
  Operation_push(Paintbrush_Y); // Start Y
  Operation_push(Paintbrush_X); // End X
  Operation_push(Paintbrush_Y); // End Y
  Operation_push(Mouse_K);   // click
}


void Polyform_12_8(void)
//  Op�ration   : OPERATION_POLYFORM
//  Click Souris: 1 ou 2
//  Taille_Pile : 8
//
//  Souris effac�e: Non
{
  short click;
  short end_y;
  short end_x;
  short start_y;
  short start_x;
  short color;
  short initial_y;
  short initial_x;

  Operation_pop(&click);
  Operation_pop(&end_y);
  Operation_pop(&end_x);
  Operation_pop(&start_y);
  Operation_pop(&start_x);

  if (click==Mouse_K)
  {
    // L'utilisateur clic toujours avec le bon bouton de souris

    if ((start_x!=Paintbrush_X) || (start_y!=Paintbrush_Y))
    {
      // Il existe un segment d�finit par (start_x,start_y)-(Paintbrush_X,Paintbrush_Y)

      Hide_cursor();
      Print_coordinates();

      Operation_pop(&color);

      // On efface la preview du segment valid�:
      Pixel_figure_preview_auto  (start_x,start_y);
      Hide_line_preview(start_x,start_y,end_x,end_y);

      // On l'affiche de fa�on d�finitive:
      Draw_line_permanent(start_x,start_y,Paintbrush_X,Paintbrush_Y,color);

      // Et on affiche un pixel de preview en (Paintbrush_X,Paintbrush_Y):
      Pixel_figure_preview(Paintbrush_X,Paintbrush_Y,color);

      Operation_push(color);

      Display_cursor();
    }

    Operation_push(Paintbrush_X); // Nouveau start_x
    Operation_push(Paintbrush_Y); // Nouveau start_y
    Operation_push(Paintbrush_X); // Nouveau end_x
    Operation_push(Paintbrush_Y); // Nouveau end_y
    Operation_push(click);
  }
  else
  {
    // L'utilisateur souhaite arr�ter l'op�ration et refermer le polygone

    Operation_pop(&color);
    Operation_pop(&initial_y);
    Operation_pop(&initial_x);

    Hide_cursor();
    Print_coordinates();

    // On efface la preview du segment annul�:
    Hide_line_preview(start_x,start_y,end_x,end_y);

    // On affiche de fa�on d�finitive le bouclage du polygone:
    Draw_line_permanent(start_x,start_y,initial_x,initial_y,color);

    Display_cursor();
    End_of_modification();
    Wait_end_of_click();
  }
}


void Polyform_0_8(void)
//  Op�ration   : OPERATION_POLYFORM
//  Click Souris: 0
//  Taille_Pile : 8
//
//  Souris effac�e: Non
{
  short click;
  short end_y;
  short end_x;
  short start_y;
  short start_x;
  short color;

  Operation_pop(&click);
  Operation_pop(&end_y);
  Operation_pop(&end_x);

  if ((end_x!=Paintbrush_X) || (end_y!=Paintbrush_Y))
  {
    Hide_cursor();
    Print_coordinates();

    Operation_pop(&start_y);
    Operation_pop(&start_x);
    Operation_pop(&color);

    // On met � jour l'affichage de la preview du prochain segment:
    Hide_line_preview(start_x,start_y,end_x,end_y);
    Draw_line_preview (start_x,start_y,Paintbrush_X,Paintbrush_Y,color);

    Operation_push(color);
    Operation_push(start_x);
    Operation_push(start_y);

    Display_cursor();
  }

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Operation_push(click);
}


/////////////////////////////////////////////////// OPERATION_FILLED_POLYFORM


void Filled_polyform_12_0(void)
//  Op�ration   : OPERATION_FILLED_POLYFORM
//  Click Souris: 1 ou 2
//  Taille_Pile : 0
//
//  Souris effac�e: Oui
{
  short color;

  Init_start_operation();

  // Cette op�ration �tant �galement utilis�e pour le lasso, on ne fait pas de
  // backup si on prend une brosse au lasso avec le bouton gauche.
  if ((Current_operation==OPERATION_FILLED_POLYFORM) || (Current_operation==OPERATION_FILLED_CONTOUR) || (Mouse_K==RIGHT_SIDE))
    Backup();

  Shade_table=(Mouse_K==LEFT_SIDE)?Shade_table_left:Shade_table_right;
  Paintbrush_hidden=1;

  color=(Mouse_K==LEFT_SIDE)?Fore_color:Back_color;

  Polyfill_table_of_points=(short *) malloc((Config.Nb_max_vertices_per_polygon<<1)*sizeof(short));
  Polyfill_table_of_points[0]=Paintbrush_X;
  Polyfill_table_of_points[1]=Paintbrush_Y;
  Polyfill_number_of_points=1;

  // On place temporairement le d�but de la ligne qui ne s'afficherait pas sinon
  Pixel_figure_preview_xor(Paintbrush_X,Paintbrush_Y,0);
  Update_part_of_screen(Paintbrush_X,Paintbrush_Y,1,1);
  
  Operation_push(Paintbrush_X); // X Initial
  Operation_push(Paintbrush_Y); // X Initial
  Operation_push(color);   // color de remplissage
  Operation_push(Paintbrush_X); // Start X
  Operation_push(Paintbrush_Y); // Start Y
  Operation_push(Paintbrush_X); // End X
  Operation_push(Paintbrush_Y); // End Y
  Operation_push(Mouse_K);   // click
}


void Filled_polyform_12_8(void)
//  Op�ration   : OPERATION_FILLED_POLYFORM
//  Click Souris: 1 ou 2
//  Taille_Pile : 8
//
//  Souris effac�e: Non
{
  short click;
  short end_y;
  short end_x;
  short start_y;
  short start_x;
  short color;
  short initial_y;
  short initial_x;

  Operation_pop(&click);
  Operation_pop(&end_y);
  Operation_pop(&end_x);
  Operation_pop(&start_y);
  Operation_pop(&start_x);

  if (click==Mouse_K)
  {
    // L'utilisateur clique toujours avec le bon bouton de souris

    if (((start_x!=Paintbrush_X) || (start_y!=Paintbrush_Y)) &&
        (Polyfill_number_of_points<Config.Nb_max_vertices_per_polygon))
    {
      // Il existe un nouveau segment d�fini par
      // (start_x,start_y)-(Paintbrush_X,Paintbrush_Y)

      Hide_cursor();
      Print_coordinates();

      // On le place � l'�cran
      if (Current_operation==OPERATION_FILLED_CONTOUR)
      {
        Draw_line_preview_xorback(start_x,start_y,end_x,end_y,0);
        Draw_line_preview_xorback(start_x,start_y,Paintbrush_X,Paintbrush_Y,0);
      }
      else
      {
        Draw_line_preview_xor(start_x,start_y,end_x,end_y,0);
        Draw_line_preview_xor(start_x,start_y,Paintbrush_X,Paintbrush_Y,0);
      }

      // On peut le rajouter au polygone

      Polyfill_table_of_points[Polyfill_number_of_points<<1]    =Paintbrush_X;
      Polyfill_table_of_points[(Polyfill_number_of_points<<1)+1]=Paintbrush_Y;
      Polyfill_number_of_points++;

      Operation_push(Paintbrush_X); // Nouveau start_x
      Operation_push(Paintbrush_Y); // Nouveau start_y
      Operation_push(Paintbrush_X); // Nouveau end_x
      Operation_push(Paintbrush_Y); // Nouveau end_y
      Operation_push(click);

      Display_cursor();
    }
    else
    {
      if (Polyfill_number_of_points==Config.Nb_max_vertices_per_polygon)
      {
        // Le curseur bouge alors qu'on ne peut plus stocker de segments ?

        if ((end_x!=Paintbrush_X) || (end_y!=Paintbrush_Y))
        {
          Hide_cursor();
          Print_coordinates();

          // On le place � l'�cran
          Draw_line_preview_xor(start_x,start_y,end_x,end_y,0);
          Draw_line_preview_xor(start_x,start_y,Paintbrush_X,Paintbrush_Y,0);
          Display_cursor();
        }

        // On remet les m�mes valeurs (comme si on n'avait pas cliqu�):
        Operation_push(start_x);
        Operation_push(start_y);
        Operation_push(Paintbrush_X);
        Operation_push(Paintbrush_Y);
        Operation_push(click);
      }
      else
      {
        Operation_push(Paintbrush_X); // Nouveau start_x
        Operation_push(Paintbrush_Y); // Nouveau start_y
        Operation_push(Paintbrush_X); // Nouveau end_x
        Operation_push(Paintbrush_Y); // Nouveau end_y
        Operation_push(click);
      }
    }
  }
  else
  {
    // L'utilisateur souhaite arr�ter l'op�ration et refermer le polygone

    Operation_pop(&color);
    Operation_pop(&initial_y);
    Operation_pop(&initial_x);

    Hide_cursor();
    Print_coordinates();

    // Pas besoin d'effacer la ligne (start_x,start_y)-(end_x,end_y)
    // puisque on les effaces toutes d'un coup.

    Display_all_screen();
    Polyfill(Polyfill_number_of_points,Polyfill_table_of_points,color);
    free(Polyfill_table_of_points);

    Paintbrush_hidden=0;

    Display_cursor();
    End_of_modification();
    Wait_end_of_click();
  }
}


void Filled_polyform_0_8(void)
//  Op�ration   : OPERATION_FILLED_POLYFORM
//  Click Souris: 0
//  Taille_Pile : 8
//
//  Souris effac�e: Non
{
  short click;
  short end_y;
  short end_x;
  short start_y;
  short start_x;

  Operation_pop(&click);
  Operation_pop(&end_y);
  Operation_pop(&end_x);

  if ((end_x!=Paintbrush_X) || (end_y!=Paintbrush_Y))
  {
    Hide_cursor();
    Print_coordinates();

    Operation_pop(&start_y);
    Operation_pop(&start_x);

    // On met � jour l'affichage de la preview du prochain segment:
    Draw_line_preview_xor(start_x,start_y,end_x,end_y,0);
    Draw_line_preview_xor(start_x,start_y,Paintbrush_X,Paintbrush_Y,0);

    Operation_push(start_x);
    Operation_push(start_y);

    Display_cursor();
  }

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Operation_push(click);
}

void Filled_contour_0_8(void)
//  Op�ration   : OPERATION_FILLED_CONTOUR
//  Click Souris: 0
//  Taille_Pile : 8
//
//  Souris effac�e: Non
{
  short click;
  short end_y;
  short end_x;
  short start_y;
  short start_x;
  short color;
  short initial_y;
  short initial_x;

  Operation_pop(&click);
  Operation_pop(&end_y);
  Operation_pop(&end_x);
  Operation_pop(&start_y);
  Operation_pop(&start_x);
  Operation_pop(&color);
  Operation_pop(&initial_y);
  Operation_pop(&initial_x);

  Hide_cursor();
  Print_coordinates();

  // Pas besoin d'effacer la ligne (start_x,start_y)-(end_x,end_y)
  // puisque on les effaces toutes d'un coup.

  Display_all_screen();
  Polyfill(Polyfill_number_of_points,Polyfill_table_of_points,color);
  free(Polyfill_table_of_points);

  Paintbrush_hidden=0;

  Display_cursor();
  End_of_modification();
}

////////////////////////////////////////////////////// OPERATION_GRAB_BRUSH


void Brush_12_0(void)
//
// Op�ration   : OPERATION_GRAB_BRUSH
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
//
{
  Init_start_operation();
  if (Mouse_K==RIGHT_SIDE) // Besoin d'effacer la brosse apr�s ?
  {
    Operation_push(1);
    // Puisque la zone o� on prend la brosse sera effac�e, on fait un backup
    Backup();
  }
  else
    Operation_push(0);

  // On laisse une trace du curseur pour que l'utilisateur puisse visualiser
  // o� demarre sa brosse:
  Display_cursor();

  Operation_push(Paintbrush_X); // D�but X
  Operation_push(Paintbrush_Y); // D�but Y
  Operation_push(Paintbrush_X); // Derni�re position X
  Operation_push(Paintbrush_Y); // Derni�re position Y

  if ((Config.Coords_rel) && (Menu_is_visible))
    Print_in_menu("\035:   1   \022:   1",0);
}


void Brush_12_5(void)
//
// Op�ration   : OPERATION_GRAB_BRUSH
// Click Souris: 1 ou 2
// Taille_Pile : 5
//
// Souris effac�e: Non
//
{
  char  str[5];
  short start_x;
  short start_y;
  short old_x;
  short old_y;
  short width;
  short height;

  Operation_pop(&old_y);
  Operation_pop(&old_x);

  if ( (Menu_is_visible) && ((Paintbrush_X!=old_x) || (Paintbrush_Y!=old_y)) )
  {
    if (Config.Coords_rel)
    {
      Operation_pop(&start_y);
      Operation_pop(&start_x);
      Operation_push(start_x);
      Operation_push(start_y);

      width=((start_x<Paintbrush_X)?Paintbrush_X-start_x:start_x-Paintbrush_X)+1;
      height=((start_y<Paintbrush_Y)?Paintbrush_Y-start_y:start_y-Paintbrush_Y)+1;

      Num2str(width,str,4);
      Print_in_menu(str,2);
      Num2str(height,str,4);
      Print_in_menu(str,11);
    }
    else
      Print_coordinates();
  }

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


void Brush_0_5(void)
//
// Op�ration   : OPERATION_GRAB_BRUSH
// Click Souris: 0
// Taille_Pile : 5
//
// Souris effac�e: Oui
//
{
  short start_x;
  short start_y;
  short old_paintbrush_x;
  short old_paintbrush_y;
  short clear;

  // Comme on a demand� l'effacement du curseur, il n'y a plus de croix en
  // (Paintbrush_X,Paintbrush_Y). C'est une bonne chose.

  Operation_stack_size-=2;
  Operation_pop(&start_y);
  Operation_pop(&start_x);
  Operation_pop(&clear);

  // On efface l'ancienne croix:
  old_paintbrush_x=Paintbrush_X;
  old_paintbrush_y=Paintbrush_Y;
  Paintbrush_X=start_x;
  Paintbrush_Y=start_y;
  Hide_cursor(); // Maintenant, il n'y a plus de croix � l'�cran.

  Paintbrush_X=old_paintbrush_x;
  Paintbrush_Y=old_paintbrush_y;

  // Prise de la brosse
  if ((Snap_mode) && (Config.Adjust_brush_pick))
  {
    if (Paintbrush_X<start_x)
    {
      old_paintbrush_x=start_x;
      start_x=Paintbrush_X;
    }
    if (Paintbrush_Y<start_y)
    {
      old_paintbrush_y=start_y;
      start_y=Paintbrush_Y;
    }
    if (old_paintbrush_x!=start_x)
      old_paintbrush_x--;
    if (old_paintbrush_y!=start_y)
      old_paintbrush_y--;
  }
  Capture_brush(start_x,start_y,old_paintbrush_x,old_paintbrush_y,clear);
  if ((Snap_mode) && (Config.Adjust_brush_pick))
  {
    Brush_offset_X=(Brush_offset_X/Snap_width)*Snap_width;
    Brush_offset_Y=(Brush_offset_Y/Snap_height)*Snap_height;
  }
  
  End_of_modification();
  End_of_modification();
  Return_to_draw_mode();
}


//////////////////////////////////////////////////////// OPERATION_POLYBRUSH


void Polybrush_12_8(void)
//  Op�ration   : OPERATION_POLYBRUSH
//  Click Souris: 1 ou 2
//  Taille_Pile : 8
//
//  Souris effac�e: Non
{
  short click;
  short end_y;
  short end_x;
  short start_y;
  short start_x;
  short color;
  short initial_y;
  short initial_x;

  Operation_pop(&click);
  Operation_pop(&end_y);
  Operation_pop(&end_x);
  Operation_pop(&start_y);
  Operation_pop(&start_x);

  if (click==Mouse_K)
  {
    // L'utilisateur clique toujours avec le bon bouton de souris

    if (((start_x!=Paintbrush_X) || (start_y!=Paintbrush_Y)) &&
        (Polyfill_number_of_points<Config.Nb_max_vertices_per_polygon))
    {
      // Il existe un nouveau segment d�fini par
      // (start_x,start_y)-(Paintbrush_X,Paintbrush_Y)

      Hide_cursor();
      Print_coordinates();

      // On le place � l'�cran
      Draw_line_preview_xor(start_x,start_y,end_x,end_y,0);
      Draw_line_preview_xor(start_x,start_y,Paintbrush_X,Paintbrush_Y,0);

      // On peut le rajouter au polygone

      Polyfill_table_of_points[Polyfill_number_of_points<<1]    =Paintbrush_X;
      Polyfill_table_of_points[(Polyfill_number_of_points<<1)+1]=Paintbrush_Y;
      Polyfill_number_of_points++;

      Operation_push(Paintbrush_X); // Nouveau start_x
      Operation_push(Paintbrush_Y); // Nouveau start_y
      Operation_push(Paintbrush_X); // Nouveau end_x
      Operation_push(Paintbrush_Y); // Nouveau end_y
      Operation_push(click);

      Display_cursor();
    }
    else
    {
      if (Polyfill_number_of_points==Config.Nb_max_vertices_per_polygon)
      {
        // Le curseur bouge alors qu'on ne peut plus stocker de segments ?

        if ((end_x!=Paintbrush_X) || (end_y!=Paintbrush_Y))
        {
          Hide_cursor();
          Print_coordinates();

          // On le place � l'�cran
          Draw_line_preview_xor(start_x,start_y,end_x,end_y,0);
          Draw_line_preview_xor(start_x,start_y,Paintbrush_X,Paintbrush_Y,0);
          Display_cursor();
        }

        // On remet les m�mes valeurs (comme si on n'avait pas cliqu�):
        Operation_push(start_x);
        Operation_push(start_y);
        Operation_push(Paintbrush_X);
        Operation_push(Paintbrush_Y);
        Operation_push(click);
      }
      else
      {
        Operation_push(Paintbrush_X); // Nouveau start_x
        Operation_push(Paintbrush_Y); // Nouveau start_y
        Operation_push(Paintbrush_X); // Nouveau end_x
        Operation_push(Paintbrush_Y); // Nouveau end_y
        Operation_push(click);
      }
    }
  }
  else
  {
    // L'utilisateur souhaite arr�ter l'op�ration et refermer le polygone

    Operation_pop(&color);
    Operation_pop(&initial_y);
    Operation_pop(&initial_x);

    Hide_cursor();
    Print_coordinates();

    // Pas besoin d'effacer la ligne (start_x,start_y)-(end_x,end_y)
    // puisqu'on les efface toutes d'un coup.

    Capture_brush_with_lasso(Polyfill_number_of_points,Polyfill_table_of_points,click==RIGHT_SIDE);
    free(Polyfill_table_of_points);

    // On raffiche l'�cran pour effacer les traits en xor et pour raffraichir
    // l'�cran si on a d�coup� une partie de l'image en prenant la brosse.
    Display_all_screen();

    Paintbrush_hidden=0;

    if ((Snap_mode) && (Config.Adjust_brush_pick))
    {
      Brush_offset_X=(Brush_offset_X/Snap_width)*Snap_width;
      Brush_offset_Y=(Brush_offset_Y/Snap_height)*Snap_height;
    }
    
    if (click==RIGHT_SIDE)
      End_of_modification();
    Return_to_draw_mode();
    Display_cursor();
  }
}


///////////////////////////////////////////////////// OPERATION_STRETCH_BRUSH


void Stretch_brush_12_0(void)
//
// Op�ration   : OPERATION_STRETCH_BRUSH
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
//
{
  Init_start_operation();
  if (Mouse_K==LEFT_SIDE)
  {
    // On laisse une trace du curseur pour que l'utilisateur puisse visualiser
    // o� demarre sa brosse:
    Display_cursor();

    Operation_push(Paintbrush_X); // Dernier calcul X
    Operation_push(Paintbrush_Y); // Dernier calcul Y
    Operation_push(Paintbrush_X); // D�but X
    Operation_push(Paintbrush_Y); // D�but Y
    Operation_push(Paintbrush_X); // Derni�re position X
    Operation_push(Paintbrush_Y); // Derni�re position Y
    Operation_push(1); // State pr�c�dent

    if ((Config.Coords_rel) && (Menu_is_visible))
      Print_in_menu("\035:   1   \022:   1",0);
  }
  else
  {
    Wait_end_of_click();
    Start_operation_stack(Operation_before_interrupt);
  }
}



void Stretch_brush_1_7(void)
//
// Op�ration   : OPERATION_STRETCH_BRUSH
// Click Souris: 1
// Taille_Pile : 7
//
// Souris effac�e: Non
//
{
  char  str[5];
  short start_x;
  short start_y;
  short old_x;
  short old_y;
  short width;
  short height;
  short prev_state;
  short dx,dy,x,y;

  Operation_pop(&prev_state);
  Operation_pop(&old_y);
  Operation_pop(&old_x);

  if ( (Paintbrush_X!=old_x) || (Paintbrush_Y!=old_y) || (prev_state!=2) )
  {
    Hide_cursor();
    Operation_pop(&start_y);
    Operation_pop(&start_x);

    if (Menu_is_visible)
    {
      if (Config.Coords_rel)
      {
        width=((start_x<Paintbrush_X)?Paintbrush_X-start_x:start_x-Paintbrush_X)+1;
        height=((start_y<Paintbrush_Y)?Paintbrush_Y-start_y:start_y-Paintbrush_Y)+1;

        if (Snap_mode && Config.Adjust_brush_pick)
        {
          if (width>1) width--;
          if (height>1) height--;
        }

        Num2str(width,str,4);
        Print_in_menu(str,2);
        Num2str(height,str,4);
        Print_in_menu(str,11);
      }
      else
        Print_coordinates();
    }

    Display_all_screen();

    x=Paintbrush_X;
    y=Paintbrush_Y;
    if (Snap_mode && Config.Adjust_brush_pick)
    {
      dx=Paintbrush_X-start_x;
      dy=Paintbrush_Y-start_y;
      if (dx<0) x++; else {if (dx>0) x--;}
      if (dy<0) y++; else {if (dy>0) y--;}
      Stretch_brush_preview(start_x,start_y,x,y);
    }
    else
      Stretch_brush_preview(start_x,start_y,Paintbrush_X,Paintbrush_Y);

    old_x=Paintbrush_X;
    old_y=Paintbrush_Y;
    Paintbrush_X=start_x;
    Paintbrush_Y=start_y;
    Display_cursor();
    Paintbrush_X=old_x;
    Paintbrush_Y=old_y;
    Display_cursor();

    Operation_stack_size-=2;
    Operation_push(x);
    Operation_push(y);

    Operation_push(start_x);
    Operation_push(start_y);
  }

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Operation_push(2);
}



void Stretch_brush_0_7(void)
//
// Op�ration   : OPERATION_STRETCH_BRUSH
// Click Souris: 0
// Taille_Pile : 7
//
// Souris effac�e: Non
//
{
  char  str[5];
  short start_x;
  short start_y;
  short old_x;
  short old_y;
  short width=0;
  short height=0;
  byte  size_change;
  short prev_state;

  Operation_pop(&prev_state);
  Operation_pop(&old_y);
  Operation_pop(&old_x);
  Operation_pop(&start_y);
  Operation_pop(&start_x);

  if ((Paintbrush_X!=old_x) || (Paintbrush_Y!=old_y) || (prev_state!=3))
  {
    if (Menu_is_visible)
    {
      if (Config.Coords_rel)
      {
        width=((start_x<Paintbrush_X)?Paintbrush_X-start_x:start_x-Paintbrush_X)+1;
        height=((start_y<Paintbrush_Y)?Paintbrush_Y-start_y:start_y-Paintbrush_Y)+1;

        Num2str(width,str,4);
        Print_in_menu(str,2);
        Num2str(height,str,4);
        Print_in_menu(str,11);
      }
      else
        Print_coordinates();
    }
  }

  // Utilise Key_ANSI au lieu de Key, car Get_input() met ce dernier
  // � zero si une operation est en cours (Operation_stack_size!=0)
  if (Key_ANSI)
  {
    size_change=1;
    switch (Key_ANSI)
    {
      case 'd': // Double
        width=start_x+(Brush_width<<1)-1;
        height=start_y+(Brush_height<<1)-1;
        break;
      case 'x': // Double X
        width=start_x+(Brush_width<<1)-1;
        height=start_y+Brush_height-1;
        break;
      case 'y': // Double Y
        width=start_x+Brush_width-1;
        height=start_y+(Brush_height<<1)-1;
        break;
      case 'h': // Moiti�
        width=(Brush_width>1)?start_x+(Brush_width>>1)-1:1;
        height=(Brush_height>1)?start_y+(Brush_height>>1)-1:1;
        break;
      case 'X': // Moiti� X
        width=(Brush_width>1)?start_x+(Brush_width>>1)-1:1;
        height=start_y+Brush_height-1;
        break;
      case 'Y': // Moiti� Y
        width=start_x+Brush_width-1;
        height=(Brush_height>1)?start_y+(Brush_height>>1)-1:1;
        break;
      case 'n': // Normal
        width=start_x+Brush_width-1;
        height=start_y+Brush_height-1;
        break;
      default :
        size_change=0;
    }
    Key_ANSI=0;
  }
  else
    size_change=0;

  if (size_change)
  {
    // On efface la preview de la brosse (et la croix)
    Display_all_screen();

    old_x=Paintbrush_X;
    old_y=Paintbrush_Y;
    Paintbrush_X=start_x;
    Paintbrush_Y=start_y;
    Display_cursor();
    Paintbrush_X=old_x;
    Paintbrush_Y=old_y;

    Stretch_brush_preview(start_x,start_y,width,height);
    Display_cursor();

    Operation_stack_size-=2;
    Operation_push(width);
    Operation_push(height);
  }

  Operation_push(start_x);
  Operation_push(start_y);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Operation_push(3);
}


void Stretch_brush_2_7(void)
//
// Op�ration   : OPERATION_STRETCH_BRUSH
// Click Souris: 2
// Taille_Pile : 7
//
// Souris effac�e: Oui
//
{
  short computed_x;
  short computed_y;
  short start_x;
  short start_y;


  Operation_stack_size-=3;
  Operation_pop(&start_y);
  Operation_pop(&start_x);
  Operation_pop(&computed_y);
  Operation_pop(&computed_x);

  // On efface la preview de la brosse (et la croix)
  Display_all_screen();

  // Et enfin on stocke pour de bon la nouvelle brosse �tir�e
  Stretch_brush(start_x,start_y,computed_x,computed_y);

  Return_to_draw_mode();
}


//////////////////////////////////////////////////// OPERATION_ROTATE_BRUSH


void Rotate_brush_12_0(void)
//
// Op�ration   : OPERATION_ROTATE_BRUSH
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
//
{
  Init_start_operation();
  if (Mouse_K==LEFT_SIDE)
  {
    Brush_rotation_center_X=Paintbrush_X+(Brush_width>>1)-Brush_width;
    Brush_rotation_center_Y=Paintbrush_Y;
    Brush_rotation_center_is_defined=1;
    Operation_push(Paintbrush_X); // Derni�re position calcul�e X
    Operation_push(Paintbrush_Y); // Derni�re position calcul�e Y
    Operation_push(Paintbrush_X); // Derni�re position X
    Operation_push(Paintbrush_Y); // Derni�re position Y
    Operation_push(1); // State pr�c�dent

    if ((Config.Coords_rel) && (Menu_is_visible))
      Print_in_menu("Angle:   0�    ",0);
  }
  else
  {
    Start_operation_stack(Operation_before_interrupt);
    Wait_end_of_click(); // FIXME: celui-la il donne un r�sultat pas tr�s chouette en visuel
  }
}



void Rotate_brush_1_5(void)
//
// Op�ration   : OPERATION_ROTATE_BRUSH
// Click Souris: 1
// Taille_Pile : 5
//
// Souris effac�e: Non
//
{
  char  str[4];
  short old_x;
  short old_y;
  short prev_state;
  float angle;
  int dx,dy;

  Operation_pop(&prev_state);
  Operation_pop(&old_y);
  Operation_pop(&old_x);

  if ( (Paintbrush_X!=old_x) || (Paintbrush_Y!=old_y) || (prev_state!=2) )
  {
    if ( (Brush_rotation_center_X==Paintbrush_X)
      && (Brush_rotation_center_Y==Paintbrush_Y) )
      angle=0.0;
    else
    {
      dx=Paintbrush_X-Brush_rotation_center_X;
      dy=Paintbrush_Y-Brush_rotation_center_Y;
      angle=acos(((float)dx)/sqrt((dx*dx)+(dy*dy)));
      if (dy>0) angle=M_2PI-angle;
    }

    if (Menu_is_visible)
    {
      if (Config.Coords_rel)
      {
        Num2str((int)(angle*180.0/M_PI),str,3);
        Print_in_menu(str,7);
      }
      else
        Print_coordinates();
    }

    Display_all_screen();
    Rotate_brush_preview(angle);
    Display_cursor();

    Operation_stack_size-=2;
    Operation_push(Paintbrush_X);
    Operation_push(Paintbrush_Y);
  }

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Operation_push(2);
}



void Rotate_brush_0_5(void)
//
// Op�ration   : OPERATION_ROTATE_BRUSH
// Click Souris: 0
// Taille_Pile : 5
//
// Souris effac�e: Non
//
{
  char  str[4];
  short old_x;
  short old_y;
  short computed_x=0;
  short computed_y=0;
  byte  angle_change;
  short prev_state;
  float angle=0.0;
  int dx,dy;

  Operation_pop(&prev_state);
  Operation_pop(&old_y);
  Operation_pop(&old_x);

  if ((Paintbrush_X!=old_x) || (Paintbrush_Y!=old_y) || (prev_state!=3))
  {
    if ( (Brush_rotation_center_X==Paintbrush_X)
      && (Brush_rotation_center_Y==Paintbrush_Y) )
      angle=0.0;
    else
    {
      dx=Paintbrush_X-Brush_rotation_center_X;
      dy=Paintbrush_Y-Brush_rotation_center_Y;
      angle=acos(((float)dx)/sqrt((dx*dx)+(dy*dy)));
      if (dy>0) angle=M_2PI-angle;
    }

    if (Menu_is_visible)
    {
      if (Config.Coords_rel)
      {
        Num2str(Round(angle*180.0/M_PI),str,3);
        Print_in_menu(str,7);
      }
      else
        Print_coordinates();
    }
  }

  // Utilise Key_ANSI au lieu de Key, car Get_input() met ce dernier
  // � zero si une operation est en cours (Operation_stack_size!=0)
  if (Key_ANSI)
  {
    angle_change=1;
    computed_x=Brush_rotation_center_X;
    computed_y=Brush_rotation_center_Y;
    switch (Key_ANSI)
    {
      case '6': angle=     0.0 ; computed_x++;             break;
      case '9': angle=M_PI*0.25; computed_x++; computed_y--; break;
      case '8': angle=M_PI*0.5 ;             computed_y--; break;
      case '7': angle=M_PI*0.75; computed_x--; computed_y--; break;
      case '4': angle=M_PI     ; computed_x--;             break;
      case '1': angle=M_PI*1.25; computed_x--; computed_y++; break;
      case '2': angle=M_PI*1.5 ;             computed_y++; break;
      case '3': angle=M_PI*1.75; computed_x++; computed_y++; break;
      default :
        angle_change=0;
    }
    Key_ANSI=0;
  }
  else
    angle_change=0;

  if (angle_change)
  {
    // On efface la preview de la brosse
    Display_all_screen();
    Rotate_brush_preview(angle);
    Display_cursor();

    Operation_stack_size-=2;
    Operation_push(computed_x);
    Operation_push(computed_y);
  }

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Operation_push(3);
}


void Rotate_brush_2_5(void)
//
// Op�ration   : OPERATION_ROTATE_BRUSH
// Click Souris: 2
// Taille_Pile : 5
//
// Souris effac�e: Oui
//
{
  short computed_x;
  short computed_y;
  int dx,dy;
  float angle;


  // On efface la preview de la brosse
  Display_all_screen();

  Operation_stack_size-=3;
  Operation_pop(&computed_y);
  Operation_pop(&computed_x);

  // Calcul de l'angle par rapport � la derni�re position calcul�e
  if ( (Brush_rotation_center_X==computed_x)
    && (Brush_rotation_center_Y==computed_y) )
    angle=0.0;
  else
  {
    dx=computed_x-Brush_rotation_center_X;
    dy=computed_y-Brush_rotation_center_Y;
    angle=acos(((float)dx)/sqrt((dx*dx)+(dy*dy)));
    if (dy>0) angle=M_2PI-angle;
  }

  // Et enfin on stocke pour de bon la nouvelle brosse �tir�e
  Rotate_brush(angle);

  Return_to_draw_mode();
}

///////////////////////////////////////////////////// OPERATION_DISTORT_BRUSH

/// Draws a 2x2 XOR square at the specified picture coordinates, on the screen.
void Draw_stretch_spot(short x_pos, short y_pos)
{
  short x,y;

  for (y=y_pos-1;y<y_pos+1;y++)
    if (y>=Limit_top && y<=Limit_visible_bottom)
      for (x=x_pos-1;x<x_pos+1;x++)
        if (x>=Limit_left && x<=Limit_visible_right)
          Pixel_preview(x,y,~Read_pixel(x-Main_offset_X,y-Main_offset_Y));
  Update_part_of_screen(x_pos-1, y_pos-1, 2, 2);
}

void Distort_brush_0_0(void)
//
// Op�ration   : OPERATION_DISTORT_BRUSH
// Click Souris: 0
// Taille_Pile : 0
//
// Souris effac�e: Non
//
{
  if ( Menu_is_visible )
  {
      Print_in_menu("POSITION BRUSH TO START ",0);
  }
}

void Distort_brush_1_0(void)
//
// Op�ration   : OPERATION_DISTORT_BRUSH
// Click Souris: 1
// Taille_Pile : 0
//
// Souris effac�e: Non
//
{
  short x_pos, y_pos;

  Init_start_operation();
  Paintbrush_hidden=1;
  Hide_cursor();
  
  // Top left angle
  x_pos=Paintbrush_X-Brush_offset_X;
  y_pos=Paintbrush_Y-Brush_offset_Y;
  Draw_stretch_spot(x_pos,y_pos);
  Operation_push(x_pos);
  Operation_push(y_pos);
  
  // Top right angle
  x_pos+=Brush_width;
  Draw_stretch_spot(x_pos,y_pos);
  Operation_push(x_pos);
  Operation_push(y_pos);
  
  // Bottom right angle
  y_pos+=Brush_height;
  Draw_stretch_spot(x_pos,y_pos);
  Operation_push(x_pos);
  Operation_push(y_pos);
  
  // Bottom left angle
  x_pos-=Brush_width;
  Draw_stretch_spot(x_pos,y_pos);
  Operation_push(x_pos);
  Operation_push(y_pos);
  
  Distort_brush_preview(
    Operation_stack[1],
    Operation_stack[2],
    Operation_stack[3],
    Operation_stack[4],
    Operation_stack[5],
    Operation_stack[6],
    Operation_stack[7],
    Operation_stack[8]);
  Display_cursor();
  Update_part_of_screen(Paintbrush_X-Brush_offset_X, Paintbrush_Y-Brush_offset_Y, Brush_width, Brush_height);
  Wait_end_of_click();
  // Erase the message in status bar
  if ( (Config.Coords_rel) && (Menu_is_visible) )
  {
      Print_in_menu("X:       Y:             ",0);
  }
}

void Distort_brush_1_8(void)
//
//  Op�ration   : OPERATION_DISTORT_BRUSH
//  Click Souris: 1
//  Taille_Pile : 8
//
//  Souris effac�e: No
//
{
  // How far (in pixels) you can catch a handle
  #define REACH_DISTANCE 100
  short i;
  short x[4];
  short y[4];
  long best_distance=REACH_DISTANCE;
  short best_spot=-1;

  for (i=3;i>=0;i--)
  {
    long distance;
    Operation_pop(&y[i]);
    Operation_pop(&x[i]);
    distance=Distance(Paintbrush_X,Paintbrush_Y,x[i],y[i]);
    if (distance<best_distance)
    {
      best_spot=i;
      best_distance=distance;
    }
  }
  
  for (i=0;i<4;i++)
  {
    Operation_push(x[i]);
    Operation_push(y[i]);
  }
  
  if (best_spot>-1)
  {
    Operation_push(best_spot);
  }
  if ( (Config.Coords_rel) && (Menu_is_visible) )
  {
      Print_in_menu("X:       Y:             ",0);
      Print_coordinates();
  }
}

void Distort_brush_1_9(void)
//
//  Op�ration   : OPERATION_DISTORT_BRUSH
//  Click Souris: 1
//  Taille_Pile : 9
//
//  Souris effac�e: No
//
{
  short i;
  short x[4];
  short y[4];
  short selected_corner;

  // Pop all arguments
  Operation_pop(&selected_corner);
  for (i=3;i>=0;i--)
  {
    Operation_pop(&y[i]);
    Operation_pop(&x[i]);
  }
  
  if (Paintbrush_X!=x[selected_corner] || Paintbrush_Y!=y[selected_corner])
  {  
    Hide_cursor();
    
    // Easiest refresh mode: make no assumptions on how the brush was
    // displayed before.
    Display_all_screen();

    x[selected_corner]=Paintbrush_X;
    y[selected_corner]=Paintbrush_Y;

    for (i=0;i<4;i++)
      Draw_stretch_spot(x[i],y[i]);

    Distort_brush_preview(x[0],y[0],x[1],y[1],x[2],y[2],x[3],y[3]);

    Display_cursor();
    
    if ( (Config.Coords_rel) && (Menu_is_visible) )
    {
        Print_in_menu("X:       Y:             ",0);
        Print_coordinates();
    }
    Update_rect(0,0,Screen_width,Menu_Y);
  }
  
  // Push back all arguments
  for (i=0;i<4;i++)
  {
    Operation_push(x[i]);
    Operation_push(y[i]);
  }
  Operation_push(selected_corner);

}
void Distort_brush_0_9(void)
//
//  Op�ration   : OPERATION_DISTORT_BRUSH
//  Click Souris: 0
//  Taille_Pile : 9
//
//  Souris effac�e: No
//
{
  short selected_corner;
  Operation_pop(&selected_corner);
  
}

void Distort_brush_2_0(void)
//
// Op�ration   : OPERATION_DISTORT_BRUSH
// Click Souris: 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
//
{
  Paintbrush_hidden=0;
  Display_all_screen();
  // Erase the message in status bar
  if ( (Config.Coords_rel) && (Menu_is_visible) )
  {
      Print_in_menu("X:       Y:             ",0);
  }
  Return_to_draw_mode();
}

void Distort_brush_2_8(void)
//
// Op�ration   : OPERATION_DISTORT_BRUSH
// Click Souris: 2
// Taille_Pile : 8
//
// Souris effac�e: Oui
//
{
  short i;
  short x[4];
  short y[4];

  // Pop all arguments
  for (i=3;i>=0;i--)
  {
    Operation_pop(&y[i]);
    Operation_pop(&x[i]);
  }
  Distort_brush(x[0],y[0],x[1],y[1],x[2],y[2],x[3],y[3]);
  
  Paintbrush_hidden=0;
  Display_all_screen();
  
  Return_to_draw_mode();
}

//////////////////////////////////////////////////////////// OPERATION_SCROLL


byte Cursor_hidden_before_scroll;

void Scroll_12_0(void)
//
//  Op�ration   : OPERATION_SCROLL
//  Click Souris: 1 ou 2
//  Taille_Pile : 0
//
//  Souris effac�e: Oui
//
{
  Init_start_operation();
  Backup();
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Operation_push(Mouse_K); // LEFT_SIDE or RIGHT_SIDE
  
  Cursor_hidden_before_scroll=Cursor_hidden;
  Cursor_hidden=1;
  if ((Config.Coords_rel) && (Menu_is_visible))
    Print_in_menu("X:�   0   Y:�   0",0);
}


void Scroll_12_5(void)
//
//  Op�ration   : OPERATION_SCROLL
//  Click Souris: 1 ou 2
//  Taille_Pile : 5
//
//  Souris effac�e: Non
//
{
  short center_x;
  short center_y;
  short x_pos;
  short y_pos;
  short x_offset;
  short y_offset;
  short side;
  //char  str[5];

  Operation_pop(&side);
  Operation_pop(&y_pos);
  Operation_pop(&x_pos);
  Operation_pop(&center_y);
  Operation_pop(&center_x);

  if ( (Paintbrush_X!=x_pos) || (Paintbrush_Y!=y_pos) )
  {
    // L'utilisateur a boug�, il faut scroller l'image

    if (Paintbrush_X>=center_x)
      x_offset=(Paintbrush_X-center_x)%Main_image_width;
    else
      x_offset=Main_image_width-((center_x-Paintbrush_X)%Main_image_width);

    if (Paintbrush_Y>=center_y)
      y_offset=(Paintbrush_Y-center_y)%Main_image_height;
    else
      y_offset=Main_image_height-((center_y-Paintbrush_Y)%Main_image_height);

    Display_coords_rel_or_abs(center_x,center_y);

    if (side == RIGHT_SIDE)
    {
      // All layers at once
      Scroll_picture(Screen_backup, Main_screen, x_offset,y_offset);
    }
    else
    {
      // One layer at once
      Scroll_picture(Main_backups->Pages->Next->Image[Main_current_layer], Main_backups->Pages->Image[Main_current_layer], x_offset, y_offset);
      Redraw_layered_image();
    }

    Display_all_screen();
  }

  Operation_push(center_x);
  Operation_push(center_y);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Operation_push(side);
}

void Scroll_0_5(void)
//
//  Op�ration   : OPERATION_SCROLL
//  Click Souris: 0
//  Taille_Pile : 5
//
//  Souris effac�e: Oui
//
{
  // All layers at once
  short center_x;
  short center_y;
  short x_pos;
  short y_pos;
  short x_offset;
  short y_offset;
  short side;
  int i;


  Operation_pop(&side);
  Operation_pop(&y_pos);
  Operation_pop(&x_pos);
  Operation_pop(&center_y);
  Operation_pop(&center_x);
  
  if (side == RIGHT_SIDE)
    {
      // All layers at once
    if (x_pos>=center_x)
      x_offset=(x_pos-center_x)%Main_image_width;
    else
      x_offset=Main_image_width-((center_x-x_pos)%Main_image_width);
  
    if (y_pos>=center_y)
      y_offset=(y_pos-center_y)%Main_image_height;
    else
      y_offset=Main_image_height-((center_y-y_pos)%Main_image_height);
    
    
    // Do the actual scroll operation on all layers.
    for (i=0; i<NB_LAYERS; i++)
      Scroll_picture(Main_backups->Pages->Next->Image[i], Main_backups->Pages->Image[i], x_offset, y_offset);
  }  
  else
  {
    // One layer : everything was done while dragging the mouse
  }
  
  Cursor_hidden=Cursor_hidden_before_scroll;

  End_of_modification();
  if ((Config.Coords_rel) && (Menu_is_visible))
  {
    Print_in_menu("X:       Y:             ",0);
    Print_coordinates();
  }
}


//////////////////////////////////////////////////// OPERATION_GRAD_CIRCLE


void Grad_circle_12_0(void)
//
// Op�ration   : OPERATION_GRAD_CIRCLE
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
{
  byte color;

  Init_start_operation();
  Backup();

  Shade_table=(Mouse_K==LEFT_SIDE)?Shade_table_left:Shade_table_right;
  color=(Mouse_K==LEFT_SIDE)?Fore_color:Back_color;

  Paintbrush_hidden_before_scroll=Paintbrush_hidden;
  Paintbrush_hidden=1;

  Pixel_figure_preview(Paintbrush_X,Paintbrush_Y,color);
  Update_part_of_screen(Paintbrush_X,Paintbrush_Y,1,1);
  
  if ((Config.Coords_rel) && (Menu_is_visible))
    Print_in_menu("Radius:   0    ",0);

  Operation_push(Mouse_K);
  Operation_push(color);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


void Grad_circle_12_6(void)
//
// Op�ration   : OPERATION_GRAD_CIRCLE
// Click Souris: 1 ou 2
// Taille_Pile : 6 (Mouse_K, color, X_Centre, Y_Centre, X_Tangente, Y_Tangente)
//
// Souris effac�e: Non
//
{
  short tangent_x;
  short tangent_y;
  short center_x;
  short center_y;
  short color;
  short radius;
  char  str[5];

  Operation_pop(&tangent_y);
  Operation_pop(&tangent_x);
  Operation_pop(&center_y);
  Operation_pop(&center_x);
  Operation_pop(&color);

  if ( (tangent_x!=Paintbrush_X) || (tangent_y!=Paintbrush_Y) )
  {
    Hide_cursor();
    if ((Config.Coords_rel) && (Menu_is_visible))
    {
      Num2str(Distance(center_x,center_y,Paintbrush_X,Paintbrush_Y),str,4);
      Print_in_menu(str,7);
    }
    else
      Print_coordinates();

    Circle_limit=((tangent_x-center_x)*(tangent_x-center_x))+
                  ((tangent_y-center_y)*(tangent_y-center_y));
    radius=sqrt(Circle_limit);
    Hide_empty_circle_preview(center_x,center_y,radius);

    Circle_limit=((Paintbrush_X-center_x)*(Paintbrush_X-center_x))+
                  ((Paintbrush_Y-center_y)*(Paintbrush_Y-center_y));
    radius=sqrt(Circle_limit);
    Draw_empty_circle_preview(center_x,center_y,radius,color);

    Display_cursor();
  }

  Operation_push(color);
  Operation_push(center_x);
  Operation_push(center_y);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


void Grad_circle_0_6(void)
//
// Op�ration   : OPERATION_GRAD_CIRCLE
// Click Souris: 0
// Taille_Pile : 6 (Mouse_K, color, X_Centre, Y_Centre, X_Tangente, Y_Tangente)
//
// Souris effac�e: Oui
//
{
  short tangent_x;
  short tangent_y;
  short center_x;
  short center_y;
  short color;
  short click;
  short radius;

  Operation_pop(&tangent_y);
  Operation_pop(&tangent_x);
  Operation_pop(&center_y);
  Operation_pop(&center_x);

  Operation_pop(&color);
  Operation_pop(&click);

  if (click==LEFT_SIDE)
  {
    Operation_push(click);
    Operation_push(color);

    Operation_push(center_x);
    Operation_push(center_y);
    Operation_push(tangent_x);
    Operation_push(tangent_y);

    Operation_push(Paintbrush_X);
    Operation_push(Paintbrush_Y);

    // On change la forme du curseur
    Cursor_shape=CURSOR_SHAPE_XOR_TARGET;

    // On affiche une croix XOR au centre du cercle
    Draw_curve_cross(center_x,center_y);

    if (Menu_is_visible)
    {
      if (Config.Coords_rel)
        Print_in_menu("X:        Y:",0);
      else
        Print_in_menu("X:       Y:             ",0);
      Display_coords_rel_or_abs(center_x,center_y);
    }
  }
  else
  {
    Circle_limit=((tangent_x-center_x)*(tangent_x-center_x))+
                  ((tangent_y-center_y)*(tangent_y-center_y));
    radius=sqrt(Circle_limit);
    Hide_empty_circle_preview(center_x,center_y,radius);

    Paintbrush_hidden=Paintbrush_hidden_before_scroll;
    Cursor_shape=CURSOR_SHAPE_TARGET;

    Draw_filled_circle(center_x,center_y,radius,Back_color);

    End_of_modification();
    if ((Config.Coords_rel) && (Menu_is_visible))
    {
      Print_in_menu("X:       Y:             ",0);
      Print_coordinates();
    }
  }
}


void Grad_circle_12_8(void)
//
// Op�ration   : OPERATION_GRAD_CIRCLE
// Click Souris: 0
// Taille_Pile : 8 (Mouse_K, color, X_Centre, Y_Centre, X_Tangente, Y_Tangente, old_x, old_y)
//
// Souris effac�e: Oui
//
{
  short tangent_x;
  short tangent_y;
  short center_x;
  short center_y;
  short color;
  short old_mouse_k;

  short radius;

  Operation_stack_size-=2;   // On fait sauter les 2 derniers �lts de la pile
  Operation_pop(&tangent_y);
  Operation_pop(&tangent_x);
  Operation_pop(&center_y);
  Operation_pop(&center_x);
  Operation_pop(&color);
  Operation_pop(&old_mouse_k);
  
  Hide_cursor();
  // On efface la croix XOR au centre du cercle
  Draw_curve_cross(center_x,center_y);

  Circle_limit=((tangent_x-center_x)*(tangent_x-center_x))+
                ((tangent_y-center_y)*(tangent_y-center_y));
  radius=sqrt(Circle_limit);
  Hide_empty_circle_preview(center_x,center_y,radius);

  Paintbrush_hidden=Paintbrush_hidden_before_scroll;
  Cursor_shape=CURSOR_SHAPE_TARGET;

  if (Mouse_K==old_mouse_k)
    Draw_grad_circle(center_x,center_y,radius,Paintbrush_X,Paintbrush_Y);

  Display_cursor();
  End_of_modification();
  Wait_end_of_click();

  if ((Config.Coords_rel) && (Menu_is_visible))
  {
    Print_in_menu("X:       Y:             ",0);
    Print_coordinates();
  }
}


void Grad_circle_or_ellipse_0_8(void)
//
// Op�ration   : OPERATION_{CERCLE|ELLIPSE}_DEGRADE
// Click Souris: 0
// Taille_Pile : 8
//
// Souris effac�e: Non
//
{
  short start_x;
  short start_y;
  short tangent_x;
  short tangent_y;
  short old_x;
  short old_y;

  Operation_pop(&old_y);
  Operation_pop(&old_x);

  if ((Paintbrush_X!=old_x) || (Paintbrush_Y!=old_y))
  {
    Operation_pop(&tangent_y);
    Operation_pop(&tangent_x);
    Operation_pop(&start_y);
    Operation_pop(&start_x);
    Display_coords_rel_or_abs(start_x,start_y);
    Operation_push(start_x);
    Operation_push(start_y);
    Operation_push(tangent_x);
    Operation_push(tangent_y);
  }

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


////////////////////////////////////////////////// OPERATION_GRAD_ELLIPSE


void Grad_ellipse_12_0(void)
//
// Op�ration   : OPERATION_GRAD_ELLIPSE
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
{
  byte color;

  Init_start_operation();
  Backup();

  Shade_table=(Mouse_K==LEFT_SIDE)?Shade_table_left:Shade_table_right;
  color=(Mouse_K==LEFT_SIDE)?Fore_color:Back_color;

  Paintbrush_hidden_before_scroll=Paintbrush_hidden;
  Paintbrush_hidden=1;

  Pixel_figure_preview(Paintbrush_X,Paintbrush_Y,color);
  Update_part_of_screen(Paintbrush_X,Paintbrush_Y,1,1);
  
  if ((Config.Coords_rel) && (Menu_is_visible))
    Print_in_menu("X:�   0   Y:�   0",0);

  Operation_push(Mouse_K);
  Operation_push(color);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


void Grad_ellipse_12_6(void)
//
// Op�ration   : OPERATION_GRAD_ELLIPSE
// Click Souris: 1 ou 2
// Taille_Pile : 6 (Mouse_K, color, X_Centre, Y_Centre, X_Tangente, Y_Tangente)
//
// Souris effac�e: Non
//
{
  short tangent_x;
  short tangent_y;
  short center_x;
  short center_y;
  short color;
  short horizontal_radius;
  short vertical_radius;

  Operation_pop(&tangent_y);
  Operation_pop(&tangent_x);
  Operation_pop(&center_y);
  Operation_pop(&center_x);
  Operation_pop(&color);

  if ( (tangent_x!=Paintbrush_X) || (tangent_y!=Paintbrush_Y) )
  {
    Hide_cursor();
    Display_coords_rel_or_abs(center_x,center_y);

    horizontal_radius=(tangent_x>center_x)?tangent_x-center_x
                                           :center_x-tangent_x;
    vertical_radius  =(tangent_y>center_y)?tangent_y-center_y
                                           :center_y-tangent_y;
    Hide_empty_ellipse_preview(center_x,center_y,horizontal_radius,vertical_radius);

    horizontal_radius=(Paintbrush_X>center_x)?Paintbrush_X-center_x
                                         :center_x-Paintbrush_X;
    vertical_radius  =(Paintbrush_Y>center_y)?Paintbrush_Y-center_y
                                         :center_y-Paintbrush_Y;
    Draw_empty_ellipse_preview(center_x,center_y,horizontal_radius,vertical_radius,color);

    Display_cursor();
  }

  Operation_push(color);
  Operation_push(center_x);
  Operation_push(center_y);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


void Grad_ellipse_0_6(void)
//
// Op�ration   : OPERATION_GRAD_ELLIPSE
// Click Souris: 0
// Taille_Pile : 6 (Mouse_K, color, X_Centre, Y_Centre, X_Tangente, Y_Tangente)
//
// Souris effac�e: Oui
//
{
  short tangent_x;
  short tangent_y;
  short center_x;
  short center_y;
  short color;
  short click;
  //short radius;
  short horizontal_radius;
  short vertical_radius;

  Operation_pop(&tangent_y);
  Operation_pop(&tangent_x);
  Operation_pop(&center_y);
  Operation_pop(&center_x);

  Operation_pop(&color);
  Operation_pop(&click);

  if (click==LEFT_SIDE)
  {
    Operation_push(click);
    Operation_push(color);

    Operation_push(center_x);
    Operation_push(center_y);
    Operation_push(tangent_x);
    Operation_push(tangent_y);

    Operation_push(Paintbrush_X);
    Operation_push(Paintbrush_Y);

    // On change la forme du curseur
    Cursor_shape=CURSOR_SHAPE_XOR_TARGET;

    // On affiche une croix XOR au centre du cercle
    Draw_curve_cross(center_x,center_y);

    if (Menu_is_visible)
    {
      if (Config.Coords_rel)
        Print_in_menu("X:        Y:",0);
      else
        Print_in_menu("X:       Y:             ",0);
      Display_coords_rel_or_abs(center_x,center_y);
    }
  }
  else
  {
    horizontal_radius=(tangent_x>center_x)?tangent_x-center_x
                                           :center_x-tangent_x;
    vertical_radius  =(tangent_y>center_y)?tangent_y-center_y
                                           :center_y-tangent_y;
    Hide_empty_ellipse_preview(center_x,center_y,horizontal_radius,vertical_radius);

    Paintbrush_hidden=Paintbrush_hidden_before_scroll;
    Cursor_shape=CURSOR_SHAPE_TARGET;

    Draw_filled_ellipse(center_x,center_y,horizontal_radius,vertical_radius,Back_color);

    End_of_modification();
    if ((Config.Coords_rel) && (Menu_is_visible))
    {
      Print_in_menu("X:       Y:             ",0);
      Print_coordinates();
    }
  }
}


void Grad_ellipse_12_8(void)
//
// Op�ration   : OPERATION_GRAD_ELLIPSE
// Click Souris: 0
// Taille_Pile : 8 (Mouse_K, color, X_Centre, Y_Centre, X_Tangente, Y_Tangente, old_x, old_y)
//
// Souris effac�e: Oui
//
{
  short tangent_x;
  short tangent_y;
  short center_x;
  short center_y;
  short color;
  short horizontal_radius;
  short vertical_radius;
  short old_mouse_k;

  Operation_stack_size-=2;   // On fait sauter les 2 derniers �lts de la pile
  Operation_pop(&tangent_y);
  Operation_pop(&tangent_x);
  Operation_pop(&center_y);
  Operation_pop(&center_x);
  Operation_pop(&color);
  Operation_pop(&old_mouse_k);

  Hide_cursor();
  // On efface la croix XOR au centre de l'ellipse
  Draw_curve_cross(center_x,center_y);

  horizontal_radius=(tangent_x>center_x)?tangent_x-center_x
                                         :center_x-tangent_x;
  vertical_radius  =(tangent_y>center_y)?tangent_y-center_y
                                         :center_y-tangent_y;
  Hide_empty_ellipse_preview(center_x,center_y,horizontal_radius,vertical_radius);

  Paintbrush_hidden=Paintbrush_hidden_before_scroll;
  Cursor_shape=CURSOR_SHAPE_TARGET;

  if (Mouse_K==old_mouse_k)
    Draw_grad_ellipse(center_x,center_y,horizontal_radius,vertical_radius,Paintbrush_X,Paintbrush_Y);

  Display_cursor();
  End_of_modification();
  Wait_end_of_click();

  if ((Config.Coords_rel) && (Menu_is_visible))
  {
    Print_in_menu("X:       Y:             ",0);
    Print_coordinates();
  }
}

/******************************
* Operation_Rectangle_Degrade *
******************************/

// 1) trac� d'un rectangle classique avec les lignes XOR
// 2) trac� d'une ligne vecteur de d�grad�, comme une ligne normale
// 3) dessin du d�grad�


void Grad_rectangle_12_0(void)
// Op�ration   : OPERATION_GRAD_RECTANGLE
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui

// Initialisation de l'�tape 1, on commence � dessiner le rectangle
{
  Init_start_operation();
  Backup();

  if ((Config.Coords_rel) && (Menu_is_visible))
    Print_in_menu("\035:   1   \022:   1",0);
  // On laisse une trace du curseur � l'�cran
  Display_cursor();

  if (Mouse_K==LEFT_SIDE)
  {
    Shade_table=Shade_table_left;
    Operation_push(Mouse_K);
  }
  else
  {
    Shade_table=Shade_table_right;
    Operation_push(Mouse_K);
  }

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}


void Grad_rectangle_12_5(void)
// Op�ration   : OPERATION_GRAD_RECTANGLE
// Click Souris: 1 ou 2
// Taille_Pile : 5
//
// Souris effac�e: Non

// Modification de la taille du rectangle
{
  short start_x;
  short start_y;
  short old_x;
  short old_y;
  char  str[5];

  Operation_pop(&old_y);
  Operation_pop(&old_x);

  if ((Paintbrush_X!=old_x) || (Paintbrush_Y!=old_y))
  {
    Operation_pop(&start_y);
    Operation_pop(&start_x);

    if ((Config.Coords_rel) && (Menu_is_visible))
    {
      Num2str(((start_x<Paintbrush_X)?Paintbrush_X-start_x:start_x-Paintbrush_X)+1,str,4);
      Print_in_menu(str,2);
      Num2str(((start_y<Paintbrush_Y)?Paintbrush_Y-start_y:start_y-Paintbrush_Y)+1,str,4);
      Print_in_menu(str,11);
    }
    else
      Print_coordinates();

    Operation_push(start_x);
    Operation_push(start_y);
  }

  Operation_push(Paintbrush_X);
  Operation_push(Paintbrush_Y);
}

void Grad_rectangle_0_5(void)
// OPERATION_GRAD_RECTANGLE
// click souris 0
// Taile pile : 5
//
// Souris effac�e : non

// Le rectangle est en place, maintenant il faut tracer le vecteur de d�grad�,
// on doit donc attendre que l'utilisateur clique quelque part
// On stocke tout de suite les coordonn�es du pinceau comme �a on change d'�tat et on passe � la suite
{
  // !!! Cette fonction remet rax ray rbx rby dans la pile � la fin donc il ne faut pas les modifier ! (sauf �ventuellement un tri)
  short rax;
  short ray;
  short rbx;
  short rby, width,height;
  short offset_width = 0;
  short offset_height = 0;
  short offset_left = 0;
  short offset_top = 0;


  // Trac� propre du rectangle
  Operation_pop(&rby);
  Operation_pop(&rbx);
  Operation_pop(&ray);
  Operation_pop(&rax);

  Paintbrush_X = rax;
  Paintbrush_Y = ray;
  Hide_cursor();

  width = abs(rbx - rax);
  height = abs(rby - ray);

  // Check if the rectangle is not fully outside the picture
  if (Min(rax, rbx) > Main_image_width || Min(ray, rby) > Main_image_height)
  {
	Operation_pop(&rby); // reset the stack
	return; // cancel the operation
  }

	// Handle clipping
	if (Max(rax, rbx)-Main_offset_X > Min(Main_image_width,
		Main_magnifier_mode?Main_separator_position:Screen_width))
	{
		offset_width = Max(rax, rbx) - Min(Main_image_width,
			Main_magnifier_mode?Main_separator_position:Screen_width);
	}

	if (Max(ray, rby)-Main_offset_Y > Min(Main_image_height, Menu_Y))
		offset_height = Max(ray, rby) - Min(Main_image_height, Menu_Y);

	// Dessin dans la zone de dessin normale
	Horizontal_XOR_line(Min(rax, rbx)-Main_offset_X,
		Min(ray, rby) - Main_offset_Y, width - offset_width);

	// If not, this line is out of the picture so there is no need to draw it
	if (offset_height == 0)
	{
		Horizontal_XOR_line(Min(rax, rbx) - Main_offset_X, Max(ray, rby) - 1
			- Main_offset_Y, width - offset_width);
	}

	Vertical_XOR_line(Min(rax, rbx)-Main_offset_X, Min(ray, rby)
		- Main_offset_Y, height - offset_height);

	if (offset_width == 0)
	{
		Vertical_XOR_line(Max(rax, rbx) - 1 - Main_offset_X, Min(ray, rby)
			- Main_offset_Y, height - offset_height);
	}

	Update_rect(Min(rax, rbx) - Main_offset_X, Min(ray, rby) - Main_offset_Y,
		width + 1 - offset_width, height + 1 - offset_height);

  // Dessin dans la zone zoom�e
  if (Main_magnifier_mode && Min(rax, rbx) <= Limit_right_zoom
	&& Max(rax, rbx) > Limit_left_zoom
	&& Min(ray, rby) <= Limit_bottom_zoom
	&& Max(ray, rby) > Limit_top_zoom )
  {
	offset_width = 0;
	offset_height = 0;

	if (Min(rax, rbx)<=Limit_left_zoom) // On d�passe du zoom � gauche
	{
		offset_width += Limit_left_zoom - Min(rax, rbx);
		offset_left = Limit_left_zoom;
	}

    if(Max(rax,rbx)>Limit_right_zoom) // On d�passe du zoom � droite
        offset_width += Max(rax,rbx) - Limit_right_zoom - 1;

    if(Min(ray,rby)<Limit_top_zoom) // On d�passe du zoom en haut
    {
        offset_height += Limit_top_zoom - Min(ray,rby);
        offset_top = Limit_top_zoom;
    }

    if(Max(ray,rby)>Limit_bottom_zoom) // On d�passe du zoom en bas
        offset_height += Max(ray,rby) - Limit_bottom_zoom - 1;

    if(width > offset_width)
    {
      if(offset_top==0) // La ligne du haut est visible
        Horizontal_XOR_line_zoom(offset_left>0?offset_left:Min(rax,rbx),Min(ray,rby),width-offset_width);

      if(Max(ray,rby)<=Limit_bottom_zoom) // La  ligne du bas est visible
        Horizontal_XOR_line_zoom(offset_left>0?offset_left:Min(rax,rbx),Max(ray,rby),width-offset_width);
    }

    if(height>offset_height)
    {
      if(offset_left==0) // La ligne de gauche est visible
        Vertical_XOR_line_zoom(Min(rax,rbx),offset_top>0?offset_top:Min(ray,rby),height-offset_height);

      if(Max(rax,rbx)<=Limit_right_zoom) // La ligne de droite est visible
        Vertical_XOR_line_zoom(Max(rax,rbx),offset_top>0?offset_top:Min(ray,rby),height-offset_height);
    }
  }

  Operation_push(rax);
  Operation_push(ray);
  Operation_push(rbx);
  Operation_push(rby);

  // On ajoute des trucs dans la pile pour forcer le passage � l'�tape suivante
  Operation_push(rbx);
  Operation_push(rby);
}

void Grad_rectangle_0_7(void)
// OPERATION_GRAD_RECTANGLE
// click souris 0
// Taile pile : 5
//
// Souris effac�e : non

// On continue � attendre que l'utilisateur clique en gardant les coords � jour
{
    Operation_stack_size -= 2;
    Print_coordinates();
    Operation_push(Paintbrush_X);
    Operation_push(Paintbrush_Y);
}

void Grad_rectangle_12_7(void)
// Op�ration   : OPERATION_GRAD_RECTANGLE
// Click Souris: 1 ou 2
// Taille_Pile : 7
//
//  Souris effac�e: Oui

//  D�but du trac� du vecteur (premier clic)
// On garde les anciennes coordonn�es dans la pile, et on ajoute les nouvelles par dessus

// Si l'utilisateur utilise le mauvais bouton, on annule le trac�. Mais �a nous oblige � vider toute la pile pour v�rifier :(
{
  short rax,rbx,ray,rby,vax,vay,click;

  Operation_pop(&vay);
  Operation_pop(&vax);
  Operation_pop(&ray);
  Operation_pop(&rax);
  Operation_pop(&rby);
  Operation_pop(&rbx);
  Operation_pop(&click);


  if(click==Mouse_K)
  {
      Operation_push(click);
      Operation_push(rbx);
      Operation_push(rby);
      Operation_push(rax);
      Operation_push(ray);
      Operation_push(vax);
      Operation_push(vay);
      Operation_push(Paintbrush_X);
      Operation_push(Paintbrush_Y);

  }
  else
  {
      // Mauvais bouton > anulation de l'op�ration.
      // On a d�j� vid� la pile, il reste � effacer le rectangle XOR
      short width, height;
      short offset_width = 0;
      short offset_height = 0;
      short offset_left = 0;
      short offset_top = 0;

      width = abs(rbx-rax);
      height = abs(rby-ray);

      if (Max(rax,rbx)-Main_offset_X > Min(Main_image_width,Main_magnifier_mode?Main_separator_position:Screen_width)) // Tous les clippings � g�rer sont l�
          offset_width = Max(rax,rbx) - Min(Main_image_width,Main_magnifier_mode?Main_separator_position:Screen_width);

      if (Max(ray,rby)-Main_offset_Y > Min(Main_image_height,Menu_Y))
          offset_height = Max(ray,rby) - Min(Main_image_height,Menu_Y);

      // Dessin dans la zone de dessin normale
      Horizontal_XOR_line(Min(rax,rbx)-Main_offset_X,Min(ray,rby)-Main_offset_Y,width - offset_width);
      if(offset_height == 0)
          Horizontal_XOR_line(Min(rax,rbx)-Main_offset_X,Max(ray,rby)-1-Main_offset_Y,width - offset_width);

      Vertical_XOR_line(Min(rax,rbx)-Main_offset_X,Min(ray,rby)-Main_offset_Y,height-offset_height);
      if (offset_width == 0) // Sinon cette ligne est en dehors de la zone image, inutile de la dessiner
          Vertical_XOR_line(Max(rax,rbx)-1-Main_offset_X,Min(ray,rby)-Main_offset_Y,height-offset_height);

      Update_rect(Min(rax,rbx)-Main_offset_X,Min(ray,rby)-Main_offset_Y,width+1-offset_width,height+1-offset_height);

      // Dessin dans la zone zoom�e
      if (Main_magnifier_mode && Min(rax, rbx) <= Limit_right_zoom
		&& Max(rax, rbx)>Limit_left_zoom && Min(ray, rby) <= Limit_bottom_zoom
		&& Max(ray,rby)>Limit_top_zoom )
      {
          offset_width = 0;
          offset_height=0;

          if(Min(rax,rbx)<Limit_left_zoom) // On d�passe du zoom � gauche
          {
              offset_width += Limit_left_zoom - Min(rax,rbx);
              offset_left = Limit_left_zoom;
          }

          if(Max(rax,rbx)>Limit_right_zoom) // On d�passe du zoom � droite
              offset_width += Max(rax,rbx) - Limit_right_zoom - 1;

          if(Min(ray,rby)<Limit_top_zoom) // On d�passe du zoom en haut
          {
              offset_height += Limit_top_zoom - Min(ray,rby);
              offset_top = Limit_top_zoom;
          }

          if(Max(ray,rby)>Limit_bottom_zoom) // On d�passe du zoom en bas
              offset_height += Max(ray,rby) - Limit_bottom_zoom - 1;

          if(width > offset_width)
          {
              if(offset_top==0) // La ligne du haut est visible
                  Horizontal_XOR_line_zoom(offset_left>0?offset_left:Min(rax,rbx),Min(ray,rby),width-offset_width);

              if(Max(ray,rby)<Limit_bottom_zoom) // La ligne du bas est visible
                  Horizontal_XOR_line_zoom(offset_left>0?offset_left:Min(rax,rbx),Max(ray,rby),width-offset_width);
          }

          if(height>offset_height)
          {
              if(offset_left==0) // La ligne de gauche est visible
                  Vertical_XOR_line_zoom(Min(rax,rbx),offset_top>0?offset_top:Min(ray,rby),height-offset_height);

              if(Max(rax,rbx)<=Limit_right_zoom) // La ligne de droite est visible
                  Vertical_XOR_line_zoom(Max(rax,rbx),offset_top>0?offset_top:Min(ray,rby),height-offset_height);
          }
      }
  }
}

void Grad_rectangle_12_9(void)
    // Op�ration   : OPERATION_GRAD_RECTANGLE
    // Click Souris: 1
    // Taille_Pile : 5
    //
    // Souris effac�e: Oui

    // Poursuite du trac� du vecteur (d�placement de la souris en gardant le curseur appuy�)
{
    short start_x;
    short start_y;
    short end_x;
    short end_y;
    short cursor_x;
    short cursor_y;

    Operation_pop(&end_y);
    Operation_pop(&end_x);
    Operation_pop(&start_y);
    Operation_pop(&start_x);

    cursor_x = Paintbrush_X;
    cursor_y = Paintbrush_Y;
    // On corrige les coordonn�es de la ligne si la touche shift est appuy�e...
    if(SDL_GetModState() & KMOD_SHIFT)
        Clamp_coordinates_regular_angle(start_x,start_y,&cursor_x,&cursor_y);
        
    if ((cursor_x!=end_x) || (cursor_y!=end_y))
    {
        Display_coords_rel_or_abs(start_x,start_y);

        Draw_line_preview_xor(start_x,start_y,end_x,end_y,0);
        Draw_line_preview_xor(start_x,start_y,cursor_x,cursor_y,0);

    }


    Operation_push(start_x);
    Operation_push(start_y);
    Operation_push(cursor_x);
    Operation_push(cursor_y);
}

void Grad_rectangle_0_9(void)
    // Op�ration   : OPERATION_GRAD_RECTANGLE
    // Click Souris: 0
    // Taille_Pile : 9
    //
    //  Souris effac�e: Oui

    // Ouf, fini ! on dessine enfin le rectangle avec son d�grad�
{
    short rect_start_x;
    short rect_start_y;
    short rect_end_x;
    short rect_end_y;

    short vector_start_x;
    short vector_start_y;
    short vector_end_x;
    short vector_end_y;

    Operation_pop(&vector_end_y);
    Operation_pop(&vector_end_x);
    Operation_pop(&vector_start_y);
    Operation_pop(&vector_start_x);
    Operation_pop(&rect_end_y);
    Operation_pop(&rect_end_x);
    Operation_pop(&rect_start_y);
    Operation_pop(&rect_start_x);
    Operation_stack_size--;

    Hide_cursor();
    // Maintenant on efface tout le bazar temporaire : rectangle et ligne XOR
    Hide_line_preview(vector_start_x,vector_start_y,vector_end_x,vector_end_y);

    // Et enfin on trace le rectangle avec le d�grad� dedans !
    if (vector_end_x==vector_start_x && vector_end_y==vector_start_y)
    {
        // Vecteur nul > pas de rectangle trac�
        // Du coup on doit effacer la preview xor ...
        short width, height;
        short offset_width = 0;
        short offset_height = 0;
        short offset_left = 0;
        short offset_top = 0;

        width = abs(rect_end_x-rect_start_x);
        height = abs(rect_end_y-rect_start_y);

        if (Max(rect_start_x,rect_end_x)-Main_offset_X > Min(Main_image_width,Main_magnifier_mode?Main_separator_position:Screen_width)) // Tous les clippings � g�rer sont l�
            offset_width = Max(rect_start_x,rect_end_x) - Min(Main_image_width,Main_magnifier_mode?Main_separator_position:Screen_width);

        if (Max(rect_start_y,rect_end_y)-Main_offset_Y > Min(Main_image_height,Menu_Y))
            offset_height = Max(rect_start_y,rect_end_y) - Min(Main_image_height,Menu_Y);

        // Dessin dans la zone de dessin normale
        Horizontal_XOR_line(Min(rect_start_x,rect_end_x)-Main_offset_X,Min(rect_start_y,rect_end_y)-Main_offset_Y,width - offset_width);
        if(offset_height == 0)
            Horizontal_XOR_line(Min(rect_start_x,rect_end_x)-Main_offset_X,Max(rect_start_y,rect_end_y)-1-Main_offset_Y,width - offset_width);

        Vertical_XOR_line(Min(rect_start_x,rect_end_x)-Main_offset_X,Min(rect_start_y,rect_end_y)-Main_offset_Y,height-offset_height);
        if (offset_width == 0) // Sinon cette ligne est en dehors de la zone image, inutile de la dessiner
            Vertical_XOR_line(Max(rect_start_x,rect_end_x)-1-Main_offset_X,Min(rect_start_y,rect_end_y)-Main_offset_Y,height-offset_height);

        Update_rect(Min(rect_start_x,rect_end_x)-Main_offset_X,Min(rect_start_y,rect_end_y)-Main_offset_Y,width+1-offset_width,height+1-offset_height);

        // Dessin dans la zone zoom�e
        if (Main_magnifier_mode
			&& Min(rect_start_x, rect_end_x) <= Limit_right_zoom
			&& Max(rect_start_x, rect_end_x) > Limit_left_zoom
			&& Min(rect_start_y, rect_end_y) <= Limit_bottom_zoom
			&& Max(rect_start_y, rect_end_y) > Limit_top_zoom )
        {
            offset_width = 0;
            offset_height=0;

            if(Min(rect_start_x,rect_end_x)<Limit_left_zoom) // On d�passe du zoom � gauche
            {
                offset_width += Limit_left_zoom - Min(rect_start_x,rect_end_x);
                offset_left = Limit_left_zoom;
            }

            if(Max(rect_start_x,rect_end_x)>Limit_right_zoom) // On d�passe du zoom � droite
                offset_width += Max(rect_start_x,rect_end_x) - Limit_right_zoom;

            if(Min(rect_start_y,rect_end_y)<Limit_top_zoom) // On d�passe du zoom en haut
            {
                offset_height += Limit_top_zoom - Min(rect_start_y,rect_end_y);
                offset_top = Limit_top_zoom;
            }

            if(Max(rect_start_y,rect_end_y)>Limit_bottom_zoom) // On d�passe du zoom en bas
                offset_height += Max(rect_start_y,rect_end_y) - Limit_bottom_zoom;

            if(width > offset_width)
            {
                if(offset_top==0) // La ligne du haut est visible
                    Horizontal_XOR_line_zoom(offset_left>0?offset_left:Min(rect_start_x,rect_end_x),Min(rect_start_y,rect_end_y),width-offset_width);

                if(Max(rect_start_y,rect_end_y) <= Limit_bottom_zoom) // La ligne du bas est visible
                    Horizontal_XOR_line_zoom(offset_left>0?offset_left:Min(rect_start_x,rect_end_x),Max(rect_start_y,rect_end_y),width-offset_width);
            }

            if(height>offset_height)
            {
                if(offset_left==0) // La ligne de gauche est visible
                    Vertical_XOR_line_zoom(Min(rect_start_x,rect_end_x),offset_top>0?offset_top:Min(rect_start_y,rect_end_y),height-offset_height);

                if(Max(rect_start_x,rect_end_x) <= Limit_right_zoom) // La ligne de droite est visible
                    Vertical_XOR_line_zoom(Max(rect_start_x,rect_end_x),offset_top>0?offset_top:Min(rect_start_y,rect_end_y),height-offset_height);
            }
        }
    }
    else
        Draw_grad_rectangle(rect_start_x,rect_start_y,rect_end_x,rect_end_y,vector_start_x,vector_start_y,vector_end_x,vector_end_y);

    Display_cursor();
    End_of_modification();
    Wait_end_of_click();

    if ((Config.Coords_rel) && (Menu_is_visible))
    {
        Print_in_menu("X:       Y:             ",0);
        Print_coordinates();
    }
}
/////////////////////////////////////////////////// OPERATION_CENTERED_LINES


void Centered_lines_12_0(void)
    // Op�ration   : OPERATION_CENTERED_LINES
    // Click Souris: 1 ou 2
    // Taille_Pile : 0
    //
    //  Souris effac�e: Oui
{
    Init_start_operation();
    Backup();
    Shade_table=(Mouse_K==LEFT_SIDE)?Shade_table_left:Shade_table_right;

    if ((Config.Coords_rel) && (Menu_is_visible))
        Print_in_menu("X:�   0   Y:�   0",0);

    Operation_push(Mouse_K);
    Operation_push(Paintbrush_X);
    Operation_push(Paintbrush_Y);
}


void Centered_lines_12_3(void)
    // Op�ration   : OPERATION_CENTERED_LINES
    // Click Souris: 1 ou 2
    // Taille_Pile : 3
    //
    // Souris effac�e: Non
{
    short start_x;
    short start_y;

    Operation_pop(&start_y);
    Operation_pop(&start_x);
    Operation_push(Paintbrush_X);
    Operation_push(Paintbrush_Y);
}


void Centered_lines_0_3(void)
    // Op�ration   : OPERATION_CENTERED_LINES
    // Click Souris: 0
    // Taille_Pile : 3
    //
    // Souris effac�e: Oui
{
    short start_x;
    short start_y;
    short Button;
    short color;

    Operation_pop(&start_y);
    Operation_pop(&start_x);
    Operation_pop(&Button);

    color=(Button==LEFT_SIDE)?Fore_color:Back_color;

    Pixel_figure_preview(Paintbrush_X,Paintbrush_Y,color);
    Paintbrush_shape_before_operation=Paintbrush_shape;
    Paintbrush_shape=PAINTBRUSH_SHAPE_POINT;

    Operation_push(Button);
    Operation_push(Paintbrush_X); // Nouveau d�but X
    Operation_push(Paintbrush_Y); // Nouveau d�but Y
    Operation_push(Paintbrush_X); // Nouvelle derni�re fin X
    Operation_push(Paintbrush_Y); // Nouvelle derni�re fin Y
    Operation_push(Paintbrush_X); // Nouvelle derni�re position X
    Operation_push(Paintbrush_Y); // Nouvelle derni�re position Y
}


void Centered_lines_12_7(void)
    // Op�ration   : OPERATION_CENTERED_LINES
    // Click Souris: 1 ou 2
    // Taille_Pile : 7
    //
    // Souris effac�e: Non
{
    short Button;
    short start_x;
    short start_y;
    short end_x;
    short end_y;
    short last_x;
    short last_y;
    short color;

    Operation_pop(&last_y);
    Operation_pop(&last_x);
    Operation_pop(&end_y);
    Operation_pop(&end_x);
    Operation_pop(&start_y);
    Operation_pop(&start_x);
    Operation_pop(&Button);

    if (Mouse_K==Button)
    {
        if ( (end_x!=Paintbrush_X) || (end_y!=Paintbrush_Y) ||
                (last_x!=Paintbrush_X) || (last_y!=Paintbrush_Y) )
        {
            Hide_cursor();

            color=(Button==LEFT_SIDE)?Fore_color:Back_color;

            Paintbrush_shape=Paintbrush_shape_before_operation;

            Pixel_figure_preview_auto  (start_x,start_y);
            Hide_line_preview (start_x,start_y,last_x,last_y);

            Smear_start=1;
            Display_paintbrush      (start_x,start_y,color,0);
            Draw_line_permanent(start_x,start_y,Paintbrush_X,Paintbrush_Y,color);

            Paintbrush_shape=PAINTBRUSH_SHAPE_POINT;
            Pixel_figure_preview(Paintbrush_X,Paintbrush_Y,color);
            Draw_line_preview(start_x,start_y,Paintbrush_X,Paintbrush_Y,color);

            Display_cursor();
        }

        Operation_push(Button);
        Operation_push(start_x);
        Operation_push(start_y);
        Operation_push(Paintbrush_X);
        Operation_push(Paintbrush_Y);
        Operation_push(Paintbrush_X);
        Operation_push(Paintbrush_Y);
    }
    else
    {
        Hide_cursor();

        Paintbrush_shape=Paintbrush_shape_before_operation;

        Pixel_figure_preview_auto  (start_x,start_y);
        Hide_line_preview (start_x,start_y,last_x,last_y);

        if ( (Config.Coords_rel) && (Menu_is_visible) )
        {
            Print_in_menu("X:       Y:             ",0);
            Print_coordinates();
        }

        Display_cursor();
        End_of_modification();
        Wait_end_of_click();
    }
}


void Centered_lines_0_7(void)
    // Op�ration   : OPERATION_CENTERED_LINES
    // Click Souris: 0
    // Taille_Pile : 7
    //
    // Souris effac�e: Non
{
    short Button;
    short start_x;
    short start_y;
    short end_x;
    short end_y;
    short last_x;
    short last_y;
    short color;

    Operation_pop(&last_y);
    Operation_pop(&last_x);
    Operation_pop(&end_y);
    Operation_pop(&end_x);

    if ((Paintbrush_X!=last_x) || (Paintbrush_Y!=last_y))
    {
        Hide_cursor();
        Operation_pop(&start_y);
        Operation_pop(&start_x);
        Operation_pop(&Button);

        color=(Button==LEFT_SIDE)?Fore_color:Back_color;

        Display_coords_rel_or_abs(start_x,start_y);

        Hide_line_preview(start_x,start_y,last_x,last_y);

        Pixel_figure_preview(start_x,start_y,color);
        Draw_line_preview(start_x,start_y,Paintbrush_X,Paintbrush_Y,color);

        Operation_push(Button);
        Operation_push(start_x);
        Operation_push(start_y);
        Display_cursor();
    }

    Operation_push(end_x);
    Operation_push(end_y);
    Operation_push(Paintbrush_X);
    Operation_push(Paintbrush_Y);
}


