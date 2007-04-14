#define VARIABLES_GLOBALES

#include "const.h"
#include "struct.h"
#include "global.h"
#include "graph.h"
#include "divers.h"
#include "init.h"
#include "boutons.h"
#include "moteur.h"
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <signal.h>
#include <time.h>
#include <SDL/SDL.h>
#include <unistd.h>
#include "linux.h" //Fichier avec diverses fonctions qui existaient sous dos mais pas sous linux...
#include "pages.h"
#include "files.h"
#include "loadsave.h"
#include "sdlscreen.h"

byte Ancien_nb_lignes;                  // Ancien nombre de lignes de l'�cran


//--- Affichage de la syntaxe, et de la liste des modes vid�os disponibles ---
void Afficher_syntaxe(void)
{
  printf("Syntax: GFX2 [<picture>] [<Video mode number>|<Help>]\n\n");
  printf("<Help> can be /? or /h\n\n");
  printf("Available video modes:\n����������������������\n");
  printf("\t 0:%s",Libelle_mode(0));
  printf("\t15:%s",Libelle_mode(15));
  printf("\t30:%s",Libelle_mode(30));
  printf("\t45:%s\n",Libelle_mode(45));
  printf("\t 1:%s",Libelle_mode(1));
  printf("\t16:%s",Libelle_mode(16));
  printf("\t31:%s",Libelle_mode(31));
  printf("\t46:%s\n",Libelle_mode(46));
  printf("\t 2:%s",Libelle_mode(2));
  printf("\t17:%s",Libelle_mode(17));
  printf("\t32:%s",Libelle_mode(32));
  printf("\t47:%s\n",Libelle_mode(47));
  printf("\t 3:%s",Libelle_mode(3));
  printf("\t18:%s",Libelle_mode(18));
  printf("\t33:%s",Libelle_mode(33));
  printf("\t48:%s\n",Libelle_mode(48));
  printf("\t 4:%s",Libelle_mode(4));
  printf("\t19:%s",Libelle_mode(19));
  printf("\t34:%s",Libelle_mode(34));
  printf("\t49:%s\n",Libelle_mode(49));
  printf("\t 5:%s",Libelle_mode(5));
  printf("\t20:%s",Libelle_mode(20));
  printf("\t35:%s",Libelle_mode(35));
  printf("\t50:%s\n",Libelle_mode(50));
  printf("\t 6:%s",Libelle_mode(6));
  printf("\t21:%s",Libelle_mode(21));
  printf("\t36:%s",Libelle_mode(36));
  printf("\t51:%s\n",Libelle_mode(51));
  printf("\t 7:%s",Libelle_mode(7));
  printf("\t22:%s",Libelle_mode(22));
  printf("\t37:%s",Libelle_mode(37));
  printf("\t52:%s\n",Libelle_mode(52));
  printf("\t 8:%s",Libelle_mode(8));
  printf("\t23:%s",Libelle_mode(23));
  printf("\t38:%s",Libelle_mode(38));
  printf("\t53:%s\n",Libelle_mode(53));
  printf("\t 9:%s",Libelle_mode(9));
  printf("\t24:%s",Libelle_mode(24));
  printf("\t39:%s",Libelle_mode(39));
  printf("\t54:%s\n",Libelle_mode(54));
  printf("\t10:%s",Libelle_mode(10));
  printf("\t25:%s",Libelle_mode(25));
  printf("\t40:%s",Libelle_mode(40));
  printf("\t55:%s\n",Libelle_mode(55));
  printf("\t11:%s",Libelle_mode(11));
  printf("\t26:%s",Libelle_mode(26));
  printf("\t41:%s",Libelle_mode(41));
  printf("\t56:%s\n",Libelle_mode(56));
  printf("\t12:%s",Libelle_mode(12));
  printf("\t27:%s",Libelle_mode(27));
  printf("\t42:%s",Libelle_mode(42));
  printf("\t57:%s\n",Libelle_mode(57));
  printf("\t13:%s",Libelle_mode(13));
  printf("\t28:%s",Libelle_mode(28));
  printf("\t43:%s",Libelle_mode(43));
  printf("\t58:%s\n",Libelle_mode(58));
  printf("\t14:%s",Libelle_mode(14));
  printf("\t29:%s",Libelle_mode(29));
  printf("\t44:%s",Libelle_mode(44));
  printf("\t59:%s\n",Libelle_mode(59));
}


