//C
#include <stdbool.h>

//POSIX
#include <unistd.h>

//SDL
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_gfxPrimitives.h>

//mine
#include "SFont.h"

#include "scancodes.h"

/*** Constants ***/
#define NB_MAX_OPTIONS 134
#define HAUTEUR_DEBUT_SETUP 7
#define HAUTEUR_FIN_SETUP 44

/* Colors */
#define COULEUR_SETUP 1
#define COULEUR_SELECT 8

/*** Types definitions and structs ***/

typedef struct{
    uint16_t Numero;
    uint16_t Touche;
    uint16_t Touche2;
    char Libelle[36];
    char Explic1[77];
    char Explic2[77];
    bool Erreur;
    bool Suppr;
} Options;

typedef struct{
    char Signature[3];
    uint8_t Version1;
    uint8_t Version2;
    uint8_t Beta1;
    uint8_t Beta2;
} Type_header;

#define Header_size 7

typedef struct{
    uint8_t Numero;
    uint16_t Taille;
} Type_chunk;

#define Chunk_size 3

/*** Global variables ***/
SFont_Font* MyFont;
SDL_Surface* Ecran;
int Numero_definition_option;
Options Config[NB_MAX_OPTIONS];
bool Choix_enreg;
uint16_t NB_OPTIONS;

/*** Fonctions de gestion des �v�nements SDL ***/

/* Attend qu'une touche soit press�e. Retourne le keysym. */
SDL_keysym Lire_Touche(void)
{
    SDL_Event Event;

    do{
        SDL_WaitEvent(&Event);
        if(Event.type==SDL_KEYDOWN) break;
    }while(1);

    return Event.key.keysym;
}

/* Writes human-readable key name to buffer Temp. Temp must be at least 35 bytes long ! */
void Nom_touche(uint16_t Touche,char* Temp)
{
    char Temp2[28];
    uint8_t Num_table =1;

    Temp[0] = Temp2[0] = 0;

    if((Touche & 0x0100) > 0)
    {
        strcat(Temp,"<Shift> + ");
        Num_table = 2;
    }

    if((Touche & 0x0200) > 0)
    {
        strcat(Temp,"<Ctrl> + ");
        Num_table = 3;
    }

    if((Touche & 0x0400) > 0)
    {
        strcat(Temp,"<Alt> + ");
        Num_table = 4;
    }

    switch(Num_table)
    {
        case 1:
            strcpy(Temp2,Table_Normal[Touche & 0xFF]);
            printf("k: %x\n",Touche & 0xFF);
            if (strcmp(Temp2,"???") == 0)
                strcpy(Temp,"********** Invalid key! **********");
            else if (Temp2[0]==0)
                Temp[0]=0;
            else
            {
                strcat(Temp,"<");
                strcat(Temp,Temp2);
                strcat(Temp,">");
            }
            break;
        case 2:
            strcpy(Temp2,Table_Shift[Touche & 0xFF]); 

            break;
    }
}

/*** Drawing functions ***/

/* Draws a filled rectanble */
void Cadre(int x,int y,int w,int h,uint8_t color)
{
    SDL_Rect rct;
    rct.x=x;
    rct.y=y;
    rct.w=w;
    rct.h=h;

    SDL_FillRect(Ecran, &rct, color);
}

/* Draws the main screen and welcome message */
void Dessiner_ecran_principal()
{
    Cadre(3,3,630,40,2);
    SFont_Write(Ecran, MyFont, 8,6,"Setup program for Grafx2 (c) 1996-98 Sunset Design and 2008 PulkoMandy");
    SFont_Write(Ecran, MyFont, 8,18,"Use Up/Down arrows & Page-Up/Page-Down to scroll, Enter to modify, Delete to remove a hot-key, and Escape to validate or cancel.");
    SFont_Write(Ecran, MyFont, 8,30,"DO NOT USE Print-screen, Pause, and other special keys!");

    Cadre(3,46,630,400,COULEUR_SETUP);
    SFont_Write(Ecran,MyFont,8,48,"Option");
    SFont_Write(Ecran,MyFont,8*38,48,"Hot-Key");
    SFont_Write(Ecran,MyFont,8*75,48,"Err");

    SDL_UpdateRect(Ecran,0,0,640,480);

}

