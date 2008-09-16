#include <stdio.h>
#include <string.h>
#include "const.h"
#include "struct.h"
#include "global.h"
#include "divers.h"
#include "graph.h"
#include "moteur.h"
#include "tables_aide.h"

#include <string.h>

#ifdef __linux__
    #include <sys/vfs.h>
#else
    #include <windows.h>
#endif

// -- Menu d'aide -----------------------------------------------------------

void Afficher_aide(void)
{
  short  X;                   // Indices d'affichage d'un caract�re
  short  Y;
  short  Position_X;          // Parcours de remplissage du buffer de ligne
  short  Indice_de_ligne;     // 0-15 (16 lignes de textes)
  short  Indice_de_caractere; // Parcours des caract�res d'une ligne
  short  Ligne_de_depart=Position_d_aide_en_cours;
  short  Repeat_Menu_Facteur_X;
  short  Repeat_Menu_Facteur_Y;
  short  Pos_Reel_X;
  short  Pos_Reel_Y;
  int    Curseur;
  short  Largeur;             // Largeur physique d'une ligne de texte
  char   TypeLigne;           // N: Normale, T: Titre, S: Sous-titre
                              // -: Ligne inf�rieur de sous-titre
  const unsigned char  * Ligne;

  Pos_Reel_X=Fenetre_Pos_X+(13*Menu_Facteur_X);
  Pos_Reel_Y=Fenetre_Pos_Y+(19*Menu_Facteur_Y);

  for (Indice_de_ligne=0;Indice_de_ligne<16;Indice_de_ligne++)
  {
    // On affiche la ligne
    Ligne = Table_d_aide[Section_d_aide_en_cours].Table_aide[Ligne_de_depart + Indice_de_ligne];
    TypeLigne = Ligne[0];
    // Si c'est une sous-ligne de titre, on utilise le texte de la ligne pr�c�dente
    if (TypeLigne == '-' && (Ligne_de_depart + Indice_de_ligne > 0))
      Ligne = Table_d_aide[Section_d_aide_en_cours].Table_aide[Ligne_de_depart + Indice_de_ligne - 1];
    // On ignore le premier caract�re
    Ligne++;
    // Calcul de la taille
    Largeur=strlen(Ligne);
    // Les lignes de titres prennent plus de place
    if (TypeLigne == 'T' || TypeLigne == '-')
      Largeur = Largeur*2;

    // Pour chaque ligne dans la fen�tre:
    for (Y=0;Y<8;Y++)
    {
      Position_X=0;
      // On cr�e une nouvelle ligne � splotcher
      for (Indice_de_caractere=0;Indice_de_caractere<Largeur;Indice_de_caractere++)
      {
        // Recherche du caract�re dans la fonte de 315 symboles.
        // Ligne titre : Si l'indice est impair on dessine le quart de caract�re
        // qui va a gauche, sinon celui qui va a droite.
        
        if (TypeLigne=='T')
          Curseur=Caracteres_Aide_Titre_haut[Ligne[Indice_de_caractere/2]-' '] + (Indice_de_caractere & 1);
        else if (TypeLigne=='-')
          Curseur=Caracteres_Aide_Titre_bas[Ligne[Indice_de_caractere/2]-' '] + (Indice_de_caractere & 1);
        else if (TypeLigne=='S')
          Curseur=Caracteres_Aide_S[Ligne[Indice_de_caractere]-' '];
        else if (TypeLigne=='N')
          Curseur=Caracteres_Aide_N[Ligne[Indice_de_caractere]-' '];
        else
          Curseur=1; // Un garde-fou en cas de probleme
          
        // Un deuxi�me garde-fou
        if (Curseur < 0 || Curseur > 315)
          Curseur = 1; // '!' affich� pour les caract�res non pr�vus
        
        for (X=0;X<6;X++)
          for (Repeat_Menu_Facteur_X=0;Repeat_Menu_Facteur_X<Menu_Facteur_X;Repeat_Menu_Facteur_X++)
            Buffer_de_ligne_horizontale[Position_X++]=Fonte_help[Curseur][X][Y];
      }

      // On la splotche
      for (Repeat_Menu_Facteur_Y=0;Repeat_Menu_Facteur_Y<Menu_Facteur_Y;Repeat_Menu_Facteur_Y++)
        Afficher_ligne(Pos_Reel_X,Pos_Reel_Y++,Largeur*Menu_Facteur_X*6,Buffer_de_ligne_horizontale);
    }

    // On efface la fin de la ligne:
    Block (Pos_Reel_X+Largeur*Menu_Facteur_X*6,
           Pos_Reel_Y-(8*Menu_Facteur_Y),
           ((44*6*Menu_Facteur_X)-Largeur*Menu_Facteur_X*6)+1,
           // 44 = Nb max de char (+1 pour �viter les plantages en mode X
           // caus�s par une largeur = 0)
           Menu_Facteur_Y<<3,
           CM_Noir);
  }

  SDL_UpdateRect(Ecran_SDL,Fenetre_Pos_X+13*Menu_Facteur_X,Fenetre_Pos_Y+19*Menu_Facteur_Y,44*6*Menu_Facteur_X,16*8*Menu_Facteur_Y);
}


