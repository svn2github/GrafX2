/*  Grafx2 - The Ultimate 256-color bitmap paint program

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
    along with Grafx2; if not, see <http://www.gnu.org/licenses/> or
    write to the Free Software Foundation, Inc.,
    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#if defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
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
#include "divers.h"
#include "erreurs.h"
#include "io.h"
#include "windows.h"
#include "sdlscreen.h"
#include "loadsave.h"
#include "mountlist.h"
#include "moteur.h"
#include "readline.h"
#include "input.h"
#include "aide.h"

#define COULEUR_FICHIER_NORMAL    CM_Clair // Couleur du texte pour une ligne de fichier non s�lectionn�
#define COULEUR_REPERTOIRE_NORMAL CM_Fonce // Couleur du texte pour une ligne de r�pertoire non s�lectionn�
#define COULEUR_FOND_NORMAL       CM_Noir  // Couleur du fond  pour une ligne non s�lectionn�e
#define COULEUR_FICHIER_SELECT    CM_Blanc // Couleur du texte pour une ligne de fichier    s�lectionn�e
#define COULEUR_REPERTOIRE_SELECT CM_Clair // Couleur du texte pour une ligne de rep�rtoire s�lectionn�e
#define COULEUR_FOND_SELECT       CM_Fonce // Couleur du fond  pour une ligne s�lectionn�e

#define FILENAMESPACE 13

// Conventions:
//
// * Le fileselect modifie le r�pertoire courant. Ceci permet de n'avoir
//   qu'un findfirst dans le r�pertoire courant � faire:


// -- D�struction de la liste cha�n�e ---------------------------------------
void Detruire_liste_du_fileselect(void)
//  Cette proc�dure d�truit la chaine des fichiers. Elle doit �tre appel�e
// avant de rappeler la fonction Lire_liste_des_fichiers, ainsi qu'en fin de
// programme.
{
  // Pointeur temporaire de destruction
  Element_de_liste_de_fileselect * Element_temporaire;

  while (Liste_du_fileselect!=NULL)
  {
    // On m�morise l'adresse du premier �l�ment de la liste
    Element_temporaire =Liste_du_fileselect;
    // On fait avancer la t�te de la liste
    Liste_du_fileselect=Liste_du_fileselect->Suivant;
    // Et on efface l'ancien premier �l�ment de la liste
    free(Element_temporaire);
  }
}


// -- Formatage graphique des noms de fichier / r�pertoire ------------------
char * Nom_formate(char * fname, int type)
{
  static char Resultat[13];
  int         c;
  int         Autre_curseur;
  int         Pos_DernierPoint;

  if (strcmp(fname,PARENT_DIR)==0)
  {
    strcpy(Resultat,"<-PARENT DIR");
  }
  else if (fname[0]=='.' || type==2)
  {
    // Fichiers ".quelquechose" ou lecteurs: Cal� � gauche sur 12 caract�res maximum.
    strcpy(Resultat,"            ");
    for (c=0;fname[c]!='\0' && c < 12;c++)
      Resultat[c]=fname[c];
    // Un caract�re sp�cial pour indiquer que l'affichage est tronqu�
    if (c >= 12)
      Resultat[11]=CARACTERE_SUSPENSION;
  }
  else
  {
    strcpy(Resultat,"        .   ");
    // On commence par recopier la partie pr�c�dent le point:
    for (c=0;( (fname[c]!='.') && (fname[c]!='\0') );c++)
    {
      if (c < 8)
        Resultat[c]=fname[c];
    }
    // Un caract�re sp�cial pour indiquer que l'affichage est tronqu�
    if (c > 8)
      Resultat[7]=CARACTERE_SUSPENSION;
    // On recherche le dernier point dans le reste du nom
    for (Pos_DernierPoint = c; fname[c]!='\0'; c++)
      if (fname[c]=='.')
        Pos_DernierPoint = c;

    // Ensuite on recopie la partie qui suit le point (si n�cessaire):
    if (fname[Pos_DernierPoint])
    {
      for (c = Pos_DernierPoint+1,Autre_curseur=9;fname[c]!='\0' && Autre_curseur < 12;c++,Autre_curseur++)
        Resultat[Autre_curseur]=fname[c];
    }
  }
  return Resultat;
}


// -- Rajouter a la liste des elements de la liste un element ---------------
void Ajouter_element_a_la_liste(char * fname, int type)
//  Cette procedure ajoute a la liste chainee un fichier pass� en argument.
{
  // Pointeur temporaire d'insertion
  Element_de_liste_de_fileselect * Element_temporaire;

  // On alloue de la place pour un nouvel element
  Element_temporaire=(Element_de_liste_de_fileselect *)malloc(sizeof(Element_de_liste_de_fileselect));

  // On met a jour le nouvel emplacement:
  strcpy(Element_temporaire->NomAbrege,Nom_formate(fname, type));
  strcpy(Element_temporaire->NomComplet,fname);
  Element_temporaire->Type = type;

  Element_temporaire->Suivant  =Liste_du_fileselect;
  Element_temporaire->Precedent=NULL;

  if (Liste_du_fileselect!=NULL)
    Liste_du_fileselect->Precedent=Element_temporaire;
  Liste_du_fileselect=Element_temporaire;
}

// -- V�rification si un fichier a l'extension demand�e.
// Autorise les '?', et '*' si c'est le seul caract�re.
int VerifieExtension(const char *NomFichier, char * Filtre)
{
  int Pos_DernierPoint = -1;
  int c = 0;

  if (Filtre[0] == '*')
    return 1;
  // On recherche la position du dernier . dans le nom
  for (c = 0; NomFichier[c]!='\0'; c++)
    if (NomFichier[c]=='.')
      Pos_DernierPoint = c;
  // Fichier sans extension (ca arrive)
  if (Pos_DernierPoint == -1)
    return (Filtre[0] == '\0');

  // V�rification caract�re par caract�re, case-insensitive.
  c = 0;
  do
  {
    if (Filtre[c] != '?' &&
      tolower(Filtre[c]) != tolower(NomFichier[Pos_DernierPoint + 1 + c]))
      return 0;

     c++;
  } while (Filtre[c++] != '\0');

  return 1;
}


// -- Lecture d'une liste de fichiers ---------------------------------------
void Lire_liste_des_fichiers(byte Format_demande)
//  Cette proc�dure charge dans la liste chain�e les fichiers dont l'extension
// correspond au format demand�.
{
  DIR*  Repertoire_Courant; //R�pertoire courant
  struct dirent* entry; // Structure de lecture des �l�ments
  char * Filtre = "*"; // Extension demand�e
  struct stat Infos_enreg;
  char * Chemin_courant;

  // Tout d'abord, on d�duit du format demand� un filtre � utiliser:
  if (Format_demande) // Format (extension) sp�cifique
    Filtre = FormatFichier[Format_demande-1].Extension;

  // Ensuite, on vide la liste actuelle:
  Detruire_liste_du_fileselect();
  // Apr�s effacement, il ne reste ni fichier ni r�pertoire dans la liste
  Liste_Nb_fichiers=0;
  Liste_Nb_repertoires=0;

  // On lit tous les r�pertoires:
  Chemin_courant=getcwd(NULL,0);
  Repertoire_Courant=opendir(Chemin_courant);
  while ((entry=readdir(Repertoire_Courant)))
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
       Config.Lire_les_repertoires_caches ||
     !isHidden(entry)))
    {
      // On rajoute le r�pertoire � la liste
      Ajouter_element_a_la_liste(entry->d_name, 1);
      Liste_Nb_repertoires++;
    }
    else if (S_ISREG(Infos_enreg.st_mode) && //Il s'agit d'un fichier
      (Config.Lire_les_fichiers_caches || //Il n'est pas cach�
      !isHidden(entry)))
    {
      if (VerifieExtension(entry->d_name, Filtre))
      {
        // On rajoute le fichier � la liste
        Ajouter_element_a_la_liste(entry->d_name, 0);
        Liste_Nb_fichiers++;
      }
    }
  }

#if defined(__MORPHOS__) || defined (__amigaos4__) || defined(__amigaos__)
  Ajouter_element_a_la_liste("/",1); // on amiga systems, / means parent. And there is no ..
  Liste_Nb_repertoires ++;
#endif

  closedir(Repertoire_Courant);
  free(Chemin_courant);

  Liste_Nb_elements=Liste_Nb_repertoires+Liste_Nb_fichiers;
}

#if defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
void bstrtostr( BSTR in, STRPTR out, TEXT max )
{
  STRPTR iptr;
  dword i;

  iptr = BADDR( in );

  if( max > iptr[0] ) max = iptr[0];

  for( i=0; i<max; i++ ) out[i] = iptr[i+1];
  out[i] = 0;
}
#endif

// -- Lecture d'une liste de lecteurs / volumes -----------------------------
void Lire_liste_des_lecteurs(void)
{

  // Empty the current content of fileselector:
  Detruire_liste_du_fileselect();
  // Reset number of items
  Liste_Nb_fichiers=0;
  Liste_Nb_repertoires=0;

  #if defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
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
        Ajouter_element_a_la_liste( tmp, 2 );
        Liste_Nb_repertoires++;
      }
      UnLockDosList( LDF_VOLUMES | LDF_READ );
    }
  }
  #elif defined (__WIN32__)
  {
    char NomLecteur[]="A:\\";
    int DriveBits = GetLogicalDrives();
    int IndiceLecteur;
    int IndiceBit;
    // Sous Windows, on a la totale, presque aussi bien que sous DOS:
    IndiceLecteur = 0;
    for (IndiceBit=0; IndiceBit<26 && IndiceLecteur<23; IndiceBit++)
    {
      if ( (1 << IndiceBit) & DriveBits )
      {
        // On a ce lecteur, il faut maintenant d�terminer son type "physique".
        // pour profiter des jolies icones de X-man.
        int TypeLecteur;
        char CheminLecteur[]="A:\\";
        // Cette API Windows est �trange, je dois m'y faire...
        CheminLecteur[0]='A'+IndiceBit;
        switch (GetDriveType(CheminLecteur))
        {
          case DRIVE_CDROM:
            TypeLecteur=LECTEUR_CDROM;
            break;
          case DRIVE_REMOTE:
            TypeLecteur=LECTEUR_NETWORK;
            break;
          case DRIVE_REMOVABLE:
            TypeLecteur=LECTEUR_FLOPPY_3_5;
            break;
          case DRIVE_FIXED:
            TypeLecteur=LECTEUR_HDD;
            break;
          default:
            TypeLecteur=LECTEUR_NETWORK;
            break;
        }
        NomLecteur[0]='A'+IndiceBit;
        Ajouter_element_a_la_liste(NomLecteur,2);
        Liste_Nb_repertoires++;
        IndiceLecteur++;
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
    Ajouter_element_a_la_liste("/", 2);
    Liste_Nb_repertoires++;
    if(home_dir)
    {
        Ajouter_element_a_la_liste(home_dir, 2);
        Liste_Nb_repertoires++;
    }

    Liste_points_montage = read_file_system_list(0);

    while(Liste_points_montage != NULL)
    {
        if(Liste_points_montage->me_dummy == 0 && strcmp(Liste_points_montage->me_mountdir,"/") && strcmp(Liste_points_montage->me_mountdir,"/home"))
        {
            Ajouter_element_a_la_liste(Liste_points_montage->me_mountdir,2);
            Liste_Nb_repertoires++;
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

  Liste_Nb_elements=Liste_Nb_repertoires+Liste_Nb_fichiers;
}


// -- Tri de la liste des fichiers et r�pertoires ---------------------------
void Trier_la_liste_des_fichiers(void)
// Tri la liste chain�e existante dans l'ordre suivant:
//
// * Les r�pertoires d'abord, dans l'ordre alphab�tique de leur nom
// * Les fichiers ensuite, dans l'ordre alphab�tique de leur nom
{
  byte   La_liste_est_triee; // Bool�en "La liste est tri�e"
  byte   need_swap;          // Bool�en "Il faut inverser les �l�ments"
  Element_de_liste_de_fileselect * Element_precedent;
  Element_de_liste_de_fileselect * Element_courant;
  Element_de_liste_de_fileselect * Element_suivant;
  Element_de_liste_de_fileselect * Element_suivant_le_suivant;

  // Avant de trier quoi que ce soit, on v�rifie qu'il y ait suffisamment
  // d'�l�ments pour qu'il soit possibles qu'ils soient en d�sordre:
  if (Liste_Nb_elements>1)
  {
    do
    {
      // Par d�faut, on consid�re que la liste est tri�e
      La_liste_est_triee=1;

      Element_courant=Liste_du_fileselect;
      Element_suivant=Element_courant->Suivant;

      while ( (Element_courant!=NULL) && (Element_suivant!=NULL) )
      {
        // On commence par supposer qu'il n'y pas pas besoin d'inversion
        need_swap=0;

        // Ensuite, on v�rifie si les deux �l�ments sont bien dans l'ordre ou
        // non:

          // Si l'�l�ment courant est un fichier est que le suivant est
          // un r�pertoire -> need_swap
        if ( Element_courant->Type < Element_suivant->Type )
          need_swap=1;
          // Si les deux �l�ments sont de m�me type et que le nom du suivant
          // est plus petit que celui du courant -> need_swap
        else if ( (Element_courant->Type==Element_suivant->Type) &&
                  (strcmp(Element_courant->NomComplet,Element_suivant->NomComplet)>0) )
          need_swap=1;


        if (need_swap)
        {
          // Si les deux �l�ments n�cessitent d'�tre invers�:

          // On les inverses:

          // On note avant tout les �l�ments qui encapsulent nos deux amis
          Element_precedent         =Element_courant->Precedent;
          Element_suivant_le_suivant=Element_suivant->Suivant;

          // On permute le cha�nage des deux �l�ments entree eux
          Element_courant->Suivant  =Element_suivant_le_suivant;
          Element_courant->Precedent=Element_suivant;
          Element_suivant->Suivant  =Element_courant;
          Element_suivant->Precedent=Element_precedent;

          // On tente un cha�nage des �l�ments encapsulant les comp�res:
          if (Element_precedent!=NULL)
            Element_precedent->Suivant=Element_suivant;
          if (Element_suivant_le_suivant!=NULL)
            Element_suivant_le_suivant->Precedent=Element_courant;

          // On fait bien attention � modifier la t�te de liste en cas de besoin
          if (Element_courant==Liste_du_fileselect)
            Liste_du_fileselect=Element_suivant;

          // Ensuite, on se pr�pare � �tudier les �l�ments pr�c�dents:
          Element_courant=Element_precedent;

          // Et on constate que la liste n'�tait pas encore g�nialement tri�e
          La_liste_est_triee=0;
        }
        else
        {
          // Si les deux �l�ments sont dans l'ordre:

          // On passe aux suivants
          Element_courant=Element_courant->Suivant;
          Element_suivant=Element_suivant->Suivant;
        }
      }
    }
    while (!La_liste_est_triee);
  }
}


// -- Affichage des �l�ments de la liste de fichier / r�pertoire ------------
void Afficher_la_liste_des_fichiers(short Decalage_premier,short Decalage_select)
//
// Decalage_premier = D�calage entre le premier fichier visible dans le
//                   s�lecteur et le premier fichier de la liste
//
// Decalage_select  = D�calage entre le premier fichier visible dans le
//                   s�lecteur et le fichier s�lectionn� dans la liste
//
{
  Element_de_liste_de_fileselect * Element_courant;
  byte   Indice;  // Indice du fichier qu'on affiche (0 -> 9)
  byte   Couleur_texte;
  byte   Couleur_fond;


  // On v�rifie s'il y a au moins 1 fichier dans la liste:
  if (Liste_Nb_elements>0)
  {
    // On commence par chercher � pointer sur le premier fichier visible:
    Element_courant=Liste_du_fileselect;
    for (;Decalage_premier>0;Decalage_premier--)
      Element_courant=Element_courant->Suivant;

    // Pour chacun des 10 �l�ments inscriptibles � l'�cran
    for (Indice=0;Indice<10;Indice++)
    {
      // S'il est s�lectionn�:
      if (!Decalage_select)
      {
        // Si c'est un fichier
        if (Element_courant->Type==0)
          Couleur_texte=COULEUR_FICHIER_SELECT;
        else
          Couleur_texte=COULEUR_REPERTOIRE_SELECT;

        Couleur_fond=COULEUR_FOND_SELECT;
      }
      else
      {
        // Si c'est un fichier
        if (Element_courant->Type==0)
          Couleur_texte=COULEUR_FICHIER_NORMAL;
        else
          Couleur_texte=COULEUR_REPERTOIRE_NORMAL;

        Couleur_fond=COULEUR_FOND_NORMAL;
      }

      // On affiche l'�l�ment
      Print_dans_fenetre(9,90+FILENAMESPACE+(Indice<<3),Element_courant->NomAbrege,Couleur_texte,Couleur_fond);

      // On passe � la ligne suivante
      Decalage_select--;
      Element_courant=Element_courant->Suivant;
      if (!Element_courant)
        break;
    } // Fin de la boucle d'affichage

  } // Fin du test d'existence de fichiers
}


// -- R�cup�rer le libell� d'un �l�ment de la liste -------------------------
void Determiner_element_de_la_liste(short Decalage_premier,short Decalage_select,char * label,int *type)
//
// Decalage_premier = D�calage entre le premier fichier visible dans le
//                   s�lecteur et le premier fichier de la liste
//
// Decalage_select  = D�calage entre le premier fichier visible dans le
//                   s�lecteur et le fichier � r�cup�rer
//
// label          = Chaine de r�ception du libell� de l'�l�ment
//
// type             = R�cup�ration du type: 0 fichier, 1 repertoire, 2 lecteur.
//                    Passer NULL si pas interess�.
{
  Element_de_liste_de_fileselect * Element_courant;

  // On v�rifie s'il y a au moins 1 fichier dans la liste:
  if (Liste_Nb_elements>0)
  {
    // On commence par chercher � pointer sur le premier fichier visible:
    Element_courant=Liste_du_fileselect;
    for (;Decalage_premier>0;Decalage_premier--)
      Element_courant=Element_courant->Suivant;

    // Ensuite, on saute autant d'�l�ments que le d�calage demand�:
    for (;Decalage_select>0;Decalage_select--)
      Element_courant=Element_courant->Suivant;

    // On recopie la cha�ne
    strcpy(label, Element_courant->NomComplet);

    if (type != NULL)
      *type=Element_courant->Type;
  } // Fin du test d'existence de fichiers
}


// ----------------- D�placements dans la liste de fichiers -----------------

void Select_Scroll_Down(short * Decalage_premier,short * Decalage_select)
// Fait scroller vers le bas le s�lecteur de fichier... (si possible)
{
  if ( ((*Decalage_select)<9)
    && ( (*Decalage_select)+1 < Liste_Nb_elements ) )
    // Si la s�lection peut descendre
    Afficher_la_liste_des_fichiers(*Decalage_premier,++(*Decalage_select));
  else // Sinon, descendre la fen�tre (si possible)
  if ((*Decalage_premier)+10<Liste_Nb_elements)
    Afficher_la_liste_des_fichiers(++(*Decalage_premier),*Decalage_select);
}


void Select_Scroll_Up(short * Decalage_premier,short * Decalage_select)
// Fait scroller vers le haut le s�lecteur de fichier... (si possible)
{
  if ((*Decalage_select)>0)
    // Si la s�lection peut monter
    Afficher_la_liste_des_fichiers(*Decalage_premier,--(*Decalage_select));
  else // Sinon, monter la fen�tre (si possible)
  if ((*Decalage_premier)>0)
    Afficher_la_liste_des_fichiers(--(*Decalage_premier),*Decalage_select);
}


void Select_Page_Down(short * Decalage_premier,short * Decalage_select, short lines)
{
  if (Liste_Nb_elements-1>*Decalage_premier+*Decalage_select)
  {
    if (*Decalage_select<9)
    {
      if (Liste_Nb_elements<10)
      {
        *Decalage_premier=0;
        *Decalage_select=Liste_Nb_elements-1;
      }
      else *Decalage_select=9;
    }
    else
    {
      if (Liste_Nb_elements>*Decalage_premier+18)
        *Decalage_premier+=lines;
      else
      {
        *Decalage_premier=Liste_Nb_elements-10;
        *Decalage_select=9;
      }
    }
  }
  Afficher_la_liste_des_fichiers(*Decalage_premier,*Decalage_select);
}


void Select_Page_Up(short * Decalage_premier,short * Decalage_select, short lines)
{
  if (*Decalage_premier+*Decalage_select>0)
  {
    if (*Decalage_select>0)
      *Decalage_select=0;
    else
    {
      if (*Decalage_premier>lines)
        *Decalage_premier-=lines;
      else
        *Decalage_premier=0;
    }
  }
  Afficher_la_liste_des_fichiers(*Decalage_premier,*Decalage_select);
}


void Select_End(short * Decalage_premier,short * Decalage_select)
{
  if (Liste_Nb_elements<10)
  {
    *Decalage_premier=0;
    *Decalage_select=Liste_Nb_elements-1;
  }
  else
  {
    *Decalage_premier=Liste_Nb_elements-10;
    *Decalage_select=9;
  }
  Afficher_la_liste_des_fichiers(*Decalage_premier,*Decalage_select);
}


void Select_Home(short * Decalage_premier,short * Decalage_select)
{
  Afficher_la_liste_des_fichiers((*Decalage_premier)=0,(*Decalage_select)=0);
}



short Calculer_decalage_click_dans_fileselector(void)
/*
  Renvoie le d�calage dans le s�lecteur de fichier sur lequel on a click�.
  Renvoie le d�calage du dernier fichier si on a click� au del�.
  Renvoie -1 si le s�lecteur est vide.
*/
{
  short Decalage_calcule;

  Decalage_calcule=(((Mouse_Y-Fenetre_Pos_Y)/Menu_Facteur_Y)-(90+FILENAMESPACE))>>3;
  if (Decalage_calcule>=Liste_Nb_elements)
    Decalage_calcule=Liste_Nb_elements-1;

  return Decalage_calcule;
}