/* Displays informations about an option */
void Ecrire(uint8_t Ligne, uint16_t Num_option, uint8_t Couleur)
{
    char NomTouche[35];
    puts(Config[Num_option].Libelle);
    SFont_Write(Ecran,MyFont,8,Ligne*9,Config[Num_option].Libelle);
    Nom_touche(Config[Num_option].Touche,NomTouche);
    SFont_Write(Ecran,MyFont,40*8,Ligne*9,NomTouche);
    if(Config[Num_option].Erreur)
        SFont_Write(Ecran,MyFont,77*8,Ligne*9,"X");
    else
        Cadre(77*8,Ligne*9,8,8,Couleur);
}

/* Displays comment about an option */
void Ecrire_commentaire(uint16_t Num_option)
{
    SFont_Write(Ecran,MyFont,8,50*9,Config[Num_option].Explic1);
    SFont_Write(Ecran,MyFont,8,51*9,Config[Num_option].Explic2);
}

/* Display the options list */
void Tout_ecrire(uint16_t Decalage_curseur,uint16_t Position_curseur)
{
    uint8_t i = HAUTEUR_DEBUT_SETUP;

    Cadre(3,(HAUTEUR_DEBUT_SETUP+Position_curseur - 1)*9,630,8,COULEUR_SELECT);

    while(i<=HAUTEUR_FIN_SETUP && i <= NB_OPTIONS + HAUTEUR_DEBUT_SETUP)
    {
        Ecrire(i,Decalage_curseur+i-HAUTEUR_DEBUT_SETUP,COULEUR_SETUP);
        i++;
    }

    Cadre(36*8,46,1,400,255);
    Cadre(78*8,46,1,400,255);

    Ecrire_commentaire(Decalage_curseur+Position_curseur-1);
    SDL_UpdateRect(Ecran,0,0,640,480);
}

/*** Configuration handling functions ***/

/* Defines an option */
void Definir_option(uint16_t Numero, char* Libelle, char* Explic1, char* Explic2, bool Deletable, 
        uint16_t Default_key)
{
    Config[Numero_definition_option].Numero = Numero;
    Config[Numero_definition_option].Touche = Default_key;
    Config[Numero_definition_option].Touche2 = 0xFF;
    strncpy(Config[Numero_definition_option].Libelle,Libelle,36);
    strncpy(Config[Numero_definition_option].Explic1,Explic1,77);
    strncpy(Config[Numero_definition_option].Explic2,Explic2,77);
    Config[Numero_definition_option].Erreur = false;
    Config[Numero_definition_option].Suppr = Deletable;
    Numero_definition_option ++ ;	
}

/* Initialize configuration */
bool Initialiser_config()
{
    bool Erreur = false;
    FILE* Fichier_INI;

    Numero_definition_option = 0;

    Definir_option(0,"Scroll up",
            "Scrolls the picture up, both in magnify and normal mode.",
            "This hotkey cannot be removed.",
            false, 0x48); // HAUT
    Definir_option(1,"Scroll down",
            "Scrolls the picture down, both in magnify and normal mode.",
            "This hotkey cannot be removed.",
            false, 0x50); // BAS
    Definir_option(2,"Scroll left",
            "Scrolls the picture to the left, both in magnify and normal mode.",
            "This hotkey cannot be removed.",
            false, 0x4B); // GAUCHE
    Definir_option(3,"Scroll right",
            "Scrolls the picture to the right, both in magnify and normal mode.",
            "This hotkey cannot be removed.",
            false, 0x4D); // DROITE


    NB_OPTIONS = Numero_definition_option - 1;
}

uint8_t Fenetre_choix(uint8_t Largeur, uint8_t Hauteur, char* Titre, char* Choix, uint8_t Choix_debut,
        uint8_t Couleur,uint8_t Couleur_choix)
{
    puts("FENETRE CHOIX UNIMPLEMENTED !!!");
    return 0;
}

