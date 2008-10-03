#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "const.h"
#include "struct.h"
#include "global.h"
#include "divers.h"
#include "graph.h"
#include "moteur.h"
#include "readline.h"
#include "boutons.h"
#include "pages.h"

// --------------------------- Menu des palettes -----------------------------
char * Libelle_reduction_palette[7]=
{
  "128"," 64"," 32"," 16","  8","  4","  2"
};


void Modifier_Rouge(byte Couleur, short Nouvelle_teinte, T_Palette Palette)
{
  if (Nouvelle_teinte< 0)
    Nouvelle_teinte= 0;
  if (Nouvelle_teinte>63)
    Nouvelle_teinte=63;

  Palette[Couleur].R=Nouvelle_teinte;
  Set_color(Couleur,Palette[Couleur].R,Palette[Couleur].V,Palette[Couleur].B);
}


void Modifier_Vert(byte Couleur, short Nouvelle_teinte, T_Palette Palette)
{
  if (Nouvelle_teinte< 0)
    Nouvelle_teinte= 0;
  if (Nouvelle_teinte>63)
    Nouvelle_teinte=63;

  Palette[Couleur].V=Nouvelle_teinte;
  Set_color(Couleur,Palette[Couleur].R,Palette[Couleur].V,Palette[Couleur].B);
}


void Modifier_Bleu(byte Couleur, short Nouvelle_teinte, T_Palette Palette)
{
  if (Nouvelle_teinte< 0)
    Nouvelle_teinte= 0;
  if (Nouvelle_teinte>63)
    Nouvelle_teinte=63;

  Palette[Couleur].B=Nouvelle_teinte;
  Set_color(Couleur,Palette[Couleur].R,Palette[Couleur].V,Palette[Couleur].B);
}


void Degrader_palette(short Debut,short Fin,T_Palette Palette)
// Modifie la palette pour obtenir un d�grad� de couleur entre les deux bornes
// pass�es en param�tre
{
  short Debut_Rouge;
  short Debut_Vert;
  short Debut_Bleu;
  short Fin_Rouge;
  short Fin_Vert;
  short Fin_Bleu;
  short Indice;

  // On v�rifie qu'il y ait assez de couleurs entre le d�but et la fin pour
  // pouvoir faire un d�grad�:
  if ( (Debut!=Fin) && (Debut+1!=Fin) )
  {
    Debut_Rouge=Palette[Debut].R;
    Debut_Vert =Palette[Debut].V;
    Debut_Bleu =Palette[Debut].B;

    Fin_Rouge  =Palette[Fin  ].R;
    Fin_Vert   =Palette[Fin  ].V;
    Fin_Bleu   =Palette[Fin  ].B;

    for (Indice=Debut+1;Indice<Fin;Indice++)
    {
      Modifier_Rouge(Indice, ((Fin_Rouge-Debut_Rouge) * (Indice-Debut))/(Fin-Debut) + Debut_Rouge,Palette);
      Modifier_Vert (Indice, ((Fin_Vert -Debut_Vert ) * (Indice-Debut))/(Fin-Debut) + Debut_Vert ,Palette);
      Modifier_Bleu (Indice, ((Fin_Bleu -Debut_Bleu ) * (Indice-Debut))/(Fin-Debut) + Debut_Bleu ,Palette);
    }
  }
}


void Compter_nb_couleurs_utilisees(short * Nb_couleurs_utilisees, dword * Utilisation_couleur)
{
  char   Chaine[10];

  Effacer_curseur();
  Forme_curseur=FORME_CURSEUR_SABLIER;
  Afficher_curseur();
  *Nb_couleurs_utilisees=Palette_Compter_nb_couleurs_utilisees(Utilisation_couleur);
  strcpy(Chaine,"Used: ");
  Num2str(*Nb_couleurs_utilisees,Chaine+6,3);
  Effacer_curseur();
  Fenetre_Dessiner_bouton_normal(132,20,83,14,Chaine,4,1);
  Forme_curseur=FORME_CURSEUR_FLECHE;
  Afficher_curseur();
}


void Remap_zone_HIGH(short X1, short Y1, short X2, short Y2,
                     byte * Table_de_conversion)
// Attention: Remappe une zone de coins X1,Y1 et X2-1,Y2-1 !!!
{
  short Pos_X;
  short Pos_Y;

  for (Pos_Y=Y1;Pos_Y<Y2;Pos_Y++)
    for (Pos_X=X1;Pos_X<X2;Pos_X++)
    {
      if ((Pos_Y>=Fenetre_Pos_Y) && (Pos_Y<Fenetre_Pos_Y+(Fenetre_Hauteur*Menu_Facteur_Y)) &&
          (Pos_X>=Fenetre_Pos_X) && (Pos_X<Fenetre_Pos_X+(Fenetre_Largeur*Menu_Facteur_X)) )
        Pos_X=Fenetre_Pos_X+(Fenetre_Largeur*Menu_Facteur_X)-1;
      else
        Pixel(Pos_X,Pos_Y,Table_de_conversion[Lit_pixel(Pos_X,Pos_Y)]);
    }
}

void Remap_image_HIGH(byte * Table_de_conversion)
{
  short Fin_X;
  short Fin_Y;
  short Fin_X_Loupe=0;
  short Fin_Y_Loupe=0;

  // On s'occupe de faire la traduction dans l'image
  Remap_general_LOWLEVEL(Table_de_conversion,Principal_Ecran,
                         Principal_Largeur_image,Principal_Hauteur_image,Principal_Largeur_image);

  // On calcule les limites � l'�cran de l'image
  if (Principal_Hauteur_image>=Menu_Ordonnee_avant_fenetre)
    Fin_Y=Menu_Ordonnee_avant_fenetre;
  else
    Fin_Y=Principal_Hauteur_image;

  if (!Loupe_Mode)
  {
    if (Principal_Largeur_image>=Largeur_ecran)
      Fin_X=Largeur_ecran;
    else
      Fin_X=Principal_Largeur_image;

  }
  else
  {
    if (Principal_Largeur_image>=Principal_Split)
      Fin_X=Principal_Split;
    else
      Fin_X=Principal_Largeur_image;

    if ((Principal_X_Zoom+(Principal_Largeur_image*Loupe_Facteur))>=Largeur_ecran)
      Fin_X_Loupe=Largeur_ecran;
    else
      Fin_X_Loupe=(Principal_X_Zoom+(Principal_Largeur_image*Loupe_Facteur));

    if (Principal_Hauteur_image*Loupe_Facteur>=Menu_Ordonnee_avant_fenetre)
      Fin_Y_Loupe=Menu_Ordonnee_avant_fenetre;
    else
      Fin_Y_Loupe=Principal_Hauteur_image*Loupe_Facteur;
  }

  // On doit maintenant faire la traduction � l'�cran
  Remap_zone_HIGH(0,0,Fin_X,Fin_Y,Table_de_conversion);

  if (Loupe_Mode)
  {
    Remap_zone_HIGH(Principal_Split,0,Fin_X_Loupe,Fin_Y_Loupe,Table_de_conversion);
    // Il peut encore rester le bas de la barre de split � remapper si la
    // partie zoom�e ne descend pas jusqu'en bas...
    Remap_zone_HIGH(Principal_Split,Fin_Y_Loupe,
                    (Principal_Split+(LARGEUR_BARRE_SPLIT*Menu_Facteur_X)),
                    Menu_Ordonnee_avant_fenetre,Table_de_conversion);
  }
}



void Swap(int X_Swap,short Debut_Bloc_1,short Debut_Bloc_2,short Taille_du_bloc,T_Palette Palette, dword * Utilisation_couleur)
{
  short Pos_1;
  short Pos_2;
  short Fin_1;
  short Fin_2;
  dword Tempo;
  byte  Table_de_conversion[256];

  struct Composantes * Palette_temporaire;
  dword * Utilisation_temporaire;

  // On fait une copie de la palette
  Palette_temporaire=(struct Composantes *)malloc(sizeof(T_Palette));
  memcpy(Palette_temporaire,Palette,sizeof(T_Palette));

  // On fait une copie de la table d'utilisation des couleurs
  Utilisation_temporaire=(dword *)malloc(sizeof(dword)*256);
  memcpy(Utilisation_temporaire,Utilisation_couleur,sizeof(dword)*256);

  // On commence � initialiser la table de conversion � un �tat o� elle ne
  // fera aucune conversion.
  for (Pos_1=0;Pos_1<=255;Pos_1++)
    Table_de_conversion[Pos_1]=Pos_1;

  // On calcul les derni�res couleurs de chaque bloc.
  Fin_1=Debut_Bloc_1+Taille_du_bloc-1;
  Fin_2=Debut_Bloc_2+Taille_du_bloc-1;

  if ((Debut_Bloc_2>=Debut_Bloc_1) && (Debut_Bloc_2<=Fin_1))
  {
    // Le bloc destination commence dans le bloc source.

    for (Pos_1=Debut_Bloc_1,Pos_2=Fin_1+1;Pos_1<=Fin_2;Pos_1++)
    {
      // Il faut transformer la couleur Pos_1 en Pos_2:

      Table_de_conversion[Pos_2]=Pos_1;
      Utilisation_couleur[Pos_1]=Utilisation_temporaire[Pos_2];
      Palette[Pos_1].R=Palette_temporaire[Pos_2].R;
      Palette[Pos_1].V=Palette_temporaire[Pos_2].V;
      Palette[Pos_1].B=Palette_temporaire[Pos_2].B;

      // On g�re la mise � jour de Pos_2
      if (Pos_2==Fin_2)
        Pos_2=Debut_Bloc_1;
      else
        Pos_2++;
    }
  }
  else
  if ((Debut_Bloc_2<Debut_Bloc_1) && (Fin_2>=Debut_Bloc_1))
  {
    // Le bloc destination d�borde dans le bloc source.

    for (Pos_1=Debut_Bloc_2,Pos_2=Debut_Bloc_1;Pos_1<=Fin_1;Pos_1++)
    {
      // Il faut transformer la couleur Pos_1 en Pos_2:

      Table_de_conversion[Pos_2]=Pos_1;
      Utilisation_couleur[Pos_1]=Utilisation_temporaire[Pos_2];
      Palette[Pos_1].R=Palette_temporaire[Pos_2].R;
      Palette[Pos_1].V=Palette_temporaire[Pos_2].V;
      Palette[Pos_1].B=Palette_temporaire[Pos_2].B;

      // On g�re la mise � jour de Pos_2
      if (Pos_2==Fin_1)
        Pos_2=Debut_Bloc_2;
      else
        Pos_2++;
    }
  }
  else
  {
    // Le bloc source et le bloc destination sont distincts.

    for (Pos_1=Debut_Bloc_1,Pos_2=Debut_Bloc_2;Pos_1<=Fin_1;Pos_1++,Pos_2++)
    {
      // Il va falloir permutter la couleur Pos_1 avec la couleur Pos_2
      Table_de_conversion[Pos_1]=Pos_2;
      Table_de_conversion[Pos_2]=Pos_1;

      //   On intervertit le nombre d'utilisation des couleurs pour garder une
      // coh�rence lors d'un �ventuel "Zap unused".
      Tempo                     =Utilisation_couleur[Pos_1];
      Utilisation_couleur[Pos_1]=Utilisation_couleur[Pos_2];
      Utilisation_couleur[Pos_2]=Tempo;

      // On fait un changement de teinte:
      Tempo           =Palette[Pos_1].R;
      Palette[Pos_1].R=Palette[Pos_2].R;
      Palette[Pos_2].R=Tempo;

      Tempo           =Palette[Pos_1].V;
      Palette[Pos_1].V=Palette[Pos_2].V;
      Palette[Pos_2].V=Tempo;

      Tempo           =Palette[Pos_1].B;
      Palette[Pos_1].B=Palette[Pos_2].B;
      Palette[Pos_2].B=Tempo;
    }
  }

  free(Utilisation_temporaire);
  free(Palette_temporaire);

  // Maintenant, tous ces calculs doivent �tres pris en compte dans la
  // palette, l'image et � l'�cran.
  Set_palette(Palette);

  if (X_Swap)
    Remap_image_HIGH(Table_de_conversion);
}



