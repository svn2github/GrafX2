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
    along with Grafx2; if not, see <http://www.gnu.org/licenses/> or
    write to the Free Software Foundation, Inc.,
    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "const.h"
#include "struct.h"
#include "global.h"
#include "divers.h"
#include "moteur.h"
#include "graph.h"
#include "operatio.h"
#include "boutons.h"
#include "pages.h"
#include "erreurs.h"

#ifdef __WATCOMC__
    #define M_PI 3.14159265358979323846 
#endif

void Demarrer_pile_operation(word Operation_demandee)
{
  Brosse_Centre_rotation_defini=0;

  // On m�morise l'op�ration pr�c�dente si on d�marre une interruption
  switch(Operation_demandee)
  {
    case OPERATION_LOUPE         :
    case OPERATION_PIPETTE       :
    case OPERATION_PRISE_BROSSE  :
    case OPERATION_POLYBROSSE    :
    case OPERATION_ETIRER_BROSSE :
    case OPERATION_TOURNER_BROSSE:
      Operation_avant_interruption=Operation_en_cours;
      // On passe � l'operation demand�e
      Operation_en_cours=Operation_demandee;
      break;
    default :
      // On passe � l'operation demand�e
      Operation_en_cours=Operation_demandee;
      Operation_avant_interruption=Operation_en_cours;
  }

  // On sp�cifie si l'op�ration autorise le changement de couleur au clavier
  switch(Operation_demandee)
  {
    case OPERATION_DESSIN_CONTINU:
    case OPERATION_DESSIN_DISCONTINU:
    case OPERATION_SPRAY:
    case OPERATION_LIGNES_CENTREES:
      Autoriser_changement_de_couleur_pendant_operation=1;
      break;
    default :
      Autoriser_changement_de_couleur_pendant_operation=0;
  }

  // Et on passe au curseur qui va avec
  Forme_curseur=CURSEUR_D_OPERATION[Operation_demandee];
  Operation_Taille_pile=0;
}


void Initialiser_debut_operation(void)
{
  Operation_dans_loupe=(Mouse_X>=Principal_X_Zoom);
  Smear_Debut=1;
}


void Operation_PUSH(short Valeur)
{
  Operation_Pile[++Operation_Taille_pile]=Valeur;
}


void Operation_POP(short * Valeur)
{
  *Valeur=Operation_Pile[Operation_Taille_pile--];
}


byte Pinceau_Forme_avant_operation;
byte Cacher_pinceau_avant_operation;



short Distance(short X1, short Y1, short X2, short Y2)
{
  short X2_moins_X1=X2-X1;
  short Y2_moins_Y1=Y2-Y1;

  return Round( sqrt( (X2_moins_X1*X2_moins_X1) + (Y2_moins_Y1*Y2_moins_Y1) ) );
}


void Aff_coords_rel_ou_abs(short Debut_X, short Debut_Y)
{
  char Chaine[6];

  if (Config.Coords_rel)
  {
    if (Menu_visible)
    {
      if (Pinceau_X>Debut_X)
      {
        Num2str(Pinceau_X-Debut_X,Chaine,5);
        Chaine[0]='+';
      }
      else if (Pinceau_X<Debut_X)
      {
        Num2str(Debut_X-Pinceau_X,Chaine,5);
        Chaine[0]='-';
      }
      else
        strcpy(Chaine,"�   0");
      Print_dans_menu(Chaine,2);

      if (Pinceau_Y>Debut_Y)
      {
        Num2str(Pinceau_Y-Debut_Y,Chaine,5);
        Chaine[0]='+';
      }
      else if (Pinceau_Y<Debut_Y)
      {
        Num2str(Debut_Y-Pinceau_Y,Chaine,5);
        Chaine[0]='-';
      }
      else
        strcpy(Chaine,"�   0");
      Print_dans_menu(Chaine,12);
    }
  }
  else
    Print_coordonnees();
}

//////////////////////////////////////////////////// OPERATION_DESSIN_CONTINU