void Afficher_bookmark(T_Bouton_dropdown * Bouton, int Numero_bookmark)
{
  if (Config.Bookmark_directory[Numero_bookmark])
  {
    int label_size;
    // Libell�
    Print_dans_fenetre_limite(Bouton->Pos_X+3+10,Bouton->Pos_Y+2,Config.Bookmark_label[Numero_bookmark],8,CM_Noir,CM_Clair);
    label_size=strlen(Config.Bookmark_label[Numero_bookmark]);
    if (label_size<8)
      Block(Fenetre_Pos_X+(Menu_Facteur_X*(Bouton->Pos_X+3+10+label_size*8)),Fenetre_Pos_Y+(Menu_Facteur_Y*(Bouton->Pos_Y+2)),Menu_Facteur_X*(8-label_size)*8,Menu_Facteur_Y*8,CM_Clair);
    // Menu apparait sur clic droit
    Bouton->Bouton_actif=A_DROITE;
    // Choix actifs
    Fenetre_Dropdown_vider_choix(Bouton);
    Fenetre_Dropdown_choix(Bouton,0,"Set");
    Fenetre_Dropdown_choix(Bouton,1,"Rename");
    Fenetre_Dropdown_choix(Bouton,2,"Clear");    
  }
  else
  {
    // Libell�
    Print_dans_fenetre(Bouton->Pos_X+3+10,Bouton->Pos_Y+2,"--------",CM_Fonce,CM_Clair);
    // Menu apparait sur clic droit ou gauche
    Bouton->Bouton_actif=A_DROITE|A_GAUCHE;
    // Choix actifs
    Fenetre_Dropdown_vider_choix(Bouton);
    Fenetre_Dropdown_choix(Bouton,0,"Set");
  }
}

