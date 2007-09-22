#define TAILLE_FICHIER_DATA  84369  // Taille du fichier GFX2.DAT

#include <fcntl.h>
#include "const.h"
#include "struct.h"
#include "global.h"
#include "modesvdo.h"
#include "graph.h"
#include "boutons.h"
#include "palette.h"
#include "aide.h"
#include "operatio.h"
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "divers.h"

#include "errno.h"
#include <endian.h>
#include <byteswap.h>

//  On d�clare m�chamment le prototype de Erreur pour �viter de faire un
// fichier "main.h":
void Erreur(int Code);

// Chercher le r�pertoire contenant GFX2.EXE
void Chercher_repertoire_du_programme(char * Chaine)
{
  /*int Position;

  strcpy(Repertoire_du_programme,Chaine);
  for (Position=strlen(Repertoire_du_programme);Repertoire_du_programme[Position]!='/';Position--); //sous linux c'est un /, sous windows c'est \\
  Repertoire_du_programme[Position+1]='\0';*/
  puts("Chercher_repertoire_du_programme: impl�mentation incompl�te");
  Repertoire_du_programme[0]=0; //On va travailler dans le dossier courant ...
}


word Drive_Touche[26]=
{
  0x041E,
  0x0430,
  0x042E,
  0x0420,
  0x0412,
  0x0421,
  0x0422,
  0x0423,
  0x0417,
  0x0424,
  0x0425,
  0x0426,
  0x0432,
  0x0431,
  0x0418,
  0x0419,
  0x0410,
  0x0413,
  0x041F,
  0x0414,
  0x0416,
  0x042F,
  0x0411,
  0x042D,
  0x0415,
  0x042C
};
// Ajouter un lecteur � la liste de lecteurs
void Ajouter_lecteur(byte Numero, byte Type)
{
  Drive[Nb_drives].Lettre=Numero+65;
  Drive[Nb_drives].Type  =Type;
  Drive[Nb_drives].Touche=Drive_Touche[Numero];

  Nb_drives++;
}


// Rechercher la liste et le type des lecteurs de la machine
void Rechercher_drives(void)
{
/*
  byte Lecteur;
  byte Nb_lecteurs_disquettes;
  byte Lecteur_de_disquettes;
  byte Type_de_lecteur=42;
  //char Bidon[256];

  Nb_drives=0;
  Nb_lecteurs_disquettes=(Type_de_lecteur_de_disquette(0)>0)+(Type_de_lecteur_de_disquette(1)>0);

  // Test du type des lecteurs A: et B:
  if (Nb_lecteurs_disquettes==2)
  for (Lecteur=0; Lecteur<=1; Lecteur++)
  {
    switch (Type_de_lecteur_de_disquette(Lecteur))
    {
      case 1 :
      case 2 :
        Ajouter_lecteur(Lecteur,DRIVE_FLOPPY_5_25);
        break;
      default:
        Ajouter_lecteur(Lecteur,DRIVE_FLOPPY_3_5);
    }
  }
  else // On n'a pas 2 lecteurs donc on regarde si "logiquement" c'est A: ou B:
  if (Nb_lecteurs_disquettes==1)
  {
    if (Disk_map(2)==Disk_map(1))
    {
      // Il n'y a pas de lecteur �mul� par un SUBST
      Lecteur_de_disquettes=Disk_map(1)-1;
      for (Lecteur=0; Lecteur<=1; Lecteur++)
      {
        switch (Type_de_lecteur_de_disquette(Lecteur))
        {
          case 0 :
            break;
          case 1 :
          case 2 :
            Ajouter_lecteur(Lecteur_de_disquettes,DRIVE_FLOPPY_5_25);
            break;
          default:
            Ajouter_lecteur(Lecteur_de_disquettes,DRIVE_FLOPPY_3_5);
        }
      }
    }
    else
    {
      // Il y a un lecteur �mul� par un SUBST
      Lecteur_de_disquettes=Disk_map(1)-1;

      // On cherche d'abord sur quel lecteur le lecteur physique est dispo
      for (Lecteur=0; Lecteur<=1; Lecteur++)
      {
        switch (Type_de_lecteur_de_disquette(Lecteur))
        {
          case 0 :
            break;
          case 1 :
          case 2 :
            Type_de_lecteur=DRIVE_FLOPPY_5_25;
            break;
          default:
            Type_de_lecteur=DRIVE_FLOPPY_3_5;
        }
      }

      // On d�clare les trucs maintenant
      if (Lecteur_de_disquettes==0)
      {
        // Situation : On a un lecteur A: qui est r�el et un lecteur B: �mul�
        Ajouter_lecteur(0,Type_de_lecteur);
        Ajouter_lecteur(1,DRIVE_NETWORK);
      }
      else
      {
        // Situation : On a un lecteur A: qui est r�el et un lecteur B: �mul�
        Ajouter_lecteur(0,DRIVE_NETWORK);
        Ajouter_lecteur(1,Type_de_lecteur);
      }
    }
  }
  else
    //  Il n'y a pas de lecteur de D7 physique, mais on v�rifie s'il n'y en a
    // pas qui seraient �mul�s par SUBST
    for (Lecteur=0; Lecteur<=1; Lecteur++)
    {
      switch (Freespace(Lecteur+1))
      {
        case -1:
          break;
        default:
          Ajouter_lecteur(Lecteur,DRIVE_NETWORK);
      }
    }

  // Test de la pr�sence d'autres lecteurs (HDD, CD, R�seau)
  // On les met tous en r�seau avant de tester leur vrai type.
  for (Lecteur=2; Lecteur<=25; Lecteur++)
  {
    if (Disque_dur_present(Lecteur-2))
      Ajouter_lecteur(Lecteur,DRIVE_HDD);
    else
    if (Lecteur_CDROM_present(Lecteur))
      Ajouter_lecteur(Lecteur,DRIVE_CDROM);
    else
    if (Freespace(Lecteur+1)!=-1)
      Ajouter_lecteur(Lecteur,DRIVE_NETWORK);
  }
*/
//Sous linux, il n'y a pas de lecteurs, on va juste mettre un disque dur qui pointera vers la racine
Ajouter_lecteur(0,DRIVE_HDD); //Le lecteur num�ro 0 est un disque dur.
}


// Fonction de d�cryptage

  #define DECRYPT_TAILLE_CLE 14
  byte Decrypt_compteur=0;
  char Decrypt_cle[DECRYPT_TAILLE_CLE]="Sunset Design";

  byte Decrypt(byte Octet)
  {
    byte Temp;

    Temp=Octet ^ Decrypt_cle[Decrypt_compteur];
    if ((++Decrypt_compteur)>=(DECRYPT_TAILLE_CLE-1))
      Decrypt_compteur=0;
    return Temp;
  }

// D�cryptage d'une donn�e

void Decrypte(byte * Donnee,int Taille)
{
  int Indice;

  for (Indice=0;Indice<Taille;Indice++)
    *(Donnee+Indice)=Decrypt(*(Donnee+Indice));
}


