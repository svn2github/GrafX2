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



********************************************************************************



    Drawing functions and effects.



*/



#include <string.h>

#include <stdlib.h>

#include <math.h>



#include "global.h"

#include "struct.h"

#include "moteur.h"

#include "boutons.h"

#include "pages.h"

#include "erreurs.h"

#include "sdlscreen.h"

#include "graph.h"

#include "divers.h"

#include "pxsimple.h"

#include "pxtall.h"

#include "pxwide.h"

#include "windows.h"



// Fonction qui met � jour la zone de l'image donn�e en param�tre sur l'�cran.

// Tient compte du d�calage X et Y et du zoom, et fait tous les controles n�cessaires

void Mettre_Ecran_A_Jour(short X, short Y, short Largeur, short Hauteur)

{

  short L_effectif, H_effectif;

  short X_effectif;

  short Y_effectif;

  short Diff;



  // Premi�re �tape, si L ou H est n�gatif, on doit remettre la zone � l'endroit

  if (Largeur < 0)

  {

    X += Largeur;

    Largeur = - Largeur;

  }



  if (Hauteur < 0)

  {

    Y += Hauteur;

    Hauteur = - Hauteur;

  }



  // D'abord on met � jour dans la zone �cran normale

  Diff = X-Principal_Decalage_X;

  if (Diff<0)

  {

    L_effectif = Largeur + Diff;

    X_effectif = 0;

  }

  else

  {

    L_effectif = Largeur;

    X_effectif = Diff;

  }

  Diff = Y-Principal_Decalage_Y;

  if (Diff<0)

  {

    H_effectif = Hauteur + Diff;

    Y_effectif = 0;

  }

  else

  {

    H_effectif = Hauteur;

    Y_effectif = Diff;

  }



  // Normalement il ne faudrait pas updater au del� du split quand on est en mode loupe,

  // mais personne ne devrait demander d'update en dehors de cette limite, m�me le fill est contraint

  // a rester dans la zone visible de l'image

  // ...Sauf l'affichage de brosse en preview - yr

  if(Loupe_Mode && X_effectif + L_effectif > Principal_Split)

    L_effectif = Principal_Split - X_effectif;

  else if(X_effectif + L_effectif > Largeur_ecran)

    L_effectif = Largeur_ecran - X_effectif;



  if(Y_effectif + H_effectif > Menu_Ordonnee)

    H_effectif = Menu_Ordonnee - Y_effectif;

  /*

  SDL_Rect r;

  r.x=X_effectif;

  r.y=Y_effectif;

  r.h=H_effectif;

  r.w=L_effectif;

  SDL_FillRect(Ecran_SDL,&r,3);

  */

  UpdateRect(X_effectif,Y_effectif,L_effectif,H_effectif);



  // Et ensuite dans la partie zoom�e

  if(Loupe_Mode)

  {

    // Clipping en X

    X_effectif = (X-Loupe_Decalage_X)*Loupe_Facteur;

    Y_effectif = (Y-Loupe_Decalage_Y)*Loupe_Facteur;

    L_effectif = Largeur * Loupe_Facteur;

    H_effectif = Hauteur * Loupe_Facteur;



    if (X_effectif < 0)

    {

      L_effectif+=X_effectif;

      if (L_effectif<0)

        return;



      X_effectif = Principal_Split + LARGEUR_BARRE_SPLIT*Menu_Facteur_X;

    }

    else

      X_effectif += Principal_Split + LARGEUR_BARRE_SPLIT*Menu_Facteur_X;

    Diff = X_effectif+L_effectif-Largeur_ecran;

    if (Diff>0)

    {

      L_effectif -=Diff;

      if (L_effectif<0)

        return;

    }





    // Clipping en Y

    if (Y_effectif < 0)

    {

      H_effectif+=Y_effectif;

      if (H_effectif<0)

        return;

      Y_effectif = 0;

    }

    Diff = Y_effectif+H_effectif-Menu_Ordonnee;

    if (Diff>0)

    {

      H_effectif -=Diff;

      if (H_effectif<0)

        return;

    }



 // Tr�s utile pour le debug :)

    /*SDL_Rect r;

    r.x=X_effectif;

    r.y=Y_effectif;

    r.h=H_effectif;

    r.w=L_effectif;

    SDL_FillRect(Ecran_SDL,&r,3);*/



    UpdateRect(X_effectif,Y_effectif,L_effectif,H_effectif);

  }

}







void Transformer_point(short X, short Y, float cosA, float sinA,

                       short * Xr, short * Yr)

{

  *Xr=Round(((float)X*cosA)+((float)Y*sinA));

  *Yr=Round(((float)Y*cosA)-((float)X*sinA));

}







//--------------------- Initialisation d'un mode vid�o -----------------------



void Initialiser_mode_video(int Largeur, int Hauteur, int Fullscreen)

{

  int Sensibilite_X;

  int Sensibilite_Y;

  int Indice;

  int Facteur;



  if (Largeur_ecran!=Largeur ||

      Hauteur_ecran!=Hauteur ||

      Mode_video[Resolution_actuelle].Fullscreen != Fullscreen)

  {

    switch (Pixel_ratio)

    {

        case PIXEL_SIMPLE:

            Pixel_width=1;

            Pixel_height=1;

            Pixel = Pixel_Simple ;

            Lit_pixel= Lit_Pixel_Simple ;

            Display_screen = Afficher_partie_de_l_ecran_Simple ;

            Block = Block_Simple ;

            Pixel_Preview_Normal = Pixel_Preview_Normal_Simple ;

            Pixel_Preview_Loupe = Pixel_Preview_Loupe_Simple ;

            Ligne_horizontale_XOR = Ligne_horizontale_XOR_Simple ;

            Ligne_verticale_XOR = Ligne_verticale_XOR_Simple ;

            Display_brush_Color = Display_brush_Color_Simple ;

            Display_brush_Mono = Display_brush_Mono_Simple ;

            Clear_brush = Clear_brush_Simple ;

            Remap_screen = Remap_screen_Simple ;

            Afficher_ligne = Afficher_une_ligne_ecran_Simple ;

            Afficher_ligne_fast = Afficher_une_ligne_ecran_Simple ;

            Lire_ligne = Lire_une_ligne_ecran_Simple ;

            Display_zoomed_screen = Afficher_partie_de_l_ecran_zoomee_Simple ;

            Display_brush_Color_zoom = Display_brush_Color_zoom_Simple ;

            Display_brush_Mono_zoom = Display_brush_Mono_zoom_Simple ;

            Clear_brush_zoom = Clear_brush_zoom_Simple ;

            Affiche_brosse = Affiche_brosse_Simple ;

        break;

        case PIXEL_TALL:

            Pixel_width=1;

            Pixel_height=2;

            Pixel = Pixel_Tall;

            Lit_pixel= Lit_Pixel_Tall;

            Display_screen = Afficher_partie_de_l_ecran_Tall;

            Block = Block_Tall;

            Pixel_Preview_Normal = Pixel_Preview_Normal_Tall;

            Pixel_Preview_Loupe = Pixel_Preview_Loupe_Tall;

            Ligne_horizontale_XOR = Ligne_horizontale_XOR_Tall;

            Ligne_verticale_XOR = Ligne_verticale_XOR_Tall;

            Display_brush_Color = Display_brush_Color_Tall;

            Display_brush_Mono = Display_brush_Mono_Tall;

            Clear_brush = Clear_brush_Tall;

            Remap_screen = Remap_screen_Tall;

            Afficher_ligne = Afficher_une_ligne_ecran_Tall;

            Afficher_ligne_fast = Afficher_une_ligne_ecran_Tall;

            Lire_ligne = Lire_une_ligne_ecran_Tall;

            Display_zoomed_screen = Afficher_partie_de_l_ecran_zoomee_Tall;

            Display_brush_Color_zoom = Display_brush_Color_zoom_Tall;

            Display_brush_Mono_zoom = Display_brush_Mono_zoom_Tall;

            Clear_brush_zoom = Clear_brush_zoom_Tall;

            Affiche_brosse = Affiche_brosse_Tall;

        break;

        case PIXEL_WIDE:

            Pixel_width=2;

            Pixel_height=1;

            Pixel = Pixel_Wide ;

            Lit_pixel= Lit_Pixel_Wide ;

            Display_screen = Afficher_partie_de_l_ecran_Wide ;

            Block = Block_Wide ;

            Pixel_Preview_Normal = Pixel_Preview_Normal_Wide ;

            Pixel_Preview_Loupe = Pixel_Preview_Loupe_Wide ;

            Ligne_horizontale_XOR = Ligne_horizontale_XOR_Wide ;

            Ligne_verticale_XOR = Ligne_verticale_XOR_Wide ;

            Display_brush_Color = Display_brush_Color_Wide ;

            Display_brush_Mono = Display_brush_Mono_Wide ;

            Clear_brush = Clear_brush_Wide ;

            Remap_screen = Remap_screen_Wide ;

            Afficher_ligne = Afficher_une_ligne_ecran_Wide ;

            Afficher_ligne_fast = Afficher_une_ligne_ecran_fast_Wide ;

            Lire_ligne = Lire_une_ligne_ecran_Wide ;

            Display_zoomed_screen = Afficher_partie_de_l_ecran_zoomee_Wide ;

            Display_brush_Color_zoom = Display_brush_Color_zoom_Wide ;

            Display_brush_Mono_zoom = Display_brush_Mono_zoom_Wide ;

            Clear_brush_zoom = Clear_brush_zoom_Wide ;

            Affiche_brosse = Affiche_brosse_Wide ;

        break;

    }

    // Valeurs raisonnables: minimum 320x200

    if (Pixel_width==1 && Pixel_height==1)

    {

      if (Largeur < 320)

        Largeur = 320;

      if (Hauteur < 200)

        Hauteur = 200;

    }

    else

    {

      if (Largeur < 640)

        Largeur = 640;

      if (Hauteur < 400)

        Hauteur = 400;

    }

    // La largeur doit �tre un multiple de 4
#ifdef __amigaos4__
    Largeur = (Largeur + 15) & 0xFFFFFFF0;
#else
    Largeur = (Largeur + 3 ) & 0xFFFFFFFC;
#endif
    Set_Mode_SDL(&Largeur, &Hauteur,Fullscreen);

    Largeur_ecran = Largeur/Pixel_width;

    Hauteur_ecran = Hauteur/Pixel_height;



    // Taille des menus

    if (Largeur_ecran/320 > Hauteur_ecran/200)

      Facteur=Hauteur_ecran/200;

    else

      Facteur=Largeur_ecran/320;



    switch (Config.Ratio)

    {

      case 1: // adapter tout

        Menu_Facteur_X=Facteur;

        Menu_Facteur_Y=Facteur;

        break;

      case 2: // adapter l�g�rement

        Menu_Facteur_X=Facteur-1;

        if (Menu_Facteur_X<1) Menu_Facteur_X=1;

        Menu_Facteur_Y=Facteur-1;

        if (Menu_Facteur_Y<1) Menu_Facteur_Y=1;

        break;

      default: // ne pas adapter

        Menu_Facteur_X=1;

        Menu_Facteur_Y=1;

    }

    if (Pixel_height>Pixel_width)

      Menu_Facteur_X*=2;

    else if (Pixel_width>Pixel_height)

      Menu_Facteur_Y*=2;

    if (Buffer_de_ligne_horizontale)

      free(Buffer_de_ligne_horizontale);



    Buffer_de_ligne_horizontale=(byte *)malloc(Pixel_width*((Largeur_ecran>Principal_Largeur_image)?Largeur_ecran:Principal_Largeur_image));



    Set_palette(Principal_Palette);



    if (!Fullscreen)

      Resolution_actuelle=0;

    else

      for (Indice=1; Indice<Nb_modes_video; Indice++)

      {

        if (Mode_video[Indice].Largeur==Largeur_ecran &&

            Mode_video[Indice].Hauteur==Hauteur_ecran)

        {

          Resolution_actuelle=Indice;

          break;

        }

      }



    Menu_Taille_couleur = ((Largeur_ecran/Menu_Facteur_X)-(LARGEUR_MENU+2)) >> 3;

    Menu_Ordonnee = Hauteur_ecran;

    if (Menu_visible)

      Menu_Ordonnee -= HAUTEUR_MENU * Menu_Facteur_Y;

    Menu_Ordonnee_Texte = Hauteur_ecran-(Menu_Facteur_Y<<3);

    Bouton[BOUTON_CHOIX_COL].Largeur=(Menu_Taille_couleur<<3)-1;



    Sensibilite_X = Config.Indice_Sensibilite_souris_X;

    Sensibilite_Y = Config.Indice_Sensibilite_souris_Y;

    Sensibilite_X>>=Mouse_Facteur_de_correction_X;

    Sensibilite_Y>>=Mouse_Facteur_de_correction_Y;

    Sensibilite_souris(Sensibilite_X?Sensibilite_X:1,Sensibilite_Y?Sensibilite_Y:1);



    Brouillon_Decalage_X=0; // |  Il faut penser � �viter les incoh�rences

    Brouillon_Decalage_Y=0; // |- de d�calage du brouillon par rapport �

    Brouillon_Loupe_Mode=0; // |  la r�solution.

  }

  if (Loupe_Mode)

  {

    Pixel_Preview=Pixel_Preview_Loupe;

  }

  else

  {

    Pixel_Preview=Pixel_Preview_Normal;

    // Recaler la vue (meme clipping que dans Scroller_ecran())

    if (Principal_Decalage_X+Largeur_ecran>Principal_Largeur_image)

      Principal_Decalage_X=Principal_Largeur_image-Largeur_ecran;

    if (Principal_Decalage_X<0)

      Principal_Decalage_X=0;

    if (Principal_Decalage_Y+Menu_Ordonnee>Principal_Hauteur_image)

      Principal_Decalage_Y=Principal_Hauteur_image-Menu_Ordonnee;

    if (Principal_Decalage_Y<0)

      Principal_Decalage_Y=0;

  }



  Calculer_donnees_loupe();

  if (Loupe_Mode)

    Recadrer_ecran_par_rapport_au_zoom();

  Calculer_limites();

  Calculer_coordonnees_pinceau();

  Afficher_ecran();

  

  Resize_Largeur=0;

  Resize_Hauteur=0;

}







  // -- Redimentionner l'image (nettoie l'�cran virtuel) --