//------------------------ Chargements et sauvegardes ------------------------

void Print_repertoire_courant(void)
//
// Affiche Principal_Repertoire_courant sur 37 caract�res
//
{
  char Nom_temporaire[TAILLE_MAXI_PATH+1]; // Nom tronqu�
  int  length; // length du r�pertoire courant
  int  Indice;   // Indice de parcours de la chaine compl�te

  Block(Fenetre_Pos_X+(Menu_Facteur_X*7),Fenetre_Pos_Y+(Menu_Facteur_Y*43),Menu_Facteur_X*37*8,Menu_Facteur_Y<<3,CM_Clair);

  length=strlen(Principal_Repertoire_courant);
  if (length>TAILLE_MAXI_PATH)
  { // Doh! il va falloir tronquer le r�pertoire (bouh !)

    // On commence par copier b�tement les 3 premiers caract�res (e.g. "C:\")
    for (Indice=0;Indice<3;Indice++)
      Nom_temporaire[Indice]=Principal_Repertoire_courant[Indice];

    // On y rajoute 3 petits points:
    strcpy(Nom_temporaire+3,"...");

    //  Ensuite, on cherche un endroit � partir duquel on pourrait loger tout
    // le reste de la chaine (Ouaaaaaah!!! Vachement fort le mec!!)
    for (Indice++;Indice<length;Indice++)
      if ( (Principal_Repertoire_courant[Indice]==SEPARATEUR_CHEMIN[0]) &&
           (length-Indice<=TAILLE_MAXI_PATH-6) )
      {
        // Ouf: on vient de trouver un endroit dans la cha�ne � partir duquel
        // on peut faire la copie:
        strcpy(Nom_temporaire+6,Principal_Repertoire_courant+Indice);
        break;
      }

    // Enfin, on peut afficher la cha�ne tronqu�e
    Print_dans_fenetre(7,43,Nom_temporaire,CM_Noir,CM_Clair);
  }
  else // Ahhh! La cha�ne peut loger tranquillement dans la fen�tre
    Print_dans_fenetre(7,43,Principal_Repertoire_courant,CM_Noir,CM_Clair);
    
  UpdateRect(Fenetre_Pos_X+(Menu_Facteur_X*7),Fenetre_Pos_Y+(Menu_Facteur_Y*43),Menu_Facteur_X*37*8,Menu_Facteur_Y<<3);
}