void Charger_DAT(void)
{
  int  Handle;
  int  Taille_fichier;
  int  Indice;
  char Nom_du_fichier[256];
  byte * Fonte_temporaire;
  byte Pos_X;
  byte Pos_Y;
  word Mot_temporaire;

  struct stat Informations_Fichier;

  strcpy(Nom_du_fichier,Repertoire_du_programme);
  strcat(Nom_du_fichier,"gfx2.dat");

  if(stat(Nom_du_fichier,&Informations_Fichier))
	switch errno
		{
			case EACCES: puts("La permission de parcours est refus�e pour un des r�pertoires contenu dans le chemin path."); break;
			case EBADF:  puts("filedes est un mauvais descripteur."); break;
			case EFAULT: puts("Un pointeur se trouve en dehors de l'espace d'adressage."); break;
			case ELOOP:  puts("Trop de liens symboliques rencontr�s dans le chemin d'acc�s."); break;
			case ENAMETOOLONG: puts("Nom de fichier trop long."); break;
			case ENOENT: puts("Un composant du chemin path n'existe pas, ou il s'agit d'une cha�ne vide."); break;
			case ENOMEM: puts("Pas assez de m�moire pour le noyau."); break;
			case ENOTDIR: puts("Un composant du chemin d'acc�s n'est pas un r�pertoire."); break;
		}
  Taille_fichier=Informations_Fichier.st_size;
  if (Taille_fichier!=TAILLE_FICHIER_DATA)
    Erreur(ERREUR_DAT_CORROMPU);
  
	Handle=open(Nom_du_fichier,O_RDONLY);
	if (Handle==-1)
		Erreur(ERREUR_DAT_ABSENT);

  if (read(Handle,Palette_defaut,sizeof(T_Palette))!=sizeof(T_Palette))
    Erreur(ERREUR_DAT_CORROMPU);
  Decrypte((byte *)Palette_defaut,sizeof(T_Palette));

  if (read(Handle,BLOCK_MENU,LARGEUR_MENU*HAUTEUR_MENU)!=LARGEUR_MENU*HAUTEUR_MENU)
    Erreur(ERREUR_DAT_CORROMPU);
  Decrypte((byte *)BLOCK_MENU,LARGEUR_MENU*HAUTEUR_MENU);

  if (read(Handle,SPRITE_EFFET,LARGEUR_SPRITE_MENU*HAUTEUR_SPRITE_MENU*NB_SPRITES_EFFETS)!=
      LARGEUR_SPRITE_MENU*HAUTEUR_SPRITE_MENU*NB_SPRITES_EFFETS)
    Erreur(ERREUR_DAT_CORROMPU);
  Decrypte((byte *)SPRITE_EFFET,LARGEUR_SPRITE_MENU*HAUTEUR_SPRITE_MENU*NB_SPRITES_EFFETS);

  if (read(Handle,SPRITE_CURSEUR,LARGEUR_SPRITE_CURSEUR*HAUTEUR_SPRITE_CURSEUR*NB_SPRITES_CURSEUR)!=
      LARGEUR_SPRITE_CURSEUR*HAUTEUR_SPRITE_CURSEUR*NB_SPRITES_CURSEUR)
    Erreur(ERREUR_DAT_CORROMPU);
  Decrypte((byte *)SPRITE_CURSEUR,LARGEUR_SPRITE_CURSEUR*HAUTEUR_SPRITE_CURSEUR*NB_SPRITES_CURSEUR);

  if (read(Handle,SPRITE_MENU,LARGEUR_SPRITE_MENU*HAUTEUR_SPRITE_MENU*NB_SPRITES_MENU)!=
      LARGEUR_SPRITE_MENU*HAUTEUR_SPRITE_MENU*NB_SPRITES_MENU)
    Erreur(ERREUR_DAT_CORROMPU);
  Decrypte((byte *)SPRITE_MENU,LARGEUR_SPRITE_MENU*HAUTEUR_SPRITE_MENU*NB_SPRITES_MENU);

  if (read(Handle,SPRITE_PINCEAU,LARGEUR_PINCEAU*HAUTEUR_PINCEAU*NB_SPRITES_PINCEAU)!=
      LARGEUR_PINCEAU*HAUTEUR_PINCEAU*NB_SPRITES_PINCEAU)
    Erreur(ERREUR_DAT_CORROMPU);
  Decrypte((byte *)SPRITE_PINCEAU,LARGEUR_PINCEAU*HAUTEUR_PINCEAU*NB_SPRITES_PINCEAU);

  if (read(Handle,SPRITE_DRIVE,LARGEUR_SPRITE_DRIVE*HAUTEUR_SPRITE_DRIVE*NB_SPRITES_DRIVES)!=
      LARGEUR_SPRITE_DRIVE*HAUTEUR_SPRITE_DRIVE*NB_SPRITES_DRIVES)
    Erreur(ERREUR_DAT_CORROMPU);
  Decrypte((byte *)SPRITE_DRIVE,LARGEUR_SPRITE_DRIVE*HAUTEUR_SPRITE_DRIVE*NB_SPRITES_DRIVES);

  if (!(Logo_GrafX2=(byte *)malloc(231*56)))
    Erreur(ERREUR_MEMOIRE);
  if (read(Handle,Logo_GrafX2,231*56)!=(231*56))
    Erreur(ERREUR_DAT_CORROMPU);
  Decrypte(Logo_GrafX2,231*56);

  if (read(Handle,TRAME_PREDEFINIE,2*16*NB_TRAMES_PREDEFINIES)!=2*16*NB_TRAMES_PREDEFINIES)
    Erreur(ERREUR_DAT_CORROMPU);
  Decrypte((byte *)TRAME_PREDEFINIE,2*16*NB_TRAMES_PREDEFINIES);
  
  // Lecture des fontes 8x8:
  if (!(Fonte_temporaire=(byte *)malloc(2048)))
    Erreur(ERREUR_MEMOIRE);

  // Lecture de la fonte syst�me
  if (read(Handle,Fonte_temporaire,2048)!=2048)
    Erreur(ERREUR_DAT_CORROMPU);
  Decrypte(Fonte_temporaire,2048);
  for (Indice=0;Indice<256;Indice++)
    for (Pos_X=0;Pos_X<8;Pos_X++)
      for (Pos_Y=0;Pos_Y<8;Pos_Y++)
        Fonte_systeme[(Indice<<6)+(Pos_X<<3)+Pos_Y]=( ((*(Fonte_temporaire+(Indice*8)+Pos_Y))&(0x80>>Pos_X)) ? 1 : 0);

  // Lecture de la fonte alternative
  if (read(Handle,Fonte_temporaire,2048)!=2048)
    Erreur(ERREUR_DAT_CORROMPU);
  Decrypte(Fonte_temporaire,2048);
  for (Indice=0;Indice<256;Indice++)
    for (Pos_X=0;Pos_X<8;Pos_X++)
      for (Pos_Y=0;Pos_Y<8;Pos_Y++)
        Fonte_fun[(Indice<<6)+(Pos_X<<3)+Pos_Y]=( ((*(Fonte_temporaire+(Indice*8)+Pos_Y))&(0x80>>Pos_X)) ? 1 : 0);

  free(Fonte_temporaire);

  Fonte=Fonte_systeme;

  // Lecture de la fonte 6x8: (sp�ciale aide)
  if (read(Handle,Fonte_help,(315*6*8))!=(315*6*8))
    Erreur(ERREUR_DAT_CORROMPU);
  Decrypte((byte*)Fonte_help,(315*6*8));

  // Lecture des diff�rentes sections de l'aide:

  // Pour chaque section "Indice" de l'aide:
  for (Indice=0;Indice<NB_SECTIONS_AIDE;Indice++)
  {
    // On lit le nombre de lignes:
    if (read(Handle,&Mot_temporaire,2)!=2)
      Erreur(ERREUR_DAT_CORROMPU);

#if __BYTE_ORDER == __BIG_ENDIAN
	Mot_temporaire=bswap_16(Mot_temporaire);
#endif

printf("%d Nb Lignes: %x ",Indice,Mot_temporaire);
    // On copie ce nombre de lignes dans la table:
    Table_d_aide[Indice].Nombre_de_lignes=Mot_temporaire;

    // On lit la place que la section prend en m�moire:
    if (read(Handle,&Mot_temporaire,2)!=2)
      Erreur(ERREUR_DAT_CORROMPU);

#if __BYTE_ORDER == __BIG_ENDIAN
	Mot_temporaire=bswap_16(Mot_temporaire);
#endif

printf("Taille: %x\n",Mot_temporaire);
    // On alloue la m�moire correspondante:
    if (!(Table_d_aide[Indice].Debut_de_la_liste=(byte *)malloc(Mot_temporaire)))
      Erreur(ERREUR_MEMOIRE);

    // Et on lit la section d'aide en question:
    if (read(Handle,Table_d_aide[Indice].Debut_de_la_liste,Mot_temporaire)!=Mot_temporaire)
      Erreur(ERREUR_DAT_CORROMPU);
  }

  close(Handle);
  
  Section_d_aide_en_cours=0;
  Position_d_aide_en_cours=0;

  Pinceau_predefini_Largeur[ 0]= 1;
  Pinceau_predefini_Hauteur[ 0]= 1;
  Pinceau_Type             [ 0]=FORME_PINCEAU_CARRE;

  Pinceau_predefini_Largeur[ 1]= 2;
  Pinceau_predefini_Hauteur[ 1]= 2;
  Pinceau_Type             [ 1]=FORME_PINCEAU_CARRE;

  Pinceau_predefini_Largeur[ 2]= 3;
  Pinceau_predefini_Hauteur[ 2]= 3;
  Pinceau_Type             [ 2]=FORME_PINCEAU_CARRE;

  Pinceau_predefini_Largeur[ 3]= 4;
  Pinceau_predefini_Hauteur[ 3]= 4;
  Pinceau_Type             [ 3]=FORME_PINCEAU_CARRE;

  Pinceau_predefini_Largeur[ 4]= 5;
  Pinceau_predefini_Hauteur[ 4]= 5;
  Pinceau_Type             [ 4]=FORME_PINCEAU_CARRE;

  Pinceau_predefini_Largeur[ 5]= 7;
  Pinceau_predefini_Hauteur[ 5]= 7;
  Pinceau_Type             [ 5]=FORME_PINCEAU_CARRE;

  Pinceau_predefini_Largeur[ 6]= 8;
  Pinceau_predefini_Hauteur[ 6]= 8;
  Pinceau_Type             [ 6]=FORME_PINCEAU_CARRE;

  Pinceau_predefini_Largeur[ 7]=12;
  Pinceau_predefini_Hauteur[ 7]=12;
  Pinceau_Type             [ 7]=FORME_PINCEAU_CARRE;

  Pinceau_predefini_Largeur[ 8]=16;
  Pinceau_predefini_Hauteur[ 8]=16;
  Pinceau_Type             [ 8]=FORME_PINCEAU_CARRE;

  Pinceau_predefini_Largeur[ 9]=16;
  Pinceau_predefini_Hauteur[ 9]=16;
  Pinceau_Type             [ 9]=FORME_PINCEAU_CARRE_TRAME;

  Pinceau_predefini_Largeur[10]=15;
  Pinceau_predefini_Hauteur[10]=15;
  Pinceau_Type             [10]=FORME_PINCEAU_LOSANGE;

  Pinceau_predefini_Largeur[11]= 5;
  Pinceau_predefini_Hauteur[11]= 5;
  Pinceau_Type             [11]=FORME_PINCEAU_LOSANGE;

  Pinceau_predefini_Largeur[12]= 3;
  Pinceau_predefini_Hauteur[12]= 3;
  Pinceau_Type             [12]=FORME_PINCEAU_ROND;

  Pinceau_predefini_Largeur[13]= 4;
  Pinceau_predefini_Hauteur[13]= 4;
  Pinceau_Type             [13]=FORME_PINCEAU_ROND;

  Pinceau_predefini_Largeur[14]= 5;
  Pinceau_predefini_Hauteur[14]= 5;
  Pinceau_Type             [14]=FORME_PINCEAU_ROND;

  Pinceau_predefini_Largeur[15]= 6;
  Pinceau_predefini_Hauteur[15]= 6;
  Pinceau_Type             [15]=FORME_PINCEAU_ROND;

  Pinceau_predefini_Largeur[16]= 8;
  Pinceau_predefini_Hauteur[16]= 8;
  Pinceau_Type             [16]=FORME_PINCEAU_ROND;

  Pinceau_predefini_Largeur[17]=10;
  Pinceau_predefini_Hauteur[17]=10;
  Pinceau_Type             [17]=FORME_PINCEAU_ROND;

  Pinceau_predefini_Largeur[18]=12;
  Pinceau_predefini_Hauteur[18]=12;
  Pinceau_Type             [18]=FORME_PINCEAU_ROND;

  Pinceau_predefini_Largeur[19]=14;
  Pinceau_predefini_Hauteur[19]=14;
  Pinceau_Type             [19]=FORME_PINCEAU_ROND;

  Pinceau_predefini_Largeur[20]=16;
  Pinceau_predefini_Hauteur[20]=16;
  Pinceau_Type             [20]=FORME_PINCEAU_ROND;

  Pinceau_predefini_Largeur[21]=15;
  Pinceau_predefini_Hauteur[21]=15;
  Pinceau_Type             [21]=FORME_PINCEAU_ROND_TRAME;

  Pinceau_predefini_Largeur[22]=11;
  Pinceau_predefini_Hauteur[22]=11;
  Pinceau_Type             [22]=FORME_PINCEAU_ROND_TRAME;

  Pinceau_predefini_Largeur[23]= 5;
  Pinceau_predefini_Hauteur[23]= 5;
  Pinceau_Type             [23]=FORME_PINCEAU_ROND_TRAME;

  Pinceau_predefini_Largeur[24]= 2;
  Pinceau_predefini_Hauteur[24]= 1;
  Pinceau_Type             [24]=FORME_PINCEAU_BARRE_HORIZONTALE;

  Pinceau_predefini_Largeur[25]= 3;
  Pinceau_predefini_Hauteur[25]= 1;
  Pinceau_Type             [25]=FORME_PINCEAU_BARRE_HORIZONTALE;

  Pinceau_predefini_Largeur[26]= 4;
  Pinceau_predefini_Hauteur[26]= 1;
  Pinceau_Type             [26]=FORME_PINCEAU_BARRE_HORIZONTALE;

  Pinceau_predefini_Largeur[27]= 8;
  Pinceau_predefini_Hauteur[27]= 1;
  Pinceau_Type             [27]=FORME_PINCEAU_BARRE_HORIZONTALE;

  Pinceau_predefini_Largeur[28]= 1;
  Pinceau_predefini_Hauteur[28]= 2;
  Pinceau_Type             [28]=FORME_PINCEAU_BARRE_VERTICALE;

  Pinceau_predefini_Largeur[29]= 1;
  Pinceau_predefini_Hauteur[29]= 3;
  Pinceau_Type             [29]=FORME_PINCEAU_BARRE_VERTICALE;

  Pinceau_predefini_Largeur[30]= 1;
  Pinceau_predefini_Hauteur[30]= 4;
  Pinceau_Type             [30]=FORME_PINCEAU_BARRE_VERTICALE;

  Pinceau_predefini_Largeur[31]= 1;
  Pinceau_predefini_Hauteur[31]= 8;
  Pinceau_Type             [31]=FORME_PINCEAU_BARRE_VERTICALE;

  Pinceau_predefini_Largeur[32]= 3;
  Pinceau_predefini_Hauteur[32]= 3;
  Pinceau_Type             [32]=FORME_PINCEAU_X;

  Pinceau_predefini_Largeur[33]= 5;
  Pinceau_predefini_Hauteur[33]= 5;
  Pinceau_Type             [33]=FORME_PINCEAU_X;

  Pinceau_predefini_Largeur[34]= 5;
  Pinceau_predefini_Hauteur[34]= 5;
  Pinceau_Type             [34]=FORME_PINCEAU_PLUS;

  Pinceau_predefini_Largeur[35]=15;
  Pinceau_predefini_Hauteur[35]=15;
  Pinceau_Type             [35]=FORME_PINCEAU_PLUS;

  Pinceau_predefini_Largeur[36]= 2;
  Pinceau_predefini_Hauteur[36]= 2;
  Pinceau_Type             [36]=FORME_PINCEAU_SLASH;

  Pinceau_predefini_Largeur[37]= 4;
  Pinceau_predefini_Hauteur[37]= 4;
  Pinceau_Type             [37]=FORME_PINCEAU_SLASH;

  Pinceau_predefini_Largeur[38]= 8;
  Pinceau_predefini_Hauteur[38]= 8;
  Pinceau_Type             [38]=FORME_PINCEAU_SLASH;

  Pinceau_predefini_Largeur[39]= 2;
  Pinceau_predefini_Hauteur[39]= 2;
  Pinceau_Type             [39]=FORME_PINCEAU_ANTISLASH;

  Pinceau_predefini_Largeur[40]= 4;
  Pinceau_predefini_Hauteur[40]= 4;
  Pinceau_Type             [40]=FORME_PINCEAU_ANTISLASH;

  Pinceau_predefini_Largeur[41]= 8;
  Pinceau_predefini_Hauteur[41]= 8;
  Pinceau_Type             [41]=FORME_PINCEAU_ANTISLASH;

  Pinceau_predefini_Largeur[42]= 4;
  Pinceau_predefini_Hauteur[42]= 4;
  Pinceau_Type             [42]=FORME_PINCEAU_ALEATOIRE;

  Pinceau_predefini_Largeur[43]= 8;
  Pinceau_predefini_Hauteur[43]= 8;
  Pinceau_Type             [43]=FORME_PINCEAU_ALEATOIRE;

  Pinceau_predefini_Largeur[44]=13;
  Pinceau_predefini_Hauteur[44]=13;
  Pinceau_Type             [44]=FORME_PINCEAU_ALEATOIRE;

  Pinceau_predefini_Largeur[45]= 3;
  Pinceau_predefini_Hauteur[45]= 3;
  Pinceau_Type             [45]=FORME_PINCEAU_DIVERS;

  Pinceau_predefini_Largeur[46]= 3;
  Pinceau_predefini_Hauteur[46]= 3;
  Pinceau_Type             [46]=FORME_PINCEAU_DIVERS;

  Pinceau_predefini_Largeur[47]= 7;
  Pinceau_predefini_Hauteur[47]= 7;
  Pinceau_Type             [47]=FORME_PINCEAU_DIVERS;

  for (Indice=0;Indice<NB_SPRITES_PINCEAU;Indice++)
  {
    Pinceau_predefini_Decalage_X[Indice]=(Pinceau_predefini_Largeur[Indice]>>1);
    Pinceau_predefini_Decalage_Y[Indice]=(Pinceau_predefini_Hauteur[Indice]>>1);
  }

  Curseur_Decalage_X[FORME_CURSEUR_FLECHE]=0;
  Curseur_Decalage_Y[FORME_CURSEUR_FLECHE]=0;

  Curseur_Decalage_X[FORME_CURSEUR_CIBLE]=7;
  Curseur_Decalage_Y[FORME_CURSEUR_CIBLE]=7;

  Curseur_Decalage_X[FORME_CURSEUR_CIBLE_PIPETTE]=7;
  Curseur_Decalage_Y[FORME_CURSEUR_CIBLE_PIPETTE]=7;

  Curseur_Decalage_X[FORME_CURSEUR_SABLIER]=7;
  Curseur_Decalage_Y[FORME_CURSEUR_SABLIER]=7;

  Curseur_Decalage_X[FORME_CURSEUR_MULTIDIRECTIONNEL]=7;
  Curseur_Decalage_Y[FORME_CURSEUR_MULTIDIRECTIONNEL]=7;

  Curseur_Decalage_X[FORME_CURSEUR_HORIZONTAL]=7;
  Curseur_Decalage_Y[FORME_CURSEUR_HORIZONTAL]=3;

  Curseur_Decalage_X[FORME_CURSEUR_CIBLE_FINE]=7;
  Curseur_Decalage_Y[FORME_CURSEUR_CIBLE_FINE]=7;

  Curseur_Decalage_X[FORME_CURSEUR_CIBLE_PIPETTE_FINE]=7;
  Curseur_Decalage_Y[FORME_CURSEUR_CIBLE_PIPETTE_FINE]=7;
}


