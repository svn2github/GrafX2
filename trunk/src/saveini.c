/* vim:expandtab:ts=2 sw=2:
*/
/*  Grafx2 - The Ultimate 256-color bitmap paint program

    Copyright 2011 Pawel G�ralski
    Copyright 2008 Peter Gordon
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
    along with Grafx2; if not, see <http://www.gnu.org/licenses/>
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "const.h"
#include "global.h"
#include "readini.h"
#include "io.h"
#include "errors.h"
#include "misc.h"
#include "saveini.h"
#include "setup.h"

int Save_INI_reach_group(FILE * old_file,FILE * new_file,char * buffer,char * group)
{
  int    stop_seek;
  char * group_upper;
  char * upper_buffer;

  // On alloue les zones de m�moire:
  group_upper =(char *)malloc(1024);
  upper_buffer=(char *)malloc(1024);

  // On commence par se faire une version majuscule du groupe � rechercher:
  strcpy(group_upper,group);
  Load_INI_clear_string(group_upper, 0);

  stop_seek=0;
  do
  {
    // On lit une ligne dans le fichier:
    if (fgets(buffer,1024,old_file)==0)
    {
      free(upper_buffer);
      free(group_upper);
      return ERROR_INI_CORRUPTED;
    }

    // On s'en fait une version en majuscule:
    strcpy(upper_buffer,buffer);
    Load_INI_clear_string(upper_buffer, 0);

    // On compare la cha�ne avec le groupe recherch�:
    stop_seek=Load_INI_seek_pattern(upper_buffer,group_upper);
    if (fprintf(new_file,"%s",buffer)<0)
    {
      free(upper_buffer);
      free(group_upper);
      return ERROR_SAVING_INI;
    }
  }
  while (stop_seek==0);

  free(upper_buffer);
  free(group_upper);

  return 0;
}



int Save_INI_char_in_value_alphabet(char c)
{
  if (
       (                    // Digit
         (c>='0') &&
         (c<='9')
       ) ||
       (                    // Uppercase letter
         (c>='A') &&
         (c<='Z')
       ) ||
       (                    // Lowerchase letter
         (c>='a') &&
         (c<='z')
       ) ||
       (c == '$') ||        // Hexa prefix
       (c == '-') ||        // Minus sign
       (c== '.')            // Dot (in filenames)
     )
    return 1;
  else
    return 0;
}



void Save_INI_set_value(char * dest,char * source,int nb_values_to_set,int * values,int litteral)
{
  int dest_index;
  int source_index;
  int value_index;

  // On commence par recopier tout jusqu'au symbole '=':
  for (source_index=0;source[source_index]!='=';source_index++)
    dest[source_index]=source[source_index];

  // Puis on recopie le symbole '=':
  dest[source_index]=source[source_index];
  source_index++;

  // Puis on recopie tous les espaces qui suivent:
  for (;source[source_index]==' ';source_index++)
    dest[source_index]=source[source_index];

  // Pour l'instant, la source et la destination en sont au m�me point:
  dest_index=source_index;

  // Puis pour chaque valeur � recopier:
  for (value_index=0;value_index<nb_values_to_set;value_index++)
  {
    // Dans la destination, on �crit la valeur:
    if (litteral)
    {
      // La valeur doit �tre �crite sous la forme Yes/No

      if (values[value_index])
      {
        // La valeur <=> Yes  

        memcpy(dest+dest_index,"yes",3);
        dest_index+=3;
      }
      else
      {
        // La valeur <=> No

        memcpy(dest+dest_index,"no",2);
        dest_index+=2;
      }
    }
    else
    {
      // La valeur doit �tre �crite sous forme num�rique

      if (source[source_index]=='$')
      {
        // On va �crire la valeur sous forme hexad�cimale:

        // On commence par inscrire le symbole '$':
        dest[dest_index]='$';

        // Puis on y concat�ne la valeur:
        sprintf(dest+dest_index+1,"%x",values[value_index]);
        dest_index+=strlen(dest+dest_index);
      }
      else
      {
        // On va �crire la valeur sous forme d�cimale:

        sprintf(dest+dest_index,"%d",values[value_index]);
        dest_index+=strlen(dest+dest_index);
      }
    }

    // Dans la source, on saute la valeur:
    for (;Save_INI_char_in_value_alphabet(source[source_index]) && (source[source_index]!='\0');source_index++);

    if (value_index!=(nb_values_to_set-1))
    {
      // Il reste d'autres valeurs � �crire

      // On recopie tous les caract�res de la source jusqu'au suivant qui
      // d�signe une valeur:
      for (;(!Save_INI_char_in_value_alphabet(source[source_index])) && (source[source_index]!='\0');source_index++,dest_index++)
        dest[dest_index]=source[source_index];
    }
    else
    {
      // C'est la derni�re valeur � initialiser

      // On recopie toute la fin de la ligne:
      for (;source[source_index]!='\0';source_index++,dest_index++)
        dest[dest_index]=source[source_index];

      // Et on n'oublie pas d'y mettre l''\0':
      dest[dest_index]='\0';
    }
  }
}

void Save_INI_set_string(char * dest,char * source,char * value)
{
  int dest_index;
  int source_index;
     
  // On commence par recopier tout jusqu'au symbole '=':
  for (source_index=0;source[source_index]!='=';source_index++)
    dest[source_index]=source[source_index];

  // Puis on recopie le symbole '=':
  dest[source_index]=source[source_index];
  source_index++;

  // Puis on recopie tous les espaces qui suivent:
  for (;source[source_index]==' ';source_index++)
    dest[source_index]=source[source_index];

  // Pour l'instant, la source et la destination en sont au m�me point:
  dest_index=source_index;

  // Dans la destination, on �crit la valeur:
  strcpy(dest+dest_index,value);
  dest_index+=strlen(value);

  // Dans la source, on saute la valeur:
  for (;Save_INI_char_in_value_alphabet(source[source_index]) && (source[source_index]!='\0');source_index++);

  // On recopie toute la fin de la ligne:
  for (;source[source_index]!='\0';source_index++,dest_index++)
    dest[dest_index]=source[source_index];

  // Et on n'oublie pas d'y mettre l''\0':
  dest[dest_index]='\0';
}

int Save_INI_set_strings(FILE * old_file,FILE * new_file,char * buffer,char * option_name,char * value)
{
  int    stop_seek;
  char * option_upper;
  char * upper_buffer;
  char * result_buffer;
  //int    buffer_index;

  // On alloue les zones de m�moire:
  option_upper=(char *)malloc(1024);
  upper_buffer=(char *)malloc(1024);
  result_buffer=(char *)malloc(1024);

  // On convertit un eventuel argument NULL en chaine vide.
  if (value == NULL)
    value="";

  // On commence par se faire une version majuscule de l'option � rechercher:
  strcpy(option_upper,option_name);
  Load_INI_clear_string(option_upper, 0);

  stop_seek=0;
  do
  {
    // On lit une ligne dans le fichier:
    if (fgets(buffer,1024,old_file)==0)
    {
      free(result_buffer);
      free(upper_buffer);
      free(option_upper);
      return ERROR_INI_CORRUPTED;
    }

    // On s'en fait une version en majuscule:
    strcpy(upper_buffer,buffer);
    Load_INI_clear_string(upper_buffer, 0);

    // On compare la cha�ne avec l'option recherch�e:
    stop_seek=Load_INI_seek_pattern(upper_buffer,option_upper);

    if (stop_seek)
    {
      // On l'a trouv�e:

      Save_INI_set_string(result_buffer,buffer,value);
      if (fprintf(new_file,"%s",result_buffer)<0)
      {
        free(result_buffer);
        free(upper_buffer);
        free(option_upper);
        return ERROR_SAVING_INI;
      }
    }
    else
    {
      // On l'a pas trouv�e:

      if (fprintf(new_file,"%s",buffer)<0)
      {
        free(result_buffer);
        free(upper_buffer);
        free(option_upper);
        return ERROR_SAVING_INI;
      }
    }
  }
  while (stop_seek==0);

  free(result_buffer);
  free(upper_buffer);
  free(option_upper);

  return 0;
}

int Save_INI_set_values(FILE * old_file,FILE * new_file,char * buffer,char * option_name,int nb_values_to_set,int * values,int litteral)
{
  int    stop_seek;
  char * option_upper;
  char * upper_buffer;
  char * result_buffer;
  //int    buffer_index;

  // On alloue les zones de m�moire:
  option_upper=(char *)malloc(1024);
  upper_buffer=(char *)malloc(1024);
  result_buffer=(char *)malloc(1024);

  // On commence par se faire une version majuscule de l'option � rechercher:
  strcpy(option_upper,option_name);
  Load_INI_clear_string(option_upper, 0);

  stop_seek=0;
  do
  {
    // On lit une ligne dans le fichier:
    if (fgets(buffer,1024,old_file)==0)
    {
      free(result_buffer);
      free(upper_buffer);
      free(option_upper);
      DEBUG("END OF FILE",0);
      return ERROR_INI_CORRUPTED;
    }

    // On s'en fait une version en majuscule:
    strcpy(upper_buffer,buffer);
    Load_INI_clear_string(upper_buffer, 0);

    // On compare la cha�ne avec l'option recherch�e:
    stop_seek=Load_INI_seek_pattern(upper_buffer,option_upper);

    if (stop_seek)
    {
      // On l'a trouv�e:

      Save_INI_set_value(result_buffer,buffer,nb_values_to_set,values,litteral);
      if (fprintf(new_file,"%s",result_buffer)<0)
      {
        free(result_buffer);
        free(upper_buffer);
        free(option_upper);
        return ERROR_SAVING_INI;
      }
    }
    else
    {
      // On l'a pas trouv�e:

      if (fprintf(new_file,"%s",buffer)<0)
      {
        free(result_buffer);
        free(upper_buffer);
        free(option_upper);
        return ERROR_SAVING_INI;
      }
    }
  }
  while (stop_seek==0);

  free(result_buffer);
  free(upper_buffer);
  free(option_upper);

  return 0;
}



void Save_INI_flush(FILE * old_file,FILE * new_file,char * buffer)
{
  while (fgets(buffer,1024,old_file)!=0)
    fprintf(new_file,"%s",buffer);
}



int Save_INI(T_Config * conf)
{
  FILE * old_file;
  FILE * new_file;
  char * buffer;
  int    values[3];
  char   filename[MAX_PATH_CHARACTERS];
  char   temp_filename[MAX_PATH_CHARACTERS];
  int    return_code;
  char   ref_ini_file[MAX_PATH_CHARACTERS];
  int    ini_file_exists;
  int index;

  // On alloue les zones de m�moire:
  buffer=(char *)malloc(1024);
  
  // On calcule les noms des fichiers qu'on manipule:
  strcpy(filename,Config_directory);
  strcat(filename,INI_FILENAME);

  // On v�rifie si le fichier INI existe
  if ((ini_file_exists = File_exists(filename)))
  {
    strcpy(temp_filename,Config_directory);
    strcat(temp_filename,INISAVE_FILENAME);
    
    // Delete gfx2.$$$
    remove(temp_filename);
    // Rename current config file as gfx2.$$$
    if (rename(filename,temp_filename)!=0)
    {
      goto Erreur_ERREUR_SAUVEGARDE_INI;
    }
  }
  // On r�cup�re un fichier INI "propre" � partir de gfx2def.ini
  strcpy(ref_ini_file,Data_directory);
  strcat(ref_ini_file,INIDEF_FILENAME);
  old_file=fopen(ref_ini_file,"rb");
  if (old_file==0)
  {
    fclose(old_file);
    free(buffer);
    return ERROR_INI_MISSING;
  }
  new_file=fopen(filename,"wb");
  if (new_file==0)
  {
    free(buffer);
    return ERROR_SAVING_INI;
  }
  if ((return_code=Save_INI_reach_group(old_file,new_file,buffer,"[MOUSE]")))
    goto Erreur_Retour;

  values[0]=conf->Mouse_sensitivity_index_x;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"X_sensitivity",1,values,0)))
    goto Erreur_Retour;

  values[0]=conf->Mouse_sensitivity_index_y;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Y_sensitivity",1,values,0)))
    goto Erreur_Retour;

  values[0]=0;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"X_correction_factor",1,values,0)))
    goto Erreur_Retour;

  values[0]=0;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Y_correction_factor",1,values,0)))
    goto Erreur_Retour;

  values[0]=(conf->Cursor)+1;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Cursor_aspect",1,values,0)))
    goto Erreur_Retour;

  if ((return_code=Save_INI_reach_group(old_file,new_file,buffer,"[MENU]")))
    goto Erreur_Retour;

  values[0]=conf->Fav_menu_colors[2].R>>2;
  values[1]=conf->Fav_menu_colors[2].G>>2;
  values[2]=conf->Fav_menu_colors[2].B>>2;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Light_color",3,values,0)))
    goto Erreur_Retour;

  values[0]=conf->Fav_menu_colors[1].R>>2;
  values[1]=conf->Fav_menu_colors[1].G>>2;
  values[2]=conf->Fav_menu_colors[1].B>>2;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Dark_color",3,values,0)))
    goto Erreur_Retour;

  values[0]=conf->Ratio;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Menu_ratio",1,values,0)))
    goto Erreur_Retour;

  if ((return_code=Save_INI_reach_group(old_file,new_file,buffer,"[FILE_SELECTOR]")))
    goto Erreur_Retour;

  values[0]=conf->Show_hidden_files?1:0;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Show_hidden_files",1,values,1)))
    goto Erreur_Retour;

  values[0]=conf->Show_hidden_directories?1:0;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Show_hidden_directories",1,values,1)))
    goto Erreur_Retour;

/*  values[0]=conf->Show_system_directories?1:0;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Show_system_directories",1,values,1)))
    goto Erreur_Retour;
*/
  values[0]=conf->Timer_delay;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Preview_delay",1,values,0)))
    goto Erreur_Retour;

  values[0]=conf->Maximize_preview;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Maximize_preview",1,values,1)))
    goto Erreur_Retour;

  values[0]=conf->Find_file_fast;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Find_file_fast",1,values,0)))
    goto Erreur_Retour;


  if ((return_code=Save_INI_reach_group(old_file,new_file,buffer,"[LOADING]")))
    goto Erreur_Retour;

  values[0]=conf->Auto_set_res;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Auto_set_resolution",1,values,1)))
    goto Erreur_Retour;

  values[0]=conf->Set_resolution_according_to;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Set_resolution_according_to",1,values,0)))
    goto Erreur_Retour;

  values[0]=conf->Clear_palette;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Clear_palette",1,values,1)))
    goto Erreur_Retour;


  if ((return_code=Save_INI_reach_group(old_file,new_file,buffer,"[MISCELLANEOUS]")))
    goto Erreur_Retour;

  values[0]=conf->Display_image_limits;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Draw_limits",1,values,1)))
    goto Erreur_Retour;

  values[0]=conf->Adjust_brush_pick;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Adjust_brush_pick",1,values,1)))
    goto Erreur_Retour;

  values[0]=2-conf->Coords_rel;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Coordinates",1,values,0)))
    goto Erreur_Retour;

  values[0]=conf->Backup;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Backup",1,values,1)))
    goto Erreur_Retour;

  values[0]=conf->Max_undo_pages;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Undo_pages",1,values,0)))
    goto Erreur_Retour;

  values[0]=conf->Delay_left_click_on_slider;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Gauges_scrolling_speed_Left",1,values,0)))
    goto Erreur_Retour;

  values[0]=conf->Delay_right_click_on_slider;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Gauges_scrolling_speed_Right",1,values,0)))
    goto Erreur_Retour;

  values[0]=conf->Auto_save;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Auto_save",1,values,1)))
    goto Erreur_Retour;

  values[0]=conf->Nb_max_vertices_per_polygon;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Vertices_per_polygon",1,values,0)))
    goto Erreur_Retour;

  values[0]=conf->Fast_zoom;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Fast_zoom",1,values,1)))
    goto Erreur_Retour;

  values[0]=conf->Separate_colors;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Separate_colors",1,values,1)))
    goto Erreur_Retour;

  values[0]=conf->FX_Feedback;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"FX_feedback",1,values,1)))
    goto Erreur_Retour;

  values[0]=conf->Safety_colors;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Safety_colors",1,values,1)))
    goto Erreur_Retour;

  values[0]=conf->Opening_message;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Opening_message",1,values,1)))
    goto Erreur_Retour;

  values[0]=conf->Clear_with_stencil;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Clear_with_stencil",1,values,1)))
    goto Erreur_Retour;

  values[0]=conf->Auto_discontinuous;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Auto_discontinuous",1,values,1)))
    goto Erreur_Retour;

  values[0]=conf->Screen_size_in_GIF;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Save_screen_size_in_GIF",1,values,1)))
    goto Erreur_Retour;

  values[0]=conf->Auto_nb_used;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Auto_nb_colors_used",1,values,1)))
    goto Erreur_Retour;

  if ((return_code=Save_INI_set_strings (old_file,new_file,buffer,"Default_video_mode",Mode_label(conf->Default_resolution))))
    goto Erreur_Retour;

  values[0]=Video_mode[0].Width;
  values[1]=Video_mode[0].Height;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Default_window_size",2,values,0)))
    goto Erreur_Retour;

  values[0]=(conf->Mouse_merge_movement);
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Merge_movement",1,values,0)))
    goto Erreur_Retour;

  values[0]=(conf->Palette_cells_X);
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Palette_cells_X",1,values,0)))
    goto Erreur_Retour;

  values[0]=(conf->Palette_cells_Y);
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Palette_cells_Y",1,values,0)))
    goto Erreur_Retour;

  for (index=0;index<NB_BOOKMARKS;index++)
  {
    if ((return_code=Save_INI_set_strings (old_file,new_file,buffer,"Bookmark_label",conf->Bookmark_label[index])))
      goto Erreur_Retour;
    if ((return_code=Save_INI_set_strings (old_file,new_file,buffer,"Bookmark_directory",conf->Bookmark_directory[index])))
      goto Erreur_Retour;
  }
  values[0]=(conf->Palette_vertical);
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Palette_vertical",1,values,1)))
    goto Erreur_Retour;

  values[0]=conf->Window_pos_x;
  values[1]=conf->Window_pos_y;
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Window_position",2,values,0)))
    goto Erreur_Retour;

  values[0]=(conf->Double_click_speed);
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Double_click_speed",1,values,0)))
    goto Erreur_Retour;
    
  values[0]=(conf->Double_key_speed);
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Double_key_speed",1,values,0)))
    goto Erreur_Retour;

  if ((return_code=Save_INI_set_strings (old_file,new_file,buffer,"Skin_file",conf->Skin_file)))
    goto Erreur_Retour;
    
  if ((return_code=Save_INI_set_strings (old_file,new_file,buffer,"Font_file",conf->Font_file)))
    goto Erreur_Retour;

  values[0]=(conf->Grid_XOR_color);
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Grid_XOR_color",1,values,0)))
    goto Erreur_Retour;

  values[0]=(Pixel_ratio);
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Pixel_ratio",1,values,0))) {
    DEBUG("saving pixel ratio",return_code);
    goto Erreur_Retour;
  }
  
  values[0]=0;
  for (index=0; index<MENUBAR_COUNT;index++)
  {
    values[0] |= Menu_bars[index].Visible ? (1<<index) : 0;
  }
  // Fill out the remaining bits. When new toolbars get implemented, they will
  // be visible by default.
  for (; index<8;index++)
    values[0] |= (1<<index);
    
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Menubars_visible",1,values,0)))
    goto Erreur_Retour;

  values[0]=(conf->Right_click_colorpick);
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Right_click_colorpick",1,values,1)))
    goto Erreur_Retour;
    
  values[0]=(conf->Sync_views);
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Sync_views",1,values,1)))
    goto Erreur_Retour;
    
  switch(conf->Swap_buttons)
  {
      case MOD_CTRL:
        values[0]=1;
        break;
      case MOD_ALT:
        values[0]=2;
        break;
      default:
        values[0]=0;
  }
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Swap_buttons",1,values,0)))
    goto Erreur_Retour;
  
  if ((return_code=Save_INI_set_strings (old_file,new_file,buffer,"Scripts_directory",conf->Scripts_directory)))
      goto Erreur_Retour;

  values[0]=(conf->Allow_multi_shortcuts);
  if ((return_code=Save_INI_set_values (old_file,new_file,buffer,"Allow_multi_shortcuts",1,values,1)))
    goto Erreur_Retour;
  
  // Insert new values here
  
  Save_INI_flush(old_file,new_file,buffer);

  fclose(new_file);
  fclose(old_file);

  // On efface le fichier temporaire <=> Ancienne version du .INI
  if (ini_file_exists)
    remove(temp_filename);
  free(buffer);
  return 0;

  // Gestion des erreurs:

  Erreur_Retour:

    fclose(new_file);
    fclose(old_file);
    free(buffer);
    return return_code;

  Erreur_ERREUR_SAUVEGARDE_INI:

    free(buffer);
    return ERROR_SAVING_INI;
}
