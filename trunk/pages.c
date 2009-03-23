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
    along with Grafx2; if not, see <http://www.gnu.org/licenses/> or
    write to the Free Software Foundation, Inc.,
    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
//////////////////////////////////////////////////////////////////////////
/////////////////////////// GESTION DU BACKUP ////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "global.h"
#include "pages.h"
#include "erreurs.h"
#include "divers.h"
#include "windows.h"

  ///
  /// GESTION DES PAGES
  ///

void Initialiser_S_Page(S_Page * Page)
{
  // Important: appeler cette fonction sur toute nouvelle structure S_Page!

  if (Page!=NULL)
  {
    Page->Image=NULL;
    Page->Width=0;
    Page->Height=0;
    memset(Page->Palette,0,sizeof(T_Palette));
    Page->Commentaire[0]='\0';
    Page->Repertoire_fichier[0]='\0';
    Page->Filename[0]='\0';
    Page->Format_fichier=FORMAT_PAR_DEFAUT;
/*
    Page->Decalage_X=0;
    Page->Decalage_Y=0;
    Page->Ancien_Decalage_X=0;
    Page->Ancien_Decalage_Y=0;
    Page->Split=0;
    Page->X_Zoom=0;
    Page->Proportion_split=PROPORTION_SPLIT;
    Page->Loupe_Mode=0;
    Page->Loupe_Facteur=FACTEUR_DE_ZOOM_PAR_DEFAUT;
    Page->Loupe_Hauteur=0;
    Page->Loupe_Largeur=0;
    Page->Loupe_Decalage_X=0;
    Page->Loupe_Decalage_Y=0;
*/
  }
}

void Download_infos_page_principal(S_Page * Page)
// Affiche la page � l'�cran
{
  //int Indice_facteur;
  int Dimensions_modifiees;

  if (Page!=NULL)
  {
    Dimensions_modifiees=(Principal_Largeur_image!=Page->Width) ||
                         (Principal_Hauteur_image!=Page->Height);

    Principal_Ecran=Page->Image;
    Principal_Largeur_image=Page->Width;
    Principal_Hauteur_image=Page->Height;
    memcpy(Principal_Palette,Page->Palette,sizeof(T_Palette));
    strcpy(Principal_Commentaire,Page->Commentaire);
    strcpy(Principal_Repertoire_fichier,Page->Repertoire_fichier);
    strcpy(Principal_Nom_fichier,Page->Filename);
    Principal_Format_fichier=Page->Format_fichier;
/*
    Principal_Decalage_X=Page->Decalage_X;
    Principal_Decalage_Y=Page->Decalage_Y;

    // On corrige les d�calages en fonction de la dimension de l'�cran
    if ( (Principal_Decalage_X>0) &&
         (Principal_Decalage_X+Largeur_ecran>Principal_Largeur_image) )
      Principal_Decalage_X=Max(0,Principal_Largeur_image-Largeur_ecran);
    if ( (Principal_Decalage_Y>0) &&
         (Principal_Decalage_Y+Menu_Ordonnee>Principal_Hauteur_image) )
      Principal_Decalage_Y=Max(0,Principal_Hauteur_image-Menu_Ordonnee);

    Ancien_Principal_Decalage_X=Page->Ancien_Decalage_X;
    Ancien_Principal_Decalage_Y=Page->Ancien_Decalage_Y;
    Principal_Split=Page->Split;
    Principal_X_Zoom=Page->X_Zoom;
    Principal_Proportion_split=Page->Proportion_split;
    Loupe_Mode=Page->Loupe_Mode;
    Loupe_Facteur=Page->Loupe_Facteur;
    Loupe_Hauteur=Page->Loupe_Hauteur;
    Loupe_Largeur=Page->Loupe_Largeur;
    Loupe_Decalage_X=Page->Loupe_Decalage_X;
    Loupe_Decalage_Y=Page->Loupe_Decalage_Y;

    // Comme le facteur de zoom a des chances d'avoir chang�, on appelle
    // "Changer_facteur_loupe".
    for (Indice_facteur=0; FACTEUR_ZOOM[Indice_facteur]!=Loupe_Facteur; Indice_facteur++);
    Changer_facteur_loupe(Indice_facteur);
*/
    if (Dimensions_modifiees)
    {
      Loupe_Mode=0;
      Principal_Decalage_X=0;
      Principal_Decalage_Y=0;
      Pixel_Preview=Pixel_Preview_Normal;
      Calculer_limites();
      Calculer_coordonnees_pinceau();
    }
  }
}

void Upload_infos_page_principal(S_Page * Page)
// Sauve l'�cran courant dans la page
{
  if (Page!=NULL)
  {
    Page->Image=Principal_Ecran;
    Page->Width=Principal_Largeur_image;
    Page->Height=Principal_Hauteur_image;
    memcpy(Page->Palette,Principal_Palette,sizeof(T_Palette));
    strcpy(Page->Commentaire,Principal_Commentaire);
    strcpy(Page->Repertoire_fichier,Principal_Repertoire_fichier);
    strcpy(Page->Filename,Principal_Nom_fichier);
    Page->Format_fichier=Principal_Format_fichier;
/*
    Page->Decalage_X=Principal_Decalage_X;
    Page->Decalage_Y=Principal_Decalage_Y;
    Page->Ancien_Decalage_X=Ancien_Principal_Decalage_X;
    Page->Ancien_Decalage_X=Ancien_Principal_Decalage_Y;
    Page->Split=Principal_Split;
    Page->X_Zoom=Principal_X_Zoom;
    Page->Proportion_split=Principal_Proportion_split;
    Page->Loupe_Mode=Loupe_Mode;
    Page->Loupe_Facteur=Loupe_Facteur;
    Page->Loupe_Hauteur=Loupe_Hauteur;
    Page->Loupe_Largeur=Loupe_Largeur;
    Page->Loupe_Decalage_X=Loupe_Decalage_X;
    Page->Loupe_Decalage_Y=Loupe_Decalage_Y;
*/
  }
}