void Freehand_Mode1_1_0(void)
//  Op�ration   : OPERATION_DESSIN_CONTINU
//  Click Souris: 1
//  Taille_Pile : 0
//
//  Souris effac�e: Oui
{
  Initialiser_debut_operation();
  Backup();
  Shade_Table=Shade_Table_gauche;
  // On affiche d�finitivement le pinceau
  Afficher_pinceau(Pinceau_X,Pinceau_Y,Fore_color,0);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


void Freehand_Mode1_1_2(void)
//  Op�ration   : OPERATION_DESSIN_CONTINU
//  Click Souris: 1
//  Taille_Pile : 2
//
//  Souris effac�e: Non
{
  short Debut_X;
  short Debut_Y;

  Operation_POP(&Debut_Y);
  Operation_POP(&Debut_X);

  if ( (Debut_Y!=Pinceau_Y) || (Debut_X!=Pinceau_X) )
  {
    Effacer_curseur();
    Print_coordonnees();
    Tracer_ligne_Definitif(Debut_X,Debut_Y,Pinceau_X,Pinceau_Y,Fore_color);
    Afficher_curseur();
  }

  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


void Freehand_Mode12_0_2(void)
//  Op�ration   : OPERATION_DESSIN_[DIS]CONTINU
//  Click Souris: 0
//  Taille_Pile : 2
//
//  Souris effac�e: Non
{
  Operation_Taille_pile=0;
}


void Freehand_Mode1_2_0(void)
//  Op�ration   : OPERATION_DESSIN_CONTINU
//  Click Souris: 2
//  Taille_Pile : 0
//
//  Souris effac�e: Oui
{
  Initialiser_debut_operation();
  Backup();
  Shade_Table=Shade_Table_droite;
  // On affiche d�finitivement le pinceau
  Afficher_pinceau(Pinceau_X,Pinceau_Y,Back_color,0);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


void Freehand_Mode1_2_2(void)
//  Op�ration   : OPERATION_DESSIN_CONTINU
//  Click Souris: 2
//  Taille_Pile : 2
//
//  Souris effac�e: Non
{
  short Debut_X;
  short Debut_Y;

  Operation_POP(&Debut_Y);
  Operation_POP(&Debut_X);

  if ( (Debut_Y!=Pinceau_Y) || (Debut_X!=Pinceau_X) )
  {
    Print_coordonnees();
    Effacer_curseur();
    Tracer_ligne_Definitif(Debut_X,Debut_Y,Pinceau_X,Pinceau_Y,Back_color);
    Afficher_curseur();
  }

  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


///////////////////////////////////////////////// OPERATION_DESSIN_DISCONTINU

void Freehand_Mode2_1_0(void)
//  Op�ration   : OPERATION_DESSIN_DISCONTINU
//  Click Souris: 1
//  Taille_Pile : 0
//
//  Souris effac�e: Oui
{
  Initialiser_debut_operation();
  Backup();
  Shade_Table=Shade_Table_gauche;
  // On affiche d�finitivement le pinceau
  Afficher_pinceau(Pinceau_X,Pinceau_Y,Fore_color,0);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Print_coordonnees();
  Wait_VBL();
}


void Freehand_Mode2_1_2(void)
//  Op�ration   : OPERATION_DESSIN_DISCONTINU
//  Click Souris: 1
//  Taille_Pile : 2
//
//  Souris effac�e: Non
{
  short Debut_X;
  short Debut_Y;

  Operation_POP(&Debut_Y);
  Operation_POP(&Debut_X);

  if ( (Debut_X!=Pinceau_X) || (Debut_Y!=Pinceau_Y) )
  {
    Effacer_curseur();
    // On affiche d�finitivement le pinceau
    Afficher_pinceau(Pinceau_X,Pinceau_Y,Fore_color,0);
    Afficher_curseur();
    Print_coordonnees();
    Wait_VBL();
  }

  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


void Freehand_Mode2_2_0(void)
//  Op�ration   : OPERATION_DESSIN_DISCONTINU
//  Click Souris: 2
//  Taille_Pile : 0
//
//  Souris effac�e: Oui
{
  Initialiser_debut_operation();
  Backup();
  Shade_Table=Shade_Table_droite;
  // On affiche d�finitivement le pinceau
  Afficher_pinceau(Pinceau_X,Pinceau_Y,Back_color,0);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Print_coordonnees();
  Wait_VBL();
}


void Freehand_Mode2_2_2(void)
//  Op�ration   : OPERATION_DESSIN_DISCONTINU
//  Click Souris: 2
//  Taille_Pile : 2
//
//  Souris effac�e: Non
{
  short Debut_X;
  short Debut_Y;

  Operation_POP(&Debut_Y);
  Operation_POP(&Debut_X);

  if ( (Debut_X!=Pinceau_X) || (Debut_Y!=Pinceau_Y) )
  {
    Effacer_curseur();
    // On affiche d�finitivement le pinceau
    Afficher_pinceau(Pinceau_X,Pinceau_Y,Back_color,0);
    Afficher_curseur();
    Print_coordonnees();
    Wait_VBL();
  }

  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


////////////////////////////////////////////////////// OPERATION_DESSIN_POINT

void Freehand_Mode3_1_0(void)
//  Op�ration   : OPERATION_DESSIN_POINT
//  Click Souris: 1
//  Taille_Pile : 0
//
//  Souris effac�e: Oui
{
  Initialiser_debut_operation();
  Backup();
  Shade_Table=Shade_Table_gauche;
  // On affiche d�finitivement le pinceau
  Afficher_pinceau(Pinceau_X,Pinceau_Y,Fore_color,0);
  Operation_PUSH(0);  // On change simplement l'�tat de la pile...
}


void Freehand_Mode3_2_0(void)
//  Op�ration   : OPERATION_DESSIN_POINT
//  Click Souris: 2
//  Taille_Pile : 0
//
//  Souris effac�e: Oui
{
  Initialiser_debut_operation();
  Backup();
  Shade_Table=Shade_Table_droite;
  // On affiche d�finitivement le pinceau
  Afficher_pinceau(Pinceau_X,Pinceau_Y,Back_color,0);
  Operation_PUSH(0);  // On change simplement l'�tat de la pile...
}


void Freehand_Mode3_0_1(void)
//  Op�ration   : OPERATION_DESSIN_POINT
//  Click Souris: 0
//  Taille_Pile : 1
//
//  Souris effac�e: Non
{
  Operation_Taille_pile--;
}


///////////////////////////////////////////////////////////// OPERATION_LIGNE

void Ligne_12_0(void)
// Op�ration   : OPERATION_LIGNE
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
//  Souris effac�e: Oui

//  D�but du trac� d'une ligne (premier clic)
{
  Initialiser_debut_operation();
  Backup();
  Pinceau_Forme_avant_operation=Pinceau_Forme;
  Pinceau_Forme=FORME_PINCEAU_POINT;

  if (Mouse_K==A_GAUCHE)
  {
    Shade_Table=Shade_Table_gauche;
    Pixel_figure_Preview(Pinceau_X,Pinceau_Y,Fore_color);
    Operation_PUSH(Fore_color);
  }
  else
  {
    Shade_Table=Shade_Table_droite;
    Pixel_figure_Preview(Pinceau_X,Pinceau_Y,Back_color);
    Operation_PUSH(Back_color);
  }

  if ((Config.Coords_rel) && (Menu_visible))
    Print_dans_menu("X:�   0   Y:�   0",0);

  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


void Ligne_12_5(void)
// Op�ration   : OPERATION_LIGNE
// Click Souris: 1
// Taille_Pile : 5
//
// Souris effac�e: Non

// Poursuite du trac� d'une ligne (d�placement de la souris en gardant le curseur appuy�)
{
  short Debut_X;
  short Debut_Y;
  short Fin_X;
  short Fin_Y;

  Operation_POP(&Fin_Y);
  Operation_POP(&Fin_X);

  if ((Pinceau_X!=Fin_X) || (Pinceau_Y!=Fin_Y))
  {
    Effacer_curseur();
    Operation_POP(&Debut_Y);
    Operation_POP(&Debut_X);
      // On corrige les coordonn�es de la ligne si la touche shift est appuy�e...
      if(SDL_GetModState() & KMOD_SHIFT)
	  Rectifier_coordonnees_a_45_degres(Debut_X,Debut_Y,&Pinceau_X,&Pinceau_Y);

    Aff_coords_rel_ou_abs(Debut_X,Debut_Y);

    Effacer_ligne_Preview(Debut_X,Debut_Y,Fin_X,Fin_Y);
    if (Mouse_K==A_GAUCHE)
    {
      Pixel_figure_Preview (Debut_X,Debut_Y,Fore_color);
      Tracer_ligne_Preview (Debut_X,Debut_Y,Pinceau_X,Pinceau_Y,Fore_color);
    }
    else
    {
      Pixel_figure_Preview (Debut_X,Debut_Y,Back_color);
      Tracer_ligne_Preview (Debut_X,Debut_Y,Pinceau_X,Pinceau_Y,Back_color);
    }

    Operation_PUSH(Debut_X);
    Operation_PUSH(Debut_Y);
    Afficher_curseur();
  }

  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


void Ligne_0_5(void)
// Op�ration   : OPERATION_LIGNE
// Click Souris: 0
// Taille_Pile : 5
//
// Souris effac�e: Oui

// Fin du trac� d'une ligne (relachage du bouton)
{
  short Debut_X;
  short Debut_Y;
  short Fin_X;
  short Fin_Y;
  short Couleur;

  Operation_POP(&Fin_Y);
  Operation_POP(&Fin_X);
  Operation_POP(&Debut_Y);
  Operation_POP(&Debut_X);
  Operation_POP(&Couleur);

  // On corrige les coordonn�es de la ligne si la touche shift est appuy�e...
  if(SDL_GetModState() & KMOD_SHIFT)
      Rectifier_coordonnees_a_45_degres(Debut_X,Debut_Y,&Fin_X,&Fin_Y);

  Pinceau_Forme=Pinceau_Forme_avant_operation;

  Pixel_figure_Preview  (Debut_X,Debut_Y,Lit_pixel_dans_ecran_courant(Debut_X,Debut_Y));
  Effacer_ligne_Preview (Debut_X,Debut_Y,Fin_X,Fin_Y);
  Afficher_pinceau      (Debut_X,Debut_Y,Couleur,0);
  Tracer_ligne_Definitif(Debut_X,Debut_Y,Fin_X,Fin_Y,Couleur);

  if ( (Config.Coords_rel) && (Menu_visible) )
  {
    Print_dans_menu("X:       Y:             ",0);
    Print_coordonnees();
  }
}


/////////////////////////////////////////////////////////// OPERATION_K_LIGNE


void K_Ligne_12_0(void)
// Op�ration   : OPERATION_K_LIGNE
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
//  Souris effac�e: Oui
{
  byte Couleur;

  Initialiser_debut_operation();
  Backup();
  Shade_Table=(Mouse_K==A_GAUCHE)?Shade_Table_gauche:Shade_Table_droite;
  Pinceau_Forme_avant_operation=Pinceau_Forme;
  Pinceau_Forme=FORME_PINCEAU_POINT;

  Couleur=(Mouse_K==A_GAUCHE)?Fore_color:Back_color;

  // On place temporairement le d�but de la ligne qui ne s'afficherait pas sinon
  Pixel_figure_Preview(Pinceau_X,Pinceau_Y,Couleur);

  if ((Config.Coords_rel) && (Menu_visible))
    Print_dans_menu("X:�   0   Y:�   0",0);

  Operation_PUSH(Mouse_K | 0x80);
  Operation_PUSH(Couleur);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  // Taille de pile 6 : phase d'appui, non interruptible
}


void K_Ligne_12_6(void)
// Op�ration   : OPERATION_K_LIGNE
// Click Souris: 1 ou 2 | 0
// Taille_Pile : 6      | 7
//
// Souris effac�e: Non
{
  short Debut_X;
  short Debut_Y;
  short Fin_X;
  short Fin_Y;
  short Couleur;

  Operation_POP(&Fin_Y);
  Operation_POP(&Fin_X);

  if ((Pinceau_X!=Fin_X) || (Pinceau_Y!=Fin_Y))
  {
    Effacer_curseur();
    Operation_POP(&Debut_Y);
    Operation_POP(&Debut_X);
    Operation_POP(&Couleur);

    Aff_coords_rel_ou_abs(Debut_X,Debut_Y);

    Effacer_ligne_Preview(Debut_X,Debut_Y,Fin_X,Fin_Y);
    Pixel_figure_Preview (Debut_X,Debut_Y,Couleur);
    Tracer_ligne_Preview (Debut_X,Debut_Y,Pinceau_X,Pinceau_Y,Couleur);

    Operation_PUSH(Couleur);
    Operation_PUSH(Debut_X);
    Operation_PUSH(Debut_Y);
    Afficher_curseur();
  }

  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


void K_Ligne_0_6(void)
// Op�ration   : OPERATION_K_LIGNE
// Click Souris: 0
// Taille_Pile : 6
//
// Souris effac�e: Oui
{
  short Debut_X;
  short Debut_Y;
  short Fin_X;
  short Fin_Y;
  short Couleur;
  short Direction;

  Operation_POP(&Fin_Y);
  Operation_POP(&Fin_X);
  Operation_POP(&Debut_Y);
  Operation_POP(&Debut_X);
  Operation_POP(&Couleur);
  Operation_POP(&Direction);

  if ((Config.Coords_rel) && (Menu_visible))
    Print_dans_menu("X:�   0   Y:�   0",0);

  Pixel_figure_Preview  (Debut_X,Debut_Y,Lit_pixel_dans_ecran_courant(Debut_X,Debut_Y));
  Effacer_ligne_Preview (Debut_X,Debut_Y,Fin_X,Fin_Y);

  Pinceau_Forme=Pinceau_Forme_avant_operation;
  if (Direction & 0x80)
  {
    Afficher_pinceau(Debut_X,Debut_Y,Couleur,0);
    Direction=(Direction & 0x7F);
  }
  Tracer_ligne_Definitif(Debut_X,Debut_Y,Pinceau_X,Pinceau_Y,Couleur);
  Pinceau_Forme=FORME_PINCEAU_POINT;

  Operation_PUSH(Direction);
  Operation_PUSH(Direction); // Valeur bidon servant de nouvel �tat de pile
  Operation_PUSH(Couleur);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  // Taille de pile 7 : phase de "repos", interruptible (comme Elliot Ness :))
}


void K_Ligne_12_7(void)
// Op�ration   : OPERATION_K_LIGNE
// Click Souris: 1 ou 2
// Taille_Pile : 7
//
// Souris effac�e: Oui
{
  short Debut_X;
  short Debut_Y;
  short Fin_X;
  short Fin_Y;
  short Couleur;
  short Direction;

  Operation_POP(&Fin_Y);
  Operation_POP(&Fin_X);
  Operation_POP(&Debut_Y);
  Operation_POP(&Debut_X);
  Operation_POP(&Couleur);
  Operation_POP(&Direction);
  Operation_POP(&Direction);

  if (Direction==Mouse_K)
  {
    Operation_PUSH(Direction);
    Operation_PUSH(Couleur);
    Operation_PUSH(Debut_X);
    Operation_PUSH(Debut_Y);
    Operation_PUSH(Fin_X);
    Operation_PUSH(Fin_Y);
    // Taille de pile 6 : phase d'appui, non interruptible
  }
  else
  {
    // La s�rie de ligne est termin�e, il faut donc effacer la derni�re
    // preview de ligne
    Pixel_figure_Preview  (Debut_X,Debut_Y,Lit_pixel_dans_ecran_courant(Debut_X,Debut_Y));
    Effacer_ligne_Preview (Debut_X,Debut_Y,Fin_X,Fin_Y);

    Afficher_curseur();
    Attendre_fin_de_click();
    Effacer_curseur();
    Pinceau_Forme=Pinceau_Forme_avant_operation;

    if ( (Config.Coords_rel) && (Menu_visible) )
    {
      Print_dans_menu("X:       Y:             ",0);
      Print_coordonnees();
    }
  }
}


// ---------------------------------------------------------- OPERATION_LOUPE


void Loupe_12_0(void)

// Op�ration   : 4      (Choix d'une Loupe)
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui

{
  Attendre_fin_de_click();

  // On passe en mode loupe
  Loupe_Mode=1;

  // La fonction d'affichage dans la partie image est d�sormais un affichage
  // sp�cial loupe.
  Pixel_Preview=Pixel_Preview_Loupe;

  // On calcule l'origine de la loupe
  Loupe_Decalage_X=Mouse_X-(Loupe_Largeur>>1);
  Loupe_Decalage_Y=Mouse_Y-(Loupe_Hauteur>>1);

  // Calcul du coin haut_gauche de la fen�tre devant �tre zoom�e DANS L'ECRAN
  if (Loupe_Decalage_X+Loupe_Largeur>=Limite_Droite-Principal_Decalage_X)
    Loupe_Decalage_X=Limite_Droite-Loupe_Largeur-Principal_Decalage_X+1;
  if (Loupe_Decalage_Y+Loupe_Hauteur>=Limite_Bas-Principal_Decalage_Y)
    Loupe_Decalage_Y=Limite_Bas-Loupe_Hauteur-Principal_Decalage_Y+1;

  // Calcul des coordonn�es absolues de ce coin DANS L'IMAGE
  Loupe_Decalage_X+=Principal_Decalage_X;
  Loupe_Decalage_Y+=Principal_Decalage_Y;

  if (Loupe_Decalage_X<0)
    Loupe_Decalage_X=0;
  if (Loupe_Decalage_Y<0)
    Loupe_Decalage_Y=0;

  // On calcule les bornes visibles dans l'�cran
  Recadrer_ecran_par_rapport_au_zoom();
  Calculer_limites();
  Afficher_ecran();

  // Repositionner le curseur en fonction des coordonn�es visibles
  Calculer_coordonnees_pinceau();

  // On fait de notre mieux pour restaurer l'ancienne op�ration:
  Demarrer_pile_operation(Operation_avant_interruption);
  DEBUG("OP",0);
}

/////////////////////////////////////////////////// OPERATION_RECTANGLE_?????

void Rectangle_12_0(void)
// Op�ration   : OPERATION_RECTANGLE_VIDE / OPERATION_RECTANGLE_PLEIN
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
{
  Initialiser_debut_operation();

  if ((Config.Coords_rel) && (Menu_visible))
    Print_dans_menu("\035:   1   \022:   1",0);
  // On laisse une trace du curseur � l'�cran
  Afficher_curseur();

  if (Mouse_K==A_GAUCHE)
  {
    Shade_Table=Shade_Table_gauche;
    Operation_PUSH(Fore_color);
  }
  else
  {
    Shade_Table=Shade_Table_droite;
    Operation_PUSH(Back_color);
  }

  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


void Rectangle_12_5(void)
// Op�ration   : OPERATION_RECTANGLE_VIDE / OPERATION_RECTANGLE_PLEIN
// Click Souris: 1 ou 2
// Taille_Pile : 5
//
// Souris effac�e: Non
{
  short Debut_X;
  short Debut_Y;
  short Ancien_X;
  short Ancien_Y;
  char  Chaine[5];

  Operation_POP(&Ancien_Y);
  Operation_POP(&Ancien_X);

  if ((Pinceau_X!=Ancien_X) || (Pinceau_Y!=Ancien_Y))
  {
    Operation_POP(&Debut_Y);
    Operation_POP(&Debut_X);

    if ((Config.Coords_rel) && (Menu_visible))
    {
      Num2str(((Debut_X<Pinceau_X)?Pinceau_X-Debut_X:Debut_X-Pinceau_X)+1,Chaine,4);
      Print_dans_menu(Chaine,2);
      Num2str(((Debut_Y<Pinceau_Y)?Pinceau_Y-Debut_Y:Debut_Y-Pinceau_Y)+1,Chaine,4);
      Print_dans_menu(Chaine,11);
    }
    else
      Print_coordonnees();

    Operation_PUSH(Debut_X);
    Operation_PUSH(Debut_Y);
  }

  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


void Rectangle_vide_0_5(void)
// Op�ration   : OPERATION_RECTANGLE_VIDE
// Click Souris: 0
// Taille_Pile : 5
//
// Souris effac�e: Oui
{
  short Ancien_Pinceau_X;
  short Ancien_Pinceau_Y;
  short Couleur;

  // On m�morise la position courante du pinceau:

  Ancien_Pinceau_X=Pinceau_X;
  Ancien_Pinceau_Y=Pinceau_Y;

  // On lit l'ancienne position du pinceau:

  Operation_POP(&Pinceau_Y);
  Operation_POP(&Pinceau_X);
  Operation_POP(&Pinceau_Y);
  Operation_POP(&Pinceau_X);

  // On va devoir effacer l'ancien curseur qu'on a laiss� trainer:
  Effacer_curseur();

  // On lit la couleur du rectangle:
  Operation_POP(&Couleur);

  // On fait un petit backup de l'image:
  Backup();

  // Et on trace le rectangle:
  Tracer_rectangle_vide(Pinceau_X,Pinceau_Y,Ancien_Pinceau_X,Ancien_Pinceau_Y,Couleur);

  // Enfin, on r�tablit la position du pinceau:
  Pinceau_X=Ancien_Pinceau_X;
  Pinceau_Y=Ancien_Pinceau_Y;

  if ((Config.Coords_rel) && (Menu_visible))
    Print_dans_menu("X:       Y:",0);
  Print_coordonnees();
}


void Rectangle_plein_0_5(void)
// Op�ration   : OPERATION_RECTANGLE_PLEIN
// Click Souris: 0
// Taille_Pile : 5
//
// Souris effac�e: Oui
{
  short Ancien_Pinceau_X;
  short Ancien_Pinceau_Y;
  short Couleur;

  // On m�morise la position courante du pinceau:

  Ancien_Pinceau_X=Pinceau_X;
  Ancien_Pinceau_Y=Pinceau_Y;

  // On lit l'ancienne position du pinceau:

  Operation_POP(&Pinceau_Y);
  Operation_POP(&Pinceau_X);
  Operation_POP(&Pinceau_Y);
  Operation_POP(&Pinceau_X);

  // On va devoir effacer l'ancien curseur qu'on a laiss� trainer:
  Effacer_curseur();

  // On lit la couleur du rectangle:
  Operation_POP(&Couleur);

  // On fait un petit backup de l'image:
  Backup();

  // Et on trace le rectangle:
  Tracer_rectangle_plein(Pinceau_X,Pinceau_Y,Ancien_Pinceau_X,Ancien_Pinceau_Y,Couleur);

  // Enfin, on r�tablit la position du pinceau:
  Pinceau_X=Ancien_Pinceau_X;
  Pinceau_Y=Ancien_Pinceau_Y;

  if ( (Config.Coords_rel) && (Menu_visible) )
  {
    Print_dans_menu("X:       Y:",0);
    Print_coordonnees();
  }
}


////////////////////////////////////////////////////// OPERATION_CERCLE_?????


void Cercle_12_0(void)
//
// Op�ration   : OPERATION_CERCLE_VIDE / OPERATION_CERCLE_PLEIN
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
{
  Initialiser_debut_operation();
  Backup();

  Pinceau_Forme_avant_operation=Pinceau_Forme;
  Pinceau_Forme=FORME_PINCEAU_POINT;

  if (Mouse_K==A_GAUCHE)
  {
    Shade_Table=Shade_Table_gauche;
    Operation_PUSH(Fore_color);
  }
  else
  {
    Shade_Table=Shade_Table_droite;
    Operation_PUSH(Back_color);
  }

  if ((Config.Coords_rel) && (Menu_visible))
    Print_dans_menu("Radius:   0    ",0);

  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


void Cercle_12_5(void)
//
// Op�ration   : OPERATION_CERCLE_VIDE / OPERATION_CERCLE_PLEIN
// Click Souris: 1 ou 2
// Taille_Pile : 5 (Couleur, X_Centre, Y_Centre, X_Tangente, Y_Tangente)
//
// Souris effac�e: Non
//
{
  short Tangente_X;
  short Tangente_Y;
  short Centre_X;
  short Centre_Y;
  short Couleur;
  short Rayon;
  char  Chaine[5];

  Operation_POP(&Tangente_Y);
  Operation_POP(&Tangente_X);
  Operation_POP(&Centre_Y);
  Operation_POP(&Centre_X);
  Operation_POP(&Couleur);

  if ( (Tangente_X!=Pinceau_X) || (Tangente_Y!=Pinceau_Y) )
  {
    Effacer_curseur();
    if ((Config.Coords_rel) && (Menu_visible))
    {
      Num2str(Distance(Centre_X,Centre_Y,Pinceau_X,Pinceau_Y),Chaine,4);
      Print_dans_menu(Chaine,7);
    }
    else
      Print_coordonnees();

    Cercle_Limite=((Tangente_X-Centre_X)*(Tangente_X-Centre_X))+
                  ((Tangente_Y-Centre_Y)*(Tangente_Y-Centre_Y));
    Rayon=sqrt(Cercle_Limite);
    Effacer_cercle_vide_Preview(Centre_X,Centre_Y,Rayon);

    Cercle_Limite=((Pinceau_X-Centre_X)*(Pinceau_X-Centre_X))+
                  ((Pinceau_Y-Centre_Y)*(Pinceau_Y-Centre_Y));
    Rayon=sqrt(Cercle_Limite);
    Tracer_cercle_vide_Preview(Centre_X,Centre_Y,Rayon,Couleur);

    Afficher_curseur();
  }

  Operation_PUSH(Couleur);
  Operation_PUSH(Centre_X);
  Operation_PUSH(Centre_Y);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


void Cercle_vide_0_5(void)
//
// Op�ration   : OPERATION_CERCLE_VIDE
// Click Souris: 0
// Taille_Pile : 5 (Couleur, X_Centre, Y_Centre, X_Tangente, Y_Tangente)
//
// Souris effac�e: Oui
//
{
  short Tangente_X;
  short Tangente_Y;
  short Centre_X;
  short Centre_Y;
  short Couleur;
  short Rayon;

  Operation_POP(&Tangente_Y);
  Operation_POP(&Tangente_X);
  Operation_POP(&Centre_Y);
  Operation_POP(&Centre_X);
  Operation_POP(&Couleur);

  Cercle_Limite=((Tangente_X-Centre_X)*(Tangente_X-Centre_X))+
                ((Tangente_Y-Centre_Y)*(Tangente_Y-Centre_Y));
  Rayon=sqrt(Cercle_Limite);
  Effacer_cercle_vide_Preview(Centre_X,Centre_Y,Rayon);

  Pinceau_Forme=Pinceau_Forme_avant_operation;

  Tracer_cercle_vide_Definitif(Centre_X,Centre_Y,Rayon,Couleur);

  if ( (Config.Coords_rel) && (Menu_visible) )
  {
    Print_dans_menu("X:       Y:",0);
    Print_coordonnees();
  }
}


void Cercle_plein_0_5(void)
//
// Op�ration   : OPERATION_CERCLE_PLEIN
// Click Souris: 0
// Taille_Pile : 5 (Couleur, X_Centre, Y_Centre, X_Tangente, Y_Tangente)
//
// Souris effac�e: Oui
//
{
  short Tangente_X;
  short Tangente_Y;
  short Centre_X;
  short Centre_Y;
  short Couleur;
  short Rayon;

  Operation_POP(&Tangente_Y);
  Operation_POP(&Tangente_X);
  Operation_POP(&Centre_Y);
  Operation_POP(&Centre_X);
  Operation_POP(&Couleur);

  Cercle_Limite=((Tangente_X-Centre_X)*(Tangente_X-Centre_X))+
                ((Tangente_Y-Centre_Y)*(Tangente_Y-Centre_Y));
  Rayon=sqrt(Cercle_Limite);
  Effacer_cercle_vide_Preview(Centre_X,Centre_Y,Rayon);

  Pinceau_Forme=Pinceau_Forme_avant_operation;

  Tracer_cercle_plein(Centre_X,Centre_Y,Rayon,Couleur);

  if ( (Config.Coords_rel) && (Menu_visible) )
  {
    Print_dans_menu("X:       Y:",0);
    Print_coordonnees();
  }
}


///////////////////////////////////////////////////// OPERATION_ELLIPSE_?????


void Ellipse_12_0(void)
//
// Op�ration   : OPERATION_ELLIPSE_VIDE / OPERATION_ELLIPSE_PLEINE
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
{
  Initialiser_debut_operation();
  Backup();

  Pinceau_Forme_avant_operation=Pinceau_Forme;
  Pinceau_Forme=FORME_PINCEAU_POINT;

  if (Mouse_K==A_GAUCHE)
  {
    Shade_Table=Shade_Table_gauche;
    Operation_PUSH(Fore_color);
  }
  else
  {
    Shade_Table=Shade_Table_droite;
    Operation_PUSH(Back_color);
  }

  if ((Config.Coords_rel) && (Menu_visible))
    Print_dans_menu("X:�   0   Y:�   0",0);

  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


void Ellipse_12_5(void)
//
// Op�ration   : OPERATION_ELLIPSE_VIDE / OPERATION_ELLIPSE_PLEINE
// Click Souris: 1 ou 2
// Taille_Pile : 5 (Couleur, X_Centre, Y_Centre, X_Tangente, Y_Tangente)
//
// Souris effac�e: Non
//
{
  short Tangente_X;
  short Tangente_Y;
  short Centre_X;
  short Centre_Y;
  short Couleur;
  short Rayon_horizontal;
  short Rayon_vertical;

  Operation_POP(&Tangente_Y);
  Operation_POP(&Tangente_X);
  Operation_POP(&Centre_Y);
  Operation_POP(&Centre_X);
  Operation_POP(&Couleur);

  if ( (Tangente_X!=Pinceau_X) || (Tangente_Y!=Pinceau_Y) )
  {
    Effacer_curseur();
    Aff_coords_rel_ou_abs(Centre_X,Centre_Y);

    Rayon_horizontal=(Tangente_X>Centre_X)?Tangente_X-Centre_X
                                           :Centre_X-Tangente_X;
    Rayon_vertical  =(Tangente_Y>Centre_Y)?Tangente_Y-Centre_Y
                                           :Centre_Y-Tangente_Y;
    Effacer_ellipse_vide_Preview(Centre_X,Centre_Y,Rayon_horizontal,Rayon_vertical);

    Rayon_horizontal=(Pinceau_X>Centre_X)?Pinceau_X-Centre_X
                                         :Centre_X-Pinceau_X;
    Rayon_vertical  =(Pinceau_Y>Centre_Y)?Pinceau_Y-Centre_Y
                                         :Centre_Y-Pinceau_Y;
    Tracer_ellipse_vide_Preview(Centre_X,Centre_Y,Rayon_horizontal,Rayon_vertical,Couleur);

    Afficher_curseur();
  }

  Operation_PUSH(Couleur);
  Operation_PUSH(Centre_X);
  Operation_PUSH(Centre_Y);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


void Ellipse_vide_0_5(void)
//
// Op�ration   : OPERATION_ELLIPSE_VIDE
// Click Souris: 0
// Taille_Pile : 5 (Couleur, X_Centre, Y_Centre, X_Tangente, Y_Tangente)
//
// Souris effac�e: Oui
//
{
  short Tangente_X;
  short Tangente_Y;
  short Centre_X;
  short Centre_Y;
  short Couleur;
  short Rayon_horizontal;
  short Rayon_vertical;

  Operation_POP(&Tangente_Y);
  Operation_POP(&Tangente_X);
  Operation_POP(&Centre_Y);
  Operation_POP(&Centre_X);
  Operation_POP(&Couleur);

  Rayon_horizontal=(Tangente_X>Centre_X)?Tangente_X-Centre_X
                                         :Centre_X-Tangente_X;
  Rayon_vertical  =(Tangente_Y>Centre_Y)?Tangente_Y-Centre_Y
                                         :Centre_Y-Tangente_Y;
  Effacer_ellipse_vide_Preview(Centre_X,Centre_Y,Rayon_horizontal,Rayon_vertical);

  Pinceau_Forme=Pinceau_Forme_avant_operation;

  Tracer_ellipse_vide_Definitif(Centre_X,Centre_Y,Rayon_horizontal,Rayon_vertical,Couleur);

  if ( (Config.Coords_rel) && (Menu_visible) )
  {
    Print_dans_menu("X:       Y:             ",0);
    Print_coordonnees();
  }
}


void Ellipse_pleine_0_5(void)
//
// Op�ration   : OPERATION_ELLIPSE_PLEINE
// Click Souris: 0
// Taille_Pile : 5 (Couleur, X_Centre, Y_Centre, X_Tangente, Y_Tangente)
//
// Souris effac�e: Oui
//
{
  short Tangente_X;
  short Tangente_Y;
  short Centre_X;
  short Centre_Y;
  short Couleur;
  short Rayon_horizontal;
  short Rayon_vertical;

  Operation_POP(&Tangente_Y);
  Operation_POP(&Tangente_X);
  Operation_POP(&Centre_Y);
  Operation_POP(&Centre_X);
  Operation_POP(&Couleur);

  Rayon_horizontal=(Tangente_X>Centre_X)?Tangente_X-Centre_X
                                         :Centre_X-Tangente_X;
  Rayon_vertical  =(Tangente_Y>Centre_Y)?Tangente_Y-Centre_Y
                                         :Centre_Y-Tangente_Y;
  Effacer_ellipse_vide_Preview(Centre_X,Centre_Y,Rayon_horizontal,Rayon_vertical);

  Pinceau_Forme=Pinceau_Forme_avant_operation;

  Tracer_ellipse_pleine(Centre_X,Centre_Y,Rayon_horizontal,Rayon_vertical,Couleur);

  if ( (Config.Coords_rel) && (Menu_visible) )
  {
    Print_dans_menu("X:       Y:             ",0);
    Print_coordonnees();
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
  // Pas besoin d'initialiser le d�but d'op�ration car le Smear n'affecte pas
  // le Fill, et on se fout de savoir si on est dans la partie gauche ou
  // droite de la loupe.
  // On ne s'occupe pas de faire un Backup: c'est "Remplir" qui s'en charge.
  Shade_Table=Shade_Table_gauche;
  Remplir(Fore_color);
  Attendre_fin_de_click();
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
  // Pas besoin d'initialiser le d�but d'op�ration car le Smear n'affecte pas
  // le Fill, et on se fout de savoir si on est dans la partie gauche ou
  // droite de la loupe.
  // On ne s'occupe pas de faire un Backup: c'est "Remplir" qui s'en charge.
  Shade_Table=Shade_Table_droite;
  Remplir(Back_color);
  Attendre_fin_de_click();
}


///////////////////////////////////////////////////////// OPERATION_REMPLACER


void Remplacer_1_0(void)
//
// Op�ration   : OPERATION_REMPLACER
// Click Souris: 1
// Taille_Pile : 0
//
// Souris effac�e: Oui
//
{
  // Pas besoin d'initialiser le d�but d'op�ration car le Smear n'affecte pas
  // le Replace, et on se fout de savoir si on est dans la partie gauche ou
  // droite de la loupe.
  Backup();
//  Shade_Table=Shade_Table_gauche;
  Remplacer(Fore_color);
  Attendre_fin_de_click();
}


void Remplacer_2_0(void)
//
// Op�ration   : OPERATION_REMPLACER
// Click Souris: 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
//
{
  // Pas besoin d'initialiser le d�but d'op�ration car le Smear n'affecte pas
  // le Replace, et on se fout de savoir si on est dans la partie gauche ou
  // droite de la loupe.
  Backup();
//  Shade_Table=Shade_Table_droite;
  Remplacer(Back_color);
  Attendre_fin_de_click();
}


/////////////////////////////////////////////////////////// OPERATION_PIPETTE


void Pipette_12_0(void)
//
// Op�ration   : OPERATION_PIPETTE
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
//
{
  Initialiser_debut_operation();

  if (Mouse_K==A_GAUCHE)
  {
    Encadrer_couleur_menu(CM_Noir);
    Fore_color=Pipette_Couleur;
    Recadrer_palette();
    Afficher_foreback();
    Encadrer_couleur_menu(CM_Blanc);
  }
  else
  {
    Back_color=Pipette_Couleur;
    Afficher_foreback();
  }
  Operation_PUSH(Mouse_K);
}


void Pipette_1_1(void)
//
// Op�ration   : OPERATION_PIPETTE
// Click Souris: 1
// Taille_Pile : 1
//
// Souris effac�e: Non
//
{
  char Chaine[4];

  if ( (Pinceau_X>=0) && (Pinceau_Y>=0)
    && (Pinceau_X<Principal_Largeur_image)
    && (Pinceau_Y<Principal_Hauteur_image) )
    Pipette_Couleur=Lit_pixel_dans_ecran_courant(Pinceau_X,Pinceau_Y);
  else
    Pipette_Couleur=0;

  if ( (Pipette_X!=Pinceau_X)
    || (Pipette_Y!=Pinceau_Y) )
  {
    Effacer_curseur();
    Pipette_X=Pinceau_X;
    Pipette_Y=Pinceau_Y;

    if (Pipette_Couleur!=Fore_color)
    {
      Encadrer_couleur_menu(CM_Noir);
      Fore_color=Pipette_Couleur;
      Recadrer_palette();
      Afficher_foreback();
      Encadrer_couleur_menu(CM_Blanc);
    }

    if (Menu_visible)
    {
      Print_coordonnees();
      Num2str(Pipette_Couleur,Chaine,3);
      Print_dans_menu(Chaine,20);
      Print_general(170*Menu_Facteur_X,Menu_Ordonnee_Texte," ",0,Pipette_Couleur);
    }
    Afficher_curseur();
  }
}


void Pipette_2_1(void)
//
// Op�ration   : OPERATION_PIPETTE
// Click Souris: 2
// Taille_Pile : 1
//
// Souris effac�e: Non
//
{
  char Chaine[4];

  if ( (Pinceau_X>=0) && (Pinceau_Y>=0)
    && (Pinceau_X<Principal_Largeur_image)
    && (Pinceau_Y<Principal_Hauteur_image) )
    Pipette_Couleur=Lit_pixel_dans_ecran_courant(Pinceau_X,Pinceau_Y);
  else
    Pipette_Couleur=0;

  if ( (Pipette_X!=Pinceau_X)
    || (Pipette_Y!=Pinceau_Y) )
  {
    Effacer_curseur();
    Pipette_X=Pinceau_X;
    Pipette_Y=Pinceau_Y;

    if (Pipette_Couleur!=Back_color)
    {
      Back_color=Pipette_Couleur;
      Afficher_foreback();
    }

    if (Menu_visible)
    {
      Print_coordonnees();
      Num2str(Pipette_Couleur,Chaine,3);
      Print_dans_menu(Chaine,20);
      Print_general(170*Menu_Facteur_X,Menu_Ordonnee_Texte," ",0,Pipette_Couleur);
    }
    Afficher_curseur();
  }
}



void Pipette_0_1(void)
//
// Op�ration   : OPERATION_PIPETTE
// Click Souris: 0
// Taille_Pile : 1
//
// Souris effac�e: Oui
//
{
  short Clic;

  Operation_POP(&Clic);
  if (Clic==A_GAUCHE)
  {
    Encadrer_couleur_menu(CM_Noir);
    Fore_color=Pipette_Couleur;
    Recadrer_palette();
    Afficher_foreback();
    Encadrer_couleur_menu(CM_Blanc);
  }
  else
  {
    Back_color=Pipette_Couleur;
    Afficher_foreback();
  }
  Desenclencher_bouton(BOUTON_PIPETTE);
}


/////////////////////////////////////////////////// OPERATION_COURBE_4_POINTS


void Courbe_Tracer_croix(short Pos_X, short Pos_Y)
{
  short Debut_X,Fin_X;
  short Debut_Y,Fin_Y;
  short i,Temp;
  //byte  Temp2;

  if (Pos_X>=Limite_Gauche+3)
    Debut_X=0;
  else
    Debut_X=3-(Pos_X-Limite_Gauche);

  if (Pos_Y>=Limite_Haut+3)
    Debut_Y=0;
  else
    Debut_Y=3-(Pos_Y-Limite_Haut);

  if (Pos_X<=Limite_visible_Droite-3)
    Fin_X=6;
  else
    Fin_X=3+(Limite_visible_Droite-Pos_X);

  if (Pos_Y<=Limite_visible_Bas-3)
    Fin_Y=6;
  else
    Fin_Y=3+(Limite_visible_Bas-Pos_Y);

  for (i=Debut_X; i<=Fin_X; i++)
  {
    Temp=Pos_X+i-3;
    Pixel_Preview(Temp,Pos_Y,~Lit_pixel(Temp -Principal_Decalage_X,
                                        Pos_Y-Principal_Decalage_Y));
  }
  for (i=Debut_Y; i<=Fin_Y; i++)
  {
    Temp=Pos_Y+i-3;
    Pixel_Preview(Pos_X,Temp,~Lit_pixel(Pos_X-Principal_Decalage_X,
                                        Temp -Principal_Decalage_Y));
  }
}


void Courbe_34_points_1_0(void)
//
//  Op�ration   : OPERATION_COURBE_?_POINTS
//  Click Souris: 1
//  Taille_Pile : 0
//
//  Souris effac�e: Oui
//
{
  Initialiser_debut_operation();
  Backup();
  Shade_Table=Shade_Table_gauche;

  Cacher_pinceau=1;

  Pixel_figure_Preview(Pinceau_X,Pinceau_Y,Fore_color);

  if ((Config.Coords_rel) && (Menu_visible))
    Print_dans_menu("X:�   0   Y:�   0",0);

  Operation_PUSH(Fore_color);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}

void Courbe_34_points_2_0(void)
//
//  Op�ration   : OPERATION_COURBE_?_POINTS
//  Click Souris: 2
//  Taille_Pile : 0
//
//  Souris effac�e: Oui
//
{
  Initialiser_debut_operation();
  Backup();
  Shade_Table=Shade_Table_droite;

  Cacher_pinceau=1;

  Pixel_figure_Preview(Pinceau_X,Pinceau_Y,Back_color);

  if ((Config.Coords_rel) && (Menu_visible))
    Print_dans_menu("X:�   0   Y:�   0",0);

  Operation_PUSH(Back_color);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


void Courbe_34_points_1_5(void)
//
//  Op�ration   : OPERATION_COURBE_?_POINTS
//  Click Souris: 1
//  Taille_Pile : 5
//
//  Souris effac�e: Non
//
{
  short X1,X2,Y1,Y2;

  Operation_POP(&Y2);
  Operation_POP(&X2);
  Operation_POP(&Y1);
  Operation_POP(&X1);

  if ( (Y2!=Pinceau_Y) || (X2!=Pinceau_X) )
  {
    Effacer_curseur();
    Aff_coords_rel_ou_abs(X1,Y1);

    Effacer_ligne_Preview(X1,Y1,X2,Y2);
    Pixel_figure_Preview (X1,Y1,Fore_color);
    Tracer_ligne_Preview (X1,Y1,Pinceau_X,Pinceau_Y,Fore_color);

    Afficher_curseur();
  }

  Operation_PUSH(X1);
  Operation_PUSH(Y1);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}

void Courbe_34_points_2_5(void)
//
//  Op�ration   : OPERATION_COURBE_?_POINTS
//  Click Souris: 2
//  Taille_Pile : 5
//
//  Souris effac�e: Non
//
{
  short X1,X2,Y1,Y2;

  Operation_POP(&Y2);
  Operation_POP(&X2);
  Operation_POP(&Y1);
  Operation_POP(&X1);

  if ( (Y2!=Pinceau_Y) || (X2!=Pinceau_X) )
  {
    Effacer_curseur();
    Aff_coords_rel_ou_abs(X1,Y1);

    Effacer_ligne_Preview(X1,Y1,X2,Y2);
    Pixel_figure_Preview (X1,Y1,Back_color);
    Tracer_ligne_Preview (X1,Y1,Pinceau_X,Pinceau_Y,Back_color);

    Afficher_curseur();
  }

  Operation_PUSH(X1);
  Operation_PUSH(Y1);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


byte Cacher_curseur_avant_courbe;

void Courbe_4_points_0_5(void)
//
//  Op�ration   : OPERATION_COURBE_4_POINTS
//  Click Souris: 0
//  Taille_Pile : 5
//
//  Souris effac�e: Oui
//
{
  short X1,Y1,X2,Y2,X3,Y3,X4,Y4;
  short Tiers_X,Tiers_Y;
  short Couleur;

  Operation_POP(&Y4);
  Operation_POP(&X4);
  Operation_POP(&Y1);
  Operation_POP(&X1);
  Operation_POP(&Couleur);

  Tiers_X=Round_div(abs(X4-X1),3);
  Tiers_Y=Round_div(abs(Y4-Y1),3);

  if (X1<X4)
  {
    X2=X1+Tiers_X;
    X3=X4-Tiers_X;
  }
  else
  {
    X3=X4+Tiers_X;
    X2=X1-Tiers_X;
  }

  if (Y1<Y4)
  {
    Y2=Y1+Tiers_Y;
    Y3=Y4-Tiers_Y;
  }
  else
  {
    Y3=Y4+Tiers_Y;
    Y2=Y1-Tiers_Y;
  }

  Effacer_ligne_Preview(X1,Y1,X4,Y4);
  Tracer_courbe_Preview(X1,Y1,X2,Y2,X3,Y3,X4,Y4,Couleur);

  // On trace les petites croix montrant les 2 points interm�diares
  Courbe_Tracer_croix(X2,Y2);
  Courbe_Tracer_croix(X3,Y3);

  Cacher_curseur_avant_courbe=Cacher_curseur;
  Cacher_curseur=0;

  Operation_PUSH(Couleur);
  Operation_PUSH(X1);
  Operation_PUSH(Y1);
  Operation_PUSH(X2);
  Operation_PUSH(Y2);
  Operation_PUSH(X3);
  Operation_PUSH(Y3);
  Operation_PUSH(X4);
  Operation_PUSH(Y4);

  if ( (Config.Coords_rel) && (Menu_visible) )
  {
    Print_dans_menu("X:       Y:             ",0);
    Print_coordonnees();
  }
}


void Courbe_4_points_1_9(void)
//
//  Op�ration   : OPERATION_COURBE_4_POINTS
//  Click Souris: 1
//  Taille_Pile : 9
//
//  Souris effac�e: Non
//
{
  short X1,Y1,X2,Y2,X3,Y3,X4,Y4;
  short Couleur;
  byte  C_est_X2;

  Operation_POP(&Y4);
  Operation_POP(&X4);
  Operation_POP(&Y3);
  Operation_POP(&X3);
  Operation_POP(&Y2);
  Operation_POP(&X2);
  Operation_POP(&Y1);
  Operation_POP(&X1);
  Operation_POP(&Couleur);

  C_est_X2=(Distance(Pinceau_X,Pinceau_Y,X2,Y2) < Distance(Pinceau_X,Pinceau_Y,X3,Y3));

  if ( (   C_est_X2  && ( (Pinceau_X!=X2) || (Pinceau_Y!=Y2) ) )
    || ( (!C_est_X2) && ( (Pinceau_X!=X3) || (Pinceau_Y!=Y3) ) ) )
  {
    Effacer_curseur();
    Print_coordonnees();

    Courbe_Tracer_croix(X2,Y2);
    Courbe_Tracer_croix(X3,Y3);

    Effacer_courbe_Preview(X1,Y1,X2,Y2,X3,Y3,X4,Y4,Couleur);

    if (C_est_X2)
    {
      X2=Pinceau_X;
      Y2=Pinceau_Y;
    }
    else
    {
      X3=Pinceau_X;
      Y3=Pinceau_Y;
    }

    Tracer_courbe_Preview(X1,Y1,X2,Y2,X3,Y3,X4,Y4,Couleur);

    Courbe_Tracer_croix(X2,Y2);
    Courbe_Tracer_croix(X3,Y3);

    Afficher_curseur();
  }

  Operation_PUSH(Couleur);
  Operation_PUSH(X1);
  Operation_PUSH(Y1);
  Operation_PUSH(X2);
  Operation_PUSH(Y2);
  Operation_PUSH(X3);
  Operation_PUSH(Y3);
  Operation_PUSH(X4);
  Operation_PUSH(Y4);
}


void Courbe_4_points_2_9(void)
//
//  Op�ration   : OPERATION_COURBE_4_POINTS
//  Click Souris: 2
//  Taille_Pile : 9
//
//  Souris effac�e: Oui
//
{
  short X1,Y1,X2,Y2,X3,Y3,X4,Y4;
  short Couleur;

  Operation_POP(&Y4);
  Operation_POP(&X4);
  Operation_POP(&Y3);
  Operation_POP(&X3);
  Operation_POP(&Y2);
  Operation_POP(&X2);
  Operation_POP(&Y1);
  Operation_POP(&X1);
  Operation_POP(&Couleur);

  Courbe_Tracer_croix(X2,Y2);
  Courbe_Tracer_croix(X3,Y3);

  Cacher_pinceau=0;
  Cacher_curseur=Cacher_curseur_avant_courbe;

  Effacer_courbe_Preview(X1,Y1,X2,Y2,X3,Y3,X4,Y4,Couleur);
  Tracer_courbe_Definitif(X1,Y1,X2,Y2,X3,Y3,X4,Y4,Couleur);

  Attendre_fin_de_click();
}


/////////////////////////////////////////////////// OPERATION_COURBE_3_POINTS


void Calculer_courbe_3_points(short X1, short Y1, short X4, short Y4,
                              short * X2, short * Y2, short * X3, short * Y3)
{
  float CX,CY; // Centre de (X1,Y1) et (X4,Y4)
  float BX,BY; // Intersect. des dtes ((X1,Y1),(X2,Y2)) et ((X3,Y3),(X4,Y4))

  CX=(float)(X1+X4)/2.0;           // P1*--_               L�gende:
  CY=(float)(Y1+Y4)/2.0;           //   �   \�� P2         -_\|/ : courbe
                                   //   �    \ �*�         * : point important
  BX=CX+((8.0/3.0)*(Pinceau_X-CX));//   �     |   ��       � : pointill�
  BY=CY+((8.0/3.0)*(Pinceau_Y-CY));//   �     |P    ��  B
                                   // C *�����*���������*  P=Pos. du pinceau
  *X2=Round((BX+X1)/2.0);          //   �     |     ��     C=milieu de [P1,P4]
  *Y2=Round((BY+Y1)/2.0);          //   �     |   ��       B=point tel que
                                   //   �    / �*�         C->B=(8/3) * C->P
  *X3=Round((BX+X4)/2.0);          //   �  _/�� P3         P2=milieu de [P1,B]
  *Y3=Round((BY+Y4)/2.0);          // P4*--                P3=milieu de [P4,B]
}


void Courbe_3_points_0_5(void)
//
//  Op�ration   : OPERATION_COURBE_3_POINTS
//  Click Souris: 0
//  Taille_Pile : 5
//
//  Souris effac�e: Oui
//
{
  short X1,Y1,X2,Y2,X3,Y3,X4,Y4;
  short Couleur;

  Operation_POP(&Y4);
  Operation_POP(&X4);
  Operation_POP(&Y1);
  Operation_POP(&X1);
  Operation_POP(&Couleur);

  Calculer_courbe_3_points(X1,Y1,X4,Y4,&X2,&Y2,&X3,&Y3);

  Effacer_ligne_Preview(X1,Y1,X4,Y4);
  Tracer_courbe_Preview(X1,Y1,X2,Y2,X3,Y3,X4,Y4,Couleur);

  if ( (Config.Coords_rel) && (Menu_visible) )
  {
    Print_dans_menu("X:       Y:             ",0);
    Print_coordonnees();
  }

  Operation_PUSH(Couleur);
  Operation_PUSH(X1);
  Operation_PUSH(Y1);
  Operation_PUSH(X2);
  Operation_PUSH(Y2);
  Operation_PUSH(X3);
  Operation_PUSH(Y3);
  Operation_PUSH(X4);
  Operation_PUSH(Y4);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


void Courbe_3_points_0_11(void)
//
//  Op�ration   : OPERATION_COURBE_3_POINTS
//  Click Souris: 0
//  Taille_Pile : 11
//
//  Souris effac�e: Non
//
{
  short X1,Y1,X2,Y2,X3,Y3,X4,Y4;
  short Old_X,Old_Y;
  short Couleur;

  Operation_POP(&Old_Y);
  Operation_POP(&Old_X);

  if ( (Pinceau_X!=Old_X) || (Pinceau_Y!=Old_Y) )
  {
    Operation_POP(&Y4);
    Operation_POP(&X4);
    Operation_POP(&Y3);
    Operation_POP(&X3);
    Operation_POP(&Y2);
    Operation_POP(&X2);
    Operation_POP(&Y1);
    Operation_POP(&X1);
    Operation_POP(&Couleur);

    Effacer_curseur();
    Print_coordonnees();

    Effacer_courbe_Preview(X1,Y1,X2,Y2,X3,Y3,X4,Y4,Couleur);
    Calculer_courbe_3_points(X1,Y1,X4,Y4,&X2,&Y2,&X3,&Y3);
    Tracer_courbe_Preview (X1,Y1,X2,Y2,X3,Y3,X4,Y4,Couleur);
    Afficher_curseur();

    Operation_PUSH(Couleur);
    Operation_PUSH(X1);
    Operation_PUSH(Y1);
    Operation_PUSH(X2);
    Operation_PUSH(Y2);
    Operation_PUSH(X3);
    Operation_PUSH(Y3);
    Operation_PUSH(X4);
    Operation_PUSH(Y4);
  }
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


void Courbe_3_points_12_11(void)
//
//  Op�ration   : OPERATION_COURBE_3_POINTS
//  Click Souris: 1 ou 2
//  Taille_Pile : 11
//
//  Souris effac�e: Oui
//
{
  short X1,Y1,X2,Y2,X3,Y3,X4,Y4;
  short Old_X,Old_Y;
  short Couleur;

  Operation_POP(&Old_Y);
  Operation_POP(&Old_X);
  Operation_POP(&Y4);
  Operation_POP(&X4);
  Operation_POP(&Y3);
  Operation_POP(&X3);
  Operation_POP(&Y2);
  Operation_POP(&X2);
  Operation_POP(&Y1);
  Operation_POP(&X1);
  Operation_POP(&Couleur);

  Cacher_pinceau=0;

  Effacer_courbe_Preview (X1,Y1,X2,Y2,X3,Y3,X4,Y4,Couleur);
  Calculer_courbe_3_points(X1,Y1,X4,Y4,&X2,&Y2,&X3,&Y3);
  Tracer_courbe_Definitif(X1,Y1,X2,Y2,X3,Y3,X4,Y4,Couleur);

  Attendre_fin_de_click();
}


///////////////////////////////////////////////////////////// OPERATION_SPRAY


void Spray_1_0(void)
//
//  Op�ration   : OPERATION_SPRAY
//  Click Souris: 1
//  Taille_Pile : 0
//
//  Souris effac�e: Non
//
{
  Initialiser_debut_operation();
  Backup();
  Shade_Table=Shade_Table_gauche;
  Aerographe(A_GAUCHE);

  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Operation_PUSH(A_GAUCHE);
}

void Spray_2_0(void)
//
//  Op�ration   : OPERATION_SPRAY
//  Click Souris: 2
//  Taille_Pile : 0
//
//  Souris effac�e: Non
//
{
  Initialiser_debut_operation();
  Backup();
  Shade_Table=Shade_Table_droite;
  Aerographe(A_DROITE);

  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Operation_PUSH(A_DROITE);
}

void Spray_12_3(void)
//
//  Op�ration   : OPERATION_SPRAY
//  Click Souris: 1 ou 2
//  Taille_Pile : 1
//
//  Souris effac�e: Non
//
{
  short Bouton_clicke;
  short Ancien_X,Ancien_Y;

  Operation_POP(&Bouton_clicke);
  Operation_POP(&Ancien_Y);
  Operation_POP(&Ancien_X);

  if ( (Menu_visible) && ((Pinceau_X!=Ancien_X) || (Pinceau_Y!=Ancien_Y)) )
  {
    Effacer_curseur();
    Print_coordonnees();
    Afficher_curseur();
  }

  Aerographe(Bouton_clicke);

  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Operation_PUSH(Bouton_clicke);
}

void Spray_0_3(void)
//
//  Op�ration   : OPERATION_SPRAY
//  Click Souris: 0
//  Taille_Pile : 3
//
//  Souris effac�e: Non
//
{
  Operation_Taille_pile-=3;
}


////////////////////////////////////////////////////////// OPERATION_POLYGONE


void Polygone_12_0(void)
// Op�ration   : OPERATION_POLYGONE
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
{
  byte Couleur;

  Initialiser_debut_operation();
  Backup();
  Shade_Table=(Mouse_K==A_GAUCHE)?Shade_Table_gauche:Shade_Table_droite;
  Pinceau_Forme_avant_operation=Pinceau_Forme;
  Pinceau_Forme=FORME_PINCEAU_POINT;

  Couleur=(Mouse_K==A_GAUCHE)?Fore_color:Back_color;

  // On place temporairement le d�but de la ligne qui ne s'afficherait pas sinon
  Pixel_figure_Preview(Pinceau_X,Pinceau_Y,Couleur);

  if ((Config.Coords_rel) && (Menu_visible))
    Print_dans_menu("X:�   0   Y:�   0",0);

  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Operation_PUSH(Mouse_K | 0x80);
  Operation_PUSH(Couleur);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  // Taille de pile 8 : phase d'appui, non interruptible
}



void Polygone_12_9(void)
// Op�ration   : OPERATION_POLYGONE
// Click Souris: 1 ou 2
// Taille_Pile : 9
//
// Souris effac�e: Oui
{
  short Debut_X;
  short Debut_Y;
  short Fin_X;
  short Fin_Y;
  short Couleur;
  short Direction;

  Operation_POP(&Fin_Y);
  Operation_POP(&Fin_X);
  Operation_POP(&Debut_Y);
  Operation_POP(&Debut_X);
  Operation_POP(&Couleur);
  Operation_POP(&Direction);
  Operation_POP(&Direction);

  if (Direction==Mouse_K)
  {
    Operation_PUSH(Direction);
    Operation_PUSH(Couleur);
    Operation_PUSH(Debut_X);
    Operation_PUSH(Debut_Y);
    Operation_PUSH(Fin_X);
    Operation_PUSH(Fin_Y);
    // Taille de pile 8 : phase d'appui, non interruptible
  }
  else
  {
    //   La s�rie de ligne est termin�e, il faut donc effacer la derni�re
    // preview de ligne et relier le dernier point avec le premier
    Pixel_figure_Preview  (Debut_X,Debut_Y,Lit_pixel_dans_ecran_courant(Debut_X,Debut_Y));
    Effacer_ligne_Preview (Debut_X,Debut_Y,Fin_X,Fin_Y);
    Operation_POP(&Fin_Y);
    Operation_POP(&Fin_X);
    Pinceau_Forme=Pinceau_Forme_avant_operation;
    // Le pied aurait �t� de ne pas repasser sur le 1er point de la 1�re ligne
    // mais c'est pas possible :(
    Tracer_ligne_Definitif(Debut_X,Debut_Y,Fin_X,Fin_Y,Couleur);
    Pinceau_Forme=FORME_PINCEAU_POINT;

    Afficher_curseur();
    Attendre_fin_de_click();
    Effacer_curseur();

    if ( (Config.Coords_rel) && (Menu_visible) )
    {
      Print_dans_menu("X:       Y:             ",0);
      Print_coordonnees();
    }

    Pinceau_Forme=Pinceau_Forme_avant_operation;
  }
}


////////////////////////////////////////////////////////// OPERATION_POLYFILL

short * Polyfill_Table_de_points;
int Polyfill_Nombre_de_points;

void Polyfill_12_0(void)
// Op�ration   : OPERATION_POLYFILL
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
{
  byte Couleur;

  Initialiser_debut_operation();
  Backup();
  Shade_Table=(Mouse_K==A_GAUCHE)?Shade_Table_gauche:Shade_Table_droite;
  Cacher_pinceau=1;

  Couleur=(Mouse_K==A_GAUCHE)?Fore_color:Back_color;

  Polyfill_Table_de_points=(short *) malloc((Config.Nb_max_de_vertex_par_polygon<<1)*sizeof(short));
  Polyfill_Table_de_points[0]=Pinceau_X;
  Polyfill_Table_de_points[1]=Pinceau_Y;
  Polyfill_Nombre_de_points=1;

  // On place temporairement le d�but de la ligne qui ne s'afficherait pas sinon
  Pixel_figure_Preview_xor(Pinceau_X,Pinceau_Y,0);

  if ((Config.Coords_rel) && (Menu_visible))
    Print_dans_menu("X:�   0   Y:�   0",0);

  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Operation_PUSH(Mouse_K | 0x80);
  Operation_PUSH(Couleur);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  // Taille de pile 8 : phase d'appui, non interruptible
}


void Polyfill_0_8(void)
// Op�ration   : OPERATION_POLYFILL
// Click Souris: 0
// Taille_Pile : 8
//
// Souris effac�e: Oui
{
  short Debut_X;
  short Debut_Y;
  short Fin_X;
  short Fin_Y;
  short Couleur;
  short Direction;

  Operation_POP(&Fin_Y);
  Operation_POP(&Fin_X);
  Operation_POP(&Debut_Y);
  Operation_POP(&Debut_X);
  Operation_POP(&Couleur);
  Operation_POP(&Direction);

  if ((Config.Coords_rel) && (Menu_visible))
    Print_dans_menu("X:�   0   Y:�   0",0);

  Tracer_ligne_Preview_xor(Debut_X,Debut_Y,Fin_X,Fin_Y,0);

  if (Direction & 0x80)
    Direction=(Direction & 0x7F);

  Operation_PUSH(Direction); // Valeur bidon servant de nouvel �tat de pile
  Operation_PUSH(Direction);
  Operation_PUSH(Couleur);

  Tracer_ligne_Preview_xor(Debut_X,Debut_Y,Pinceau_X,Pinceau_Y,0);

  if (Polyfill_Nombre_de_points<Config.Nb_max_de_vertex_par_polygon)
  {
    Polyfill_Table_de_points[Polyfill_Nombre_de_points<<1]    =Pinceau_X;
    Polyfill_Table_de_points[(Polyfill_Nombre_de_points<<1)+1]=Pinceau_Y;
    Polyfill_Nombre_de_points++;

    Operation_PUSH(Pinceau_X);
    Operation_PUSH(Pinceau_Y);
    Operation_PUSH(Pinceau_X);
    Operation_PUSH(Pinceau_Y);
  }
  else
  {
    Operation_PUSH(Debut_X);
    Operation_PUSH(Debut_Y);
    Operation_PUSH(Pinceau_X);
    Operation_PUSH(Pinceau_Y);
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
  short Debut_X;
  short Debut_Y;
  short Fin_X;
  short Fin_Y;

  Operation_POP(&Fin_Y);
  Operation_POP(&Fin_X);

  if ((Pinceau_X!=Fin_X) || (Pinceau_Y!=Fin_Y))
  {
    Effacer_curseur();
    Operation_POP(&Debut_Y);
    Operation_POP(&Debut_X);

    Aff_coords_rel_ou_abs(Debut_X,Debut_Y);

    Tracer_ligne_Preview_xor(Debut_X,Debut_Y,Fin_X,Fin_Y,0);
    Tracer_ligne_Preview_xor(Debut_X,Debut_Y,Pinceau_X,Pinceau_Y,0);

    Operation_PUSH(Debut_X);
    Operation_PUSH(Debut_Y);
    Afficher_curseur();
  }

  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


void Polyfill_12_9(void)
// Op�ration   : OPERATION_POLYFILL
// Click Souris: 1 ou 2
// Taille_Pile : 9
//
// Souris effac�e: Oui
{
  short Debut_X;
  short Debut_Y;
  short Fin_X;
  short Fin_Y;
  short Couleur;
  short Direction;

  Operation_POP(&Fin_Y);
  Operation_POP(&Fin_X);
  Operation_POP(&Debut_Y);
  Operation_POP(&Debut_X);
  Operation_POP(&Couleur);
  Operation_POP(&Direction);
  Operation_POP(&Direction);

  if (Direction==Mouse_K)
  {
    Operation_PUSH(Direction);
    Operation_PUSH(Couleur);
    Operation_PUSH(Debut_X);
    Operation_PUSH(Debut_Y);
    Operation_PUSH(Fin_X);
    Operation_PUSH(Fin_Y);
    // Taille de pile 8 : phase d'appui, non interruptible
  }
  else
  {
    //   La s�rie de lignes est termin�e, il faut donc effacer la derni�re
    // preview de ligne et relier le dernier point avec le premier
    Tracer_ligne_Preview_xor(Debut_X,Debut_Y,Fin_X,Fin_Y,0);
    Operation_POP(&Fin_Y);
    Operation_POP(&Fin_X);
    Tracer_ligne_Preview_xor(Debut_X,Debut_Y,Fin_X,Fin_Y,0);

    Afficher_curseur();
    Attendre_fin_de_click();
    Effacer_curseur();

    Afficher_ecran();
    Polyfill(Polyfill_Nombre_de_points,Polyfill_Table_de_points,Couleur);
    free(Polyfill_Table_de_points);

    if ( (Config.Coords_rel) && (Menu_visible) )
    {
      Print_dans_menu("X:       Y:             ",0);
      Print_coordonnees();
    }

    Cacher_pinceau=0;
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
  short Couleur;

  Initialiser_debut_operation();
  Backup();
  Shade_Table=(Mouse_K==A_GAUCHE)?Shade_Table_gauche:Shade_Table_droite;

  Couleur=(Mouse_K==A_GAUCHE)?Fore_color:Back_color;

  // On place un premier pinceau en (Pinceau_X,Pinceau_Y):
  Afficher_pinceau(Pinceau_X,Pinceau_Y,Couleur,0);
  // Et on affiche un pixel de preview en (Pinceau_X,Pinceau_Y):
  Pixel_figure_Preview(Pinceau_X,Pinceau_Y,Couleur);

  Operation_PUSH(Pinceau_X); // X Initial
  Operation_PUSH(Pinceau_Y); // X Initial
  Operation_PUSH(Couleur);   // Couleur de remplissage
  Operation_PUSH(Pinceau_X); // Debut X
  Operation_PUSH(Pinceau_Y); // Debut Y
  Operation_PUSH(Pinceau_X); // Fin X
  Operation_PUSH(Pinceau_Y); // Fin Y
  Operation_PUSH(Mouse_K);   // Clic
}


void Polyform_12_8(void)
//  Op�ration   : OPERATION_POLYFORM
//  Click Souris: 1 ou 2
//  Taille_Pile : 8
//
//  Souris effac�e: Non
{
  short Clic;
  short Fin_Y;
  short Fin_X;
  short Debut_Y;
  short Debut_X;
  short Couleur;
  short Initial_Y;
  short Initial_X;

  Operation_POP(&Clic);
  Operation_POP(&Fin_Y);
  Operation_POP(&Fin_X);
  Operation_POP(&Debut_Y);
  Operation_POP(&Debut_X);

  if (Clic==Mouse_K)
  {
    // L'utilisateur clic toujours avec le bon bouton de souris

    if ((Debut_X!=Pinceau_X) || (Debut_Y!=Pinceau_Y))
    {
      // Il existe un segment d�finit par (Debut_X,Debut_Y)-(Pinceau_X,Pinceau_Y)

      Effacer_curseur();
      Print_coordonnees();

      Operation_POP(&Couleur);

      // On efface la preview du segment valid�:
      Effacer_ligne_Preview(Debut_X,Debut_Y,Fin_X,Fin_Y);

      // On l'affiche de fa�on d�finitive:
      Tracer_ligne_Definitif(Debut_X,Debut_Y,Pinceau_X,Pinceau_Y,Couleur);

      // Et on affiche un pixel de preview en (Pinceau_X,Pinceau_Y):
      Pixel_figure_Preview(Pinceau_X,Pinceau_Y,Couleur);

      Operation_PUSH(Couleur);

      Afficher_curseur();
    }

    Operation_PUSH(Pinceau_X); // Nouveau Debut_X
    Operation_PUSH(Pinceau_Y); // Nouveau Debut_Y
    Operation_PUSH(Pinceau_X); // Nouveau Fin_X
    Operation_PUSH(Pinceau_Y); // Nouveau Fin_Y
    Operation_PUSH(Clic);
  }
  else
  {
    // L'utilisateur souhaite arr�ter l'op�ration et refermer le polygone

    Operation_POP(&Couleur);
    Operation_POP(&Initial_Y);
    Operation_POP(&Initial_X);

    Attendre_fin_de_click();
    Effacer_curseur();
    Print_coordonnees();

    // On efface la preview du segment annul�:
    Effacer_ligne_Preview(Debut_X,Debut_Y,Fin_X,Fin_Y);

    // On affiche de fa�on d�finitive le bouclage du polygone:
    Tracer_ligne_Definitif(Debut_X,Debut_Y,Initial_X,Initial_Y,Couleur);

    Afficher_curseur();
  }
}


void Polyform_0_8(void)
//  Op�ration   : OPERATION_POLYFORM
//  Click Souris: 0
//  Taille_Pile : 8
//
//  Souris effac�e: Non
{
  short Clic;
  short Fin_Y;
  short Fin_X;
  short Debut_Y;
  short Debut_X;
  short Couleur;

  Operation_POP(&Clic);
  Operation_POP(&Fin_Y);
  Operation_POP(&Fin_X);

  if ((Fin_X!=Pinceau_X) || (Fin_Y!=Pinceau_Y))
  {
    Effacer_curseur();
    Print_coordonnees();

    Operation_POP(&Debut_Y);
    Operation_POP(&Debut_X);
    Operation_POP(&Couleur);

    // On met � jour l'affichage de la preview du prochain segment:
    Effacer_ligne_Preview(Debut_X,Debut_Y,Fin_X,Fin_Y);
    Tracer_ligne_Preview (Debut_X,Debut_Y,Pinceau_X,Pinceau_Y,Couleur);

    Operation_PUSH(Couleur);
    Operation_PUSH(Debut_X);
    Operation_PUSH(Debut_Y);

    Afficher_curseur();
  }

  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Operation_PUSH(Clic);
}


/////////////////////////////////////////////////// OPERATION_FILLED_POLYFORM


void Filled_polyform_12_0(void)
//  Op�ration   : OPERATION_FILLED_POLYFORM
//  Click Souris: 1 ou 2
//  Taille_Pile : 0
//
//  Souris effac�e: Oui
{
  short Couleur;

  Initialiser_debut_operation();

  // Cette op�ration �tant �galement utilis�e pour le lasso, on ne fait pas de
  // backup si on prend une brosse au lasso avec le bouton gauche.
  if ((Operation_en_cours==OPERATION_FILLED_POLYFORM) || (Mouse_K==A_DROITE))
    Backup();

  Shade_Table=(Mouse_K==A_GAUCHE)?Shade_Table_gauche:Shade_Table_droite;
  Cacher_pinceau=1;

  Couleur=(Mouse_K==A_GAUCHE)?Fore_color:Back_color;

  Polyfill_Table_de_points=(short *) malloc((Config.Nb_max_de_vertex_par_polygon<<1)*sizeof(short));
  Polyfill_Table_de_points[0]=Pinceau_X;
  Polyfill_Table_de_points[1]=Pinceau_Y;
  Polyfill_Nombre_de_points=1;

  // On place temporairement le d�but de la ligne qui ne s'afficherait pas sinon
  Pixel_figure_Preview_xor(Pinceau_X,Pinceau_Y,0);

  Operation_PUSH(Pinceau_X); // X Initial
  Operation_PUSH(Pinceau_Y); // X Initial
  Operation_PUSH(Couleur);   // Couleur de remplissage
  Operation_PUSH(Pinceau_X); // Debut X
  Operation_PUSH(Pinceau_Y); // Debut Y
  Operation_PUSH(Pinceau_X); // Fin X
  Operation_PUSH(Pinceau_Y); // Fin Y
  Operation_PUSH(Mouse_K);   // Clic
}


void Filled_polyform_12_8(void)
//  Op�ration   : OPERATION_FILLED_POLYFORM
//  Click Souris: 1 ou 2
//  Taille_Pile : 8
//
//  Souris effac�e: Non
{
  short Clic;
  short Fin_Y;
  short Fin_X;
  short Debut_Y;
  short Debut_X;
  short Couleur;
  short Initial_Y;
  short Initial_X;

  Operation_POP(&Clic);
  Operation_POP(&Fin_Y);
  Operation_POP(&Fin_X);
  Operation_POP(&Debut_Y);
  Operation_POP(&Debut_X);

  if (Clic==Mouse_K)
  {
    // L'utilisateur clique toujours avec le bon bouton de souris

    if (((Debut_X!=Pinceau_X) || (Debut_Y!=Pinceau_Y)) &&
        (Polyfill_Nombre_de_points<Config.Nb_max_de_vertex_par_polygon))
    {
      // Il existe un nouveau segment d�fini par
      // (Debut_X,Debut_Y)-(Pinceau_X,Pinceau_Y)

      Effacer_curseur();
      Print_coordonnees();

      // On le place � l'�cran
      Tracer_ligne_Preview_xor(Debut_X,Debut_Y,Fin_X,Fin_Y,0);
      Tracer_ligne_Preview_xor(Debut_X,Debut_Y,Pinceau_X,Pinceau_Y,0);

      // On peut le rajouter au polygone

      Polyfill_Table_de_points[Polyfill_Nombre_de_points<<1]    =Pinceau_X;
      Polyfill_Table_de_points[(Polyfill_Nombre_de_points<<1)+1]=Pinceau_Y;
      Polyfill_Nombre_de_points++;

      Operation_PUSH(Pinceau_X); // Nouveau Debut_X
      Operation_PUSH(Pinceau_Y); // Nouveau Debut_Y
      Operation_PUSH(Pinceau_X); // Nouveau Fin_X
      Operation_PUSH(Pinceau_Y); // Nouveau Fin_Y
      Operation_PUSH(Clic);

      Afficher_curseur();
    }
    else
    {
      if (Polyfill_Nombre_de_points==Config.Nb_max_de_vertex_par_polygon)
      {
        // Le curseur bouge alors qu'on ne peut plus stocker de segments ?

        if ((Fin_X!=Pinceau_X) || (Fin_Y!=Pinceau_Y))
        {
          Effacer_curseur();
          Print_coordonnees();

          // On le place � l'�cran
          Tracer_ligne_Preview_xor(Debut_X,Debut_Y,Fin_X,Fin_Y,0);
          Tracer_ligne_Preview_xor(Debut_X,Debut_Y,Pinceau_X,Pinceau_Y,0);
          Afficher_curseur();
        }

        // On remet les m�mes valeurs (comme si on n'avait pas cliqu�):
        Operation_PUSH(Debut_X);
        Operation_PUSH(Debut_Y);
        Operation_PUSH(Pinceau_X);
        Operation_PUSH(Pinceau_Y);
        Operation_PUSH(Clic);
      }
      else
      {
        Operation_PUSH(Pinceau_X); // Nouveau Debut_X
        Operation_PUSH(Pinceau_Y); // Nouveau Debut_Y
        Operation_PUSH(Pinceau_X); // Nouveau Fin_X
        Operation_PUSH(Pinceau_Y); // Nouveau Fin_Y
        Operation_PUSH(Clic);
      }
    }
  }
  else
  {
    // L'utilisateur souhaite arr�ter l'op�ration et refermer le polygone

    Operation_POP(&Couleur);
    Operation_POP(&Initial_Y);
    Operation_POP(&Initial_X);

    Attendre_fin_de_click();
    Effacer_curseur();
    Print_coordonnees();

    // Pas besoin d'effacer la ligne (Debut_X,Debut_Y)-(Fin_X,Fin_Y)
    // puisque on les effaces toutes d'un coup.

    Afficher_ecran();
    Polyfill(Polyfill_Nombre_de_points,Polyfill_Table_de_points,Couleur);
    free(Polyfill_Table_de_points);

    Cacher_pinceau=0;

    Afficher_curseur();
  }
}


void Filled_polyform_0_8(void)
//  Op�ration   : OPERATION_FILLED_POLYFORM
//  Click Souris: 0
//  Taille_Pile : 8
//
//  Souris effac�e: Non
{
  short Clic;
  short Fin_Y;
  short Fin_X;
  short Debut_Y;
  short Debut_X;

  Operation_POP(&Clic);
  Operation_POP(&Fin_Y);
  Operation_POP(&Fin_X);

  if ((Fin_X!=Pinceau_X) || (Fin_Y!=Pinceau_Y))
  {
    Effacer_curseur();
    Print_coordonnees();

    Operation_POP(&Debut_Y);
    Operation_POP(&Debut_X);

    // On met � jour l'affichage de la preview du prochain segment:
    Tracer_ligne_Preview_xor(Debut_X,Debut_Y,Fin_X,Fin_Y,0);
    Tracer_ligne_Preview_xor(Debut_X,Debut_Y,Pinceau_X,Pinceau_Y,0);

    Operation_PUSH(Debut_X);
    Operation_PUSH(Debut_Y);

    Afficher_curseur();
  }

  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Operation_PUSH(Clic);
}


////////////////////////////////////////////////////// OPERATION_PRISE_BROSSE


void Brosse_12_0(void)
//
// Op�ration   : OPERATION_PRISE_BROSSE
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
//
{
  Initialiser_debut_operation();
  if (Mouse_K==A_DROITE) // Besoin d'effacer la brosse apr�s ?
  {
    Operation_PUSH(1);
    // Puisque la zone o� on prend la brosse sera effac�e, on fait un backup
    Backup();
  }
  else
    Operation_PUSH(0);

  // On laisse une trace du curseur pour que l'utilisateur puisse visualiser
  // o� demarre sa brosse:
  Afficher_curseur();

  Operation_PUSH(Pinceau_X); // D�but X
  Operation_PUSH(Pinceau_Y); // D�but Y
  Operation_PUSH(Pinceau_X); // Derni�re position X
  Operation_PUSH(Pinceau_Y); // Derni�re position Y

  if ((Config.Coords_rel) && (Menu_visible))
    Print_dans_menu("\035:   1   \022:   1",0);
}


void Brosse_12_5(void)
//
// Op�ration   : OPERATION_PRISE_BROSSE
// Click Souris: 1 ou 2
// Taille_Pile : 5
//
// Souris effac�e: Non
//
{
  char  Chaine[5];
  short Debut_X;
  short Debut_Y;
  short Ancien_X;
  short Ancien_Y;
  short Largeur;
  short Hauteur;

  Operation_POP(&Ancien_Y);
  Operation_POP(&Ancien_X);

  if ( (Menu_visible) && ((Pinceau_X!=Ancien_X) || (Pinceau_Y!=Ancien_Y)) )
  {
    if (Config.Coords_rel)
    {
      Operation_POP(&Debut_Y);
      Operation_POP(&Debut_X);
      Operation_PUSH(Debut_X);
      Operation_PUSH(Debut_Y);

      Largeur=((Debut_X<Pinceau_X)?Pinceau_X-Debut_X:Debut_X-Pinceau_X)+1;
      Hauteur=((Debut_Y<Pinceau_Y)?Pinceau_Y-Debut_Y:Debut_Y-Pinceau_Y)+1;

      Num2str(Largeur,Chaine,4);
      Print_dans_menu(Chaine,2);
      Num2str(Hauteur,Chaine,4);
      Print_dans_menu(Chaine,11);
    }
    else
      Print_coordonnees();
  }

  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


void Brosse_0_5(void)
//
// Op�ration   : OPERATION_PRISE_BROSSE
// Click Souris: 0
// Taille_Pile : 5
//
// Souris effac�e: Oui
//
{
  short Debut_X;
  short Debut_Y;
  short Ancien_Pinceau_X;
  short Ancien_Pinceau_Y;
  short Effacement;

  // Comme on a demand� l'effacement du curseur, il n'y a plus de croix en
  // (Pinceau_X,Pinceau_Y). C'est une bonne chose.

  Operation_Taille_pile-=2;
  Operation_POP(&Debut_Y);
  Operation_POP(&Debut_X);
  Operation_POP(&Effacement);

  // On efface l'ancienne croix:
  Ancien_Pinceau_X=Pinceau_X;
  Ancien_Pinceau_Y=Pinceau_Y;
  Pinceau_X=Debut_X;
  Pinceau_Y=Debut_Y;
  Effacer_curseur(); // Maintenant, il n'y a plus de croix � l'�cran.

  Pinceau_X=Ancien_Pinceau_X;
  Pinceau_Y=Ancien_Pinceau_Y;

  // Prise de la brosse
  if ((Snap_Mode) && (Config.Adjust_brush_pick))
  {
    if (Pinceau_X<Debut_X)
    {
      Ancien_Pinceau_X=Debut_X;
      Debut_X=Pinceau_X;
    }
    if (Pinceau_Y<Debut_Y)
    {
      Ancien_Pinceau_Y=Debut_Y;
      Debut_Y=Pinceau_Y;
    }
    if (Ancien_Pinceau_X!=Debut_X)
      Ancien_Pinceau_X--;
    if (Ancien_Pinceau_Y!=Debut_Y)
      Ancien_Pinceau_Y--;
  }
  Capturer_brosse(Debut_X,Debut_Y,Ancien_Pinceau_X,Ancien_Pinceau_Y,Effacement);
  if ((Snap_Mode) && (Config.Adjust_brush_pick))
  {
    Brosse_Decalage_X=(Brosse_Decalage_X/Snap_Largeur)*Snap_Largeur;
    Brosse_Decalage_Y=(Brosse_Decalage_Y/Snap_Hauteur)*Snap_Hauteur;
  }

  // Simuler l'appui du bouton "Dessin"

  // Comme l'enclenchement du bouton efface le curseur, il faut l'afficher au
  // pr�alable:
  Afficher_curseur();
  // !!! Efface la croix puis affiche le viseur !!!
  Enclencher_bouton(BOUTON_DESSIN,A_GAUCHE); // D�senclenche au passage le bouton brosse
  if (Config.Auto_discontinuous)
  {
    // On se place en mode Dessin discontinu � la main
    while (Operation_en_cours!=OPERATION_DESSIN_DISCONTINU)
      Enclencher_bouton(BOUTON_DESSIN,A_DROITE);
  }
  // Maintenant, il faut r�effacer le curseur parce qu'il sera raffich� en fin
  // d'appel � cette action:
  Effacer_curseur();

  // On passe en brosse couleur:
  Changer_la_forme_du_pinceau(FORME_PINCEAU_BROSSE_COULEUR);

  if ((Config.Coords_rel) && (Menu_visible))
    Print_dans_menu("X:       Y:",0);
  Print_coordonnees();
}


//////////////////////////////////////////////////////// OPERATION_POLYBROSSE


void Polybrosse_12_8(void)
//  Op�ration   : OPERATION_POLYBROSSE
//  Click Souris: 1 ou 2
//  Taille_Pile : 8
//
//  Souris effac�e: Non
{
  short Clic;
  short Fin_Y;
  short Fin_X;
  short Debut_Y;
  short Debut_X;
  short Couleur;
  short Initial_Y;
  short Initial_X;

  Operation_POP(&Clic);
  Operation_POP(&Fin_Y);
  Operation_POP(&Fin_X);
  Operation_POP(&Debut_Y);
  Operation_POP(&Debut_X);

  if (Clic==Mouse_K)
  {
    // L'utilisateur clique toujours avec le bon bouton de souris

    if (((Debut_X!=Pinceau_X) || (Debut_Y!=Pinceau_Y)) &&
        (Polyfill_Nombre_de_points<Config.Nb_max_de_vertex_par_polygon))
    {
      // Il existe un nouveau segment d�fini par
      // (Debut_X,Debut_Y)-(Pinceau_X,Pinceau_Y)

      Effacer_curseur();
      Print_coordonnees();

      // On le place � l'�cran
      Tracer_ligne_Preview_xor(Debut_X,Debut_Y,Fin_X,Fin_Y,0);
      Tracer_ligne_Preview_xor(Debut_X,Debut_Y,Pinceau_X,Pinceau_Y,0);

      // On peut le rajouter au polygone

      Polyfill_Table_de_points[Polyfill_Nombre_de_points<<1]    =Pinceau_X;
      Polyfill_Table_de_points[(Polyfill_Nombre_de_points<<1)+1]=Pinceau_Y;
      Polyfill_Nombre_de_points++;

      Operation_PUSH(Pinceau_X); // Nouveau Debut_X
      Operation_PUSH(Pinceau_Y); // Nouveau Debut_Y
      Operation_PUSH(Pinceau_X); // Nouveau Fin_X
      Operation_PUSH(Pinceau_Y); // Nouveau Fin_Y
      Operation_PUSH(Clic);

      Afficher_curseur();
    }
    else
    {
      if (Polyfill_Nombre_de_points==Config.Nb_max_de_vertex_par_polygon)
      {
        // Le curseur bouge alors qu'on ne peut plus stocker de segments ?

        if ((Fin_X!=Pinceau_X) || (Fin_Y!=Pinceau_Y))
        {
          Effacer_curseur();
          Print_coordonnees();

          // On le place � l'�cran
          Tracer_ligne_Preview_xor(Debut_X,Debut_Y,Fin_X,Fin_Y,0);
          Tracer_ligne_Preview_xor(Debut_X,Debut_Y,Pinceau_X,Pinceau_Y,0);
          Afficher_curseur();
        }

        // On remet les m�mes valeurs (comme si on n'avait pas cliqu�):
        Operation_PUSH(Debut_X);
        Operation_PUSH(Debut_Y);
        Operation_PUSH(Pinceau_X);
        Operation_PUSH(Pinceau_Y);
        Operation_PUSH(Clic);
      }
      else
      {
        Operation_PUSH(Pinceau_X); // Nouveau Debut_X
        Operation_PUSH(Pinceau_Y); // Nouveau Debut_Y
        Operation_PUSH(Pinceau_X); // Nouveau Fin_X
        Operation_PUSH(Pinceau_Y); // Nouveau Fin_Y
        Operation_PUSH(Clic);
      }
    }
  }
  else
  {
    // L'utilisateur souhaite arr�ter l'op�ration et refermer le polygone

    Operation_POP(&Couleur);
    Operation_POP(&Initial_Y);
    Operation_POP(&Initial_X);

    Attendre_fin_de_click();
    Effacer_curseur();
    Print_coordonnees();

    // Pas besoin d'effacer la ligne (Debut_X,Debut_Y)-(Fin_X,Fin_Y)
    // puisqu'on les efface toutes d'un coup.

    Capturer_brosse_au_lasso(Polyfill_Nombre_de_points,Polyfill_Table_de_points,Clic==A_DROITE);
    free(Polyfill_Table_de_points);

    // On raffiche l'�cran pour effacer les traits en xor et pour raffraichir
    // l'�cran si on a d�coup� une partie de l'image en prenant la brosse.
    Afficher_ecran();

    Cacher_pinceau=0;

    if ((Snap_Mode) && (Config.Adjust_brush_pick))
    {
      Brosse_Decalage_X=(Brosse_Decalage_X/Snap_Largeur)*Snap_Largeur;
      Brosse_Decalage_Y=(Brosse_Decalage_Y/Snap_Hauteur)*Snap_Hauteur;
    }

    // Simuler l'appui du bouton "Dessin"

    // Comme l'enclenchement du bouton efface le curseur, il faut l'afficher au
    // pr�alable:
    Afficher_curseur();
    // !!! Efface la croix puis affiche le viseur !!!
    Enclencher_bouton(BOUTON_DESSIN,A_GAUCHE); // D�senclenche au passage le bouton brosse
    if (Config.Auto_discontinuous)
    {
      // On se place en mode Dessin discontinu � la main
      while (Operation_en_cours!=OPERATION_DESSIN_DISCONTINU)
        Enclencher_bouton(BOUTON_DESSIN,A_DROITE);
    }
    // Maintenant, il faut r�effacer le curseur parce qu'il sera raffich� en fin
    // d'appel � cette action:
    Effacer_curseur();

    // On passe en brosse couleur:
    Changer_la_forme_du_pinceau(FORME_PINCEAU_BROSSE_COULEUR);

    Afficher_curseur();
  }
}


///////////////////////////////////////////////////// OPERATION_ETIRER_BROSSE


void Etirer_brosse_12_0(void)
//
// Op�ration   : OPERATION_ETIRER_BROSSE
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
//
{
  Initialiser_debut_operation();
  if (Mouse_K==A_GAUCHE)
  {
    // On laisse une trace du curseur pour que l'utilisateur puisse visualiser
    // o� demarre sa brosse:
    Afficher_curseur();

    Operation_PUSH(Pinceau_X); // Dernier calcul X
    Operation_PUSH(Pinceau_Y); // Dernier calcul Y
    Operation_PUSH(Pinceau_X); // D�but X
    Operation_PUSH(Pinceau_Y); // D�but Y
    Operation_PUSH(Pinceau_X); // Derni�re position X
    Operation_PUSH(Pinceau_Y); // Derni�re position Y
    Operation_PUSH(1); // Etat pr�c�dent

    if ((Config.Coords_rel) && (Menu_visible))
      Print_dans_menu("\035:   1   \022:   1",0);
  }
  else
  {
    Attendre_fin_de_click();
    Demarrer_pile_operation(Operation_avant_interruption);
  }
}



void Etirer_brosse_1_7(void)
//
// Op�ration   : OPERATION_ETIRER_BROSSE
// Click Souris: 1
// Taille_Pile : 7
//
// Souris effac�e: Non
//
{
  char  Chaine[5];
  short Debut_X;
  short Debut_Y;
  short Ancien_X;
  short Ancien_Y;
  short Largeur;
  short Hauteur;
  short Etat_prec;
  short dX,dY,X,Y;

  Operation_POP(&Etat_prec);
  Operation_POP(&Ancien_Y);
  Operation_POP(&Ancien_X);

  if ( (Pinceau_X!=Ancien_X) || (Pinceau_Y!=Ancien_Y) || (Etat_prec!=2) )
  {
    Effacer_curseur();
    Operation_POP(&Debut_Y);
    Operation_POP(&Debut_X);

    if (Menu_visible)
    {
      if (Config.Coords_rel)
      {
        Largeur=((Debut_X<Pinceau_X)?Pinceau_X-Debut_X:Debut_X-Pinceau_X)+1;
        Hauteur=((Debut_Y<Pinceau_Y)?Pinceau_Y-Debut_Y:Debut_Y-Pinceau_Y)+1;

        if (Snap_Mode && Config.Adjust_brush_pick)
        {
          if (Largeur>1) Largeur--;
          if (Hauteur>1) Hauteur--;
        }

        Num2str(Largeur,Chaine,4);
        Print_dans_menu(Chaine,2);
        Num2str(Hauteur,Chaine,4);
        Print_dans_menu(Chaine,11);
      }
      else
        Print_coordonnees();
    }

    Afficher_ecran();

    X=Pinceau_X;
    Y=Pinceau_Y;
    if (Snap_Mode && Config.Adjust_brush_pick)
    {
      dX=Pinceau_X-Debut_X;
      dY=Pinceau_Y-Debut_Y;
      if (dX<0) X++; else {if (dX>0) X--;}
      if (dY<0) Y++; else {if (dY>0) Y--;}
      Etirer_brosse_preview(Debut_X,Debut_Y,X,Y);
    }
    else
      Etirer_brosse_preview(Debut_X,Debut_Y,Pinceau_X,Pinceau_Y);

    Ancien_X=Pinceau_X;
    Ancien_Y=Pinceau_Y;
    Pinceau_X=Debut_X;
    Pinceau_Y=Debut_Y;
    Afficher_curseur();
    Pinceau_X=Ancien_X;
    Pinceau_Y=Ancien_Y;
    Afficher_curseur();

    Operation_Taille_pile-=2;
    Operation_PUSH(X);
    Operation_PUSH(Y);

    Operation_PUSH(Debut_X);
    Operation_PUSH(Debut_Y);
  }

  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Operation_PUSH(2);
}



void Etirer_brosse_0_7(void)
//
// Op�ration   : OPERATION_ETIRER_BROSSE
// Click Souris: 0
// Taille_Pile : 7
//
// Souris effac�e: Non
//
{
  char  Chaine[5];
  short Debut_X;
  short Debut_Y;
  short Ancien_X;
  short Ancien_Y;
  short Largeur;
  short Hauteur;
  byte  Changement_de_taille;
  short Etat_prec;

  Operation_POP(&Etat_prec);
  Operation_POP(&Ancien_Y);
  Operation_POP(&Ancien_X);
  Operation_POP(&Debut_Y);
  Operation_POP(&Debut_X);

  if ((Pinceau_X!=Ancien_X) || (Pinceau_Y!=Ancien_Y) || (Etat_prec!=3))
  {
    if (Menu_visible)
    {
      if (Config.Coords_rel)
      {
        Largeur=((Debut_X<Pinceau_X)?Pinceau_X-Debut_X:Debut_X-Pinceau_X)+1;
        Hauteur=((Debut_Y<Pinceau_Y)?Pinceau_Y-Debut_Y:Debut_Y-Pinceau_Y)+1;

        Num2str(Largeur,Chaine,4);
        Print_dans_menu(Chaine,2);
        Num2str(Hauteur,Chaine,4);
        Print_dans_menu(Chaine,11);
      }
      else
        Print_coordonnees();
    }
  }

  if (Touche)
  {
    Changement_de_taille=1;
    switch (Touche)
    {
      case SDLK_d: // Double
        Largeur=Debut_X+(Brosse_Largeur<<1)-1;
        Hauteur=Debut_Y+(Brosse_Hauteur<<1)-1;
        break;
      case SDLK_x: // Double X
        Largeur=Debut_X+(Brosse_Largeur<<1)-1;
        Hauteur=Debut_Y+Brosse_Hauteur-1;
        break;
      case SDLK_y: // Double Y
        Largeur=Debut_X+Brosse_Largeur-1;
        Hauteur=Debut_Y+(Brosse_Hauteur<<1)-1;
        break;
      case SDLK_h: // Moiti�
        Largeur=(Brosse_Largeur>1)?Debut_X+(Brosse_Largeur>>1)-1:1;
        Hauteur=(Brosse_Hauteur>1)?Debut_Y+(Brosse_Hauteur>>1)-1:1;
        break;
      case SDLK_x|MOD_SHIFT: // Moiti� X
        Largeur=(Brosse_Largeur>1)?Debut_X+(Brosse_Largeur>>1)-1:1;
        Hauteur=Debut_Y+Brosse_Hauteur-1;
        break;
      case SDLK_y|MOD_SHIFT: // Moiti� Y
        Largeur=Debut_X+Brosse_Largeur-1;
        Hauteur=(Brosse_Hauteur>1)?Debut_Y+(Brosse_Hauteur>>1)-1:1;
        break;
      case SDLK_n: // Normal
        Largeur=Debut_X+Brosse_Largeur-1;
        Hauteur=Debut_Y+Brosse_Hauteur-1;
        break;
      default :
        Changement_de_taille=0;
    }
  }
  else
    Changement_de_taille=0;

  if (Changement_de_taille)
  {
    // On efface la preview de la brosse (et la croix)
    Afficher_ecran();

    Ancien_X=Pinceau_X;
    Ancien_Y=Pinceau_Y;
    Pinceau_X=Debut_X;
    Pinceau_Y=Debut_Y;
    Afficher_curseur();
    Pinceau_X=Ancien_X;
    Pinceau_Y=Ancien_Y;

    Etirer_brosse_preview(Debut_X,Debut_Y,Largeur,Hauteur);
    Afficher_curseur();

    Operation_Taille_pile-=2;
    Operation_PUSH(Largeur);
    Operation_PUSH(Hauteur);
  }

  Operation_PUSH(Debut_X);
  Operation_PUSH(Debut_Y);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Operation_PUSH(3);
}


void Etirer_brosse_2_7(void)
//
// Op�ration   : OPERATION_ETIRER_BROSSE
// Click Souris: 2
// Taille_Pile : 7
//
// Souris effac�e: Oui
//
{
  short Calcul_X;
  short Calcul_Y;
  short Debut_X;
  short Debut_Y;


  Operation_Taille_pile-=3;
  Operation_POP(&Debut_Y);
  Operation_POP(&Debut_X);
  Operation_POP(&Calcul_Y);
  Operation_POP(&Calcul_X);

  // On efface la preview de la brosse (et la croix)
  Afficher_ecran();

  // Et enfin on stocke pour de bon la nouvelle brosse �tir�e
  Etirer_brosse(Debut_X,Debut_Y,Calcul_X,Calcul_Y);

  // Simuler l'appui du bouton "Dessin"

  // Comme l'enclenchement du bouton efface le curseur, il faut l'afficher au
  // pr�alable:
  Afficher_curseur();
  // !!! Efface la croix puis affiche le viseur !!!
  Enclencher_bouton(BOUTON_DESSIN,A_GAUCHE); // D�senclenche au passage le bouton brosse
  if (Config.Auto_discontinuous)
  {
    // On se place en mode Dessin discontinu � la main
    while (Operation_en_cours!=OPERATION_DESSIN_DISCONTINU)
      Enclencher_bouton(BOUTON_DESSIN,A_DROITE);
  }
  // Maintenant, il faut r�effacer le curseur parce qu'il sera raffich� en fin
  // d'appel � cette action:
  Effacer_curseur();

  // On passe en brosse couleur:
  Changer_la_forme_du_pinceau(FORME_PINCEAU_BROSSE_COULEUR);

  if ((Config.Coords_rel) && (Menu_visible))
    Print_dans_menu("X:       Y:",0);
  Print_coordonnees();

  // Inutile de de faire un Attendre_fin_de_click car c'est fait dans Enclencher_bouton
}


//////////////////////////////////////////////////// OPERATION_TOURNER_BROSSE


void Tourner_brosse_12_0(void)
//
// Op�ration   : OPERATION_TOURNER_BROSSE
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
//
{
  Initialiser_debut_operation();
  if (Mouse_K==A_GAUCHE)
  {
    Brosse_Centre_rotation_X=Pinceau_X+(Brosse_Largeur>>1)-Brosse_Largeur;
    Brosse_Centre_rotation_Y=Pinceau_Y;
    Brosse_Centre_rotation_defini=1;
    Operation_PUSH(Pinceau_X); // Derni�re position calcul�e X
    Operation_PUSH(Pinceau_Y); // Derni�re position calcul�e Y
    Operation_PUSH(Pinceau_X); // Derni�re position X
    Operation_PUSH(Pinceau_Y); // Derni�re position Y
    Operation_PUSH(1); // Etat pr�c�dent

    if ((Config.Coords_rel) && (Menu_visible))
      Print_dans_menu("Angle:   0�    ",0);
  }
  else
  {
    Attendre_fin_de_click();
    Demarrer_pile_operation(Operation_avant_interruption);
  }
}



void Tourner_brosse_1_5(void)
//
// Op�ration   : OPERATION_TOURNER_BROSSE
// Click Souris: 1
// Taille_Pile : 5
//
// Souris effac�e: Non
//
{
  char  Chaine[4];
  short Ancien_X;
  short Ancien_Y;
  short Etat_prec;
  float Angle;
  int dX,dY;

  Operation_POP(&Etat_prec);
  Operation_POP(&Ancien_Y);
  Operation_POP(&Ancien_X);

  if ( (Pinceau_X!=Ancien_X) || (Pinceau_Y!=Ancien_Y) || (Etat_prec!=2) )
  {
    if ( (Brosse_Centre_rotation_X==Pinceau_X)
      && (Brosse_Centre_rotation_Y==Pinceau_Y) )
      Angle=0.0;
    else
    {
      dX=Pinceau_X-Brosse_Centre_rotation_X;
      dY=Pinceau_Y-Brosse_Centre_rotation_Y;
      Angle=acos(((float)dX)/sqrt((dX*dX)+(dY*dY)));
      if (dY>0) Angle=M_2PI-Angle;
    }

    if (Menu_visible)
    {
      if (Config.Coords_rel)
      {
        Num2str((int)(Angle*180.0/M_PI),Chaine,3);
        Print_dans_menu(Chaine,7);
      }
      else
        Print_coordonnees();
    }

    Afficher_ecran();
    Tourner_brosse_preview(Angle);
    Afficher_curseur();

    Operation_Taille_pile-=2;
    Operation_PUSH(Pinceau_X);
    Operation_PUSH(Pinceau_Y);
  }

  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Operation_PUSH(2);
}



void Tourner_brosse_0_5(void)
//
// Op�ration   : OPERATION_TOURNER_BROSSE
// Click Souris: 0
// Taille_Pile : 5
//
// Souris effac�e: Non
//
{
  char  Chaine[4];
  short Ancien_X;
  short Ancien_Y;
  short Calcul_X;
  short Calcul_Y;
  byte  Changement_angle;
  short Etat_prec;
  float Angle;
  int dX,dY;

  Operation_POP(&Etat_prec);
  Operation_POP(&Ancien_Y);
  Operation_POP(&Ancien_X);

  if ((Pinceau_X!=Ancien_X) || (Pinceau_Y!=Ancien_Y) || (Etat_prec!=3))
  {
    if ( (Brosse_Centre_rotation_X==Pinceau_X)
      && (Brosse_Centre_rotation_Y==Pinceau_Y) )
      Angle=0.0;
    else
    {
      dX=Pinceau_X-Brosse_Centre_rotation_X;
      dY=Pinceau_Y-Brosse_Centre_rotation_Y;
      Angle=acos(((float)dX)/sqrt((dX*dX)+(dY*dY)));
      if (dY>0) Angle=M_2PI-Angle;
    }

    if (Menu_visible)
    {
      if (Config.Coords_rel)
      {
        Num2str(Round(Angle*180.0/M_PI),Chaine,3);
        Print_dans_menu(Chaine,7);
      }
      else
        Print_coordonnees();
    }
  }

  if (Touche)
  {
    Changement_angle=1;
    Calcul_X=Brosse_Centre_rotation_X;
    Calcul_Y=Brosse_Centre_rotation_Y;
    switch (Touche)
    {
      case SDLK_6:case SDLK_KP6: Angle=     0.0 ; Calcul_X++;             break;
      case SDLK_9:case SDLK_KP9: Angle=M_PI*0.25; Calcul_X++; Calcul_Y--; break;
      case SDLK_8:case SDLK_KP8: Angle=M_PI*0.5 ;             Calcul_Y--; break;
      case SDLK_7:case SDLK_KP7: Angle=M_PI*0.75; Calcul_X--; Calcul_Y--; break;
      case SDLK_4:case SDLK_KP4: Angle=M_PI     ; Calcul_X--;             break;
      case SDLK_1:case SDLK_KP1: Angle=M_PI*1.25; Calcul_X--; Calcul_Y++; break;
      case SDLK_2:case SDLK_KP2: Angle=M_PI*1.5 ;             Calcul_Y++; break;
      case SDLK_3:case SDLK_KP3: Angle=M_PI*1.75; Calcul_X++; Calcul_Y++; break;
      default :
        Changement_angle=0;
    }
  }
  else
    Changement_angle=0;

  if (Changement_angle)
  {
    // On efface la preview de la brosse
    Afficher_ecran();
    Tourner_brosse_preview(Angle);
    Afficher_curseur();

    Operation_Taille_pile-=2;
    Operation_PUSH(Calcul_X);
    Operation_PUSH(Calcul_Y);
  }

  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Operation_PUSH(3);
}


void Tourner_brosse_2_5(void)
//
// Op�ration   : OPERATION_TOURNER_BROSSE
// Click Souris: 2
// Taille_Pile : 5
//
// Souris effac�e: Oui
//
{
  short Calcul_X;
  short Calcul_Y;
  int dX,dY;
  float Angle;


  // On efface la preview de la brosse
  Afficher_ecran();

  Operation_Taille_pile-=3;
  Operation_POP(&Calcul_Y);
  Operation_POP(&Calcul_X);

  // Calcul de l'angle par rapport � la derni�re position calcul�e
  if ( (Brosse_Centre_rotation_X==Calcul_X)
    && (Brosse_Centre_rotation_Y==Calcul_Y) )
    Angle=0.0;
  else
  {
    dX=Calcul_X-Brosse_Centre_rotation_X;
    dY=Calcul_Y-Brosse_Centre_rotation_Y;
    Angle=acos(((float)dX)/sqrt((dX*dX)+(dY*dY)));
    if (dY>0) Angle=M_2PI-Angle;
  }

  // Et enfin on stocke pour de bon la nouvelle brosse �tir�e
  Tourner_brosse(Angle);

  // Simuler l'appui du bouton "Dessin"

  // Comme l'enclenchement du bouton efface le curseur, il faut l'afficher au
  // pr�alable:
  Afficher_curseur();
  // !!! Efface le curseur de l'op�ration puis affiche le viseur !!!
  Enclencher_bouton(BOUTON_DESSIN,A_GAUCHE); // D�senclenche au passage le bouton brosse
  if (Config.Auto_discontinuous)
  {
    // On se place en mode Dessin discontinu � la main
    while (Operation_en_cours!=OPERATION_DESSIN_DISCONTINU)
      Enclencher_bouton(BOUTON_DESSIN,A_DROITE);
  }
  // Maintenant, il faut r�effacer le curseur parce qu'il sera raffich� en fin
  // d'appel � cette action:
  Effacer_curseur();

  // On passe en brosse couleur:
  Changer_la_forme_du_pinceau(FORME_PINCEAU_BROSSE_COULEUR);

  if ((Config.Coords_rel) && (Menu_visible))
    Print_dans_menu("X:       Y:",0);
  Print_coordonnees();

  // Inutile de de faire un Attendre_fin_de_click car c'est fait dans Enclencher_bouton
}


//////////////////////////////////////////////////////////// OPERATION_SCROLL


byte Cacher_curseur_avant_scroll;

void Scroll_12_0(void)
//
//  Op�ration   : OPERATION_SCROLL
//  Click Souris: 1 ou 2
//  Taille_Pile : 0
//
//  Souris effac�e: Oui
//
{
  Initialiser_debut_operation();
  Backup();
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Cacher_curseur_avant_scroll=Cacher_curseur;
  Cacher_curseur=1;
  if ((Config.Coords_rel) && (Menu_visible))
    Print_dans_menu("X:�   0   Y:�   0",0);
}


void Scroll_12_4(void)
//
//  Op�ration   : OPERATION_SCROLL
//  Click Souris: 1 ou 2
//  Taille_Pile : 4
//
//  Souris effac�e: Non
//
{
  short Centre_X;
  short Centre_Y;
  short Pos_X;
  short Pos_Y;
  short Decalage_X;
  short Decalage_Y;
  //char  Chaine[5];

  Operation_POP(&Pos_Y);
  Operation_POP(&Pos_X);
  Operation_POP(&Centre_Y);
  Operation_POP(&Centre_X);

  if ( (Pinceau_X!=Pos_X) || (Pinceau_Y!=Pos_Y) )
  {
    // L'utilisateur a boug�, il faut scroller l'image

    if (Pinceau_X>=Centre_X)
      Decalage_X=(Pinceau_X-Centre_X)%Principal_Largeur_image;
    else
      Decalage_X=Principal_Largeur_image-((Centre_X-Pinceau_X)%Principal_Largeur_image);

    if (Pinceau_Y>=Centre_Y)
      Decalage_Y=(Pinceau_Y-Centre_Y)%Principal_Hauteur_image;
    else
      Decalage_Y=Principal_Hauteur_image-((Centre_Y-Pinceau_Y)%Principal_Hauteur_image);

    Aff_coords_rel_ou_abs(Centre_X,Centre_Y);

    Scroll_picture(Decalage_X,Decalage_Y);

    Afficher_ecran();
  }

  Operation_PUSH(Centre_X);
  Operation_PUSH(Centre_Y);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}

void Scroll_0_4(void)
//
//  Op�ration   : OPERATION_SCROLL
//  Click Souris: 0
//  Taille_Pile : 4
//
//  Souris effac�e: Oui
//
{
  Operation_Taille_pile-=4;
  Cacher_curseur=Cacher_curseur_avant_scroll;
  if ((Config.Coords_rel) && (Menu_visible))
  {
    Print_dans_menu("X:       Y:             ",0);
    Print_coordonnees();
  }
}


//////////////////////////////////////////////////// OPERATION_CERCLE_DEGRADE


void Cercle_degrade_12_0(void)
//
// Op�ration   : OPERATION_CERCLE_DEGRADE
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
{
  byte Couleur;

  Initialiser_debut_operation();
  Backup();

  Shade_Table=(Mouse_K==A_GAUCHE)?Shade_Table_gauche:Shade_Table_droite;
  Couleur=(Mouse_K==A_GAUCHE)?Fore_color:Back_color;

  Cacher_pinceau_avant_operation=Cacher_pinceau;
  Cacher_pinceau=1;

  Pixel_figure_Preview(Pinceau_X,Pinceau_Y,Couleur);

  if ((Config.Coords_rel) && (Menu_visible))
    Print_dans_menu("Radius:   0    ",0);

  Operation_PUSH(Mouse_K);
  Operation_PUSH(Couleur);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


void Cercle_degrade_12_6(void)
//
// Op�ration   : OPERATION_CERCLE_DEGRADE
// Click Souris: 1 ou 2
// Taille_Pile : 6 (Mouse_K, Couleur, X_Centre, Y_Centre, X_Tangente, Y_Tangente)
//
// Souris effac�e: Non
//
{
  short Tangente_X;
  short Tangente_Y;
  short Centre_X;
  short Centre_Y;
  short Couleur;
  short Rayon;
  char  Chaine[5];

  Operation_POP(&Tangente_Y);
  Operation_POP(&Tangente_X);
  Operation_POP(&Centre_Y);
  Operation_POP(&Centre_X);
  Operation_POP(&Couleur);

  if ( (Tangente_X!=Pinceau_X) || (Tangente_Y!=Pinceau_Y) )
  {
    Effacer_curseur();
    if ((Config.Coords_rel) && (Menu_visible))
    {
      Num2str(Distance(Centre_X,Centre_Y,Pinceau_X,Pinceau_Y),Chaine,4);
      Print_dans_menu(Chaine,7);
    }
    else
      Print_coordonnees();

    Cercle_Limite=((Tangente_X-Centre_X)*(Tangente_X-Centre_X))+
                  ((Tangente_Y-Centre_Y)*(Tangente_Y-Centre_Y));
    Rayon=sqrt(Cercle_Limite);
    Effacer_cercle_vide_Preview(Centre_X,Centre_Y,Rayon);

    Cercle_Limite=((Pinceau_X-Centre_X)*(Pinceau_X-Centre_X))+
                  ((Pinceau_Y-Centre_Y)*(Pinceau_Y-Centre_Y));
    Rayon=sqrt(Cercle_Limite);
    Tracer_cercle_vide_Preview(Centre_X,Centre_Y,Rayon,Couleur);

    Afficher_curseur();
  }

  Operation_PUSH(Couleur);
  Operation_PUSH(Centre_X);
  Operation_PUSH(Centre_Y);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


void Cercle_degrade_0_6(void)
//
// Op�ration   : OPERATION_CERCLE_DEGRADE
// Click Souris: 0
// Taille_Pile : 6 (Mouse_K, Couleur, X_Centre, Y_Centre, X_Tangente, Y_Tangente)
//
// Souris effac�e: Oui
//
{
  short Tangente_X;
  short Tangente_Y;
  short Centre_X;
  short Centre_Y;
  short Couleur;
  short Click;
  short Rayon;

  Operation_POP(&Tangente_Y);
  Operation_POP(&Tangente_X);
  Operation_POP(&Centre_Y);
  Operation_POP(&Centre_X);

  Operation_POP(&Couleur);
  Operation_POP(&Click);

  if (Click==A_GAUCHE)
  {
    Operation_PUSH(Click);
    Operation_PUSH(Couleur);

    Operation_PUSH(Centre_X);
    Operation_PUSH(Centre_Y);
    Operation_PUSH(Tangente_X);
    Operation_PUSH(Tangente_Y);

    Operation_PUSH(Pinceau_X);
    Operation_PUSH(Pinceau_Y);

    // On change la forme du curseur
    Forme_curseur=FORME_CURSEUR_CIBLE_XOR;

    // On affiche une croix XOR au centre du cercle
    Courbe_Tracer_croix(Centre_X,Centre_Y);

    if (Menu_visible)
    {
      if (Config.Coords_rel)
        Print_dans_menu("X:        Y:",0);
      else
        Print_dans_menu("X:       Y:             ",0);
      Aff_coords_rel_ou_abs(Centre_X,Centre_Y);
    }
  }
  else
  {
    Cercle_Limite=((Tangente_X-Centre_X)*(Tangente_X-Centre_X))+
                  ((Tangente_Y-Centre_Y)*(Tangente_Y-Centre_Y));
    Rayon=sqrt(Cercle_Limite);
    Effacer_cercle_vide_Preview(Centre_X,Centre_Y,Rayon);

    Cacher_pinceau=Cacher_pinceau_avant_operation;
    Forme_curseur=FORME_CURSEUR_CIBLE;

    Tracer_cercle_plein(Centre_X,Centre_Y,Rayon,Back_color);

    if ((Config.Coords_rel) && (Menu_visible))
    {
      Print_dans_menu("X:       Y:             ",0);
      Print_coordonnees();
    }
  }
}


void Cercle_degrade_12_8(void)
//
// Op�ration   : OPERATION_CERCLE_DEGRADE
// Click Souris: 0
// Taille_Pile : 8 (Mouse_K, Couleur, X_Centre, Y_Centre, X_Tangente, Y_Tangente, Ancien_X, Ancien_Y)
//
// Souris effac�e: Oui
//
{
  short Tangente_X;
  short Tangente_Y;
  short Centre_X;
  short Centre_Y;
  short Couleur;
  short Ancien_Mouse_K;

  short Rayon;

  Operation_Taille_pile-=2;   // On fait sauter les 2 derniers �lts de la pile
  Operation_POP(&Tangente_Y);
  Operation_POP(&Tangente_X);
  Operation_POP(&Centre_Y);
  Operation_POP(&Centre_X);
  Operation_POP(&Couleur);
  Operation_POP(&Ancien_Mouse_K);

  // On efface la croix XOR au centre du cercle
  Courbe_Tracer_croix(Centre_X,Centre_Y);

  Cercle_Limite=((Tangente_X-Centre_X)*(Tangente_X-Centre_X))+
                ((Tangente_Y-Centre_Y)*(Tangente_Y-Centre_Y));
  Rayon=sqrt(Cercle_Limite);
  Effacer_cercle_vide_Preview(Centre_X,Centre_Y,Rayon);

  Cacher_pinceau=Cacher_pinceau_avant_operation;
  Forme_curseur=FORME_CURSEUR_CIBLE;

  if (Mouse_K==Ancien_Mouse_K)
    Tracer_cercle_degrade(Centre_X,Centre_Y,Rayon,Pinceau_X,Pinceau_Y);

  Attendre_fin_de_click();

  if ((Config.Coords_rel) && (Menu_visible))
  {
    Print_dans_menu("X:       Y:             ",0);
    Print_coordonnees();
  }
}


void Cercle_ou_ellipse_degrade_0_8(void)
//
// Op�ration   : OPERATION_{CERCLE|ELLIPSE}_DEGRADE
// Click Souris: 0
// Taille_Pile : 8
//
// Souris effac�e: Non
//
{
  short Debut_X;
  short Debut_Y;
  short Tangente_X;
  short Tangente_Y;
  short Ancien_X;
  short Ancien_Y;

  Operation_POP(&Ancien_Y);
  Operation_POP(&Ancien_X);

  if ((Pinceau_X!=Ancien_X) || (Pinceau_Y!=Ancien_Y))
  {
    Operation_POP(&Tangente_Y);
    Operation_POP(&Tangente_X);
    Operation_POP(&Debut_Y);
    Operation_POP(&Debut_X);
    Aff_coords_rel_ou_abs(Debut_X,Debut_Y);
    Operation_PUSH(Debut_X);
    Operation_PUSH(Debut_Y);
    Operation_PUSH(Tangente_X);
    Operation_PUSH(Tangente_Y);
  }

  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


////////////////////////////////////////////////// OPERATION_ELLIPSE_DEGRADEE


void Ellipse_degradee_12_0(void)
//
// Op�ration   : OPERATION_ELLIPSE_DEGRADEE
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
// Souris effac�e: Oui
{
  byte Couleur;

  Initialiser_debut_operation();
  Backup();

  Shade_Table=(Mouse_K==A_GAUCHE)?Shade_Table_gauche:Shade_Table_droite;
  Couleur=(Mouse_K==A_GAUCHE)?Fore_color:Back_color;

  Cacher_pinceau_avant_operation=Cacher_pinceau;
  Cacher_pinceau=1;

  Pixel_figure_Preview(Pinceau_X,Pinceau_Y,Couleur);

  if ((Config.Coords_rel) && (Menu_visible))
    Print_dans_menu("X:�   0   Y:�   0",0);

  Operation_PUSH(Mouse_K);
  Operation_PUSH(Couleur);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


void Ellipse_degradee_12_6(void)
//
// Op�ration   : OPERATION_ELLIPSE_DEGRADEE
// Click Souris: 1 ou 2
// Taille_Pile : 6 (Mouse_K, Couleur, X_Centre, Y_Centre, X_Tangente, Y_Tangente)
//
// Souris effac�e: Non
//
{
  short Tangente_X;
  short Tangente_Y;
  short Centre_X;
  short Centre_Y;
  short Couleur;
  short Rayon_horizontal;
  short Rayon_vertical;

  Operation_POP(&Tangente_Y);
  Operation_POP(&Tangente_X);
  Operation_POP(&Centre_Y);
  Operation_POP(&Centre_X);
  Operation_POP(&Couleur);

  if ( (Tangente_X!=Pinceau_X) || (Tangente_Y!=Pinceau_Y) )
  {
    Effacer_curseur();
    Aff_coords_rel_ou_abs(Centre_X,Centre_Y);

    Rayon_horizontal=(Tangente_X>Centre_X)?Tangente_X-Centre_X
                                           :Centre_X-Tangente_X;
    Rayon_vertical  =(Tangente_Y>Centre_Y)?Tangente_Y-Centre_Y
                                           :Centre_Y-Tangente_Y;
    Effacer_ellipse_vide_Preview(Centre_X,Centre_Y,Rayon_horizontal,Rayon_vertical);

    Rayon_horizontal=(Pinceau_X>Centre_X)?Pinceau_X-Centre_X
                                         :Centre_X-Pinceau_X;
    Rayon_vertical  =(Pinceau_Y>Centre_Y)?Pinceau_Y-Centre_Y
                                         :Centre_Y-Pinceau_Y;
    Tracer_ellipse_vide_Preview(Centre_X,Centre_Y,Rayon_horizontal,Rayon_vertical,Couleur);

    Afficher_curseur();
  }

  Operation_PUSH(Couleur);
  Operation_PUSH(Centre_X);
  Operation_PUSH(Centre_Y);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


void Ellipse_degradee_0_6(void)
//
// Op�ration   : OPERATION_ELLIPSE_DEGRADEE
// Click Souris: 0
// Taille_Pile : 6 (Mouse_K, Couleur, X_Centre, Y_Centre, X_Tangente, Y_Tangente)
//
// Souris effac�e: Oui
//
{
  short Tangente_X;
  short Tangente_Y;
  short Centre_X;
  short Centre_Y;
  short Couleur;
  short Click;
  //short Rayon;
  short Rayon_horizontal;
  short Rayon_vertical;

  Operation_POP(&Tangente_Y);
  Operation_POP(&Tangente_X);
  Operation_POP(&Centre_Y);
  Operation_POP(&Centre_X);

  Operation_POP(&Couleur);
  Operation_POP(&Click);

  if (Click==A_GAUCHE)
  {
    Operation_PUSH(Click);
    Operation_PUSH(Couleur);

    Operation_PUSH(Centre_X);
    Operation_PUSH(Centre_Y);
    Operation_PUSH(Tangente_X);
    Operation_PUSH(Tangente_Y);

    Operation_PUSH(Pinceau_X);
    Operation_PUSH(Pinceau_Y);

    // On change la forme du curseur
    Forme_curseur=FORME_CURSEUR_CIBLE_XOR;

    // On affiche une croix XOR au centre du cercle
    Courbe_Tracer_croix(Centre_X,Centre_Y);

    if (Menu_visible)
    {
      if (Config.Coords_rel)
        Print_dans_menu("X:        Y:",0);
      else
        Print_dans_menu("X:       Y:             ",0);
      Aff_coords_rel_ou_abs(Centre_X,Centre_Y);
    }
  }
  else
  {
    Rayon_horizontal=(Tangente_X>Centre_X)?Tangente_X-Centre_X
                                           :Centre_X-Tangente_X;
    Rayon_vertical  =(Tangente_Y>Centre_Y)?Tangente_Y-Centre_Y
                                           :Centre_Y-Tangente_Y;
    Effacer_ellipse_vide_Preview(Centre_X,Centre_Y,Rayon_horizontal,Rayon_vertical);

    Cacher_pinceau=Cacher_pinceau_avant_operation;
    Forme_curseur=FORME_CURSEUR_CIBLE;

    Tracer_ellipse_pleine(Centre_X,Centre_Y,Rayon_horizontal,Rayon_vertical,Back_color);

    if ((Config.Coords_rel) && (Menu_visible))
    {
      Print_dans_menu("X:       Y:             ",0);
      Print_coordonnees();
    }
  }
}


void Ellipse_degradee_12_8(void)
//
// Op�ration   : OPERATION_ELLIPSE_DEGRADEE
// Click Souris: 0
// Taille_Pile : 8 (Mouse_K, Couleur, X_Centre, Y_Centre, X_Tangente, Y_Tangente, Ancien_X, Ancien_Y)
//
// Souris effac�e: Oui
//
{
  short Tangente_X;
  short Tangente_Y;
  short Centre_X;
  short Centre_Y;
  short Couleur;
  short Rayon_horizontal;
  short Rayon_vertical;
  short Ancien_Mouse_K;

  Operation_Taille_pile-=2;   // On fait sauter les 2 derniers �lts de la pile
  Operation_POP(&Tangente_Y);
  Operation_POP(&Tangente_X);
  Operation_POP(&Centre_Y);
  Operation_POP(&Centre_X);
  Operation_POP(&Couleur);
  Operation_POP(&Ancien_Mouse_K);

  // On efface la croix XOR au centre de l'ellipse
  Courbe_Tracer_croix(Centre_X,Centre_Y);

  Rayon_horizontal=(Tangente_X>Centre_X)?Tangente_X-Centre_X
                                         :Centre_X-Tangente_X;
  Rayon_vertical  =(Tangente_Y>Centre_Y)?Tangente_Y-Centre_Y
                                         :Centre_Y-Tangente_Y;
  Effacer_ellipse_vide_Preview(Centre_X,Centre_Y,Rayon_horizontal,Rayon_vertical);

  Cacher_pinceau=Cacher_pinceau_avant_operation;
  Forme_curseur=FORME_CURSEUR_CIBLE;

  if (Mouse_K==Ancien_Mouse_K)
    Tracer_ellipse_degradee(Centre_X,Centre_Y,Rayon_horizontal,Rayon_vertical,Pinceau_X,Pinceau_Y);

  Attendre_fin_de_click();

  if ((Config.Coords_rel) && (Menu_visible))
  {
    Print_dans_menu("X:       Y:             ",0);
    Print_coordonnees();
  }
}


/////////////////////////////////////////////////// OPERATION_LIGNES_CENTREES


void Lignes_centrees_12_0(void)
// Op�ration   : OPERATION_LIGNES_CENTREES
// Click Souris: 1 ou 2
// Taille_Pile : 0
//
//  Souris effac�e: Oui
{
  Initialiser_debut_operation();
  Backup();
  Shade_Table=(Mouse_K==A_GAUCHE)?Shade_Table_gauche:Shade_Table_droite;

  if ((Config.Coords_rel) && (Menu_visible))
    Print_dans_menu("X:�   0   Y:�   0",0);

  Operation_PUSH(Mouse_K);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


void Lignes_centrees_12_3(void)
// Op�ration   : OPERATION_LIGNES_CENTREES
// Click Souris: 1 ou 2
// Taille_Pile : 3
//
// Souris effac�e: Non
{
  short Debut_X;
  short Debut_Y;

  Operation_POP(&Debut_Y);
  Operation_POP(&Debut_X);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


void Lignes_centrees_0_3(void)
// Op�ration   : OPERATION_LIGNES_CENTREES
// Click Souris: 0
// Taille_Pile : 3
//
// Souris effac�e: Oui
{
  short Debut_X;
  short Debut_Y;
  short Bouton;
  short Couleur;

  Operation_POP(&Debut_Y);
  Operation_POP(&Debut_X);
  Operation_POP(&Bouton);

  Couleur=(Bouton==A_GAUCHE)?Fore_color:Back_color;

  Pixel_figure_Preview(Pinceau_X,Pinceau_Y,Couleur);
  Pinceau_Forme_avant_operation=Pinceau_Forme;
  Pinceau_Forme=FORME_PINCEAU_POINT;

  Operation_PUSH(Bouton);
  Operation_PUSH(Pinceau_X); // Nouveau d�but X
  Operation_PUSH(Pinceau_Y); // Nouveau d�but Y
  Operation_PUSH(Pinceau_X); // Nouvelle derni�re fin X
  Operation_PUSH(Pinceau_Y); // Nouvelle derni�re fin Y
  Operation_PUSH(Pinceau_X); // Nouvelle derni�re position X
  Operation_PUSH(Pinceau_Y); // Nouvelle derni�re position Y
}


void Lignes_centrees_12_7(void)
// Op�ration   : OPERATION_LIGNES_CENTREES
// Click Souris: 1 ou 2
// Taille_Pile : 7
//
// Souris effac�e: Non
{
  short Bouton;
  short Debut_X;
  short Debut_Y;
  short Fin_X;
  short Fin_Y;
  short Dernier_X;
  short Dernier_Y;
  short Couleur;

  Operation_POP(&Dernier_Y);
  Operation_POP(&Dernier_X);
  Operation_POP(&Fin_Y);
  Operation_POP(&Fin_X);
  Operation_POP(&Debut_Y);
  Operation_POP(&Debut_X);
  Operation_POP(&Bouton);

  if (Mouse_K==Bouton)
  {
    if ( (Fin_X!=Pinceau_X) || (Fin_Y!=Pinceau_Y) ||
         (Dernier_X!=Pinceau_X) || (Dernier_Y!=Pinceau_Y) )
    {
      Effacer_curseur();

      Couleur=(Bouton==A_GAUCHE)?Fore_color:Back_color;

      Pinceau_Forme=Pinceau_Forme_avant_operation;

      Pixel_figure_Preview  (Debut_X,Debut_Y,Lit_pixel_dans_ecran_courant(Debut_X,Debut_Y));
      Effacer_ligne_Preview (Debut_X,Debut_Y,Dernier_X,Dernier_Y);

      Smear_Debut=1;
      Afficher_pinceau      (Debut_X,Debut_Y,Couleur,0);
      Tracer_ligne_Definitif(Debut_X,Debut_Y,Pinceau_X,Pinceau_Y,Couleur);

      Pinceau_Forme=FORME_PINCEAU_POINT;
      Pixel_figure_Preview(Pinceau_X,Pinceau_Y,Couleur);
      Tracer_ligne_Preview(Debut_X,Debut_Y,Pinceau_X,Pinceau_Y,Couleur);

      Afficher_curseur();
    }

    Operation_PUSH(Bouton);
    Operation_PUSH(Debut_X);
    Operation_PUSH(Debut_Y);
    Operation_PUSH(Pinceau_X);
    Operation_PUSH(Pinceau_Y);
    Operation_PUSH(Pinceau_X);
    Operation_PUSH(Pinceau_Y);
  }
  else
  {
    Effacer_curseur();

    Pinceau_Forme=Pinceau_Forme_avant_operation;

    Pixel_figure_Preview  (Debut_X,Debut_Y,Lit_pixel_dans_ecran_courant(Debut_X,Debut_Y));
    Effacer_ligne_Preview (Debut_X,Debut_Y,Dernier_X,Dernier_Y);

    if ( (Config.Coords_rel) && (Menu_visible) )
    {
      Print_dans_menu("X:       Y:             ",0);
      Print_coordonnees();
    }

    Afficher_curseur();

    Attendre_fin_de_click();
  }
}


void Lignes_centrees_0_7(void)
// Op�ration   : OPERATION_LIGNES_CENTREES
// Click Souris: 0
// Taille_Pile : 7
//
// Souris effac�e: Non
{
  short Bouton;
  short Debut_X;
  short Debut_Y;
  short Fin_X;
  short Fin_Y;
  short Dernier_X;
  short Dernier_Y;
  short Couleur;

  Operation_POP(&Dernier_Y);
  Operation_POP(&Dernier_X);
  Operation_POP(&Fin_Y);
  Operation_POP(&Fin_X);

  if ((Pinceau_X!=Dernier_X) || (Pinceau_Y!=Dernier_Y))
  {
    Effacer_curseur();
    Operation_POP(&Debut_Y);
    Operation_POP(&Debut_X);
    Operation_POP(&Bouton);

    Couleur=(Bouton==A_GAUCHE)?Fore_color:Back_color;

    Aff_coords_rel_ou_abs(Debut_X,Debut_Y);

    Effacer_ligne_Preview(Debut_X,Debut_Y,Dernier_X,Dernier_Y);

    Pixel_figure_Preview(Debut_X,Debut_Y,Couleur);
    Tracer_ligne_Preview(Debut_X,Debut_Y,Pinceau_X,Pinceau_Y,Couleur);

    Operation_PUSH(Bouton);
    Operation_PUSH(Debut_X);
    Operation_PUSH(Debut_Y);
    Afficher_curseur();
  }

  Operation_PUSH(Fin_X);
  Operation_PUSH(Fin_Y);
  Operation_PUSH(Pinceau_X);
  Operation_PUSH(Pinceau_Y);
}