void Redimentionner_image(word Largeur_choisie,word Hauteur_choisie)

{

  word Ancienne_largeur=Principal_Largeur_image;

  word Ancienne_hauteur=Principal_Hauteur_image;



  // +-+-+

  // |C| |  A+B+C = Ancienne image

  // +-+A|

  // |B| |    C   = Nouvelle image

  // +-+-+



  if (Backup_avec_nouvelles_dimensions(1,Largeur_choisie,Hauteur_choisie))

  {

    // La nouvelle page a pu �tre allou�e, elle est pour l'instant pleine de

    // 0s. Elle fait Principal_Largeur_image de large.



    // On copie donc maintenant la partie C dans la nouvelle image.

    Copier_une_partie_d_image_dans_une_autre(

      Ecran_backup,0,0,Min(Ancienne_largeur,Principal_Largeur_image),

      Min(Ancienne_hauteur,Principal_Hauteur_image),Ancienne_largeur,

      Principal_Ecran,0,0,Principal_Largeur_image);

  }

  else

  {

    // Afficher un message d'erreur

    Afficher_curseur();

    Message_Memoire_insuffisante();

    Effacer_curseur();

  }

}







void Remap_picture(void)

{

  short Pos_X; // Variable de balayage de la brosse

  short Pos_Y; // Variable de balayage de la brosse

  byte  Utilisee[256]; // Tableau de bool�ens "La couleur est utilis�e"

  int   Couleur;



  // On commence par initialiser le tableau de bool�ens � faux

  for (Couleur=0;Couleur<=255;Couleur++)

    Utilisee[Couleur]=0;



  // On calcule la table d'utilisation des couleurs

  for (Pos_Y=0;Pos_Y<Brouillon_Hauteur_image;Pos_Y++)

    for (Pos_X=0;Pos_X<Brouillon_Largeur_image;Pos_X++)

      Utilisee[Lit_pixel_dans_ecran_brouillon(Pos_X,Pos_Y)]=1;



  //   On va maintenant se servir de la table "Utilisee" comme table de

  // conversion: pour chaque indice, la table donne une couleur de

  // remplacement.

  // Note : Seules les couleurs utilis�es on besoin d'�tres recalcul�es: les

  //       autres ne seront jamais consult�es dans la nouvelle table de

  //       conversion puisque elles n'existent pas dans l'image, donc elles

  //       ne seront pas utilis�es par Remap_general_LOWLEVEL.

  for (Couleur=0;Couleur<=255;Couleur++)

    if (Utilisee[Couleur])

      Utilisee[Couleur]=Meilleure_couleur(Brouillon_Palette[Couleur].R,Brouillon_Palette[Couleur].V,Brouillon_Palette[Couleur].B);



  //   Maintenant qu'on a une super table de conversion qui n'a que le nom

  // qui craint un peu, on peut faire l'�change dans la brosse de toutes les

  // teintes.

  Remap_general_LOWLEVEL(Utilisee,Brouillon_Ecran,Brouillon_Largeur_image,Brouillon_Hauteur_image,Brouillon_Largeur_image);

}







void Get_colors_from_brush(void)

{

  short Pos_X; // Variable de balayage de la brosse

  short Pos_Y; // Variable de balayage de la brosse

  byte  Utilisee[256]; // Tableau de bool�ens "La couleur est utilis�e"

  int   Couleur;



  if (Demande_de_confirmation("Modify current palette ?"))

  {

    Backup();



    // On commence par initialiser le tableau de bool�en � faux

    for (Couleur=0;Couleur<=255;Couleur++)

      Utilisee[Couleur]=0;



    // On calcule la table d'utilisation des couleurs

    for (Pos_Y=0;Pos_Y<Brosse_Hauteur;Pos_Y++)

      for (Pos_X=0;Pos_X<Brosse_Largeur;Pos_X++)

        Utilisee[Lit_pixel_dans_brosse(Pos_X,Pos_Y)]=1;



    // On recopie dans la palette principale les teintes des couleurs utilis�es

    // dans la palette du brouillon

    for (Couleur=0;Couleur<=255;Couleur++)

      if (Utilisee[Couleur])

      {

        Principal_Palette[Couleur].R=Brouillon_Palette[Couleur].R;

        Principal_Palette[Couleur].V=Brouillon_Palette[Couleur].V;

        Principal_Palette[Couleur].B=Brouillon_Palette[Couleur].B;

      }



    Set_palette(Principal_Palette);

    Calculer_couleurs_menu_optimales(Principal_Palette);

    Effacer_curseur();

    Afficher_ecran();

    Afficher_menu();

    Afficher_curseur();



    Principal_Image_modifiee=1;

  }

}







//////////////////////////////////////////////////////////////////////////////

////////////////////////////// GESTION DU FILLER /////////////////////////////

//////////////////////////////////////////////////////////////////////////////





void Fill(short * Limite_atteinte_Haut  , short * Limite_atteinte_Bas,

          short * Limite_atteinte_Gauche, short * Limite_atteinte_Droite)

//

//   Cette fonction fait un remplissage classique d'une zone d�limit�e de

// l'image. Les limites employ�es sont Limite_Haut, Limite_Bas, Limite_Gauche

// et Limite_Droite. Le point de d�part du remplissage est Pinceau_X,Pinceau_Y

// et s'effectue en th�orie sur la couleur 1 et emploie la couleur 2 pour le

// remplissage. Ces restrictions sont d�es � l'utilisation qu'on en fait dans

// la fonction principale "Remplir", qui se charge de faire une gestion de

// tous les effets.

//   Cette fonction ne doit pas �tre directement appel�e.

//

{

  short Pos_X;   // Abscisse de balayage du segment, utilis�e lors de l'"affichage"

  short Ligne;   // Ordonn�e de la ligne en cours de traitement

  short Debut_X; // Abscisse de d�part du segment trait�

  short Fin_X;   // Abscisse de fin du segment trait�

  int   Modifs_effectuees;    // Bool�en "On a fait une modif dans le dernier passage"

  int   Propagation_possible; // Bool�en "On peut propager la couleur dans le segment"

  short Limite_courante_Bas;  // Intervalle vertical restreint

  short Limite_courante_Haut;

  int   Ligne_modifiee;       // Bool�en "On a fait une modif dans la ligne"



  Modifs_effectuees=1;

  Limite_courante_Haut=Pinceau_Y;

  Limite_courante_Bas =Min(Pinceau_Y+1,Limite_Bas);

  *Limite_atteinte_Gauche=Pinceau_X;

  *Limite_atteinte_Droite=Pinceau_X+1;

  Pixel_dans_ecran_courant(Pinceau_X,Pinceau_Y,2);



  while (Modifs_effectuees)

  {

    Modifs_effectuees=0;



    for (Ligne=Limite_courante_Haut;Ligne<=Limite_courante_Bas;Ligne++)

    {

      Ligne_modifiee=0;

      // On va traiter le cas de la ligne n� Ligne.



      // On commence le traitement � la gauche de l'�cran

      Debut_X=Limite_Gauche;



      // Pour chaque segment de couleur 1 que peut contenir la ligne

      while (Debut_X<=Limite_Droite)

      {

        // On cherche son d�but

        while((Debut_X<=Limite_Droite) &&

                (Lit_pixel_dans_ecran_courant(Debut_X,Ligne)!=1))

             Debut_X++;



        if (Debut_X<=Limite_Droite)

        {

          // Un segment de couleur 1 existe et commence � la position Debut_X.

          // On va donc en chercher la fin.

          for (Fin_X=Debut_X+1;(Fin_X<=Limite_Droite) &&

               (Lit_pixel_dans_ecran_courant(Fin_X,Ligne)==1);Fin_X++);



          //   On sait qu'il existe un segment de couleur 1 qui commence en

          // Debut_X et qui se termine en Fin_X-1.



          //   On va maintenant regarder si une couleur sur la p�riph�rie

          // permet de colorier ce segment avec la couleur 2.



          Propagation_possible=(

            // Test de la pr�sence d'un point � gauche du segment

            ((Debut_X>Limite_Gauche) &&

             (Lit_pixel_dans_ecran_courant(Debut_X-1,Ligne)==2)) ||

            // Test de la pr�sence d'un point � droite du segment

            ((Fin_X-1<Limite_Droite) &&

             (Lit_pixel_dans_ecran_courant(Fin_X    ,Ligne)==2))

                               );



          // Test de la pr�sence d'un point en haut du segment

          if (!Propagation_possible && (Ligne>Limite_Haut))

            for (Pos_X=Debut_X;Pos_X<Fin_X;Pos_X++)

              if (Lit_pixel_dans_ecran_courant(Pos_X,Ligne-1)==2)

              {

                Propagation_possible=1;

                break;

              }



          if (Propagation_possible)

          {

            if (Debut_X<*Limite_atteinte_Gauche)

              *Limite_atteinte_Gauche=Debut_X;

            if (Fin_X>*Limite_atteinte_Droite)

              *Limite_atteinte_Droite=Fin_X;

            // On remplit le segment de Debut_X � Fin_X-1.

            for (Pos_X=Debut_X;Pos_X<Fin_X;Pos_X++)

              Pixel_dans_ecran_courant(Pos_X,Ligne,2);

            // On vient d'effectuer des modifications.

            Modifs_effectuees=1;

            Ligne_modifiee=1;

          }



          Debut_X=Fin_X+1;

        }

      }



      // Si on est en bas, et qu'on peut se propager vers le bas...

      if ( (Ligne==Limite_courante_Bas) &&

           (Ligne_modifiee) &&

           (Limite_courante_Bas<Limite_Bas) )

        Limite_courante_Bas++; // On descend cette limite vers le bas

    }



    // Pour le prochain balayage vers le haut, on va se permettre d'aller

    // voir une ligne plus haut.

    // Si on ne le fait pas, et que la premi�re ligne (Limite_courante_Haut)

    // n'�tait pas modifi�e, alors cette limite ne serait pas remont�e, donc

    // le filler ne progresserait pas vers le haut.

    if (Limite_courante_Haut>Limite_Haut)

      Limite_courante_Haut--;



    for (Ligne=Limite_courante_Bas;Ligne>=Limite_courante_Haut;Ligne--)

    {

      Ligne_modifiee=0;

      // On va traiter le cas de la ligne n� Ligne.



      // On commence le traitement � la gauche de l'�cran

      Debut_X=Limite_Gauche;



      // Pour chaque segment de couleur 1 que peut contenir la ligne

      while (Debut_X<=Limite_Droite)

      {

        // On cherche son d�but

        for (;(Debut_X<=Limite_Droite) &&

             (Lit_pixel_dans_ecran_courant(Debut_X,Ligne)!=1);Debut_X++);



        if (Debut_X<=Limite_Droite)

        {

          // Un segment de couleur 1 existe et commence � la position Debut_X.

          // On va donc en chercher la fin.

          for (Fin_X=Debut_X+1;(Fin_X<=Limite_Droite) &&

               (Lit_pixel_dans_ecran_courant(Fin_X,Ligne)==1);Fin_X++);



          //   On sait qu'il existe un segment de couleur 1 qui commence en

          // Debut_X et qui se termine en Fin_X-1.



          //   On va maintenant regarder si une couleur sur la p�riph�rie

          // permet de colorier ce segment avec la couleur 2.



          Propagation_possible=(

            // Test de la pr�sence d'un point � gauche du segment

            ((Debut_X>Limite_Gauche) &&

             (Lit_pixel_dans_ecran_courant(Debut_X-1,Ligne)==2)) ||

            // Test de la pr�sence d'un point � droite du segment

            ((Fin_X-1<Limite_Droite) &&

             (Lit_pixel_dans_ecran_courant(Fin_X    ,Ligne)==2))

                               );



          // Test de la pr�sence d'un point en bas du segment

          if (!Propagation_possible && (Ligne<Limite_Bas))

            for (Pos_X=Debut_X;Pos_X<Fin_X;Pos_X++)

              if (Lit_pixel_dans_ecran_courant(Pos_X,Ligne+1)==2)

              {

                Propagation_possible=1;

                break;

              }



          if (Propagation_possible)

          {

            if (Debut_X<*Limite_atteinte_Gauche)

              *Limite_atteinte_Gauche=Debut_X;

            if (Fin_X>*Limite_atteinte_Droite)

              *Limite_atteinte_Droite=Fin_X;

            // On remplit le segment de Debut_X � Fin_X-1.

            for (Pos_X=Debut_X;Pos_X<Fin_X;Pos_X++)

              Pixel_dans_ecran_courant(Pos_X,Ligne,2);

            // On vient d'effectuer des modifications.

            Modifs_effectuees=1;

            Ligne_modifiee=1;

          }



          Debut_X=Fin_X+1;

        }

      }



      // Si on est en haut, et qu'on peut se propager vers le haut...

      if ( (Ligne==Limite_courante_Haut) &&

           (Ligne_modifiee) &&

           (Limite_courante_Haut>Limite_Haut) )

        Limite_courante_Haut--; // On monte cette limite vers le haut

    }

  }



  *Limite_atteinte_Haut=Limite_courante_Haut;

  *Limite_atteinte_Bas =Limite_courante_Bas;

  (*Limite_atteinte_Droite)--;

} // Fin de la routine de remplissage "Fill"