// Initialisation des boutons:

  // Action factice:

void Rien_du_tout(void)
{}

  // Initialiseur d'un bouton:

void Initialiser_bouton(byte   Numero,
                        word   Decalage_X      , word   Decalage_Y,
                        word   Largeur         , word   Hauteur,
                        byte   Forme,
                        fonction_action Gauche , fonction_action Droite,
                        fonction_action Desenclencher,
                        byte   Famille)
{
  Bouton[Numero].Decalage_X      =Decalage_X;
  Bouton[Numero].Decalage_Y      =Decalage_Y;
  Bouton[Numero].Largeur         =Largeur-1;
  Bouton[Numero].Hauteur         =Hauteur-1;
  Bouton[Numero].Enfonce         =0;
  Bouton[Numero].Forme           =Forme;
  Bouton[Numero].Gauche          =Gauche;
  Bouton[Numero].Droite          =Droite;
  Bouton[Numero].Desenclencher   =Desenclencher;
  Bouton[Numero].Famille         =Famille;
}


  // Initiliseur de tous les boutons:

void Initialisation_des_boutons(void)
{
  byte Indice_bouton;

  for (Indice_bouton=0;Indice_bouton<NB_BOUTONS;Indice_bouton++)
  {
    Bouton[Indice_bouton].Raccourci_gauche=0xFFFF;
    Bouton[Indice_bouton].Raccourci_droite=0xFFFF;
    Initialiser_bouton(Indice_bouton,
                       0,0,
                       1,1,
                       FORME_BOUTON_RECTANGLE,
                       Rien_du_tout,Rien_du_tout,
                       Rien_du_tout,
                       0);
  }

  // Ici viennent les d�clarations des boutons que l'on sait g�rer

  Initialiser_bouton(BOUTON_PINCEAUX,
                     0,1,
                     16,16,
                     FORME_BOUTON_RECTANGLE,
                     Bouton_Menu_pinceaux,Bouton_Brosse_monochrome,
                     Rien_du_tout,
                     FAMILLE_INSTANTANE);

// !!! TEMPORAIRE !!!
  Initialiser_bouton(BOUTON_AJUSTER,
                     0,18,
                     16,16,
                     FORME_BOUTON_RECTANGLE,
                     Bouton_Ajuster,Bouton_Ajuster,
                     Rien_du_tout,
                     FAMILLE_OUTIL);
/*
  Initialiser_bouton(BOUTON_AJUSTER,
                     0,18,
                     16,16,
                     FORME_BOUTON_RECTANGLE,
                     Bouton_Ajuster,Rien_du_tout,
                     Rien_du_tout,
                     FAMILLE_OUTIL);
*/

  Initialiser_bouton(BOUTON_DESSIN,
                     17,1,
                     16,16,
                     FORME_BOUTON_RECTANGLE,
                     Bouton_Dessin,Bouton_Dessin_Switch_mode,
                     Rien_du_tout,
                     FAMILLE_OUTIL);

  Initialiser_bouton(BOUTON_COURBES,
                     17,18,
                     16,16,
                     FORME_BOUTON_RECTANGLE,
                     Bouton_Courbes,Bouton_Courbes_Switch_mode,
                     Rien_du_tout,
                     FAMILLE_OUTIL);

  Initialiser_bouton(BOUTON_LIGNES,
                     34,1,
                     16,16,
                     FORME_BOUTON_RECTANGLE,
                     Bouton_Lignes,Bouton_Lignes_Switch_mode,
                     Rien_du_tout,
                     FAMILLE_OUTIL);

  Initialiser_bouton(BOUTON_SPRAY,
                     34,18,
                     16,16,
                     FORME_BOUTON_RECTANGLE,
                     Bouton_Spray,Bouton_Spray_Menu,
                     Rien_du_tout,
                     FAMILLE_OUTIL);

  Initialiser_bouton(BOUTON_FLOODFILL,
                     51,1,
                     16,16,
                     FORME_BOUTON_RECTANGLE,
                     Bouton_Fill,Bouton_Remplacer,
                     Bouton_desenclencher_Fill,
                     FAMILLE_OUTIL);

  Initialiser_bouton(BOUTON_POLYGONES,
                     51,18,
                     15,15,
                     FORME_BOUTON_TRIANGLE_HAUT_GAUCHE,
                     Bouton_Polygone,Bouton_Polyform,
                     Rien_du_tout,
                     FAMILLE_OUTIL);

  Initialiser_bouton(BOUTON_POLYFILL,
                     52,19,
                     15,15,
                     FORME_BOUTON_TRIANGLE_BAS_DROITE,
                     Bouton_Polyfill,Bouton_Filled_polyform,
                     Rien_du_tout,
                     FAMILLE_OUTIL);

  Initialiser_bouton(BOUTON_RECTANGLES,
                     68,1,
                     15,15,
                     FORME_BOUTON_TRIANGLE_HAUT_GAUCHE,
                     Bouton_Rectangle_vide,Bouton_Rectangle_vide,
                     Rien_du_tout,
                     FAMILLE_OUTIL);

  Initialiser_bouton(BOUTON_FILLRECT,
                     69,2,
                     15,15,
                     FORME_BOUTON_TRIANGLE_BAS_DROITE,
                     Bouton_Rectangle_plein,Bouton_Rectangle_plein,
                     Rien_du_tout,
                     FAMILLE_OUTIL);

  Initialiser_bouton(BOUTON_CERCLES,
                     68,18,
                     15,15,
                     FORME_BOUTON_TRIANGLE_HAUT_GAUCHE,
                     Bouton_Cercle_vide,Bouton_Ellipse_vide,
                     Rien_du_tout,
                     FAMILLE_OUTIL);

  Initialiser_bouton(BOUTON_FILLCERC,
                     69,19,
                     15,15,
                     FORME_BOUTON_TRIANGLE_BAS_DROITE,
		     Bouton_Cercle_plein,Bouton_Ellipse_pleine,
                     Rien_du_tout,
                     FAMILLE_OUTIL);

// !!! TEMPORAIRE !!!
  Initialiser_bouton(BOUTON_GRADRECT,
                     85,1,
                     15,15,
                     FORME_BOUTON_TRIANGLE_HAUT_GAUCHE,
                     Message_Non_disponible,Message_Non_disponible,
                     Rien_du_tout,
                     FAMILLE_INSTANTANE);
/*
  Initialiser_bouton(BOUTON_GRADRECT,
                     85,1,
                     15,15,
                     FORME_BOUTON_TRIANGLE_HAUT_GAUCHE,
                     Rien_du_tout,Rien_du_tout,
                     Rien_du_tout,
                     FAMILLE_OUTIL);
*/

  Initialiser_bouton(BOUTON_GRADMENU,
                     86,2,
                     15,15,
                     FORME_BOUTON_TRIANGLE_BAS_DROITE,
                     Bouton_Degrades,Bouton_Degrades,
                     Rien_du_tout,
                     FAMILLE_INSTANTANE);

  Initialiser_bouton(BOUTON_SPHERES,
                     85,18,
                     16,16,
                     FORME_BOUTON_RECTANGLE,
                     Bouton_Cercle_degrade,Bouton_Ellipse_degrade,
                     Rien_du_tout,
                     FAMILLE_OUTIL);

  Initialiser_bouton(BOUTON_BROSSE,
                     106,1,
                     15,15,
                     FORME_BOUTON_TRIANGLE_HAUT_GAUCHE,
                     Bouton_Brosse,Bouton_Restaurer_brosse,
                     Bouton_desenclencher_Brosse,
                     FAMILLE_INTERRUPTION);

  Initialiser_bouton(BOUTON_POLYBROSSE,
                     107,2,
                     15,15,
                     FORME_BOUTON_TRIANGLE_BAS_DROITE,
                     Bouton_Lasso,Bouton_Restaurer_brosse,
                     Bouton_desenclencher_Lasso,
                     FAMILLE_INTERRUPTION);

  Initialiser_bouton(BOUTON_EFFETS_BROSSE,
                     106,18,
                     16,16,
		     FORME_BOUTON_RECTANGLE,
                     Bouton_Brush_FX,Bouton_Brush_FX,
                     Rien_du_tout,
                     FAMILLE_INSTANTANE);

  Initialiser_bouton(BOUTON_EFFETS,
		     123,1,
                     16,16,
                     FORME_BOUTON_RECTANGLE,
                     Bouton_Effets,Bouton_Effets,
                     Rien_du_tout,
                     FAMILLE_EFFETS);

// !!! TEMPORAIRE !!!
  Initialiser_bouton(BOUTON_TEXTE,
                     123,18,
                     16,16,
                     FORME_BOUTON_RECTANGLE,
                     Message_Non_disponible,Message_Non_disponible,
                     Rien_du_tout,
                     FAMILLE_INSTANTANE);
/*
  Initialiser_bouton(BOUTON_TEXTE,
                     123,18,
                     16,16,
                     FORME_BOUTON_RECTANGLE,
                     Rien_du_tout,Rien_du_tout,
                     Rien_du_tout,
                     FAMILLE_INSTANTANE);
*/

  Initialiser_bouton(BOUTON_LOUPE,
                     140,1,
                     16,16,
                     FORME_BOUTON_RECTANGLE,
                     Bouton_Loupe,Bouton_Menu_Loupe,
                     Bouton_desenclencher_Loupe,
                     FAMILLE_INTERRUPTION);

  Initialiser_bouton(BOUTON_PIPETTE,
                     140,18,
                     16,16,
                     FORME_BOUTON_RECTANGLE,
                     Bouton_Pipette,Bouton_Inverser_foreback,
                     Bouton_desenclencher_Pipette,
                     FAMILLE_INTERRUPTION);

  Initialiser_bouton(BOUTON_RESOL,
                     161,1,
                     16,16,
                     FORME_BOUTON_RECTANGLE,
                     Bouton_Resol,Bouton_Safety_resol,
                     Rien_du_tout,
                     FAMILLE_INSTANTANE);

  Initialiser_bouton(BOUTON_PAGE,
                     161,18,
                     16,16,
                     FORME_BOUTON_RECTANGLE,
                     Bouton_Page,Bouton_Copy_page,
                     Rien_du_tout,
                     FAMILLE_INSTANTANE);

  Initialiser_bouton(BOUTON_SAUVER,
                     178,1,
                     15,15,
                     FORME_BOUTON_TRIANGLE_HAUT_GAUCHE,
                     Bouton_Save,Bouton_Autosave,
                     Rien_du_tout,
                     FAMILLE_INSTANTANE);

  Initialiser_bouton(BOUTON_CHARGER,
                     179,2,
                     15,15,
                     FORME_BOUTON_TRIANGLE_BAS_DROITE,
                     Bouton_Load,Bouton_Reload,
                     Rien_du_tout,
                     FAMILLE_INSTANTANE);

  Initialiser_bouton(BOUTON_PARAMETRES,
                     178,18,
                     16,16,
                     FORME_BOUTON_RECTANGLE,
                     Bouton_Settings,Bouton_Settings,
                     Rien_du_tout,
                     FAMILLE_INSTANTANE);

  Initialiser_bouton(BOUTON_CLEAR,
                     195,1,
                     17,16,
                     FORME_BOUTON_RECTANGLE,
                     Bouton_Clear,Bouton_Clear_colore,
                     Rien_du_tout,
                     FAMILLE_INSTANTANE);

  Initialiser_bouton(BOUTON_AIDE,
                     195,18,
                     17,16,
                     FORME_BOUTON_RECTANGLE,
                     Bouton_Aide,Bouton_Stats,
                     Rien_du_tout,
                     FAMILLE_INSTANTANE);

  Initialiser_bouton(BOUTON_UNDO,
                     213,1,
                     19,12,
                     FORME_BOUTON_RECTANGLE,
                     Bouton_Undo,Bouton_Redo,
                     Rien_du_tout,
                     FAMILLE_INSTANTANE);

  Initialiser_bouton(BOUTON_KILL,
                     213,14,
                     19,7,
                     FORME_BOUTON_RECTANGLE,
                     Bouton_Kill,Bouton_Kill,
                     Rien_du_tout,
                     FAMILLE_INSTANTANE);

  Initialiser_bouton(BOUTON_QUIT,
                     213,22,
                     19,12,
                     FORME_BOUTON_RECTANGLE,
                     Bouton_Quit,Bouton_Quit,
                     Rien_du_tout,
                     FAMILLE_INSTANTANE);

  Initialiser_bouton(BOUTON_PALETTE,
                     237,9,
                     16,8,
                     FORME_BOUTON_RECTANGLE,
                     Bouton_Palette,Bouton_Palette_secondaire,
                     Rien_du_tout,
                     FAMILLE_INSTANTANE);

  Initialiser_bouton(BOUTON_PAL_LEFT,
                     237,18,
                     15,15,
                     FORME_BOUTON_TRIANGLE_HAUT_GAUCHE,
                     Bouton_Pal_left,Bouton_Pal_left_fast,
                     Rien_du_tout,
                     FAMILLE_INSTANTANE);

  Initialiser_bouton(BOUTON_PAL_RIGHT,
                     238,19,
                     15,15,
                     FORME_BOUTON_TRIANGLE_BAS_DROITE,
                     Bouton_Pal_right,Bouton_Pal_right_fast,
                     Rien_du_tout,
                     FAMILLE_INSTANTANE);

  Initialiser_bouton(BOUTON_CHOIX_COL,
                     LARGEUR_MENU+1,2,
                     1,32, // La largeur est mise � jour � chq chngmnt de mode
                     FORME_BOUTON_SANS_CADRE,
                     Bouton_Choix_forecolor,Bouton_Choix_backcolor,
                     Rien_du_tout,
                     FAMILLE_INSTANTANE);

  Initialiser_bouton(BOUTON_CACHER,
                     0,35,
                     16,9,
                     FORME_BOUTON_RECTANGLE,
                     Bouton_Cacher_menu,Bouton_Cacher_menu,
                     Rien_du_tout,
                     FAMILLE_TOOLBAR);
}



