/*  Grafx2 - The Ultimate 256-color bitmap paint program

    Copyright 2009 Franck Charlet
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
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#if defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__) || defined(__amigaos__)
    #include <proto/dos.h>
    #include <dirent.h>
    #define isHidden(x) (0)
#elif defined(__WIN32__)
    #include <dirent.h>
    #include <windows.h>
    #define isHidden(x) (GetFileAttributesA((x)->d_name)&FILE_ATTRIBUTE_HIDDEN)
#else
    #include <dirent.h>
    #define isHidden(x) ((x)->d_name[0]=='.')
#endif

#include "const.h"
#include "struct.h"
#include "global.h"
#include "misc.h"
#include "errors.h"
#include "io.h"
#include "windows.h"
#include "sdlscreen.h"
#include "loadsave.h"
#include "mountlist.h"
#include "engine.h"
#include "readline.h"
#include "input.h"
#include "help.h"
#include "filesel.h"

#define NORMAL_FILE_COLOR    MC_Light // color du texte pour une ligne de fichier non s�lectionn�
#define NORMAL_DIRECTORY_COLOR MC_Dark // color du texte pour une ligne de r�pertoire non s�lectionn�
#define NORMAL_BACKGROUND_COLOR       MC_Black  // color du fond  pour une ligne non s�lectionn�e
#define SELECTED_FILE_COLOR    MC_White // color du texte pour une ligne de fichier    s�lectionn�e
#define SELECTED_DIRECTORY_COLOR MC_Light // color du texte pour une ligne de rep�rtoire s�lectionn�e
#define SELECTED_BACKGROUND_COLOR       MC_Dark // color du fond  pour une ligne s�lectionn�e

// Conventions:
//
// * Le fileselect modifie le r�pertoire courant. Ceci permet de n'avoir
//   qu'un findfirst dans le r�pertoire courant � faire:


// -- D�struction de la liste cha�n�e ---------------------------------------
void Free_fileselector_list(void)
//  Cette proc�dure d�truit la chaine des fichiers. Elle doit �tre appel�e
// avant de rappeler la fonction Read_list_of_files, ainsi qu'en fin de
// programme.
{
  // Pointeur temporaire de destruction
  T_Fileselector_item * temp_item;

  while (Filelist!=NULL)
  {
    // On m�morise l'adresse du premier �l�ment de la liste
    temp_item =Filelist;
    // On fait avancer la t�te de la liste
    Filelist=Filelist->Next;
    // Et on efface l'ancien premier �l�ment de la liste
    free(temp_item);
  }
}


///
/// Formats a display name for a file, directory, or similar name (drive, volume).
/// The returned value is a pointer to a single static buffer of 19 characters
/// including the '\0'.
char * Format_filename(char * fname, int type)
{
  static char result[19];
  int         c;
  int         other_cursor;
  int         pos_last_dot;

  if (strcmp(fname,PARENT_DIR)==0)
  {
    strcpy(result,"<-PARENT DIRECTORY");
  }
  else if (fname[0]=='.' || type==1 || type==2)
  {
    // Files ".something" or drives or directories: Aligned left on 18 chars max
    strcpy(result,"                  ");
    for (c=0;fname[c]!='\0' && c < 18;c++)
      result[c]=fname[c];
    // A special character indicates the filename is truncated
    if (c >= 18)
      result[17]=ELLIPSIS_CHARACTER;
  }
  else
  {
    strcpy(result,"              .   ");
    // Look for the last dot in filename
    pos_last_dot = -1;
    for (c = 0; fname[c]!='\0'; c++)
      if (fname[c]=='.')
        pos_last_dot = c;

    // Copy the part before the dot
    for (c=0; c!=pos_last_dot && fname[c]!='\0'; c++)
    {
      if (c > 13)
      {
        result[13]=ELLIPSIS_CHARACTER;
        break;
      }
      result[c]=fname[c];
    }

    // Ensuite on recopie la partie qui suit le point (si n�cessaire):
    if (pos_last_dot != -1)
    {
      for (c = pos_last_dot+1,other_cursor=15;fname[c]!='\0' && other_cursor < 18;c++,other_cursor++)
        result[other_cursor]=fname[c];
    }
  }
  return result;
}


// -- Rajouter a la liste des elements de la liste un element ---------------
void Add_element_to_list(char * fname, int type)
//  Cette procedure ajoute a la liste chainee un fichier pass� en argument.
{
  // Pointeur temporaire d'insertion
  T_Fileselector_item * temp_item;

  // On alloue de la place pour un nouvel element
  temp_item=(T_Fileselector_item *)malloc(sizeof(T_Fileselector_item));

  // On met a jour le nouvel emplacement:
  strcpy(temp_item->Short_name,Format_filename(fname, type));
  strcpy(temp_item->Full_name,fname);
  temp_item->Type = type;

  temp_item->Next  =Filelist;
  temp_item->Previous=NULL;

  if (Filelist!=NULL)
    Filelist->Previous=temp_item;
  Filelist=temp_item;
}

// -- V�rification si un fichier a l'extension demand�e.
// Autorise les '?', et '*' si c'est le seul caract�re.
int Check_extension(const char *filename, char * filter)
{
  int pos_last_dot = -1;
  int c = 0;

  if (filter[0] == '*')
    return 1;
  // On recherche la position du dernier . dans le nom
  for (c = 0; filename[c]!='\0'; c++)
    if (filename[c]=='.')
      pos_last_dot = c;
  // Fichier sans extension (ca arrive)
  if (pos_last_dot == -1)
    return (filter[0] == '\0');

  // V�rification caract�re par caract�re, case-insensitive.
  c = 0;
  do
  {
    if (filter[c] != '?' &&
      tolower(filter[c]) != tolower(filename[pos_last_dot + 1 + c]))
      return 0;

     c++;
  } while (filter[c++] != '\0');

  return 1;
}


// -- Lecture d'une liste de fichiers ---------------------------------------
void Read_list_of_files(byte selected_format)
//  Cette proc�dure charge dans la liste chain�e les fichiers dont l'extension
// correspond au format demand�.
{
  DIR*  current_directory; //R�pertoire courant
  struct dirent* entry; // Structure de lecture des �l�ments
  char * filter = "*"; // Extension demand�e
  struct stat Infos_enreg;
  char * current_path;

  // Tout d'abord, on d�duit du format demand� un filtre � utiliser:
  if (selected_format) // Format (extension) sp�cifique
    filter = File_formats[selected_format-1].Extension;

  // Ensuite, on vide la liste actuelle:
  Free_fileselector_list();
  // Apr�s effacement, il ne reste ni fichier ni r�pertoire dans la liste
  Filelist_nb_files=0;
  Filelist_nb_directories=0;

  // On lit tous les r�pertoires:
  current_path=getcwd(NULL,0);
  current_directory=opendir(current_path);
  while ((entry=readdir(current_directory)))
  {
    // On ignore le r�pertoire courant
    if ( !strcmp(entry->d_name, "."))
    {
      continue;
    }
    stat(entry->d_name,&Infos_enreg);
    // et que l'�l�ment trouv� est un r�pertoire
    if( S_ISDIR(Infos_enreg.st_mode) &&
      // et que c'est ".."
      (!strcmp(entry->d_name, PARENT_DIR) ||
      // ou qu'il n'est pas cach�
       Config.Show_hidden_directories ||
     !isHidden(entry)))
    {
      // On rajoute le r�pertoire � la liste
      Add_element_to_list(entry->d_name, 1);
      Filelist_nb_directories++;
    }
    else if (S_ISREG(Infos_enreg.st_mode) && //Il s'agit d'un fichier
      (Config.Show_hidden_files || //Il n'est pas cach�
      !isHidden(entry)))
    {
      if (Check_extension(entry->d_name, filter))
      {
        // On rajoute le fichier � la liste
        Add_element_to_list(entry->d_name, 0);
        Filelist_nb_files++;
      }
    }
  }

#if defined(__MORPHOS__) || defined(__AROS__) || defined (__amigaos4__) || defined(__amigaos__)
  Add_element_to_list("/",1); // on amiga systems, / means parent. And there is no ..
  Filelist_nb_directories ++;
#endif

  closedir(current_directory);
  free(current_path);

  Filelist_nb_elements=Filelist_nb_directories+Filelist_nb_files;
}

#if defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__) || defined(__amigaos__)
void bstrtostr( BSTR in, STRPTR out, TEXT max )
{
  STRPTR iptr;
  dword i;

  iptr = BADDR( in );

  if( max > iptr[0] ) max = iptr[0];

#if defined(__AROS__)
  for ( i=0 ; i<max ; i++ ) out[i] = *(AROS_BSTR_ADDR(iptr+i));
#else
  for( i=0; i<max; i++ ) out[i] = iptr[i+1];
#endif
  out[i] = 0;
}
#endif

// -- Lecture d'une liste de lecteurs / volumes -----------------------------
void Read_list_of_drives(void)
{

  // Empty the current content of fileselector:
  Free_fileselector_list();
  // Reset number of items
  Filelist_nb_files=0;
  Filelist_nb_directories=0;

  #if defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__) || defined(__amigaos__)
  {
    struct DosList *dl;
    char tmp[256];

    dl = LockDosList( LDF_VOLUMES | LDF_READ );
    if( dl )
    {
      while( ( dl = NextDosEntry( dl, LDF_VOLUMES | LDF_READ ) ) )
      {
        bstrtostr( dl->dol_Name, tmp, 254 );
        strcat( tmp, ":" );
        Add_element_to_list( tmp, 2 );
        Filelist_nb_directories++;
      }
      UnLockDosList( LDF_VOLUMES | LDF_READ );
    }
  }
  #elif defined (__WIN32__)
  {
    char drive_name[]="A:\\";
    int drive_bits = GetLogicalDrives();
    int drive_index;
    int bit_index;
    // Sous Windows, on a la totale, presque aussi bien que sous DOS:
    drive_index = 0;
    for (bit_index=0; bit_index<26 && drive_index<23; bit_index++)
    {
      if ( (1 << bit_index) & drive_bits )
      {
        // On a ce lecteur, il faut maintenant d�terminer son type "physique".
        // pour profiter des jolies icones de X-man.
        int drive_type;
        char drive_path[]="A:\\";
        // Cette API Windows est �trange, je dois m'y faire...
        drive_path[0]='A'+bit_index;
        switch (GetDriveType(drive_path))
        {
          case DRIVE_CDROM:
            drive_type=ICON_CDROM;
            break;
          case DRIVE_REMOTE:
            drive_type=ICON_NETWORK;
            break;
          case DRIVE_REMOVABLE:
            drive_type=ICON_FLOPPY_3_5;
            break;
          case DRIVE_FIXED:
            drive_type=ICON_HDD;
            break;
          default:
            drive_type=ICON_NETWORK;
            break;
        }
        drive_name[0]='A'+bit_index;
        Add_element_to_list(drive_name,2);
        Filelist_nb_directories++;
        drive_index++;
      }
    }
  }
  #else
  {
    //Sous les diff�rents unix, on va mettre
    // un disque dur qui pointera vers la racine,
    // et un autre vers le home directory de l'utilisateur.

    // Ensuite on utilise read_file_system_list pour compl�ter

    struct mount_entry* Liste_points_montage;
    struct mount_entry* next;

    #if defined(__BEOS__) || defined(__HAIKU__)
        char * home_dir = getenv("$HOME");
    #else
        char * home_dir = getenv("HOME");
    #endif
    Add_element_to_list("/", 2);
    Filelist_nb_directories++;
    if(home_dir)
    {
        Add_element_to_list(home_dir, 2);
        Filelist_nb_directories++;
    }

    Liste_points_montage = read_file_system_list(0);

    while(Liste_points_montage != NULL)
    {
        if(Liste_points_montage->me_dummy == 0 && strcmp(Liste_points_montage->me_mountdir,"/") && strcmp(Liste_points_montage->me_mountdir,"/home"))
        {
            Add_element_to_list(Liste_points_montage->me_mountdir,2);
            Filelist_nb_directories++;
        }
        next = Liste_points_montage -> me_next;
        #if !(defined(__macosx__) || defined(__FreeBSD__))
          free(Liste_points_montage -> me_type);
        #endif
        free(Liste_points_montage -> me_devname);
        free(Liste_points_montage -> me_mountdir);
        free(Liste_points_montage);
        Liste_points_montage = next;
    }

  }
  #endif

  Filelist_nb_elements=Filelist_nb_directories+Filelist_nb_files;
}


// -- Tri de la liste des fichiers et r�pertoires ---------------------------
void Sort_list_of_files(void)
// Tri la liste chain�e existante dans l'ordre suivant:
//
// * Les r�pertoires d'abord, dans l'ordre alphab�tique de leur nom
// * Les fichiers ensuite, dans l'ordre alphab�tique de leur nom
{
  byte   list_is_sorted; // Bool�en "La liste est tri�e"
  byte   need_swap;          // Bool�en "Il faut inverser les �l�ments"
  T_Fileselector_item * prev_item;
  T_Fileselector_item * current_item;
  T_Fileselector_item * next_item;
  T_Fileselector_item * next_to_next_item;

  // Avant de trier quoi que ce soit, on v�rifie qu'il y ait suffisamment
  // d'�l�ments pour qu'il soit possibles qu'ils soient en d�sordre:
  if (Filelist_nb_elements>1)
  {
    do
    {
      // Par d�faut, on consid�re que la liste est tri�e
      list_is_sorted=1;

      current_item=Filelist;
      next_item=current_item->Next;

      while ( (current_item!=NULL) && (next_item!=NULL) )
      {
        // On commence par supposer qu'il n'y pas pas besoin d'inversion
        need_swap=0;

        // Ensuite, on v�rifie si les deux �l�ments sont bien dans l'ordre ou
        // non:

          // Si l'�l�ment courant est un fichier est que le suivant est
          // un r�pertoire -> need_swap
        if ( current_item->Type < next_item->Type )
          need_swap=1;
          // Si les deux �l�ments sont de m�me type et que le nom du suivant
          // est plus petit que celui du courant -> need_swap
        else if ( (current_item->Type==next_item->Type) &&
                  (strcmp(current_item->Full_name,next_item->Full_name)>0) )
          need_swap=1;


        if (need_swap)
        {
          // Si les deux �l�ments n�cessitent d'�tre invers�:

          // On les inverses:

          // On note avant tout les �l�ments qui encapsulent nos deux amis
          prev_item         =current_item->Previous;
          next_to_next_item=next_item->Next;

          // On permute le cha�nage des deux �l�ments entree eux
          current_item->Next  =next_to_next_item;
          current_item->Previous=next_item;
          next_item->Next  =current_item;
          next_item->Previous=prev_item;

          // On tente un cha�nage des �l�ments encapsulant les comp�res:
          if (prev_item!=NULL)
            prev_item->Next=next_item;
          if (next_to_next_item!=NULL)
            next_to_next_item->Previous=current_item;

          // On fait bien attention � modifier la t�te de liste en cas de besoin
          if (current_item==Filelist)
            Filelist=next_item;

          // Ensuite, on se pr�pare � �tudier les �l�ments pr�c�dents:
          current_item=prev_item;

          // Et on constate que la liste n'�tait pas encore g�nialement tri�e
          list_is_sorted=0;
        }
        else
        {
          // Si les deux �l�ments sont dans l'ordre:

          // On passe aux suivants
          current_item=current_item->Next;
          next_item=next_item->Next;
        }
      }
    }
    while (!list_is_sorted);
  }
}


// -- Affichage des �l�ments de la liste de fichier / r�pertoire ------------
void Display_file_list(short offset_first,short selector_offset)
//
// offset_first = D�calage entre le premier fichier visible dans le
//                   s�lecteur et le premier fichier de la liste
//
// selector_offset  = D�calage entre le premier fichier visible dans le
//                   s�lecteur et le fichier s�lectionn� dans la liste
//
{
  T_Fileselector_item * current_item;
  byte   index;  // index du fichier qu'on affiche (0 -> 9)
  byte   text_color;
  byte   background_color;


  // On v�rifie s'il y a au moins 1 fichier dans la liste:
  if (Filelist_nb_elements>0)
  {
    // On commence par chercher � pointer sur le premier fichier visible:
    current_item=Filelist;
    for (;offset_first>0;offset_first--)
      current_item=current_item->Next;

    // Pour chacun des 10 �l�ments inscriptibles � l'�cran
    for (index=0;index<10;index++)
    {
      // S'il est s�lectionn�:
      if (!selector_offset)
      {
        // Si c'est un fichier
        if (current_item->Type==0)
          text_color=SELECTED_FILE_COLOR;
        else
          text_color=SELECTED_DIRECTORY_COLOR;

        background_color=SELECTED_BACKGROUND_COLOR;
      }
      else
      {
        // Si c'est un fichier
        if (current_item->Type==0)
          text_color=NORMAL_FILE_COLOR;
        else
          text_color=NORMAL_DIRECTORY_COLOR;

        background_color=NORMAL_BACKGROUND_COLOR;
      }

      // On affiche l'�l�ment
      Print_in_window(8,95+index*8,current_item->Short_name,text_color,background_color);

      // On passe � la ligne suivante
      selector_offset--;
      current_item=current_item->Next;
      if (!current_item)
        break;
    } // End de la boucle d'affichage

  } // End du test d'existence de fichiers
}


// -- R�cup�rer le libell� d'un �l�ment de la liste -------------------------
void Get_selected_item(short offset_first,short selector_offset,char * label,int *type)
//
// offset_first = D�calage entre le premier fichier visible dans le
//                   s�lecteur et le premier fichier de la liste
//
// selector_offset  = D�calage entre le premier fichier visible dans le
//                   s�lecteur et le fichier � r�cup�rer
//
// label          = str de r�ception du libell� de l'�l�ment
//
// type             = R�cup�ration du type: 0 fichier, 1 repertoire, 2 lecteur.
//                    Passer NULL si pas interess�.
{
  T_Fileselector_item * current_item;

  // On v�rifie s'il y a au moins 1 fichier dans la liste:
  if (Filelist_nb_elements>0)
  {
    // On commence par chercher � pointer sur le premier fichier visible:
    current_item=Filelist;
    for (;offset_first>0;offset_first--)
      current_item=current_item->Next;

    // Ensuite, on saute autant d'�l�ments que le d�calage demand�:
    for (;selector_offset>0;selector_offset--)
      current_item=current_item->Next;

    // On recopie la cha�ne
    strcpy(label, current_item->Full_name);

    if (type != NULL)
      *type=current_item->Type;
  } // End du test d'existence de fichiers
}


// ----------------- D�placements dans la liste de fichiers -----------------

void Selector_scroll_down(short * offset_first,short * selector_offset)
// Fait scroller vers le bas le s�lecteur de fichier... (si possible)
{
  if ( ((*selector_offset)<9)
    && ( (*selector_offset)+1 < Filelist_nb_elements ) )
    // Si la s�lection peut descendre
    Display_file_list(*offset_first,++(*selector_offset));
  else // Sinon, descendre la fen�tre (si possible)
  if ((*offset_first)+10<Filelist_nb_elements)
    Display_file_list(++(*offset_first),*selector_offset);
}


void Selector_scroll_up(short * offset_first,short * selector_offset)
// Fait scroller vers le haut le s�lecteur de fichier... (si possible)
{
  if ((*selector_offset)>0)
    // Si la s�lection peut monter
    Display_file_list(*offset_first,--(*selector_offset));
  else // Sinon, monter la fen�tre (si possible)
  if ((*offset_first)>0)
    Display_file_list(--(*offset_first),*selector_offset);
}


void Selector_page_down(short * offset_first,short * selector_offset, short lines)
{
  if (Filelist_nb_elements-1>*offset_first+*selector_offset)
  {
    if (*selector_offset<9)
    {
      if (Filelist_nb_elements<10)
      {
        *offset_first=0;
        *selector_offset=Filelist_nb_elements-1;
      }
      else *selector_offset=9;
    }
    else
    {
      if (Filelist_nb_elements>*offset_first+18)
        *offset_first+=lines;
      else
      {
        *offset_first=Filelist_nb_elements-10;
        *selector_offset=9;
      }
    }
  }
  Display_file_list(*offset_first,*selector_offset);
}


void Selector_page_up(short * offset_first,short * selector_offset, short lines)
{
  if (*offset_first+*selector_offset>0)
  {
    if (*selector_offset>0)
      *selector_offset=0;
    else
    {
      if (*offset_first>lines)
        *offset_first-=lines;
      else
        *offset_first=0;
    }
  }
  Display_file_list(*offset_first,*selector_offset);
}


void Selector_end(short * offset_first,short * selector_offset)
{
  if (Filelist_nb_elements<10)
  {
    *offset_first=0;
    *selector_offset=Filelist_nb_elements-1;
  }
  else
  {
    *offset_first=Filelist_nb_elements-10;
    *selector_offset=9;
  }
  Display_file_list(*offset_first,*selector_offset);
}


void Selector_home(short * offset_first,short * selector_offset)
{
  Display_file_list((*offset_first)=0,(*selector_offset)=0);
}



short Compute_click_offset_in_fileselector(void)
/*
  Renvoie le d�calage dans le s�lecteur de fichier sur lequel on a click�.
  Renvoie le d�calage du dernier fichier si on a click� au del�.
  Renvoie -1 si le s�lecteur est vide.
*/
{
  short computed_offset;

  computed_offset=(((Mouse_Y-Window_pos_Y)/Menu_factor_Y)-95)>>3;
  if (computed_offset>=Filelist_nb_elements)
    computed_offset=Filelist_nb_elements-1;

  return computed_offset;
}