// ---------------------------- Sortie impromptue ----------------------------
void Erreur(int Code)
{
  T_Palette Palette_temporaire;
  int       Indice;

  if (Code==0)
  {
    memcpy(Palette_temporaire,Principal_Palette,sizeof(T_Palette));
    for (Indice=0;Indice<=255;Indice++)
      Palette_temporaire[Indice].R=255;
    Set_palette(Palette_temporaire);
    for (Indice=0;Indice<10;Indice++)
      Wait_VBL();
    Set_palette(Principal_Palette);
  }
  else
  {
    Passer_en_mode_texte(Ancien_nb_lignes);
    switch (Code)
    {
      case ERREUR_DAT_ABSENT         : printf("Error: File GFX2.DAT is missing!\n");
                                       printf("This program cannot run without this file.\n");
                                       break;
      case ERREUR_DAT_CORROMPU       : printf("Error: File GFX2.DAT is corrupt!\n");
                                       printf("This program cannot run without a correct version of this file.\n");
                                       break;
      case ERREUR_CFG_ABSENT         : printf("Error: File GFX2.CFG is missing!\n");
                                       printf("Please run GFXCFG to create it.\n");
                                       break;
      case ERREUR_CFG_CORROMPU       : printf("Error: File GFX2.CFG is corrupt!\n");
                                       printf("Please run GFXCFG to make a valid file.\n");
                                       break;
      case ERREUR_CFG_ANCIEN         : printf("Error: File GFX2.CFG is from another version of GrafX2.\n");
                                       printf("Please run GFXCFG to update this file.\n");
                                       break;
      case ERREUR_MEMOIRE            : printf("Error: Not enough memory!\n\n");
                                       printf("You should try removing memory drivers and useless TSRs to free some memory.\n\n");
                                       printf("If it still doesn't work, try running GFX2_MEM.BAT if you have got DOS4GW.EXE\n");
                                       printf("somewhere on your disk (after having edited the batch file).\n");
                                       printf("This also requires 16 free Megabytes on your disk to create the memory cache.\n");
                                       printf("This will slow down the program but, at least, you'll get GrafX2 running.\n");
                                       break;
      case ERREUR_LIGNE_COMMANDE     : printf("Error: Too many parameters.\n\n");
                                       Afficher_syntaxe();
                                       break;
      case ERREUR_DRIVER_SOURIS      : printf("Error: No mouse detected!\n");
                                       printf("Check if a mouse driver is installed and if your mouse is correctly connected.\n");
                                       break;
      case ERREUR_MODE_VESA_INVALIDE : printf("Error: Requested VESA mode not supported by your video card!\n");
                                       printf("You should try to run a VESA driver such as UNIVESA or UNIVBE.\n");
                                       break;
      case ERREUR_MODE_INTERDIT      : printf("Error: The requested video mode has been disabled from the resolution menu!\n");
                                       printf("If you want to run the program in this mode, you'll have to start it with an\n");
                                       printf("enabled mode, then enter the resolution menu and enable the mode you want.\n");
                                       printf("Check also if the 'Default_video_mode' parameter in GFX2.INI is correct.\n");
                                       break;
      case ERREUR_NUMERO_MODE        : printf("Error: Invalid parameter or file not found.\n\n");
                                       Afficher_syntaxe();
                                       break;
      case ERREUR_SAUVEGARDE_CFG     : printf("Error: Write error while saving settings!\n");
                                       printf("Settings have not been saved correctly, and the GFX2.CFG file may have been\n");
                                       printf("corrupt. If so, please run GFXCFG to make a new valid file.\n");
                                       break;
      case ERREUR_REPERTOIRE_DISPARU : printf("Error: Directory you ran the program from not found!\n");
                                       break;
      case ERREUR_INI_ABSENT         : printf("Error: File GFX2.INI is missing!\n");
                                       break;
      case ERREUR_INI_CORROMPU       : printf("Error: File GFX2.INI is corrupt!\n");
                                       printf("It contains bad values at line %d.\n",Ligne_INI);
                                       break;
      case ERREUR_SAUVEGARDE_INI     : printf("Error: Cannot rewrite file GFX2.INI!\n");
                                       break;
      case ERREUR_SORRY_SORRY_SORRY  : printf("Error: Sorry! Sorry! Sorry! Please forgive me!\n");
                                       break;
    }
    Clavier_de_depart();
    exit(Code);
  }
}