void Remplir(byte Couleur_de_remplissage)

//

//  Cette fonction fait un remplissage qui g�re tous les effets. Elle fait

// appel � "Fill()".

//

{

  byte   Forme_curseur_avant_remplissage;

  byte * FX_Feedback_Ecran_avant_remplissage;

  short  Pos_X,Pos_Y;

  short  Limite_atteinte_Haut  ,Limite_atteinte_Bas;

  short  Limite_atteinte_Gauche,Limite_atteinte_Droite;

  byte   Table_de_remplacement[256];





  // Avant toute chose, on v�rifie que l'on n'est pas en train de remplir

  // en dehors de l'image:



  if ( (Pinceau_X>=Limite_Gauche) &&

       (Pinceau_X<=Limite_Droite) &&

       (Pinceau_Y>=Limite_Haut)   &&

       (Pinceau_Y<=Limite_Bas) )

  {

    // On suppose que le curseur est d�j� cach�.

    // Effacer_curseur();



    //   On va faire patienter l'utilisateur en lui affichant un joli petit

    // sablier:

    Forme_curseur_avant_remplissage=Forme_curseur;

    Forme_curseur=FORME_CURSEUR_SABLIER;

    Afficher_curseur();



    // On commence par effectuer un backup de l'image.

    Backup();



    // On fait attention au Feedback qui DOIT se faire avec le backup.

    FX_Feedback_Ecran_avant_remplissage=FX_Feedback_Ecran;

    FX_Feedback_Ecran=Ecran_backup;



    // On va maintenant "�purer" la zone visible de l'image:

    memset(Table_de_remplacement,0,256);

    Table_de_remplacement[Lit_pixel_dans_ecran_courant(Pinceau_X,Pinceau_Y)]=1;

    Remplacer_toutes_les_couleurs_dans_limites(Table_de_remplacement);



    // On fait maintenant un remplissage classique de la couleur 1 avec la 2

   Fill(&Limite_atteinte_Haut  ,&Limite_atteinte_Bas,

         &Limite_atteinte_Gauche,&Limite_atteinte_Droite);



    //  On s'appr�te � faire des op�rations qui n�cessitent un affichage. Il

    // faut donc retirer de l'�cran le curseur:

    Effacer_curseur();

    Forme_curseur=Forme_curseur_avant_remplissage;



    //  Il va maintenant falloir qu'on "turn" ce gros caca "into" un truc qui

    // ressemble un peu plus � ce � quoi l'utilisateur peut s'attendre.

    if (Limite_atteinte_Haut>Limite_Haut)

      Copier_une_partie_d_image_dans_une_autre(Ecran_backup,                    // Source

                                               Limite_Gauche,Limite_Haut,       // Pos X et Y dans source

                                               (Limite_Droite-Limite_Gauche)+1, // Largeur copie

                                               Limite_atteinte_Haut-Limite_Haut,// Hauteur copie

                                               Principal_Largeur_image,         // Largeur de la source

                                               Principal_Ecran,                 // Destination

                                               Limite_Gauche,Limite_Haut,       // Pos X et Y destination

                                               Principal_Largeur_image);        // Largeur destination

    if (Limite_atteinte_Bas<Limite_Bas)

      Copier_une_partie_d_image_dans_une_autre(Ecran_backup,

                                               Limite_Gauche,Limite_atteinte_Bas+1,

                                               (Limite_Droite-Limite_Gauche)+1,

                                               Limite_Bas-Limite_atteinte_Bas,

                                               Principal_Largeur_image,Principal_Ecran,

                                               Limite_Gauche,Limite_atteinte_Bas+1,Principal_Largeur_image);

    if (Limite_atteinte_Gauche>Limite_Gauche)

      Copier_une_partie_d_image_dans_une_autre(Ecran_backup,

                                               Limite_Gauche,Limite_atteinte_Haut,

                                               Limite_atteinte_Gauche-Limite_Gauche,

                                               (Limite_atteinte_Bas-Limite_atteinte_Haut)+1,

                                               Principal_Largeur_image,Principal_Ecran,

                                               Limite_Gauche,Limite_atteinte_Haut,Principal_Largeur_image);

    if (Limite_atteinte_Droite<Limite_Droite)

      Copier_une_partie_d_image_dans_une_autre(Ecran_backup,

                                               Limite_atteinte_Droite+1,Limite_atteinte_Haut,

                                               Limite_Droite-Limite_atteinte_Droite,

                                               (Limite_atteinte_Bas-Limite_atteinte_Haut)+1,

                                               Principal_Largeur_image,Principal_Ecran,

                                               Limite_atteinte_Droite+1,Limite_atteinte_Haut,Principal_Largeur_image);



    for (Pos_Y=Limite_atteinte_Haut;Pos_Y<=Limite_atteinte_Bas;Pos_Y++)

      for (Pos_X=Limite_atteinte_Gauche;Pos_X<=Limite_atteinte_Droite;Pos_X++)

        if (Lit_pixel_dans_ecran_courant(Pos_X,Pos_Y)==2)

        {

          //   Si le pixel en cours de traitement a �t� touch� par le Fill()

          // on se doit d'afficher le pixel modifi� par la couleur de

          // remplissage:



          //  Ceci se fait en commen�ant par restaurer la couleur qu'il y avait

          // pr�c�demment (c'est important pour que les effets ne s'emm�lent

          // pas le pinceaux)

          Pixel_dans_ecran_courant(Pos_X,Pos_Y,Lit_pixel_dans_ecran_backup(Pos_X,Pos_Y));



          //  Enfin, on peut afficher le pixel, en le soumettant aux effets en

          // cours:

          Afficher_pixel(Pos_X,Pos_Y,Couleur_de_remplissage);

        }

        else

          Pixel_dans_ecran_courant(Pos_X,Pos_Y,Lit_pixel_dans_ecran_backup(Pos_X,Pos_Y));



    FX_Feedback_Ecran=FX_Feedback_Ecran_avant_remplissage;



    //   A la fin, on n'a pas besoin de r�afficher le curseur puisque c'est

    // l'appelant qui s'en charge, et on n'a pas besoin de rafficher l'image

    // puisque les seuls points qui ont chang� dans l'image ont �t� raffich�s

    // par l'utilisation de "Afficher_pixel()", et que les autres... eh bein

    // on n'y a jamais touch� � l'�cran les autres: ils sont donc corrects.



    UpdateRect(0,0,0,0);

  }

}







//////////////////////////////////////////////////////////////////////////////

////////////////// TRAC�S DE FIGURES G�OM�TRIQUES STANDARDS //////////////////

////////////////////////// avec gestion de previews //////////////////////////

//////////////////////////////////////////////////////////////////////////////





  // Affichage d'un point de fa�on d�finitive (utilisation du pinceau)

  inline void Pixel_figure_Definitif(word Pos_X,word Pos_Y,byte Couleur)

  {

    Afficher_pinceau(Pos_X,Pos_Y,Couleur,0);

  }



  // Affichage d'un point pour une preview

  void Pixel_figure_Preview(word Pos_X,word Pos_Y,byte Couleur)

  {

    if ( (Pos_X>=Limite_Gauche) &&

         (Pos_X<=Limite_Droite) &&

         (Pos_Y>=Limite_Haut)   &&

         (Pos_Y<=Limite_Bas) )

      Pixel_Preview(Pos_X,Pos_Y,Couleur);

  }

  // Affichage d'un point pour une preview, avec sa propre couleur

  void Pixel_figure_Preview_auto(word Pos_X,word Pos_Y)

  {

    if ( (Pos_X>=Limite_Gauche) &&

         (Pos_X<=Limite_Droite) &&

         (Pos_Y>=Limite_Haut)   &&

         (Pos_Y<=Limite_Bas) )

      Pixel_Preview(Pos_X,Pos_Y,Lit_pixel_dans_ecran_courant(Pos_X,Pos_Y));

  }



  // Affichage d'un point pour une preview en xor

  void Pixel_figure_Preview_xor(word Pos_X,word Pos_Y,__attribute__((unused)) byte Couleur)

  {

    if ( (Pos_X>=Limite_Gauche) &&

         (Pos_X<=Limite_Droite) &&

         (Pos_Y>=Limite_Haut)   &&

         (Pos_Y<=Limite_Bas) )

      Pixel_Preview(Pos_X,Pos_Y,~Lit_pixel(Pos_X-Principal_Decalage_X,

                                           Pos_Y-Principal_Decalage_Y));

  }



  // Effacement d'un point de preview

  void Pixel_figure_Effacer_preview(word Pos_X,word Pos_Y,__attribute__((unused)) byte Couleur)

  {

    if ( (Pos_X>=Limite_Gauche) &&

         (Pos_X<=Limite_Droite) &&

         (Pos_Y>=Limite_Haut)   &&

         (Pos_Y<=Limite_Bas) )

      Pixel_Preview(Pos_X,Pos_Y,Lit_pixel_dans_ecran_courant(Pos_X,Pos_Y));

  }



  // Affichage d'un point dans la brosse

  void Pixel_figure_Dans_brosse(word Pos_X,word Pos_Y,byte Couleur)

  {

    Pos_X-=Brosse_Decalage_X;

    Pos_Y-=Brosse_Decalage_Y;

    if ( (Pos_X<Brosse_Largeur) && // Les pos sont des word donc jamais < 0 ...

         (Pos_Y<Brosse_Hauteur) )

      Pixel_dans_brosse(Pos_X,Pos_Y,Couleur);

  }





  // -- Tracer g�n�ral d'un cercle vide -------------------------------------