// Initialisation des op�rations:

  // Initialiseur d'une op�ration:

void Initialiser_operation(byte Numero_operation,
                           byte Numero_bouton_souris,
                           byte Taille_de_pile,
                           fonction_action Action,
                           byte Effacer_curseur)
{
  Operation[Numero_operation][Numero_bouton_souris]
           [Taille_de_pile].Action=Action;
  Operation[Numero_operation][Numero_bouton_souris]
           [Taille_de_pile].Effacer_curseur=Effacer_curseur;
}


  // Initiliseur de toutes les op�rations:

void Initialisation_des_operations(void)
{
  byte Numero; // Num�ro de l'option en cours d'auto-initialisation
  byte Bouton; // Bouton souris en cours d'auto-initialisation
  byte Taille; // Taille de la pile en cours d'auto-initialisation

  // Auto-initialisation des op�rations (vers des actions inoffensives)

  for (Numero=0;Numero<NB_OPERATIONS;Numero++)
    for (Bouton=0;Bouton<3;Bouton++)
      for (Taille=0;Taille<TAILLE_PILE_OPERATIONS;Taille++)
        Initialiser_operation(Numero,Bouton,Taille,Print_coordonnees,0);


  // Ici viennent les d�clarations d�taill�es des op�rations
  Initialiser_operation(OPERATION_DESSIN_CONTINU,1,0,
                        Freehand_Mode1_1_0,1);
  Initialiser_operation(OPERATION_DESSIN_CONTINU,1,2,
                        Freehand_Mode1_1_2,0);
  Initialiser_operation(OPERATION_DESSIN_CONTINU,0,2,
                        Freehand_Mode12_0_2,0);
  Initialiser_operation(OPERATION_DESSIN_CONTINU,2,0,
                        Freehand_Mode1_2_0,1);
  Initialiser_operation(OPERATION_DESSIN_CONTINU,2,2,
                        Freehand_Mode1_2_2,0);

  Initialiser_operation(OPERATION_DESSIN_DISCONTINU,1,0,
                        Freehand_Mode2_1_0,1);
  Initialiser_operation(OPERATION_DESSIN_DISCONTINU,1,2,
                        Freehand_Mode2_1_2,0);
  Initialiser_operation(OPERATION_DESSIN_DISCONTINU,0,2,
                        Freehand_Mode12_0_2,0);
  Initialiser_operation(OPERATION_DESSIN_DISCONTINU,2,0,
                        Freehand_Mode2_2_0,1);
  Initialiser_operation(OPERATION_DESSIN_DISCONTINU,2,2,
                        Freehand_Mode2_2_2,0);

  Initialiser_operation(OPERATION_DESSIN_POINT,1,0,
                        Freehand_Mode3_1_0,1);
  Initialiser_operation(OPERATION_DESSIN_POINT,2,0,
                        Freehand_Mode3_2_0,1);
  Initialiser_operation(OPERATION_DESSIN_POINT,0,1,
                        Freehand_Mode3_0_1,0);

  Initialiser_operation(OPERATION_LIGNE,1,0,
                        Ligne_12_0,1);
  Initialiser_operation(OPERATION_LIGNE,1,5,
                        Ligne_12_5,0);
  Initialiser_operation(OPERATION_LIGNE,0,5,
                        Ligne_0_5,1);
  Initialiser_operation(OPERATION_LIGNE,2,0,
                        Ligne_12_0,1);
  Initialiser_operation(OPERATION_LIGNE,2,5,
                        Ligne_12_5,0);

  Initialiser_operation(OPERATION_K_LIGNE,1,0,
                        K_Ligne_12_0,1);
  Initialiser_operation(OPERATION_K_LIGNE,1,6,
                        K_Ligne_12_6,0);
  Initialiser_operation(OPERATION_K_LIGNE,1,7,
                        K_Ligne_12_7,1);
  Initialiser_operation(OPERATION_K_LIGNE,2,0,
                        K_Ligne_12_0,1);
  Initialiser_operation(OPERATION_K_LIGNE,2,6,
                        K_Ligne_12_6,0);
  Initialiser_operation(OPERATION_K_LIGNE,2,7,
                        K_Ligne_12_7,1);
  Initialiser_operation(OPERATION_K_LIGNE,0,6,
                        K_Ligne_0_6,1);
  Initialiser_operation(OPERATION_K_LIGNE,0,7,
                        K_Ligne_12_6,0);

  Initialiser_operation(OPERATION_RECTANGLE_VIDE,1,0,
                        Rectangle_12_0,1);
  Initialiser_operation(OPERATION_RECTANGLE_VIDE,2,0,
                        Rectangle_12_0,1);
  Initialiser_operation(OPERATION_RECTANGLE_VIDE,1,5,
                        Rectangle_12_5,0);
  Initialiser_operation(OPERATION_RECTANGLE_VIDE,2,5,
                        Rectangle_12_5,0);
  Initialiser_operation(OPERATION_RECTANGLE_VIDE,0,5,
                        Rectangle_vide_0_5,1);

  Initialiser_operation(OPERATION_RECTANGLE_PLEIN,1,0,
                        Rectangle_12_0,1);
  Initialiser_operation(OPERATION_RECTANGLE_PLEIN,2,0,
                        Rectangle_12_0,1);
  Initialiser_operation(OPERATION_RECTANGLE_PLEIN,1,5,
                        Rectangle_12_5,0);
  Initialiser_operation(OPERATION_RECTANGLE_PLEIN,2,5,
                        Rectangle_12_5,0);
  Initialiser_operation(OPERATION_RECTANGLE_PLEIN,0,5,
                        Rectangle_plein_0_5,1);

  Initialiser_operation(OPERATION_CERCLE_VIDE,1,0,
                        Cercle_12_0,1);
  Initialiser_operation(OPERATION_CERCLE_VIDE,2,0,
                        Cercle_12_0,1);
  Initialiser_operation(OPERATION_CERCLE_VIDE,1,5,
                        Cercle_12_5,0);
  Initialiser_operation(OPERATION_CERCLE_VIDE,2,5,
                        Cercle_12_5,0);
  Initialiser_operation(OPERATION_CERCLE_VIDE,0,5,
                        Cercle_vide_0_5,1);

  Initialiser_operation(OPERATION_CERCLE_PLEIN,1,0,
                        Cercle_12_0,1);
  Initialiser_operation(OPERATION_CERCLE_PLEIN,2,0,
                        Cercle_12_0,1);
  Initialiser_operation(OPERATION_CERCLE_PLEIN,1,5,
                        Cercle_12_5,0);
  Initialiser_operation(OPERATION_CERCLE_PLEIN,2,5,
                        Cercle_12_5,0);
  Initialiser_operation(OPERATION_CERCLE_PLEIN,0,5,
                        Cercle_plein_0_5,1);

  Initialiser_operation(OPERATION_ELLIPSE_VIDE,1,0,
                        Ellipse_12_0,1);
  Initialiser_operation(OPERATION_ELLIPSE_VIDE,2,0,
                        Ellipse_12_0,1);
  Initialiser_operation(OPERATION_ELLIPSE_VIDE,1,5,
                        Ellipse_12_5,0);
  Initialiser_operation(OPERATION_ELLIPSE_VIDE,2,5,
                        Ellipse_12_5,0);
  Initialiser_operation(OPERATION_ELLIPSE_VIDE,0,5,
                        Ellipse_vide_0_5,1);

  Initialiser_operation(OPERATION_ELLIPSE_PLEINE,1,0,
                        Ellipse_12_0,1);
  Initialiser_operation(OPERATION_ELLIPSE_PLEINE,2,0,
                        Ellipse_12_0,1);
  Initialiser_operation(OPERATION_ELLIPSE_PLEINE,1,5,
                        Ellipse_12_5,0);
  Initialiser_operation(OPERATION_ELLIPSE_PLEINE,2,5,
                        Ellipse_12_5,0);
  Initialiser_operation(OPERATION_ELLIPSE_PLEINE,0,5,
                        Ellipse_pleine_0_5,1);

  Initialiser_operation(OPERATION_FILL,1,0,
                        Fill_1_0,1);
  Initialiser_operation(OPERATION_FILL,2,0,
                        Fill_2_0,1);

  Initialiser_operation(OPERATION_REMPLACER,1,0,
                        Remplacer_1_0,1);
  Initialiser_operation(OPERATION_REMPLACER,2,0,
                        Remplacer_2_0,1);

  Initialiser_operation(OPERATION_PRISE_BROSSE,1,0,
                        Brosse_12_0,1);
  Initialiser_operation(OPERATION_PRISE_BROSSE,2,0,
                        Brosse_12_0,1);
  Initialiser_operation(OPERATION_PRISE_BROSSE,1,5,
                        Brosse_12_5,0);
  Initialiser_operation(OPERATION_PRISE_BROSSE,2,5,
                        Brosse_12_5,0);
  Initialiser_operation(OPERATION_PRISE_BROSSE,0,5,
                        Brosse_0_5,1);

  Initialiser_operation(OPERATION_ETIRER_BROSSE,1,0,
                        Etirer_brosse_12_0,1);
  Initialiser_operation(OPERATION_ETIRER_BROSSE,2,0,
                        Etirer_brosse_12_0,1);
  Initialiser_operation(OPERATION_ETIRER_BROSSE,1,7,
                        Etirer_brosse_1_7,0);
  Initialiser_operation(OPERATION_ETIRER_BROSSE,0,7,
                        Etirer_brosse_0_7,0);
  Initialiser_operation(OPERATION_ETIRER_BROSSE,2,7,
                        Etirer_brosse_2_7,1);

  Initialiser_operation(OPERATION_TOURNER_BROSSE,1,0,
                        Tourner_brosse_12_0,1);
  Initialiser_operation(OPERATION_TOURNER_BROSSE,2,0,
                        Tourner_brosse_12_0,1);
  Initialiser_operation(OPERATION_TOURNER_BROSSE,1,5,
                        Tourner_brosse_1_5,0);
  Initialiser_operation(OPERATION_TOURNER_BROSSE,0,5,
                        Tourner_brosse_0_5,0);
  Initialiser_operation(OPERATION_TOURNER_BROSSE,2,5,
                        Tourner_brosse_2_5,1);

  Initialiser_operation(OPERATION_POLYBROSSE,1,0,
                        Filled_polyform_12_0,1);
  Initialiser_operation(OPERATION_POLYBROSSE,2,0,
                        Filled_polyform_12_0,1);
  Initialiser_operation(OPERATION_POLYBROSSE,1,8,
                        Polybrosse_12_8,0);
  Initialiser_operation(OPERATION_POLYBROSSE,2,8,
                        Polybrosse_12_8,0);
  Initialiser_operation(OPERATION_POLYBROSSE,0,8,
                        Filled_polyform_0_8,0);

  Pipette_Couleur=-1;
  Initialiser_operation(OPERATION_PIPETTE,1,0,
                        Pipette_12_0,1);
  Initialiser_operation(OPERATION_PIPETTE,2,0,
                        Pipette_12_0,0);
  Initialiser_operation(OPERATION_PIPETTE,1,1,
                        Pipette_1_1,0);
  Initialiser_operation(OPERATION_PIPETTE,2,1,
                        Pipette_2_1,0);
  Initialiser_operation(OPERATION_PIPETTE,0,1,
                        Pipette_0_1,1);

  Initialiser_operation(OPERATION_LOUPE,1,0,
                        Loupe_12_0,1);
  Initialiser_operation(OPERATION_LOUPE,2,0,
                        Loupe_12_0,1);

  Initialiser_operation(OPERATION_COURBE_4_POINTS,1,0,
                        Courbe_34_points_1_0,1);
  Initialiser_operation(OPERATION_COURBE_4_POINTS,2,0,
                        Courbe_34_points_2_0,1);
  Initialiser_operation(OPERATION_COURBE_4_POINTS,1,5,
                        Courbe_34_points_1_5,0);
  Initialiser_operation(OPERATION_COURBE_4_POINTS,2,5,
                        Courbe_34_points_2_5,0);
  Initialiser_operation(OPERATION_COURBE_4_POINTS,0,5,
                        Courbe_4_points_0_5,1);
  Initialiser_operation(OPERATION_COURBE_4_POINTS,1,9,
                        Courbe_4_points_1_9,0);
  Initialiser_operation(OPERATION_COURBE_4_POINTS,2,9,
                        Courbe_4_points_2_9,1);

  Initialiser_operation(OPERATION_COURBE_3_POINTS,1,0,
                        Courbe_34_points_1_0,1);
  Initialiser_operation(OPERATION_COURBE_3_POINTS,2,0,
                        Courbe_34_points_2_0,1);
  Initialiser_operation(OPERATION_COURBE_3_POINTS,1,5,
                        Courbe_34_points_1_5,0);
  Initialiser_operation(OPERATION_COURBE_3_POINTS,2,5,
                        Courbe_34_points_2_5,0);
  Initialiser_operation(OPERATION_COURBE_3_POINTS,0,5,
                        Courbe_3_points_0_5,1);
  Initialiser_operation(OPERATION_COURBE_3_POINTS,0,11,
                        Courbe_3_points_0_11,0);
  Initialiser_operation(OPERATION_COURBE_3_POINTS,1,11,
                        Courbe_3_points_12_11,1);
  Initialiser_operation(OPERATION_COURBE_3_POINTS,2,11,
                        Courbe_3_points_12_11,1);

  Initialiser_operation(OPERATION_SPRAY,1,0,
                        Spray_1_0,0);
  Initialiser_operation(OPERATION_SPRAY,2,0,
                        Spray_2_0,0);
  Initialiser_operation(OPERATION_SPRAY,1,3,
                        Spray_12_3,0);
  Initialiser_operation(OPERATION_SPRAY,2,3,
                        Spray_12_3,0);
  Initialiser_operation(OPERATION_SPRAY,0,3,
                        Spray_0_3,0);

  Initialiser_operation(OPERATION_POLYGONE,1,0,
                        Polygone_12_0,1);
  Initialiser_operation(OPERATION_POLYGONE,2,0,
                        Polygone_12_0,1);
  Initialiser_operation(OPERATION_POLYGONE,1,8,
                        K_Ligne_12_6,0);
  Initialiser_operation(OPERATION_POLYGONE,2,8,
                        K_Ligne_12_6,0);
  Initialiser_operation(OPERATION_POLYGONE,1,9,
                        Polygone_12_9,1);
  Initialiser_operation(OPERATION_POLYGONE,2,9,
                        Polygone_12_9,1);
  Initialiser_operation(OPERATION_POLYGONE,0,8,
                        K_Ligne_0_6,1);
  Initialiser_operation(OPERATION_POLYGONE,0,9,
                        K_Ligne_12_6,0);

  Initialiser_operation(OPERATION_POLYFILL,1,0,
                        Polyfill_12_0,1);
  Initialiser_operation(OPERATION_POLYFILL,2,0,
                        Polyfill_12_0,1);
  Initialiser_operation(OPERATION_POLYFILL,1,8,
                        Polyfill_12_8,0);
  Initialiser_operation(OPERATION_POLYFILL,2,8,
                        Polyfill_12_8,0);
  Initialiser_operation(OPERATION_POLYFILL,1,9,
                        Polyfill_12_9,1);
  Initialiser_operation(OPERATION_POLYFILL,2,9,
                        Polyfill_12_9,1);
  Initialiser_operation(OPERATION_POLYFILL,0,8,
                        Polyfill_0_8,1);
  Initialiser_operation(OPERATION_POLYFILL,0,9,
                        Polyfill_12_8,0);

  Initialiser_operation(OPERATION_POLYFORM,1,0,
                        Polyform_12_0,1);
  Initialiser_operation(OPERATION_POLYFORM,2,0,
                        Polyform_12_0,1);
  Initialiser_operation(OPERATION_POLYFORM,1,8,
                        Polyform_12_8,0);
  Initialiser_operation(OPERATION_POLYFORM,2,8,
                        Polyform_12_8,0);
  Initialiser_operation(OPERATION_POLYFORM,0,8,
                        Polyform_0_8,0);

  Initialiser_operation(OPERATION_FILLED_POLYFORM,1,0,
                        Filled_polyform_12_0,1);
  Initialiser_operation(OPERATION_FILLED_POLYFORM,2,0,
                        Filled_polyform_12_0,1);
  Initialiser_operation(OPERATION_FILLED_POLYFORM,1,8,
                        Filled_polyform_12_8,0);
  Initialiser_operation(OPERATION_FILLED_POLYFORM,2,8,
                        Filled_polyform_12_8,0);
  Initialiser_operation(OPERATION_FILLED_POLYFORM,0,8,
                        Filled_polyform_0_8,0);

  Initialiser_operation(OPERATION_SCROLL,1,0,
                        Scroll_12_0,1);
  Initialiser_operation(OPERATION_SCROLL,2,0,
                        Scroll_12_0,1);
  Initialiser_operation(OPERATION_SCROLL,1,4,
                        Scroll_12_4,0);
  Initialiser_operation(OPERATION_SCROLL,2,4,
                        Scroll_12_4,0);
  Initialiser_operation(OPERATION_SCROLL,0,4,
                        Scroll_0_4,1);

  Initialiser_operation(OPERATION_CERCLE_DEGRADE,1,0,
                        Cercle_degrade_12_0,1);
  Initialiser_operation(OPERATION_CERCLE_DEGRADE,2,0,
                        Cercle_degrade_12_0,1);
  Initialiser_operation(OPERATION_CERCLE_DEGRADE,1,6,
                        Cercle_degrade_12_6,0);
  Initialiser_operation(OPERATION_CERCLE_DEGRADE,2,6,
                        Cercle_degrade_12_6,0);
  Initialiser_operation(OPERATION_CERCLE_DEGRADE,0,6,
                        Cercle_degrade_0_6,1);
  Initialiser_operation(OPERATION_CERCLE_DEGRADE,1,8,
                        Cercle_degrade_12_8,1);
  Initialiser_operation(OPERATION_CERCLE_DEGRADE,2,8,
                        Cercle_degrade_12_8,1);
  Initialiser_operation(OPERATION_CERCLE_DEGRADE,0,8,
                        Cercle_ou_ellipse_degrade_0_8,0);

  Initialiser_operation(OPERATION_ELLIPSE_DEGRADEE,0,8,
                        Cercle_ou_ellipse_degrade_0_8,0);
  Initialiser_operation(OPERATION_ELLIPSE_DEGRADEE,1,0,
                        Ellipse_degradee_12_0,1);
  Initialiser_operation(OPERATION_ELLIPSE_DEGRADEE,2,0,
                        Ellipse_degradee_12_0,1);
  Initialiser_operation(OPERATION_ELLIPSE_DEGRADEE,1,6,
                        Ellipse_degradee_12_6,0);
  Initialiser_operation(OPERATION_ELLIPSE_DEGRADEE,2,6,
                        Ellipse_degradee_12_6,0);
  Initialiser_operation(OPERATION_ELLIPSE_DEGRADEE,0,6,
                        Ellipse_degradee_0_6,1);
  Initialiser_operation(OPERATION_ELLIPSE_DEGRADEE,1,8,
                        Ellipse_degradee_12_8,1);
  Initialiser_operation(OPERATION_ELLIPSE_DEGRADEE,2,8,
                        Ellipse_degradee_12_8,1);

  Initialiser_operation(OPERATION_LIGNES_CENTREES,1,0,
                        Lignes_centrees_12_0,1);
  Initialiser_operation(OPERATION_LIGNES_CENTREES,2,0,
                        Lignes_centrees_12_0,1);
  Initialiser_operation(OPERATION_LIGNES_CENTREES,1,3,
                        Lignes_centrees_12_3,0);
  Initialiser_operation(OPERATION_LIGNES_CENTREES,2,3,
                        Lignes_centrees_12_3,0);
  Initialiser_operation(OPERATION_LIGNES_CENTREES,0,3,
                        Lignes_centrees_0_3,1);
  Initialiser_operation(OPERATION_LIGNES_CENTREES,1,7,
                        Lignes_centrees_12_7,0);
  Initialiser_operation(OPERATION_LIGNES_CENTREES,2,7,
                        Lignes_centrees_12_7,0);
  Initialiser_operation(OPERATION_LIGNES_CENTREES,0,7,
                        Lignes_centrees_0_7,0);
}