void Download_infos_page_brouillon(S_Page * Page)
{
  if (Page!=NULL)
  {
    Brouillon_Ecran=Page->Image;
    Brouillon_Largeur_image=Page->Width;
    Brouillon_Hauteur_image=Page->Height;
    memcpy(Brouillon_Palette,Page->Palette,sizeof(T_Palette));
    strcpy(Brouillon_Commentaire,Page->Commentaire);
    strcpy(Brouillon_Repertoire_fichier,Page->Repertoire_fichier);
    strcpy(Brouillon_Nom_fichier,Page->Filename);
    Brouillon_Format_fichier=Page->Format_fichier;
/*
    Brouillon_Decalage_X=Page->Decalage_X;
    Brouillon_Decalage_Y=Page->Decalage_Y;
    Ancien_Brouillon_Decalage_X=Page->Ancien_Decalage_X;
    Ancien_Brouillon_Decalage_Y=Page->Ancien_Decalage_Y;
    Brouillon_Split=Page->Split;
    Brouillon_X_Zoom=Page->X_Zoom;
    Brouillon_Proportion_split=Page->Proportion_split;
    Brouillon_Loupe_Mode=Page->Loupe_Mode;
    Brouillon_Loupe_Facteur=Page->Loupe_Facteur;
    Brouillon_Loupe_Hauteur=Page->Loupe_Hauteur;
    Brouillon_Loupe_Largeur=Page->Loupe_Largeur;
    Brouillon_Loupe_Decalage_X=Page->Loupe_Decalage_X;
    Brouillon_Loupe_Decalage_Y=Page->Loupe_Decalage_Y;
*/
  }
}

void Upload_infos_page_brouillon(S_Page * Page)
{
  if (Page!=NULL)
  {
    Page->Image=Brouillon_Ecran;
    Page->Width=Brouillon_Largeur_image;
    Page->Height=Brouillon_Hauteur_image;
    memcpy(Page->Palette,Brouillon_Palette,sizeof(T_Palette));
    strcpy(Page->Commentaire,Brouillon_Commentaire);
    strcpy(Page->Repertoire_fichier,Brouillon_Repertoire_fichier);
    strcpy(Page->Filename,Brouillon_Nom_fichier);
    Page->Format_fichier=Brouillon_Format_fichier;
/*
    Page->Decalage_X=Brouillon_Decalage_X;
    Page->Decalage_Y=Brouillon_Decalage_Y;
    Page->Ancien_Decalage_X=Ancien_Brouillon_Decalage_X;
    Page->Ancien_Decalage_Y=Ancien_Brouillon_Decalage_Y;
    Page->Split=Brouillon_Split;
    Page->X_Zoom=Brouillon_X_Zoom;
    Page->Proportion_split=Brouillon_Proportion_split;
    Page->Loupe_Mode=Brouillon_Loupe_Mode;
    Page->Loupe_Facteur=Brouillon_Loupe_Facteur;
    Page->Loupe_Hauteur=Brouillon_Loupe_Hauteur;
    Page->Loupe_Largeur=Brouillon_Loupe_Largeur;
    Page->Loupe_Decalage_X=Brouillon_Loupe_Decalage_X;
    Page->Loupe_Decalage_Y=Brouillon_Loupe_Decalage_Y;
*/
  }
}

void Download_infos_backup(S_Liste_de_pages * list)
{
  Ecran_backup=list->Pages[1].Image;

  if (Config.FX_Feedback)
    FX_Feedback_Ecran=list->Pages[0].Image;
  else
    FX_Feedback_Ecran=list->Pages[1].Image;
}

int Allouer_une_page(S_Page * Page,int width,int height)
{
  // Important: la S_Page ne doit pas d�j� d�signer une page allou�e auquel
  //            cas celle-ci serait perdue.

  /* Debug : if (Page->Image!=NULL) exit(666); */

  // On alloue la m�moire pour le bitmap
  Page->Image=(byte *)malloc(width*height);

  // On v�rifie que l'allocation se soit bien pass�e
  if (Page->Image==NULL)
    return 0; // Echec
  else
  {
    Page->Width=width;
    Page->Height=height;
    // Important: La mise � jour des autres infos est du ressort de
    //            l'appelant.

    return 1; // Succ�s
  }
}

void Liberer_une_page(S_Page * Page)
{
  // On peut appeler cette fonction sur une page non allou�e.

  if (Page->Image!=NULL)
    free(Page->Image);
  Page->Image=NULL;
  Page->Width=0;
  Page->Height=0;
  // On ne se pr�occupe pas de ce que deviens le reste des infos de l'image.
}

void Copier_S_page(S_Page * dest,S_Page * Source)
{
  *dest=*Source;
}