void Tracer_cercle_vide_General(short Centre_X,short Centre_Y,short Rayon,byte Couleur)

{

  short Debut_X;

  short Debut_Y;

  short Pos_X;

  short Pos_Y;



  // Ensuite, on va parcourire le quart haut gauche du cercle

  Debut_X=Centre_X-Rayon;

  Debut_Y=Centre_Y-Rayon;



  // Affichage des extremit�es du cercle sur chaque quart du cercle:

  for (Pos_Y=Debut_Y,Cercle_Curseur_Y=-Rayon;Pos_Y<Centre_Y;Pos_Y++,Cercle_Curseur_Y++)

    for (Pos_X=Debut_X,Cercle_Curseur_X=-Rayon;Pos_X<Centre_X;Pos_X++,Cercle_Curseur_X++)

      if (Pixel_dans_cercle())

      {

        // On vient de tomber sur le premier point sur la ligne horizontale

        // qui fait partie du cercle.

        // Donc on peut l'afficher (lui et ses copains sym�triques)



         // Quart Haut-gauche

        Pixel_figure(Pos_X,Pos_Y,Couleur);

         // Quart Haut-droite

        Pixel_figure((Centre_X<<1)-Pos_X,Pos_Y,Couleur);

         // Quart Bas-droite

        Pixel_figure((Centre_X<<1)-Pos_X,(Centre_Y<<1)-Pos_Y,Couleur);

         // Quart Bas-gauche

        Pixel_figure(Pos_X,(Centre_Y<<1)-Pos_Y,Couleur);



        // On peut ensuite afficher tous les points qui le suivent dont le

        // pixel voisin du haut n'appartient pas au cercle:

        for (Cercle_Curseur_Y--,Pos_X++,Cercle_Curseur_X++;Pos_X<Centre_X;Pos_X++,Cercle_Curseur_X++)

          if (!Pixel_dans_cercle())

          {

             // Quart Haut-gauche

            Pixel_figure(Pos_X,Pos_Y,Couleur);

             // Quart Haut-droite

            Pixel_figure((Centre_X<<1)-Pos_X,Pos_Y,Couleur);

             // Quart Bas-gauche

            Pixel_figure(Pos_X,(Centre_Y<<1)-Pos_Y,Couleur);

             // Quart Bas-droite

            Pixel_figure((Centre_X<<1)-Pos_X,(Centre_Y<<1)-Pos_Y,Couleur);

          }

          else

            break;



        Cercle_Curseur_Y++;

        break;

      }



  // On affiche � la fin les points cardinaux:

  Pixel_figure(Centre_X,Centre_Y-Rayon,Couleur); // Haut

  Pixel_figure(Centre_X-Rayon,Centre_Y,Couleur); // Gauche

  Pixel_figure(Centre_X+Rayon,Centre_Y,Couleur); // Droite

  Pixel_figure(Centre_X,Centre_Y+Rayon,Couleur); // Bas



  if(Loupe_Mode) Mettre_Ecran_A_Jour(Centre_X-Rayon,Centre_Y-Rayon,2*Rayon+1,2*Rayon+1);

}



  // -- Trac� d�finitif d'un cercle vide --



void Tracer_cercle_vide_Definitif(short Centre_X,short Centre_Y,short Rayon,byte Couleur)

{

  Pixel_figure=Pixel_figure_Definitif;

  Tracer_cercle_vide_General(Centre_X,Centre_Y,Rayon,Couleur);

  Mettre_Ecran_A_Jour(Centre_X - Rayon, Centre_Y - Rayon, 2* Rayon+1, 2*Rayon+1);

}



  // -- Tracer la preview d'un cercle vide --



void Tracer_cercle_vide_Preview(short Centre_X,short Centre_Y,short Rayon,byte Couleur)

{

  Pixel_figure=Pixel_figure_Preview;

  Tracer_cercle_vide_General(Centre_X,Centre_Y,Rayon,Couleur);

  Mettre_Ecran_A_Jour(Centre_X - Rayon, Centre_Y - Rayon, 2* Rayon+1, 2*Rayon+1);

}



  // -- Effacer la preview d'un cercle vide --



void Effacer_cercle_vide_Preview(short Centre_X,short Centre_Y,short Rayon)

{

  Pixel_figure=Pixel_figure_Effacer_preview;

  Tracer_cercle_vide_General(Centre_X,Centre_Y,Rayon,0);

  Mettre_Ecran_A_Jour(Centre_X - Rayon, Centre_Y - Rayon, 2* Rayon+1, 2*Rayon+1);

}



  // -- Tracer un cercle plein --



void Tracer_cercle_plein(short Centre_X,short Centre_Y,short Rayon,byte Couleur)

{

  short Debut_X;

  short Debut_Y;

  short Pos_X;

  short Pos_Y;

  short Fin_X;

  short Fin_Y;



  Debut_X=Centre_X-Rayon;

  Debut_Y=Centre_Y-Rayon;

  Fin_X=Centre_X+Rayon;

  Fin_Y=Centre_Y+Rayon;



  // Correction des bornes d'apr�s les limites

  if (Debut_Y<Limite_Haut)

    Debut_Y=Limite_Haut;

  if (Fin_Y>Limite_Bas)

    Fin_Y=Limite_Bas;

  if (Debut_X<Limite_Gauche)

    Debut_X=Limite_Gauche;

  if (Fin_X>Limite_Droite)

    Fin_X=Limite_Droite;



  // Affichage du cercle

  for (Pos_Y=Debut_Y,Cercle_Curseur_Y=(long)Debut_Y-Centre_Y;Pos_Y<=Fin_Y;Pos_Y++,Cercle_Curseur_Y++)

    for (Pos_X=Debut_X,Cercle_Curseur_X=(long)Debut_X-Centre_X;Pos_X<=Fin_X;Pos_X++,Cercle_Curseur_X++)

      if (Pixel_dans_cercle())

        Afficher_pixel(Pos_X,Pos_Y,Couleur);



  Mettre_Ecran_A_Jour(Debut_X,Debut_Y,Fin_X+1-Debut_X,Fin_Y+1-Debut_Y);

}





  // -- Tracer g�n�ral d'une ellipse vide -----------------------------------



void Tracer_ellipse_vide_General(short Centre_X,short Centre_Y,short Rayon_horizontal,short Rayon_vertical,byte Couleur)

{

  short Debut_X;

  short Debut_Y;

  short Pos_X;

  short Pos_Y;



  Debut_X=Centre_X-Rayon_horizontal;

  Debut_Y=Centre_Y-Rayon_vertical;



  // Calcul des limites de l'ellipse

  Ellipse_Calculer_limites(Rayon_horizontal+1,Rayon_vertical+1);



  // Affichage des extremit�es de l'ellipse sur chaque quart de l'ellipse:

  for (Pos_Y=Debut_Y,Ellipse_Curseur_Y=-Rayon_vertical;Pos_Y<Centre_Y;Pos_Y++,Ellipse_Curseur_Y++)

    for (Pos_X=Debut_X,Ellipse_Curseur_X=-Rayon_horizontal;Pos_X<Centre_X;Pos_X++,Ellipse_Curseur_X++)

      if (Pixel_dans_ellipse())

      {

        // On vient de tomber sur le premier point qui sur la ligne

        // horizontale fait partie de l'ellipse.



        // Donc on peut l'afficher (lui et ses copains sym�triques)



         // Quart Haut-gauche

        Pixel_figure(Pos_X,Pos_Y,Couleur);

         // Quart Haut-droite

        Pixel_figure((Centre_X<<1)-Pos_X,Pos_Y,Couleur);

         // Quart Bas-gauche

        Pixel_figure(Pos_X,(Centre_Y<<1)-Pos_Y,Couleur);

         // Quart Bas-droite

        Pixel_figure((Centre_X<<1)-Pos_X,(Centre_Y<<1)-Pos_Y,Couleur);



        // On peut ensuite afficher tous les points qui le suivent dont le

        // pixel voisin du haut n'appartient pas � l'ellipse:

        for (Ellipse_Curseur_Y--,Pos_X++,Ellipse_Curseur_X++;Pos_X<Centre_X;Pos_X++,Ellipse_Curseur_X++)

          if (!Pixel_dans_ellipse())

          {

             // Quart Haut-gauche

            Pixel_figure(Pos_X,Pos_Y,Couleur);

             // Quart Haut-droite

            Pixel_figure((Centre_X<<1)-Pos_X,Pos_Y,Couleur);

             // Quart Bas-gauche

            Pixel_figure(Pos_X,(Centre_Y<<1)-Pos_Y,Couleur);

             // Quart Bas-droite

            Pixel_figure((Centre_X<<1)-Pos_X,(Centre_Y<<1)-Pos_Y,Couleur);

          }

          else

            break;



        Ellipse_Curseur_Y++;

        break;

      }



  // On affiche � la fin les points cardinaux:



  // Points verticaux:

  Pos_X=Centre_X;

  Ellipse_Curseur_X=-1;

  for (Pos_Y=Centre_Y+1-Rayon_vertical,Ellipse_Curseur_Y=-Rayon_vertical+1;Pos_Y<Centre_Y+Rayon_vertical;Pos_Y++,Ellipse_Curseur_Y++)

    if (!Pixel_dans_ellipse())

      Pixel_figure(Pos_X,Pos_Y,Couleur);



  // Points horizontaux:

  Pos_Y=Centre_Y;

  Ellipse_Curseur_Y=-1;

  for (Pos_X=Centre_X+1-Rayon_horizontal,Ellipse_Curseur_X=-Rayon_horizontal+1;Pos_X<Centre_X+Rayon_horizontal;Pos_X++,Ellipse_Curseur_X++)

    if (!Pixel_dans_ellipse())

      Pixel_figure(Pos_X,Pos_Y,Couleur);



  Pixel_figure(Centre_X,Centre_Y-Rayon_vertical,Couleur);   // Haut

  Pixel_figure(Centre_X-Rayon_horizontal,Centre_Y,Couleur); // Gauche

  Pixel_figure(Centre_X+Rayon_horizontal,Centre_Y,Couleur); // Droite

  Pixel_figure(Centre_X,Centre_Y+Rayon_vertical,Couleur);   // Bas



  Mettre_Ecran_A_Jour(Centre_X-Rayon_horizontal,Centre_Y-Rayon_vertical,2*Rayon_horizontal+1,2*Rayon_vertical+1);

}



  // -- Trac� d�finitif d'une ellipse vide --



void Tracer_ellipse_vide_Definitif(short Centre_X,short Centre_Y,short Rayon_horizontal,short Rayon_vertical,byte Couleur)

{

  Pixel_figure=Pixel_figure_Definitif;

  Tracer_ellipse_vide_General(Centre_X,Centre_Y,Rayon_horizontal,Rayon_vertical,Couleur);

  Mettre_Ecran_A_Jour(Centre_X - Rayon_horizontal, Centre_Y - Rayon_vertical, 2* Rayon_horizontal+1, 2*Rayon_vertical+1);

}



  // -- Tracer la preview d'une ellipse vide --



void Tracer_ellipse_vide_Preview(short Centre_X,short Centre_Y,short Rayon_horizontal,short Rayon_vertical,byte Couleur)

{

  Pixel_figure=Pixel_figure_Preview;

  Tracer_ellipse_vide_General(Centre_X,Centre_Y,Rayon_horizontal,Rayon_vertical,Couleur);

  Mettre_Ecran_A_Jour(Centre_X - Rayon_horizontal, Centre_Y - Rayon_vertical, 2* Rayon_horizontal+1, 2*Rayon_vertical +1);

}



  // -- Effacer la preview d'une ellipse vide --



void Effacer_ellipse_vide_Preview(short Centre_X,short Centre_Y,short Rayon_horizontal,short Rayon_vertical)