void Scroller_aide(void)
{
  Effacer_curseur();
  Fenetre_Liste_boutons_scroller->Position=Position_d_aide_en_cours;
  Calculer_hauteur_curseur_jauge(Fenetre_Liste_boutons_scroller);
  Fenetre_Dessiner_jauge(Fenetre_Liste_boutons_scroller);
  Afficher_aide();
  Afficher_curseur();
}


void Bouton_Aide(void)
{
  short Bouton_clicke;
  short Nb_lignes=Table_d_aide[Section_d_aide_en_cours].Nombre_de_lignes;

  Ouvrir_fenetre(310,175,"Help / About...");

  // dessiner de la fen�tre o� va d�filer le texte
  Fenetre_Afficher_cadre_creux(8,17,274,132);
  Block(Fenetre_Pos_X+(Menu_Facteur_X*9),
        Fenetre_Pos_Y+(Menu_Facteur_Y*18),
        Menu_Facteur_X*272,Menu_Facteur_Y*130,CM_Noir);

  Fenetre_Definir_bouton_normal(266,153,35,14,"Exit",0,1,0x0001); // 1
  Fenetre_Definir_bouton_scroller(290,18,130,Nb_lignes,
                                  16,Position_d_aide_en_cours);   // 2

  Fenetre_Definir_bouton_normal(  9,154, 59,14,"Credits"  ,1,1,0x002E); // 3
  Fenetre_Definir_bouton_normal( 71,154, 75,14,"Register?",1,1,0x0013); // 4
  Fenetre_Definir_bouton_normal(149,154, 75,14,"Greetings",1,1,0x0022); // 5

  Afficher_aide();

  Afficher_curseur();

  do
  {
    Bouton_clicke=Fenetre_Bouton_clicke();

    switch (Bouton_clicke)
    {
      case -1:
      case  0:
      case  1:
        break;
      default:
        Effacer_curseur();
        if (Bouton_clicke>2)
        {
          Section_d_aide_en_cours=Bouton_clicke-3;
          Position_d_aide_en_cours=0;
          Nb_lignes=Table_d_aide[Section_d_aide_en_cours].Nombre_de_lignes;
          Fenetre_Liste_boutons_scroller->Position=0;
          Fenetre_Liste_boutons_scroller->Nb_elements=Nb_lignes;
          Calculer_hauteur_curseur_jauge(Fenetre_Liste_boutons_scroller);
          Fenetre_Dessiner_jauge(Fenetre_Liste_boutons_scroller);
        }
        else
          Position_d_aide_en_cours=Fenetre_Attribut2;

        Afficher_aide();
        Afficher_curseur();
    }


    // Gestion des touches de d�placement dans la liste
    switch (Touche)
    {
      case 0x0048 : // Haut
        if (Position_d_aide_en_cours>0)
          Position_d_aide_en_cours--;
        Scroller_aide();
        break;
      case 0x0050 : // Bas
        if (Position_d_aide_en_cours<Nb_lignes-16)
          Position_d_aide_en_cours++;
        Scroller_aide();
        break;
      case 0x0049 : // PageUp
        if (Position_d_aide_en_cours>15)
          Position_d_aide_en_cours-=15;
        else
          Position_d_aide_en_cours=0;
        Scroller_aide();
        break;
      case 0x0051 : // PageDown
        if (Position_d_aide_en_cours<Nb_lignes-31)
          Position_d_aide_en_cours+=15;
        else
          Position_d_aide_en_cours=Nb_lignes-16;
        Scroller_aide();
        break;
      case 0x0047 : // Home
        Position_d_aide_en_cours=0;
        Scroller_aide();
        break;
      case 0x004F : // End
        Position_d_aide_en_cours=Nb_lignes-16;
        Scroller_aide();
        break;
    }

  }
  while ((Bouton_clicke!=1) && (Touche!=0x001C));

  Fermer_fenetre();
  Desenclencher_bouton(BOUTON_AIDE);
  Afficher_curseur();
}