int Taille_d_une_page(S_Page * Page)
{
  return sizeof(S_Page)+(Page->Width*Page->Height)+8;
  // 8 = 4 + 4
  // (Toute zone allou�e en m�moire est pr�c�d�e d'un mot double indiquant sa
  // taille, or la taille d'un mot double est de 4 octets, et on utilise deux
  // allocations de m�moires: une pour la S_Page et une pour l'image)
}


  ///
  /// GESTION DES LISTES DE PAGES
  ///

void Initialiser_S_Liste_de_pages(S_Liste_de_pages * list)
{
  // Important: appeler cette fonction sur toute nouvelle structure
  //            S_Liste_de_pages!

  list->Taille_liste=0;
  list->Nb_pages_allouees=0;
  list->Pages=NULL;
}

int Allouer_une_liste_de_pages(S_Liste_de_pages * list,int size)
{
  // Important: la S_Liste_de_pages ne doit pas d�j� d�signer une liste de
  //            pages allou�e auquel cas celle-ci serait perdue.
  int Indice;

  /* Debug : if (list->Pages!=NULL) exit(666); */

  // On alloue la m�moire pour la liste
  list->Pages=(S_Page *)malloc(size*sizeof(S_Page));

  // On v�rifie que l'allocation se soit bien pass�e
  if (list->Pages==NULL)
    return 0; // Echec
  else
  {
    // On initialise chacune des nouvelles pages
    for (Indice=0;Indice<size;Indice++)
      Initialiser_S_Page(list->Pages+Indice);
    list->Taille_liste=size;
    list->Nb_pages_allouees=0;

    return 1; // Succ�s
  }
}

void Liberer_une_liste_de_pages(S_Liste_de_pages * list)
{
  // On peut appeler cette fonction sur une liste de pages non allou�e.

  // Important: cette fonction ne lib�re pas les pages de la liste. Il faut
  //            donc le faire pr�alablement si n�cessaire.

  if (list->Pages!=NULL)
    free(list->Pages);
  list->Pages=NULL;
  list->Taille_liste=0;
  list->Nb_pages_allouees=0;
}

int Taille_d_une_liste_de_pages(S_Liste_de_pages * list)
{
  int Resultat=0;
  int Indice;

  for (Indice=0;Indice<list->Nb_pages_allouees;Indice++)
    Resultat+=Taille_d_une_page(list->Pages+Indice);

  return Resultat+sizeof(S_Liste_de_pages)+4;

  // C.F. la remarque � propos de Taille_d_une_page pour la valeur 4.
}

void Reculer_dans_une_liste_de_pages(S_Liste_de_pages * list)
{
  // Cette fonction fait l'�quivalent d'un "Undo" dans la liste de pages.
  // Elle effectue une sorte de ROL (Rotation Left) sur la liste:
  // ���������������������ͻ  |
  // �0�1�2�3�4�5�6�7�8�9�A�  |
  // ���������������������ͼ  |  0=Page courante
  //  � � � � � � � � � � �   |_ A=Page la plus ancienne
  //  v v v v v v v v v v v   |  1=Derni�re page (1er backup)
  // ���������������������ͻ  |
  // �1�2�3�4�5�6�7�8�9�A�0�  |
  // ���������������������ͼ  |

  // Pour simuler un v�ritable Undo, l'appelant doit mettre la structure
  // de page courante � jour avant l'appel, puis en r�extraire les infos en
  // sortie, ainsi que celles relatives � la plus r�cente page d'undo (1�re
  // page de la liste).

  int Indice;
  S_Page * Page_tempo;

  if (list->Nb_pages_allouees>1)
  {
    // On cr�e la page tempo
    Page_tempo=(S_Page *)malloc(sizeof(S_Page));
    Initialiser_S_Page(Page_tempo);

    // On copie la 1�re page (Page 0) dans la page temporaire
    Copier_S_page(Page_tempo,list->Pages);

    // On copie toutes les pages 1-A � leur gauche
    for (Indice=1;Indice<list->Nb_pages_allouees;Indice++)
      Copier_S_page(list->Pages+Indice-1,list->Pages+Indice);

    // On copie la page 0 (dont la sauvegarde a �t� effectu�e dans la page
    // temporaire) en derni�re position
    Copier_S_page(list->Pages+list->Nb_pages_allouees-1,Page_tempo);

    // On d�truit la page tempo
    free(Page_tempo);
  }
}

void Avancer_dans_une_liste_de_pages(S_Liste_de_pages * list)
{
  // Cette fonction fait l'�quivalent d'un "Redo" dans la liste de pages.
  // Elle effectue une sorte de ROR (Rotation Right) sur la liste:
  // ���������������������ͻ  |
  // �0�1�2�3�4�5�6�7�8�9�A�  |
  // ���������������������ͼ  |  0=Page courante
  //  � � � � � � � � � � �   |_ A=Page la plus ancienne
  //  v v v v v v v v v v v   |  1=Derni�re page (1er backup)
  // ���������������������ͻ  |
  // �A�0�1�2�3�4�5�6�7�8�9�  |
  // ���������������������ͼ  |

  // Pour simuler un v�ritable Redo, l'appelant doit mettre la structure
  // de page courante � jour avant l'appel, puis en r�extraire les infos en
  // sortie, ainsi que celles relatives � la plus r�cente page d'undo (1�re
  // page de la liste).

  int Indice;
  S_Page * Page_tempo;

  if (list->Nb_pages_allouees>1)
  {
    // On cr�e la page tempo
    Page_tempo=(S_Page *)malloc(sizeof(S_Page));
    Initialiser_S_Page(Page_tempo);

    // On copie la derni�re page dans la page temporaire
    Copier_S_page(Page_tempo,list->Pages+list->Nb_pages_allouees-1);

    // On copie toutes les pages 0-9 � leur droite
    for (Indice=list->Nb_pages_allouees-1;Indice>0;Indice--)
      Copier_S_page(list->Pages+Indice,list->Pages+Indice-1);

    // On copie la page plus ancienne page (la "A", dont la sauvegarde a �t�
    // effectu�e dans la page temporaire) en 1�re position
    Copier_S_page(list->Pages,Page_tempo);

    // On d�truit la page tempo
    free(Page_tempo);
  }
}

