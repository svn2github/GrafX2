/* vim:expandtab:ts=2 sw=2:
*/
/*  Grafx2 - The Ultimate 256-color bitmap paint program

    Copyright 2011 Pawel G�ralski
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

#include <SDL_syswm.h>

#if defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__) || defined(__amigaos__)
    #include <proto/dos.h>
    #include <sys/types.h>
    #include <dirent.h>
#elif defined (__MINT__)
    #include <mint/sysbind.h>
    #include <dirent.h>
#elif defined(__WIN32__)
    #include <dirent.h>
    #include <windows.h>
    #include <commdlg.h>
#else
    #include <dirent.h>
#endif

#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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

#define NORMAL_FILE_COLOR    MC_Light // color du texte pour une ligne de
	// fichier non s�lectionn�
#define NORMAL_DIRECTORY_COLOR MC_Dark // color du texte pour une ligne de
	// r�pertoire non s�lectionn�
#define NORMAL_BACKGROUND_COLOR       MC_Black  // color du fond  pour une ligne
	// non s�lectionn�e
#define SELECTED_FILE_COLOR    MC_White // color du texte pour une ligne de
	// fichier s�lectionn�e
#define SELECTED_DIRECTORY_COLOR MC_Light // color du texte pour une ligne de
	// rep�rtoire s�lectionn�e
#define SELECTED_BACKGROUND_COLOR       MC_Dark // color du fond  pour une ligne
	// s�lectionn�e

// -- Native fileselector for WIN32

// Returns 0 if all ok, something else if failed
byte Native_filesel(byte load)
{
	//load = load;
#ifdef __WIN32__
  OPENFILENAME ofn;
  char szFileName[MAX_PATH] = "";
	SDL_SysWMinfo wminfo;
	HWND hwnd;
	
	SDL_VERSION(&wminfo.version);
	SDL_GetWMInfo(&wminfo);
	hwnd = wminfo.window;

  ZeroMemory(&ofn, sizeof(ofn));

  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = hwnd;
  ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
  ofn.lpstrFile = szFileName;
  ofn.nMaxFile = MAX_PATH;
  ofn.Flags = OFN_EXPLORER;
  if(load) ofn.Flags |= OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
  ofn.lpstrDefExt = "txt";

  if(load)
  {
    if (GetOpenFileName(&ofn))
      // Do something usefull with the filename stored in szFileName 
      return 0;
    else
      // error - check if its just user pressing cancel or something else
      return CommDlgExtendedError();
  } else if(GetSaveFileName(&ofn)) {
    return 0;
  } else {
    // Check if cancel
    return CommDlgExtendedError();
  }
#else
  #ifndef __linux__ // This makes no sense on X11-oriented platforms. Nothing is really native there.
    #warning "EXPERIMENTAL function for native fileselector not available for this platform!"
  #endif
    return 255; // fail !
#endif
}  

// -- "Standard" fileselector for other platforms

// -- Fileselector data

T_Fileselector Filelist;

/// Name of the current directory
//static char Selector_directory[1024];
/// Filename (without directory) of the highlighted file
static char Selector_filename[256];

// Conventions:
//
// * Le fileselect modifie le r�pertoire courant. Ceci permet de n'avoir
//   qu'un findfirst dans le r�pertoire courant � faire:

void Recount_files(T_Fileselector *list)
{
  T_Fileselector_item *item;

  list->Nb_files=0;
  list->Nb_directories=0;
  list->Nb_elements=0;
  
  for (item = list->First; item != NULL; item = item->Next)
  {
    if (item->Type == 0)
      list->Nb_files ++;
    else
      list->Nb_directories ++;
    list->Nb_elements ++;
  }
  
  if (list->Index)
  {
    free(list->Index);
    list->Index = NULL;
  }
  
  if (list->Nb_elements>0)
  {
    int i;
    
    list->Index = (T_Fileselector_item **) malloc(list->Nb_elements * sizeof(T_Fileselector_item **));
    if (list->Index)
    {
      // Fill the index
      for (item = list->First, i=0; item != NULL; item = item->Next, i++)
      {
        list->Index[i] = item;
      }
    }
    // If the malloc failed, we're probably in trouble, but I don't know
    // how to recover from that..I'll just make the index bulletproof.
  }
}

// -- Destruction de la liste cha�n�e ---------------------------------------
void Free_fileselector_list(T_Fileselector *list)
//  Cette proc�dure d�truit la chaine des fichiers. Elle doit �tre appel�e
// avant de rappeler la fonction Read_list_of_files, ainsi qu'en fin de
// programme.
{
  // Pointeur temporaire de destruction
  T_Fileselector_item * temp_item;

  while (list->First!=NULL)
  {
    // On m�morise l'adresse du premier �l�ment de la liste
    temp_item =list->First;
    // On fait avancer la t�te de la liste
    list->First=list->First->Next;
    // Et on efface l'ancien premier �l�ment de la liste
    free(temp_item);
    temp_item = NULL;
  }
  Recount_files(list);
}

char * Format_filename(const char * fname, word max_length, int type)
{
  static char result[40];
  int         c;
  int         other_cursor;
  int         pos_last_dot;

  // safety
  if (max_length>40)
    max_length=40;
  
  if (strcmp(fname,PARENT_DIR)==0)
  {
    strcpy(result,"<-PARENT DIRECTORY");
    // Append spaces
    for (c=18; c<max_length-1; c++)
      result[c]=' ';
    result[c]='\0';
  }
  else if (fname[0]=='.' || type==1 || type==2)
  {
    // Files ".something" or drives or directories: Aligned left on (max_length-1) chars max
    // Initialize as all spaces
    for (c=0; c<max_length-1; c++)
      result[c]=' ';
    result[c]='\0';
    
    for (c=0;fname[c]!='\0' && c < max_length-1;c++)
      result[c]=fname[c];
    // A special character indicates the filename is truncated
    if (c >= max_length-1)
      result[max_length-2]=ELLIPSIS_CHARACTER;
  }
  else
  {
    // Initialize as all spaces
    for (c = 0; c<max_length-1; c++)
      result[c]=' ';
    result[c]='\0';
       
    result[max_length-5]='.';
    
    // Look for the last dot in filename
    pos_last_dot = -1;
    for (c = 0; fname[c]!='\0'; c++)
      if (fname[c]=='.')
        pos_last_dot = c;

    // Copy the part before the dot
    for (c=0; c!=pos_last_dot && fname[c]!='\0'; c++)
    {
      if (c > max_length-6)
      {
        result[max_length-6]=ELLIPSIS_CHARACTER;
        break;
      }
      result[c]=fname[c];
    }

    // Ensuite on recopie la partie qui suit le point (si n�cessaire):
    if (pos_last_dot != -1)
    {
      for (c = pos_last_dot+1,other_cursor=max_length-4;fname[c]!='\0' && other_cursor < max_length-1;c++,other_cursor++)
        result[other_cursor]=fname[c];
    }
  }
  return result;
}


// -- Rajouter a la liste des elements de la liste un element ---------------
void Add_element_to_list(T_Fileselector *list, const char * full_name, const char *short_name, int type, byte icon)
//  Cette procedure ajoute a la liste chainee un fichier pass� en argument.
{
  // Working element
  T_Fileselector_item * temp_item;

  // Allocate enough room for one struct + the visible label
  temp_item=(T_Fileselector_item *)malloc(sizeof(T_Fileselector_item)+strlen(short_name));

  // Initialize element
  strcpy(temp_item->Short_name,short_name);
  strcpy(temp_item->Full_name,full_name);
  temp_item->Type = type;
  temp_item->Icon = icon;

  // Doubly-linked
  temp_item->Next    =list->First;
  temp_item->Previous=NULL;

  if (list->First!=NULL)
    list->First->Previous=temp_item;
    
  // Put new element at the beginning
  list->First=temp_item;
}

///
/// Checks if a file has the requested file extension.
/// The extension string can end with a ';' (remainder is ignored)
/// This function allows wildcard '?', and '*' if it's the only character.
int Check_extension(const char *filename, const char * filter)
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
    return (filter[0] == '\0' || filter[0] == ';');

  // V�rification caract�re par caract�re, case-insensitive.
  c = 0;
  while (1)
  {
    if (filter[c] == '\0' || filter[c] == ';')
      return filename[pos_last_dot + 1 + c] == '\0';
    
    if (filter[c] != '?' &&
      tolower(filter[c]) != tolower(filename[pos_last_dot + 1 + c]))
      return 0;

     c++;
  }
}


// -- Lecture d'une liste de fichiers ---------------------------------------
void Read_list_of_files(T_Fileselector *list, byte selected_format)
//  Cette proc�dure charge dans la liste chain�e les fichiers dont l'extension
// correspond au format demand�.
{
  DIR*  current_directory; //R�pertoire courant
  struct dirent* entry; // Structure de lecture des �l�ments
  char * filter = "*"; // Extension demand�e
  struct stat Infos_enreg;
  char * current_path;

  // Tout d'abord, on d�duit du format demand� un filtre � utiliser:
  filter = Get_fileformat(selected_format)->Extensions;

  // Ensuite, on vide la liste actuelle:
  Free_fileselector_list(list);
  // Apr�s effacement, il ne reste ni fichier ni r�pertoire dans la liste

  // On lit tous les r�pertoires:

#if defined (__MINT__)
  static char path[1024];
  static char path2[1024];
  path[0]='\0';
  path2[0]='\0';
  
  char currentDrive='A';
  currentDrive=currentDrive+Dgetdrv(); 
  
  Dgetpath(path,0);
  sprintf(path2,"%c:\%s",currentDrive,path);
 
  strcat(path2,PATH_SEPARATOR);
  current_directory=opendir(path2);
#else  
  current_path=getcwd(NULL,0);
  current_directory=opendir(current_path);
#endif
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
     !File_is_hidden(entry->d_name, entry->d_name)))
    {
      // On rajoute le r�pertoire � la liste
      Add_element_to_list(list, entry->d_name, Format_filename(entry->d_name, 19, 1), 1, ICON_NONE);
      list->Nb_directories++;
    }
    else if (S_ISREG(Infos_enreg.st_mode) && //Il s'agit d'un fichier
      (Config.Show_hidden_files || //Il n'est pas cach�
      !File_is_hidden(entry->d_name, entry->d_name)))
    {
      const char * ext = filter;
      while (ext!=NULL)
      {      
        if (Check_extension(entry->d_name, ext))
        {
          // On rajoute le fichier � la liste
          Add_element_to_list(list, entry->d_name, Format_filename(entry->d_name, 19, 0), 0, ICON_NONE);
          list->Nb_files++;
          // Stop searching
          ext=NULL;
        }
        else
        {
          ext = strchr(ext, ';');
          if (ext)
            ext++;
        }
      }
    }
  }

#if defined(__MORPHOS__) || defined(__AROS__) || defined (__amigaos4__) || defined(__amigaos__)
  Add_element_to_list(list, "/", Format_filename("/",19,1), 1, ICON_NONE); // on amiga systems, / means parent. And there is no ..
  list->Nb_directories ++;
#elif defined (__MINT__)
  T_Fileselector_item *item=NULL;
  // check if ".." exists if not add it
  // FreeMinT lists ".." already, but this is not so for TOS 
  // simply adding it will cause double PARENT_DIR under FreeMiNT
  
  bool bFound= false;
  
  for (item = list->First; (((item != NULL) && (bFound==false))); item = item->Next){
    if (item->Type == 1){
	if(strncmp(item->Full_name,"..",(sizeof(char)*2))==0) bFound=true;
    }
  }
  
  if(!bFound){
    Add_element_to_list(list, "..",Format_filename("/",19,1),1,ICON_NONE); // add if not present
    list->Nb_directories ++;  
  }
  
#endif

  closedir(current_directory);
#if defined (__MINT__)  

#else
  free(current_path);
#endif
  current_path = NULL;

  Recount_files(list);
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
void Read_list_of_drives(T_Fileselector *list, byte name_length)
{

  // Empty the current content of fileselector:
  Free_fileselector_list(list);
  // Reset number of items
  list->Nb_files=0;
  list->Nb_directories=0;

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
        Add_element_to_list(list, tmp, Format_filename(tmp, name_length, 2), 2, ICON_NONE );
        list->Nb_directories++;
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
    byte icon;
    
    // Sous Windows, on a la totale, presque aussi bien que sous DOS:
    drive_index = 0;
    for (bit_index=0; bit_index<26 && drive_index<23; bit_index++)
    {
      if ( (1 << bit_index) & drive_bits )
      {
        // On a ce lecteur, il faut maintenant d�terminer son type "physique".
        // pour profiter des jolies icones de X-man.
        char drive_path[]="A:\\";
        // Cette API Windows est �trange, je dois m'y faire...
        drive_path[0]='A'+bit_index;
        switch (GetDriveType(drive_path))
        {
          case DRIVE_CDROM:
            icon=ICON_CDROM;
            break;
          case DRIVE_REMOTE:
            icon=ICON_NETWORK;
            break;
          case DRIVE_REMOVABLE:
            icon=ICON_FLOPPY_3_5;
            break;
          case DRIVE_FIXED:
            icon=ICON_HDD;
            break;
          default:
            icon=ICON_NETWORK;
            break;
        }
        drive_name[0]='A'+bit_index;
        Add_element_to_list(list, drive_name, Format_filename(drive_name,name_length-1,2), 2, icon);
        list->Nb_directories++;
        drive_index++;
      }
    }
  }
  #elif defined(__MINT__)
    char drive_name[]="A:\\";
    unsigned long drive_bits = Drvmap(); //get drive map bitfield
    int drive_index;
    int bit_index;
    drive_index = 0;
    for (bit_index=0; bit_index<32; bit_index++)
    {
      if ( (1 << bit_index) & drive_bits )
      {
        drive_name[0]='A'+bit_index;
        Add_element_to_list(list, drive_name,Format_filename(drive_name,name_length,2),2,ICON_NONE);
        list->Nb_directories++;
        drive_index++;
      }
    }
  
  #else
  {
    //Sous les diff�rents unix, on va mettre
    // un disque dur qui pointera vers la racine,
    // et un autre vers le home directory de l'utilisateur.

    // Ensuite on utilise read_file_system_list pour compl�ter

    struct mount_entry* mount_points_list;
    struct mount_entry* next;

    #if defined(__BEOS__) || defined(__HAIKU__)
        char * home_dir = getenv("$HOME");
    #else
        char * home_dir = getenv("HOME");
    #endif
    Add_element_to_list(list, "/", Format_filename("/",name_length,2), 2, ICON_NONE);
    list->Nb_directories++;
    if(home_dir)
    {
        Add_element_to_list(list, home_dir, Format_filename(home_dir, name_length, 2), 2, ICON_NONE);
        list->Nb_directories++;
    }

    mount_points_list = read_file_system_list(0);

    while(mount_points_list != NULL)
    {
        if(mount_points_list->me_dummy == 0 && strcmp(mount_points_list->me_mountdir,"/") && strcmp(mount_points_list->me_mountdir,"/home"))
        {
            Add_element_to_list(list, mount_points_list->me_mountdir, Format_filename(mount_points_list->me_mountdir, name_length, 2), 2, ICON_NONE);
            list->Nb_directories++;
        }
        next = mount_points_list -> me_next;
        #if !(defined(__macosx__) || defined(__FreeBSD__))
          free(mount_points_list -> me_type);
        #endif
        free(mount_points_list -> me_devname);
        free(mount_points_list -> me_mountdir);
        free(mount_points_list);
        mount_points_list = next;
    }

  }
  #endif

  Recount_files(list);
}

// Comparison of file names:
#ifdef WIN32
// case-insensitive
  #define FILENAME_COMPARE strcasecmp
#else
// case-sensitive
  #define FILENAME_COMPARE strcmp
#endif


// -- Tri de la liste des fichiers et r�pertoires ---------------------------
void Sort_list_of_files(T_Fileselector *list)
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

  // Check there are at least two elements before sorting
  if (list->First && list->First->Next)
  {
    do
    {
      // Par d�faut, on consid�re que la liste est tri�e
      list_is_sorted=1;

      current_item=list->First;
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
                  (FILENAME_COMPARE(current_item->Full_name,next_item->Full_name)>0) )
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
          if (current_item==list->First)
            list->First=next_item;

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
  // Force a recount / re-index
  Recount_files(list);
}

T_Fileselector_item * Get_item_by_index(T_Fileselector *list, short index)
{
  // Safety
  if (index >= list->Nb_elements)
    index=list->Nb_elements-1;

  if (list->Index)
  {
    return list->Index[index];
  }
  else
  {
    // Index not available.
    // Can only happen in case of malloc error.
    // Fall back anyway on iterative search
 
    T_Fileselector_item * item = list->First;
    for (;index>0;index--)
      item = item->Next;
    
    return item;
  }

}


// -- Affichage des �l�ments de la liste de fichier / r�pertoire ------------
void Display_file_list(T_Fileselector *list, short offset_first,short selector_offset)
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
  if (list->Nb_elements>0)
  {
    // On commence par chercher � pointer sur le premier fichier visible:
    current_item = Get_item_by_index(list, offset_first);

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
      if (current_item->Icon != ICON_NONE)
      {
        // Name preceded by an icon
        Print_in_window(16,95+index*8,current_item->Short_name,text_color,background_color);
        Window_display_icon_sprite(8,95+index*8,current_item->Icon);
      } else
        // Name without icon
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
void Get_selected_item(T_Fileselector *list, short offset_first,short selector_offset,char * label,int *type)
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
  if (list->Nb_elements>0)
  {
    // On commence par chercher � pointer sur le premier fichier visible:
    // Ensuite, on saute autant d'�l�ments que le d�calage demand�:
    current_item = Get_item_by_index(list, offset_first + selector_offset);

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
    && ( (*selector_offset)+1 < Filelist.Nb_elements ) )
    // Si la s�lection peut descendre
    Display_file_list(&Filelist, *offset_first,++(*selector_offset));
  else // Sinon, descendre la fen�tre (si possible)
  if ((*offset_first)+10<Filelist.Nb_elements)
    Display_file_list(&Filelist, ++(*offset_first),*selector_offset);
}


void Selector_scroll_up(short * offset_first,short * selector_offset)
// Fait scroller vers le haut le s�lecteur de fichier... (si possible)
{
  if ((*selector_offset)>0)
    // Si la s�lection peut monter
    Display_file_list(&Filelist, *offset_first,--(*selector_offset));
  else // Sinon, monter la fen�tre (si possible)
  if ((*offset_first)>0)
    Display_file_list(&Filelist, --(*offset_first),*selector_offset);
}


void Selector_page_down(short * offset_first,short * selector_offset, short lines)
{
  if (Filelist.Nb_elements-1>*offset_first+*selector_offset)
  {
    if (*selector_offset<9)
    {
      if (Filelist.Nb_elements<10)
      {
        *offset_first=0;
        *selector_offset=Filelist.Nb_elements-1;
      }
      else *selector_offset=9;
    }
    else
    {
      if (Filelist.Nb_elements>*offset_first+18)
        *offset_first+=lines;
      else
      {
        *offset_first=Filelist.Nb_elements-10;
        *selector_offset=9;
      }
    }
  }
  Display_file_list(&Filelist, *offset_first,*selector_offset);
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
  Display_file_list(&Filelist, *offset_first,*selector_offset);
}


void Selector_end(short * offset_first,short * selector_offset)
{
  if (Filelist.Nb_elements<10)
  {
    *offset_first=0;
    *selector_offset=Filelist.Nb_elements-1;
  }
  else
  {
    *offset_first=Filelist.Nb_elements-10;
    *selector_offset=9;
  }
  Display_file_list(&Filelist, *offset_first,*selector_offset);
}


void Selector_home(short * offset_first,short * selector_offset)
{
  Display_file_list(&Filelist, (*offset_first)=0,(*selector_offset)=0);
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
  if (computed_offset>=Filelist.Nb_elements)
    computed_offset=Filelist.Nb_elements-1;

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

//
// Print the current file name
//
void Print_filename_in_fileselector(void)
{
  Window_rectangle(82,48,27*8,8,MC_Light);
  Print_in_window_limited(82,48,Selector_filename,27,MC_Black,MC_Light);
  Update_window_area(82,48,27*8,8);
}

int   Selected_type; // Utilis� pour m�moriser le type d'entr�e choisi
                        // dans le selecteur de fichier.

void Prepare_and_display_filelist(short Position, short offset, T_Scroller_button * button)
{
  button->Nb_elements=Filelist.Nb_elements;
  button->Position=Position;
  Compute_slider_cursor_length(button);
  Window_draw_slider(button);
  // On efface les anciens noms de fichier:
  Window_rectangle(8-1,95-1,144+2,80+2,MC_Black);
  // On affiche les nouveaux:
  Display_file_list(&Filelist, Position,offset);

  Update_window_area(8-1,95-1,144+2,80+2);

  // On r�cup�re le nom du schmilblick � "acc�der"
  Get_selected_item(&Filelist, Position,offset,Selector_filename,&Selected_type);
  // On affiche le nouveau nom de fichier
  Print_filename_in_fileselector();
  // On affiche le nom du r�pertoire courant
  Print_current_directory();
}


void Reload_list_of_files(byte filter, T_Scroller_button * button)
{
  Read_list_of_files(&Filelist, filter);
  Sort_list_of_files(&Filelist);
  //
  // Check and fix the fileselector positions, because 
  // the directory content may have changed.
  //
  // Make the offset absolute
  Main_fileselector_offset += Main_fileselector_position;
  // Ensure it's within limits
  if (Main_fileselector_offset >= Filelist.Nb_elements)
  {
    Main_fileselector_offset = Filelist.Nb_elements-1;
  }
  // Ensure the position doesn't show "too many files"
  if (Main_fileselector_position!=0 && Main_fileselector_position>(Filelist.Nb_elements-10))
  {
    if (Filelist.Nb_elements<10)
    {
      Main_fileselector_position=0;
    }
    else
    {
      Main_fileselector_position=Filelist.Nb_elements-10;
    }
  }
  // Restore the offset as relative to the position.
  Main_fileselector_offset -= Main_fileselector_position;

  Prepare_and_display_filelist(Main_fileselector_position,Main_fileselector_offset,button);
}

void Scroll_fileselector(T_Scroller_button * file_scroller)
{
  char old_filename[MAX_PATH_CHARACTERS];

  strcpy(old_filename,Selector_filename);

  // On regarde si la liste a boug�
  if (file_scroller->Position!=Main_fileselector_position)
  {
    // Si c'est le cas, il faut mettre � jour la jauge
    file_scroller->Position=Main_fileselector_position;
    Window_draw_slider(file_scroller);
  }
  // On r�cup�re le nom du schmilblick � "acc�der"
  Get_selected_item(&Filelist, Main_fileselector_position,Main_fileselector_offset,Selector_filename,&Selected_type);
  if (strcmp(old_filename,Selector_filename))
    New_preview_is_needed=1;

  // On affiche le nouveau nom de fichier
  Print_filename_in_fileselector();
  Display_cursor();
}


short Find_file_in_fileselector(T_Fileselector *list, const char * fname)
{
  T_Fileselector_item * item;
  short  index;
  short  close_match=0;

  index=0;
  for (item=list->First; item!=NULL; item=item->Next)
  {
    if (strcmp(item->Full_name,fname)==0)
      return index;
    if (strcasecmp(item->Full_name,fname)==0)
      close_match=index;
      
    index++;
  }

  return close_match;
}

void Highlight_file(short index)
{

  if ((Filelist.Nb_elements<=10) || (index<5))
  {
    Main_fileselector_position=0;
    Main_fileselector_offset=index;
  }
  else
  {
    if (index>=Filelist.Nb_elements-5)
    {
      Main_fileselector_position=Filelist.Nb_elements-10;
      Main_fileselector_offset=index-Main_fileselector_position;
    }
    else
    {
      Main_fileselector_position=index-4;
      Main_fileselector_offset=4;
    }
  }
}


short Find_filename_match(T_Fileselector *list, char * fname)
{
  short best_match;
  T_Fileselector_item * current_item;
  short item_number;
  byte matching_letters=0;
  byte counter;

  best_match=-1;
  item_number=0;
  
  for (current_item=list->First; current_item!=NULL; current_item=current_item->Next)
  {
    if ( (!Config.Find_file_fast)
      || (Config.Find_file_fast==(current_item->Type+1)) )
    {
      // On compare et si c'est mieux, on stocke dans Meilleur_nom
      for (counter=0; fname[counter]!='\0' && tolower(current_item->Full_name[counter])==tolower(fname[counter]); counter++);
      if (counter>matching_letters)
      {
        matching_letters=counter;
        best_match=item_number;
      }
    }
    item_number++;
  }

  return best_match;
}

// Quicksearch system
char  quicksearch_filename[MAX_PATH_CHARACTERS]="";

void Reset_quicksearch(void)
{
  quicksearch_filename[0]='\0';
}

short Quicksearch(T_Fileselector *selector)
{
  int len;
  short most_matching_item;
  
  // Autre => On se place sur le nom de fichier qui correspond
  len=strlen(quicksearch_filename);
  if (Key_ANSI>= ' ' && Key_ANSI < 255 && len<50)
  {
    quicksearch_filename[len]=Key_ANSI;
    quicksearch_filename[len+1]='\0';
    most_matching_item=Find_filename_match(selector, quicksearch_filename);
    if ( most_matching_item >= 0 )
    {
      return most_matching_item;
    }
    *quicksearch_filename=0;
  }
  return -1;
}

// Translated from Highlight_file
void Locate_list_item(T_List_button * list, short selected_item)
{

  // Safety bounds
  if (selected_item<0)
    selected_item=0;
  else if (selected_item>=list->Scroller->Nb_elements)
    selected_item=list->Scroller->Nb_elements-1;
    
    
  if ((list->Scroller->Nb_elements<=list->Scroller->Nb_visibles) || (selected_item<(list->Scroller->Nb_visibles/2)))
  {
    list->List_start=0;
    list->Cursor_position=selected_item;
  }
  else
  {
    if (selected_item>=list->Scroller->Nb_elements-(list->Scroller->Nb_visibles/2))
    {
      list->List_start=list->Scroller->Nb_elements-list->Scroller->Nb_visibles;
      list->Cursor_position=selected_item-list->List_start;
    }
    else
    {
      list->List_start=selected_item-(list->Scroller->Nb_visibles/2-1);
      list->Cursor_position=(list->Scroller->Nb_visibles/2-1);
    }
  }
}

int Quicksearch_list(T_List_button * list, T_Fileselector * selector)
{
  // Try Quicksearch
  short selected_item=Quicksearch(selector);
  if (selected_item>=0 && selected_item!=list->Cursor_position+list->List_start)
  {
    Locate_list_item(list, selected_item);
    
    Hide_cursor();
    // Mise � jour du scroller
    list->Scroller->Position=list->List_start;
    Window_draw_slider(list->Scroller);
    
    Window_redraw_list(list);
    Display_cursor();
    // Store the selected value as attribute2
    Window_attribute2=list->List_start + list->Cursor_position;
    // Return the control ID of the list.
    return list->Number;
  }
  return 0;
}

byte Button_Load_or_Save(byte load, T_IO_Context *context)
  // load=1 => On affiche le menu du bouton LOAD
  // load=0 => On affiche le menu du bouton SAVE
{
  short clicked_button;
  T_Scroller_button * file_scroller;
  T_Dropdown_button * formats_dropdown;
  T_Dropdown_button * bookmark_dropdown[4];
  short temp;
  unsigned int format;
  int dummy=0;       // Sert � appeler SDL_GetKeyState
  byte  save_or_load_image=0;
  byte  has_clicked_ok=0;// Indique si on a click� sur Load ou Save ou sur
                             //un bouton enclenchant Load ou Save juste apr�s.
  byte  initial_back_color;	// preview destroys it (how nice)
  char  previous_directory[MAX_PATH_CHARACTERS]; // R�pertoire d'o� l'on vient apr�s un CHDIR
  char  save_filename[MAX_PATH_CHARACTERS];
  char  initial_comment[COMMENT_SIZE+1];
  short window_shortcut;

  Reset_quicksearch();
  
  //if (Native_filesel(load) != 0); // TODO : handle this
  
  if (context->Type == CONTEXT_MAIN_IMAGE)
    window_shortcut = load?(0x100+BUTTON_LOAD):(0x100+BUTTON_SAVE);
  else
    window_shortcut = load?SPECIAL_LOAD_BRUSH:SPECIAL_SAVE_BRUSH;

  // Backup data that needs be restored on "cancel"  
  initial_back_color=Back_color;
  strcpy(initial_comment,context->Comment);
  
  if (load)
  {
    if (context->Type == CONTEXT_MAIN_IMAGE)
      Open_window(310,200,"Load picture");
    else
      Open_window(310,200,"Load brush");
    Window_set_normal_button(198,180,51,14,"Load",0,1,SDLK_RETURN); // 1
  }
  else
  {
    if (context->Type == CONTEXT_MAIN_IMAGE)
      Open_window(310,200,"Save picture");
    else
      Open_window(310,200,"Save brush");
    Window_set_normal_button(198,180,51,14,"Save",0,1,SDLK_RETURN); // 1
    if (Main_format<=FORMAT_ALL_FILES) // Correction du *.*
    {
      Main_format=Main_fileformat;
      Main_fileselector_position=0;
      Main_fileselector_offset=0;
    }

    if (Get_fileformat(Main_format)->Save == NULL) // Correction d'un format insauvable
    {
      Main_format=DEFAULT_FILEFORMAT;
      Main_fileselector_position=0;
      Main_fileselector_offset=0;
    }
    // Affichage du commentaire
    if (Get_fileformat(Main_format)->Comment)
      Print_in_window(45,70,context->Comment,MC_Black,MC_Light);      
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
    Window_set_dropdown_button(68,28,52,11,0,
      Get_fileformat(Main_format)->Label,
      1,0,1,RIGHT_SIDE|LEFT_SIDE,0); // 6

  for (format=0; format < Nb_known_formats(); format++)
  {
    if ((load && (File_formats[format].Identifier <= FORMAT_ALL_FILES || File_formats[format].Load)) || 
      (!load && File_formats[format].Save))
        Window_dropdown_add_item(formats_dropdown,File_formats[format].Identifier,File_formats[format].Label);
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
      Window_set_dropdown_button(127+(88+1)*(temp%2),18+(temp/2)*12,88,11,56,"",0,0,1,RIGHT_SIDE,0); // 10-13
    Window_display_icon_sprite(bookmark_dropdown[temp]->Pos_X+3,bookmark_dropdown[temp]->Pos_Y+2,ICON_STAR);
    Display_bookmark(bookmark_dropdown[temp],temp);
  }
  // On prend bien soin de passer dans le r�pertoire courant (le bon qui faut! Oui madame!)
  if (load)
  {
  #if defined(__MINT__)    
    static char path[1024]={0};
  
    chdir(Main_current_directory);
    Dgetpath(path,0);
    strcat(path,PATH_SEPARATOR);
    strcpy(Main_current_directory,path);  
  #else
    chdir(Main_current_directory);
    getcwd(Main_current_directory,256);
  #endif
  }
  else
  {
    #if defined(__MINT__)    
    static char path[1024]={0};
    chdir(context->File_directory);
    Dgetpath(path,0);
    strcat(path,PATH_SEPARATOR);
    strcpy(Main_current_directory,path);  
 #else
    chdir(context->File_directory);
    getcwd(Main_current_directory,256);
  #endif


  }
  
  // Affichage des premiers fichiers visibles:
  Reload_list_of_files(Main_format,file_scroller);

  if (!load)
  {
    // On initialise le nom de fichier � celui en cours et non pas celui sous
    // la barre de s�lection
    strcpy(Selector_filename,context->File_name);
    // On affiche le nouveau nom de fichier
    Print_filename_in_fileselector();

    Highlight_file(Find_file_in_fileselector(&Filelist, context->File_name));
    Prepare_and_display_filelist(Main_fileselector_position,Main_fileselector_offset,file_scroller);
  }

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
          if(File_exists(Selector_filename)) 
          {
            Selected_type = 0;
            if(Directory_exists(Selector_filename)) Selected_type = 1;
          }
          else
          {
            Selected_type = 1;
          }
        }
        else
        {
          if(Directory_exists(Selector_filename)) Selected_type = 1;
          else Selected_type = 0;
        }
        has_clicked_ok=1;
        break;

      case  2 : // Cancel
        break;

      case  3 : // Delete
        if (Filelist.Nb_elements && (*Selector_filename!='.') && Selected_type!=2)
        {
          char * message;
          Hide_cursor();
          // On affiche une demande de confirmation
          if (Main_fileselector_position+Main_fileselector_offset>=Filelist.Nb_directories)
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
            if (Main_fileselector_position+Main_fileselector_offset>=Filelist.Nb_directories)
              // On efface le fichier (si on peut)
              temp=(!remove(Selector_filename));
            else // Si c'est un repertoire
              // On efface le repertoire (si on peut)
              temp=(!rmdir(Selector_filename));

            if (temp) // temp indique si l'effacement s'est bien pass�
            {
              // On remonte si c'�tait le dernier �l�ment de la liste
              if (Main_fileselector_position+Main_fileselector_offset==Filelist.Nb_elements-1)
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
                  && (Main_fileselector_position+10==Filelist.Nb_elements) )
                  {
                    Main_fileselector_position--;
                    Main_fileselector_offset++;
                  }
              }
              // On relit les informations
              Reload_list_of_files(Main_format,file_scroller);
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
            Get_selected_item(&Filelist, Main_fileselector_position,Main_fileselector_offset,Selector_filename,&Selected_type);
            // On affiche le nouveau nom de fichier
            Print_filename_in_fileselector();
            // On affiche � nouveau la liste
            Display_file_list(&Filelist, Main_fileselector_position,Main_fileselector_offset);

            // On vient de changer de nom de fichier, donc on doit s'appreter
            // a rafficher une preview
            New_preview_is_needed=1;
            Reset_quicksearch();
            
          }
          else
          {
            //   En sauvegarde, si on a double-click� sur un r�pertoire, il
            // faut mettre le nom de fichier au nom du r�pertoire. Sinon, dans
            // certains cas, on risque de sauvegarder avec le nom du fichier
            // actuel au lieu de changer de r�pertoire.
            if (Main_fileselector_position+Main_fileselector_offset<Filelist.Nb_directories)
              Get_selected_item(&Filelist, Main_fileselector_position,Main_fileselector_offset,Selector_filename,&Selected_type);

            has_clicked_ok=1;
            New_preview_is_needed=1;
            Reset_quicksearch();
          }
        }
        Display_cursor();
        Wait_end_of_click();
        break;

      case  5 : // Scroller de fichiers
        Hide_cursor();
        Main_fileselector_position=Window_attribute2;
        // On r�cup�re le nom du schmilblick � "acc�der"
        Get_selected_item(&Filelist, Main_fileselector_position,Main_fileselector_offset,Selector_filename,&Selected_type);
        // On affiche le nouveau nom de fichier
        Print_filename_in_fileselector();
        // On affiche � nouveau la liste
        Display_file_list(&Filelist, Main_fileselector_position,Main_fileselector_offset);
        Display_cursor();
        New_preview_is_needed=1;
        Reset_quicksearch();
        break;

	  case  6 : // Scroller des formats
		// On met � jour le format de browsing du fileselect:
		if (Main_format != Window_attribute2) {
			char* savename = (char *)strdup(Selector_filename);
			int nameLength = strlen(savename);
			Main_format = Window_attribute2;
			// Comme on change de liste, on se place en d�but de liste:
			Main_fileselector_position = 0;
			Main_fileselector_offset = 0;
			// Affichage des premiers fichiers visibles:
			Hide_cursor();
			Reload_list_of_files(Main_format, file_scroller);
			New_preview_is_needed = 1;
			Reset_quicksearch();
			strcpy(Selector_filename, savename);
			if (Get_fileformat(Main_format)->Default_extension[0] != '\0' &&
				Selector_filename[nameLength - 4] == '.')
			{
				strcpy(Selector_filename + nameLength - 3,
					Get_fileformat(Main_format)->Default_extension);
			}
			free(savename);
			Print_filename_in_fileselector();
        	Display_cursor();
		}
        break;
      case  7 : // Saisie d'un commentaire pour la sauvegarde
        if ( (!load) && (Get_fileformat(Main_format)->Comment) )
        {
          Readline(45, 70, context->Comment, 32, INPUT_TYPE_STRING);
          Display_cursor();
        }
        break;
      case  8 : // Saisie du nom de fichier

        // Save the filename
        strcpy(save_filename, Selector_filename);

        if (Readline(82,48,Selector_filename,27,INPUT_TYPE_FILENAME))
        {
          //   On regarde s'il faut rajouter une extension. C'est-�-dire s'il
          // n'y a pas de '.' dans le nom du fichier.
          for(temp=0,dummy=0; ((Selector_filename[temp]) && (!dummy)); temp++)
            if (Selector_filename[temp]=='.')
              dummy=1;
          if (!dummy)
          {
            if (Get_fileformat(Main_format)->Default_extension)
            {
              if(!Directory_exists(Selector_filename))
              {
                 strcat(Selector_filename, ".");
                strcat(Selector_filename, Get_fileformat(Main_format)->Default_extension);
              }
            }
            else
            {
              // put default extension
              // (but maybe we should browse through all available ones until we find
              //  something suitable ?)
              if(!Directory_exists(Selector_filename))
              {
                 strcat(Selector_filename, ".pkm");
              }
            }
          }
          if(load)
          {
            // Determine the type
            if(File_exists(Selector_filename))
            {
              Selected_type = 0;
              if(Directory_exists(Selector_filename)) Selected_type = 1;
            }
            else
            {
              Selected_type = 1;
            }
          }
          else
          {
            if(Directory_exists(Selector_filename)) Selected_type = 1;
            else Selected_type = 0;
          }

          // Now load immediately, but only if the user exited readline by pressing ENTER
          if (Mouse_K == 0) has_clicked_ok = 1;
        }
        else
        {
          // Restore the old filename
          strcpy(Selector_filename, save_filename);
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
          Read_list_of_drives(&Filelist,19);
          Sort_list_of_files(&Filelist);
          Prepare_and_display_filelist(Main_fileselector_position,Main_fileselector_offset,file_scroller);
          Display_cursor();
          New_preview_is_needed=1;
          Reset_quicksearch();
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
                  strcpy(Selector_filename,Config.Bookmark_directory[clicked_button-10]);
                  Selected_type=1;
                  has_clicked_ok=1;
                  Reset_quicksearch();
                }
                break;
                
              case 0: // Set
                free(Config.Bookmark_directory[clicked_button-10]);
                Config.Bookmark_directory[clicked_button-10] = NULL;
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
                  if (Readline_ex(bookmark_dropdown[clicked_button-10]->Pos_X+3+10,bookmark_dropdown[clicked_button-10]->Pos_Y+2,bookmark_label,8,8,INPUT_TYPE_STRING,0))
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
        Reset_quicksearch();
        Hide_cursor();
        Selector_scroll_down(&Main_fileselector_position,&Main_fileselector_offset);
        Scroll_fileselector(file_scroller);
        Key=0;
        break;
      case SDLK_UP : // Haut
        Reset_quicksearch();
        Hide_cursor();
        Selector_scroll_up(&Main_fileselector_position,&Main_fileselector_offset);
        Scroll_fileselector(file_scroller);
        Key=0;
        break;
      case SDLK_PAGEDOWN : // PageDown
        Reset_quicksearch();
        Hide_cursor();
        Selector_page_down(&Main_fileselector_position,&Main_fileselector_offset,9);
        Scroll_fileselector(file_scroller);
        Key=0;
        break;
      case SDLK_PAGEUP : // PageUp
        Reset_quicksearch();
        Hide_cursor();
        Selector_page_up(&Main_fileselector_position,&Main_fileselector_offset,9);
        Scroll_fileselector(file_scroller);
        Key=0;
        break;
      case SDLK_END : // End
        Reset_quicksearch();
        Hide_cursor();
        Selector_end(&Main_fileselector_position,&Main_fileselector_offset);
        Scroll_fileselector(file_scroller);
        Key=0;
        break;
      case SDLK_HOME : // Home
        Reset_quicksearch();
        Hide_cursor();
        Selector_home(&Main_fileselector_position,&Main_fileselector_offset);
        Scroll_fileselector(file_scroller);
        Key=0;
        break;
      case KEY_MOUSEWHEELDOWN :
        Reset_quicksearch();
        Hide_cursor();
        Selector_page_down(&Main_fileselector_position,&Main_fileselector_offset,3);
        Scroll_fileselector(file_scroller);
        Key=0;
        break;
      case KEY_MOUSEWHEELUP :
        Reset_quicksearch();
        Hide_cursor();
        Selector_page_up(&Main_fileselector_position,&Main_fileselector_offset,3);
        Scroll_fileselector(file_scroller);
        Key=0;
        break;
      case SDLK_BACKSPACE : // Backspace
        Reset_quicksearch();
        // Si le choix ".." est bien en t�te des propositions...
        if (!strcmp(Filelist.First->Full_name,PARENT_DIR))
        {                              
          // On va dans le r�pertoire parent.
          strcpy(Selector_filename,PARENT_DIR);
          Selected_type=1;
          has_clicked_ok=1;
        }
        Key=0;
        break;
      default:
        if (clicked_button<=0)
        {
          short selected_item;
          
          if (Is_shortcut(Key,0x100+BUTTON_HELP))
          {
            Window_help(load?BUTTON_LOAD:BUTTON_SAVE, NULL);
            break;
          }
          if (Is_shortcut(Key,window_shortcut))
          {
            clicked_button=2;
            break;
          }
          
          selected_item=Quicksearch(&Filelist);
          if (selected_item>=0)
          {
              temp=Main_fileselector_position+Main_fileselector_offset;
              Hide_cursor();
              Highlight_file(selected_item);
              Prepare_and_display_filelist(Main_fileselector_position,Main_fileselector_offset,file_scroller);
              Display_cursor();
              if (temp!=Main_fileselector_position+Main_fileselector_offset)
                New_preview_is_needed=1;
          }
          // Key=0; ?
        }
        else
          Reset_quicksearch();
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
        if (strcmp(Selector_filename,PARENT_DIR))
        {
          strcpy(previous_directory,PARENT_DIR);
        }
        else
        {
            Extract_filename(previous_directory, Main_current_directory);
        }

        // On doit rentrer dans le r�pertoire:
        if (!chdir(Selector_filename))
        {
        #if defined (__MINT__)          
          static char path[1024]={0};
          char currentDrive='A';
          currentDrive=currentDrive+Dgetdrv();
          Dgetpath(path,0);
         sprintf(Main_current_directory,"%c:\%s",currentDrive,path);
        #else
          getcwd(Main_current_directory,256);
        #endif
          // On lit le nouveau r�pertoire
          Read_list_of_files(&Filelist, Main_format);
          Sort_list_of_files(&Filelist);
          // On place la barre de s�lection sur le r�pertoire d'o� l'on vient
          Highlight_file(Find_file_in_fileselector(&Filelist, previous_directory));
        }
        else
          Error(0);
        // Affichage des premiers fichiers visibles:
        Prepare_and_display_filelist(Main_fileselector_position,Main_fileselector_offset,file_scroller);
        Display_cursor();
        New_preview_is_needed=1;

        // On est dans un nouveau r�pertoire, donc on remet le quicksearch � 0
        Reset_quicksearch();
      }
      else  // Sinon on essaye de charger ou sauver le fichier
      {
        strcpy(context->File_directory,Main_current_directory);
        if (!load && !Get_fileformat(Main_format)->Palette_only)
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
        Window_rectangle(183,95,PREVIEW_WIDTH,PREVIEW_HEIGHT,MC_Light);
        // On efface les dimensions de l'image
        Window_rectangle(143,59,72,8,MC_Light);
        // On efface la taille du fichier
        Window_rectangle(236,59,56,8,MC_Light);
        // On efface le format du fichier
        Window_rectangle(59,59,5*8,8,MC_Light);
        // Affichage du commentaire
        if ( (!load) && (Get_fileformat(Main_format)->Comment) )
        {
          Print_in_window(45,70,context->Comment,MC_Black,MC_Light);
        }
        Display_cursor();
        // Un update pour couvrir les 4 zones: 3 libell�s plus le commentaire
        Update_window_area(45,48,256,30);
        // Zone de preview
        Update_window_area(183,95,PREVIEW_WIDTH,PREVIEW_HEIGHT);
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
      if ( (Main_fileselector_position+Main_fileselector_offset>=Filelist.Nb_directories) && (Filelist.Nb_elements) )
      {
        T_IO_Context preview_context;
      
        Init_context_preview(&preview_context, Selector_filename, Main_current_directory);
        Hide_cursor();

        Load_image(&preview_context);
        Destroy_context(&preview_context);

        Update_window_area(0,0,Window_width,Window_height);
        Display_cursor();

      }

      Timer_state=2; // On arr�te le chrono
    }
  }
  while ( (!has_clicked_ok) && (clicked_button!=2) );

  if (has_clicked_ok)
  {
    strcpy(context->File_name, Selector_filename);
    strcpy(context->File_directory, Main_current_directory);
    if (!load)
      context->Format = Main_format;
  }
  else
  {
    // Data to restore
    strcpy(context->Comment, initial_comment);
  }
  

  //   On restaure les donn�es de l'image qui ont certainement �t� modifi�es
  // par la preview.
  Set_palette(Main_palette);
  Back_color=initial_back_color;

  Compute_optimal_menu_colors(Main_palette);
  temp=(Window_pos_Y+(Window_height*Menu_factor_Y)<Menu_Y_before_window);

  Close_window();

  if (temp)
    Display_menu();

  Unselect_button((load)?BUTTON_LOAD:BUTTON_SAVE);
  Display_cursor();
  Free_fileselector_list(&Filelist);

  return save_or_load_image;
}