void Display_bookmark(T_Dropdown_button * Button, int bookmark_number)
{
  if (Config.Bookmark_directory[bookmark_number])
  {
    int label_size;
    // Libell�
    Print_in_window_limited(Button->Pos_X+3+10,Button->Pos_Y+2,Config.Bookmark_label[bookmark_number],8,MC_Black,MC_Light);
    label_size=strlen(Config.Bookmark_label[bookmark_number]);
    if (label_size<8)
      Window_rectangle(Button->Pos_X+3+10+label_size*8,Button->Pos_Y+2,(8-label_size)*8,8,MC_Light);
    // Menu apparait sur clic droit
    Button->Active_button=RIGHT_SIDE;
    // item actifs
    Window_dropdown_clear_items(Button);
    Window_dropdown_add_item(Button,0,"Set");
    Window_dropdown_add_item(Button,1,"Rename");
    Window_dropdown_add_item(Button,2,"Clear");    
  }
  else
  {
    // Libell�
    Print_in_window(Button->Pos_X+3+10,Button->Pos_Y+2,"--------",MC_Dark,MC_Light);
    // Menu apparait sur clic droit ou gauche
    Button->Active_button=RIGHT_SIDE|LEFT_SIDE;
    // item actifs
    Window_dropdown_clear_items(Button);
    Window_dropdown_add_item(Button,0,"Set");
  }
}