int Nouvelle_page_possible(
        S_Page           * Nouvelle_page,
        S_Liste_de_pages * Liste_courante,
        S_Liste_de_pages * Liste_secondaire
)
{
  unsigned long Taille_immediatement_disponible;
  unsigned long Taille_liste_courante;
  unsigned long Taille_liste_brouillon;
  unsigned long Taille_page_courante;
  unsigned long Taille_page_brouillon;
  unsigned long Taille_nouvelle_page;

  Taille_immediatement_disponible = Memoire_libre()
        - QUANTITE_MINIMALE_DE_MEMOIRE_A_CONSERVER;
  Taille_liste_courante =Taille_d_une_liste_de_pages(Liste_courante);
  Taille_liste_brouillon=Taille_d_une_liste_de_pages(Liste_secondaire);
  Taille_page_courante  =Taille_d_une_page(Liste_courante->Pages);
  Taille_page_brouillon =Taille_d_une_page(Liste_secondaire->Pages);
  Taille_nouvelle_page  =Taille_d_une_page(Nouvelle_page);

  // Il faut pouvoir loger la nouvelle page et son backup dans la page
  // courante, en conservant au pire la 1�re page de brouillon.
  if ( (Taille_immediatement_disponible + Taille_liste_courante +
       Taille_liste_brouillon - Taille_page_brouillon)
       < (2*Taille_nouvelle_page) )
    return 0;

  // Il faut pouvoir loger le brouillon et son backup dans la page de
  // brouillon, en conservant au pire un exemplaire de la nouvelle page dans
  // la page courante. (pour permettre � l'utilisateur de travailler sur son
  // brouillon)
  if ((Taille_immediatement_disponible+Taille_liste_courante+
       Taille_liste_brouillon-Taille_nouvelle_page)<(2*Taille_page_brouillon))
    return 0;

  return 1;
}

void Detruire_derniere_page_allouee_de_la_liste(S_Liste_de_pages * list)
{
  if (list!=NULL)
  {
    if (list->Nb_pages_allouees>0)
    {
      list->Nb_pages_allouees--;
      Liberer_une_page(list->Pages+list->Nb_pages_allouees);
    }
  }
}