{

  Pixel_figure=Pixel_figure_Effacer_preview;

  Tracer_ellipse_vide_General(Centre_X,Centre_Y,Rayon_horizontal,Rayon_vertical,0);

  Mettre_Ecran_A_Jour(Centre_X - Rayon_horizontal, Centre_Y - Rayon_vertical, 2* Rayon_horizontal+1, 2*Rayon_vertical+1);

}



  // -- Tracer une ellipse pleine --



void Tracer_ellipse_pleine(short Centre_X,short Centre_Y,short Rayon_horizontal,short Rayon_vertical,byte Couleur)

{

  short Debut_X;

  short Debut_Y;

  short Pos_X;

  short Pos_Y;

  short Fin_X;

  short Fin_Y;



  Debut_X=Centre_X-Rayon_horizontal;

  Debut_Y=Centre_Y-Rayon_vertical;

  Fin_X=Centre_X+Rayon_horizontal;

  Fin_Y=Centre_Y+Rayon_vertical;



  // Calcul des limites de l'ellipse

  Ellipse_Calculer_limites(Rayon_horizontal+1,Rayon_vertical+1);



  // Correction des bornes d'apr�s les limites

  if (Debut_Y<Limite_Haut)

    Debut_Y=Limite_Haut;

  if (Fin_Y>Limite_Bas)

    Fin_Y=Limite_Bas;

  if (Debut_X<Limite_Gauche)

    Debut_X=Limite_Gauche;

  if (Fin_X>Limite_Droite)

    Fin_X=Limite_Droite;



  // Affichage de l'ellipse

  for (Pos_Y=Debut_Y,Ellipse_Curseur_Y=Debut_Y-Centre_Y;Pos_Y<=Fin_Y;Pos_Y++,Ellipse_Curseur_Y++)

    for (Pos_X=Debut_X,Ellipse_Curseur_X=Debut_X-Centre_X;Pos_X<=Fin_X;Pos_X++,Ellipse_Curseur_X++)

      if (Pixel_dans_ellipse())

        Afficher_pixel(Pos_X,Pos_Y,Couleur);

  Mettre_Ecran_A_Jour(Centre_X-Rayon_horizontal,Centre_Y-Rayon_vertical,2*Rayon_horizontal+1,2*Rayon_vertical+1);

}



/******************

* TRAC� DE LIGNES *

******************/



void Rectifier_coordonnees_a_45_degres(short AX, short AY, short* BX, short* BY)

// Modifie BX et BY pour que la ligne AXAY - BXBY soit

//  - une droite horizontale

//  - une droite verticale

//  - une droite avec une pente de 45 degr�s

{

    int dx, dy;

    float tan;



    dx = (*BX)-AX;

    dy = AY- *BY; // On prend l'oppos�e car � l'�cran les Y sont positifs en bas, et en maths, positifs en haut



    if (dx==0) return; // On est en lockx et de toutes fa�ons le X n'a pas boug�, on sort tout de suite pour �viter une m�chante division par 0



    tan = (float)dy/(float)dx;



    if (tan <= 0.4142 && tan >= -0.4142)

    {

      // Cas 1 : Lock Y

      *BY = AY;

    }

    else if ( tan > 0.4142 && tan < 2.4142)

    {

      // Cas 2 : dy=dx

      int nBY = AY - dx;

      *BY = (*BY + nBY)/2;

      *BX = AX  + AY - *BY;

    }

    else if (tan < -0.4142 && tan >= -2.4142)

    {

      // Cas 8 : dy = -dx

      int nBY = AY + dx;

      *BY = (*BY + nBY)/2;

      *BX = AX  - AY + *BY;

    }

    else

    {

      // Cas 3 : Lock X

      *BX = AX;

    }



    return;

}



  // -- Tracer g�n�ral d'une ligne ------------------------------------------



void Tracer_ligne_General(short Debut_X,short Debut_Y,short Fin_X,short Fin_Y, byte Couleur)

{

  short Pos_X,Pos_Y;

  short Incr_X,Incr_Y;

  short i,Cumul;

  short Delta_X,Delta_Y;





  Pos_X=Debut_X;

  Pos_Y=Debut_Y;



  if (Debut_X<Fin_X)

  {

    Incr_X=+1;

    Delta_X=Fin_X-Debut_X;

  }

  else

  {

    Incr_X=-1;

    Delta_X=Debut_X-Fin_X;

  }



  if (Debut_Y<Fin_Y)

  {

    Incr_Y=+1;

    Delta_Y=Fin_Y-Debut_Y;

  }

  else

  {

    Incr_Y=-1;

    Delta_Y=Debut_Y-Fin_Y;

  }



  if (Delta_Y>Delta_X)

  {

    Cumul=Delta_Y>>1;

    for (i=1; i<Delta_Y; i++)

    {

      Pos_Y+=Incr_Y;

      Cumul+=Delta_X;

      if (Cumul>=Delta_Y)

      {

        Cumul-=Delta_Y;

        Pos_X+=Incr_X;

      }

      Pixel_figure(Pos_X,Pos_Y,Couleur);

    }

  }

  else

  {

    Cumul=Delta_X>>1;

    for (i=1; i<Delta_X; i++)

    {

      Pos_X+=Incr_X;

      Cumul+=Delta_Y;

      if (Cumul>=Delta_X)

      {

        Cumul-=Delta_X;

        Pos_Y+=Incr_Y;

      }

      Pixel_figure(Pos_X,Pos_Y,Couleur);

    }

  }



  if ( (Debut_X!=Fin_X) || (Debut_Y!=Fin_Y) )

    Pixel_figure(Fin_X,Fin_Y,Couleur);



}



  // -- Tracer d�finitif d'une ligne --



void Tracer_ligne_Definitif(short Debut_X,short Debut_Y,short Fin_X,short Fin_Y, byte Couleur)

{



  int L = Fin_X-Debut_X, H = Fin_Y - Debut_Y;

  Pixel_figure=Pixel_figure_Definitif;

  Tracer_ligne_General(Debut_X,Debut_Y,Fin_X,Fin_Y,Couleur);

  Mettre_Ecran_A_Jour((Debut_X<Fin_X)?Debut_X:Fin_X,(Debut_Y<Fin_Y)?Debut_Y:Fin_Y,abs(L)+1,abs(H)+1);

}



  // -- Tracer la preview d'une ligne --



void Tracer_ligne_Preview(short Debut_X,short Debut_Y,short Fin_X,short Fin_Y,byte Couleur)

{

  int L = Fin_X-Debut_X, H = Fin_Y - Debut_Y;

  Pixel_figure=Pixel_figure_Preview;

  Tracer_ligne_General(Debut_X,Debut_Y,Fin_X,Fin_Y,Couleur);

  Mettre_Ecran_A_Jour((Debut_X<Fin_X)?Debut_X:Fin_X,(Debut_Y<Fin_Y)?Debut_Y:Fin_Y,abs(L)+1,abs(H)+1);

}



  // -- Tracer la preview d'une ligne en xor --



void Tracer_ligne_Preview_xor(short Debut_X,short Debut_Y,short Fin_X,short Fin_Y,byte Couleur)

{

  int L = Fin_X-Debut_X, H = Fin_Y - Debut_Y;

  Pixel_figure=Pixel_figure_Preview_xor;

  Tracer_ligne_General(Debut_X,Debut_Y,Fin_X,Fin_Y,Couleur);

  Mettre_Ecran_A_Jour((Debut_X<Fin_X)?Debut_X:Fin_X,(Debut_Y<Fin_Y)?Debut_Y:Fin_Y,abs(L)+1,abs(H)+1);

}



  // -- Effacer la preview d'une ligne --



void Effacer_ligne_Preview(short Debut_X,short Debut_Y,short Fin_X,short Fin_Y)

{

  int L = Fin_X-Debut_X, H = Fin_Y - Debut_Y;

  Pixel_figure=Pixel_figure_Effacer_preview;

  Tracer_ligne_General(Debut_X,Debut_Y,Fin_X,Fin_Y,0);

  Mettre_Ecran_A_Jour((Debut_X<Fin_X)?Debut_X:Fin_X,(Debut_Y<Fin_Y)?Debut_Y:Fin_Y,abs(L)+1,abs(H)+1);

}





  // -- Tracer un rectangle vide --



void Tracer_rectangle_vide(short Debut_X,short Debut_Y,short Fin_X,short Fin_Y,byte Couleur)

{

  short Tempo;

  short Pos_X;

  short Pos_Y;





  // On v�rifie que les bornes soient dans le bon sens:

  if (Debut_X>Fin_X)

  {

    Tempo=Debut_X;

    Debut_X=Fin_X;

    Fin_X=Tempo;

  }

  if (Debut_Y>Fin_Y)

  {

    Tempo=Debut_Y;

    Debut_Y=Fin_Y;

    Fin_Y=Tempo;

  }



  // On trace le rectangle:



  for (Pos_X=Debut_X;Pos_X<=Fin_X;Pos_X++)

    Afficher_pinceau(Pos_X,Debut_Y,Couleur,0);



  for (Pos_Y=Debut_Y+1;Pos_Y<Fin_Y;Pos_Y++)

  {

    Afficher_pinceau(Debut_X,Pos_Y,Couleur,0);

    Afficher_pinceau(  Fin_X,Pos_Y,Couleur,0);

  }



  for (Pos_X=Debut_X;Pos_X<=Fin_X;Pos_X++)

    Afficher_pinceau(Pos_X,  Fin_Y,Couleur,0);

#ifdef __macosx__

  Mettre_Ecran_A_Jour(Debut_X,Fin_X,Fin_X-Debut_X,Fin_Y-Debut_Y);

#endif

}



  // -- Tracer un rectangle plein --



void Tracer_rectangle_plein(short Debut_X,short Debut_Y,short Fin_X,short Fin_Y,byte Couleur)

{

  short Tempo;

  short Pos_X;

  short Pos_Y;





  // On v�rifie que les bornes sont dans le bon sens:

  if (Debut_X>Fin_X)

  {

    Tempo=Debut_X;

    Debut_X=Fin_X;

    Fin_X=Tempo;

  }

  if (Debut_Y>Fin_Y)

  {

    Tempo=Debut_Y;

    Debut_Y=Fin_Y;

    Fin_Y=Tempo;

  }



  // Correction en cas de d�passement des limites de l'image

  if (Fin_X>Limite_Droite)

    Fin_X=Limite_Droite;

  if (Fin_Y>Limite_Bas)

    Fin_Y=Limite_Bas;



  // On trace le rectangle:

  for (Pos_Y=Debut_Y;Pos_Y<=Fin_Y;Pos_Y++)

    for (Pos_X=Debut_X;Pos_X<=Fin_X;Pos_X++)

      // Afficher_pixel traite chaque pixel avec tous les effets ! (smear, ...)

      // Donc on ne peut pas otimiser en tra�ant ligne par ligne avec memset :(

      Afficher_pixel(Pos_X,Pos_Y,Couleur);

  Mettre_Ecran_A_Jour(Debut_X,Debut_Y,Fin_X-Debut_X,Fin_Y-Debut_Y);



}









  // -- Tracer une courbe de B�zier --



void Tracer_courbe_General(short X1, short Y1,

                           short X2, short Y2,

                           short X3, short Y3,

                           short X4, short Y4,

                           byte Couleur)

{

  float Delta,T,T2,T3;

  short X,Y,Old_X,Old_Y;

  word  i;

  int   CX[4];

  int   CY[4];



  // Calcul des vecteurs de coefficients

  CX[0]= -   X1 + 3*X2 - 3*X3 + X4;

  CX[1]= + 3*X1 - 6*X2 + 3*X3;

  CX[2]= - 3*X1 + 3*X2;

  CX[3]= +   X1;

  CY[0]= -   Y1 + 3*Y2 - 3*Y3 + Y4;

  CY[1]= + 3*Y1 - 6*Y2 + 3*Y3;

  CY[2]= - 3*Y1 + 3*Y2;

  CY[3]= +   Y1;



  // Tra�age de la courbe

  Old_X=X1;

  Old_Y=Y1;

  Pixel_figure(Old_X,Old_Y,Couleur);

  Delta=0.05; // 1.0/20

  T=0;

  for (i=1; i<=20; i++)

  {

    T=T+Delta; T2=T*T; T3=T2*T;

    X=Round(T3*CX[0] + T2*CX[1] + T*CX[2] + CX[3]);

    Y=Round(T3*CY[0] + T2*CY[1] + T*CY[2] + CY[3]);

    Tracer_ligne_General(Old_X,Old_Y,X,Y,Couleur);

    Old_X=X;

    Old_Y=Y;

  }



  X = Min(Min(X1,X2),Min(X3,X4));

  Y = Min(Min(Y1,Y2),Min(Y3,Y4));

  Old_X = Max(Max(X1,X2),Max(X3,X4)) - X;

  Old_Y = Max(Max(Y1,Y2),Max(Y3,Y4)) - Y;

  Mettre_Ecran_A_Jour(X,Y,Old_X+1,Old_Y+1);

}



  // -- Tracer une courbe de B�zier d�finitivement --