//-- D�finition des modes vid�o: --------------------------------------------

  // D�finition d'un mode:

void Definir_mode_video(int    Numero,
                        short  Largeur, short Hauteur,
                        byte   Mode,
                        word   Facteur_X, word Facteur_Y,
                        char * Ratio,
                        unsigned char Refresh,
                        word   Mode_VESA_de_base,
                        void * Pointeur)
{
  Mode_video[Numero].Largeur          =Largeur;
  Mode_video[Numero].Hauteur          =Hauteur;
  Mode_video[Numero].Mode             =Mode;
  Mode_video[Numero].Facteur_X        =Facteur_X;
  Mode_video[Numero].Facteur_Y        =Facteur_Y;
  strcpy(Mode_video[Numero].Ratio,Ratio);
  Mode_video[Numero].Refresh          =Refresh;
  Mode_video[Numero].Mode_VESA_de_base=Mode_VESA_de_base;
  Mode_video[Numero].Pointeur         =Pointeur;
}


  // Initiliseur de toutes les op�rations:

void Definition_des_modes_video(void)
{                   // Numero       LargHaut Mode      FXFY Ratio Ref Vesa  Pointeur
  Definir_mode_video( MODE_320_200, 320,200,MODE_SDL  ,1,1,"0.85", 71,0    ,NULL    );
  Definir_mode_video( MODE_320_224, 320,224,MODE_SDL  ,1,1,"1.20", 51,0    ,X320Y224);
  Definir_mode_video( MODE_320_240, 320,240,MODE_SDL  ,1,1,"1.00", 60,0    ,X320Y240);
  Definir_mode_video( MODE_320_256, 320,256,MODE_SDL  ,1,1,"1.10", 58,0    ,X320Y256);
  Definir_mode_video( MODE_320_270, 320,270,MODE_SDL  ,1,1,"1.15", 64,0    ,X320Y270);
  Definir_mode_video( MODE_320_282, 320,282,MODE_SDL  ,1,1,"1.20", 52,0    ,X320Y282);
  Definir_mode_video( MODE_320_300, 320,300,MODE_SDL  ,1,1,"1.20",-49,0    ,X320Y300);
  Definir_mode_video( MODE_320_360, 320,360,MODE_SDL  ,1,1,"1.65", 71,0    ,X320Y360);
  Definir_mode_video( MODE_320_400, 320,400,MODE_SDL  ,1,2,"1.70", 71,0    ,X320Y400);
  Definir_mode_video( MODE_320_448, 320,448,MODE_SDL  ,1,2,"2.20", 51,0    ,X320Y448);
  Definir_mode_video( MODE_320_480, 320,480,MODE_SDL  ,1,2,"2.00", 60,0    ,X320Y480);
  Definir_mode_video( MODE_320_512, 320,512,MODE_SDL  ,1,2,"2.20", 58,0    ,X320Y512);
  Definir_mode_video( MODE_320_540, 320,540,MODE_SDL  ,1,2,"2.25", 64,0    ,X320Y540);
  Definir_mode_video( MODE_320_564, 320,564,MODE_SDL  ,1,2,"2.40", 59,0    ,X320Y564);
  Definir_mode_video( MODE_320_600, 320,600,MODE_SDL  ,1,2,"2.80", 51,0    ,X320Y600);

  Definir_mode_video( MODE_360_200, 360,200,MODE_SDL  ,1,1,"0.80", 72,0    ,X360Y200);
  Definir_mode_video( MODE_360_224, 360,224,MODE_SDL  ,1,1,"1.10", 51,0    ,X360Y224);
  Definir_mode_video( MODE_360_240, 360,240,MODE_SDL  ,1,1,"0.90", 61,0    ,X360Y240);
  Definir_mode_video( MODE_360_256, 360,256,MODE_SDL  ,1,1,"1.00", 57,0    ,X360Y256);
  Definir_mode_video( MODE_360_270, 360,270,MODE_SDL  ,1,1,"0.95", 57,0    ,X360Y270);
  Definir_mode_video( MODE_360_282, 360,282,MODE_SDL  ,1,1,"1.10", 52,0    ,X360Y282);
  Definir_mode_video( MODE_360_300, 360,300,MODE_SDL  ,1,1,"1.10",-49,0    ,X360Y300);
  Definir_mode_video( MODE_360_360, 360,360,MODE_SDL  ,1,1,"1.50", 72,0    ,X360Y360);
  Definir_mode_video( MODE_360_400, 360,400,MODE_SDL  ,1,2,"1.60", 72,0    ,X360Y400);
  Definir_mode_video( MODE_360_448, 360,448,MODE_SDL  ,1,2,"2.20", 51,0    ,X360Y448);
  Definir_mode_video( MODE_360_480, 360,480,MODE_SDL  ,1,2,"1.80", 61,0    ,X360Y480);
  Definir_mode_video( MODE_360_512, 360,512,MODE_SDL  ,1,2,"2.00", 57,0    ,X360Y512);
  Definir_mode_video( MODE_360_540, 360,540,MODE_SDL  ,1,2,"1.85", 57,0    ,X360Y540);
  Definir_mode_video( MODE_360_564, 360,564,MODE_SDL  ,1,2,"2.20", 60,0    ,X360Y564);
  Definir_mode_video( MODE_360_600, 360,600,MODE_SDL  ,1,2,"2.40", 45,0    ,X360Y600);

  Definir_mode_video( MODE_400_200, 400,200,MODE_SDL  ,1,1,"0.75", 68,0    ,X400Y200);
  Definir_mode_video( MODE_400_224, 400,224,MODE_SDL  ,1,1,"1.00", 49,0    ,X400Y224);
  Definir_mode_video( MODE_400_240, 400,240,MODE_SDL  ,1,1,"0.85", 58,0    ,X400Y240);
  Definir_mode_video( MODE_400_256, 400,256,MODE_SDL  ,1,1,"0.90", 55,0    ,X400Y256);
  Definir_mode_video( MODE_400_270, 400,270,MODE_SDL  ,1,1,"0.90", 54,0    ,X400Y270);
  Definir_mode_video( MODE_400_282, 400,282,MODE_SDL  ,1,1,"1.00", 50,0    ,X400Y282);
  Definir_mode_video( MODE_400_300, 400,300,MODE_SDL  ,1,1,"1.00",-46,0    ,X400Y300);
  Definir_mode_video( MODE_400_360, 400,360,MODE_SDL  ,1,1,"1.40", 68,0    ,X400Y360);
  Definir_mode_video( MODE_400_400, 400,400,MODE_SDL  ,1,2,"1.50", 67,0    ,X400Y400);
  Definir_mode_video( MODE_400_448, 400,448,MODE_SDL  ,1,2,"1.95", 49,0    ,X400Y448);
  Definir_mode_video( MODE_400_480, 400,480,MODE_SDL  ,1,2,"1.70", 58,0    ,X400Y480);
  Definir_mode_video( MODE_400_512, 400,512,MODE_SDL  ,1,2,"1.80", 55,0    ,X400Y512);
  Definir_mode_video( MODE_400_540, 400,540,MODE_SDL  ,1,2,"1.90", 54,0    ,X400Y540);
  Definir_mode_video( MODE_400_564, 400,564,MODE_SDL  ,1,2,"2.00", 57,0    ,X400Y564);
  Definir_mode_video( MODE_400_600, 400,600,MODE_SDL  ,1,2,"2.20", 43,0    ,X400Y600);

  Definir_mode_video( MODE_640_224, 640,224,MODE_SDL  ,2,1,"0.60", -1,0x101,Y224);
  Definir_mode_video( MODE_640_240, 640,240,MODE_SDL  ,2,1,"0.50", -1,0x101,Y240);
  Definir_mode_video( MODE_640_256, 640,256,MODE_SDL  ,2,1,"0.55", -1,0x101,Y256);
  Definir_mode_video( MODE_640_270, 640,270,MODE_SDL  ,2,1,"0.60", -1,0x101,Y270);
//  Definir_mode_video( MODE_640_282, 640,282,MODE_SDL  ,2,1,"?.??", -1,0x101,Y282);
  Definir_mode_video( MODE_640_300, 640,300,MODE_SDL  ,2,1,"0.60", -1,0x101,Y300);
  Definir_mode_video( MODE_640_350, 640,350,MODE_SDL  ,2,1,"0.75", -1,0x100,Y350);
//  Definir_mode_video( MODE_640_360, 640,360,MODE_SDL  ,2,1,"?.??", -1,0x101,Y360);
  Definir_mode_video( MODE_640_400, 640,400,MODE_SDL  ,2,2,"0.85", -1,0x100,NULL);
  Definir_mode_video( MODE_640_448, 640,448,MODE_SDL  ,2,2,"1.20", -1,0x101,Y448);
  Definir_mode_video( MODE_640_480, 640,480,MODE_SDL  ,2,2,"1.00", -1,0x101,NULL);
  Definir_mode_video( MODE_640_512, 640,512,MODE_SDL  ,2,2,"1.10", -1,0x101,Y512);
  Definir_mode_video( MODE_640_540, 640,540,MODE_SDL  ,2,2,"1.15", -1,0x101,Y540);
  Definir_mode_video( MODE_640_564, 640,564,MODE_SDL  ,2,2,"1.25", -1,0x101,Y564);
  Definir_mode_video( MODE_640_600, 640,600,MODE_SDL  ,2,2,"1.45", -1,0x101,Y600);
  Definir_mode_video( MODE_800_600, 800,600,MODE_SDL  ,2,2,"1.00", -1,0x103,NULL);
  Definir_mode_video(MODE_1024_768,1024,768,MODE_SDL  ,3,3,"1.00", -1,0x105,NULL);
}