void Creer_nouvelle_page(S_Page * Nouvelle_page,S_Liste_de_pages * Liste_courante,S_Liste_de_pages * Liste_secondaire)
{

//   Cette fonction cr�e une nouvelle page dont les attributs correspondent �
// ceux de Nouvelle_page (width,height,...) (le champ Image est invalide
// � l'appel, c'est la fonction qui le met � jour), et l'enfile dans
// Liste_courante.
//   Il est imp�ratif que la cr�ation de cette page soit possible,
// �ventuellement au d�triment des backups de la page de brouillon
// (Liste_secondaire). Afin de s'en assurer, il faut v�rifier cette
// possibilit� � l'aide de
// Nouvelle_page_possible(Nouvelle_page,Liste_courante,Liste_secondaire) avant
// l'appel � cette fonction.
//   De plus, il faut qu'il y ait au moins une page dans chacune des listes.

  int                Il_faut_liberer;
  S_Liste_de_pages * Liste_a_raboter=NULL;
  S_Page *           Page_a_supprimer;
  int                Indice;

  // On regarde s'il faut lib�rer des pages:
  Il_faut_liberer=
    // C'est le cas si la Liste_courante est pleine
  (  (Liste_courante->Taille_liste==Liste_courante->Nb_pages_allouees)
    // ou qu'il ne reste plus assez de place pour allouer la Nouvelle_page
  || ( (Memoire_libre()-QUANTITE_MINIMALE_DE_MEMOIRE_A_CONSERVER)<
       (unsigned long)(Nouvelle_page->Height*Nouvelle_page->Width) )  );

  if (!Il_faut_liberer)
  {
    // On a assez de place pour allouer une page, et de plus la Liste_courante
    // n'est pas pleine. On n'a donc aucune page � supprimer. On peut en
    // allouer une directement.
    Nouvelle_page->Image=(byte *)malloc(Nouvelle_page->Height*Nouvelle_page->Width);
  }
  else
  {
    // On manque de m�moire ou la Liste_courante est pleine. Dans tous les
    // cas, il faut lib�rer une page... qui peut-�tre pourra re-servir.

    // Tant qu'il faut lib�rer
    while (Il_faut_liberer)
    {
      // On cherche sur quelle liste on va virer une page

      // S'il reste des pages � lib�rer dans la Liste_courante
      if (Liste_courante->Nb_pages_allouees>1)
        // Alors on va d�truire la derni�re page allou�e de la Liste_courante
        Liste_a_raboter=Liste_courante;
      else
      {
        if (Liste_secondaire->Nb_pages_allouees>1)
        {
          // Sinon on va d�truire la derni�re page allou�e de la
          // Liste_secondaire
          Liste_a_raboter=Liste_secondaire;
        }
        else
        {
          // Bon, alors l�, on vient de vider toutes les pages et on a toujours pas asez de m�moire... C'est donc qu'un vilain programmeur a oubli� de v�rifier avec Noiuvelle_page_possible avant de venir ici.
          // On sort m�chament du programme sans sauvegarde ni rien. De toutes fa�ons, �a ne devrait jamais se produire...
          Erreur(ERREUR_SORRY_SORRY_SORRY);
        }
      }

      // Puis on d�termine la page que l'on va supprimer (c'est la derni�re de
      // la liste)
      Page_a_supprimer=Liste_a_raboter->Pages+(Liste_a_raboter->Nb_pages_allouees)-1;

      // On regarde si on peut recycler directement la page (cas o� elle
      // aurait la m�me surface que la Nouvelle_page)
      if ((Page_a_supprimer->Height*Page_a_supprimer->Width)==
          (Nouvelle_page->Height*Nouvelle_page->Width))
      {
        // Alors
        // On r�cup�re le bitmap de la page � supprimer (�vite de faire des
        // allocations/d�sallocations fastidieuses et inutiles)
        Nouvelle_page->Image=Page_a_supprimer->Image;

        // On fait semblant que la derni�re page allou�e ne l'est pas
        Liste_a_raboter->Nb_pages_allouees--;

        // On n'a plus besoin de lib�rer de la m�moire puisqu'on a refil� �
        // Nouvelle_page un bitmap valide
        Il_faut_liberer=0;
      }
      else
      {
        // Sinon

        // D�truire la derni�re page allou�e dans la Liste_�_raboter
        Detruire_derniere_page_allouee_de_la_liste(Liste_a_raboter);

        // On regarde s'il faut continuer � lib�rer de la place
        Il_faut_liberer=(Memoire_libre()-QUANTITE_MINIMALE_DE_MEMOIRE_A_CONSERVER)
                       <(unsigned long)(Nouvelle_page->Height*Nouvelle_page->Width);

        // S'il ne faut pas, c'est qu'on peut allouer un bitmap
        // pour la Nouvelle_page
        if (!Il_faut_liberer)
          Nouvelle_page->Image=(byte *)malloc(Nouvelle_page->Height*Nouvelle_page->Width);
      }
    }
  }

  // D'apr�s l'hypoth�se de d�part, la boucle ci-dessus doit s'arr�ter car
  // on a assez de m�moire pour allouer la nouvelle page.
  // D�sormais Nouvelle_page contient un pointeur sur une zone bitmap valide.

  // D�caler la Liste_courante d'un cran vers le pass�.
  for (Indice=Liste_courante->Taille_liste-1;Indice>0;Indice--)
    Copier_S_page(Liste_courante->Pages+Indice,Liste_courante->Pages+Indice-1);

  // Recopier la Nouvelle_page en 1�re position de la Liste_courante
  Copier_S_page(Liste_courante->Pages,Nouvelle_page);
  Liste_courante->Nb_pages_allouees++;
}

void Changer_nombre_de_pages_d_une_liste(S_Liste_de_pages * list,int number)
{
  int Indice;
  S_Page * Nouvelles_pages;

  // Si la liste a d�j� la taille demand�e
  if (list->Taille_liste==number)
    // Alors il n'y a rien � faire
    return;

  // Si la liste contient plus de pages que souhait�
  if (list->Taille_liste>number)
    // Alors pour chaque page en exc�s
    for (Indice=number;Indice<list->Taille_liste;Indice++)
      // On lib�re la page
      Liberer_une_page(list->Pages+Indice);

  // On fait une nouvelle liste de pages:
  Nouvelles_pages=(S_Page *)malloc(number*sizeof(S_Page));
  for (Indice=0;Indice<number;Indice++)
    Initialiser_S_Page(Nouvelles_pages+Indice);

  // On recopie les pages � conserver de l'ancienne liste
  for (Indice=0;Indice<Min(number,list->Taille_liste);Indice++)
    Copier_S_page(Nouvelles_pages+Indice,list->Pages+Indice);

  // On lib�re l'ancienne liste
  free(list->Pages);

  // On met � jour les champs de la nouvelle liste
  list->Pages=Nouvelles_pages;
  list->Taille_liste=number;
  if (list->Nb_pages_allouees>number)
    list->Nb_pages_allouees=number;
}

void Detruire_la_page_courante_d_une_liste(S_Liste_de_pages * list)
{
  // On ne peut pas d�truire la page courante de la liste si apr�s
  // destruction il ne reste pas encore au moins une page.
  if (list->Nb_pages_allouees>1)
  {
    // On fait faire un undo � la liste, comme �a, la nouvelle page courante
    // est la page pr�c�dente
    Reculer_dans_une_liste_de_pages(Principal_Backups);

    // Puis on d�truit la derni�re page, qui est l'ancienne page courante
    Detruire_derniere_page_allouee_de_la_liste(list);
  }
}


  ///
  /// GESTION DES BACKUPS
  ///