#define STATS_COULEUR_TITRES  CM_Blanc
#define STATS_COULEUR_DONNEES CM_Clair
void Bouton_Stats(void)
{
  short Bouton_clicke;
  char  Buffer[37];
  dword Utilisation_couleur[256];
  unsigned long long freeRam;

  #ifdef __linux__
    struct statfs Informations_Disque;
    unsigned long long Taille = 0;
  #else
    unsigned __int64 Taille;
    ULARGE_INTEGER tailleU;
  #endif


  Ouvrir_fenetre(310,174,"Statistics");

  // Dessin de la fenetre ou va s'afficher le texte
  Fenetre_Afficher_cadre_creux(8,17,294,132);
  Block(Fenetre_Pos_X+(Menu_Facteur_X*9),
        Fenetre_Pos_Y+(Menu_Facteur_Y*18),
        Menu_Facteur_X*292,Menu_Facteur_Y*130,CM_Noir);

  Fenetre_Definir_bouton_normal(120,153,70,14,"OK",0,1,0x0001); // 1

  // Affichage du num�ro de version
  Print_dans_fenetre(10,19,"Version:",STATS_COULEUR_TITRES,CM_Noir);
  sprintf(Buffer,"GrafX 2.00 %s%s",ALPHA_BETA,POURCENTAGE_VERSION);
  Print_dans_fenetre(82,19,Buffer,STATS_COULEUR_DONNEES,CM_Noir);

  // Affichage de la m�moire restante
  Print_dans_fenetre(10,35,"Free memory: ",STATS_COULEUR_TITRES,CM_Noir);

  freeRam = Memoire_libre();
  
  if(freeRam > (100ULL*1024*1024*1024))
        sprintf(Buffer,"%d Gigabytes",(unsigned int)(freeRam/(1024*1024*1024)));
  else if(freeRam > (100*1024*1024))
        sprintf(Buffer,"%d Megabytes",(unsigned int)(freeRam/(1024*1024)));
  else if(freeRam > 100*1024)
        sprintf(Buffer,"%d Kilobytes",(unsigned int)(freeRam/1024));
  else
        sprintf(Buffer,"%d bytes",(unsigned int)freeRam);
  Print_dans_fenetre(114,35,Buffer,STATS_COULEUR_DONNEES,CM_Noir);

  // Affichage de l'espace disque libre
  sprintf(Buffer,"Free space on %c:",Principal_Repertoire_courant[0]);
  Print_dans_fenetre(10,51,Buffer,STATS_COULEUR_TITRES,CM_Noir);

  #ifdef __linux__
    statfs(Principal_Repertoire_courant,&Informations_Disque);
    Taille=Informations_Disque.f_bfree * Informations_Disque.f_bsize;
  #else
    GetDiskFreeSpaceEx(Principal_Repertoire_courant,&tailleU,NULL,NULL);
    Taille = tailleU.QuadPart;
  #endif
  
  if (Taille>=0)
  {
    if(Taille > (100ULL*1024*1024*1024))
        sprintf(Buffer,"%d Gigabytes",(unsigned int)(Taille/(1024*1024*1024)));
    else if(Taille > (100*1024*1024))
        sprintf(Buffer,"%d Megabytes",(unsigned int)(Taille/(1024*1024)));
    else if(Taille > (100*1024))
        sprintf(Buffer,"%d Kilobytes",(unsigned int)(Taille/1024));
    else 
        sprintf(Buffer,"%d bytes",(unsigned int)Taille);
    Print_dans_fenetre(146,51,Buffer,STATS_COULEUR_DONNEES,CM_Noir);
  }
  else
    Print_dans_fenetre(146,51,"* Error *",STATS_COULEUR_DONNEES,CM_Noir);

  // Affichage des informations sur l'image
  Print_dans_fenetre(10,67,"Picture info.:",STATS_COULEUR_TITRES,CM_Noir);

  // Affichage des dimensions de l'image
  Print_dans_fenetre(18,75,"Dimensions :",STATS_COULEUR_TITRES,CM_Noir);
  sprintf(Buffer,"%dx%d",Principal_Largeur_image,Principal_Hauteur_image);
  Print_dans_fenetre(122,75,Buffer,STATS_COULEUR_DONNEES,CM_Noir);

  // Affichage du nombre de couleur utilis�
  Print_dans_fenetre(18,83,"Colors used:",STATS_COULEUR_TITRES,CM_Noir);
  bzero(Utilisation_couleur,256*sizeof(Utilisation_couleur[0]));
  sprintf(Buffer,"%d",Palette_Compter_nb_couleurs_utilisees(Utilisation_couleur));
  Print_dans_fenetre(122,83,Buffer,STATS_COULEUR_DONNEES,CM_Noir);

  // Affichage des dimensions de l'�cran
  Print_dans_fenetre(10,99,"Resolution:",STATS_COULEUR_TITRES,CM_Noir);
  sprintf(Buffer,"%dx%d",Largeur_ecran,Hauteur_ecran);
  Print_dans_fenetre(106,99,Buffer,STATS_COULEUR_DONNEES,CM_Noir);

  // Affichage des infos VESA
  Print_dans_fenetre(10,115,"VESA info.:",STATS_COULEUR_TITRES,CM_Noir);
  if (((VESA_Version_Unite*10)+VESA_Version_Decimale)>=12)
  {
    Print_dans_fenetre(18,123,"Version  :",STATS_COULEUR_TITRES,CM_Noir);
    sprintf(Buffer,"%d.%d",VESA_Version_Unite,VESA_Version_Decimale);
    Print_dans_fenetre(106,123,Buffer,STATS_COULEUR_DONNEES,CM_Noir);

    Print_dans_fenetre(18,131,"Manufact.:",STATS_COULEUR_TITRES,CM_Noir);
    strncpy(Buffer,VESA_Constructeur,TAILLE_NOM_CONSTRUCTEUR);
    Buffer[TAILLE_NOM_CONSTRUCTEUR]='\0';
    Print_dans_fenetre(106,131,Buffer,STATS_COULEUR_DONNEES,CM_Noir);

    Print_dans_fenetre(18,139,"Memory   :",STATS_COULEUR_TITRES,CM_Noir);
    sprintf(Buffer,"%d Kb",VESA_Taille_memoire*64);
    Print_dans_fenetre(106,139,Buffer,STATS_COULEUR_DONNEES,CM_Noir);
  }
  else
    Print_dans_fenetre(106,115,"* No VESA support *",STATS_COULEUR_DONNEES,CM_Noir);


  Afficher_curseur();

  do
  {
    Bouton_clicke=Fenetre_Bouton_clicke();
  }
  while ( (Bouton_clicke!=1) && (Touche!=0x001C) );

  Fermer_fenetre();
  Desenclencher_bouton(BOUTON_AIDE);
  Afficher_curseur();
}