//---------------------------------------------------------------------------




word Ordonnancement[NB_TOUCHES]=
{
  SPECIAL_SCROLL_UP,                // Scroll up
  SPECIAL_SCROLL_DOWN,              // Scroll down
  SPECIAL_SCROLL_LEFT,              // Scroll left
  SPECIAL_SCROLL_RIGHT,             // Scroll right
  SPECIAL_SCROLL_UP_FAST,           // Scroll up faster
  SPECIAL_SCROLL_DOWN_FAST,         // Scroll down faster
  SPECIAL_SCROLL_LEFT_FAST,         // Scroll left faster
  SPECIAL_SCROLL_RIGHT_FAST,        // Scroll right faster
  SPECIAL_SCROLL_UP_SLOW,           // Scroll up slower
  SPECIAL_SCROLL_DOWN_SLOW,         // Scroll down slower
  SPECIAL_SCROLL_LEFT_SLOW,         // Scroll left slower
  SPECIAL_SCROLL_RIGHT_SLOW,        // Scroll right slower
  SPECIAL_MOUSE_UP,                 // Emulate mouse up
  SPECIAL_MOUSE_DOWN,               // Emulate mouse down
  SPECIAL_MOUSE_LEFT,               // Emulate mouse left
  SPECIAL_MOUSE_RIGHT,              // Emulate mouse right
  SPECIAL_CLICK_LEFT,               // Emulate mouse click left
  SPECIAL_CLICK_RIGHT,              // Emulate mouse click right
  0x100+BOUTON_CACHER,              // Show / Hide menu
  SPECIAL_SHOW_HIDE_CURSOR,         // Show / Hide cursor
  SPECIAL_PINCEAU_POINT,            // Paintbrush = "."
  0x100+BOUTON_PINCEAUX,            // Paintbrush choice
  0x200+BOUTON_PINCEAUX,            // Monochrome brush
  0x100+BOUTON_DESSIN,              // Freehand drawing
  0x200+BOUTON_DESSIN,              // Switch freehand drawing mode
  SPECIAL_DESSIN_CONTINU,           // Continuous freehand drawing
  0x100+BOUTON_LIGNES,              // Line
  0x200+BOUTON_LIGNES,              // Knotted lines
  0x100+BOUTON_SPRAY,               // Spray
  0x200+BOUTON_SPRAY,               // Spray menu
  0x100+BOUTON_FLOODFILL,           // Floodfill
  0x200+BOUTON_FLOODFILL,           // Replace color
  0x100+BOUTON_COURBES,             // B�zier's curves
  0x200+BOUTON_COURBES,             // B�zier's curve with 3 or 4 points
  0x100+BOUTON_RECTANGLES,          // Empty rectangle
  0x100+BOUTON_FILLRECT,            // Filled rectangle
  0x100+BOUTON_CERCLES,             // Empty circle
  0x200+BOUTON_CERCLES,             // Empty ellipse
  0x100+BOUTON_FILLCERC,            // Filled circle
  0x200+BOUTON_FILLCERC,            // Filled ellipse
  0x100+BOUTON_POLYGONES,           // Empty polygon
  0x200+BOUTON_POLYGONES,           // Empty polyform
  0x100+BOUTON_POLYFILL,            // Polyfill
  0x200+BOUTON_POLYFILL,            // Filled polyform
  0x100+BOUTON_GRADRECT,            // Gradient rectangle
  0x100+BOUTON_GRADMENU,            // Gradation menu
  0x100+BOUTON_SPHERES,             // Spheres
  0x200+BOUTON_SPHERES,             // Gradient ellipses
  0x100+BOUTON_AJUSTER,             // Adjust picture
  0x200+BOUTON_AJUSTER,             // Flip picture menu
  0x100+BOUTON_EFFETS,              // Menu des effets
  SPECIAL_SHADE_MODE,               // Shade mode
  SPECIAL_SHADE_MENU,               // Shade menu
  SPECIAL_QUICK_SHADE_MODE,         // Quick-shade mode
  SPECIAL_QUICK_SHADE_MENU,         // Quick-shade menu
  SPECIAL_STENCIL_MODE,             // Stencil mode
  SPECIAL_STENCIL_MENU,             // Stencil menu
  SPECIAL_MASK_MODE,                // Mask mode
  SPECIAL_MASK_MENU,                // Mask menu
  SPECIAL_GRID_MODE,                // Grid mode
  SPECIAL_GRID_MENU,                // Grid menu
  SPECIAL_SIEVE_MODE,               // Sieve mode
  SPECIAL_SIEVE_MENU,               // Sieve menu
  SPECIAL_INVERT_SIEVE,             // Inverser la trame du mode Sieve
  SPECIAL_COLORIZE_MODE,            // Colorize mode
  SPECIAL_COLORIZE_MENU,            // Colorize menu
  SPECIAL_SMOOTH_MODE,              // Smooth mode
  SPECIAL_SMOOTH_MENU,              // Smooth menu
  SPECIAL_SMEAR_MODE,               // Smear mode
  SPECIAL_TILING_MODE,              // Tiling mode
  SPECIAL_TILING_MENU,              // Tiling menu
  0x100+BOUTON_BROSSE,              // Pick brush
  0x100+BOUTON_POLYBROSSE,          // Pick polyform brush
  0x200+BOUTON_BROSSE,              // Restore brush
  SPECIAL_FLIP_X,                   // Flip X
  SPECIAL_FLIP_Y,                   // Flip Y
  SPECIAL_ROTATE_90,                // 90� brush rotation
  SPECIAL_ROTATE_180,               // 180� brush rotation
  SPECIAL_STRETCH,                  // Stretch brush
  SPECIAL_DISTORT,                  // Distort brush
  SPECIAL_OUTLINE,                  // Outline brush
  SPECIAL_NIBBLE,                   // Nibble brush
  SPECIAL_GET_BRUSH_COLORS,         // Get colors from brush
  SPECIAL_RECOLORIZE_BRUSH,         // Recolorize brush
  SPECIAL_ROTATE_ANY_ANGLE,         // Rotate brush by any angle
  0x100+BOUTON_PIPETTE,             // Pipette
  0x200+BOUTON_PIPETTE,             // Swap fore/back color
  0x100+BOUTON_LOUPE,               // Magnifier mode
  0x200+BOUTON_LOUPE,               // Zoom factor menu
  SPECIAL_ZOOM_IN,                  // Zoom in
  SPECIAL_ZOOM_OUT,                 // Zoom out
  0x100+BOUTON_EFFETS_BROSSE,       // Brush effects menu
  0x100+BOUTON_TEXTE,               // Text
  0x100+BOUTON_RESOL,               // Resolution menu
  0x200+BOUTON_RESOL,               // Safety resolution
  0x100+BOUTON_AIDE,                // Help & credits
  0x200+BOUTON_AIDE,                // Statistics
  0x100+BOUTON_PAGE,                // Go to spare page
  0x200+BOUTON_PAGE,                // Copy to spare page
  0x100+BOUTON_SAUVER,              // Save as
  0x200+BOUTON_SAUVER,              // Save
  0x100+BOUTON_CHARGER,             // Load
  0x200+BOUTON_CHARGER,             // Re-load
  SPECIAL_SAVE_BRUSH,               // Save brush
  SPECIAL_LOAD_BRUSH,               // Load brush
  0x100+BOUTON_PARAMETRES,          // Settings
  0x100+BOUTON_UNDO,                // Undo
  0x200+BOUTON_UNDO,                // Redo
  0x100+BOUTON_KILL,                // Kill
  0x100+BOUTON_CLEAR,               // Clear
  0x200+BOUTON_CLEAR,               // Clear with backcolor
  0x100+BOUTON_QUIT,                // Quit
  0x100+BOUTON_PALETTE,             // Palette menu
  0x200+BOUTON_PALETTE,             // Palette menu secondaire
  SPECIAL_EXCLUDE_COLORS_MENU,      // Exclude colors menu
  0x100+BOUTON_PAL_LEFT,            // Scroll palette left
  0x100+BOUTON_PAL_RIGHT,           // Scroll palette right
  0x200+BOUTON_PAL_LEFT,            // Scroll palette left faster
  0x200+BOUTON_PAL_RIGHT,           // Scroll palette right faster
  SPECIAL_CENTER_ATTACHMENT,        // Center brush attachement
  SPECIAL_TOP_LEFT_ATTACHMENT,      // Top-left brush attachement
  SPECIAL_TOP_RIGHT_ATTACHMENT,     // Top-right brush attachement
  SPECIAL_BOTTOM_LEFT_ATTACHMENT,   // Bottom-left brush attachement
  SPECIAL_BOTTOM_RIGHT_ATTACHMENT,  // Bottom right brush attachement
  SPECIAL_NEXT_FORECOLOR,           // Next foreground color
  SPECIAL_PREVIOUS_FORECOLOR,       // Previous foreground color
  SPECIAL_NEXT_BACKCOLOR,           // Next background color
  SPECIAL_PREVIOUS_BACKCOLOR,       // Previous background color
  SPECIAL_NEXT_USER_FORECOLOR,      // Next user-defined foreground color
  SPECIAL_PREVIOUS_USER_FORECOLOR,  // Previous user-defined foreground color
  SPECIAL_NEXT_USER_BACKCOLOR,      // Next user-defined background color
  SPECIAL_PREVIOUS_USER_BACKCOLOR,  // Previous user-defined background color
  SPECIAL_RETRECIR_PINCEAU,         // R�tr�cir le pinceau
  SPECIAL_GROSSIR_PINCEAU           // Grossir le pinceau
};

