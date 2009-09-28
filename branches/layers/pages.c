/*  Grafx2 - The Ultimate 256-color bitmap paint program

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
    along with Grafx2; if not, see <http://www.gnu.org/licenses/>
*/
//////////////////////////////////////////////////////////////////////////
/////////////////////////// GESTION DU BACKUP ////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "global.h"
#include "pages.h"
#include "errors.h"
#include "misc.h"
#include "windows.h"

  ///
  /// GESTION DES PAGES
  ///

/// Bitfield which records which layers are backed up in Page 0.
static word Last_backed_up_layers=0;

/// Allocate and initialize a new page.
T_Page * New_page(byte nb_layers)
{
  T_Page * page;
  
  page = (T_Page *)malloc(sizeof(T_Page)+NB_LAYERS*sizeof(byte *));
  if (page!=NULL)
  {
    int i;
    for (i=0; i<nb_layers; i++)
      page->Image[i]=NULL;
    page->Width=0;
    page->Height=0;
    memset(page->Palette,0,sizeof(T_Palette));
    page->Comment[0]='\0';
    page->File_directory[0]='\0';
    page->Filename[0]='\0';
    page->File_format=DEFAULT_FILEFORMAT;
    page->Nb_layers=nb_layers;
    page->Next = page->Prev = NULL;
  }
  return page;
}

byte * New_layer(long pixel_size)
{
  return (byte *)(malloc(pixel_size));
}
void Free_layer(byte * layer)
{
  free(layer);
}

byte * Dup_layer(byte * layer)
{
  return layer;
}

void Download_infos_page_main(T_Page * page)
// Affiche la page � l'�cran
{
  //int factor_index;
  int size_is_modified;
  
  if (page!=NULL)
  {
    size_is_modified=(Main_image_width!=page->Width) ||
                         (Main_image_height!=page->Height);

    Main_image_width=page->Width;
    Main_image_height=page->Height;
    memcpy(Main_palette,page->Palette,sizeof(T_Palette));
    strcpy(Main_comment,page->Comment);
    strcpy(Main_file_directory,page->File_directory);
    strcpy(Main_filename,page->Filename);
    Main_fileformat=page->File_format;

    if (size_is_modified)
    {
      Main_magnifier_mode=0;
      Main_offset_X=0;
      Main_offset_Y=0;
      Pixel_preview=Pixel_preview_normal;
      Compute_limits();
      Compute_paintbrush_coordinates();
    }
    
  }
  //Update_visible_page_buffer(Visible_image_index, page->Width, page->Height);
  //memcpy(Main_screen, page->Image[Main_current_layer], page->Width*page->Height);
  
}

void Redraw_layered_image(void)
{
  // Re-construct the image with the visible layers
  int layer;  
  // First layer
  for (layer=0; layer<Main_backups->Pages->Nb_layers; layer++)
  {
    if ((1<<layer) & Main_layers_visible)
    {
       // Copy it in Visible_image[0]
       memcpy(Visible_image[0].Image,
         Main_backups->Pages->Image[layer],
         Main_image_width*Main_image_height);
       
       // Initialize the depth buffer
       memset(Visible_image_depth_buffer.Image,
         layer,
         Main_image_width*Main_image_height);
       
       // skip all other layers
       layer++;
       break;
    }
  }
  // subsequent layer(s)
  for (; layer<Main_backups->Pages->Nb_layers; layer++)
  {
    if ((1<<layer) & Main_layers_visible)
    {
      int i;
      for (i=0; i<Main_image_width*Main_image_height; i++)
      {
        byte color = *(Main_backups->Pages->Image[layer]+i);
        if (color != 0) /* transp color */
        {
          *(Visible_image[0].Image+i) = color;
          if (layer != Main_current_layer)
            *(Visible_image_depth_buffer.Image+i) = layer;
        }
      }
    }
  }
  Download_infos_backup(Main_backups);
}

void Upload_infos_page_main(T_Page * page)
// Sauve l'�cran courant dans la page
{
  if (page!=NULL)
  {
    //page->Image[Main_current_layer]=Main_screen;
    page->Width=Main_image_width;
    page->Height=Main_image_height;
    memcpy(page->Palette,Main_palette,sizeof(T_Palette));
    strcpy(page->Comment,Main_comment);
    strcpy(page->File_directory,Main_file_directory);
    strcpy(page->Filename,Main_filename);
    page->File_format=Main_fileformat;
  }
}