int Initialiser_les_listes_de_backups_en_debut_de_programme(int size,int width,int height)
{
  // size correspond au nombre de pages que l'on souhaite dans chaque liste
  // (1 pour la page courante, puis 1 pour chaque backup, soit 2 au minimum).
  // width et height correspondent � la dimension des images de d�part.

  S_Page * Page;
  int return_code=0;

  if (Allouer_une_liste_de_pages(Principal_Backups,size) &&
      Allouer_une_liste_de_pages(Brouillon_Backups,size))
  {
    // On a r�ussi � allouer deux listes de pages dont la taille correspond �
    // celle demand�e par l'utilisateur.

    // On cr�e un descripteur de page correspondant � la page principale
    Page=(S_Page *)malloc(sizeof(S_Page));
    Initialiser_S_Page(Page);
    Upload_infos_page_principal(Page);
    // On y met les infos sur la dimension de d�marrage
    Page->Width=width;
    Page->Height=height;

    // On regarde si on peut ajouter cette page
    if (Nouvelle_page_possible(Page,Principal_Backups,Brouillon_Backups))
    {
      // On peut, donc on va la cr�er
      Creer_nouvelle_page(Page,Principal_Backups,Brouillon_Backups);
      Download_infos_page_principal(Page);
      Download_infos_backup(Principal_Backups);

      // Maintenant, on regarde si on a le droit de cr�er la m�me page dans
      // la page de brouillon.
      if (Nouvelle_page_possible(Page,Brouillon_Backups,Principal_Backups))
      {
        // On peut donc on le fait
        Creer_nouvelle_page(Page,Brouillon_Backups,Principal_Backups);
        Download_infos_page_brouillon(Page);

        // Et on efface les 2 images en les remplacant de "0"
        memset(Principal_Ecran,0,Principal_Largeur_image*Principal_Hauteur_image);
        memset(Brouillon_Ecran,0,Brouillon_Largeur_image*Brouillon_Hauteur_image);

        return_code=1;
      }
      else
      {
        // Il n'est pas possible de d�marrer le programme avec la page 
        // principale et la page de brouillon aux dimensions demand�e par 
        // l'utilisateur. ==> On l'envoie ballader
        return_code=0;
      }
    }
    else
    {
      // On ne peut pas d�marrer le programme avec ne serait-ce qu'une
      // page de la dimension souhait�e, donc on laisse tout tomber et on
      // le renvoie chier.
      free(Page);
      return_code=0;
    }
  }
  else
  {
    // On n'a m�me pas r�ussi � cr�er les listes. Donc c'est m�me pas la 
    // peine de continuer : l'utilisateur ne pourra jamais rien faire, 
    // autant avorter le chargement du programme.
    return_code=0;
  }

  return return_code;
}

void Detruire_les_listes_de_backups_en_fin_de_programme(void)
{
  // On commence par supprimer les pages une � une dans chacune des listes
    // Liste de la page principale
  while (Principal_Backups->Nb_pages_allouees>0)
    Detruire_derniere_page_allouee_de_la_liste(Principal_Backups);
    // Liste de la page de brouillon
  while (Brouillon_Backups->Nb_pages_allouees>0)
    Detruire_derniere_page_allouee_de_la_liste(Brouillon_Backups);

  // Puis on peut d�truire les structures de liste elles-m�mes
  Liberer_une_liste_de_pages(Principal_Backups);
  Liberer_une_liste_de_pages(Brouillon_Backups);
  free(Principal_Backups);
  free(Brouillon_Backups);
}

void Nouveau_nombre_de_backups(int nb_backups)
{
  Changer_nombre_de_pages_d_une_liste(Principal_Backups,nb_backups+1);
  Changer_nombre_de_pages_d_une_liste(Brouillon_Backups,nb_backups+1);

  // Le +1 vient du fait que dans chaque liste, en 1�re position on retrouve
  // les infos de la page courante sur le brouillon et la page principale.
  // (nb_backups = Nombre de backups, sans compter les pages courantes)
}

int Backup_avec_nouvelles_dimensions(int Upload,int width,int height)
{
  // Retourne 1 si une nouvelle page est disponible (alors pleine de 0) et
  // 0 sinon.

  S_Page * Nouvelle_page;
  int return_code=0;

  if (Upload)
    // On remet � jour l'�tat des infos de la page courante (pour pouvoir les
    // retrouver plus tard)
    Upload_infos_page_principal(Principal_Backups->Pages);

  // On cr�e un descripteur pour la nouvelle page courante
  Nouvelle_page=(S_Page *)malloc(sizeof(S_Page));
  Initialiser_S_Page(Nouvelle_page);

  Upload_infos_page_principal(Nouvelle_page);
  Nouvelle_page->Width=width;
  Nouvelle_page->Height=height;
  if (Nouvelle_page_possible(Nouvelle_page,Principal_Backups,Brouillon_Backups))
  {
    Creer_nouvelle_page(Nouvelle_page,Principal_Backups,Brouillon_Backups);
    Download_infos_page_principal(Nouvelle_page);
    Download_infos_backup(Principal_Backups);
    // On nettoie la nouvelle image:
    memset(Principal_Ecran,0,Principal_Largeur_image*Principal_Hauteur_image);
    return_code=1;
  }

  // On d�truit le descripteur de la page courante
  free(Nouvelle_page);

  return return_code;
}

