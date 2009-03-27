/*  Grafx2 - The Ultimate 256-color bitmap paint program

    Copyright 2008 Yves Rizoud
    Copyright 2008 Franck Charlet
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

#include <string.h>
#include <stdlib.h>
#include <SDL.h>
#include "global.h"
#include "sdlscreen.h"
#include "divers.h"
#include "pxsimple.h"

void Pixel_tall (word x,word y,byte color)
/* Affiche un pixel de la color aux coords x;y � l'�cran */
{
  *(Screen + x + y * 2 * Screen_width)=color;
  *(Screen + x + (y * 2 + 1) * Screen_width)=color;
}

byte Read_pixel_tall (word x,word y)
/* On retourne la couleur du pixel aux coords donn�es */
{
  return *( Screen + y * 2 * Screen_width + x );
}

void Block_tall (word start_x,word start_y,word width,word height,byte color)
/* On affiche un rectangle de la couleur donn�e */
{
  SDL_Rect rectangle;
  rectangle.x=start_x;
  rectangle.y=start_y*2;
  rectangle.w=width;
  rectangle.h=height*2;
  SDL_FillRect(Screen_SDL,&rectangle,color);
}

void Display_part_of_screen_tall (word width,word height,word image_width)
/* Afficher une partie de l'image telle quelle sur l'�cran */
{
  byte* dest=Screen; //On va se mettre en 0,0 dans l'�cran (dest)
  byte* src=Main_offset_Y*image_width+Main_offset_X+Main_screen; //Coords de d�part ds la source (src)
  int y;

  for(y=height;y!=0;y--)
  // Pour chaque ligne
  {
    // On fait une copie de la ligne
    memcpy(dest,src,width);
    dest+=Screen_width;
    memcpy(dest,src,width);

    // On passe � la ligne suivante
    src+=image_width;
    dest+=Screen_width;
  }
  //Update_rect(0,0,width,height);
}

void Pixel_preview_normal_tall (word x,word y,byte color)
/* Affichage d'un pixel dans l'�cran, par rapport au d�calage de l'image 
 * dans l'�cran, en mode normal (pas en mode loupe)
 * Note: si on modifie cette proc�dure, il faudra penser � faire �galement 
 * la modif dans la proc�dure Pixel_Preview_Loupe_SDL. */
{
//  if(x-Main_offset_X >= 0 && y - Main_offset_Y >= 0)
  Pixel_tall(x-Main_offset_X,y-Main_offset_Y,color);
}

void Pixel_preview_magnifier_tall  (word x,word y,byte color)
{
  // Affiche le pixel dans la partie non zoom�e
  Pixel_tall(x-Main_offset_X,y-Main_offset_Y,color);
  
  // Regarde si on doit aussi l'afficher dans la partie zoom�e
  if (y >= Limit_top_zoom && y <= Limit_visible_bottom_zoom
          && x >= Limit_left_zoom && x <= Limit_visible_right_zoom)
  {
    // On est dedans
    int height;
    int y_zoom = Zoom_factor_table[y-Main_magnifier_offset_Y];

    if (Menu_Y - y_zoom < Main_magnifier_factor)
      // On ne doit dessiner qu'un morceau du pixel
      // sinon on d�passe sur le menu
      height = Menu_Y - y_zoom;
    else
      height = Main_magnifier_factor;

    Block_tall(
      Zoom_factor_table[x-Main_magnifier_offset_X]+Main_X_zoom, 
      y_zoom, Main_magnifier_factor, height, color
      );
  }
}

void Horizontal_XOR_line_tall(word x_pos,word y_pos,word width)
{
  //On calcule la valeur initiale de dest:
  byte* dest=y_pos*2*Screen_width+x_pos+Screen;

  int x;

  for (x=0;x<width;x++)
    *(dest+x)=~*(dest+x);

  dest=(y_pos*2+1)*Screen_width+x_pos+Screen;
  for (x=0;x<width;x++)
    *(dest+x)=~*(dest+x);
}

void Vertical_XOR_line_tall(word x_pos,word y_pos,word height)
{
  int i;
  byte color;
  for (i=y_pos*2;i<(y_pos+height)*2;i++)
  {
    color=*(Screen+x_pos+i*Screen_width);
    *(Screen+x_pos+i*Screen_width)=~color;
  }
}