void Download_infos_page_spare(T_Page * page)
{
  if (page!=NULL)
  {
    //Spare_screen=page->Image[Spare_current_layer];
    Spare_image_width=page->Width;
    Spare_image_height=page->Height;
    memcpy(Spare_palette,page->Palette,sizeof(T_Palette));
    strcpy(Spare_comment,page->Comment);
    strcpy(Spare_file_directory,page->File_directory);
    strcpy(Spare_filename,page->Filename);
    Spare_fileformat=page->File_format;
  }
}

void Upload_infos_page_spare(T_Page * page)
{
  if (page!=NULL)
  {
    //page->Image[Spare_current_layer]=Spare_screen;
    page->Width=Spare_image_width;
    page->Height=Spare_image_height;
    memcpy(page->Palette,Spare_palette,sizeof(T_Palette));
    strcpy(page->Comment,Spare_comment);
    strcpy(page->File_directory,Spare_file_directory);
    strcpy(page->Filename,Spare_filename);
    page->File_format=Spare_fileformat;
  }
}

void Download_infos_backup(T_List_of_pages * list)
{
  //list->Pages->Next->Image[Main_current_layer];

  if (Config.FX_Feedback)
    FX_feedback_screen=list->Pages->Image[Main_current_layer];
    // Visible_image[0].Image;
  else
    FX_feedback_screen=list->Pages->Next->Image[Main_current_layer];
    // Visible_image[1].Image;
}

void Clear_page(T_Page * page)
{
  // On peut appeler cette fonction sur une page non allou�e.
  int i;
  for (i=0; i<page->Nb_layers; i++)
  {
    Free_layer(page->Image[i]);
    page->Image[i]=NULL;
  }
  page->Width=0;
  page->Height=0;
  // On ne se pr�occupe pas de ce que deviens le reste des infos de l'image.
}

void Copy_S_page(T_Page * dest,T_Page * source)
{
  *dest=*source;
}


  ///
  /// GESTION DES LISTES DE PAGES
  ///

void Init_list_of_pages(T_List_of_pages * list)
{
  // Important: appeler cette fonction sur toute nouvelle structure
  //            T_List_of_pages!

  list->List_size=0;
  list->Pages=NULL;
}

int Allocate_list_of_pages(T_List_of_pages * list)
{
  // Important: la T_List_of_pages ne doit pas d�j� d�signer une liste de
  //            pages allou�e auquel cas celle-ci serait perdue.
  T_Page * page;

  // On initialise chacune des nouvelles pages
  page=New_page(NB_LAYERS);
  if (!page)
    return 0;
  
  // Set as first page of the list
  page->Next = page;
  page->Prev = page;
  list->Pages = page;

  list->List_size=1;

  return 1; // Succ�s
}


void Backward_in_list_of_pages(T_List_of_pages * list)
{
  // Cette fonction fait l'�quivalent d'un "Undo" dans la liste de pages.
  // Elle effectue une sorte de ROL (Rotation Left) sur la liste:
  // +---+-+-+-+-+-+-+-+-+-+  |
  // �0�1�2�3�4�5�6�7�8�9�A�  |
  // +---+-+-+-+-+-+-+-+-+-+  |  0=page courante
  //  � � � � � � � � � � �   |_ A=page la plus ancienne
  //  v v v v v v v v v v v   |  1=Derni�re page (1er backup)
  // +---+-+-+-+-+-+-+-+-+-+  |
  // �1�2�3�4�5�6�7�8�9�A�0�  |
  // +---+-+-+-+-+-+-+-+-+-+  |

  // Pour simuler un v�ritable Undo, l'appelant doit mettre la structure
  // de page courante � jour avant l'appel, puis en r�extraire les infos en
  // sortie, ainsi que celles relatives � la plus r�cente page d'undo (1�re
  // page de la liste).

  if (Last_backed_up_layers)
  {
    // First page contains a ready-made backup of its ->Next.
    // We have swap the first two pages, so the original page 0
    // will end up in position 0 again, and then overwrite it with a backup
    // of the 'new' page1.
    T_Page * page0;
    T_Page * page1;

      page0 = list->Pages;
      page1 = list->Pages->Next;
      
      page0->Next = page1->Next;
      page1->Prev = page0->Prev;
      page0->Prev = page1;
      page1->Next = page0;
      list->Pages = page0;
      return;
  }
  list->Pages = list->Pages->Next;
}