void Tracer_courbe_Definitif(short X1, short Y1,

                             short X2, short Y2,

                             short X3, short Y3,

                             short X4, short Y4,

                             byte Couleur)

{

  Pixel_figure=Pixel_figure_Definitif;

  Tracer_courbe_General(X1,Y1,X2,Y2,X3,Y3,X4,Y4,Couleur);

}



  // -- Tracer la preview d'une courbe de B�zier --



void Tracer_courbe_Preview(short X1, short Y1,

                           short X2, short Y2,

                           short X3, short Y3,

                           short X4, short Y4,

                           byte Couleur)

{

  Pixel_figure=Pixel_figure_Preview;

  Tracer_courbe_General(X1,Y1,X2,Y2,X3,Y3,X4,Y4,Couleur);

}



  // -- Effacer la preview d'une courbe de B�zier --



void Effacer_courbe_Preview(short X1, short Y1,

                            short X2, short Y2,

                            short X3, short Y3,

                            short X4, short Y4,

                            byte Couleur)

{

  Pixel_figure=Pixel_figure_Effacer_preview;

  Tracer_courbe_General(X1,Y1,X2,Y2,X3,Y3,X4,Y4,Couleur);

}









  // -- Spray : un petit coup de Pschiitt! --



void Aerographe(short Bouton_clicke)

{

  short Pos_X,Pos_Y;

  short Rayon=Spray_Size>>1;

  long  Rayon_au_carre=(long)Rayon*Rayon;

  short Indice,Count;

  byte  Indice_couleur;

  byte  Sens;





  Effacer_curseur();



  if (Spray_Mode)

  {

    for (Count=1; Count<=Spray_Mono_flow; Count++)

    {

      Pos_X=(rand()%Spray_Size)-Rayon;

      Pos_Y=(rand()%Spray_Size)-Rayon;

      if ( (Pos_X*Pos_X)+(Pos_Y*Pos_Y) <= Rayon_au_carre )

      {

        Pos_X+=Pinceau_X;

        Pos_Y+=Pinceau_Y;

        if (Bouton_clicke==1)

          Afficher_pinceau(Pos_X,Pos_Y,Fore_color,0);

        else

          Afficher_pinceau(Pos_X,Pos_Y,Back_color,0);

      }

    }

  }

  else

  {

    //   On essaye de se balader dans la table des flux de fa�on � ce que ce

    // ne soit pas toujours la derni�re couleur qui soit affich�e en dernier

    // Pour �a, on part d'une couleur au pif dans une direction al�atoire.

    Sens=rand()&1;

    for (Indice=0,Indice_couleur=rand()/*%256*/; Indice<256; Indice++)

    {

      for (Count=1; Count<=Spray_Multi_flow[Indice_couleur]; Count++)

      {

        Pos_X=(rand()%Spray_Size)-Rayon;

        Pos_Y=(rand()%Spray_Size)-Rayon;

        if ( (Pos_X*Pos_X)+(Pos_Y*Pos_Y) <= Rayon_au_carre )

        {

          Pos_X+=Pinceau_X;

          Pos_Y+=Pinceau_Y;

          if (Bouton_clicke==A_GAUCHE)

            Afficher_pinceau(Pos_X,Pos_Y,Indice_couleur,0);

          else

            Afficher_pinceau(Pos_X,Pos_Y,Back_color,0);

        }

      }

      if (Sens)

        Indice_couleur++;

      else

        Indice_couleur--;

    }

  }



  Afficher_curseur();



  for (Count=1; Count<=Spray_Delay; Count++)

    Wait_VBL();

}







  //////////////////////////////////////////////////////////////////////////

  ////////////////////////// GESTION DES DEGRADES //////////////////////////

  //////////////////////////////////////////////////////////////////////////





  // -- Gestion d'un d�grad� de base (le plus moche) --



void Degrade_de_base(long Indice,short Pos_X,short Pos_Y)

{

  long Position;



  // On fait un premier calcul partiel

  Position=(Indice*Degrade_Intervalle_bornes);



  // On g�re un d�placement au hasard

  Position+=(Degrade_Intervalle_total*(rand()%Degrade_Melange_aleatoire)) >>6;

  Position-=(Degrade_Intervalle_total*Degrade_Melange_aleatoire) >>7;



  Position/=Degrade_Intervalle_total;



  //   On va v�rifier que nos petites idioties n'ont pas �ject� la valeur hors

  // des valeurs autoris�es par le d�grad� d�fini par l'utilisateur.



  if (Position<0)

    Position=0;

  else if (Position>=Degrade_Intervalle_bornes)

    Position=Degrade_Intervalle_bornes-1;



  // On ram�ne ensuite la position dans le d�grad� vers un num�ro de couleur

  if (Degrade_Inverse)

    Traiter_pixel_de_degrade(Pos_X,Pos_Y,Degrade_Borne_Superieure-Position);

  else

    Traiter_pixel_de_degrade(Pos_X,Pos_Y,Degrade_Borne_Inferieure+Position);

}





  // -- Gestion d'un d�grad� par trames simples --



void Degrade_de_trames_simples(long Indice,short Pos_X,short Pos_Y)

{

  long Position_dans_degrade;

  long Position_dans_segment;



  //

  //   But de l'op�ration: en plus de calculer la position de base (d�sign�e

  // dans cette proc�dure par "Position_dans_degrade", on calcule la position

  // de l'indice dans le sch�ma suivant:

  //

  //         | Les indices qui tra�nent de ce c�t� du segment se voient subir

  //         | une incr�mentation conditionnelle � leur position dans l'�cran.

  //         v

  //  |---|---|---|---- - - -

  //   ^

  //   |_ Les indices qui tra�nent de ce c�t� du segment se voient subir une

  //      d�cr�mentation conditionnelle � leur position dans l'�cran.



  // On fait d'abord un premier calcul partiel

  Position_dans_degrade=(Indice*Degrade_Intervalle_bornes);



  // On g�re un d�placement au hasard...

  Position_dans_degrade+=(Degrade_Intervalle_total*(rand()%Degrade_Melange_aleatoire)) >>6;

  Position_dans_degrade-=(Degrade_Intervalle_total*Degrade_Melange_aleatoire) >>7;



  if (Position_dans_degrade<0)

    Position_dans_degrade=0;



  // ... qui nous permet de calculer la position dans le segment

  Position_dans_segment=((Position_dans_degrade<<2)/Degrade_Intervalle_total)&3;



  // On peut ensuite terminer le calcul de l'indice dans le d�grad�

  Position_dans_degrade/=Degrade_Intervalle_total;



  // On va pouvoir discuter de la valeur de Position_dans_degrade en fonction

  // de la position dans l'�cran et de la Position_dans_segment.



  switch (Position_dans_segment)

  {

    case 0 : // On est sur la gauche du segment

      if (((Pos_X+Pos_Y)&1)==0)

        Position_dans_degrade--;

      break;



      // On n'a pas � traiter les cas 1 et 2 car ils repr�sentent des valeurs

      // suffisament au centre du segment pour ne pas avoir � subir la trame



    case 3 : // On est sur la droite du segment

      if (((Pos_X+Pos_Y)&1)!=0) // Note: on doit faire le test inverse au cas gauche pour synchroniser les 2 c�t�s de la trame.

        Position_dans_degrade++;

  }



  //   On va v�rifier que nos petites idioties n'ont pas �ject� la valeur hors

  // des valeurs autoris�es par le d�grad� d�fini par l'utilisateur.



  if (Position_dans_degrade<0)

    Position_dans_degrade=0;

  else if (Position_dans_degrade>=Degrade_Intervalle_bornes)

    Position_dans_degrade=Degrade_Intervalle_bornes-1;



  // On ram�ne ensuite la position dans le d�grad� vers un num�ro de couleur

  if (Degrade_Inverse)

    Position_dans_degrade=Degrade_Borne_Superieure-Position_dans_degrade;

  else

    Position_dans_degrade=Degrade_Borne_Inferieure+Position_dans_degrade;



  Traiter_pixel_de_degrade(Pos_X,Pos_Y,Position_dans_degrade);

}





  // -- Gestion d'un d�grad� par trames �tendues --



void Degrade_de_trames_etendues(long Indice,short Pos_X,short Pos_Y)

{

  long Position_dans_degrade;

  long Position_dans_segment;



//

  //   But de l'op�ration: en plus de calculer la position de base (d�sign�e

  // dans cette proc�dure par "Position_dans_degrade", on calcule la position

  // de l'indice dans le sch�ma suivant:

  //

  //         | Les indices qui tra�nent de ce c�t� du segment se voient subir

  //         | une incr�mentation conditionnelle � leur position dans l'�cran.

  //         v

  //  |---|---|---|---- - - -

  //   ^

  //   |_ Les indices qui tra�nent de ce c�t� du segment se voient subir une

  //      d�cr�mentation conditionnelle � leur position dans l'�cran.



  // On fait d'abord un premier calcul partiel

  Position_dans_degrade=(Indice*Degrade_Intervalle_bornes);



  // On g�re un d�placement au hasard

  Position_dans_degrade+=(Degrade_Intervalle_total*(rand()%Degrade_Melange_aleatoire)) >>6;

  Position_dans_degrade-=(Degrade_Intervalle_total*Degrade_Melange_aleatoire) >>7;



  if (Position_dans_degrade<0)

    Position_dans_degrade=0;



  // Qui nous permet de calculer la position dans le segment

  Position_dans_segment=((Position_dans_degrade<<3)/Degrade_Intervalle_total)&7;



  // On peut ensuite terminer le calcul de l'indice dans le d�grad�

  Position_dans_degrade/=Degrade_Intervalle_total;



  // On va pouvoir discuter de la valeur de Position_dans_degrade en fonction

  // de la position dans l'�cran et de la Position_dans_segment.



  switch (Position_dans_segment)

  {

    case 0 : // On est sur l'extr�me gauche du segment

      if (((Pos_X+Pos_Y)&1)==0)

        Position_dans_degrade--;

      break;



    case 1 : // On est sur la gauche du segment

    case 2 : // On est sur la gauche du segment

      if (((Pos_X & 1)==0) && ((Pos_Y & 1)==0))

        Position_dans_degrade--;

      break;



      // On n'a pas � traiter les cas 3 et 4 car ils repr�sentent des valeurs

      // suffisament au centre du segment pour ne pas avoir � subir la trame



    case 5 : // On est sur la droite du segment

    case 6 : // On est sur la droite du segment

      if (((Pos_X & 1)==0) && ((Pos_Y & 1)!=0))

        Position_dans_degrade++;

      break;



    case 7 : // On est sur l'extreme droite du segment

      if (((Pos_X+Pos_Y)&1)!=0) // Note: on doit faire le test inverse au cas gauche pour synchroniser les 2 c�t�s de la trame.

        Position_dans_degrade++;

  }



  //   On va v�rifier que nos petites idioties n'ont pas �ject� la valeur hors

  // des valeurs autoris�es par le d�grad� d�fini par l'utilisateur.



  if (Position_dans_degrade<0)

    Position_dans_degrade=0;

  else if (Position_dans_degrade>=Degrade_Intervalle_bornes)

    Position_dans_degrade=Degrade_Intervalle_bornes-1;



  // On ram�ne ensuite la position dans le d�grad� vers un num�ro de couleur

  if (Degrade_Inverse)

    Position_dans_degrade=Degrade_Borne_Superieure-Position_dans_degrade;

  else

    Position_dans_degrade=Degrade_Borne_Inferieure+Position_dans_degrade;



  Traiter_pixel_de_degrade(Pos_X,Pos_Y,Position_dans_degrade);

}







  // -- Tracer un cercle degrad� (une sph�re) --