//------------------------ Chargements et sauvegardes ------------------------

void Print_current_directory(void)
//
// Affiche Main_current_directory sur 37 caract�res
//
{
  char temp_name[MAX_DISPLAYABLE_PATH+1]; // Nom tronqu�
  int  length; // length du r�pertoire courant
  int  index;   // index de parcours de la chaine compl�te

  Window_rectangle(10,84,37*8,8,MC_Light);

  length=strlen(Main_current_directory);
  if (length>MAX_DISPLAYABLE_PATH)
  { // Doh! il va falloir tronquer le r�pertoire (bouh !)

    // On commence par copier b�tement les 3 premiers caract�res (e.g. "C:\")
    for (index=0;index<3;index++)
      temp_name[index]=Main_current_directory[index];

    // On y rajoute 3 petits points:
    strcpy(temp_name+3,"...");

    //  Ensuite, on cherche un endroit � partir duquel on pourrait loger tout
    // le reste de la chaine (Ouaaaaaah!!! Vachement fort le mec!!)
    for (index++;index<length;index++)
      if ( (Main_current_directory[index]==PATH_SEPARATOR[0]) &&
           (length-index<=MAX_DISPLAYABLE_PATH-6) )
      {
        // Ouf: on vient de trouver un endroit dans la cha�ne � partir duquel
        // on peut faire la copie:
        strcpy(temp_name+6,Main_current_directory+index);
        break;
      }

    // Enfin, on peut afficher la cha�ne tronqu�e
    Print_in_window(10,84,temp_name,MC_Black,MC_Light);
  }
  else // Ahhh! La cha�ne peut loger tranquillement dans la fen�tre
    Print_in_window(10,84,Main_current_directory,MC_Black,MC_Light);
    
  Update_window_area(10,84,37*8,8);
}