void Advance_in_list_of_pages(T_List_of_pages * list)
{
  // Cette fonction fait l'�quivalent d'un "Redo" dans la liste de pages.
  // Elle effectue une sorte de ROR (Rotation Right) sur la liste:
  // +-+-+-+-+-+-+-+-+-+-+-+  |
  // |0|1|2|3|4|5|6|7|8|9|A|  |
  // +-+-+-+-+-+-+-+-+-+-+-+  |  0=page courante
  //  | | | | | | | | | | |   |_ A=page la plus ancienne
  //  v v v v v v v v v v v   |  1=Derni�re page (1er backup)
  // +-+-+-+-+-+-+-+-+-+-+-+  |
  // |A|0|1|2|3|4|5|6|7|8|9|  |
  // +-+-+-+-+-+-+-+-+-+-+-+  |

  // Pour simuler un v�ritable Redo, l'appelant doit mettre la structure
  // de page courante � jour avant l'appel, puis en r�extraire les infos en
  // sortie, ainsi que celles relatives � la plus r�cente page d'undo (1�re
  // page de la liste).
  if (Last_backed_up_layers)
  {
    // First page contains a ready-made backup of its ->Next.
    // We have swap the first two pages, so the original page 0
    // will end up in position -1 again, and then overwrite it with a backup
    // of the 'new' page1.
    T_Page * page0;
    T_Page * page1;

      page0 = list->Pages;
      page1 = list->Pages->Prev;
      
      page0->Prev = page1->Prev;
      page1->Next = page0->Next;
      page0->Next = page1;
      page1->Prev = page0;
      list->Pages = page1;
      return;
  }
  list->Pages = list->Pages->Prev;
}

void Free_last_page_of_list(T_List_of_pages * list)
{
  if (list!=NULL)
  {
    if (list->List_size>0)
    {
        T_Page * page;
        // The last page is the one before first
        page = list->Pages->Prev;
        
        page->Next->Prev = page->Prev;
        page->Prev->Next = page->Next;
        Clear_page(page);
        free(page);
        list->List_size--;
    }
  }
}

int Create_new_page(T_Page * new_page,T_List_of_pages * list)
{

//   Cette fonction cr�e une nouvelle page dont les attributs correspondent �
// ceux de new_page (width,height,...) (le champ Image est invalide
// � l'appel, c'est la fonction qui le met � jour), et l'enfile dans
// list.


  if (list->List_size >= (Config.Max_undo_pages+1))
  {
    // On manque de m�moire ou la list est pleine. Dans tous les
    // cas, il faut lib�rer une page.
    
    // D�truire la derni�re page allou�e dans la Liste_�_raboter
    Free_last_page_of_list(list);
  }
  {
    int i;
    for (i=0; i<new_page->Nb_layers; i++)
      new_page->Image[i]=New_layer(new_page->Height*new_page->Width);
  }

  
  // Insert as first
  new_page->Next = list->Pages;
  new_page->Prev = list->Pages->Prev;
  list->Pages->Prev->Next = new_page;
  list->Pages->Prev = new_page;
  list->Pages = new_page;
  list->List_size++;
  
  return 1;
}

void Change_page_number_of_list(T_List_of_pages * list,int number)
{
  // Truncate the list if larger than requested
  while(list->List_size > number)
  {
    Free_last_page_of_list(list);
  }
}

void Free_page_of_a_list(T_List_of_pages * list)
{
  // On ne peut pas d�truire la page courante de la liste si apr�s
  // destruction il ne reste pas encore au moins une page.
  if (list->List_size>1)
  {
    // On fait faire un undo � la liste, comme �a, la nouvelle page courante
    // est la page pr�c�dente
    Backward_in_list_of_pages(Main_backups);

    // Puis on d�truit la derni�re page, qui est l'ancienne page courante
    Free_last_page_of_list(list);
  }
}

int Update_visible_page_buffer(int index, int width, int height)
{
  if (Visible_image[index].Width != width || Visible_image[index].Height != height)
  {
    Visible_image[index].Width = width;
    Visible_image[index].Height = height;
    free(Visible_image[index].Image);
    Visible_image[index].Image = (byte *)malloc(width * height);
    if (Visible_image[index].Image == NULL)
      return 0;
  }
  return 1;
}