void Print_Nom_fichier_dans_selecteur(void)
//
// Affiche Principal_Nom_fichier dans le Fileselect
//
{
  Block(Fenetre_Pos_X+(Menu_Facteur_X*(13+9*8)),Fenetre_Pos_Y+(Menu_Facteur_Y*90),Menu_Facteur_X*(27*8),Menu_Facteur_Y<<3,CM_Clair);
  Print_dans_fenetre_limite(13+9*8,90,Principal_Nom_fichier,27,CM_Noir,CM_Clair);
  UpdateRect(Fenetre_Pos_X+(Menu_Facteur_X*(13+9*8)),Fenetre_Pos_Y+(Menu_Facteur_Y*90),Menu_Facteur_X*(27*8),Menu_Facteur_Y<<3);
}

int   Type_selectionne; // Utilis� pour m�moriser le type d'entr�e choisi
                        // dans le selecteur de fichier.

void Preparer_et_afficher_liste_fichiers(short Position, short offset,
                                         T_Bouton_scroller * button)
{
  button->Nb_elements=Liste_Nb_elements;
  button->Position=Position;
  Calculer_hauteur_curseur_jauge(button);
  Fenetre_Dessiner_jauge(button);
  // On efface les anciens noms de fichier:
  Block(Fenetre_Pos_X+(Menu_Facteur_X<<3),Fenetre_Pos_Y+(Menu_Facteur_Y*(89+FILENAMESPACE)),Menu_Facteur_X*98,Menu_Facteur_Y*82,CM_Noir);
  // On affiche les nouveaux:
  Afficher_la_liste_des_fichiers(Position,offset);

  UpdateRect(Fenetre_Pos_X+(Menu_Facteur_X<<3),Fenetre_Pos_Y+(Menu_Facteur_Y*(89+FILENAMESPACE)),Menu_Facteur_X*98,Menu_Facteur_Y*82);

  // On r�cup�re le nom du schmilblick � "acc�der"
  Determiner_element_de_la_liste(Position,offset,Principal_Nom_fichier,&Type_selectionne);
  // On affiche le nouveau nom de fichier
  Print_Nom_fichier_dans_selecteur();
  // On affiche le nom du r�pertoire courant
  Print_repertoire_courant();
}


void Relire_liste_fichiers(byte Filtre, short Position, short offset,
                           T_Bouton_scroller * button)
{
  Lire_liste_des_fichiers(Filtre);
  Trier_la_liste_des_fichiers();
  Preparer_et_afficher_liste_fichiers(Position,offset,button);
}

void On_vient_de_scroller_dans_le_fileselect(T_Bouton_scroller * Scroller_de_fichiers)
{
  char Ancien_nom_de_fichier[TAILLE_CHEMIN_FICHIER];

  strcpy(Ancien_nom_de_fichier,Principal_Nom_fichier);

  // On regarde si la liste a boug�
  if (Scroller_de_fichiers->Position!=Principal_File_list_Position)
  {
    // Si c'est le cas, il faut mettre � jour la jauge
    Scroller_de_fichiers->Position=Principal_File_list_Position;
    Fenetre_Dessiner_jauge(Scroller_de_fichiers);
  }
  // On r�cup�re le nom du schmilblick � "acc�der"
  Determiner_element_de_la_liste(Principal_File_list_Position,Principal_File_list_Decalage,Principal_Nom_fichier,&Type_selectionne);
  if (strcmp(Ancien_nom_de_fichier,Principal_Nom_fichier))
    Nouvelle_preview=1;

  // On affiche le nouveau nom de fichier
  Print_Nom_fichier_dans_selecteur();
  Afficher_curseur();
}


short Position_fichier_dans_liste(char * fname)
{
  Element_de_liste_de_fileselect * Element_courant;
  short  Indice;

  for (Indice=0, Element_courant=Liste_du_fileselect;
       ((Element_courant!=NULL) && (strcmp(Element_courant->NomComplet,fname)));
       Indice++,Element_courant=Element_courant->Suivant);

  return (Element_courant!=NULL)?Indice:0;
}


void Placer_barre_de_selection_sur(char * fname)
{
  short Indice;

  Indice=Position_fichier_dans_liste(fname);

  if ((Liste_Nb_elements<=10) || (Indice<5))
  {
    Principal_File_list_Position=0;
    Principal_File_list_Decalage=Indice;
  }
  else
  {
    if (Indice>=Liste_Nb_elements-5)
    {
      Principal_File_list_Position=Liste_Nb_elements-10;
      Principal_File_list_Decalage=Indice-Principal_File_list_Position;
    }
    else
    {
      Principal_File_list_Position=Indice-4;
      Principal_File_list_Decalage=4;
    }
  }
}


char FFF_Meilleur_nom[TAILLE_CHEMIN_FICHIER];
char * Nom_correspondant_le_mieux_a(char * fname)
{
  char * Pointeur_Meilleur_nom;
  Element_de_liste_de_fileselect * Element_courant;
  byte   Lettres_identiques=0;
  byte   counter;

  strcpy(FFF_Meilleur_nom,Principal_Nom_fichier);
  Pointeur_Meilleur_nom=NULL;

  for (Element_courant=Liste_du_fileselect; Element_courant!=NULL; Element_courant=Element_courant->Suivant)
  {
    if ( (!Config.Find_file_fast)
      || (Config.Find_file_fast==(Element_courant->Type+1)) )
    {
      // On compare et si c'est mieux, on stocke dans Meilleur_nom
      for (counter=0; fname[counter]!='\0' && tolower(Element_courant->NomComplet[counter])==tolower(fname[counter]); counter++);
      if (counter>Lettres_identiques)
      {
        Lettres_identiques=counter;
        strcpy(FFF_Meilleur_nom,Element_courant->NomComplet);
        Pointeur_Meilleur_nom=Element_courant->NomComplet;
      }
    }
  }

  return Pointeur_Meilleur_nom;
}