void Display_brush_color_tall(word x_pos,word y_pos,word x_offset,word y_offset,word width,word height,byte transp_color,word brush_width)
{
  // dest = Position � l'�cran
  byte* dest = Screen + y_pos * 2 * Screen_width + x_pos;
  // src = Position dans la brosse
  byte* src = Brush + y_offset * brush_width + x_offset;

  word x,y;

  // Pour chaque ligne
  for(y = height;y > 0; y--)
  {
    // Pour chaque pixel
    for(x = width;x > 0; x--)
    {
      // On v�rifie que ce n'est pas la transparence
      if(*src != transp_color)
      {
        *dest = *src;
        *(dest+Screen_width) = *src;
      }

      // Pixel suivant
      src++; dest++;
    }

    // On passe � la ligne suivante
    dest = dest + 2 * Screen_width - width;
    src = src + brush_width - width;
  }
  Update_rect(x_pos,y_pos,width,height);
}

void Display_brush_mono_tall(word x_pos, word y_pos,
        word x_offset, word y_offset, word width, word height,
        byte transp_color, byte color, word brush_width)
/* On affiche la brosse en monochrome */
{
  byte* dest=y_pos*2*Screen_width+x_pos+Screen; // dest = adr Destination � 
      // l'�cran
  byte* src=brush_width*y_offset+x_offset+Brush; // src = adr ds 
      // la brosse
  int x,y;

  for(y=height;y!=0;y--)
  //Pour chaque ligne
  {
    for(x=width;x!=0;x--)
    //Pour chaque pixel
    {
      if (*src!=transp_color)
      {
        *dest=color;
        *(dest+Screen_width)=color;
      }

      // On passe au pixel suivant
      src++;
      dest++;
    }

    // On passe � la ligne suivante
    src+=brush_width-width;
    dest+=2*Screen_width-width;
  }
  Update_rect(x_pos,y_pos,width,height);
}

void Clear_brush_tall(word x_pos,word y_pos,__attribute__((unused)) word x_offset,__attribute__((unused)) word y_offset,word width,word height,__attribute__((unused))byte transp_color,word image_width)
{
  byte* dest=Screen+x_pos+y_pos*2*Screen_width; //On va se mettre en 0,0 dans l'�cran (dest)
  byte* src = ( y_pos + Main_offset_Y ) * image_width + x_pos + Main_offset_X + Main_screen; //Coords de d�part ds la source (src)
  int y;

  for(y=height;y!=0;y--)
  // Pour chaque ligne
  {
    // On fait une copie de la ligne
    memcpy(dest,src,width);
    dest+=Screen_width;
    memcpy(dest,src,width);

    // On passe � la ligne suivante
    src+=image_width;
    dest+=Screen_width;
  }
  Update_rect(x_pos,y_pos,width,height);
}

// Affiche une brosse (arbitraire) � l'�cran
void Display_brush_tall(byte * brush, word x_pos,word y_pos,word x_offset,word y_offset,word width,word height,byte transp_color,word brush_width)
{
  // dest = Position � l'�cran
  byte* dest = Screen + y_pos * 2 * Screen_width + x_pos;
  // src = Position dans la brosse
  byte* src = brush + y_offset * brush_width + x_offset;
  
  word x,y;
  
  // Pour chaque ligne
  for(y = height;y > 0; y--)
  {
    // Pour chaque pixel
    for(x = width;x > 0; x--)
    {
      // On v�rifie que ce n'est pas la transparence
      if(*src != transp_color)
      {
        *dest = *src;
        *(dest+Screen_width) = *src;
      }

      // Pixel suivant
      src++; dest++;
    }

    // On passe � la ligne suivante
    dest = dest + Screen_width * 2 - width;
    src = src + brush_width - width;
  }
}

void Remap_screen_tall(word x_pos,word y_pos,word width,word height,byte * conversion_table)
{
  // dest = coords a l'�cran
  byte* dest = Screen + y_pos * 2 * Screen_width + x_pos;
  int x,y;

  // Pour chaque ligne
  for(y=height*2;y>0;y--)
  {
    // Pour chaque pixel
    for(x=width;x>0;x--)
    {
      *dest = conversion_table[*dest];
      dest ++;
    }

    dest = dest + Screen_width - width;
  }

  Update_rect(x_pos,y_pos,width,height);
}

void Display_line_on_screen_tall(word x_pos,word y_pos,word width,byte * line)
/* On affiche toute une ligne de pixels. Utilis� pour les textes. */
{
  memcpy(Screen+x_pos+y_pos*2*Screen_width,line,width);
  memcpy(Screen+x_pos+(y_pos*2+1)*Screen_width,line,width);
}

void Read_line_screen_tall(word x_pos,word y_pos,word width,byte * line)
{
  memcpy(line,Screen_width * 2 * y_pos + x_pos + Screen,width);
}