void Tracer_cercle_degrade(short Centre_X,short Centre_Y,short Rayon,short Eclairage_X,short Eclairage_Y)

{

  long Debut_X;

  long Debut_Y;

  long Pos_X;

  long Pos_Y;

  long Fin_X;

  long Fin_Y;

  long Distance_X; // Distance (au carr�) sur les X du point en cours au centre d'�clairage

  long Distance_Y; // Distance (au carr�) sur les Y du point en cours au centre d'�clairage



  Debut_X=Centre_X-Rayon;

  Debut_Y=Centre_Y-Rayon;

  Fin_X=Centre_X+Rayon;

  Fin_Y=Centre_Y+Rayon;



  // Correction des bornes d'apr�s les limites

  if (Debut_Y<Limite_Haut)

    Debut_Y=Limite_Haut;

  if (Fin_Y>Limite_Bas)

    Fin_Y=Limite_Bas;

  if (Debut_X<Limite_Gauche)

    Debut_X=Limite_Gauche;

  if (Fin_X>Limite_Droite)

    Fin_X=Limite_Droite;



  Degrade_Intervalle_total=Cercle_Limite+

                           ((Centre_X-Eclairage_X)*(Centre_X-Eclairage_X))+

                           ((Centre_Y-Eclairage_Y)*(Centre_Y-Eclairage_Y))+

                           (2L*Rayon*sqrt(

                           ((Centre_X-Eclairage_X)*(Centre_X-Eclairage_X))+

                           ((Centre_Y-Eclairage_Y)*(Centre_Y-Eclairage_Y))));



  if (Degrade_Intervalle_total==0)

    Degrade_Intervalle_total=1;



  // Affichage du cercle

  for (Pos_Y=Debut_Y,Cercle_Curseur_Y=(long)Debut_Y-Centre_Y;Pos_Y<=Fin_Y;Pos_Y++,Cercle_Curseur_Y++)

  {

    Distance_Y =(Pos_Y-Eclairage_Y);

    Distance_Y*=Distance_Y;

    for (Pos_X=Debut_X,Cercle_Curseur_X=(long)Debut_X-Centre_X;Pos_X<=Fin_X;Pos_X++,Cercle_Curseur_X++)

      if (Pixel_dans_cercle())

      {

        Distance_X =(Pos_X-Eclairage_X);

        Distance_X*=Distance_X;

        Traiter_degrade(Distance_X+Distance_Y,Pos_X,Pos_Y);

      }

  }



  Mettre_Ecran_A_Jour(Centre_X-Rayon,Centre_Y-Rayon,2*Rayon+1,2*Rayon+1);

}





  // -- Tracer une ellipse degrad�e --



void Tracer_ellipse_degradee(short Centre_X,short Centre_Y,short Rayon_horizontal,short Rayon_vertical,short Eclairage_X,short Eclairage_Y)

{

  long Debut_X;

  long Debut_Y;

  long Pos_X;

  long Pos_Y;

  long Fin_X;

  long Fin_Y;

  long Distance_X; // Distance (au carr�) sur les X du point en cours au centre d'�clairage

  long Distance_Y; // Distance (au carr�) sur les Y du point en cours au centre d'�clairage





  Debut_X=Centre_X-Rayon_horizontal;

  Debut_Y=Centre_Y-Rayon_vertical;

  Fin_X=Centre_X+Rayon_horizontal;

  Fin_Y=Centre_Y+Rayon_vertical;



  // Calcul des limites de l'ellipse

  Ellipse_Calculer_limites(Rayon_horizontal+1,Rayon_vertical+1);



  // On calcule la distance maximale:

  Degrade_Intervalle_total=(Rayon_horizontal*Rayon_horizontal)+

                           (Rayon_vertical*Rayon_vertical)+

                           ((Centre_X-Eclairage_X)*(Centre_X-Eclairage_X))+

                           ((Centre_Y-Eclairage_Y)*(Centre_Y-Eclairage_Y))+

                           (2L

                           *sqrt(

                           (Rayon_horizontal*Rayon_horizontal)+

                           (Rayon_vertical  *Rayon_vertical  ))

                           *sqrt(

                           ((Centre_X-Eclairage_X)*(Centre_X-Eclairage_X))+

                           ((Centre_Y-Eclairage_Y)*(Centre_Y-Eclairage_Y))));



  if (Degrade_Intervalle_total==0)

    Degrade_Intervalle_total=1;



  // Correction des bornes d'apr�s les limites

  if (Debut_Y<Limite_Haut)

    Debut_Y=Limite_Haut;

  if (Fin_Y>Limite_Bas)

    Fin_Y=Limite_Bas;

  if (Debut_X<Limite_Gauche)

    Debut_X=Limite_Gauche;

  if (Fin_X>Limite_Droite)

    Fin_X=Limite_Droite;



  // Affichage de l'ellipse

  for (Pos_Y=Debut_Y,Ellipse_Curseur_Y=Debut_Y-Centre_Y;Pos_Y<=Fin_Y;Pos_Y++,Ellipse_Curseur_Y++)

  {

    Distance_Y =(Pos_Y-Eclairage_Y);

    Distance_Y*=Distance_Y;

    for (Pos_X=Debut_X,Ellipse_Curseur_X=Debut_X-Centre_X;Pos_X<=Fin_X;Pos_X++,Ellipse_Curseur_X++)

      if (Pixel_dans_ellipse())

      {

        Distance_X =(Pos_X-Eclairage_X);

        Distance_X*=Distance_X;

        Traiter_degrade(Distance_X+Distance_Y,Pos_X,Pos_Y);

      }

  }



  Mettre_Ecran_A_Jour(Debut_X,Debut_Y,Fin_X-Debut_X+1,Fin_Y-Debut_Y+1);

}





// Trac� d'un rectangle (RAX RAY - RBX RBY) d�grad� selon le vecteur (VAX VAY - VBX - VBY)

void Tracer_rectangle_degrade(short RAX,short RAY,short RBX,short RBY,short VAX,short VAY, short VBX, short VBY)

{

    short Pos_Y, Pos_X;



    // On commence par s'assurer que le rectangle est � l'endroit

    if(RBX < RAX)

    {

      Pos_X = RBX;

      RBX = RAX;

      RAX = Pos_X;

    }



    if(RBY < RAY)

    {

      Pos_Y = RBY;

      RBY = RAY;

      RAY = Pos_Y;

    }



    // Correction des bornes d'apr�s les limites

    if (RAY<Limite_Haut)

      RAY=Limite_Haut;

    if (RBY>Limite_Bas)

      RBY=Limite_Bas;

    if (RAX<Limite_Gauche)

      RAX=Limite_Gauche;

    if (RBX>Limite_Droite)

      RBX=Limite_Droite;



    if(VBX == VAX)

    {

      // Le vecteur est vertical, donc on �vite la partie en dessous qui foirerait avec une division par 0...

      if (VBY == VAY) return;  // L'utilisateur fait n'importe quoi

      Degrade_Intervalle_total = abs(VBY - VAY);

      for(Pos_Y=RAY;Pos_Y<=RBY;Pos_Y++)

        for(Pos_X=RAX;Pos_X<=RBX;Pos_X++)

          Traiter_degrade(abs(VBY - Pos_Y),Pos_X,Pos_Y);



    }

    else

    {

      float a;

      float b;

      float Distance_X, Distance_Y;



      Degrade_Intervalle_total = sqrt(pow(VBY - VAY,2)+pow(VBX - VAX,2));

      a = (float)(VBY - VAY)/(float)(VBX - VAX);

      b = VAY - a*VAX;

      

      for (Pos_Y=RAY;Pos_Y<=RBY;Pos_Y++)

        for (Pos_X = RAX;Pos_X<=RBX;Pos_X++)

        {

          // On calcule ou on en est dans le d�grad�

          Distance_X = pow((Pos_Y - VAY),2)+pow((Pos_X - VAX),2);

          Distance_Y = pow((-a * Pos_X + Pos_Y - b),2)/(a*a+1);

      

          Traiter_degrade((int)sqrt(Distance_X - Distance_Y),Pos_X,Pos_Y);

        }

    }

    Mettre_Ecran_A_Jour(RAX,RAY,RBX,RBY);

}









// -- Tracer un polyg�ne plein --



typedef struct POLYGON_EDGE      /* an active edge */

{

    short top;                     /* top y position */

    short bottom;                  /* bottom y position */

    float x, dx;                   /* floating point x position and gradient */

    float w;                       /* width of line segment */

    struct POLYGON_EDGE *prev;     /* doubly linked list */

    struct POLYGON_EDGE *next;

} POLYGON_EDGE;







/* fill_edge_structure:

 *  Polygon helper function: initialises an edge structure for the 2d

 *  rasteriser.

 */

void fill_edge_structure(POLYGON_EDGE *edge, short *i1, short *i2)

{

  short *it;



  if (i2[1] < i1[1])

  {

    it = i1;

    i1 = i2;

    i2 = it;

  }



  edge->top = i1[1];

  edge->bottom = i2[1] - 1;

  edge->dx = ((float) i2[0] - (float) i1[0]) / ((float) i2[1] - (float) i1[1]);

  edge->x = i1[0] + 0.4999999;

  edge->prev = NULL;

  edge->next = NULL;



  if (edge->dx+1 < 0.0)

    edge->x += edge->dx+1;



  if (edge->dx >= 0.0)

    edge->w = edge->dx;

  else

    edge->w = -(edge->dx);



  if (edge->w-1.0<0.0)

    edge->w = 0.0;

  else

    edge->w = edge->w-1;

}







/* add_edge:

 *  Adds an edge structure to a linked list, returning the new head pointer.

 */

POLYGON_EDGE * add_edge(POLYGON_EDGE *list, POLYGON_EDGE *edge, int sort_by_x)

{

  POLYGON_EDGE *pos = list;

  POLYGON_EDGE *prev = NULL;



  if (sort_by_x)

  {

    while ( (pos) && ((pos->x+((pos->w+pos->dx)/2)) < (edge->x+((edge->w+edge->dx)/2))) )

    {

      prev = pos;

      pos = pos->next;

    }

  }

  else

  {

    while ((pos) && (pos->top < edge->top))

    {

      prev = pos;

      pos = pos->next;

    }

  }



  edge->next = pos;

  edge->prev = prev;



  if (pos)

    pos->prev = edge;



  if (prev)

  {

    prev->next = edge;

    return list;

  }

  else

    return edge;

}







/* remove_edge:

 *  Removes an edge structure from a list, returning the new head pointer.

 */

POLYGON_EDGE * remove_edge(POLYGON_EDGE *list, POLYGON_EDGE *edge)

{

  if (edge->next)

    edge->next->prev = edge->prev;



  if (edge->prev)

  {

    edge->prev->next = edge->next;

    return list;

  }

  else

    return edge->next;

}







/* polygon:

 *  Draws a filled polygon with an arbitrary number of corners. Pass the

 *  number of vertices, then an array containing a series of x, y points

 *  (a total of vertices*2 values).

 */

void Polyfill_General(int Vertices, short * Points, int Color)