void Remettre_proprement_les_couleurs_du_menu(dword * Utilisation_couleur)
{
  short Indice,Indice2;
  byte Couleur;
  byte Table_de_remplacement[256];
  struct Composantes RVB[4];
  short Nouvelles[4]={255,254,253,252};

  // On initialise la table de remplacement
  for (Indice=0; Indice<256; Indice++)
    Table_de_remplacement[Indice]=Indice;

  // On recherche les 4 couleurs les moins utilis�es dans l'image pour pouvoir
  // les remplacer par les nouvelles couleurs.
  for (Indice2=0; Indice2<4; Indice2++)
    for (Indice=255; Indice>=0; Indice--)
    {
      if ((Indice!=Nouvelles[0]) && (Indice!=Nouvelles[1])
       && (Indice!=Nouvelles[2]) && (Indice!=Nouvelles[3])
       && (Utilisation_couleur[Indice]<Utilisation_couleur[Nouvelles[Indice2]]))
        Nouvelles[Indice2]=Indice;
    }

  // On trie maintenant la table dans le sens d�croissant.
  // (Ce n'est pas indispensable, mais �a fera plus joli dans la palette).
  do
  {
    Couleur=0; // Bool�en qui dit si le tri n'est pas termin�.
    for (Indice=0; Indice<3; Indice++)
    {
      if (Nouvelles[Indice]>Nouvelles[Indice+1])
      {
        Indice2            =Nouvelles[Indice];
        Nouvelles[Indice]  =Nouvelles[Indice+1];
        Nouvelles[Indice+1]=Indice2;
        Couleur=1;
      }
    }
  } while (Couleur);

  //   On sauvegarde dans RVB les teintes qu'on va remplacer et on met les
  // couleurs du menu par d�faut
  for (Indice=0; Indice<4; Indice++)
  {
    Couleur=Nouvelles[Indice];
    RVB[Indice].R=Principal_Palette[Couleur].R;
    RVB[Indice].V=Principal_Palette[Couleur].V;
    RVB[Indice].B=Principal_Palette[Couleur].B;
    Principal_Palette[Couleur].R=Coul_menu_pref[Indice].R;
    Principal_Palette[Couleur].V=Coul_menu_pref[Indice].V;
    Principal_Palette[Couleur].B=Coul_menu_pref[Indice].B;
  }

  //   Maintenant qu'on a plac� notre nouvelle palette, on va chercher quelles
  // sont les couleurs qui peuvent remplacer les anciennes
  Effacer_curseur();
  for (Indice=0; Indice<4; Indice++)
    Table_de_remplacement[Nouvelles[Indice]]=Meilleure_couleur_sans_exclusion
                                  (RVB[Indice].R,RVB[Indice].V,RVB[Indice].B);

  // On fait un changement des couleurs visibles � l'�cran et dans l'image
  Remap_image_HIGH(Table_de_remplacement);

  Afficher_curseur();
}



void Reduce_palette(short * Nb_couleurs_utilisees,int Nb_couleurs_demandees,T_Palette Palette,dword * Utilisation_couleur)
{
  char  Chaine[4];                // Buffer d'affichage du compteur
  byte  Table_de_conversion[256]; // Table de conversion
  int   Couleur_1;                // |_ Variables de balayages
  int   Couleur_2;                // |  de la palette
  int   Meilleure_couleur_1=0;
  int   Meilleure_couleur_2=0;
  int   Difference;
  int   Meilleure_difference;
  dword Utilisation;
  dword Meilleure_utilisation;

  //   On commence par initialiser la table de conversion dans un �tat o�
  // aucune conversion ne sera effectu�e.
  for (Couleur_1=0; Couleur_1<=255; Couleur_1++)
    Table_de_conversion[Couleur_1]=Couleur_1;

  //   Si on ne connait pas encore le nombre de couleurs utilis�es, on le
  // calcule! (!!! La fonction appel�e Efface puis Affiche le curseur !!!)
  if ((*Nb_couleurs_utilisees)<0)
    Compter_nb_couleurs_utilisees(Nb_couleurs_utilisees,Utilisation_couleur);

  Effacer_curseur();

  //   On tasse la palette vers le d�but parce qu'elle doit ressembler �
  // du Gruy�re (et comme Papouille il aime pas le fromage...)

  // Pour cela, on va scruter la couleur Couleur_1 et se servir de l'indice
  // Couleur_2 comme position de destination.
  for (Couleur_1=Couleur_2=0;Couleur_1<=255;Couleur_1++)
  {
    if (Utilisation_couleur[Couleur_1])
    {
      // On commence par s'occuper des teintes de la palette
      Palette[Couleur_2].R=Palette[Couleur_1].R;
      Palette[Couleur_2].V=Palette[Couleur_1].V;
      Palette[Couleur_2].B=Palette[Couleur_1].B;

      // Ensuite, on met � jour le tableau d'occupation des couleurs.
      Utilisation_couleur[Couleur_2]=Utilisation_couleur[Couleur_1];

      // On va maintenant s'occuper de la table de conversion:
      Table_de_conversion[Couleur_1]=Couleur_2;

      // Maintenant, la place d�sign�e par Couleur_2 est occup�e, alors on
      // doit passer � un indice de destination suivant.
      Couleur_2++;
    }
  }

  // On met toutes les couleurs inutilis�es en noir
  for (;Couleur_2<256;Couleur_2++)
  {
    Palette[Couleur_2].R=0;
    Palette[Couleur_2].V=0;
    Palette[Couleur_2].B=0;
    Utilisation_couleur[Couleur_2]=0;
  }

  //   Maintenant qu'on a une palette clean, on va boucler en r�duisant
  // le nombre de couleurs jusqu'� ce qu'on atteigne le nombre d�sir�.
  while ((*Nb_couleurs_utilisees)>Nb_couleurs_demandees)
  {
    //   Il s'agit de trouver les 2 couleurs qui se ressemblent le plus
    // parmis celles qui sont utilis�es (bien s�r) et de les remplacer par
    // une seule couleur qui est la moyenne pond�r�e de ces 2 couleurs
    // en fonction de leur utilisation dans l'image.

    Meilleure_difference =0x7FFF;
    Meilleure_utilisation=0x7FFFFFFF;

    for (Couleur_1=0;Couleur_1<(*Nb_couleurs_utilisees);Couleur_1++)
      for (Couleur_2=Couleur_1+1;Couleur_2<(*Nb_couleurs_utilisees);Couleur_2++)
        if (Couleur_1!=Couleur_2)
        {
          Difference =abs((short)Palette[Couleur_1].R-Palette[Couleur_2].R)+
                      abs((short)Palette[Couleur_1].V-Palette[Couleur_2].V)+
                      abs((short)Palette[Couleur_1].B-Palette[Couleur_2].B);

          if (Difference<=Meilleure_difference)
          {
            Utilisation=Utilisation_couleur[Couleur_1]+Utilisation_couleur[Couleur_2];
            if ((Difference<Meilleure_difference) || (Utilisation<Meilleure_utilisation))
            {
              Meilleure_difference =Difference;
              Meilleure_utilisation=Utilisation;
              Meilleure_couleur_1  =Couleur_1;
              Meilleure_couleur_2  =Couleur_2;
            }
          }
        }

    //   Maintenant qu'on les a trouv�es, on va pouvoir mettre � jour nos
    // donn�es pour que le remplacement se fasse sans encombres.

    // En somme, on va remplacer Meilleure_couleur_2 par Meilleure_couleur_1,
    // mais attention, on ne remplace pas Meilleure_couleur_1 par
    // Meilleure_couleur_2 !

    // On met � jour la palette.
    Palette[Meilleure_couleur_1].R=Round_div((Utilisation_couleur[Meilleure_couleur_1]*Palette[Meilleure_couleur_1].R)+
                                             (Utilisation_couleur[Meilleure_couleur_2]*Palette[Meilleure_couleur_2].R),
                                             Meilleure_utilisation);
    Palette[Meilleure_couleur_1].V=Round_div((Utilisation_couleur[Meilleure_couleur_1]*Palette[Meilleure_couleur_1].V)+
                                             (Utilisation_couleur[Meilleure_couleur_2]*Palette[Meilleure_couleur_2].V),
                                             Meilleure_utilisation);
    Palette[Meilleure_couleur_1].B=Round_div((Utilisation_couleur[Meilleure_couleur_1]*Palette[Meilleure_couleur_1].B)+
                                             (Utilisation_couleur[Meilleure_couleur_2]*Palette[Meilleure_couleur_2].B),
                                             Meilleure_utilisation);

    // On met � jour la table d'utilisation.
    Utilisation_couleur[Meilleure_couleur_1]+=Utilisation_couleur[Meilleure_couleur_2];
    Utilisation_couleur[Meilleure_couleur_2]=0;

    // On met � jour la table de conversion.
    for (Couleur_1=0;Couleur_1<=255;Couleur_1++)
    {
      if (Table_de_conversion[Couleur_1]==Meilleure_couleur_2)
      {
        //   La Couleur_1 avait d�j� pr�vue de se faire remplacer par la
        // couleur que l'on veut maintenant �liminer. On va maintenant
        // demander � ce que la Couleur_1 se fasse remplacer par la
        // Meilleure_couleur_1.
        Table_de_conversion[Couleur_1]=Meilleure_couleur_1;
      }
    }

    //   Bon, maintenant que l'on a fait bouger nos petites choses concernants
    // la couleur � �liminer, on va s'occuper de faire bouger les couleurs
    // situ�es apr�s la couleur � �liminer pour qu'elles se d�pla�ent d'une
    // couleur en arri�re.
    for (Couleur_1=0;Couleur_1<=255;Couleur_1++)
    {
      //   Commen�ons par nous occuper des tables d'utilisation et de la
      // palette.

      if (Couleur_1>Meilleure_couleur_2)
      {
        // La Couleur_1 va scroller en arri�re.

        //   Donc on transf�re son utilisation dans l'utilisation de la
        // couleur qui la pr�c�de.
        Utilisation_couleur[Couleur_1-1]=Utilisation_couleur[Couleur_1];

        //   Et on transf�re ses teintes dans les teintes de la couleur qui
        // la pr�c�de.
        Palette[Couleur_1-1].R=Palette[Couleur_1].R;
        Palette[Couleur_1-1].V=Palette[Couleur_1].V;
        Palette[Couleur_1-1].B=Palette[Couleur_1].B;
      }

      //   Une fois la palette et la table d'utilisation g�r�es, on peut
      // s'occuper de notre table de conversion.
      if (Table_de_conversion[Couleur_1]>Meilleure_couleur_2)
        //   La Couleur_1 avait l'intention de se faire remplacer par une
        // couleur que l'on va (ou que l'on a d�j�) bouger en arri�re.
        Table_de_conversion[Couleur_1]--;
    }

    //   On vient d'�jecter une couleur, donc on peut mettre � jour le nombre
    // de couleurs utilis�es.
    (*Nb_couleurs_utilisees)--;

    // A la fin, on doit passer (dans la palette) les teintes du dernier
    // �l�ment de notre ensemble en noir.
    Palette[*Nb_couleurs_utilisees].R=0;
    Palette[*Nb_couleurs_utilisees].V=0;
    Palette[*Nb_couleurs_utilisees].B=0;

    // Au passage, on va s'assurer que l'on a pas oubli� de la mettre � une
    // utilisation nulle.
    Utilisation_couleur[*Nb_couleurs_utilisees]=0;

    // Apr�s avoir �ject� une couleur, on le fait savoir � l'utilisateur par
    // l'interm�diaire du compteur de nombre utilis�es.
    Num2str(*Nb_couleurs_utilisees,Chaine,3);
    Print_dans_fenetre(186,23,Chaine,CM_Noir,CM_Clair);
  }

  //   Maintenant, tous ces calculs doivent �tres pris en compte dans la
  // palette, l'image et � l'�cran.
  Remap_image_HIGH(Table_de_conversion); // Et voila pour l'image et l'�cran
  Afficher_curseur();
}