byte Numero_option[NB_TOUCHES]=
{
    0, // Scroll up
    1, // Scroll down
    2, // Scroll left
    3, // Scroll right
    4, // Scroll up faster
    5, // Scroll down faster
    6, // Scroll left faster
    7, // Scroll right faster
    8, // Scroll up slower
    9, // Scroll down slower
   10, // Scroll left slower
   11, // Scroll right slower
   12, // Emulate mouse up
   13, // Emulate mouse down
   14, // Emulate mouse left
   15, // Emulate mouse right
   16, // Emulate mouse click left
   17, // Emulate mouse click right
   18, // Show / Hide menu
   19, // Show / Hide cursor
   20, // Paintbrush = "."
   21, // Paintbrush choice
   22, // Monochrome brush
   23, // Freehand drawing
   24, // Switch freehand drawing mode
   25, // Continuous freehand drawing
   26, // Line
   27, // Knotted lines
   28, // Spray
   29, // Spray menu
   30, // Floodfill
  124, // Replace color
   31, // B�zier's curves
   32, // B�zier's curve with 3 or 4 points
   33, // Empty rectangle
   34, // Filled rectangle
   35, // Empty circle
   36, // Empty ellipse
   37, // Filled circle
   38, // Filled ellipse
   39, // Empty polygon
   40, // Empty polyform
   41, // Polyfill
   42, // Filled polyform
   43, // Gradient rectangle
   44, // Gradation menu
   45, // Spheres
   46, // Gradient ellipses
   47, // Adjust picture
   48, // Flip picture menu
   49, // Menu des effets
   50, // Shade mode
   51, // Shade menu
  131, // Quick-shade mode
  132, // Quick-shade menu
   52, // Stencil mode
   53, // Stencil menu
   54, // Mask mode
   55, // Mask menu
   56, // Grid mode
   57, // Grid menu
   58, // Sieve mode
   59, // Sieve menu
   60, // Inverser la trame du mode Sieve
   61, // Colorize mode
   62, // Colorize menu
   63, // Smooth mode
  123, // Smooth menu
   64, // Smear mode
   65, // Tiling mode
   66, // Tiling menu
   67, // Pick brush
   68, // Pick polyform brush
   69, // Restore brush
   70, // Flip X
   71, // Flip Y
   72, // 90� brush rotation
   73, // 180� brush rotation
   74, // Stretch brush
   75, // Distort brush
   76, // Outline brush
   77, // Nibble brush
   78, // Get colors from brush
   79, // Recolorize brush
   80, // Rotate brush by any angle
   81, // Pipette
   82, // Swap fore/back color
   83, // Magnifier mode
   84, // Zoom factor menu
   85, // Zoom in
   86, // Zoom out
   87, // Brush effects menu
   88, // Text
   89, // Resolution menu
   90, // Safety resolution
   91, // Help & credits
   92, // Statistics
   93, // Go to spare page
   94, // Copy to spare page
   95, // Save as
   96, // Save
   97, // Load
   98, // Re-load
   99, // Save brush
  100, // Load brush
  101, // Settings
  102, // Undo
  103, // Redo
  133, // Kill
  104, // Clear
  105, // Clear with backcolor
  106, // Quit
  107, // Palette menu
  125, // Palette menu secondaire
  130, // Exclude colors menu
  108, // Scroll palette left
  109, // Scroll palette right
  110, // Scroll palette left faster
  111, // Scroll palette right faster
  112, // Center brush attachement
  113, // Top-left brush attachement
  114, // Top-right brush attachement
  115, // Bottom-left brush attachement
  116, // Bottom right brush attachement
  117, // Next foreground color
  118, // Previous foreground color
  119, // Next background color
  120, // Previous background color
  126, // Next user-defined foreground color
  127, // Previous user-defined foreground color
  128, // Next user-defined background color
  129, // Previous user-defined background color
  121, // R�tr�cir le pinceau
  122  // Grossir le pinceau
};


int Charger_CFG(int Tout_charger)
{
  int  Handle;
  char Nom_du_fichier[256];
  long Taille_fichier;
  int  Indice,Indice2;
  struct Config_Header       CFG_Header;
  struct Config_Chunk        Chunk;
  struct Config_Infos_touche CFG_Infos_touche;
  struct Config_Mode_video   CFG_Mode_video;
  struct stat Informations_Fichier;

  strcpy(Nom_du_fichier,Repertoire_du_programme);
  strcat(Nom_du_fichier,"gfx2.cfg");

  stat(Nom_du_fichier,&Informations_Fichier);
  Taille_fichier=Informations_Fichier.st_size;
  
  if ((Handle=open(Nom_du_fichier,O_RDONLY))==-1)
    return ERREUR_CFG_ABSENT;
  
  if ( (Taille_fichier<sizeof(CFG_Header))
    || (read(Handle,&CFG_Header,sizeof(CFG_Header))!=sizeof(CFG_Header))
    || memcmp(CFG_Header.Signature,"CFG",3) )
      goto Erreur_lecture_config;

  if ( (CFG_Header.Version1!=VERSION1)
    || (CFG_Header.Version2!=VERSION2)
    || (CFG_Header.Beta1!=BETA1)
    || (CFG_Header.Beta2!=BETA2) )
    goto Erreur_config_ancienne;
  
  if (Taille_fichier!=TAILLE_FICHIER_CONFIG)
    goto Erreur_lecture_config;
  
  // - Lecture des infos contenues dans le fichier de config -
  while (read(Handle,&(Chunk.Numero),sizeof(byte))==sizeof(byte))
  {
		read(Handle,&(Chunk.Taille),sizeof(word));
		#if __BYTE_ORDER == __BIG_ENDIAN
			Chunk.Taille=bswap_16(Chunk.Taille);
		#endif
    switch (Chunk.Numero)
    {
      case CHUNK_TOUCHES: // Touches
        if (Tout_charger)
        {
          if ((Chunk.Taille/sizeof(CFG_Infos_touche))!=NB_TOUCHES)
            goto Erreur_lecture_config;
          for (Indice=1; Indice<=NB_TOUCHES; Indice++)
          {
            if (read(Handle,&CFG_Infos_touche,sizeof(CFG_Infos_touche))!=sizeof(CFG_Infos_touche))
              goto Erreur_lecture_config;
            else
            {
							#if __BYTE_ORDER == __BIG_ENDIAN
								CFG_Infos_touche.Touche=bswap_16(CFG_Infos_touche.Touche);
								CFG_Infos_touche.Touche2=bswap_16(CFG_Infos_touche.Touche2);
								CFG_Infos_touche.Numero=bswap_16(CFG_Infos_touche.Numero);
							#endif
              for (Indice2=0;
                   ((Indice2<NB_TOUCHES) && (Numero_option[Indice2]!=CFG_Infos_touche.Numero));
                   Indice2++);
              if (Indice2<NB_TOUCHES)
              {
                switch(Ordonnancement[Indice2]>>8)
                {
                  case 0 : Config_Touche[Ordonnancement[Indice2]&0xFF]=CFG_Infos_touche.Touche; break;
                  case 1 : Bouton[Ordonnancement[Indice2]&0xFF].Raccourci_gauche=CFG_Infos_touche.Touche; break;
                  case 2 : Bouton[Ordonnancement[Indice2]&0xFF].Raccourci_droite=CFG_Infos_touche.Touche; break;
                }
              }
              else
                goto Erreur_lecture_config;
            }
          }
        }
        else
        {
          if (lseek(Handle,Chunk.Taille,SEEK_CUR)==-1)
            goto Erreur_lecture_config;
        }
        break;
      case CHUNK_MODES_VIDEO: // Modes vid�o
        if ((Chunk.Taille/5/*sizeof(CFG_Mode_video)*/)!=NB_MODES_VIDEO)
          goto Erreur_lecture_config;
        for (Indice=1; Indice<=NB_MODES_VIDEO; Indice++)
        {
					read(Handle,&(CFG_Mode_video.Etat),1);
					read(Handle,&(CFG_Mode_video.Largeur),2);
          if (read(Handle,&(CFG_Mode_video.Hauteur),2)!=2)
            goto Erreur_lecture_config;
          else
          {
            for (Indice2=0;
                 ( (Indice2<NB_MODES_VIDEO) &&
                   ( (Mode_video[Indice2].Largeur!=CFG_Mode_video.Largeur) ||
                     (Mode_video[Indice2].Hauteur!=CFG_Mode_video.Hauteur) ||
                     (Mode_video[Indice2].Mode!=(CFG_Mode_video.Etat>>6)) ) );
                 Indice2++);
            if (Indice2<NB_MODES_VIDEO)
              Mode_video[Indice2].Etat=(Mode_video[Indice2].Etat&0xFC) | (CFG_Mode_video.Etat&3);
          }
        }
        break;
      case CHUNK_SHADE: // Shade
        if (Tout_charger)
        {
          if (read(Handle,&Shade_Actuel,sizeof(Shade_Actuel))!=sizeof(Shade_Actuel))
            goto Erreur_lecture_config;
          else
          {
            if (read(Handle,Shade_Liste,sizeof(Shade_Liste))!=sizeof(Shade_Liste))
              goto Erreur_lecture_config;
            else
              Liste2tables(Shade_Liste[Shade_Actuel].Liste,
                           Shade_Liste[Shade_Actuel].Pas,
                           Shade_Liste[Shade_Actuel].Mode,
                           Shade_Table_gauche,Shade_Table_droite);
          }
        }
        else
        {
          if (lseek(Handle,Chunk.Taille,SEEK_CUR)==-1)
            goto Erreur_lecture_config;
        }
        break;
      case CHUNK_MASQUE: // Masque
        if (Tout_charger)
        {
          if (read(Handle,Mask,sizeof(Mask))!=sizeof(Mask))
            goto Erreur_lecture_config;
        }
        else
        {
          if (lseek(Handle,Chunk.Taille,SEEK_CUR)==-1)
            goto Erreur_lecture_config;
        }
        break;
      case CHUNK_STENCIL: // Stencil
        if (Tout_charger)
        {
          if (read(Handle,Stencil,sizeof(Stencil))!=sizeof(Stencil))
            goto Erreur_lecture_config;
        }
        else
        {
          if (lseek(Handle,Chunk.Taille,SEEK_CUR)==-1)
            goto Erreur_lecture_config;
        }
        break;
      case CHUNK_DEGRADES: // Infos sur les d�grad�s
        if (Tout_charger)
        {
          if (read(Handle,&Degrade_Courant,1)!=1)
            goto Erreur_lecture_config;
					for(Indice=0;Indice<16;Indice++)
					{
						read(Handle,&(Degrade_Tableau[Indice].Debut),1);
						read(Handle,&(Degrade_Tableau[Indice].Fin),1);
						read(Handle,&(Degrade_Tableau[Indice].Inverse),4);
						read(Handle,&(Degrade_Tableau[Indice].Melange),4);
          	if (read(Handle,&(Degrade_Tableau[Indice]).Technique,4)!=4)
            	goto Erreur_lecture_config;
					}
          Degrade_Charger_infos_du_tableau(Degrade_Courant);
        }
        else
        {
          if (lseek(Handle,Chunk.Taille,SEEK_CUR)==-1)
            goto Erreur_lecture_config;
        }
        break;
      case CHUNK_SMOOTH: // Matrice du smooth
        if (Tout_charger)
        {
          if (read(Handle,Smooth_Matrice,sizeof(Smooth_Matrice))!=sizeof(Smooth_Matrice))
            goto Erreur_lecture_config;
        }
        else
        {
          if (lseek(Handle,Chunk.Taille,SEEK_CUR)==-1)
            goto Erreur_lecture_config;
        }
        break;
      case CHUNK_EXCLUDE_COLORS: // Exclude_color
        if (Tout_charger)
        {
          if (read(Handle,Exclude_color,sizeof(Exclude_color))!=sizeof(Exclude_color))
            goto Erreur_lecture_config;
        }
        else
        {
          if (lseek(Handle,Chunk.Taille,SEEK_CUR)==-1)
            goto Erreur_lecture_config;
        }
        break;
      case CHUNK_QUICK_SHADE: // Quick-shade
        if (Tout_charger)
        {
          if (read(Handle,&Quick_shade_Step,sizeof(Quick_shade_Step))!=sizeof(Quick_shade_Step))
            goto Erreur_lecture_config;
          if (read(Handle,&Quick_shade_Loop,sizeof(Quick_shade_Loop))!=sizeof(Quick_shade_Loop))
            goto Erreur_lecture_config;
        }
        else
        {
          if (lseek(Handle,Chunk.Taille,SEEK_CUR)==-1)
            goto Erreur_lecture_config;
        }
        break;
      default: // Chunk inconnu
        goto Erreur_lecture_config;
    }
  }

  if (close(Handle))
    return ERREUR_CFG_CORROMPU;

  return 0;

Erreur_lecture_config:
  close(Handle);
  return ERREUR_CFG_CORROMPU;
Erreur_config_ancienne:
  close(Handle);
  return ERREUR_CFG_ANCIEN;
}