{

  short c;

  short top = 0x7FFF;

  short bottom = 0;

  short *i1, *i2;

  short Pos_X,Fin_X;

  POLYGON_EDGE *edge, *next_edge, *initial_edge;

  POLYGON_EDGE *active_edges = NULL;

  POLYGON_EDGE *inactive_edges = NULL;





  /* allocate some space and fill the edge table */

  initial_edge=edge=(POLYGON_EDGE *) malloc(sizeof(POLYGON_EDGE) * Vertices);



  i1 = Points;

  i2 = Points + ((Vertices-1)<<1);



  for (c=0; c<Vertices; c++)

  {

    if (i1[1] != i2[1])

    {

      fill_edge_structure(edge, i1, i2);



      if (edge->bottom >= edge->top)

      {

        if (edge->top < top)

          top = edge->top;



        if (edge->bottom > bottom)

          bottom = edge->bottom;



        inactive_edges = add_edge(inactive_edges, edge, 0);

        edge++;

      }

    }

    i2 = i1;

    i1 += 2;

  }



  /* for each scanline in the polygon... */

  for (c=top; c<=bottom; c++)

  {

    /* check for newly active edges */

    edge = inactive_edges;

    while ((edge) && (edge->top == c))

    {

      next_edge = edge->next;

      inactive_edges = remove_edge(inactive_edges, edge);

      active_edges = add_edge(active_edges, edge, 1);

      edge = next_edge;

    }



    /* draw horizontal line segments */

    if ((c>=Limite_Haut) && (c<=Limite_Bas))

    {

      edge = active_edges;

      while ((edge) && (edge->next))

      {

        Pos_X=/*Round*/(edge->x);

        Fin_X=/*Round*/(edge->next->x+edge->next->w);

        if (Pos_X<Limite_Gauche)

          Pos_X=Limite_Gauche;

        if (Fin_X>Limite_Droite)

          Fin_X=Limite_Droite;

        for (; Pos_X<=Fin_X; Pos_X++)

          Pixel_figure(Pos_X,c,Color);

        edge = edge->next->next;

      }

    }



    /* update edges, sorting and removing dead ones */

    edge = active_edges;

    while (edge)

    {

      next_edge = edge->next;

      if (c >= edge->bottom)

        active_edges = remove_edge(active_edges, edge);

      else

      {

        edge->x += edge->dx;

        while ((edge->prev) && ( (edge->x+(edge->w/2)) < (edge->prev->x+(edge->prev->w/2))) )

        {

          if (edge->next)

            edge->next->prev = edge->prev;

          edge->prev->next = edge->next;

          edge->next = edge->prev;

          edge->prev = edge->prev->prev;

          edge->next->prev = edge;

          if (edge->prev)

            edge->prev->next = edge;

          else

            active_edges = edge;

        }

      }

      edge = next_edge;

    }

  }



  free(initial_edge);

  // On ne connait pas simplement les xmin et xmax ici, mais de toutes fa�on ce n'est pas utilis� en preview

  Mettre_Ecran_A_Jour(0,top,Principal_Largeur_image,bottom-top+1);

}





void Polyfill(int Vertices, short * Points, int Color)

{

  Pixel_figure=Afficher_pixel;

  Polyfill_General(Vertices,Points,Color);

}







//------------ Remplacement de la couleur point�e par une autre --------------



void Remplacer(byte Nouvelle_couleur)

{

  byte Ancienne_couleur;



  if ((Pinceau_X<Principal_Largeur_image)

   && (Pinceau_Y<Principal_Hauteur_image))

  {

    Ancienne_couleur=Lit_pixel_dans_ecran_courant(Pinceau_X,Pinceau_Y);

    if ( (Ancienne_couleur!=Nouvelle_couleur)

      && ((!Stencil_Mode) || (!Stencil[Ancienne_couleur])) )

    {

      Remplacer_une_couleur(Ancienne_couleur,Nouvelle_couleur);

      Afficher_ecran();

    }

  }

}







/******************************************************************************/

/********************************** SHADES ************************************/



// Transformer une liste de shade en deux tables de conversion

void Liste2tables(word * Liste,short Pas,byte Mode,byte * Table_inc,byte * Table_dec)

{

  int Indice;

  int Premier;

  int Dernier;

  int Couleur;

  int Temp;





  // On initialise les deux tables de conversion en Identit�

  for (Indice=0;Indice<256;Indice++)

  {

    Table_inc[Indice]=Indice;

    Table_dec[Indice]=Indice;

  }



  // On s'appr�te � examiner l'ensemble de la liste

  for (Indice=0;Indice<512;Indice++)

  {

    // On recherche la premi�re case de la liste non vide (et non inhib�e)

    while ((Indice<512) && (Liste[Indice]>255))

      Indice++;



    // On note la position de la premi�re case de la s�quence

    Premier=Indice;



    // On recherche la position de la derni�re case de la s�quence

    for (Dernier=Premier;Liste[Dernier+1]<256;Dernier++);



    // Pour toutes les cases non vides (et non inhib�es) qui suivent

    switch (Mode)

    {

      case MODE_SHADE_NORMAL :

        for (;(Indice<512) && (Liste[Indice]<256);Indice++)

        { // On met � jour les tables de conversion

          Couleur=Liste[Indice];

          Table_inc[Couleur]=Liste[(Indice+Pas<=Dernier)?Indice+Pas:Dernier];

          Table_dec[Couleur]=Liste[(Indice-Pas>=Premier)?Indice-Pas:Premier];

        }

        break;

      case MODE_SHADE_BOUCLE :

        Temp=1+Dernier-Premier;

        for (;(Indice<512) && (Liste[Indice]<256);Indice++)

        { // On met � jour les tables de conversion

          Couleur=Liste[Indice];

          Table_inc[Couleur]=Liste[Premier+((Pas+Indice-Premier)%Temp)];

          Table_dec[Couleur]=Liste[Premier+(((Temp-Pas)+Indice-Premier)%Temp)];

        }

        break;

      default : // MODE_SHADE_NOSAT

        for (;(Indice<512) && (Liste[Indice]<256);Indice++)

        { // On met � jour les tables de conversion

          Couleur=Liste[Indice];

          if (Indice+Pas<=Dernier)

            Table_inc[Couleur]=Liste[Indice+Pas];

          if (Indice-Pas>=Premier)

            Table_dec[Couleur]=Liste[Indice-Pas];

        }

    }

  }

}







// -- Interface avec l'image, affect�e par le facteur de grossissement -------



  // fonction d'affichage "Pixel" utilis�e pour les op�rations d�finitivement

  // Ne doit � aucune condition �tre appel�e en dehors de la partie visible

  // de l'image dans l'�cran (�a pourrait �tre grave)

void Afficher_pixel(word X,word Y,byte Couleur)

  // X & Y    sont la position d'un point dans l'IMAGE

  // Couleur  est la couleur du point

  // Le Stencil est g�r�.

  // Les effets sont g�r�s par appel � Fonction_effet().

  // La Loupe est g�r�e par appel � Pixel_Preview().

{

  if ( ( (!Trame_Mode)   || (Effet_Trame(X,Y)) )

    && (!((Stencil_Mode) && (Stencil[Lit_pixel_dans_ecran_courant(X,Y)])))

    && (!((Mask_Mode)    && (Mask_table[Lit_pixel_dans_ecran_brouillon(X,Y)]))) )

  {

    Couleur=Fonction_effet(X,Y,Couleur);

    Pixel_dans_ecran_courant(X,Y,Couleur);

    Pixel_Preview(X,Y,Couleur);

  }

}







// -- Calcul des diff�rents effets -------------------------------------------



  // -- Aucun effet en cours --



byte Aucun_effet(__attribute__((unused)) word X,__attribute__((unused)) word Y,byte Couleur)

{

  return Couleur;

}



  // -- Effet de Shading --



byte Effet_Shade(word X,word Y,__attribute__((unused)) byte Couleur)

{

  return Shade_Table[Lit_pixel_dans_ecran_feedback(X,Y)];

}



byte Effet_Quick_shade(word X,word Y,byte Couleur)

{

  int C=Couleur=Lit_pixel_dans_ecran_feedback(X,Y);

  int Sens=(Fore_color<=Back_color);

  byte Debut,Fin;

  int Largeur;



  if (Sens)

  {

    Debut=Fore_color;

    Fin  =Back_color;

  }

  else

  {

    Debut=Back_color;

    Fin  =Fore_color;

  }



  if ((C>=Debut) && (C<=Fin) && (Debut!=Fin))

  {

    Largeur=1+Fin-Debut;



    if ( ((Mouse_K==A_GAUCHE) && Sens) || ((Mouse_K==A_DROITE) && (!Sens)) )

      C-=Quick_shade_Step%Largeur;

    else

      C+=Quick_shade_Step%Largeur;



    if (C<Debut)

      switch (Quick_shade_Loop)

      {

        case MODE_SHADE_NORMAL : return Debut;

        case MODE_SHADE_BOUCLE : return (Largeur+C);

        default : return Couleur;

      }



    if (C>Fin)

      switch (Quick_shade_Loop)

      {

        case MODE_SHADE_NORMAL : return Fin;

        case MODE_SHADE_BOUCLE : return (C-Largeur);

        default : return Couleur;

      }

  }



  return C;

}



  // -- Effet de Tiling --



byte Effet_Tiling(word X,word Y,__attribute__((unused)) byte Couleur)

{

  return Lit_pixel_dans_brosse((X+Brosse_Largeur-Tiling_Decalage_X)%Brosse_Largeur,

                               (Y+Brosse_Hauteur-Tiling_Decalage_Y)%Brosse_Hauteur);

}



  // -- Effet de Smooth --



byte Effet_Smooth(word X,word Y,__attribute__((unused)) byte Couleur)

{

  int R,V,B;

  byte C;

  int Poids,Poids_total;

  byte X2=((X+1)<Principal_Largeur_image);

  byte Y2=((Y+1)<Principal_Hauteur_image);



  // On commence par le pixel central

  C=Lit_pixel_dans_ecran_feedback(X,Y);

  Poids_total=Smooth_Matrice[1][1];

  R=Poids_total*Principal_Palette[C].R;

  V=Poids_total*Principal_Palette[C].V;

  B=Poids_total*Principal_Palette[C].B;



  if (X)

  {

    C=Lit_pixel_dans_ecran_feedback(X-1,Y);

    Poids_total+=(Poids=Smooth_Matrice[0][1]);

    R+=Poids*Principal_Palette[C].R;

    V+=Poids*Principal_Palette[C].V;

    B+=Poids*Principal_Palette[C].B;



    if (Y)

    {

      C=Lit_pixel_dans_ecran_feedback(X-1,Y-1);

      Poids_total+=(Poids=Smooth_Matrice[0][0]);

      R+=Poids*Principal_Palette[C].R;

      V+=Poids*Principal_Palette[C].V;

      B+=Poids*Principal_Palette[C].B;



      if (Y2)

      {

        C=Lit_pixel_dans_ecran_feedback(X-1,Y+1);

        Poids_total+=(Poids=Smooth_Matrice[0][2]);

        R+=Poids*Principal_Palette[C].R;

        V+=Poids*Principal_Palette[C].V;

        B+=Poids*Principal_Palette[C].B;

      }

    }

  }



  if (X2)

  {

    C=Lit_pixel_dans_ecran_feedback(X+1,Y);

    Poids_total+=(Poids=Smooth_Matrice[2][1]);

    R+=Poids*Principal_Palette[C].R;

    V+=Poids*Principal_Palette[C].V;

    B+=Poids*Principal_Palette[C].B;



    if (Y)

    {

      C=Lit_pixel_dans_ecran_feedback(X+1,Y-1);

      Poids_total+=(Poids=Smooth_Matrice[2][0]);

      R+=Poids*Principal_Palette[C].R;

      V+=Poids*Principal_Palette[C].V;

      B+=Poids*Principal_Palette[C].B;



      if (Y2)

      {

        C=Lit_pixel_dans_ecran_feedback(X+1,Y+1);

        Poids_total+=(Poids=Smooth_Matrice[2][2]);

        R+=Poids*Principal_Palette[C].R;

        V+=Poids*Principal_Palette[C].V;

        B+=Poids*Principal_Palette[C].B;

      }

    }

  }



  if (Y)

  {

    C=Lit_pixel_dans_ecran_feedback(X,Y-1);

    Poids_total+=(Poids=Smooth_Matrice[1][0]);

    R+=Poids*Principal_Palette[C].R;

    V+=Poids*Principal_Palette[C].V;

    B+=Poids*Principal_Palette[C].B;

  }



  if (Y2)

  {

    C=Lit_pixel_dans_ecran_feedback(X,Y+1);

    Poids_total+=(Poids=Smooth_Matrice[1][2]);

    R+=Poids*Principal_Palette[C].R;

    V+=Poids*Principal_Palette[C].V;

    B+=Poids*Principal_Palette[C].B;

  }



  return (Poids_total)? // On regarde s'il faut �viter le 0/0.

    Meilleure_couleur(Round_div(R,Poids_total),

                      Round_div(V,Poids_total),

                      Round_div(B,Poids_total)):

    Lit_pixel_dans_ecran_courant(X,Y); // C'est bien l'�cran courant et pas

                                       // l'�cran feedback car il s'agit de ne

}                                      // pas modifier l'�cran courant.