int Update_depth_buffer(int width, int height)
{
  if (Visible_image_depth_buffer.Width != width || Visible_image_depth_buffer.Height != height)
  {
    Visible_image_depth_buffer.Width = width;
    Visible_image_depth_buffer.Height = height;
    free(Visible_image_depth_buffer.Image);
    Visible_image_depth_buffer.Image = (byte *)malloc(width * height);
    if (Visible_image_depth_buffer.Image == NULL)
      return 0;
  }
  return 1;
}

  ///
  /// GESTION DES BACKUPS
  ///

int Init_all_backup_lists(int width,int height)
{
  // width et height correspondent � la dimension des images de d�part.
  int i;

  if (! Allocate_list_of_pages(Main_backups) ||
      ! Allocate_list_of_pages(Spare_backups))
    return 0;
  // On a r�ussi � allouer deux listes de pages dont la taille correspond �
  // celle demand�e par l'utilisateur.

  // On cr�e un descripteur de page correspondant � la page principale
  Upload_infos_page_main(Main_backups->Pages);
  // On y met les infos sur la dimension de d�marrage
  Main_backups->Pages->Width=width;
  Main_backups->Pages->Height=height;
  for (i=0; i<Main_backups->Pages->Nb_layers; i++)
  {
    Main_backups->Pages->Image[i]=New_layer(width*height);
    if (! Main_backups->Pages->Image[i])
      return 0;
  }

  if (!Update_visible_page_buffer(0, width, height))
    return 0;
  Main_screen=Visible_image[0].Image;
  
  if (!Update_visible_page_buffer(1, width, height))
    return 0;
  Screen_backup=Visible_image[1].Image;
      
  Download_infos_page_main(Main_backups->Pages); 
  Download_infos_backup(Main_backups);

  // Default values for spare page
  Spare_backups->Pages->Width = width;
  Spare_backups->Pages->Height = height;
  memcpy(Spare_backups->Pages->Palette,Main_palette,sizeof(T_Palette));
  strcpy(Spare_backups->Pages->Comment,"");
  strcpy(Spare_backups->Pages->File_directory,Spare_current_directory);
  strcpy(Spare_backups->Pages->Filename,"NO_NAME.GIF");
  Spare_backups->Pages->File_format=DEFAULT_FILEFORMAT;
  // Copy this informations in the global Spare_ variables
  Download_infos_page_spare(Spare_backups->Pages);
    
  // Clear the initial Visible buffer
  //memset(Main_screen,0,Main_image_width*Main_image_height);

  // Spare
  for (i=0; i<NB_LAYERS; i++)
  {
    Spare_backups->Pages->Image[i]=New_layer(width*height);
    if (! Spare_backups->Pages->Image[i])
      return 0;
  }
  //memset(Spare_screen,0,Spare_image_width*Spare_image_height);

  Visible_image[0].Width = width;
  Visible_image[0].Height = height;
  Visible_image[0].Image = NULL;

  Visible_image[1].Width = width;
  Visible_image[1].Height = height;
  Visible_image[1].Image = NULL;

  Visible_image_depth_buffer.Width = width;
  Visible_image_depth_buffer.Height = height;
  Visible_image_depth_buffer.Image = NULL;

  Visible_image[0].Image = (byte *)malloc(Visible_image[0].Width * Visible_image[0].Height);
  if (! Visible_image[0].Image)
    return 0;
    
  Visible_image[1].Image = (byte *)malloc(Visible_image[1].Width * Visible_image[1].Height);
  if (! Visible_image[1].Image)
    return 0;
    
  Visible_image_depth_buffer.Image = (byte *)malloc(Visible_image_depth_buffer.Width * Visible_image_depth_buffer.Height);
  if (! Visible_image_depth_buffer.Image)
    return 0;
    
  End_of_modification();
  return 1;
}

void Set_number_of_backups(int nb_backups)
{
  Change_page_number_of_list(Main_backups,nb_backups+1);
  Change_page_number_of_list(Spare_backups,nb_backups+1);

  // Le +1 vient du fait que dans chaque liste, en 1�re position on retrouve
  // les infos de la page courante sur le brouillon et la page principale.
  // (nb_backups = Nombre de backups, sans compter les pages courantes)
}