int Sauver_CFG(void)
{
  int  Handle;
  int  Indice;
  //byte Octet;
  char Nom_du_fichier[256];
  struct Config_Header       CFG_Header;
  struct Config_Chunk        Chunk;
  struct Config_Infos_touche CFG_Infos_touche;
  struct Config_Mode_video   CFG_Mode_video;


  strcpy(Nom_du_fichier,Repertoire_du_programme);
  strcat(Nom_du_fichier,"GFX2.CFG");

  if ((Handle=open(Nom_du_fichier,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP))<0)
    return ERREUR_SAUVEGARDE_CFG;

  // Ecriture du header
  memcpy(CFG_Header.Signature,"CFG",3);
  CFG_Header.Version1=VERSION1;
  CFG_Header.Version2=VERSION2;
  CFG_Header.Beta1   =BETA1;
  CFG_Header.Beta2   =BETA2;
  if (write(Handle,&CFG_Header,sizeof(CFG_Header))!=sizeof(CFG_Header))
    goto Erreur_sauvegarde_config;

  // Enregistrement des touches
  Chunk.Numero=CHUNK_TOUCHES;
  Chunk.Taille=NB_TOUCHES*sizeof(CFG_Infos_touche);
  if (write(Handle,&Chunk,sizeof(Chunk))!=sizeof(Chunk))
    goto Erreur_sauvegarde_config;
  for (Indice=0; Indice<NB_TOUCHES; Indice++)
  {
    CFG_Infos_touche.Numero =Numero_option[Indice];
    switch(Ordonnancement[Indice]>>8)
    {
      case 0 : CFG_Infos_touche.Touche=Config_Touche[Ordonnancement[Indice]&0xFF]; break;
      case 1 : CFG_Infos_touche.Touche=Bouton[Ordonnancement[Indice]&0xFF].Raccourci_gauche; break;
      case 2 : CFG_Infos_touche.Touche=Bouton[Ordonnancement[Indice]&0xFF].Raccourci_droite; break;
    }
    CFG_Infos_touche.Touche2=0x00FF;
    if (write(Handle,&CFG_Infos_touche,sizeof(CFG_Infos_touche))!=sizeof(CFG_Infos_touche))
      goto Erreur_sauvegarde_config;
  }

  // Sauvegarde de l'�tat de chaque mode vid�o
  Chunk.Numero=CHUNK_MODES_VIDEO;
  Chunk.Taille=NB_MODES_VIDEO*sizeof(CFG_Mode_video);
  if (write(Handle,&Chunk,sizeof(Chunk))!=sizeof(Chunk))
    goto Erreur_sauvegarde_config;
  for (Indice=0; Indice<NB_MODES_VIDEO; Indice++)
  {
    CFG_Mode_video.Etat   =(Mode_video[Indice].Mode<<6) | (Mode_video[Indice].Etat&3);
    CFG_Mode_video.Largeur=Mode_video[Indice].Largeur;
    CFG_Mode_video.Hauteur=Mode_video[Indice].Hauteur;
    if (write(Handle,&CFG_Mode_video,sizeof(CFG_Mode_video))!=sizeof(CFG_Mode_video))
      goto Erreur_sauvegarde_config;
  }

  // Ecriture des donn�es du Shade (pr�c�d�es du shade en cours)
  Chunk.Numero=CHUNK_SHADE;
  Chunk.Taille=sizeof(Shade_Liste)+sizeof(Shade_Actuel);
  if (write(Handle,&Chunk,sizeof(Chunk))!=sizeof(Chunk))
    goto Erreur_sauvegarde_config;
  if (write(Handle,&Shade_Actuel,sizeof(Shade_Actuel))!=sizeof(Shade_Actuel))
    goto Erreur_sauvegarde_config;
  if (write(Handle,Shade_Liste,sizeof(Shade_Liste))!=sizeof(Shade_Liste))
    goto Erreur_sauvegarde_config;

  // Sauvegarde des informations du Masque
  Chunk.Numero=CHUNK_MASQUE;
  Chunk.Taille=sizeof(Mask);
  if (write(Handle,&Chunk,sizeof(Chunk))!=sizeof(Chunk))
    goto Erreur_sauvegarde_config;
  if (write(Handle,Mask,sizeof(Mask))!=sizeof(Mask))
    goto Erreur_sauvegarde_config;

  // Sauvegarde des informations du Stencil
  Chunk.Numero=CHUNK_STENCIL;
  Chunk.Taille=sizeof(Stencil);
  if (write(Handle,&Chunk,sizeof(Chunk))!=sizeof(Chunk))
    goto Erreur_sauvegarde_config;
  if (write(Handle,Stencil,sizeof(Stencil))!=sizeof(Stencil))
    goto Erreur_sauvegarde_config;

  // Sauvegarde des informations des d�grad�s
  Chunk.Numero=CHUNK_DEGRADES;
  Chunk.Taille=sizeof(Degrade_Tableau)+1;
  if (write(Handle,&Chunk,sizeof(Chunk))!=sizeof(Chunk))
    goto Erreur_sauvegarde_config;
  if (write(Handle,&Degrade_Courant,1)!=1)
    goto Erreur_sauvegarde_config;
  if (write(Handle,Degrade_Tableau,sizeof(Degrade_Tableau))!=sizeof(Degrade_Tableau))
    goto Erreur_sauvegarde_config;

  // Sauvegarde de la matrice du Smooth
  Chunk.Numero=CHUNK_SMOOTH;
  Chunk.Taille=sizeof(Smooth_Matrice);
  if (write(Handle,&Chunk,sizeof(Chunk))!=sizeof(Chunk))
    goto Erreur_sauvegarde_config;
  if (write(Handle,Smooth_Matrice,sizeof(Smooth_Matrice))!=sizeof(Smooth_Matrice))
    goto Erreur_sauvegarde_config;

  // Sauvegarde des couleurs � exclure
  Chunk.Numero=CHUNK_EXCLUDE_COLORS;
  Chunk.Taille=sizeof(Exclude_color);
  if (write(Handle,&Chunk,sizeof(Chunk))!=sizeof(Chunk))
    goto Erreur_sauvegarde_config;
  if (write(Handle,Exclude_color,sizeof(Exclude_color))!=sizeof(Exclude_color))
    goto Erreur_sauvegarde_config;

  // Sauvegarde des informations du Quick-shade
  Chunk.Numero=CHUNK_QUICK_SHADE;
  Chunk.Taille=sizeof(Quick_shade_Step)+sizeof(Quick_shade_Loop);
  if (write(Handle,&Chunk,sizeof(Chunk))!=sizeof(Chunk))
    goto Erreur_sauvegarde_config;
  if (write(Handle,&Quick_shade_Step,sizeof(Quick_shade_Step))!=sizeof(Quick_shade_Step))
    goto Erreur_sauvegarde_config;
  if (write(Handle,&Quick_shade_Loop,sizeof(Quick_shade_Loop))!=sizeof(Quick_shade_Loop))
    goto Erreur_sauvegarde_config;

  if (close(Handle))
    return ERREUR_SAUVEGARDE_CFG;

  return 0;

Erreur_sauvegarde_config:
  close(Handle);
  return ERREUR_SAUVEGARDE_CFG;
}


void Initialiser_les_tables_de_multiplication(void)
{
  word Indice_de_facteur;
  word Facteur_de_zoom;
  word Indice_de_multiplication;

  for (Indice_de_facteur=0;Indice_de_facteur<NB_FACTEURS_DE_ZOOM;Indice_de_facteur++)
  {
    Facteur_de_zoom=FACTEUR_ZOOM[Indice_de_facteur];

    for (Indice_de_multiplication=0;Indice_de_multiplication<512;Indice_de_multiplication++)
    {
      TABLE_ZOOM[Indice_de_facteur][Indice_de_multiplication]=Facteur_de_zoom*Indice_de_multiplication;
    }
  }
}


void Initialiser_la_table_des_carres(void)
{
  long Indice;

  for (Indice=0;Indice<1025;Indice++)
    Table_des_carres[Indice]=(Indice*Indice);
}


void Initialiser_la_table_precalculee_des_distances_de_couleur(void)
{
  int Indice;

  // On commence par allouer la m�moire utilis�e par la table:
  // 128 valeurs pour chaque teinte, 3 teintes (Rouge, vert et bleu)
  MC_Table_differences=(int *)malloc(sizeof(int)*(3*128));

  // Pour chacune des 128 positions correspondant � une valeur de diff�rence:
  for (Indice=0;Indice<128;Indice++)
  {
    if (Indice<64)
    {
      // Valeur pour le rouge:
      MC_Table_differences[Indice+  0]=(Indice*30)*(Indice*30);
      // Valeur pour le vert :
      MC_Table_differences[Indice+128]=(Indice*59)*(Indice*59);
      // Valeur pour le bleu :
      MC_Table_differences[Indice+256]=(Indice*11)*(Indice*11);
    }
    else
    {
      // Valeur pour le rouge:
      MC_Table_differences[Indice+  0]=((128-Indice)*30)*((128-Indice)*30);
      // Valeur pour le vert :
      MC_Table_differences[Indice+128]=((128-Indice)*59)*((128-Indice)*59);
      // Valeur pour le bleu :
      MC_Table_differences[Indice+256]=((128-Indice)*11)*((128-Indice)*11);
    }
  }
}


#include "readini.c"
#include "saveini.c"