void Print_filename_in_fileselector(void)
//
// Affiche Main_filename dans le Fileselect
//
{
  Window_rectangle(82,48,27*8,8,MC_Light);
  Print_in_window_limited(82,48,Main_filename,27,MC_Black,MC_Light);
  Update_window_area(82,48,27*8,8);
}

int   Selected_type; // Utilis� pour m�moriser le type d'entr�e choisi
                        // dans le selecteur de fichier.

void Prepare_and_display_filelist(short Position, short offset,
                                         T_Scroller_button * button)
{
  button->Nb_elements=Filelist_nb_elements;
  button->Position=Position;
  Compute_slider_cursor_height(button);
  Window_draw_slider(button);
  // On efface les anciens noms de fichier:
  Window_rectangle(8-1,95-1,144+2,80+2,MC_Black);
  // On affiche les nouveaux:
  Display_file_list(Position,offset);

  Update_window_area(8-1,95-1,144+2,80+2);

  // On r�cup�re le nom du schmilblick � "acc�der"
  Get_selected_item(Position,offset,Main_filename,&Selected_type);
  // On affiche le nouveau nom de fichier
  Print_filename_in_fileselector();
  // On affiche le nom du r�pertoire courant
  Print_current_directory();
}


void Reload_list_of_files(byte filter, short Position, short offset,
                           T_Scroller_button * button)
{
  Read_list_of_files(filter);
  Sort_list_of_files();
  Prepare_and_display_filelist(Position,offset,button);
}

void Scroll_fileselector(T_Scroller_button * file_scroller)
{
  char old_filename[MAX_PATH_CHARACTERS];

  strcpy(old_filename,Main_filename);

  // On regarde si la liste a boug�
  if (file_scroller->Position!=Main_fileselector_position)
  {
    // Si c'est le cas, il faut mettre � jour la jauge
    file_scroller->Position=Main_fileselector_position;
    Window_draw_slider(file_scroller);
  }
  // On r�cup�re le nom du schmilblick � "acc�der"
  Get_selected_item(Main_fileselector_position,Main_fileselector_offset,Main_filename,&Selected_type);
  if (strcmp(old_filename,Main_filename))
    New_preview_is_needed=1;

  // On affiche le nouveau nom de fichier
  Print_filename_in_fileselector();
  Display_cursor();
}


short Find_file_in_fileselector(char * fname)
{
  T_Fileselector_item * current_item;
  short  index;

  for (index=0, current_item=Filelist;
       ((current_item!=NULL) && (strcmp(current_item->Full_name,fname)));
       index++,current_item=current_item->Next);

  return (current_item!=NULL)?index:0;
}