int Backup_with_new_dimensions(int upload,int width,int height)
{
  // Retourne 1 si une nouvelle page est disponible (alors pleine de 0) et
  // 0 sinon.

  T_Page * new_page;
  byte nb_layers;
  int return_code=0;
  int i;

  if (upload)
    // On remet � jour l'�tat des infos de la page courante (pour pouvoir les
    // retrouver plus tard)
    Upload_infos_page_main(Main_backups->Pages);

  nb_layers=Main_backups->Pages->Nb_layers;
  // On cr�e un descripteur pour la nouvelle page courante
  new_page=New_page(nb_layers);
  if (!new_page)
  {
    Error(0);
    return 0;
  }
  //Copy_S_page(new_page,Main_backups->Pages);
  Upload_infos_page_main(new_page);
  new_page->Width=width;
  new_page->Height=height;
  if (Create_new_page(new_page,Main_backups))
  {
    for (i=0; i<nb_layers;i++)
    {
      //Main_backups->Pages->Image[i]=(byte *)malloc(width*height);
      memset(Main_backups->Pages->Image[i], 0, width*height);
    }
    Update_depth_buffer(width, height);

    Update_visible_page_buffer(0, width, height);
    Main_screen=Visible_image[0].Image;
    
    Update_visible_page_buffer(1, width, height);
    Screen_backup=Visible_image[1].Image;


    Download_infos_page_main(Main_backups->Pages);
    Download_infos_backup(Main_backups);
    // On nettoie la nouvelle image:
    //memset(Main_screen,0,width*height);
    
    return_code=1;
  }

  // On d�truit le descripteur de la page courante
  //free(new_page);

  return return_code;
}

int Backup_and_resize_the_spare(int width,int height)
{
  // Retourne 1 si la page de dimension souhaitee est disponible en brouillon
  // et 0 sinon.

  T_Page * new_page;
  int return_code=0;

  // On remet � jour l'�tat des infos de la page de brouillon (pour pouvoir
  // les retrouver plus tard)
  Upload_infos_page_spare(Spare_backups->Pages);

  // On cr�e un descripteur pour la nouvelle page de brouillon
  new_page=New_page(Spare_backups->Pages->Nb_layers);
  if (!new_page)
  {
    Error(0);
    return 0;
  }
  
  Upload_infos_page_spare(new_page);
  new_page->Width=width;
  new_page->Height=height;
  if (Create_new_page(new_page,Spare_backups))
  {
    Download_infos_page_spare(new_page);
    return_code=1;
  }

  // On d�truit le descripteur de la page courante
  free(new_page);

  return return_code;
}

void Backup(void)
// Sauve la page courante comme premi�re page de backup et cr�e une nouvelle page
// pur continuer � dessiner. Utilis� par exemple pour le fill
{
  Backup_layers(1<<Main_current_layer);
}

void Backup_layers(byte layer_mask)
{
    int i;
  
  T_Page *new_page;

  /*
  if (Last_backed_up_layers == (1<<Main_current_layer))
    return; // Already done.
  */

  // On remet � jour l'�tat des infos de la page courante (pour pouvoir les
  // retrouver plus tard)
  Upload_infos_page_main(Main_backups->Pages);

  // On cr�e un descripteur pour la nouvelle page courante
  new_page=New_page(Main_backups->Pages->Nb_layers);
  if (!new_page)
  {
    Error(0);
    return;
  }
  
  // Enrichissement de l'historique
  Copy_S_page(new_page,Main_backups->Pages);
  Create_new_page(new_page,Main_backups);
  Download_infos_page_main(new_page);

  Download_infos_backup(Main_backups);

  // On copie l'image du backup vers la page courante:
  for (i=0; i<Main_backups->Pages->Nb_layers;i++)
    memcpy(Main_backups->Pages->Image[i],
           Main_backups->Pages->Next->Image[i],
           Main_image_width*Main_image_height);

  // On allume l'indicateur de modification de l'image
  Main_image_is_modified=1;
  
  /*
  Last_backed_up_layers = 1<<Main_current_layer;
  */
}