void Display_part_of_screen_scaled_tall(
        word width, // width non zoom�e
        word height, // height zoom�e
        word image_width,byte * buffer)
{
  byte* src = Main_screen + Main_magnifier_offset_Y * image_width 
                      + Main_magnifier_offset_X;
  int y = 0; // Ligne en cours de traitement

  // Pour chaque ligne � zoomer
  while(1)
  {
    int x;
    
    // On �clate la ligne
    Zoom_a_line(src,buffer,Main_magnifier_factor,width);
    // On l'affiche Facteur fois, sur des lignes cons�cutives
    x = Main_magnifier_factor*2;
    // Pour chaque ligne
    do{
      // On affiche la ligne zoom�e
      Display_line_on_screen_simple(
        Main_X_zoom, y, width*Main_magnifier_factor,
        buffer
      );
      // On passe � la suivante
      y++;
      if(y==height*2)
      {
        Update_rect(Main_X_zoom,0,
          width*Main_magnifier_factor,height);
        return;
      }
      x--;
    }while (x > 0);
    src += image_width;
  }
// ATTENTION on n'arrive jamais ici !
}

// Affiche une partie de la brosse couleur zoom�e
void Display_brush_color_zoom_tall(word x_pos,word y_pos,
        word x_offset,word y_offset,
        word width, // width non zoom�e
        word end_y_pos,byte transp_color,
        word brush_width, // width r�elle de la brosse
        byte * buffer)
{
  byte* src = Brush+y_offset*brush_width + x_offset;
  word y = y_pos;
  byte bx;

  // Pour chaque ligne
  while(1)
  {
    Zoom_a_line(src,buffer,Main_magnifier_factor,width);
    // On affiche facteur fois la ligne zoom�e
    for(bx=Main_magnifier_factor;bx>0;bx--)
    {
      Display_transparent_line_on_screen_simple(x_pos,y*2,width*Main_magnifier_factor,buffer,transp_color);
      memcpy(Screen + (y*2 +1) * Screen_width + x_pos, Screen + y*2* Screen_width + x_pos, width*Main_magnifier_factor);
      y++;
      if(y==end_y_pos)
      {
        return;
      }
    }
    src += brush_width;
  }
  // ATTENTION zone jamais atteinte
}

void Display_brush_mono_zoom_tall(word x_pos, word y_pos,
        word x_offset, word y_offset, 
        word width, // width non zoom�e 
        word end_y_pos,
        byte transp_color, byte color, 
        word brush_width, // width r�elle de la brosse
        byte * buffer
)

{
  byte* src = Brush + y_offset * brush_width + x_offset;
  int y=y_pos*2;

  //Pour chaque ligne � zoomer :
  while(1)
  {
    int bx;
    // src = Ligne originale
    // On �clate la ligne
    Zoom_a_line(src,buffer,Main_magnifier_factor,width);

    // On affiche la ligne Facteur fois � l'�cran (sur des
    // lignes cons�cutives)
    bx = Main_magnifier_factor*2;

    // Pour chaque ligne �cran
    do
    {
      // On affiche la ligne zoom�e
      Display_transparent_mono_line_on_screen_simple(
        x_pos, y, width * Main_magnifier_factor, 
        buffer, transp_color, color
      );
      // On passe � la ligne suivante
      y++;
      // On v�rifie qu'on est pas � la ligne finale
      if(y == end_y_pos*2)
      {
        Update_rect( x_pos, y_pos,
          width * Main_magnifier_factor, end_y_pos - y_pos );
        return;
      }
      bx --;
    }
    while (bx > 0);
    
    // Passage � la ligne suivante dans la brosse aussi
    src+=brush_width;
  }
}

void Clear_brush_scaled_tall(word x_pos,word y_pos,word x_offset,word y_offset,word width,word end_y_pos,__attribute__((unused)) byte transp_color,word image_width,byte * buffer)
{
  // En fait on va recopier l'image non zoom�e dans la partie zoom�e !
  byte* src = Main_screen + y_offset * image_width + x_offset;
  int y = y_pos;
  int bx;

  // Pour chaque ligne � zoomer
  while(1){
    Zoom_a_line(src,buffer,Main_magnifier_factor,width);

    bx=Main_magnifier_factor;

    // Pour chaque ligne
    do{
      Display_line_on_screen_tall(x_pos,y,
        width * Main_magnifier_factor,buffer);

      // Ligne suivante
      y++;
      if(y==end_y_pos)
      {
        Update_rect(x_pos,y_pos,
          width*Main_magnifier_factor,end_y_pos-y_pos);
        return;
      }
      bx--;
    }while(bx!=0);

    src+= image_width;
  }
}