byte Bouton_Load_ou_Save(byte load, byte image)
  // load=1 => On affiche le menu du bouton LOAD
  // load=0 => On affiche le menu du bouton SAVE
{
  short Bouton_clicke;
  T_Bouton_scroller * Scroller_de_fichiers;
  T_Bouton_dropdown * Dropdown_des_formats;
  T_Bouton_dropdown * Dropdown_bookmark[4];
  short Temp;
  int Bidon=0;       // Sert � appeler SDL_GetKeyState
  byte  Charger_ou_sauver_l_image=0;
  byte  On_a_clicke_sur_OK=0;// Indique si on a click� sur Load ou Save ou sur
                             //un bouton enclenchant Load ou Save juste apr�s.
  Composantes * Palette_initiale; // |  Donn�es concernant l'image qui
  byte  Image_modifiee_initiale;         // |  sont m�moris�es pour pouvoir
  short Largeur_image_initiale;          // |- �tre restaur�es en sortant,
  short Hauteur_image_initiale;          // |  parce que la preview elle les
  byte  Back_color_initiale;             // |  fout en l'air (c'te conne).
  char  Nom_fichier_initial[TAILLE_CHEMIN_FICHIER]; // Sert � laisser le nom courant du fichier en cas de sauvegarde
  char  Repertoire_precedent[TAILLE_CHEMIN_FICHIER]; // R�pertoire d'o� l'on vient apr�s un CHDIR
  char  Commentaire_initial[TAILLE_COMMENTAIRE+1];
  char  Fichier_recherche[TAILLE_CHEMIN_FICHIER]="";
  char  Nom_fichier_Save[TAILLE_CHEMIN_FICHIER];
  char * Fichier_le_plus_ressemblant;

  Palette_initiale=(Composantes *)malloc(sizeof(T_Palette));
  memcpy(Palette_initiale,Principal_Palette,sizeof(T_Palette));

  Back_color_initiale=Back_color;
  Image_modifiee_initiale=Principal_Image_modifiee;
  Largeur_image_initiale=Principal_Largeur_image;
  Hauteur_image_initiale=Principal_Hauteur_image;
  strcpy(Nom_fichier_initial,Principal_Nom_fichier);
  strcpy(Commentaire_initial,Principal_Commentaire);
  if (load)
  {
    if (image)
      Ouvrir_fenetre(310,187+FILENAMESPACE,"Load picture");
    else
      Ouvrir_fenetre(310,187+FILENAMESPACE,"Load brush");
    Fenetre_Definir_bouton_normal(125,157+FILENAMESPACE,51,14,"Load",0,1,SDLK_RETURN); // 1
  }
  else
  {
    if (image)
      Ouvrir_fenetre(310,187+FILENAMESPACE,"Save picture");
    else
      Ouvrir_fenetre(310,187+FILENAMESPACE,"Save brush");
    Fenetre_Definir_bouton_normal(125,157+FILENAMESPACE,51,14,"Save",0,1,SDLK_RETURN); // 1
    if (Principal_Format==0) // Correction du *.*
    {
      Principal_Format=Principal_Format_fichier;
      Principal_File_list_Position=0;
      Principal_File_list_Decalage=0;
    }

    if (Principal_Format>NB_FORMATS_SAVE) // Correction d'un format insauvable
    {
      Principal_Format=FORMAT_PAR_DEFAUT;
      Principal_File_list_Position=0;
      Principal_File_list_Decalage=0;
    }
    // Affichage du commentaire
    if (FormatFichier[Principal_Format-1].Commentaire)
      Print_dans_fenetre(46,175+FILENAMESPACE,Principal_Commentaire,CM_Noir,CM_Clair);
  }

  Fenetre_Definir_bouton_normal(125,139+FILENAMESPACE,51,14,"Cancel",0,1,TOUCHE_ESC);  // 2
  Fenetre_Definir_bouton_normal(125, 89+FILENAMESPACE,51,14,"Delete",0,1,SDLK_DELETE); // 3

  // Cadre autour des formats
  Fenetre_Afficher_cadre(  7, 51,104, 35);
  // Cadre autour des infos sur le fichier de dessin
  Fenetre_Afficher_cadre(116, 51,187, 35);
  // Cadre autour de la preview
  Fenetre_Afficher_cadre_creux(179,88+FILENAMESPACE,124,84);
  // Cadre autour du fileselector
  Fenetre_Afficher_cadre_creux(  7,88+FILENAMESPACE,100,84);

  Fenetre_Definir_bouton_special(9,90+FILENAMESPACE,96,80);                      // 4

  // Scroller du fileselector
  Scroller_de_fichiers = Fenetre_Definir_bouton_scroller(110,89+FILENAMESPACE,82,1,10,0);               // 5

  // Dropdown pour les formats de fichier
  Dropdown_des_formats=
    Fenetre_Definir_bouton_dropdown(70,56,36,16,0,
      (Principal_Format==0)?"*.*":FormatFichier[Principal_Format-1].Extension,
      1,0,1,A_DROITE|A_GAUCHE); // 6
  if (load)
    Fenetre_Dropdown_choix(Dropdown_des_formats,0,"*.*");
  for (Temp=0;Temp<NB_FORMATS_CONNUS;Temp++)
  {
    if ((load && FormatFichier[Temp].Load) || 
      (!load && FormatFichier[Temp].Save))
        Fenetre_Dropdown_choix(Dropdown_des_formats,Temp+1,FormatFichier[Temp].Extension);
  }
  Print_dans_fenetre(12,61,"Format:",CM_Fonce,CM_Clair);
  
  // Texte de commentaire des dessins
  Print_dans_fenetre(7,174+FILENAMESPACE,"Txt:",CM_Fonce,CM_Clair);
  Fenetre_Definir_bouton_saisie(44,173+FILENAMESPACE,TAILLE_COMMENTAIRE); // 7

  // Cadre autour du nom de fichier
  //Fenetre_Afficher_cadre_creux(  7,87,296,15);
  Print_dans_fenetre(9,90,"Filename:",CM_Fonce,CM_Clair);
  // Saisie du nom de fichier
  Fenetre_Definir_bouton_saisie(11+9*8,88,27); // 8

  Print_dans_fenetre(120,55,"Image size :",CM_Fonce,CM_Clair);
  Print_dans_fenetre(120,63,"File size  :",CM_Fonce,CM_Clair);
  Print_dans_fenetre(120,72,"Format     :",CM_Fonce,CM_Clair);

  // Selecteur de Lecteur / Volume
  Fenetre_Definir_bouton_normal(8,17,117,23,"Select drive",0,1,SDLK_LAST); // 9

  // Bookmarks
  for (Temp=0;Temp<NB_BOOKMARKS;Temp++)
  {
    Dropdown_bookmark[Temp]=
      Fenetre_Definir_bouton_dropdown(126+(88+1)*(Temp%2),17+(Temp/2)*12,88,11,56,"",0,0,1,A_DROITE); // 10-13
    Fenetre_Afficher_sprite_drive(Dropdown_bookmark[Temp]->Pos_X+3,Dropdown_bookmark[Temp]->Pos_Y+2,5);
    Afficher_bookmark(Dropdown_bookmark[Temp],Temp);
  }
  // On prend bien soin de passer dans le r�pertoire courant (le bon qui faut! Oui madame!)
  if (load)
  {
    chdir(Principal_Repertoire_courant);
    getcwd(Principal_Repertoire_courant,256);
  }
  else
  {
    chdir(Principal_Repertoire_fichier);
    getcwd(Principal_Repertoire_courant,256);
  }
  
  // Affichage des premiers fichiers visibles:
  Relire_liste_fichiers(Principal_Format,Principal_File_list_Position,Principal_File_list_Decalage,Scroller_de_fichiers);

  if (!load)
  {
    // On initialise le nom de fichier � celui en cours et non pas celui sous
    // la barre de s�lection
    strcpy(Principal_Nom_fichier,Nom_fichier_initial);
    // On affiche le nouveau nom de fichier
    Print_Nom_fichier_dans_selecteur();
  }

  Pixel_de_chargement=Pixel_Chargement_dans_preview;
  Nouvelle_preview=1;
  Display_Window(310,(187+FILENAMESPACE));

  Afficher_curseur();

  do
  {
    Bouton_clicke=Fenetre_Bouton_clicke();

    switch (Bouton_clicke)
    {
      case -1 :
      case  0 :
        break;

      case  1 : // Load ou Save
      if(load)
        {
          // Determine the type
          if(Fichier_existe(Principal_Nom_fichier)) 
          {
            Type_selectionne = 0;
            if(Repertoire_existe(Principal_Nom_fichier)) Type_selectionne = 1;
          }
          else
          {
            Type_selectionne = 1;
          }
        }
        else
        {
          if(Repertoire_existe(Principal_Nom_fichier)) Type_selectionne = 1;
          else Type_selectionne = 0;
        }
        On_a_clicke_sur_OK=1;
        break;

      case  2 : // Cancel
        break;

      case  3 : // Delete
        if (Liste_Nb_elements && (*Principal_Nom_fichier!='.') && Type_selectionne!=2)
        {
          char * Message;
          Effacer_curseur();
          // On affiche une demande de confirmation
          if (Principal_File_list_Position+Principal_File_list_Decalage>=Liste_Nb_repertoires)
          {
            Message="Delete file ?";
          }
          else
          {
            Message="Remove directory ?";
          }
          if (Demande_de_confirmation(Message))
          {
            // Si c'est un fichier
            if (Principal_File_list_Position+Principal_File_list_Decalage>=Liste_Nb_repertoires)
              // On efface le fichier (si on peut)
              Temp=(!remove(Principal_Nom_fichier));
            else // Si c'est un repertoire
              // On efface le repertoire (si on peut)
              Temp=(!rmdir(Principal_Nom_fichier));

            if (Temp) // Temp indique si l'effacement s'est bien pass�
            {
              // On remonte si c'�tait le dernier �l�ment de la liste
              if (Principal_File_list_Position+Principal_File_list_Decalage==Liste_Nb_elements-1)
              {
                if (Principal_File_list_Position)
                  Principal_File_list_Position--;
                else
                  if (Principal_File_list_Decalage)
                    Principal_File_list_Decalage--;
              }
              else // Si ce n'�tait pas le dernier, il faut faire gaffe � ce
              {    // que ses copains d'en dessous ne remontent pas trop.
                if ( (Principal_File_list_Position)
                  && (Principal_File_list_Position+10==Liste_Nb_elements) )
                  {
                    Principal_File_list_Position--;
                    Principal_File_list_Decalage++;
                  }
              }
              // On relit les informations
              Relire_liste_fichiers(Principal_Format,Principal_File_list_Position,Principal_File_list_Decalage,Scroller_de_fichiers);
              // On demande la preview du nouveau fichier sur lequel on se trouve
              Nouvelle_preview=1;
            }
            else
              Erreur(0);

            // On place la barre de s�lection du brouillon au d�but s'il a le
            // m�me r�pertoire que l'image principale.
            if (!strcmp(Principal_Repertoire_courant,Brouillon_Repertoire_courant))
            {
              Brouillon_File_list_Position=0;
              Brouillon_File_list_Decalage=0;
            }
          }
        }
        break;

      case  4 : // Zone d'affichage de la liste de fichiers
        Effacer_curseur();

        Temp=Calculer_decalage_click_dans_fileselector();
        if (Temp>=0)
        {
          if (Temp!=Principal_File_list_Decalage)
          {
            // On met � jour le d�calage
            Principal_File_list_Decalage=Temp;

            // On r�cup�re le nom du schmilblick � "acc�der"
            Determiner_element_de_la_liste(Principal_File_list_Position,Principal_File_list_Decalage,Principal_Nom_fichier,&Type_selectionne);
            // On affiche le nouveau nom de fichier
            Print_Nom_fichier_dans_selecteur();
            // On affiche � nouveau la liste
            Afficher_la_liste_des_fichiers(Principal_File_list_Position,Principal_File_list_Decalage);

            // On vient de changer de nom de fichier, donc on doit s'appreter
            // a rafficher une preview
            Nouvelle_preview=1;
            *Fichier_recherche=0;
          }
          else
          {
            //   En sauvegarde, si on a double-click� sur un r�pertoire, il
            // faut mettre le nom de fichier au nom du r�pertoire. Sinon, dans
            // certains cas, on risque de sauvegarder avec le nom du fichier
            // actuel au lieu de changer de r�pertoire.
            if (Principal_File_list_Position+Principal_File_list_Decalage<Liste_Nb_repertoires)
              Determiner_element_de_la_liste(Principal_File_list_Position,Principal_File_list_Decalage,Principal_Nom_fichier,&Type_selectionne);

            On_a_clicke_sur_OK=1;
            Nouvelle_preview=1;
            *Fichier_recherche=0;
          }
        }
        Afficher_curseur();
        Attendre_fin_de_click();
        break;

      case  5 : // Scroller de fichiers
        Effacer_curseur();
        Principal_File_list_Position=Fenetre_Attribut2;
        // On r�cup�re le nom du schmilblick � "acc�der"
        Determiner_element_de_la_liste(Principal_File_list_Position,Principal_File_list_Decalage,Principal_Nom_fichier,&Type_selectionne);
        // On affiche le nouveau nom de fichier
        Print_Nom_fichier_dans_selecteur();
        // On affiche � nouveau la liste
        Afficher_la_liste_des_fichiers(Principal_File_list_Position,Principal_File_list_Decalage);
        Afficher_curseur();
        Nouvelle_preview=1;
        *Fichier_recherche=0;
        break;

      case  6 : // Scroller des formats
        Effacer_curseur();
        // On met � jour le format de browsing du fileselect:
        Principal_Format=Fenetre_Attribut2;
        // Comme on change de liste, on se place en d�but de liste:
        Principal_File_list_Position=0;
        Principal_File_list_Decalage=0;
        // Affichage des premiers fichiers visibles:
        Relire_liste_fichiers(Principal_Format,Principal_File_list_Position,Principal_File_list_Decalage,Scroller_de_fichiers);
        Afficher_curseur();
        Nouvelle_preview=1;
        *Fichier_recherche=0;
        break;
      case  7 : // Saisie d'un commentaire pour la sauvegarde
        if ( (!load) && (FormatFichier[Principal_Format-1].Commentaire) )
        {
          Readline(46,175+FILENAMESPACE,Principal_Commentaire,32,0);
          Afficher_curseur();
        }
        break;
      case  8 : // Saisie du nom de fichier

        // Save the filename
        strcpy(Nom_fichier_Save, Principal_Nom_fichier);

        if (Readline(13+9*8,90,Principal_Nom_fichier,27,2))
        {
          //   On regarde s'il faut rajouter une extension. C'est-�-dire s'il
          // n'y a pas de '.' dans le nom du fichier.
          for(Temp=0,Bidon=0; ((Principal_Nom_fichier[Temp]) && (!Bidon)); Temp++)
            if (Principal_Nom_fichier[Temp]=='.')
              Bidon=1;
          if (!Bidon)
          {
            if (Principal_Format)
            {
              if(!Repertoire_existe(Principal_Nom_fichier))
              {
                 strcat(Principal_Nom_fichier,".");
                 strcat(Principal_Nom_fichier,FormatFichier[Principal_Format-1].Extension);
              }
            }
            else
            {
              // put default extension
              // (but maybe we should browse through all available ones until we find
              //  something suitable ?)
              if(!Repertoire_existe(Principal_Nom_fichier))
              {
                 strcat(Principal_Nom_fichier, ".pkm");
              }
            }
          }
          if(load)
          {
            // Determine the type
            if(Fichier_existe(Principal_Nom_fichier)) 
            {
              Type_selectionne = 0;
              if(Repertoire_existe(Principal_Nom_fichier)) Type_selectionne = 1;
            }
            else
            {
              Type_selectionne = 1;
            }
          }
          else
          {
            if(Repertoire_existe(Principal_Nom_fichier)) Type_selectionne = 1;
            else Type_selectionne = 0;
          }
          On_a_clicke_sur_OK=1;
        }
        else
        {
          // Restore the old filename
          strcpy(Principal_Nom_fichier, Nom_fichier_Save);
          Print_Nom_fichier_dans_selecteur();
        }
        Afficher_curseur();
        break;
      case  9 : // Volume Select
          Effacer_curseur();
          //   Comme on tombe sur un disque qu'on connait pas, on se place en
          // d�but de liste:
          Principal_File_list_Position=0;
          Principal_File_list_Decalage=0;
          // Affichage des premiers fichiers visibles:
          Lire_liste_des_lecteurs();
          Trier_la_liste_des_fichiers();
          Preparer_et_afficher_liste_fichiers(Principal_File_list_Position,Principal_File_list_Decalage,Scroller_de_fichiers);
          Afficher_curseur();
          Nouvelle_preview=1;
          *Fichier_recherche=0;
          break;
      default:
          if (Bouton_clicke>=10 && Bouton_clicke<10+NB_BOOKMARKS)
          {
            // Bookmark
            char * Nom_repertoire;
            
            switch(Fenetre_Attribut2)
            {
              case -1: // bouton lui-m�me: aller au r�pertoire m�moris�
                if (Config.Bookmark_directory[Bouton_clicke-10])
                {
                  *Fichier_recherche=0;
                  strcpy(Principal_Nom_fichier,Config.Bookmark_directory[Bouton_clicke-10]);
                  Type_selectionne=1;
                  On_a_clicke_sur_OK=1;
                  *Fichier_recherche=0;
                }
                break;
                
              case 0: // Set
                if (Config.Bookmark_directory[Bouton_clicke-10])
                  free(Config.Bookmark_directory[Bouton_clicke-10]);
                Config.Bookmark_label[Bouton_clicke-10][0]='\0';
                Temp=strlen(Principal_Repertoire_courant);
                Config.Bookmark_directory[Bouton_clicke-10]=malloc(Temp+1);
                strcpy(Config.Bookmark_directory[Bouton_clicke-10],Principal_Repertoire_courant);
                
                Nom_repertoire=Position_dernier_slash(Principal_Repertoire_courant);
                if (Nom_repertoire && Nom_repertoire[1]!='\0')
                  Nom_repertoire++;
                else
                  Nom_repertoire=Principal_Repertoire_courant;
                Temp=strlen(Nom_repertoire);
                strncpy(Config.Bookmark_label[Bouton_clicke-10],Nom_repertoire,8);
                if (Temp>8)
                {
                  Config.Bookmark_label[Bouton_clicke-10][7]=CARACTERE_SUSPENSION;
                  Config.Bookmark_label[Bouton_clicke-10][8]='\0';
                }
                Afficher_bookmark(Dropdown_bookmark[Bouton_clicke-10],Bouton_clicke-10);
                break;
                
              case 1: // Rename
                if (Config.Bookmark_directory[Bouton_clicke-10])
                {
                  // On enl�ve les "..." avant l'�dition
                  char bookmark_label[8+1];
                  strcpy(bookmark_label, Config.Bookmark_label[Bouton_clicke-10]);
                  if (bookmark_label[7]==CARACTERE_SUSPENSION)
                    bookmark_label[7]='\0';
                  if (Readline_ex(Dropdown_bookmark[Bouton_clicke-10]->Pos_X+3+10,Dropdown_bookmark[Bouton_clicke-10]->Pos_Y+2,bookmark_label,8,8,0))
                    strcpy(Config.Bookmark_label[Bouton_clicke-10],bookmark_label);
                  Afficher_bookmark(Dropdown_bookmark[Bouton_clicke-10],Bouton_clicke-10);
                  Afficher_curseur();
                }
                break;

              case 2: // Clear
                if (Config.Bookmark_directory[Bouton_clicke-10] && Demande_de_confirmation("Erase bookmark ?"))
                {
                  free(Config.Bookmark_directory[Bouton_clicke-10]);
                  Config.Bookmark_directory[Bouton_clicke-10]=NULL;
                  Config.Bookmark_label[Bouton_clicke-10][0]='\0';
                  Afficher_bookmark(Dropdown_bookmark[Bouton_clicke-10],Bouton_clicke-10);
                }
                break;
            }
          }
          break;
    }

    switch (Touche)
    {
      case SDLK_UNKNOWN : break;
      case SDLK_DOWN : // Bas
        *Fichier_recherche=0;
        Effacer_curseur();
        Select_Scroll_Down(&Principal_File_list_Position,&Principal_File_list_Decalage);
        On_vient_de_scroller_dans_le_fileselect(Scroller_de_fichiers);
        Touche=0;
        break;
      case SDLK_UP : // Haut
        *Fichier_recherche=0;
        Effacer_curseur();
        Select_Scroll_Up(&Principal_File_list_Position,&Principal_File_list_Decalage);
        On_vient_de_scroller_dans_le_fileselect(Scroller_de_fichiers);
        Touche=0;
        break;
      case SDLK_PAGEDOWN : // PageDown
        *Fichier_recherche=0;
        Effacer_curseur();
        Select_Page_Down(&Principal_File_list_Position,&Principal_File_list_Decalage,9);
        On_vient_de_scroller_dans_le_fileselect(Scroller_de_fichiers);
        Touche=0;
        break;
      case SDLK_PAGEUP : // PageUp
        *Fichier_recherche=0;
        Effacer_curseur();
        Select_Page_Up(&Principal_File_list_Position,&Principal_File_list_Decalage,9);
        On_vient_de_scroller_dans_le_fileselect(Scroller_de_fichiers);
        Touche=0;
        break;
      case SDLK_END : // End
        *Fichier_recherche=0;
        Effacer_curseur();
        Select_End(&Principal_File_list_Position,&Principal_File_list_Decalage);
        On_vient_de_scroller_dans_le_fileselect(Scroller_de_fichiers);
        Touche=0;
        break;
      case SDLK_HOME : // Home
        *Fichier_recherche=0;
        Effacer_curseur();
        Select_Home(&Principal_File_list_Position,&Principal_File_list_Decalage);
        On_vient_de_scroller_dans_le_fileselect(Scroller_de_fichiers);
        Touche=0;
        break;
      case TOUCHE_MOUSEWHEELDOWN :
        *Fichier_recherche=0;
        Effacer_curseur();
        Select_Page_Down(&Principal_File_list_Position,&Principal_File_list_Decalage,3);
        On_vient_de_scroller_dans_le_fileselect(Scroller_de_fichiers);
        Touche=0;
        break;
      case TOUCHE_MOUSEWHEELUP :
        *Fichier_recherche=0;
        Effacer_curseur();
        Select_Page_Up(&Principal_File_list_Position,&Principal_File_list_Decalage,3);
        On_vient_de_scroller_dans_le_fileselect(Scroller_de_fichiers);
        Touche=0;
        break;
      case SDLK_BACKSPACE : // Backspace
        *Fichier_recherche=0;
        // Si le choix ".." est bien en t�te des propositions...
        if (!strcmp(Liste_du_fileselect->NomComplet,PARENT_DIR))
        {                              
          // On va dans le r�pertoire parent.
          strcpy(Principal_Nom_fichier,PARENT_DIR);
          Type_selectionne=1;
          On_a_clicke_sur_OK=1;
        }
        Touche=0;
        break;
      default: // Autre => On se place sur le nom de fichier qui correspond
        if (Bouton_clicke<=0)
        {
          if (Est_Raccourci(Touche,0x100+BOUTON_AIDE))
          {
            Fenetre_aide(load?BOUTON_CHARGER:BOUTON_SAUVER, NULL);
            break;
          }
          Temp=strlen(Fichier_recherche);
          if (Touche_ANSI>= ' ' && Touche_ANSI < 255 && Temp<50)
          {
            Fichier_recherche[Temp]=Touche_ANSI;
            Fichier_recherche[Temp+1]='\0';
            Fichier_le_plus_ressemblant=Nom_correspondant_le_mieux_a(Fichier_recherche);
            if ( (Fichier_le_plus_ressemblant) )
            {
              Temp=Principal_File_list_Position+Principal_File_list_Decalage;
              Effacer_curseur();
              Placer_barre_de_selection_sur(Fichier_le_plus_ressemblant);
              Preparer_et_afficher_liste_fichiers(Principal_File_list_Position,Principal_File_list_Decalage,Scroller_de_fichiers);
              Afficher_curseur();
              if (Temp!=Principal_File_list_Position+Principal_File_list_Decalage)
                Nouvelle_preview=1;
            }
            else
              *Fichier_recherche=0;
            Touche=0;
          }
        }
        else
          *Fichier_recherche=0;
    }

    if (On_a_clicke_sur_OK)
    {
      //   Si c'est un r�pertoire, on annule "On_a_clicke_sur_OK" et on passe
      // dedans.
      if (Type_selectionne!=0)
      {
        Effacer_curseur();
        On_a_clicke_sur_OK=0;

        // On m�morise le r�pertoire dans lequel on �tait
        if (strcmp(Principal_Nom_fichier,PARENT_DIR))
          strcpy(Repertoire_precedent,Nom_formate(PARENT_DIR, 1));
        else
        {
          strcpy(Repertoire_precedent,
            Nom_formate(Position_dernier_slash(Principal_Repertoire_courant), 1)
            );
        }

        // On doit rentrer dans le r�pertoire:
        if (!chdir(Principal_Nom_fichier))
        {
          getcwd(Principal_Repertoire_courant,256);
  
          // On lit le nouveau r�pertoire
          Lire_liste_des_fichiers(Principal_Format);
          Trier_la_liste_des_fichiers();
          // On place la barre de s�lection sur le r�pertoire d'o� l'on vient
          Placer_barre_de_selection_sur(Repertoire_precedent);
        }
        else
          Erreur(0);
        // Affichage des premiers fichiers visibles:
        Preparer_et_afficher_liste_fichiers(Principal_File_list_Position,Principal_File_list_Decalage,Scroller_de_fichiers);
        Afficher_curseur();
        Nouvelle_preview=1;
      }
      else  // Sinon on essaye de charger ou sauver le fichier
      {
        strcpy(Principal_Repertoire_fichier,Principal_Repertoire_courant);
        if (!load)
          Principal_Format_fichier=Principal_Format;
        Charger_ou_sauver_l_image=1;
      }
    }

    // Gestion du chrono et des previews
    if (Nouvelle_preview)
    {
      // On efface les infos de la preview pr�c�dente s'il y en a une
      // d'affich�e
      if (Etat_chrono==2)
      {
        Effacer_curseur();
        // On efface le commentaire pr�c�dent
        Block(Fenetre_Pos_X+ 46*Menu_Facteur_X,Fenetre_Pos_Y+(175+FILENAMESPACE)*Menu_Facteur_Y,
              Menu_Facteur_X<<8,Menu_Facteur_Y<<3,CM_Clair);
        // On nettoie la zone o� va s'afficher la preview:
        Block(Fenetre_Pos_X+180*Menu_Facteur_X,Fenetre_Pos_Y+ (89+FILENAMESPACE)*Menu_Facteur_Y,
              Menu_Facteur_X*122,Menu_Facteur_Y*82,CM_Clair);
        // On efface les dimensions de l'image
        Block(Fenetre_Pos_X+226*Menu_Facteur_X,Fenetre_Pos_Y+ 55*Menu_Facteur_Y,
              Menu_Facteur_X*72,Menu_Facteur_Y<<3,CM_Clair);
        // On efface la taille du fichier
        Block(Fenetre_Pos_X+226*Menu_Facteur_X,Fenetre_Pos_Y+ 63*Menu_Facteur_Y,
              Menu_Facteur_X*72,Menu_Facteur_Y<<3,CM_Clair);
        // On efface le format du fichier
        Block(Fenetre_Pos_X+226*Menu_Facteur_X,Fenetre_Pos_Y+ 72*Menu_Facteur_Y,
              Menu_Facteur_X*72,Menu_Facteur_Y<<3,CM_Clair);
        // Affichage du commentaire
        if ( (!load) && (FormatFichier[Principal_Format-1].Commentaire) )
        {
          Print_dans_fenetre(46,175+FILENAMESPACE,Principal_Commentaire,CM_Noir,CM_Clair);
        }
        Afficher_curseur();
        // Un update pour couvrir les 4 zones: 3 libell�s plus la zone de preview
        UpdateRect(Fenetre_Pos_X+180*Menu_Facteur_X,Fenetre_Pos_Y+55*Menu_Facteur_Y,Menu_Facteur_X*122,Menu_Facteur_Y*(116+FILENAMESPACE));
        // Zone de commentaire
        UpdateRect(Fenetre_Pos_X+46*Menu_Facteur_X,Fenetre_Pos_Y+(175+FILENAMESPACE)*Menu_Facteur_Y,Menu_Facteur_X*32*8,Menu_Facteur_Y*8);
      }

      Nouvelle_preview=0;
      Etat_chrono=0;         // Etat du chrono = Attente d'un X�me de seconde
      // On lit le temps de d�part du chrono
      Initialiser_chrono(Config.Chrono_delay);
    }

    if (!Etat_chrono)  // Prendre une nouvelle mesure du chrono et regarder
      Tester_chrono(); // s'il ne faut pas afficher la preview

    if (Etat_chrono==1) // Il faut afficher la preview
    {
      if ( (Principal_File_list_Position+Principal_File_list_Decalage>=Liste_Nb_repertoires) && (Liste_Nb_elements) )
      {
        strcpy(Principal_Repertoire_fichier,Principal_Repertoire_courant);

        Effacer_curseur();
        Charger_image(image);
        UpdateRect(ToWinX(179),ToWinY(88+FILENAMESPACE),ToWinL(124),ToWinH(84));
        Afficher_curseur();

        // Apr�s le chargement de la preview, on restaure tout ce qui aurait
        // pu �tre modifi� par le chargement de l'image:
        memcpy(Principal_Palette,Palette_initiale,sizeof(T_Palette));
        Principal_Image_modifiee=Image_modifiee_initiale;
        Principal_Largeur_image=Largeur_image_initiale;
        Principal_Hauteur_image=Hauteur_image_initiale;
      }

      Etat_chrono=2; // On arr�te le chrono
    }
  }
  while ( (!On_a_clicke_sur_OK) && (Bouton_clicke!=2) );

  // Si on annule, on restaure l'ancien commentaire
  if (Bouton_clicke==2)
    strcpy(Principal_Commentaire,Commentaire_initial);

  //   On restaure les donn�es de l'image qui ont certainement �t� modifi�es
  // par la preview.
  memcpy(Principal_Palette,Palette_initiale,sizeof(T_Palette));
  Set_palette(Principal_Palette);
  Back_color=Back_color_initiale;
  Principal_Image_modifiee=Image_modifiee_initiale;
  Principal_Largeur_image=Largeur_image_initiale;
  Principal_Hauteur_image=Hauteur_image_initiale;
  Set_palette(Principal_Palette);

  Calculer_couleurs_menu_optimales(Principal_Palette);
  Temp=(Fenetre_Pos_Y+(Fenetre_Hauteur*Menu_Facteur_Y)<Menu_Ordonnee_avant_fenetre);

  Fermer_fenetre();

  if (Temp)
    Afficher_menu();

  Desenclencher_bouton((load)?BOUTON_CHARGER:BOUTON_SAUVER);
  Afficher_curseur();
  Detruire_liste_du_fileselect();

  Pixel_de_chargement=Pixel_Chargement_dans_ecran_courant;

  free(Palette_initiale);

  return Charger_ou_sauver_l_image;
}