void Highlight_file(char * fname)
{
  short index;

  index=Find_file_in_fileselector(fname);

  if ((Filelist_nb_elements<=10) || (index<5))
  {
    Main_fileselector_position=0;
    Main_fileselector_offset=index;
  }
  else
  {
    if (index>=Filelist_nb_elements-5)
    {
      Main_fileselector_position=Filelist_nb_elements-10;
      Main_fileselector_offset=index-Main_fileselector_position;
    }
    else
    {
      Main_fileselector_position=index-4;
      Main_fileselector_offset=4;
    }
  }
}


char FFF_best_name[MAX_PATH_CHARACTERS];
char * Find_filename_match(char * fname)
{
  char * best_name_ptr;
  T_Fileselector_item * current_item;
  byte   matching_letters=0;
  byte   counter;

  strcpy(FFF_best_name,Main_filename);
  best_name_ptr=NULL;

  for (current_item=Filelist; current_item!=NULL; current_item=current_item->Next)
  {
    if ( (!Config.Find_file_fast)
      || (Config.Find_file_fast==(current_item->Type+1)) )
    {
      // On compare et si c'est mieux, on stocke dans Meilleur_nom
      for (counter=0; fname[counter]!='\0' && tolower(current_item->Full_name[counter])==tolower(fname[counter]); counter++);
      if (counter>matching_letters)
      {
        matching_letters=counter;
        strcpy(FFF_best_name,current_item->Full_name);
        best_name_ptr=current_item->Full_name;
      }
    }
  }

  return best_name_ptr;
}