void Palette_Modifier_jauge(struct Fenetre_Bouton_scroller * Jauge,
                            word Nb_elements, word Position,
                            char * Valeur, short Pos_X)
{
  Jauge->Nb_elements=Nb_elements;
  Jauge->Position=Position;
  Calculer_hauteur_curseur_jauge(Jauge);
  Fenetre_Dessiner_jauge(Jauge);
  Print_dans_fenetre(Pos_X,172,Valeur,CM_Noir,CM_Clair);
}



void Afficher_les_jauges(struct Fenetre_Bouton_scroller * Jauge_rouge,
                         struct Fenetre_Bouton_scroller * Jauge_verte,
                         struct Fenetre_Bouton_scroller * Jauge_bleue,
                         byte Bloc_selectionne, struct Composantes * Palette)
{
  char Chaine[3];

  if (Bloc_selectionne)
  {
    Palette_Modifier_jauge(Jauge_rouge,127,63,"� 0",176);
    Palette_Modifier_jauge(Jauge_verte,127,63,"� 0",203);
    Palette_Modifier_jauge(Jauge_bleue,127,63,"� 0",230);
  }
  else
  {
    Num2str(Palette[Fore_color].R,Chaine,2);
    Palette_Modifier_jauge(Jauge_rouge,64,63-Palette[Fore_color].R,Chaine,180);
    Num2str(Palette[Fore_color].V,Chaine,2);
    Palette_Modifier_jauge(Jauge_verte,64,63-Palette[Fore_color].V,Chaine,207);
    Num2str(Palette[Fore_color].B,Chaine,2);
    Palette_Modifier_jauge(Jauge_bleue,64,63-Palette[Fore_color].B,Chaine,234);
  }
}



void Palette_Reafficher_jauges(struct Fenetre_Bouton_scroller * Jauge_rouge,
                               struct Fenetre_Bouton_scroller * Jauge_verte,
                               struct Fenetre_Bouton_scroller * Jauge_bleue,
                               T_Palette Palette,byte Debut,byte Fin)
{
  byte Couleur;
  char Chaine[3];

  Effacer_curseur();
  // R�affichage des jauges:
  if (Debut!=Fin)
  {
    // Dans le cas d'un bloc, tout � 0.
    Jauge_rouge->Position   =63;
    Fenetre_Dessiner_jauge(Jauge_rouge);
    Print_dans_fenetre(176,172,"� 0",CM_Noir,CM_Clair);

    Jauge_verte->Position   =63;
    Fenetre_Dessiner_jauge(Jauge_verte);
    Print_dans_fenetre(203,172,"� 0",CM_Noir,CM_Clair);

    Jauge_bleue->Position   =63;
    Fenetre_Dessiner_jauge(Jauge_bleue);
    Print_dans_fenetre(230,172,"� 0",CM_Noir,CM_Clair);
  }
  else
  {
    // Dans le cas d'une seule couleur, composantes.
    Couleur=Palette[Debut].R;
    Num2str(Couleur,Chaine,2);
    Jauge_rouge->Position=63-Couleur;
    Fenetre_Dessiner_jauge(Jauge_rouge);
    Print_dans_fenetre(180,172,Chaine,CM_Noir,CM_Clair);

    Couleur=Palette[Debut].V;
    Num2str(Couleur,Chaine,2);
    Jauge_verte->Position=63-Couleur;
    Fenetre_Dessiner_jauge(Jauge_verte);
    Print_dans_fenetre(207,172,Chaine,CM_Noir,CM_Clair);

    Couleur=Palette[Debut].B;
    Num2str(Couleur,Chaine,2);
    Jauge_bleue->Position=63-Couleur;
    Fenetre_Dessiner_jauge(Jauge_bleue);
    Print_dans_fenetre(234,172,Chaine,CM_Noir,CM_Clair);
  }
  Afficher_curseur();
}