int Backuper_et_redimensionner_brouillon(int width,int height)
{
  // Retourne 1 si la page de dimension souhaitee est disponible en brouillon
  // et 0 sinon.

  S_Page * Nouvelle_page;
  int return_code=0;

  // On remet � jour l'�tat des infos de la page de brouillon (pour pouvoir
  // les retrouver plus tard)
  Upload_infos_page_brouillon(Brouillon_Backups->Pages);

  // On cr�e un descripteur pour la nouvelle page de brouillon
  Nouvelle_page=(S_Page *)malloc(sizeof(S_Page));
  Initialiser_S_Page(Nouvelle_page);

  Upload_infos_page_brouillon(Nouvelle_page);
  Nouvelle_page->Width=width;
  Nouvelle_page->Height=height;
  if (Nouvelle_page_possible(Nouvelle_page,Brouillon_Backups,Principal_Backups))
  {
    Creer_nouvelle_page(Nouvelle_page,Brouillon_Backups,Principal_Backups);
    Download_infos_page_brouillon(Nouvelle_page);
    return_code=1;
  }

  // On d�truit le descripteur de la page courante
  free(Nouvelle_page);

  return return_code;
}

void Backup(void)
// Sauve la page courante comme premi�re page de backup et cr�e une nouvelle page
// pur continuer � dessiner. Utilis� par exemple pour le fill
{
  #if defined(__macosx__) || defined(__FreeBSD__)
    S_Page Nouvelle_page;
  #else
    S_Page *Nouvelle_page;
  #endif

  // On remet � jour l'�tat des infos de la page courante (pour pouvoir les
  // retrouver plus tard)
  Upload_infos_page_principal(Principal_Backups->Pages);

  // On cr�e un descripteur pour la nouvelle page courante
#if defined(__macosx__) || defined(__FreeBSD__)
  Initialiser_S_Page(&Nouvelle_page);

  // Enrichissement de l'historique
  Copier_S_page(&Nouvelle_page,Principal_Backups->Pages);
  Creer_nouvelle_page(&Nouvelle_page,Principal_Backups,Brouillon_Backups);
  Download_infos_page_principal(&Nouvelle_page);
#else
  Nouvelle_page=(S_Page *)malloc(sizeof(S_Page));
  Initialiser_S_Page(Nouvelle_page);

  // Enrichissement de l'historique
  Copier_S_page(Nouvelle_page,Principal_Backups->Pages);
  Creer_nouvelle_page(Nouvelle_page,Principal_Backups,Brouillon_Backups);
  Download_infos_page_principal(Nouvelle_page);
#endif

  Download_infos_backup(Principal_Backups);

  // On copie l'image du backup vers la page courante:
  memcpy(Principal_Ecran,Ecran_backup,Principal_Largeur_image*Principal_Hauteur_image);

  // On d�truit le descripteur de la page courante
#if !(defined(__macosx__) || defined(__FreeBSD__))
  free(Nouvelle_page);
#endif

  // On allume l'indicateur de modification de l'image
  Principal_Image_modifiee=1;
}

void Undo(void)
{
  // On remet � jour l'�tat des infos de la page courante (pour pouvoir les
  // retrouver plus tard)
  Upload_infos_page_principal(Principal_Backups->Pages);
  // On fait faire un undo � la liste des backups de la page principale
  Reculer_dans_une_liste_de_pages(Principal_Backups);

  // On extrait ensuite les infos sur la nouvelle page courante
  Download_infos_page_principal(Principal_Backups->Pages);
  // Et celles du backup
  Download_infos_backup(Principal_Backups);
  // Note: le backup n'a pas obligatoirement les m�mes dimensions ni la m�me
  //       palette que la page courante. Mais en temps normal, le backup
  //       n'est pas utilis� � la suite d'un Undo. Donc �a ne devrait pas
  //       poser de probl�mes.
}

void Redo(void)
{
  // On remet � jour l'�tat des infos de la page courante (pour pouvoir les
  // retrouver plus tard)
  Upload_infos_page_principal(Principal_Backups->Pages);
  // On fait faire un redo � la liste des backups de la page principale
  Avancer_dans_une_liste_de_pages(Principal_Backups);

  // On extrait ensuite les infos sur la nouvelle page courante
  Download_infos_page_principal(Principal_Backups->Pages);
  // Et celles du backup
  Download_infos_backup(Principal_Backups);
  // Note: le backup n'a pas obligatoirement les m�mes dimensions ni la m�me
  //       palette que la page courante. Mais en temps normal, le backup
  //       n'est pas utilis� � la suite d'un Redo. Donc �a ne devrait pas
  //       poser de probl�mes.
}

void Detruire_la_page_courante(void)
{
  // On d�truit la page courante de la liste principale
  Detruire_la_page_courante_d_une_liste(Principal_Backups);
  // On extrait ensuite les infos sur la nouvelle page courante
  Download_infos_page_principal(Principal_Backups->Pages);
  // Et celles du backup
  Download_infos_backup(Principal_Backups);
  // Note: le backup n'a pas obligatoirement les m�mes dimensions ni la m�me
  //       palette que la page courante. Mais en temps normal, le backup
  //       n'est pas utilis� � la suite d'une destruction de page. Donc �a ne
  //       devrait pas poser de probl�mes.
}