byte Button_Load_or_Save(byte load, byte image)
  // load=1 => On affiche le menu du bouton LOAD
  // load=0 => On affiche le menu du bouton SAVE
{
  short clicked_button;
  T_Scroller_button * file_scroller;
  T_Dropdown_button * formats_dropdown;
  T_Dropdown_button * bookmark_dropdown[4];
  short temp;
  int dummy=0;       // Sert � appeler SDL_GetKeyState
  byte  save_or_load_image=0;
  byte  has_clicked_ok=0;// Indique si on a click� sur Load ou Save ou sur
                             //un bouton enclenchant Load ou Save juste apr�s.
  T_Components * initial_palette; // |  Donn�es concernant l'image qui
  byte  initial_image_is_modified;         // |  sont m�moris�es pour pouvoir
  short initial_image_width;          // |- �tre restaur�es en sortant,
  short initial_image_height;          // |  parce que la preview elle les
  byte  old_back_color;             // |  fout en l'air (c'te conne).
  char  initial_filename[MAX_PATH_CHARACTERS]; // Sert � laisser le nom courant du fichier en cas de sauvegarde
  char  previous_directory[MAX_PATH_CHARACTERS]; // R�pertoire d'o� l'on vient apr�s un CHDIR
  char  initial_comment[COMMENT_SIZE+1];
  char  quicksearch_filename[MAX_PATH_CHARACTERS]="";
  char  save_filename[MAX_PATH_CHARACTERS];
  char * most_matching_filename;

  initial_palette=(T_Components *)malloc(sizeof(T_Palette));
  memcpy(initial_palette,Main_palette,sizeof(T_Palette));

  old_back_color=Back_color;
  initial_image_is_modified=Main_image_is_modified;
  initial_image_width=Main_image_width;
  initial_image_height=Main_image_height;
  strcpy(initial_filename,Main_filename);
  strcpy(initial_comment,Main_comment);
  if (load)
  {
    if (image)
      Open_window(310,200,"Load picture");
    else
      Open_window(310,200,"Load brush");
    Window_set_normal_button(198,180,51,14,"Load",0,1,SDLK_RETURN); // 1
  }
  else
  {
    if (image)
      Open_window(310,200,"Save picture");
    else
      Open_window(310,200,"Save brush");
    Window_set_normal_button(198,180,51,14,"Save",0,1,SDLK_RETURN); // 1
    if (Main_format==FORMAT_ANY) // Correction du *.*
    {
      Main_format=Main_fileformat;
      Main_fileselector_position=0;
      Main_fileselector_offset=0;
    }

    if (Main_format>NB_FORMATS_SAVE) // Correction d'un format insauvable
    {
      Main_format=DEFAULT_FILEFORMAT;
      Main_fileselector_position=0;
      Main_fileselector_offset=0;
    }
    // Affichage du commentaire
    if (File_formats[Main_format-1].Comment)
      Print_in_window(47,70,Main_comment,MC_Black,MC_Light);
  }

  Window_set_normal_button(253,180,51,14,"Cancel",0,1,KEY_ESC); // 2
  Window_set_normal_button(7,180,51,14,"Delete",0,1,SDLK_DELETE); // 3

  // Frame autour des infos sur le fichier de dessin
  Window_display_frame_in(6, 44,299, 37);
  // Frame autour de la preview
  Window_display_frame_in(181,93,124,84);
  // Frame autour du fileselector
  Window_display_frame_in(6,93,148,84);

  // Fileselector
  Window_set_special_button(9,95,144,80); // 4

  // Scroller du fileselector
  file_scroller = Window_set_scroller_button(160,94,82,1,10,0);               // 5

  // Dropdown pour les formats de fichier
  formats_dropdown=
    Window_set_dropdown_button(69,28,49,11,0,
      (Main_format==FORMAT_ANY)?"*.*":File_formats[Main_format-1].Extension,
      1,0,1,RIGHT_SIDE|LEFT_SIDE); // 6
  if (load)
    Window_dropdown_add_item(formats_dropdown,0,"*.*");
  for (temp=0;temp<NB_KNOWN_FORMATS;temp++)
  {
    if ((load && File_formats[temp].Load) || 
      (!load && File_formats[temp].Save))
        Window_dropdown_add_item(formats_dropdown,temp+1,File_formats[temp].Extension);
  }
  Print_in_window(70,18,"Format",MC_Dark,MC_Light);
  
  // Texte de commentaire des dessins
  Print_in_window(9,70,"Txt:",MC_Dark,MC_Light);
  Window_set_input_button(43,68,COMMENT_SIZE); // 7

  // Saisie du nom de fichier
  Window_set_input_button(80,46,27); // 8

  Print_in_window(9,47,"Filename",MC_Dark,MC_Light);
  Print_in_window(9,59,"Image:",MC_Dark,MC_Light);
  Print_in_window(101,59,"Size:",MC_Dark,MC_Light);
  Print_in_window(228,59,"(",MC_Dark,MC_Light);
  Print_in_window(292,59,")",MC_Dark,MC_Light);

  // Selecteur de Lecteur / Volume
  Window_set_normal_button(7,18,53,23,"",0,1,SDLK_LAST); // 9
  Print_in_window(10,22,"Select",MC_Black,MC_Light);
  Print_in_window(14,30,"drive",MC_Black,MC_Light);
 
  // Bookmarks
  for (temp=0;temp<NB_BOOKMARKS;temp++)
  {
    bookmark_dropdown[temp]=
      Window_set_dropdown_button(127+(88+1)*(temp%2),18+(temp/2)*12,88,11,56,"",0,0,1,RIGHT_SIDE); // 10-13
    Window_display_icon_sprite(bookmark_dropdown[temp]->Pos_X+3,bookmark_dropdown[temp]->Pos_Y+2,ICON_STAR);
    Display_bookmark(bookmark_dropdown[temp],temp);
  }
  // On prend bien soin de passer dans le r�pertoire courant (le bon qui faut! Oui madame!)
  if (load)
  {
    chdir(Main_current_directory);
    getcwd(Main_current_directory,256);
  }
  else
  {
    chdir(Main_file_directory);
    getcwd(Main_current_directory,256);
  }
  
  // Affichage des premiers fichiers visibles:
  Reload_list_of_files(Main_format,Main_fileselector_position,Main_fileselector_offset,file_scroller);

  if (!load)
  {
    // On initialise le nom de fichier � celui en cours et non pas celui sous
    // la barre de s�lection
    strcpy(Main_filename,initial_filename);
    // On affiche le nouveau nom de fichier
    Print_filename_in_fileselector();
  }

  Pixel_load_function=Pixel_load_in_preview;
  New_preview_is_needed=1;
  Update_window_area(0,0,Window_width, Window_height);

  Display_cursor();

  do
  {
    clicked_button=Window_clicked_button();

    switch (clicked_button)
    {
      case -1 :
      case  0 :
        break;

      case  1 : // Load ou Save
      if(load)
        {
          // Determine the type
          if(File_exists(Main_filename)) 
          {
            Selected_type = 0;
            if(Directory_exists(Main_filename)) Selected_type = 1;
          }
          else
          {
            Selected_type = 1;
          }
        }
        else
        {
          if(Directory_exists(Main_filename)) Selected_type = 1;
          else Selected_type = 0;
        }
        has_clicked_ok=1;
        break;

      case  2 : // Cancel
        break;

      case  3 : // Delete
        if (Filelist_nb_elements && (*Main_filename!='.') && Selected_type!=2)
        {
          char * message;
          Hide_cursor();
          // On affiche une demande de confirmation
          if (Main_fileselector_position+Main_fileselector_offset>=Filelist_nb_directories)
          {
            message="Delete file ?";
          }
          else
          {
            message="Remove directory ?";
          }
          if (Confirmation_box(message))
          {
            // Si c'est un fichier
            if (Main_fileselector_position+Main_fileselector_offset>=Filelist_nb_directories)
              // On efface le fichier (si on peut)
              temp=(!remove(Main_filename));
            else // Si c'est un repertoire
              // On efface le repertoire (si on peut)
              temp=(!rmdir(Main_filename));

            if (temp) // temp indique si l'effacement s'est bien pass�
            {
              // On remonte si c'�tait le dernier �l�ment de la liste
              if (Main_fileselector_position+Main_fileselector_offset==Filelist_nb_elements-1)
              {
                if (Main_fileselector_position)
                  Main_fileselector_position--;
                else
                  if (Main_fileselector_offset)
                    Main_fileselector_offset--;
              }
              else // Si ce n'�tait pas le dernier, il faut faire gaffe � ce
              {    // que ses copains d'en dessous ne remontent pas trop.
                if ( (Main_fileselector_position)
                  && (Main_fileselector_position+10==Filelist_nb_elements) )
                  {
                    Main_fileselector_position--;
                    Main_fileselector_offset++;
                  }
              }
              // On relit les informations
              Reload_list_of_files(Main_format,Main_fileselector_position,Main_fileselector_offset,file_scroller);
              // On demande la preview du nouveau fichier sur lequel on se trouve
              New_preview_is_needed=1;
            }
            else
              Error(0);

            // On place la barre de s�lection du brouillon au d�but s'il a le
            // m�me r�pertoire que l'image principale.
            if (!strcmp(Main_current_directory,Spare_current_directory))
            {
              Spare_fileselector_position=0;
              Spare_fileselector_offset=0;
            }
          }
        }
        break;

      case  4 : // Zone d'affichage de la liste de fichiers
        Hide_cursor();

        temp=Compute_click_offset_in_fileselector();
        if (temp>=0)
        {
          if (temp!=Main_fileselector_offset)
          {
            // On met � jour le d�calage
            Main_fileselector_offset=temp;

            // On r�cup�re le nom du schmilblick � "acc�der"
            Get_selected_item(Main_fileselector_position,Main_fileselector_offset,Main_filename,&Selected_type);
            // On affiche le nouveau nom de fichier
            Print_filename_in_fileselector();
            // On affiche � nouveau la liste
            Display_file_list(Main_fileselector_position,Main_fileselector_offset);

            // On vient de changer de nom de fichier, donc on doit s'appreter
            // a rafficher une preview
            New_preview_is_needed=1;
            *quicksearch_filename=0;
          }
          else
          {
            //   En sauvegarde, si on a double-click� sur un r�pertoire, il
            // faut mettre le nom de fichier au nom du r�pertoire. Sinon, dans
            // certains cas, on risque de sauvegarder avec le nom du fichier
            // actuel au lieu de changer de r�pertoire.
            if (Main_fileselector_position+Main_fileselector_offset<Filelist_nb_directories)
              Get_selected_item(Main_fileselector_position,Main_fileselector_offset,Main_filename,&Selected_type);

            has_clicked_ok=1;
            New_preview_is_needed=1;
            *quicksearch_filename=0;
          }
        }
        Display_cursor();
        Wait_end_of_click();
        break;

      case  5 : // Scroller de fichiers
        Hide_cursor();
        Main_fileselector_position=Window_attribute2;
        // On r�cup�re le nom du schmilblick � "acc�der"
        Get_selected_item(Main_fileselector_position,Main_fileselector_offset,Main_filename,&Selected_type);
        // On affiche le nouveau nom de fichier
        Print_filename_in_fileselector();
        // On affiche � nouveau la liste
        Display_file_list(Main_fileselector_position,Main_fileselector_offset);
        Display_cursor();
        New_preview_is_needed=1;
        *quicksearch_filename=0;
        break;

      case  6 : // Scroller des formats
        Hide_cursor();
        // On met � jour le format de browsing du fileselect:
        Main_format=Window_attribute2;
        // Comme on change de liste, on se place en d�but de liste:
        Main_fileselector_position=0;
        Main_fileselector_offset=0;
        // Affichage des premiers fichiers visibles:
        Reload_list_of_files(Main_format,Main_fileselector_position,Main_fileselector_offset,file_scroller);
        Display_cursor();
        New_preview_is_needed=1;
        *quicksearch_filename=0;
        break;
      case  7 : // Saisie d'un commentaire pour la sauvegarde
        if ( (!load) && (File_formats[Main_format-1].Comment) )
        {
          Readline(45,70,Main_comment,32,0);
          Display_cursor();
        }
        break;
      case  8 : // Saisie du nom de fichier

        // Save the filename
        strcpy(save_filename, Main_filename);

        if (Readline(82,48,Main_filename,27,2))
        {
          //   On regarde s'il faut rajouter une extension. C'est-�-dire s'il
          // n'y a pas de '.' dans le nom du fichier.
          for(temp=0,dummy=0; ((Main_filename[temp]) && (!dummy)); temp++)
            if (Main_filename[temp]=='.')
              dummy=1;
          if (!dummy)
          {
            if (Main_format != FORMAT_ANY)
            {
              if(!Directory_exists(Main_filename))
              {
                 strcat(Main_filename,".");
                 strcat(Main_filename,File_formats[Main_format-1].Extension);
              }
            }
            else
            {
              // put default extension
              // (but maybe we should browse through all available ones until we find
              //  something suitable ?)
              if(!Directory_exists(Main_filename))
              {
                 strcat(Main_filename, ".pkm");
              }
            }
          }
          if(load)
          {
            // Determine the type
            if(File_exists(Main_filename)) 
            {
              Selected_type = 0;
              if(Directory_exists(Main_filename)) Selected_type = 1;
            }
            else
            {
              Selected_type = 1;
            }
          }
          else
          {
            if(Directory_exists(Main_filename)) Selected_type = 1;
            else Selected_type = 0;
          }
          has_clicked_ok=1;
        }
        else
        {
          // Restore the old filename
          strcpy(Main_filename, save_filename);
          Print_filename_in_fileselector();
        }
        Display_cursor();
        break;
      case  9 : // Volume Select
          Hide_cursor();
          //   Comme on tombe sur un disque qu'on connait pas, on se place en
          // d�but de liste:
          Main_fileselector_position=0;
          Main_fileselector_offset=0;
          // Affichage des premiers fichiers visibles:
          Read_list_of_drives();
          Sort_list_of_files();
          Prepare_and_display_filelist(Main_fileselector_position,Main_fileselector_offset,file_scroller);
          Display_cursor();
          New_preview_is_needed=1;
          *quicksearch_filename=0;
          break;
      default:
          if (clicked_button>=10 && clicked_button<10+NB_BOOKMARKS)
          {
            // Bookmark
            char * directory_name;
            
            switch(Window_attribute2)
            {
              case -1: // bouton lui-m�me: aller au r�pertoire m�moris�
                if (Config.Bookmark_directory[clicked_button-10])
                {
                  *quicksearch_filename=0;
                  strcpy(Main_filename,Config.Bookmark_directory[clicked_button-10]);
                  Selected_type=1;
                  has_clicked_ok=1;
                  *quicksearch_filename=0;
                }
                break;
                
              case 0: // Set
                if (Config.Bookmark_directory[clicked_button-10])
                  free(Config.Bookmark_directory[clicked_button-10]);
                Config.Bookmark_label[clicked_button-10][0]='\0';
                temp=strlen(Main_current_directory);
                Config.Bookmark_directory[clicked_button-10]=malloc(temp+1);
                strcpy(Config.Bookmark_directory[clicked_button-10],Main_current_directory);
                
                directory_name=Find_last_slash(Main_current_directory);
                if (directory_name && directory_name[1]!='\0')
                  directory_name++;
                else
                  directory_name=Main_current_directory;
                temp=strlen(directory_name);
                strncpy(Config.Bookmark_label[clicked_button-10],directory_name,8);
                if (temp>8)
                {
                  Config.Bookmark_label[clicked_button-10][7]=ELLIPSIS_CHARACTER;
                  Config.Bookmark_label[clicked_button-10][8]='\0';
                }
                Display_bookmark(bookmark_dropdown[clicked_button-10],clicked_button-10);
                break;
                
              case 1: // Rename
                if (Config.Bookmark_directory[clicked_button-10])
                {
                  // On enl�ve les "..." avant l'�dition
                  char bookmark_label[8+1];
                  strcpy(bookmark_label, Config.Bookmark_label[clicked_button-10]);
                  if (bookmark_label[7]==ELLIPSIS_CHARACTER)
                    bookmark_label[7]='\0';
                  if (Readline_ex(bookmark_dropdown[clicked_button-10]->Pos_X+3+10,bookmark_dropdown[clicked_button-10]->Pos_Y+2,bookmark_label,8,8,0))
                    strcpy(Config.Bookmark_label[clicked_button-10],bookmark_label);
                  Display_bookmark(bookmark_dropdown[clicked_button-10],clicked_button-10);
                  Display_cursor();
                }
                break;

              case 2: // Clear
                if (Config.Bookmark_directory[clicked_button-10] && Confirmation_box("Erase bookmark ?"))
                {
                  free(Config.Bookmark_directory[clicked_button-10]);
                  Config.Bookmark_directory[clicked_button-10]=NULL;
                  Config.Bookmark_label[clicked_button-10][0]='\0';
                  Display_bookmark(bookmark_dropdown[clicked_button-10],clicked_button-10);
                }
                break;
            }
          }
          break;
    }

    switch (Key)
    {
      case SDLK_UNKNOWN : break;
      case SDLK_DOWN : // Bas
        *quicksearch_filename=0;
        Hide_cursor();
        Selector_scroll_down(&Main_fileselector_position,&Main_fileselector_offset);
        Scroll_fileselector(file_scroller);
        Key=0;
        break;
      case SDLK_UP : // Haut
        *quicksearch_filename=0;
        Hide_cursor();
        Selector_scroll_up(&Main_fileselector_position,&Main_fileselector_offset);
        Scroll_fileselector(file_scroller);
        Key=0;
        break;
      case SDLK_PAGEDOWN : // PageDown
        *quicksearch_filename=0;
        Hide_cursor();
        Selector_page_down(&Main_fileselector_position,&Main_fileselector_offset,9);
        Scroll_fileselector(file_scroller);
        Key=0;
        break;
      case SDLK_PAGEUP : // PageUp
        *quicksearch_filename=0;
        Hide_cursor();
        Selector_page_up(&Main_fileselector_position,&Main_fileselector_offset,9);
        Scroll_fileselector(file_scroller);
        Key=0;
        break;
      case SDLK_END : // End
        *quicksearch_filename=0;
        Hide_cursor();
        Selector_end(&Main_fileselector_position,&Main_fileselector_offset);
        Scroll_fileselector(file_scroller);
        Key=0;
        break;
      case SDLK_HOME : // Home
        *quicksearch_filename=0;
        Hide_cursor();
        Selector_home(&Main_fileselector_position,&Main_fileselector_offset);
        Scroll_fileselector(file_scroller);
        Key=0;
        break;
      case KEY_MOUSEWHEELDOWN :
        *quicksearch_filename=0;
        Hide_cursor();
        Selector_page_down(&Main_fileselector_position,&Main_fileselector_offset,3);
        Scroll_fileselector(file_scroller);
        Key=0;
        break;
      case KEY_MOUSEWHEELUP :
        *quicksearch_filename=0;
        Hide_cursor();
        Selector_page_up(&Main_fileselector_position,&Main_fileselector_offset,3);
        Scroll_fileselector(file_scroller);
        Key=0;
        break;
      case SDLK_BACKSPACE : // Backspace
        *quicksearch_filename=0;
        // Si le choix ".." est bien en t�te des propositions...
        if (!strcmp(Filelist->Full_name,PARENT_DIR))
        {                              
          // On va dans le r�pertoire parent.
          strcpy(Main_filename,PARENT_DIR);
          Selected_type=1;
          has_clicked_ok=1;
        }
        Key=0;
        break;
      default: // Autre => On se place sur le nom de fichier qui correspond
        if (clicked_button<=0)
        {
          if (Is_shortcut(Key,0x100+BUTTON_HELP))
          {
            Window_help(load?BUTTON_LOAD:BUTTON_SAVE, NULL);
            break;
          }
          temp=strlen(quicksearch_filename);
          if (Key_ANSI>= ' ' && Key_ANSI < 255 && temp<50)
          {
            quicksearch_filename[temp]=Key_ANSI;
            quicksearch_filename[temp+1]='\0';
            most_matching_filename=Find_filename_match(quicksearch_filename);
            if ( (most_matching_filename) )
            {
              temp=Main_fileselector_position+Main_fileselector_offset;
              Hide_cursor();
              Highlight_file(most_matching_filename);
              Prepare_and_display_filelist(Main_fileselector_position,Main_fileselector_offset,file_scroller);
              Display_cursor();
              if (temp!=Main_fileselector_position+Main_fileselector_offset)
                New_preview_is_needed=1;
            }
            else
              *quicksearch_filename=0;
            Key=0;
          }
        }
        else
          *quicksearch_filename=0;
    }

    if (has_clicked_ok)
    {
      //   Si c'est un r�pertoire, on annule "has_clicked_ok" et on passe
      // dedans.
      if (Selected_type!=0)
      {
        Hide_cursor();
        has_clicked_ok=0;

        // On m�morise le r�pertoire dans lequel on �tait
        if (strcmp(Main_filename,PARENT_DIR))
          strcpy(previous_directory,Format_filename(PARENT_DIR, 1));
        else
        {
          strcpy(previous_directory,
            Format_filename(Find_last_slash(Main_current_directory), 1)
            );
        }

        // On doit rentrer dans le r�pertoire:
        if (!chdir(Main_filename))
        {
          getcwd(Main_current_directory,256);
  
          // On lit le nouveau r�pertoire
          Read_list_of_files(Main_format);
          Sort_list_of_files();
          // On place la barre de s�lection sur le r�pertoire d'o� l'on vient
          Highlight_file(previous_directory);
        }
        else
          Error(0);
        // Affichage des premiers fichiers visibles:
        Prepare_and_display_filelist(Main_fileselector_position,Main_fileselector_offset,file_scroller);
        Display_cursor();
        New_preview_is_needed=1;
      }
      else  // Sinon on essaye de charger ou sauver le fichier
      {
        strcpy(Main_file_directory,Main_current_directory);
        if (!load)
          Main_fileformat=Main_format;
        save_or_load_image=1;
      }
    }

    // Gestion du chrono et des previews
    if (New_preview_is_needed)
    {
      // On efface les infos de la preview pr�c�dente s'il y en a une
      // d'affich�e
      if (Timer_state==2)
      {
        Hide_cursor();
        // On efface le commentaire pr�c�dent
        Window_rectangle(45,70,32*8,8,MC_Light);
        // On nettoie la zone o� va s'afficher la preview:
        Window_rectangle(183,95,120,80,MC_Light);
        // On efface les dimensions de l'image
        Window_rectangle(143,59,72,8,MC_Light);
        // On efface la taille du fichier
        Window_rectangle(236,59,56,8,MC_Light);
        // On efface le format du fichier
        Window_rectangle(59,59,3*8,8,MC_Light);
        // Affichage du commentaire
        if ( (!load) && (File_formats[Main_format-1].Comment) )
        {
          Print_in_window(45,70,Main_comment,MC_Black,MC_Light);
        }
        Display_cursor();
        // Un update pour couvrir les 4 zones: 3 libell�s plus le commentaire
        Update_window_area(45,48,256,30);
        // Zone de preview
        Update_window_area(183,95,120,80);
      }

      New_preview_is_needed=0;
      Timer_state=0;         // State du chrono = Attente d'un X�me de seconde
      // On lit le temps de d�part du chrono
      Init_chrono(Config.Timer_delay);
    }

    if (!Timer_state)  // Prendre une nouvelle mesure du chrono et regarder
      Check_timer(); // s'il ne faut pas afficher la preview

    if (Timer_state==1) // Il faut afficher la preview
    {
      if ( (Main_fileselector_position+Main_fileselector_offset>=Filelist_nb_directories) && (Filelist_nb_elements) )
      {
        strcpy(Main_file_directory,Main_current_directory);

        Hide_cursor();
        Load_image(image);
        //Update_window_area(183,95,120,80);
        Update_window_area(0,0,Window_width,Window_height);
        Display_cursor();

        // Apr�s le chargement de la preview, on restaure tout ce qui aurait
        // pu �tre modifi� par le chargement de l'image:
        memcpy(Main_palette,initial_palette,sizeof(T_Palette));
        Main_image_is_modified=initial_image_is_modified;
        Main_image_width=initial_image_width;
        Main_image_height=initial_image_height;
      }

      Timer_state=2; // On arr�te le chrono
    }
  }
  while ( (!has_clicked_ok) && (clicked_button!=2) );

  // Si on annule, on restaure l'ancien commentaire
  if (clicked_button==2)
    strcpy(Main_comment,initial_comment);

  //   On restaure les donn�es de l'image qui ont certainement �t� modifi�es
  // par la preview.
  memcpy(Main_palette,initial_palette,sizeof(T_Palette));
  Set_palette(Main_palette);
  Back_color=old_back_color;
  Main_image_is_modified=initial_image_is_modified;
  Main_image_width=initial_image_width;
  Main_image_height=initial_image_height;
  Set_palette(Main_palette);

  Compute_optimal_menu_colors(Main_palette);
  temp=(Window_pos_Y+(Window_height*Menu_factor_Y)<Menu_Y_before_window);

  Close_window();

  if (temp)
    Display_menu();

  Unselect_button((load)?BUTTON_LOAD:BUTTON_SAVE);
  Display_cursor();
  Free_fileselector_list();

  Pixel_load_function=Pixel_load_in_current_screen;

  free(initial_palette);

  return save_or_load_image;
}