// --------------------- Analyse de la ligne de commande ---------------------
void Analyse_de_la_ligne_de_commande(int argc,char * argv[])
{
  byte Option2=1;
  char *Buffer;


  Un_fichier_a_ete_passe_en_parametre=0;
  Une_resolution_a_ete_passee_en_parametre=0;

  switch (argc)
  {
    case 1 : // Mode par d�faut
      Resolution_actuelle=Config.Resolution_par_defaut;
      break;

    case 2 :
    case 3 :
      if (Fichier_existe(argv[1]))
      {
        Un_fichier_a_ete_passe_en_parametre=1;
        Option2=2;

        // On r�cup�re le chemin complet du param�tre
        Buffer=realpath(argv[1],NULL);
        // Et on d�coupe ce chemin en r�pertoire(path) + fichier(.ext)
        _splitpath(Buffer,Principal_Repertoire_fichier,Principal_Nom_fichier);
        // chdir(Principal_Repertoire_fichier);
      }
      else
      {
        if (argc==3)
          Erreur(ERREUR_NUMERO_MODE);
      }

      if (Option2<argc)
      {
        if ( (!strcmp(argv[Option2],"/?")) ||
             (!strcmp(argv[Option2],"/h")) ||
             (!strcmp(argv[Option2],"/H")) )
        {
          Passer_en_mode_texte(Ancien_nb_lignes);
          Clavier_de_depart();
          Afficher_syntaxe();
          exit(0);
        }

        Resolution_actuelle=Str2num(argv[Option2]); // Mode d�sir� par l'utilisateur
        if ( (Resolution_actuelle<MODE_320_200) || (Resolution_actuelle>MODE_1024_768) )
          Erreur(ERREUR_NUMERO_MODE);
        Une_resolution_a_ete_passee_en_parametre=Un_fichier_a_ete_passe_en_parametre;
      }
      break;

    default:
      // Erreur: trop de param�tres sur la ligne de commande
      Erreur(ERREUR_LIGNE_COMMANDE);
  }

  if (Mode_video[Resolution_actuelle].Etat>=128)
    Erreur(ERREUR_MODE_VESA_INVALIDE);
  else
    if (Mode_video[Resolution_actuelle].Etat==3)
      Erreur(ERREUR_MODE_INTERDIT);
}