void Bouton_Palette(void)
{
  static short Indice_Reduction_palette=0;
  static short Reduce_Nb_couleurs=256;
  short  Couleur_temporaire; // Variable pouvant reservir pour diff�rents calculs interm�diaires
  dword  Temp;
  byte   Couleur,Click; // Variables pouvant reservir pour diff�rents calculs interm�diaires
  short  Bouton_clicke;
  word   Ancien_Mouse_X;
  word   Ancien_Mouse_Y;
  byte   Ancien_Mouse_K;
  byte   Debut_block;
  byte   Fin_block;
  byte   Premiere_couleur;
  byte   Derniere_couleur;
  char   Chaine[10];
  word   i;
  //short  Pos_X,Pos_Y;
  struct Fenetre_Bouton_normal   * Bouton_Used;
  struct Fenetre_Bouton_scroller * Jauge_rouge;
  struct Fenetre_Bouton_scroller * Jauge_verte;
  struct Fenetre_Bouton_scroller * Jauge_bleue;
  struct Fenetre_Bouton_scroller * Jauge_Reduction;
  byte   Backup_de_l_image_effectue=0;
  byte   Il_faut_remapper=0;

  dword  Utilisation_couleur[256];
  short  Nb_couleurs_utilisees=-1; // -1 <=> Inconnu
  byte   Table_de_conversion[256];

  struct Composantes * Palette_backup;
  struct Composantes * Palette_temporaire;
  struct Composantes * Palette_de_travail;


  Palette_backup    =(struct Composantes *)malloc(sizeof(T_Palette));
  Palette_temporaire=(struct Composantes *)malloc(sizeof(T_Palette));
  Palette_de_travail=(struct Composantes *)malloc(sizeof(T_Palette));

  Ouvrir_fenetre(299,188,"Palette");

  memcpy(Palette_de_travail,Principal_Palette,sizeof(T_Palette));
  memcpy(Palette_backup    ,Principal_Palette,sizeof(T_Palette));
  memcpy(Palette_temporaire,Principal_Palette,sizeof(T_Palette));

  Fenetre_Definir_bouton_palette(5,79);                              // 1

  Fenetre_Afficher_cadre      (173, 67,121,116);
  Fenetre_Afficher_cadre      (128, 16, 91, 39);
  Fenetre_Afficher_cadre      (221, 16, 73, 39);
  // Cadre creux destin� � l'affichage de la(les) couleur(s) s�lectionn�e(s)
  Fenetre_Afficher_cadre_creux(259, 88, 26, 74);

  // Graduation des jauges de couleur
  Block(Fenetre_Pos_X+(Menu_Facteur_X*179),Fenetre_Pos_Y+(Menu_Facteur_Y*109),Menu_Facteur_X*17,Menu_Facteur_Y,CM_Fonce);
  Block(Fenetre_Pos_X+(Menu_Facteur_X*206),Fenetre_Pos_Y+(Menu_Facteur_Y*109),Menu_Facteur_X*17,Menu_Facteur_Y,CM_Fonce);
  Block(Fenetre_Pos_X+(Menu_Facteur_X*233),Fenetre_Pos_Y+(Menu_Facteur_Y*109),Menu_Facteur_X*17,Menu_Facteur_Y,CM_Fonce);
  Block(Fenetre_Pos_X+(Menu_Facteur_X*179),Fenetre_Pos_Y+(Menu_Facteur_Y*125),Menu_Facteur_X*17,Menu_Facteur_Y,CM_Fonce);
  Block(Fenetre_Pos_X+(Menu_Facteur_X*206),Fenetre_Pos_Y+(Menu_Facteur_Y*125),Menu_Facteur_X*17,Menu_Facteur_Y,CM_Fonce);
  Block(Fenetre_Pos_X+(Menu_Facteur_X*233),Fenetre_Pos_Y+(Menu_Facteur_Y*125),Menu_Facteur_X*17,Menu_Facteur_Y,CM_Fonce);
  Block(Fenetre_Pos_X+(Menu_Facteur_X*179),Fenetre_Pos_Y+(Menu_Facteur_Y*141),Menu_Facteur_X*17,Menu_Facteur_Y,CM_Fonce);
  Block(Fenetre_Pos_X+(Menu_Facteur_X*206),Fenetre_Pos_Y+(Menu_Facteur_Y*141),Menu_Facteur_X*17,Menu_Facteur_Y,CM_Fonce);
  Block(Fenetre_Pos_X+(Menu_Facteur_X*233),Fenetre_Pos_Y+(Menu_Facteur_Y*141),Menu_Facteur_X*17,Menu_Facteur_Y,CM_Fonce);
  // Jauges de couleur
  Fenetre_Definir_bouton_scroller(182, 81, 88,64,1,63-Palette_de_travail[Fore_color].R);// 2
  Jauge_rouge=Fenetre_Liste_boutons_scroller;
  Fenetre_Definir_bouton_scroller(209, 81, 88,64,1,63-Palette_de_travail[Fore_color].V);// 3
  Jauge_verte=Fenetre_Liste_boutons_scroller;
  Fenetre_Definir_bouton_scroller(236, 81, 88,64,1,63-Palette_de_travail[Fore_color].B);// 4
  Jauge_bleue=Fenetre_Liste_boutons_scroller;
  Print_dans_fenetre(184,71,"R",CM_Fonce,CM_Clair);
  Print_dans_fenetre(211,71,"G",CM_Fonce,CM_Clair);
  Print_dans_fenetre(238,71,"B",CM_Fonce,CM_Clair);

  Premiere_couleur=Derniere_couleur=Debut_block=Fin_block=Fore_color;
  Tagger_intervalle_palette(Debut_block,Fin_block);

  // Affichage dans le block de visu de la couleur en cours
  Block(Fenetre_Pos_X+(Menu_Facteur_X*260),Fenetre_Pos_Y+(Menu_Facteur_Y*89),Menu_Facteur_X*24,Menu_Facteur_Y*72,Back_color);
  Block(Fenetre_Pos_X+(Menu_Facteur_X*264),Fenetre_Pos_Y+(Menu_Facteur_Y*93),Menu_Facteur_X<<4,Menu_Facteur_Y*64,Fore_color);

  // Affichage des valeurs de la couleur courante (pour 1 couleur)
  Num2str(Principal_Palette[Fore_color].R,Chaine,2);
  Print_dans_fenetre(180,172,Chaine,CM_Noir,CM_Clair);
  Num2str(Principal_Palette[Fore_color].V,Chaine,2);
  Print_dans_fenetre(207,172,Chaine,CM_Noir,CM_Clair);
  Num2str(Principal_Palette[Fore_color].B,Chaine,2);
  Print_dans_fenetre(234,172,Chaine,CM_Noir,CM_Clair);

  Print_dans_fenetre(129,58,"Color number:",CM_Fonce,CM_Clair);
  Num2str(Fore_color,Chaine,3);
  Print_dans_fenetre(237,58,Chaine,CM_Noir,CM_Clair);


  Fenetre_Definir_bouton_normal( 6,17,59,14,"Default",3,1,SDLK_f);   // 5
  Fenetre_Definir_bouton_normal(66,17,29,14,"Gry"    ,1,1,SDLK_g);   // 6
  Fenetre_Definir_bouton_normal( 6,47,59,14,"Swap"   ,1,1,SDLK_s);   // 7
  Fenetre_Definir_bouton_normal(66,47,59,14,"X-Swap" ,1,1,SDLK_x);   // 8
  Fenetre_Definir_bouton_normal( 6,62,59,14,"Copy"   ,1,1,SDLK_c);   // 9
  Fenetre_Definir_bouton_normal(66,62,59,14,"Spread" ,4,1,SDLK_e);   // 10

  Fenetre_Definir_bouton_normal(239,20,51,14,"Reduce" ,1,1,SDLK_r);  // 11
  Print_dans_fenetre(241,41,"to",CM_Fonce,CM_Clair);

  Fenetre_Definir_bouton_normal(  6,168,35,14,"Undo"  ,1,1,SDLK_u);  // 12
  Fenetre_Definir_bouton_normal( 62,168,51,14,"Cancel",0,1,SDLK_ESCAPE);  // 13
  Fenetre_Definir_bouton_normal(117,168,51,14,"OK"    ,0,1,SDLK_RETURN);  // 14

  Fenetre_Definir_bouton_normal(132,20,83,14,"Used: ???",4,1,SDLK_d);// 15
  Bouton_Used=Fenetre_Liste_boutons_normal;
  Fenetre_Definir_bouton_normal(132,37,83,14,"Zap unused",0,1,SDLK_DELETE);//16

  // Jauge de r�duction de palette
  Fenetre_Definir_bouton_scroller(225,20,31,7,1,Indice_Reduction_palette);// 17
  Jauge_Reduction=Fenetre_Liste_boutons_scroller;

  Fenetre_Definir_bouton_normal(266, 74,12,11,"+",0,1,SDLK_KP_PLUS);       // 18
  Fenetre_Definir_bouton_normal(266,165,12,11,"-",0,1,SDLK_KP_MINUS);       // 19

  Fenetre_Definir_bouton_normal(96,17,29,14,"Neg"    ,1,1,SDLK_n);   // 20
  Fenetre_Definir_bouton_normal( 6,32,59,14,"Invert" ,1,1,SDLK_i);   // 21
  Fenetre_Definir_bouton_normal(66,32,59,14,"X-Inv." ,5,1,SDLK_v);   // 22

  Fenetre_Definir_bouton_saisie(263,39,3);                           // 23
  // Affichage du facteur de r�duction de la palette
  Num2str(Reduce_Nb_couleurs,Chaine,3);
  Print_dans_fenetre(265,41,Chaine,CM_Noir,CM_Clair);

  // Dessin des petits effets sp�ciaux pour les boutons [+] et [-]
  Dessiner_zigouigoui(263, 74,CM_Blanc,-1);
  Dessiner_zigouigoui(280, 74,CM_Blanc,+1);
  Dessiner_zigouigoui(263,165,CM_Fonce,-1);
  Dessiner_zigouigoui(280,165,CM_Fonce,+1);

  Afficher_curseur();

  Clavier_americain(); // On est oblig� de rester en clavier am�ricain pour
                       // que l'on puisse d�caler la couleur s�lectionn�e �
                       // l'aide des touche "^" et "$" ("[" et "]").

  if (Config.Auto_nb_used)
    Compter_nb_couleurs_utilisees(&Nb_couleurs_utilisees,Utilisation_couleur);

  do
  {
    Ancien_Mouse_X=Mouse_X;
    Ancien_Mouse_Y=Mouse_Y;
    Ancien_Mouse_K=Mouse_K;
    Bouton_clicke=Fenetre_Bouton_clicke();

    switch (Bouton_clicke)
    {
      case  0 : // Nulle part
        break;
      case -1 : // Hors de la fen�tre
      case  1 : // Palette
        if ( (Mouse_X!=Ancien_Mouse_X) || (Mouse_Y!=Ancien_Mouse_Y) || (Mouse_K!=Ancien_Mouse_K) )
        {
          Effacer_curseur();
          Couleur_temporaire=Lit_pixel(Mouse_X,Mouse_Y);
          if (Mouse_K==A_DROITE)
          {
            if (Back_color!=Couleur_temporaire)
            {
              Back_color=Couleur_temporaire;
              // 4 blocks de back_color entourant la fore_color
              Block(Fenetre_Pos_X+(Menu_Facteur_X*260),Fenetre_Pos_Y+(Menu_Facteur_Y* 89),Menu_Facteur_X*24,Menu_Facteur_Y<<2,Back_color);
              Block(Fenetre_Pos_X+(Menu_Facteur_X*260),Fenetre_Pos_Y+(Menu_Facteur_Y*157),Menu_Facteur_X*24,Menu_Facteur_Y<<2,Back_color);
              Block(Fenetre_Pos_X+(Menu_Facteur_X*260),Fenetre_Pos_Y+(Menu_Facteur_Y* 93),Menu_Facteur_X<<2,Menu_Facteur_Y<<6,Back_color);
              Block(Fenetre_Pos_X+(Menu_Facteur_X*280),Fenetre_Pos_Y+(Menu_Facteur_Y* 93),Menu_Facteur_X<<2,Menu_Facteur_Y<<6,Back_color);
            }
          }
          else
          {
            if (!Ancien_Mouse_K)
            {
              // On vient de clicker sur une couleur (et une seule)
              if ( (Fore_color!=Couleur_temporaire) || (Debut_block!=Fin_block) )
              {
                // La couleur en question est nouvelle ou elle annule un
                // ancien bloc. Il faut donc s�lectionner cette couleur comme
                // unique couleur choisie.

                Fore_color=Premiere_couleur=Derniere_couleur=Debut_block=Fin_block=Couleur_temporaire;
                Tagger_intervalle_palette(Debut_block,Fin_block);

                // Affichage du n� de la couleur s�lectionn�e
                Block(Fenetre_Pos_X+(Menu_Facteur_X*261),Fenetre_Pos_Y+(Menu_Facteur_Y*58),Menu_Facteur_X*32,Menu_Facteur_Y*7,CM_Clair);
                Num2str(Fore_color,Chaine,3);
                Print_dans_fenetre(237,58,Chaine,CM_Noir,CM_Clair);

                // Affichage des jauges
                Block(Fenetre_Pos_X+(Menu_Facteur_X*176),Fenetre_Pos_Y+(Menu_Facteur_Y*172),Menu_Facteur_X*84,Menu_Facteur_Y*7,CM_Clair);
                Afficher_les_jauges(Jauge_rouge,Jauge_verte,Jauge_bleue,0,Palette_de_travail);

                // Affichage dans le block de visu de la couleur en cours
                Block(Fenetre_Pos_X+(Menu_Facteur_X*264),Fenetre_Pos_Y+(Menu_Facteur_Y*93),Menu_Facteur_X<<4,Menu_Facteur_Y*64,Fore_color);

                memcpy(Palette_backup    ,Palette_de_travail,sizeof(T_Palette));
                memcpy(Palette_temporaire,Palette_de_travail,sizeof(T_Palette));
              }
            }
            else
            {
              // On maintient le click, on va donc tester si le curseur bouge
              if (Couleur_temporaire!=Derniere_couleur)
              {
                // On commence par ordonner la 1�re et derni�re couleur du bloc
                if (Premiere_couleur<Couleur_temporaire)
                {
                  Debut_block=Premiere_couleur;
                  Fin_block=Couleur_temporaire;

                  // Affichage du n� de la couleur s�lectionn�e
                  Num2str(Debut_block,Chaine  ,3);
                  Num2str(Fin_block  ,Chaine+4,3);
                  Chaine[3]=26; // Fl�che vers la droite
                  Print_dans_fenetre(237,58,Chaine,CM_Noir,CM_Clair);

                  // Affichage des jauges
                  Afficher_les_jauges(Jauge_rouge,Jauge_verte,Jauge_bleue,1,NULL);

                  // Affichage dans le block de visu du bloc (d�grad�) en cours
                  Bloc_degrade_dans_fenetre(264,93,Debut_block,Fin_block);
                }
                else if (Premiere_couleur>Couleur_temporaire)
                {
                  Debut_block=Couleur_temporaire;
                  Fin_block=Premiere_couleur;

                  // Affichage du n� de la couleur s�lectionn�e
                  Num2str(Debut_block,Chaine  ,3);
                  Num2str(Fin_block  ,Chaine+4,3);
                  Chaine[3]=26; // Fl�che vers la droite
                  Print_dans_fenetre(237,58,Chaine,CM_Noir,CM_Clair);

                  // Affichage des jauges
                  Afficher_les_jauges(Jauge_rouge,Jauge_verte,Jauge_bleue,1,NULL);

                  // Affichage dans le block de visu du bloc (d�grad�) en cours
                  Bloc_degrade_dans_fenetre(264,93,Debut_block,Fin_block);
                }
                else
                {
                  Debut_block=Fin_block=Premiere_couleur;
                  Block(Fenetre_Pos_X+(Menu_Facteur_X*176),Fenetre_Pos_Y+(Menu_Facteur_Y*172),Menu_Facteur_X*84,Menu_Facteur_Y*7,CM_Clair);

                  // Affichage du n� de la couleur s�lectionn�e
                  Block(Fenetre_Pos_X+(Menu_Facteur_X*261),Fenetre_Pos_Y+(Menu_Facteur_Y*58),Menu_Facteur_X*32,Menu_Facteur_Y*7,CM_Clair);
                  Num2str(Fore_color,Chaine,3);
                  Print_dans_fenetre(237,58,Chaine,CM_Noir,CM_Clair);

                  // Affichage des jauges
                  Afficher_les_jauges(Jauge_rouge,Jauge_verte,Jauge_bleue,0,Palette_de_travail);

                  // Affichage dans le block de visu de la couleur en cours
                  Block(Fenetre_Pos_X+(Menu_Facteur_X*264),Fenetre_Pos_Y+(Menu_Facteur_Y*93),Menu_Facteur_X<<4,Menu_Facteur_Y*64,Fore_color);
                }

                // On tagge le bloc (ou la couleur)
                Tagger_intervalle_palette(Debut_block,Fin_block);
              }

              Derniere_couleur=Couleur_temporaire;
            }
          }
          Afficher_curseur();
        }
        break;
      case  2 : // Jauge rouge
        Effacer_curseur();
        if (Debut_block==Fin_block)
        {
          Modifier_Rouge(Fore_color,63-Jauge_rouge->Position,Palette_de_travail);
          Num2str(Palette_de_travail[Fore_color].R,Chaine,2);
          Print_dans_fenetre(180,172,Chaine,CM_Noir,CM_Clair);
        }
        else
        {
          for (i=Debut_block; i<=Fin_block; i++)
            Modifier_Rouge(i,Palette_temporaire[i].R+63-Jauge_rouge->Position,Palette_de_travail);

          if (Jauge_rouge->Position>63)
          {
            // Jauge dans les n�gatifs:
            Num2str(-(63-Jauge_rouge->Position),Chaine,3);
            Chaine[0]='-';
          }
          else if (Jauge_rouge->Position<63)
          {
            // Jauge dans les positifs:
            Num2str(  63-Jauge_rouge->Position ,Chaine,3);
            Chaine[0]='+';
          }
          else
          {
            // Jauge nulle:
            strcpy(Chaine,"� 0");
          }
          Print_dans_fenetre(176,172,Chaine,CM_Noir,CM_Clair);
        }

        Il_faut_remapper=1;

        Afficher_curseur();
        break;
      case  3 : // Jauge verte
        Effacer_curseur();
        if (Debut_block==Fin_block)
        {
          Modifier_Vert (Fore_color,63-Jauge_verte->Position,Palette_de_travail);
          Num2str(Palette_de_travail[Fore_color].V,Chaine,2);
          Print_dans_fenetre(207,172,Chaine,CM_Noir,CM_Clair);
        }
        else
        {
          for (i=Debut_block; i<=Fin_block; i++)
            Modifier_Vert (i,Palette_temporaire[i].V+63-Jauge_verte->Position,Palette_de_travail);

          if (Jauge_verte->Position>63)
          {
            // Jauge dans les n�gatifs:
            Num2str(-(63-Jauge_verte->Position),Chaine,3);
            Chaine[0]='-';
          }
          else if (Jauge_verte->Position<63)
          {
            // Jauge dans les positifs:
            Num2str(  63-Jauge_verte->Position ,Chaine,3);
            Chaine[0]='+';
          }
          else
          {
            // Jauge nulle:
            strcpy(Chaine,"� 0");
          }
          Print_dans_fenetre(203,172,Chaine,CM_Noir,CM_Clair);
        }

        Il_faut_remapper=1;

        Afficher_curseur();
        break;
      case  4 : // Jauge bleue
        Effacer_curseur();
        if (Debut_block==Fin_block)
        {
          Modifier_Bleu (Fore_color,63-Jauge_bleue->Position,Palette_de_travail);
          Num2str(Palette_de_travail[Fore_color].B,Chaine,2);
          Print_dans_fenetre(234,172,Chaine,CM_Noir,CM_Clair);
        }
        else
        {
          for (i=Debut_block; i<=Fin_block; i++)
            Modifier_Bleu(i,Palette_temporaire[i].B+63-Jauge_bleue->Position,Palette_de_travail);

          if (Jauge_bleue->Position>63)
          {
            // Jauge dans les n�gatifs:
            Num2str(-(63-Jauge_bleue->Position),Chaine,3);
            Chaine[0]='-';
          }
          else if (Jauge_bleue->Position<63)
          {
            // Jauge dans les positifs:
            Num2str(  63-Jauge_bleue->Position ,Chaine,3);
            Chaine[0]='+';
          }
          else
          {
            // Jauge nulle:
            strcpy(Chaine,"� 0");
          }
          Print_dans_fenetre(230,172,Chaine,CM_Noir,CM_Clair);
        }

        Il_faut_remapper=1;

        Afficher_curseur();
        break;

      case 5 : // Default
        memcpy(Palette_backup,Palette_de_travail,sizeof(T_Palette));
        memcpy(Palette_de_travail,Palette_defaut,sizeof(T_Palette));
        memcpy(Palette_temporaire,Palette_defaut,sizeof(T_Palette));
        Set_palette(Palette_defaut);
        Palette_Reafficher_jauges(Jauge_rouge,Jauge_verte,Jauge_bleue,Palette_de_travail,Debut_block,Fin_block);
        // On pr�pare la "modifiabilit�" des nouvelles couleurs
        memcpy(Palette_temporaire,Palette_de_travail,sizeof(T_Palette));

        Il_faut_remapper=1;
        break;

      case 6 : // Grey scale
        // Backup
        memcpy(Palette_backup,Palette_de_travail,sizeof(T_Palette));
        // Grey Scale
        for (i=Debut_block;i<=Fin_block;i++)
        {
          Couleur_temporaire=(dword)( ((dword)Palette_de_travail[i].R*30) + ((dword)Palette_de_travail[i].V*59) + ((dword)Palette_de_travail[i].B*11) )/100;
          Modifier_Rouge(i,Couleur_temporaire,Palette_de_travail);
          Modifier_Vert (i,Couleur_temporaire,Palette_de_travail);
          Modifier_Bleu (i,Couleur_temporaire,Palette_de_travail);
        }
        Palette_Reafficher_jauges(Jauge_rouge,Jauge_verte,Jauge_bleue,Palette_de_travail,Debut_block,Fin_block);
        // On pr�pare la "modifiabilit�" des nouvelles couleurs
        memcpy(Palette_temporaire,Palette_de_travail,sizeof(T_Palette));

        Il_faut_remapper=1;
        break;

      case  7 : // Swap
      case  8 : // X-Swap
        Couleur_temporaire=Attendre_click_dans_palette(Fenetre_Liste_boutons_palette);
        if ((Couleur_temporaire>=0)
         && (Couleur_temporaire!=Debut_block))
        {
          Effacer_curseur();
          memcpy(Palette_backup,Palette_de_travail,sizeof(T_Palette));

          // On calcule le nombre de couleurs a swapper sans risquer de sortir
          // de la palette (La var. Premiere_couleur est utilis�e pour �conomiser 1 var; c'est tout)
          Premiere_couleur=(Couleur_temporaire+Fin_block-Debut_block<=255)?Fin_block+1-Debut_block:256-Couleur_temporaire;

          if (Bouton_clicke==8) // On ne fait de backup de l'image que si on
                                // est en mode X-SWAP.
            if (!Backup_de_l_image_effectue)
            {
              Backup();
              Backup_de_l_image_effectue=1;
            }

          Swap(Bouton_clicke==8,Debut_block,Couleur_temporaire,Premiere_couleur,Palette_de_travail,Utilisation_couleur);

          memcpy(Palette_temporaire,Palette_de_travail,sizeof(T_Palette));

          // On d�place le bloc vers les modifs:
          Derniere_couleur=Fin_block=Couleur_temporaire+Premiere_couleur-1;
          Fore_color=Premiere_couleur=Debut_block=Couleur_temporaire;
          // On raffiche le n� des bornes du bloc:
          if (Debut_block!=Fin_block)
          {
            // Cas d'un bloc multi-couleur
            Num2str(Debut_block,Chaine  ,3);
            Num2str(Fin_block  ,Chaine+4,3);
            Chaine[3]=26; // Fl�che vers la droite
            // Affichage dans le block de visu du bloc (d�grad�) en cours
            Bloc_degrade_dans_fenetre(264,93,Debut_block,Fin_block);
          }
          else
          {
            // Cas d'une seule couleur
            Num2str(Fore_color,Chaine,3);
            Block(Fenetre_Pos_X+(Menu_Facteur_X*237),Fenetre_Pos_Y+(Menu_Facteur_Y*58),Menu_Facteur_X*56,Menu_Facteur_Y* 7,CM_Clair);
            // Affichage dans le block de visu de la couleur en cours
            Block(Fenetre_Pos_X+(Menu_Facteur_X*264),Fenetre_Pos_Y+(Menu_Facteur_Y*93),Menu_Facteur_X<<4,Menu_Facteur_Y*64,Fore_color);
          }
          Print_dans_fenetre(237,58,Chaine,CM_Noir,CM_Clair);
          // On tag le bloc (ou la couleur)
          Tagger_intervalle_palette(Debut_block,Fin_block);

          Il_faut_remapper=1;

          Afficher_curseur();
          Palette_Reafficher_jauges(Jauge_rouge,Jauge_verte,Jauge_bleue,Palette_de_travail,Debut_block,Fin_block);
        }
        break;

      case  9 : // Copy
        Couleur_temporaire=Attendre_click_dans_palette(Fenetre_Liste_boutons_palette);
        if ((Couleur_temporaire>=0) && (Couleur_temporaire!=Debut_block))
        {
          Effacer_curseur();
          memcpy(Palette_backup,Palette_de_travail,sizeof(T_Palette));
          memcpy(Palette_de_travail+Couleur_temporaire,Palette_backup+Debut_block,
                 ((Couleur_temporaire+Fin_block-Debut_block<=255)?Fin_block+1-Debut_block:256-Couleur_temporaire)*3);
          memcpy(Palette_temporaire,Palette_de_travail,sizeof(T_Palette));
          Set_palette(Palette_de_travail);
          // On d�place le bloc vers les modifs:
          Derniere_couleur=Fin_block=((Couleur_temporaire+Fin_block-Debut_block<=255)?(Couleur_temporaire+Fin_block-Debut_block):255);
          Fore_color=Premiere_couleur=Debut_block=Couleur_temporaire;
          // On raffiche le n� des bornes du bloc:
          if (Debut_block!=Fin_block)
          {
            // Cas d'un bloc multi-couleur
            Num2str(Debut_block,Chaine  ,3);
            Num2str(Fin_block  ,Chaine+4,3);
            Chaine[3]=26; // Fl�che vers la droite
            // Affichage dans le block de visu du bloc (d�grad�) en cours
            Bloc_degrade_dans_fenetre(264,93,Debut_block,Fin_block);
          }
          else
          {
            // Cas d'une seule couleur
            Num2str(Fore_color,Chaine,3);
            Block(Fenetre_Pos_X+(Menu_Facteur_X*237),Fenetre_Pos_Y+(Menu_Facteur_Y*58),Menu_Facteur_X*56,Menu_Facteur_Y* 7,CM_Clair);
            // Affichage dans le block de visu de la couleur en cours
            Block(Fenetre_Pos_X+(Menu_Facteur_X*264),Fenetre_Pos_Y+(Menu_Facteur_Y*93),Menu_Facteur_X<<4,Menu_Facteur_Y*64,Fore_color);
          }
          Print_dans_fenetre(237,58,Chaine,CM_Noir,CM_Clair);
          // On tag le bloc (ou la couleur)
          Tagger_intervalle_palette(Debut_block,Fin_block);

          Il_faut_remapper=1;

          Afficher_curseur();
          Palette_Reafficher_jauges(Jauge_rouge,Jauge_verte,Jauge_bleue,Palette_de_travail,Debut_block,Fin_block);
        }
        break;

      case 10 : // Spread
        if (Debut_block!=Fin_block)
        {
          memcpy(Palette_backup,Palette_de_travail,sizeof(T_Palette));
          Degrader_palette(Debut_block,Fin_block,Palette_de_travail);
        }
        else
        {
          Couleur_temporaire=Attendre_click_dans_palette(Fenetre_Liste_boutons_palette);
          if (Couleur_temporaire>=0)
          {
            memcpy(Palette_backup,Palette_de_travail,sizeof(T_Palette));
            if (Couleur_temporaire<Fore_color)
              Degrader_palette(Couleur_temporaire,Fore_color,Palette_de_travail);
            else
              Degrader_palette(Fore_color,Couleur_temporaire,Palette_de_travail);
          }
        }

        Palette_Reafficher_jauges(Jauge_rouge,Jauge_verte,Jauge_bleue,Palette_de_travail,Debut_block,Fin_block);
        // On pr�pare la "modifiabilit�" des nouvelles couleurs
        memcpy(Palette_temporaire,Palette_de_travail,sizeof(T_Palette));

        Il_faut_remapper=1;
        break;

      case 11: // Reduce
        memcpy(Palette_backup,Palette_de_travail,sizeof(T_Palette));
        if (!Backup_de_l_image_effectue)
        {
          Backup();
          Backup_de_l_image_effectue=1;
        }
        Reduce_palette(&Nb_couleurs_utilisees,Reduce_Nb_couleurs,Palette_de_travail,Utilisation_couleur);

        if ((Config.Safety_colors) && (Nb_couleurs_utilisees<4))
        {
          memcpy(Palette_temporaire,Principal_Palette,sizeof(T_Palette));
          memcpy(Principal_Palette,Palette_de_travail,sizeof(T_Palette));
          Remettre_proprement_les_couleurs_du_menu(Utilisation_couleur);
          memcpy(Palette_de_travail,Principal_Palette,sizeof(T_Palette));
          memcpy(Principal_Palette,Palette_temporaire,sizeof(T_Palette));
        }

        Set_palette(Palette_de_travail);     // On d�finit la nouvelle palette
        Palette_Reafficher_jauges(Jauge_rouge,Jauge_verte,Jauge_bleue,Palette_de_travail,Debut_block,Fin_block);
        memcpy(Palette_temporaire,Palette_de_travail,sizeof(T_Palette));

        Il_faut_remapper=1;
        break;

      case 12: // Undo
        memcpy(Palette_temporaire,Palette_backup    ,sizeof(T_Palette));
        memcpy(Palette_backup    ,Palette_de_travail,sizeof(T_Palette));
        memcpy(Palette_de_travail,Palette_temporaire,sizeof(T_Palette));
        Palette_Reafficher_jauges(Jauge_rouge,Jauge_verte,Jauge_bleue,Palette_de_travail,Debut_block,Fin_block);
        Set_palette(Palette_de_travail);

        Il_faut_remapper=1;
        break;

      case 15 : // Used
        if (Nb_couleurs_utilisees==-1)
          Compter_nb_couleurs_utilisees(&Nb_couleurs_utilisees,Utilisation_couleur);
        break;

      case 16 : // Zap unused
        memcpy(Palette_backup,Palette_de_travail,sizeof(T_Palette));
        if (Nb_couleurs_utilisees==-1)
          Compter_nb_couleurs_utilisees(&Nb_couleurs_utilisees,Utilisation_couleur);
        for (i=0; i<256; i++)
        {
          if (!Utilisation_couleur[i])
          {
            Couleur_temporaire=Debut_block+(i % (Fin_block+1-Debut_block));
            Palette_de_travail[i].R=Palette_backup[Couleur_temporaire].R;
            Palette_de_travail[i].V=Palette_backup[Couleur_temporaire].V;
            Palette_de_travail[i].B=Palette_backup[Couleur_temporaire].B;
          }
        }

        if ((Config.Safety_colors) && (Nb_couleurs_utilisees<4) && (Fin_block==Debut_block))
        {
          memcpy(Palette_temporaire,Principal_Palette,sizeof(T_Palette));
          memcpy(Principal_Palette,Palette_de_travail,sizeof(T_Palette));
          Remettre_proprement_les_couleurs_du_menu(Utilisation_couleur);
          memcpy(Palette_de_travail,Principal_Palette,sizeof(T_Palette));
          memcpy(Principal_Palette,Palette_temporaire,sizeof(T_Palette));
        }

        Set_palette(Palette_de_travail);
        Palette_Reafficher_jauges(Jauge_rouge,Jauge_verte,Jauge_bleue,Palette_de_travail,Debut_block,Fin_block);

        Il_faut_remapper=1;
        break;

      case 17 : // Jauge de r�duction de palette
        if (Indice_Reduction_palette!=Jauge_Reduction->Position)
        {
          Indice_Reduction_palette=Jauge_Reduction->Position;
          // Affichage du facteur de r�duction de la palette
          Effacer_curseur();
          Print_dans_fenetre(265,41,Libelle_reduction_palette[Indice_Reduction_palette],CM_Noir,CM_Clair);
          Afficher_curseur();
          Reduce_Nb_couleurs=atoi(Libelle_reduction_palette[Indice_Reduction_palette]);
        }
        break;

      case 18 : // [+]
        Effacer_curseur();
        if (Debut_block==Fin_block)
        {
          if (Jauge_rouge->Position)
          {
            (Jauge_rouge->Position)--;
            Fenetre_Dessiner_jauge(Jauge_rouge);
            Modifier_Rouge(Fore_color,63-Jauge_rouge->Position,Palette_de_travail);
            Num2str(Palette_de_travail[Fore_color].R,Chaine,2);
            Print_dans_fenetre(180,172,Chaine,CM_Noir,CM_Clair);
          }
          if (Jauge_verte->Position)
          {
            (Jauge_verte->Position)--;
            Fenetre_Dessiner_jauge(Jauge_verte);
            Modifier_Vert (Fore_color,63-Jauge_verte->Position,Palette_de_travail);
            Num2str(Palette_de_travail[Fore_color].V,Chaine,2);
            Print_dans_fenetre(207,172,Chaine,CM_Noir,CM_Clair);
          }
          if (Jauge_bleue->Position)
          {
            (Jauge_bleue->Position)--;
            Fenetre_Dessiner_jauge(Jauge_bleue);
            Modifier_Bleu (Fore_color,63-Jauge_bleue->Position,Palette_de_travail);
            Num2str(Palette_de_travail[Fore_color].B,Chaine,2);
            Print_dans_fenetre(234,172,Chaine,CM_Noir,CM_Clair);
          }
        }
        else
        {
          if (Jauge_rouge->Position)
          {
            (Jauge_rouge->Position)--;
            Fenetre_Dessiner_jauge(Jauge_rouge);
          }
          if (Jauge_verte->Position)
          {
            (Jauge_verte->Position)--;
            Fenetre_Dessiner_jauge(Jauge_verte);
          }
          if (Jauge_bleue->Position)
          {
            (Jauge_bleue->Position)--;
            Fenetre_Dessiner_jauge(Jauge_bleue);
          }

          for (i=Debut_block; i<=Fin_block; i++)
          {
            Modifier_Rouge(i,Palette_temporaire[i].R+63-Jauge_rouge->Position,Palette_de_travail);
            Modifier_Vert (i,Palette_temporaire[i].V+63-Jauge_verte->Position,Palette_de_travail);
            Modifier_Bleu (i,Palette_temporaire[i].B+63-Jauge_bleue->Position,Palette_de_travail);
          }

          // -- Rouge --
          if (Jauge_rouge->Position>63)
          {
            // Jauge dans les n�gatifs:
            Num2str(-(63-Jauge_rouge->Position),Chaine,3);
            Chaine[0]='-';
          }
          else if (Jauge_rouge->Position<63)
          {
            // Jauge dans les positifs:
            Num2str(  63-Jauge_rouge->Position ,Chaine,3);
            Chaine[0]='+';
          }
          else
          {
            // Jauge nulle:
            strcpy(Chaine,"� 0");
          }
          Print_dans_fenetre(176,172,Chaine,CM_Noir,CM_Clair);


          // -- Vert --
          if (Jauge_verte->Position>63)
          {
            // Jauge dans les n�gatifs:
            Num2str(-(63-Jauge_verte->Position),Chaine,3);
            Chaine[0]='-';
          }
          else if (Jauge_verte->Position<63)
          {
            // Jauge dans les positifs:
            Num2str(  63-Jauge_verte->Position ,Chaine,3);
            Chaine[0]='+';
          }
          else
          {
            // Jauge nulle:
            strcpy(Chaine,"� 0");
          }
          Print_dans_fenetre(203,172,Chaine,CM_Noir,CM_Clair);


          // -- Bleu --
          if (Jauge_bleue->Position>63)
          {
            // Jauge dans les n�gatifs:
            Num2str(-(63-Jauge_bleue->Position),Chaine,3);
            Chaine[0]='-';
          }
          else if (Jauge_bleue->Position<63)
          {
            // Jauge dans les positifs:
            Num2str(  63-Jauge_bleue->Position ,Chaine,3);
            Chaine[0]='+';
          }
          else
          {
            // Jauge nulle:
            strcpy(Chaine,"� 0");
          }
          Print_dans_fenetre(230,172,Chaine,CM_Noir,CM_Clair);
        }

        Il_faut_remapper=1;

        Afficher_curseur();
        break;

      case 19 : // [-]
        Effacer_curseur();
        if (Debut_block==Fin_block)
        {
          if (Jauge_rouge->Position<63)
          {
            (Jauge_rouge->Position)++;
            Fenetre_Dessiner_jauge(Jauge_rouge);
            Modifier_Rouge(Fore_color,63-Jauge_rouge->Position,Palette_de_travail);
            Num2str(Palette_de_travail[Fore_color].R,Chaine,2);
            Print_dans_fenetre(180,172,Chaine,CM_Noir,CM_Clair);
          }
          if (Jauge_verte->Position<63)
          {
            (Jauge_verte->Position)++;
            Fenetre_Dessiner_jauge(Jauge_verte);
            Modifier_Vert (Fore_color,63-Jauge_verte->Position,Palette_de_travail);
            Num2str(Palette_de_travail[Fore_color].V,Chaine,2);
            Print_dans_fenetre(207,172,Chaine,CM_Noir,CM_Clair);
          }
          if (Jauge_bleue->Position<63)
          {
            (Jauge_bleue->Position)++;
            Fenetre_Dessiner_jauge(Jauge_bleue);
            Modifier_Bleu (Fore_color,63-Jauge_bleue->Position,Palette_de_travail);
            Num2str(Palette_de_travail[Fore_color].B,Chaine,2);
            Print_dans_fenetre(234,172,Chaine,CM_Noir,CM_Clair);
          }
        }
        else
        {
          if (Jauge_rouge->Position<126)
          {
            (Jauge_rouge->Position)++;
            Fenetre_Dessiner_jauge(Jauge_rouge);
          }
          if (Jauge_verte->Position<126)
          {
            (Jauge_verte->Position)++;
            Fenetre_Dessiner_jauge(Jauge_verte);
          }
          if (Jauge_bleue->Position<126)
          {
            (Jauge_bleue->Position)++;
            Fenetre_Dessiner_jauge(Jauge_bleue);
          }

          for (i=Debut_block; i<=Fin_block; i++)
          {
            Modifier_Rouge(i,Palette_temporaire[i].R+63-Jauge_rouge->Position,Palette_de_travail);
            Modifier_Vert (i,Palette_temporaire[i].V+63-Jauge_verte->Position,Palette_de_travail);
            Modifier_Bleu (i,Palette_temporaire[i].B+63-Jauge_bleue->Position,Palette_de_travail);
          }

          // -- Rouge --
          if (Jauge_rouge->Position>63)
          {
            // Jauge dans les n�gatifs:
            Num2str(-(63-Jauge_rouge->Position),Chaine,3);
            Chaine[0]='-';
          }
          else if (Jauge_rouge->Position<63)
          {
            // Jauge dans les positifs:
            Num2str(  63-Jauge_rouge->Position ,Chaine,3);
            Chaine[0]='+';
          }
          else
          {
            // Jauge nulle:
            strcpy(Chaine,"� 0");
          }
          Print_dans_fenetre(176,172,Chaine,CM_Noir,CM_Clair);


          // -- Vert --
          if (Jauge_verte->Position>63)
          {
            // Jauge dans les n�gatifs:
            Num2str(-(63-Jauge_verte->Position),Chaine,3);
            Chaine[0]='-';
          }
          else if (Jauge_verte->Position<63)
          {
            // Jauge dans les positifs:
            Num2str(  63-Jauge_verte->Position ,Chaine,3);
            Chaine[0]='+';
          }
          else
          {
            // Jauge nulle:
            strcpy(Chaine,"� 0");
          }
          Print_dans_fenetre(203,172,Chaine,CM_Noir,CM_Clair);


          // -- Bleu --
          if (Jauge_bleue->Position>63)
          {
            // Jauge dans les n�gatifs:
            Num2str(-(63-Jauge_bleue->Position),Chaine,3);
            Chaine[0]='-';
          }
          else if (Jauge_bleue->Position<63)
          {
            // Jauge dans les positifs:
            Num2str(  63-Jauge_bleue->Position ,Chaine,3);
            Chaine[0]='+';
          }
          else
          {
            // Jauge nulle:
            strcpy(Chaine,"� 0");
          }
          Print_dans_fenetre(230,172,Chaine,CM_Noir,CM_Clair);
        }

        Il_faut_remapper=1;

        Afficher_curseur();
        break;

      case 20 : // Negative
        // Backup
        memcpy(Palette_backup,Palette_de_travail,sizeof(T_Palette));
        // Negative
        for (i=Debut_block;i<=Fin_block;i++)
        {
          Modifier_Rouge(i,63-Palette_de_travail[i].R,Palette_de_travail);
          Modifier_Vert (i,63-Palette_de_travail[i].V,Palette_de_travail);
          Modifier_Bleu (i,63-Palette_de_travail[i].B,Palette_de_travail);
        }
        Palette_Reafficher_jauges(Jauge_rouge,Jauge_verte,Jauge_bleue,Palette_de_travail,Debut_block,Fin_block);
        // On pr�pare la "modifiabilit�" des nouvelles couleurs
        memcpy(Palette_temporaire,Palette_de_travail,sizeof(T_Palette));

        Il_faut_remapper=1;
        break;

      case 21 : // Inversion
      case 22 : // X-Inversion
        // Backup
        memcpy(Palette_backup,Palette_de_travail,sizeof(T_Palette));
        //   On initialise la table de conversion
        for (i=0; i<=255; i++)
          Table_de_conversion[i]=i;
        // Inversion
        for (i=Debut_block;i<=Fin_block;i++)
        {
          Couleur_temporaire=Fin_block-(i-Debut_block);
          Modifier_Rouge(i,Palette_backup[Couleur_temporaire].R,Palette_de_travail);
          Modifier_Vert (i,Palette_backup[Couleur_temporaire].V,Palette_de_travail);
          Modifier_Bleu (i,Palette_backup[Couleur_temporaire].B,Palette_de_travail);
          if (Bouton_clicke==22)
          {
            Table_de_conversion[i]=Couleur_temporaire;
            Table_de_conversion[Couleur_temporaire]=i;

            Temp=Utilisation_couleur[i];
            Utilisation_couleur[i]=Utilisation_couleur[Couleur_temporaire];
            Utilisation_couleur[Couleur_temporaire]=Temp;
          }
        }
        Palette_Reafficher_jauges(Jauge_rouge,Jauge_verte,Jauge_bleue,Palette_de_travail,Debut_block,Fin_block);
        // Si on est en X-Invert, on remap l'image (=> on fait aussi 1 backup)
        if (Bouton_clicke==22)
        {
          if (!Backup_de_l_image_effectue)
          {
            Backup();
            Backup_de_l_image_effectue=1;
          }
          Effacer_curseur();
          Remap_image_HIGH(Table_de_conversion);
          Afficher_curseur();
        }
        // On pr�pare la "modifiabilit�" des nouvelles couleurs
        memcpy(Palette_temporaire,Palette_de_travail,sizeof(T_Palette));

        Il_faut_remapper=1;
        break;

      case 23 : // Saisie du nombre de couleurs pour la r�duction de palette
        Num2str(Reduce_Nb_couleurs,Chaine,3);
        Effacer_curseur();

        if (Readline(265,41,Chaine,3,1))
        {
          Couleur_temporaire=atoi(Chaine);
          // Correction de la valeur lue
          if ( (Couleur_temporaire>256) || (Couleur_temporaire<2) )
          {
            if (Couleur_temporaire>256)
              Couleur_temporaire=256;
            else
              Couleur_temporaire=2;

            Num2str(Couleur_temporaire,Chaine,3);
            Fenetre_Contenu_bouton_saisie(Fenetre_Liste_boutons_special,Chaine);
          }

          Reduce_Nb_couleurs=Couleur_temporaire;
        }
        Afficher_curseur();
        break;
    }


    if (!Mouse_K)
    {
      switch (Touche)
      {
        case SDLK_LEFTBRACKET : // D�caler Forecolor vers la gauche
          if (Debut_block==Fin_block)
          {
            Fore_color--;
            Premiere_couleur--;
            Derniere_couleur--;
            Debut_block--;
            Fin_block--;
            Palette_Reafficher_jauges(Jauge_rouge,Jauge_verte,Jauge_bleue,Palette_de_travail,Debut_block,Fin_block);
            Effacer_curseur();
            Tagger_intervalle_palette(Debut_block,Fin_block);
            // Affichage du n� de la couleur s�lectionn�e
            Num2str(Fore_color,Chaine,3);
            Print_dans_fenetre(237,58,Chaine,CM_Noir,CM_Clair);
            // Affichage dans le block de visu de la couleur en cours
            Block(Fenetre_Pos_X+(Menu_Facteur_X*264),Fenetre_Pos_Y+(Menu_Facteur_Y*93),Menu_Facteur_X<<4,Menu_Facteur_Y*64,Fore_color);
            Afficher_curseur();
          }
          break;

        case SDLK_RIGHTBRACKET : // D�caler Forecolor vers la droite
          if (Debut_block==Fin_block)
          {
            Fore_color++;
            Premiere_couleur++;
            Derniere_couleur++;
            Debut_block++;
            Fin_block++;
            Palette_Reafficher_jauges(Jauge_rouge,Jauge_verte,Jauge_bleue,Palette_de_travail,Debut_block,Fin_block);
            Effacer_curseur();
            Tagger_intervalle_palette(Debut_block,Fin_block);
            // Affichage du n� de la couleur s�lectionn�e
            Num2str(Fore_color,Chaine,3);
            Print_dans_fenetre(237,58,Chaine,CM_Noir,CM_Clair);
            // Affichage dans le block de visu de la couleur en cours
            Block(Fenetre_Pos_X+(Menu_Facteur_X*264),Fenetre_Pos_Y+(Menu_Facteur_Y*93),Menu_Facteur_X<<4,Menu_Facteur_Y*64,Fore_color);
            Afficher_curseur();
          }
          break;

        case (SDLK_LEFTBRACKET|MOD_SHIFT) : // Decaler Backcolor vers la gauche
          Back_color--;
        case (SDLK_RIGHTBRACKET|MOD_SHIFT) : // Decaler Backcolor vers la droite
          // attention: pas de break ci-dessus
          if (Touche==(SDLK_RIGHTBRACKET|MOD_SHIFT))
            Back_color++;
          Effacer_curseur();
          Block(Fenetre_Pos_X+(Menu_Facteur_X*260),Fenetre_Pos_Y+(Menu_Facteur_Y* 89),Menu_Facteur_X*24,Menu_Facteur_Y<<2,Back_color);
          Block(Fenetre_Pos_X+(Menu_Facteur_X*260),Fenetre_Pos_Y+(Menu_Facteur_Y*157),Menu_Facteur_X*24,Menu_Facteur_Y<<2,Back_color);
          Block(Fenetre_Pos_X+(Menu_Facteur_X*260),Fenetre_Pos_Y+(Menu_Facteur_Y* 93),Menu_Facteur_X<<2,Menu_Facteur_Y<<6,Back_color);
          Block(Fenetre_Pos_X+(Menu_Facteur_X*280),Fenetre_Pos_Y+(Menu_Facteur_Y* 93),Menu_Facteur_X<<2,Menu_Facteur_Y<<6,Back_color);
          Afficher_curseur();
          break;

        case SDLK_BACKSPACE : // Remise des couleurs du menu � l'�tat normal en essayant
                      // de ne pas trop modifier l'image.
          if (!Backup_de_l_image_effectue)
          {
            Backup();
            Backup_de_l_image_effectue=1;
          }
          if (Nb_couleurs_utilisees==-1)
            Compter_nb_couleurs_utilisees(&Nb_couleurs_utilisees,Utilisation_couleur);

          memcpy(Palette_backup,Palette_de_travail,sizeof(T_Palette));
          memcpy(Palette_temporaire,Principal_Palette,sizeof(T_Palette));
          memcpy(Principal_Palette,Palette_de_travail,sizeof(T_Palette));
          Remettre_proprement_les_couleurs_du_menu(Utilisation_couleur);
          memcpy(Palette_de_travail,Principal_Palette,sizeof(T_Palette));
          memcpy(Principal_Palette,Palette_temporaire,sizeof(T_Palette));
          Set_palette(Palette_de_travail);
          memcpy(Palette_temporaire,Palette_de_travail,sizeof(T_Palette));
          Palette_Reafficher_jauges(Jauge_rouge,Jauge_verte,Jauge_bleue,Palette_de_travail,Debut_block,Fin_block);
          Compter_nb_couleurs_utilisees(&Nb_couleurs_utilisees,Utilisation_couleur);
          Il_faut_remapper=1;
          break;

        case SDLK_BACKQUOTE : // R�cup�ration d'une couleur derri�re le menu
        case SDLK_COMMA :
          Recuperer_couleur_derriere_fenetre(&Couleur,&Click);
          if (Click)
          {
            Effacer_curseur();
            if (Click==A_DROITE)
            {
              if (Back_color!=Couleur)
              {
                Back_color=Couleur;
                // 4 blocks de back_color entourant la fore_color
                Block(Fenetre_Pos_X+(Menu_Facteur_X*260),Fenetre_Pos_Y+(Menu_Facteur_Y* 89),Menu_Facteur_X*24,Menu_Facteur_Y<<2,Back_color);
                Block(Fenetre_Pos_X+(Menu_Facteur_X*260),Fenetre_Pos_Y+(Menu_Facteur_Y*157),Menu_Facteur_X*24,Menu_Facteur_Y<<2,Back_color);
                Block(Fenetre_Pos_X+(Menu_Facteur_X*260),Fenetre_Pos_Y+(Menu_Facteur_Y* 93),Menu_Facteur_X<<2,Menu_Facteur_Y<<6,Back_color);
                Block(Fenetre_Pos_X+(Menu_Facteur_X*280),Fenetre_Pos_Y+(Menu_Facteur_Y* 93),Menu_Facteur_X<<2,Menu_Facteur_Y<<6,Back_color);
              }
            }
            else
            {
              Fore_color=Premiere_couleur=Derniere_couleur=Debut_block=Fin_block=Couleur;
              Tagger_intervalle_palette(Debut_block,Fin_block);

              // Affichage du n� de la couleur s�lectionn�e
              Block(Fenetre_Pos_X+(Menu_Facteur_X*261),Fenetre_Pos_Y+(Menu_Facteur_Y*58),Menu_Facteur_X*32,Menu_Facteur_Y*7,CM_Clair);
              Num2str(Fore_color,Chaine,3);
              Print_dans_fenetre(237,58,Chaine,CM_Noir,CM_Clair);

              // Affichage des jauges
              Block(Fenetre_Pos_X+(Menu_Facteur_X*176),Fenetre_Pos_Y+(Menu_Facteur_Y*172),Menu_Facteur_X*84,Menu_Facteur_Y*7,CM_Clair);

              Jauge_rouge->Nb_elements=64;
              Jauge_rouge->Position   =63-Palette_de_travail[Fore_color].R;
              Calculer_hauteur_curseur_jauge(Jauge_rouge);
              Fenetre_Dessiner_jauge(Jauge_rouge);
              Num2str(Palette_de_travail[Fore_color].R,Chaine,2);
              Print_dans_fenetre(180,172,Chaine,CM_Noir,CM_Clair);

              Jauge_verte->Nb_elements=64;
              Jauge_verte->Position   =63-Palette_de_travail[Fore_color].V;
              Calculer_hauteur_curseur_jauge(Jauge_verte);
              Fenetre_Dessiner_jauge(Jauge_verte);
              Num2str(Palette_de_travail[Fore_color].V,Chaine,2);
              Print_dans_fenetre(207,172,Chaine,CM_Noir,CM_Clair);

              Jauge_bleue->Nb_elements=64;
              Jauge_bleue->Position   =63-Palette_de_travail[Fore_color].B;
              Calculer_hauteur_curseur_jauge(Jauge_bleue);
              Fenetre_Dessiner_jauge(Jauge_bleue);
              Num2str(Palette_de_travail[Fore_color].B,Chaine,2);
              Print_dans_fenetre(234,172,Chaine,CM_Noir,CM_Clair);

              // Affichage dans le block de visu de la couleur en cours
              Block(Fenetre_Pos_X+(Menu_Facteur_X*264),Fenetre_Pos_Y+(Menu_Facteur_Y*93),Menu_Facteur_X<<4,Menu_Facteur_Y*64,Fore_color);

              memcpy(Palette_backup    ,Palette_de_travail,sizeof(T_Palette));
              memcpy(Palette_temporaire,Palette_de_travail,sizeof(T_Palette));
            }
            Afficher_curseur();
          }
      }

      if (Il_faut_remapper)
      {
        Effacer_curseur();
        Calculer_couleurs_menu_optimales(Palette_de_travail);

        // On remappe brutalement
        Remapper_ecran_apres_changement_couleurs_menu();
        // Puis on remet les trucs qui ne devaient pas changer
        Fenetre_Dessiner_bouton_palette(5,79);
        Block(Fenetre_Pos_X+(Menu_Facteur_X*260),Fenetre_Pos_Y+(Menu_Facteur_Y*89),Menu_Facteur_X*24,Menu_Facteur_Y*72,Back_color);
        Bloc_degrade_dans_fenetre(264,93,Debut_block,Fin_block);

        Afficher_curseur();
        Il_faut_remapper=0;
      }
    }
  }
  while ((Bouton_clicke!=13) && (Bouton_clicke!=14));

  Clavier_de_depart();

  if (Bouton_clicke==14)         // Sortie par OK
  {
    if ( (!Backup_de_l_image_effectue)
      && memcmp(Principal_Palette,Palette_de_travail,sizeof(T_Palette)) )
      Backup();
    memcpy(Principal_Palette,Palette_de_travail,sizeof(T_Palette));
  }

  Calculer_couleurs_menu_optimales(Principal_Palette);

  // La variable employ�e ici n'a pas vraiment de rapport avec son nom...
  Il_faut_remapper=(Fenetre_Pos_Y+(Fenetre_Hauteur*Menu_Facteur_Y)<Menu_Ordonnee_avant_fenetre);

  Fermer_fenetre();
  Desenclencher_bouton(BOUTON_PALETTE);

  Recadrer_palette();

  //   On affiche les "ForeBack" car le menu n'est raffich� que si la fen�tre
  // empi�tait sur le menu. Mais si les couleurs on �t� modifi�es, il faut
  // rafficher tout le menu remapp�.
  if (Il_faut_remapper)
    Afficher_menu();

  Afficher_curseur();

  if (Bouton_clicke==13)                         // Sortie par CANCEL
  {
    Set_palette(Principal_Palette);
    if (Backup_de_l_image_effectue)
      Enclencher_bouton(BOUTON_UNDO,A_GAUCHE);
  }

  free(Palette_backup);
  free(Palette_temporaire);
  free(Palette_de_travail);
}




//---------------------- Menu de palettes secondaires ------------------------

void Bouton_Palette_secondaire(void)
{
  short Bouton_clicke;
  byte Dummy;


  Ouvrir_fenetre(200,80,"Palettes");

  Fenetre_Definir_bouton_normal(10,20,180,14,"Colors for best match",12,1,SDLK_b); // 1
  Fenetre_Definir_bouton_normal(10,37,180,14,"User's color series"  ,14,0,SDLK_s); // 2
  Fenetre_Definir_bouton_normal(60,60, 80,14,"Cancel"               , 0,1,SDLK_ESCAPE); // 3

  Afficher_curseur();

  do
  {
    Bouton_clicke=Fenetre_Bouton_clicke();
  }
  while (Bouton_clicke<=0);

  Fermer_fenetre();
  Desenclencher_bouton(BOUTON_PALETTE);
  Afficher_curseur();

  if (Bouton_clicke==1)
  {
    Menu_Tag_couleurs("Tag colors to exclude",Exclude_color,&Dummy,1);
  }
/*
  else
  {
    if (Bouton_clicke==2)
    {

    }
  }
*/
}