/* Checks if some key is used twice */
void Test_duplic()
{
    uint16_t i,j;
    bool Pas_encore_erreur;

    for(i=0;i<NB_OPTIONS;i++)
    {
        if(Config[i].Touche!=0xFF)
        {
            j=1;
            Pas_encore_erreur=true;
            while(j<NB_OPTIONS && Pas_encore_erreur)
            {
                if(i!=j && Config[i].Touche==Config[j].Touche)
                {
                    Pas_encore_erreur = false;
                    Config[i].Erreur = true;
                }
                j++;
            }
            if (Pas_encore_erreur) Config[i].Erreur = false;
        }
    }
}

/* Checks if everything is OK */
bool Validation()
{
    bool Y_a_des_erreurs = false;
    uint16_t i = 0;

    while(i<NB_OPTIONS && !Y_a_des_erreurs)
    {
        Y_a_des_erreurs = Config[i].Erreur;
        i++;
    }

    if (Y_a_des_erreurs)
        Choix_enreg = 4 - Fenetre_choix(30,12,
                "There are errors in the\nhot-keys configuration.\nCheck out the \"Err\" column\nin order to correct them.","OK\nQuit anyway",1,0x4C,0x3F);
    else
        Choix_enreg = Fenetre_choix(30,10,"Save configuration?","Yes\nNo\nCancel",1,0x2A,0x6F);
    if (Choix_enreg!=3) return true; else return false;
}

/* Let the user do things */
void Setup()
{
    bool Sortie_OK = false;
    SDL_keysym Touche;
    uint16_t Decalage_curseur=0;
    uint16_t Position_curseur=1;
    Test_duplic();
    Tout_ecrire(0,1);
    do{
        Touche = Lire_Touche();
        switch(Touche.sym)
        {
            case SDLK_UP:
            case SDLK_DOWN:
            case SDLK_PAGEUP:
            case SDLK_PAGEDOWN:
            case SDLK_RETURN:
            case SDLK_DELETE:
                break;
            case SDLK_ESCAPE:
                Sortie_OK=Validation();
                break; 
            default:
                break; // On ne fait rien pour les autres touches
        }
    } while(!Sortie_OK);
}

/*** File management functions ***/

/* Checks if we can write the config file */
bool Verifier_ecriture_possible()
{
    return access("./",W_OK) == 0;
}

/* Save the config file */
void Enregistrer_config()
{
    FILE* Fichier;
    uint16_t Indice;
    uint8_t Octet;

    if(Choix_enreg==true) // Save keys if wanted
    {
        Fichier = fopen("gfx2.cfg","wb");
        fseek(Fichier,Header_size+Chunk_size,SEEK_SET); // Positionnement sur la premi�re touche

        for(Indice = 0;Indice < NB_OPTIONS;Indice++)
        {
            fwrite(&Config[Indice].Numero,sizeof(uint16_t),1,Fichier);
            fwrite(&Config[Indice].Touche,sizeof(uint16_t),1,Fichier);
            fwrite(&Config[Indice].Touche2,sizeof(uint16_t),1,Fichier);
        }

        fclose(Fichier);
    }
}

/*** Main program ***/

int main(void)
{	

    if (Verifier_ecriture_possible())
    {
        /* On initialise SDL */
        SDL_Init(SDL_INIT_VIDEO);
        Ecran = SDL_SetVideoMode(640,480,8,0);
        SDL_WM_SetCaption ("Grafx2 configuration tool","../gfx2.gif");

        /* On initialise SFont */
        MyFont = SFont_InitFont(IMG_Load("5pxtinyfont.png"));

        Dessiner_ecran_principal();

        if(!Initialiser_config())
        {
            Setup();
            Enregistrer_config();

            /* On fait un peu de nettoyage avant de s'en aller */
            SFont_FreeFont(MyFont);
            SDL_Quit();
            exit(0);
        }
        else
        {
            puts("Error reading GFX2.DAT! File is absent or corrupt.");
            SFont_FreeFont(MyFont);
            SDL_Quit();
            exit(1);
        }
    }
    else
    {
        puts("Error: you mustn't run this setup program from a read-only drive!\n");
        puts("The most probable cause of this error is that you are running this program");
        puts("from a CD-Rom or a protected floppy disk.");
        puts("You should try to copy all the files of Grafx2 on a hard drive or to");
        puts("unprotect your floppy disk if you really want to run it from this outdated medium.");
        exit(1);
    }

    return 0;
}