// ------------------------ Initialiser le programme -------------------------
void Initialisation_du_programme(int argc,char * argv[])
{
  int Temp;
  int Mode_dans_lequel_on_demarre;

  // On commence �galement par interdire d'appuyer sur Ctrl+Pause et Ctrl+C
  signal(SIGINT  ,SIG_IGN);

  printf("���� GrafX 2.00 %s%s � Copyright (c)1996-1999 Sunset Design ۲��\n",ALPHA_BETA,POURCENTAGE_VERSION);

  // On cr�e d�s maintenant les descripteurs des listes de pages pour la page
  // principale et la page de brouillon afin que leurs champs ne soient pas
  // invalide lors des appels aux multiples fonctions manipul�es �
  // l'initialisation du programme.
  Principal_Backups=(S_Liste_de_pages *)malloc(sizeof(S_Liste_de_pages));
  Brouillon_Backups=(S_Liste_de_pages *)malloc(sizeof(S_Liste_de_pages));
  Initialiser_S_Liste_de_pages(Principal_Backups);
  Initialiser_S_Liste_de_pages(Brouillon_Backups);

  // On calcule tout de suite la table pr�calcul�e utilis�e pour la recherche
  // des meilleures couleurs afin de ne pas avoir de valeurs catastrophiques
  // d�sign�es au d�marrage (couleurs du menu, ...)
  Initialiser_la_table_precalculee_des_distances_de_couleur();

  // On d�termine d�s le d�part o� se trouve le fichier:
  Chercher_repertoire_du_programme(argv[0]);
  // On d�tecte les lecteurs qui sont accessibles:
  Rechercher_drives();
  // On d�termine le r�pertoire courant:
  Determiner_repertoire_courant();

  // On en profite pour le m�moriser dans le r�pertoire principal:
  strcpy(Repertoire_initial,Principal_Repertoire_courant);

  // On initialise les donn�es sur le nom de fichier de l'image principale:
  strcpy(Principal_Repertoire_fichier,Principal_Repertoire_courant);
  strcpy(Principal_Nom_fichier,"NO_NAME.PKM");
  Principal_Format_fichier=FORMAT_PAR_DEFAUT;
  // On initialise les donn�es sur le nom de fichier de l'image de brouillon:
  strcpy(Brouillon_Repertoire_courant,Principal_Repertoire_courant);
  strcpy(Brouillon_Repertoire_fichier,Principal_Repertoire_fichier);
  strcpy(Brouillon_Nom_fichier       ,Principal_Nom_fichier);
  Brouillon_Format_fichier    =Principal_Format_fichier;
  strcpy(Brosse_Repertoire_courant,Principal_Repertoire_courant);
  strcpy(Brosse_Repertoire_fichier,Principal_Repertoire_fichier);
  strcpy(Brosse_Nom_fichier       ,Principal_Nom_fichier);
  Brosse_Format_fichier    =Principal_Format_fichier;

  // On initialise ce qu'il faut pour que les fileselects ne plantent pas:
  Liste_du_fileselect=NULL;       // Au d�but, il n'y a pas de fichiers dans la liste
  Principal_File_list_Position=0; // Au d�but, le fileselect est en haut de la liste des fichiers
  Principal_File_list_Decalage=0; // Au d�but, le fileselect est en haut de la liste des fichiers
  Principal_Format=0;
  Brouillon_File_list_Position=0;
  Brouillon_File_list_Decalage=0;
  Brouillon_Format=0;
  Brosse_File_list_Position=0;
  Brosse_File_list_Decalage=0;
  Brosse_Format=0;

  // On initialise les commentaires des images � des cha�nes vides
  Principal_Commentaire[0]='\0';
  Brouillon_Commentaire[0]='\0';
  Brosse_Commentaire[0]='\0';

  // On initialise d'ot' trucs
  Principal_Decalage_X=0;
  Principal_Decalage_Y=0;
  Ancien_Principal_Decalage_X=0;
  Ancien_Principal_Decalage_Y=0;
  Principal_Split=0;
  Principal_X_Zoom=0;
  Principal_Proportion_split=PROPORTION_SPLIT;
  Loupe_Mode=0;
  Loupe_Facteur=FACTEUR_DE_ZOOM_PAR_DEFAUT;
  Loupe_Hauteur=0;
  Loupe_Largeur=0;
  Loupe_Decalage_X=0;
  Loupe_Decalage_Y=0;
  Brouillon_Decalage_X=0;
  Brouillon_Decalage_Y=0;
  Ancien_Brouillon_Decalage_X=0;
  Ancien_Brouillon_Decalage_Y=0;
  Brouillon_Split=0;
  Brouillon_X_Zoom=0;
  Brouillon_Proportion_split=PROPORTION_SPLIT;
  Brouillon_Loupe_Mode=0;
  Brouillon_Loupe_Facteur=FACTEUR_DE_ZOOM_PAR_DEFAUT;
  Brouillon_Loupe_Hauteur=0;
  Brouillon_Loupe_Largeur=0;
  Brouillon_Loupe_Decalage_X=0;
  Brouillon_Loupe_Decalage_Y=0;

  // On initialise tous les modes vid�o
  Definition_des_modes_video();

  // On initialise les donn�es sur l'�tat du programme:
    // Donn�e sur la sortie du programme:
  Sortir_du_programme=0;
    // Donn�es sur l'�tat du menu:
  Pixel_dans_menu=Pixel_dans_barre_d_outil;
  Menu_visible=1;
    // Donn�es sur les couleurs et la palette:
  Fore_color=15;
  Back_color=0;
  Couleur_debut_palette=0;
    // Donn�es sur le curseur:
  Forme_curseur=FORME_CURSEUR_CIBLE;
  Cacher_curseur=0;
    // Donn�es sur le pinceau:
  Pinceau_X=0;
  Pinceau_Y=0;
  Pinceau_Forme=FORME_PINCEAU_ROND;
  Cacher_pinceau=0;

  Pixel_de_chargement=Pixel_Chargement_dans_ecran_courant;

  Forcer_affichage_curseur=0;

  // On initialise la table des carr�s:
  Initialiser_la_table_des_carres();

  // On initialise tout ce qui concerne les op�rations et les effets
  Operation_Taille_pile=0;
  Mode_de_dessin_en_cours=OPERATION_DESSIN_CONTINU;
  Ligne_en_cours         =OPERATION_LIGNE;
  Courbe_en_cours        =OPERATION_COURBE_3_POINTS;
  Fonction_effet=Aucun_effet;
    // On initialise les infos de la loupe:
  Loupe_Mode=0;
  Loupe_Facteur=FACTEUR_DE_ZOOM_PAR_DEFAUT;
  Initialiser_les_tables_de_multiplication();
  Table_mul_facteur_zoom=TABLE_ZOOM[2];
  Principal_Proportion_split=PROPORTION_SPLIT;
  Brouillon_Proportion_split=PROPORTION_SPLIT;
    // On initialise les infos du mode smear:
  Smear_Mode=0;
  Smear_Brosse_Largeur=LARGEUR_PINCEAU;
  Smear_Brosse_Hauteur=HAUTEUR_PINCEAU;
    // On initialise les infos du mode smooth:
  Smooth_Mode=0;
    // On initialise les infos du mode shade:
  Shade_Mode=0;     // Les autres infos du Shade sont charg�es avec la config
  Quick_shade_Mode=0; // idem
    // On initialise les infos sur les d�grad�s:
  Traiter_pixel_de_degrade =Afficher_pixel; // Les autres infos sont charg�es avec la config
    // On initialise les infos de la grille:
  Snap_Mode=0;
  Snap_Largeur=8;
  Snap_Hauteur=8;
  Snap_Decalage_X=0;
  Snap_Decalage_Y=0;
    // On initialise les infos du mode Colorize:
  Colorize_Mode=0;          // Mode colorize inactif par d�faut
  Colorize_Opacite=50;      // Une interpolation de 50% par d�faut
  Colorize_Mode_en_cours=0; // Par d�faut, la m�thode par interpolation
  Calculer_les_tables_de_Colorize();
    // On initialise les infos du mode Tiling:
  Tiling_Mode=0;  //   Pas besoin d'initialiser les d�calages car �a se fait
                  // en prenant une brosse (toujours mis � 0).
    // On initialise les infos du mode Mask:
  Mask_Mode=0;

    // Infos du Spray
  Spray_Mode=1; // Mode Mono
  Spray_Size=31;
  Spray_Delay=1;
  Spray_Mono_flow=10;
  memset(Spray_Multi_flow,0,256);
  srand(time(NULL)); // On randomize un peu tout �a...

  // R�cup�ration du nombre de lignes de l'ancien mode
  Ancien_nb_lignes=Recuperer_nb_lignes();
  // Passer en clavier am�ricain
  Clavier_americain();

  // Tester la pr�sence de la souris
  if (!Detection_souris()) Erreur(ERREUR_DRIVER_SOURIS);

  // Initialisation des boutons
  Initialisation_des_boutons();
  // Initialisation des op�rations
  Initialisation_des_operations();

  Une_fenetre_est_ouverte=0;

  // Charger les sprites et la palette
  Charger_DAT();
  // Charger la configuration des touches
  Temp=Charger_CFG(1);
  if (Temp)
    Erreur(Temp);
  // Charger la configuration du .INI
  Temp=Charger_INI(&Config);
  if (Temp)
    Erreur(Temp);

  // Transfert des valeurs du .INI qui ne changent pas dans des variables
  // plus accessibles:
  Palette_defaut[252]=Coul_menu_pref[0]=Config.Coul_menu_pref[0];
  Palette_defaut[253]=Coul_menu_pref[1]=Config.Coul_menu_pref[1];
  Palette_defaut[254]=Coul_menu_pref[2]=Config.Coul_menu_pref[2];
  Palette_defaut[255]=Coul_menu_pref[3]=Config.Coul_menu_pref[3];
  memcpy(Principal_Palette,Palette_defaut,sizeof(T_Palette));

  CM_Noir =0;
  CM_Fonce=25;
  CM_Clair=7;
  CM_Blanc=15;
  CM_Trans=1;
  Calculer_couleurs_menu_optimales(Palette_defaut);

  // Infos sur les trames (Sieve)
  Trame_Mode=0;
  Copier_trame_predefinie(0);

  // On s�lectionne toutes les couleurs pour le masque de copie de couleurs vers le brouillon
  memset(Masque_copie_couleurs,1,256);

  // Prise en compte de la fonte
  if (Config.Fonte)
    Fonte=Fonte_fun;
  else
    Fonte=Fonte_systeme;

  // Allocation de m�moire pour la brosse
  if (!(Brosse         =(byte *)malloc(   1*   1))) Erreur(ERREUR_MEMOIRE);
  if (!(Smear_Brosse   =(byte *)malloc(TAILLE_MAXI_PINCEAU*TAILLE_MAXI_PINCEAU))) Erreur(ERREUR_MEMOIRE);

  // Pinceau
  if (!(Pinceau_Sprite=(byte *)malloc(TAILLE_MAXI_PINCEAU*TAILLE_MAXI_PINCEAU))) Erreur(ERREUR_MEMOIRE);
  *Pinceau_Sprite=1;
  Pinceau_Largeur=1;
  Pinceau_Hauteur=1;

  // D�tection des modes SDL en �tat de fonctionner:
  Liste_Modes_Videos_SDL= SDL_ListModes(NULL, 0);

//Ici, trier les modes dispos ou pas dans le tableau global ...

  /*
  Support_VESA();
  if (!VESA_Erreur)
  {
    Mode_VESA_supporte(0x100,0);
    Mode_VESA_supporte(0x101,1);
    Mode_VESA_supporte(0x103,2);
    Mode_VESA_supporte(0x105,3);
  }
  else
  //Pas de VESA...
  {
    for (Temp=0; Temp<NB_MODES_VIDEO; Temp++)
    {
      if (Mode_video[Temp].Mode>=MODE_VESA)
        Mode_video[Temp].Etat+=128;
    }
  }*/

  Analyse_de_la_ligne_de_commande(argc,argv);

  Mode_dans_lequel_on_demarre=Resolution_actuelle;
  Buffer_de_ligne_horizontale=NULL;
  Resolution_actuelle=-1; // On n'�tait pas dans un mode graphique
  Initialiser_mode_video(Mode_dans_lequel_on_demarre);

  Principal_Largeur_image=Largeur_ecran;
  Principal_Hauteur_image=Hauteur_ecran;
  Brouillon_Largeur_image=Largeur_ecran;
  Brouillon_Hauteur_image=Hauteur_ecran;

  // Allocation de m�moire pour les diff�rents �crans virtuels (et brosse)
  if (Initialiser_les_listes_de_backups_en_debut_de_programme(Config.Nb_pages_Undo+1,Largeur_ecran,Hauteur_ecran)==0)
    Erreur(ERREUR_MEMOIRE);

  // On remet le nom par d�faut pour la page de brouillon car il �t� modifi�
  // par le passage d'un fichier en param�tre lors du traitement pr�c�dent.
  // Note: le fait que l'on ne modifie que les variables globales Brouillon_*
  // et pas les infos contenues dans la page de brouillon elle-m�me ne m'ins-
  // -pire pas confiance mais �a a l'air de marcher sans poser de probl�mes,
  // alors...
  if (Un_fichier_a_ete_passe_en_parametre)
  {
    strcpy(Brouillon_Repertoire_fichier,Brouillon_Repertoire_courant);
    strcpy(Brouillon_Nom_fichier,"NO_NAME.PKM");
    Brouillon_Format_fichier=FORMAT_PAR_DEFAUT;
  }

  // Nettoyage de l'�cran virtuel (les autres recevront celui-ci par copie)
  memset(Principal_Ecran,0,Principal_Largeur_image*Principal_Hauteur_image);

  // Initialisation de diverses variables par calcul:
  Calculer_donnees_loupe();
  Calculer_limites();
  Calculer_coordonnees_pinceau();

  // On affiche le menu:
  Afficher_menu();
  Afficher_pinceau_dans_menu();

  // On affiche le curseur pour d�butter correctement l'�tat du programme:
  Afficher_curseur();

  Brouillon_Image_modifiee=0;
  Principal_Image_modifiee=0;

  // Le programme d�bute en mode de dessin � la main
  Enclencher_bouton(BOUTON_DESSIN,A_GAUCHE);

  // On initialise la brosse initiale � 1 pixel blanc:
  Brosse_Largeur=1;
  Brosse_Hauteur=1;
  Capturer_brosse(0,0,0,0,0);
  *Brosse=CM_Blanc;
}