void Interchanger_image_principale_et_brouillon(void)
{
  S_Liste_de_pages * Liste_tempo;

  // On commence par mettre � jour dans les descripteurs les infos sur les
  // pages qu'on s'appr�te � �changer, pour qu'on se retrouve pas avec de
  // vieilles valeurs qui datent de mathuzalem.
  Upload_infos_page_principal(Principal_Backups->Pages);
  Upload_infos_page_brouillon(Brouillon_Backups->Pages);

  // On inverse les listes de pages
  Liste_tempo=Principal_Backups;
  Principal_Backups=Brouillon_Backups;
  Brouillon_Backups=Liste_tempo;

  // On extrait ensuite les infos sur les nouvelles pages courante, brouillon
  // et backup.

    /* SECTION GROS CACA PROUT PROUT */
    // Auparavant on ruse en mettant d�j� � jour les dimensions de la
    // nouvelle page courante. Si on ne le fait pas, le "Download" va d�tecter
    // un changement de dimensions et va b�tement sortir du mode loupe, alors
    // que lors d'un changement de page, on veut bien conserver l'�tat du mode
    // loupe du brouillon.
    Principal_Largeur_image=Principal_Backups->Pages->Width;
    Principal_Hauteur_image=Principal_Backups->Pages->Height;

  Download_infos_page_principal(Principal_Backups->Pages);
  Download_infos_page_brouillon(Brouillon_Backups->Pages);
  Download_infos_backup(Principal_Backups);
}


int Emprunt_memoire_de_page_possible(int size)
{
  int Taille_immediatement_disponible;
  int Taille_liste_courante;
  int Taille_liste_brouillon;
  int Taille_page_courante;
  int Taille_page_brouillon;

  Taille_immediatement_disponible=Memoire_libre()-QUANTITE_MINIMALE_DE_MEMOIRE_A_CONSERVER;
  Taille_liste_courante =Taille_d_une_liste_de_pages(Principal_Backups);
  Taille_liste_brouillon=Taille_d_une_liste_de_pages(Brouillon_Backups);
  Taille_page_courante  =Taille_d_une_page(Principal_Backups->Pages);
  Taille_page_brouillon =Taille_d_une_page(Brouillon_Backups->Pages);

  // Il faut pouvoir loger la zone m�moire ainsi qu'un exemplaire de la page
  // courante, en conservant au pire la 1�re page de brouillon.
  if ((Taille_immediatement_disponible
      +Taille_liste_courante
      +Taille_liste_brouillon
      -Taille_page_courante
      -Taille_page_brouillon)<size)
    return 0;

  return 1;
}

void * Emprunter_memoire_de_page(int size)
{
  int                Il_faut_liberer;
  S_Liste_de_pages * Liste_a_raboter;
  S_Page *           Page_a_supprimer;
  //int                Indice;

  if (Emprunt_memoire_de_page_possible(size))
  {
    // On regarde s'il faut lib�rer des pages:
    Il_faut_liberer=
      (Memoire_libre()-QUANTITE_MINIMALE_DE_MEMOIRE_A_CONSERVER)<(unsigned long)size;

    if (!Il_faut_liberer)
    {
      // On a assez de place pour allouer une page. On n'a donc aucune page
      // � supprimer. On peut allouer de la m�moire directement.
      return malloc(size);
    }
    else
    {
      // On manque de m�moire. Il faut lib�rer une page...

      // Tant qu'il faut lib�rer
      while (Il_faut_liberer)
      {
        // On cherche sur quelle liste on va virer une page

        // S'il reste des pages � lib�rer dans la liste des brouillons
        if (Brouillon_Backups->Nb_pages_allouees>1)
          // Alors on va d�truire la derni�re page allou�e de la liste des
          // brouillons
          Liste_a_raboter=Brouillon_Backups;
        else
        {
          if (Principal_Backups->Nb_pages_allouees>1)
          {
            // Sinon on va d�truire la derni�re page allou�e de la
            // liste principale
            Liste_a_raboter=Principal_Backups;
          }
          else
          {
            // Dans cette branche, il �tait pr�vu qu'on obtienne la m�moire
            // n�cessaire mais on n'arrive pas � la trouver. On indique donc
            // qu'elle n'est pas disponible, et on aura perdu des backups
            // pour rien
            return 0;
          }
        }

        // Puis on d�termine la page que l'on va supprimer (c'est la derni�re
        // de la liste)
        Page_a_supprimer=Liste_a_raboter->Pages+(Liste_a_raboter->Nb_pages_allouees)-1;

        // D�truire la derni�re page allou�e dans la Liste_�_raboter
        Detruire_derniere_page_allouee_de_la_liste(Liste_a_raboter);

        // On regarde s'il faut continuer � lib�rer de la place
        Il_faut_liberer=
          (Memoire_libre()-QUANTITE_MINIMALE_DE_MEMOIRE_A_CONSERVER)<(unsigned long)size;

        // S'il ne faut pas, c'est qu'on peut allouer un bitmap
        // pour la Nouvelle_page
        if (!Il_faut_liberer)
          return malloc(size);
      }
    }
  }
  else
  {
    // Il n'y a pas assez de place pour allouer la m�moire temporaire dans
    // la m�moire r�serv�e aux pages.
    return 0;
  }

  // Pour que le compilateur ne dise pas qu'il manque une valeur de sortie:
  return 0;
}