void Undo(void)
{
  if (Last_backed_up_layers)
  {
    Free_page_of_a_list(Main_backups);
    Last_backed_up_layers=0;
  }

  // On remet � jour l'�tat des infos de la page courante (pour pouvoir les
  // retrouver plus tard)
  Upload_infos_page_main(Main_backups->Pages);
  // On fait faire un undo � la liste des backups de la page principale
  Backward_in_list_of_pages(Main_backups);

  // On extrait ensuite les infos sur la nouvelle page courante
  Download_infos_page_main(Main_backups->Pages);
  // Et celles du backup
  Download_infos_backup(Main_backups);
  // Note: le backup n'a pas obligatoirement les m�mes dimensions ni la m�me
  //       palette que la page courante. Mais en temps normal, le backup
  //       n'est pas utilis� � la suite d'un Undo. Donc �a ne devrait pas
  //       poser de probl�mes.
  Redraw_layered_image();
  
}

void Redo(void)
{
  if (Last_backed_up_layers)
  {
    Free_page_of_a_list(Main_backups);
    Last_backed_up_layers=0;
  }
  // On remet � jour l'�tat des infos de la page courante (pour pouvoir les
  // retrouver plus tard)
  Upload_infos_page_main(Main_backups->Pages);
  // On fait faire un redo � la liste des backups de la page principale
  Advance_in_list_of_pages(Main_backups);

  // On extrait ensuite les infos sur la nouvelle page courante
  Download_infos_page_main(Main_backups->Pages);
  // Et celles du backup
  Download_infos_backup(Main_backups);
  // Note: le backup n'a pas obligatoirement les m�mes dimensions ni la m�me
  //       palette que la page courante. Mais en temps normal, le backup
  //       n'est pas utilis� � la suite d'un Redo. Donc �a ne devrait pas
  //       poser de probl�mes.
  Redraw_layered_image();

}

void Free_current_page(void)
{
  // On d�truit la page courante de la liste principale
  Free_page_of_a_list(Main_backups);
  // On extrait ensuite les infos sur la nouvelle page courante
  Download_infos_page_main(Main_backups->Pages);
  // Et celles du backup
  Download_infos_backup(Main_backups);
  // Note: le backup n'a pas obligatoirement les m�mes dimensions ni la m�me
  //       palette que la page courante. Mais en temps normal, le backup
  //       n'est pas utilis� � la suite d'une destruction de page. Donc �a ne
  //       devrait pas poser de probl�mes.
}

void Exchange_main_and_spare(void)
{
  T_List_of_pages * temp_list;

  // On commence par mettre � jour dans les descripteurs les infos sur les
  // pages qu'on s'appr�te � �changer, pour qu'on se retrouve pas avec de
  // vieilles valeurs qui datent de mathuzalem.
  Upload_infos_page_main(Main_backups->Pages);
  Upload_infos_page_spare(Spare_backups->Pages);

  // On inverse les listes de pages
  temp_list=Main_backups;
  Main_backups=Spare_backups;
  Spare_backups=temp_list;

  // On extrait ensuite les infos sur les nouvelles pages courante, brouillon
  // et backup.

  Update_depth_buffer(Main_backups->Pages->Width, Main_backups->Pages->Height);

  Update_visible_page_buffer(0, Main_backups->Pages->Width, Main_backups->Pages->Height);
  Main_screen=Visible_image[0].Image;
  
  Update_visible_page_buffer(1, Main_backups->Pages->Width, Main_backups->Pages->Height);
  Screen_backup=Visible_image[1].Image;


  /* SECTION GROS CACA PROUT PROUT */
  // Auparavant on ruse en mettant d�j� � jour les dimensions de la
  // nouvelle page courante. Si on ne le fait pas, le "Download" va d�tecter
  // un changement de dimensions et va b�tement sortir du mode loupe, alors
  // que lors d'un changement de page, on veut bien conserver l'�tat du mode
  // loupe du brouillon.
  Main_image_width=Main_backups->Pages->Width;
  Main_image_height=Main_backups->Pages->Height;

  Download_infos_page_main(Main_backups->Pages);
  Download_infos_backup(Main_backups);
  Download_infos_page_spare(Spare_backups->Pages);
  Redraw_layered_image();
}

void End_of_modification(void)
{

  //Update_visible_page_buffer(1, Main_image_width, Main_image_height);
  
  
  memcpy(Visible_image[1].Image,
         Visible_image[0].Image,
         Main_image_width*Main_image_height);
  
  Download_infos_backup(Main_backups);
/*  
  Last_backed_up_layers = 0;
  Backup();
  */
}