// ------------------------- Fermeture du programme --------------------------
void Fermeture_du_programme(void)
{
  unsigned Bidon;
  int      Retour;

  // On lib�re le buffer de gestion de lignes
  free(Buffer_de_ligne_horizontale);

  // On lib�re le pinceau sp�cial
  free(Pinceau_Sprite);

  // On lib�re la table pr�calcul�e des distances de teintes
  free(MC_Table_differences);

  // On lib�re les diff�rents �crans virtuels et brosse:
  free(Brosse);
  Nouveau_nombre_de_backups(0);
  free(Brouillon_Ecran);
  free(Principal_Ecran);

  // On lib�re �galement les donn�es de l'aide:
  for (Bidon=0;Bidon<NB_SECTIONS_AIDE;Bidon++)
    free(Table_d_aide[Bidon].Debut_de_la_liste);

  Passer_en_mode_texte(Ancien_nb_lignes);
  Clavier_de_depart();

  // On prend bien soin de passer dans le r�pertoire initial:
  if (chdir(Repertoire_initial)!=-1)
  {
    /* A revoir ...
    _dos_setdrive(Repertoire_initial[0]-64,&Bidon);
    */
    // On sauvegarde les donn�es dans le .CFG et dans le .INI
    if (Config.Auto_save)
    {
      Retour=Sauver_CFG();
      if (Retour)
        Erreur(Retour);
      Retour=Sauver_INI(&Config);
      if (Retour)
        Erreur(Retour);
    }
  }
  else
    Erreur(ERREUR_REPERTOIRE_DISPARU);
}


// -------------------------- Proc�dure principale ---------------------------
int main(int argc,char * argv[])
{
  Initialisation_du_programme(argc,argv);

  if (Config.Opening_message && (!Un_fichier_a_ete_passe_en_parametre))
    Bouton_Message_initial();
  free(Logo_GrafX2);

  if (Un_fichier_a_ete_passe_en_parametre)
  {
    Bouton_reload();
    Une_resolution_a_ete_passee_en_parametre=0;
  }

  Gestion_principale();

  Fermeture_du_programme();
  return 0;
}
