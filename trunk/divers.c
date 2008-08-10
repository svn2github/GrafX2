#include <SDL/SDL.h>
#include "struct.h"
#include "sdlscreen.h"
#include "global.h"
#include "graph.h" //Afficher_curseur
#include "erreurs.h"
#include "boutons.h"
#include "moteur.h"
#include "divers.h"

// Gestion du mode texte de d�part (pour pouvoir y retourner en cas de probl�me
byte Recuperer_nb_lignes(void)
{
        puts("Recuperer_nb_lignes non impl�ment�!\n");
        return 0;
}

word Palette_Compter_nb_couleurs_utilisees(dword* Tableau)
{
    int Nombre_De_Pixels=0;
    Uint8* Pixel_Courant=Principal_Ecran;
    Uint8 Couleur;
    word Nombre_Couleurs=0;
    int i;

    //Calcul du nombre de pixels dans l'image
    Nombre_De_Pixels=Principal_Hauteur_image*Principal_Largeur_image;

    // On parcourt l'�cran courant pour compter les utilisations des couleurs
    for(i=0;i<Nombre_De_Pixels;i++)
    {
        Couleur=*Pixel_Courant; //on lit la couleur dans l'�cran

        Tableau[Couleur]++; //Un point de plus pour cette couleur

        // On passe au pixel suivant
        Pixel_Courant++;
    }

    //On va maintenant compter dans la table les couleurs utilis�es:
    for(i=0;i<256;i++)
    {
        if (Tableau[i]!=0)
            Nombre_Couleurs++;
    }

    return Nombre_Couleurs;
}

void Set_palette(T_Palette Palette)
{
        register int i;
        SDL_Color PaletteSDL[256];
        for(i=0;i<256;i++)
        {
                PaletteSDL[i].r=Palette[i].R*4; //Les couleurs VGA ne vont que de 0 � 63
                PaletteSDL[i].g=Palette[i].V*4;
                PaletteSDL[i].b=Palette[i].B*4;
        }
        SDL_SetPalette(Ecran_SDL,SDL_PHYSPAL|SDL_LOGPAL,PaletteSDL,0,256);
}

void Attendre_fin_de_click(void)
{
    SDL_Event event;

    //On attend que l'utilisateur relache la souris. Tous les autres �v�nements
    //sont ignor�s
    while(SDL_PollEvent(&event) && event.type == SDL_MOUSEBUTTONUP);

    //On indique � la gestion des E/S que le bouton est lach� et on rend la main
    Mouse_K=0;
    INPUT_Nouveau_Mouse_K=0;
}

void Effacer_image_courante_Stencil(byte Couleur, byte * Pochoir)
//Effacer l'image courante avec une certaine couleur en mode Stencil
{
    int Nombre_De_Pixels=0; //ECX
    //al=Couleur
    //edi=Ecran
    byte* Pixel_Courant=Ecran; //dl
    int i;

    Nombre_De_Pixels=Principal_Hauteur_image*Principal_Largeur_image;

    for(i=0;i<Nombre_De_Pixels;i++)
    {
        if (Pochoir[*Pixel_Courant]==0)
            *Pixel_Courant=Couleur;
        Pixel_Courant++;
    }
}

void Effacer_image_courante(byte Couleur)
// Effacer l'image courante avec une certaine couleur
{
    memset(
        Principal_Ecran ,
        Couleur ,
        Principal_Largeur_image * Principal_Hauteur_image
    );
}

void Sensibilite_souris(word X,word Y)
{
        puts("Sensibilite_souris non impl�ment�!");
}

void Get_input(void)
//Gestion des �v�nements: mouvement de la souris, clic sur les boutons, et utilisation du clavier.
{
    SDL_Event event;

    Touche=0;
    Touche_ASCII=0; // Par d�faut, il n'y a pas d'action sur le clavier.

    if( SDL_PollEvent(&event)) /* Il y a un �v�nement en attente */
    {
        switch( event.type)
        {
            case SDL_MOUSEMOTION:
                //Mouvement de la souris
                INPUT_Nouveau_Mouse_X = event.motion.x>>Mouse_Facteur_de_correction_X;
                INPUT_Nouveau_Mouse_Y = event.motion.y>>Mouse_Facteur_de_correction_Y;
            break;
            case SDL_MOUSEBUTTONDOWN:
                //Clic sur un des boutons de la souris
                switch(event.button.button)
		{
			case SDL_BUTTON_LEFT: INPUT_Nouveau_Mouse_K = 1; break;
			case SDL_BUTTON_MIDDLE: // Pour SDL, 2 = clic milieu. Pour nous c'est le clic droit
			case SDL_BUTTON_RIGHT: // Clic droit SDL, clic droit pour nous aussi ( pour le moment en tout cas)
				INPUT_Nouveau_Mouse_K = 2;
		}
            break;
            case SDL_MOUSEBUTTONUP:
                //Bouton souris relach�
                INPUT_Nouveau_Mouse_K=0;
            break;
            case SDL_KEYDOWN:
            {
                byte ok = 0;
                //Appui sur une touche du clavier

                //On met le scancode dans Touche"
                Touche = event.key.keysym.scancode;

                //...et le code ASCII dans Touche_ASCII
                Touche_ASCII=event.key.keysym.sym;

                //On ajoute aussi l'�tat des modifiers
                #define ekkm event.key.keysym.mod
                if (ekkm & (KMOD_LSHIFT | KMOD_RSHIFT))
                {
                    Touche |= 0x0100;
                }
                if (ekkm & (KMOD_LCTRL | KMOD_RCTRL))
                {
                    Touche |= 0x0200;
                }
                if (ekkm & (KMOD_LALT | KMOD_RALT))
                {
                    Touche |= 0x0400;
                }
                #undef ekkm
                
                //TODO revoir les scancodes qui sont dans le tableau
                //Config_Touche, �a correspond � rien !
                
                //Cas particulier: d�placement du curseur avec haut bas gauche droite
                //On doit interpr�ter �a comme un mvt de la souris
                
                if(Touche == Config_Touche[0])
                {
                    //[Touche] = Emulation de MOUSE UP
                    //si on est d�j� en haut on peut plus bouger
                    if(INPUT_Nouveau_Mouse_Y!=0)
                    {
                        INPUT_Nouveau_Mouse_Y--;
                        ok=1;
                    }
                }
                else if(Touche == Config_Touche[1])
                {
                    //[Touche] = Emulation de MOUSE DOWN
                    if(INPUT_Nouveau_Mouse_Y<Hauteur_ecran-1)
                    {
                        INPUT_Nouveau_Mouse_Y++;
                        ok=1;
                    }
                }
                else if(Touche == Config_Touche[2])
                {
                    //[Touche] = Emulation de MOUSE LEFT
                    if(INPUT_Nouveau_Mouse_X!=0)
                    {
                        INPUT_Nouveau_Mouse_X--;
                        ok=1;
                    }
                }
                else if(Touche == Config_Touche[3])
                {
                    //[Touche] = Emulation de MOUSE RIGHT

                    if(INPUT_Nouveau_Mouse_X<Largeur_ecran-1)
                    {
                        INPUT_Nouveau_Mouse_X++;
                        ok=1;
                    }
                }
                else if(Touche == Config_Touche[4])
                {
                    //[Touche] = Emulation de MOUSE CLICK LEFT
                    INPUT_Nouveau_Mouse_K=1;
                    ok=1;
                }
                else if(Touche == Config_Touche[5])
                {
                    //[Touche] = Emulation de MOUSE CLICK RIGHT
                    INPUT_Nouveau_Mouse_K=2;
                    ok=1;
                }

                if(ok)
                {
                    SDL_WarpMouse(
                        INPUT_Nouveau_Mouse_X<<Mouse_Facteur_de_correction_X,
                        INPUT_Nouveau_Mouse_Y<<Mouse_Facteur_de_correction_Y
                    );
                }
            }
            break;
        }
    }

    //Gestion "avanc�e" du curseur: interdire la descente du curseur dans le
    //menu lorsqu'on est en train de travailler dans l'image

    if(Operation_Taille_pile!=0)
    {
        byte bl=0;//BL va indiquer si on doit corriger la position du curseur

        //Si le curseur ne se trouve plus dans l'image
        if(Menu_Ordonnee<INPUT_Nouveau_Mouse_Y)
        {
            //On bloque le curseur en fin d'image
            bl++;
            INPUT_Nouveau_Mouse_Y=Menu_Ordonnee-1; //La ligne !!au-dessus!! du menu
        }

        if(Loupe_Mode)
        {
            if(Operation_dans_loupe==0)
            {
                if(INPUT_Nouveau_Mouse_X>=Principal_Split)
                {
                    bl++;
                    INPUT_Nouveau_Mouse_X=Principal_Split-1;
                }
            }
            else
            {
                if(INPUT_Nouveau_Mouse_X<Principal_X_Zoom)
                {
                    bl++;
                    INPUT_Nouveau_Mouse_X=Principal_X_Zoom;
                }
            }
        }

        if (bl)
        {
            SDL_WarpMouse(
                INPUT_Nouveau_Mouse_X<<Mouse_Facteur_de_correction_X,
                INPUT_Nouveau_Mouse_Y<<Mouse_Facteur_de_correction_Y
            );
        }


        if (Touche != 0)
        {
            //Enfin, on inhibe les touches (sauf si c'est un changement de couleur
            //ou de taille de pinceau lors d'une des operations suivantes:
            //OPERATION_DESSIN_CONTINU, OPERATION_DESSIN_DISCONTINU, OPERATION_SPRAY)
            if(Autoriser_changement_de_couleur_pendant_operation)
            {
                //A ce stade l�, on sait qu'on est dans une des 3 op�rations
                //supportant le changement de couleur ou de taille de pinceau.

                if(
                    (Touche != Config_Touche[6]) &&
                    (Touche != Config_Touche[7]) &&
                    (Touche != Config_Touche[8]) &&
                    (Touche != Config_Touche[9]) &&
                    (Touche != Config_Touche[10]) &&
                    (Touche != Config_Touche[11]) &&
                    (Touche != Config_Touche[12]) &&
                    (Touche != Config_Touche[13]) &&
                    (Touche != Config_Touche[14]) &&
                    (Touche != Config_Touche[15])
                )
                {
                        Touche=0;
                }
            }
            else Touche = 0;
        }
    }

    if (
        (INPUT_Nouveau_Mouse_X != Mouse_X) ||
        (INPUT_Nouveau_Mouse_Y != Mouse_Y) ||
        (INPUT_Nouveau_Mouse_K != Mouse_K)
    )
    {
        Forcer_affichage_curseur=0;
        Effacer_curseur(); // On efface le curseur AVANT de le d�placer...
        Mouse_X=INPUT_Nouveau_Mouse_X;
        Mouse_Y=INPUT_Nouveau_Mouse_Y;
        Mouse_K=INPUT_Nouveau_Mouse_K;
        Calculer_coordonnees_pinceau();
        Afficher_curseur();
    }
}


void Initialiser_chrono(dword Delai)
// D�marrer le chrono
{
  Chrono_delay = Delai;
  Chrono_cmp = SDL_GetTicks()/55;
  return;
}

void Wait_VBL(void)
// Attente de VBL. Pour avoir des scrollbars qui ont une vitesse raisonnable par exemple. SDL ne sait pas faire ?
{
	SDL_Delay(20); // On consid�re un �cran � 50Hz
}

void Passer_en_mode_texte(byte Nb_lignes)
{
        SDL_Quit(); //Ceci sera appell� � chaque sortie de mode vid�o == sortie du programme. le Nb_Lignes serait � enlever, cela dit, SDL s'en occupe tr�s bien tout seul.
}

void Pixel_dans_brosse             (word X,word Y,byte Couleur)
{
	*(Brosse+Y*Brosse_Largeur+X)=Couleur;
}

byte Lit_pixel_dans_brosse         (word X,word Y)
{
	return *(Brosse + Y * Brosse_Largeur + X);
}

void Clavier_de_depart(void)
{
	UNIMPLEMENTED
}

void Clavier_americain(void)
{
	UNIMPLEMENTED
}

word Detection_souris(void)
{
	UNIMPLEMENTED
        return 0;
}

byte Lit_pixel_dans_ecran_courant  (word X,word Y)
{
        return *(Principal_Ecran+Y*Principal_Largeur_image+X);
}

void Pixel_dans_ecran_courant      (word X,word Y,byte Couleur)
{
    byte* dest=(X+Y*Principal_Largeur_image+Principal_Ecran);
    *dest=Couleur;
}

void Remplacer_une_couleur(byte Ancienne_couleur, byte Nouvelle_couleur)
{
	byte* edi;
	
	// pour chaque pixel :
	for(edi = Principal_Ecran;edi < Principal_Ecran + Principal_Hauteur_image * Principal_Largeur_image;edi++)
		if (*edi == Ancienne_couleur)
			*edi = Nouvelle_couleur;
	SDL_UpdateRect(Ecran_SDL,0,0,0,0); // On pet TOUT a jour
	// C'est pas un probl�me car il n'y a pas de preview
}

void Ellipse_Calculer_limites(short Rayon_horizontal,short Rayon_vertical)
{
	uint64_t Ellipse_Limite;
	Ellipse_Rayon_horizontal_au_carre = 
		Table_des_carres[Rayon_horizontal];
	Ellipse_Rayon_vertical_au_carre = 
		Table_des_carres[Rayon_vertical];
	Ellipse_Limite = Ellipse_Rayon_horizontal_au_carre * Ellipse_Rayon_vertical_au_carre;
	Ellipse_Limite_Low = Ellipse_Limite & 0xFFFFFFFF;
	Ellipse_Limite >>= 32;
	Ellipse_Limite_High = Ellipse_Limite;
}

byte Pixel_dans_ellipse(void)
{
	uint64_t ediesi = Table_des_carres[abs(Ellipse_Curseur_X)] *
		(Ellipse_Rayon_vertical_au_carre) + 
		Table_des_carres[abs(Ellipse_Curseur_Y)] * 
		(Ellipse_Rayon_horizontal_au_carre);
	if((ediesi >> 32) <= Ellipse_Limite_High &&
	(ediesi & 0xFFFFFFFF)  <= Ellipse_Limite_Low) return 255;
	
        return 0;
}

byte Pixel_dans_cercle(void)
{
        if((Table_des_carres[abs(Cercle_Curseur_X)] + 
            Table_des_carres[abs(Cercle_Curseur_Y)] ) <= Cercle_Limite)
                return 255;
        return 0;
}

void Copier_une_partie_d_image_dans_une_autre(byte * Source,word S_Pos_X,word S_Pos_Y,word Largeur,word Hauteur,word Largeur_source,byte * Destination,word D_Pos_X,word D_Pos_Y,word Largeur_destination)
{
        // ESI = adresse de la source en (S_Pox_X,S_Pos_Y)
        byte* esi = Source + S_Pos_Y * Largeur_source + S_Pos_X;

        // EDI = adresse de la destination (D_Pos_X,D_Pos_Y)
        byte* edi = Destination + D_Pos_Y * Largeur_destination + D_Pos_X;

        int Ligne;

        // Pour chaque ligne
        for (Ligne=0;Ligne < Hauteur; Ligne++)
        {
                memcpy(edi,esi,Largeur);

                // Passe � la ligne suivante
                esi+=Largeur_source;
                edi+=Largeur_destination;
        }
        

}

byte Lit_pixel_dans_ecran_brouillon(word X,word Y)
{
        UNIMPLEMENTED
        return 0;
}

void Rotate_90_deg_LOWLEVEL(byte * Source,byte * Destination)
{
	byte* esi;
	byte* edi;
	word dx,bx,cx;

	//ESI = Point haut droit de la source
	byte* Debut_de_colonne = Source + Brosse_Largeur - 1;
	edi = Destination;

	// Largeur de la source = Hauteur de la destination
	dx = bx = Brosse_Largeur;
	
	// Pour chaque ligne
	for(dx = Brosse_Largeur;dx>0;dx--)
	{
		esi = Debut_de_colonne;
		// Pout chaque colonne
		for(cx=Brosse_Hauteur;cx>0;cx--)
		{
			*edi = *esi;
			esi+=Brosse_Largeur;
			edi++;
		}

		Debut_de_colonne--;
	}
}

void Remap_general_LOWLEVEL(byte * Table_conv,byte * Buffer,short Largeur,short Hauteur,short Largeur_buffer)
{
        UNIMPLEMENTED
}

void Copier_image_dans_brosse(short Debut_X,short Debut_Y,short Brosse_Largeur,short Brosse_Hauteur,word Largeur_image)
{
    byte* Src=Debut_Y*Largeur_image+Debut_X+Principal_Ecran; //Adr d�part image (ESI)
    byte* Dest=Brosse; //Adr dest brosse (EDI)
    int dx;

  for (dx=Brosse_Hauteur;dx!=0;dx--)
  //Pour chaque ligne
  {

    // On fait une copie de la ligne
    memcpy(Dest,Src,Brosse_Largeur);

    // On passe � la ligne suivante
    Src+=Largeur_image;
    Dest+=Brosse_Largeur;
   }

}

byte Lit_pixel_dans_ecran_feedback (word X,word Y)
{
        UNIMPLEMENTED
        return 0;
}

dword Round_div(dword Numerateur,dword Diviseur)
{
        return Numerateur/Diviseur;
}

byte Effet_Trame(word X,word Y)
{
        UNIMPLEMENTED
        return 0;
}

void Set_mouse_position(void)
{
    SDL_WarpMouse(
        Mouse_X << Mouse_Facteur_de_correction_X,
        Mouse_Y << Mouse_Facteur_de_correction_Y
    );
}

void Clip_mouse(void)
{
        UNIMPLEMENTED
}

void Remplacer_toutes_les_couleurs_dans_limites(byte * Table_de_remplacement)
{
        int Ligne;
        int Compteur;
	byte* Adresse;

	byte Ancien;

        // Pour chaque ligne :
        for(Ligne = Limite_Haut;Ligne <= Limite_Bas; Ligne++)
        {
                // Pour chaque pixel sur la ligne :
                for (Compteur = Limite_Gauche;Compteur <= Limite_Droite;Compteur ++)
                {
			Adresse = Principal_Ecran+Ligne*Principal_Largeur_image+Compteur;
			Ancien=*Adresse;
			*Adresse = Table_de_remplacement[Ancien];
                }
        }
}               

byte Lit_pixel_dans_ecran_backup (word X,word Y)
{
        return *(Ecran_backup + X + Principal_Largeur_image * Y);
}

byte Type_de_lecteur_de_disquette(byte Numero_de_lecteur)
// Numero_de_lecteur compris entre 0 et 3 (4 lecteurs de disquettes)
//
// R�sultat = 0 : Pas de lecteur
//            1 : Lecteur 360 Ko
//            2 : Lecteur 1.2 Mo
//            3 : Lecteur 720 Ko
//            4 : Lecteur 1.4 Mo
//            5 : Lecteur 2.8 Mo (??? pas sur ???)
//            6 : Lecteur 2.8 Mo
{
        UNIMPLEMENTED
        return 0;
}

byte Disk_map(byte Numero_de_lecteur)
{
        UNIMPLEMENTED
        return 0;
}

byte Disque_dur_present(byte Numero_de_disque)
{
        UNIMPLEMENTED
        return 0;
}

byte Lecteur_CDROM_present(byte Numero_de_lecteur)
{
        UNIMPLEMENTED
        return 0;
}

long Freespace(byte Numero_de_lecteur)
{
        UNIMPLEMENTED
        return 0;
}

byte Couleur_ILBM_line(word Pos_X, word Vraie_taille_ligne)
{
	// CL sera le rang auquel on extrait les bits de la couleur
	byte cl = 7 - (Pos_X & 7);
	int ax,bh,dx;
	byte bl;

	for(dx = HBPm1;dx>=0;dx--);
	{
	//CIL_Loop
		ax = (Vraie_taille_ligne * HBPm1 + Pos_X) >> 3;
		bh = (LBM_Buffer[ax] >> cl) & 1;

		bl = (bl << 1) + bh;
	}

	return bl;
}

void Palette_256_to_64(T_Palette Palette)
{
	int i;
	for(i=0;i<256;i++)
	{
		Palette[i].R = Palette[i].R >> 2;
		Palette[i].V = Palette[i].V >> 2;
		Palette[i].B = Palette[i].B >> 2;
	}
}

void Palette_64_to_256(T_Palette Palette)
{
        UNIMPLEMENTED
}

byte Effet_Colorize_interpole  (word X,word Y,byte Couleur)
{
        UNIMPLEMENTED
        return 0;
}

byte Effet_Colorize_additif    (word X,word Y,byte Couleur)
{
        UNIMPLEMENTED
        return 0;
}

byte Effet_Colorize_soustractif(word X,word Y,byte Couleur)
{
        UNIMPLEMENTED
        return 0;
}

void Tester_chrono(void)
{
        if((SDL_GetTicks()/55)-Chrono_delay>Chrono_cmp) Etat_chrono=1;
}

// Effectue uyne inversion de la brosse selon une droite horizontale
void Flip_Y_LOWLEVEL(void)
{
	// ESI pointe sur la partie haute de la brosse
	// EDI sur la partie basse
	byte* ESI = Brosse ;
	byte* EDI = Brosse + (Brosse_Hauteur - 1) *Brosse_Largeur;
	byte tmp;
	word cx;

	while(ESI < EDI)
	{
		// Il faut inverser les lignes point�es par ESI et
		// EDI ("Brosse_Largeur" octets en tout)

		for(cx = Brosse_Largeur;cx>0;cx--)
		{
			tmp = *ESI;
			*ESI = *EDI;
			*EDI = tmp;
			ESI++;
			EDI++;
		}

		// On change de ligne :
		// ESI pointe d�j� sur le d�but de la ligne suivante
		// EDI pointe sur la fin de la ligne en cours, il
		// doit pointer sur le d�but de la pr�c�dente...
		EDI -= 2 * Brosse_Largeur; // On recule de 2 lignes
	}
}

// Effectue une inversion de la brosse selon une droite verticale
void Flip_X_LOWLEVEL(void)
{
	// ESI pointe sur la partie gauche et EDI sur la partie
	// droite
	byte* ESI = Brosse;
	byte* EDI = Brosse + Brosse_Largeur - 1;

	byte* Debut_Ligne;
	byte* Fin_Ligne;
	byte tmp;
	word cx;

	while(ESI<EDI)
	{
		Debut_Ligne = ESI;
		Fin_Ligne = EDI;

		// On �change par colonnes
		for(cx=Brosse_Hauteur;cx>0;cx--)
		{
			tmp=*ESI;
			*ESI=*EDI;
			*EDI=tmp;
			EDI+=Brosse_Largeur;
			ESI+=Brosse_Largeur;
		}

		// On change de colonne
		// ESI > colonne suivante
		// EDI > colonne pr�c�dente
		ESI = Debut_Ligne + 1;
		EDI = Fin_Ligne - 1;
	}
}

// Faire une rotation de 180� de la brosse
void Rotate_180_deg_LOWLEVEL(void)
{
	// ESI pointe sur la partie sup�rieure de la brosse
	// EDI pointe sur la partie basse
	byte* ESI = Brosse;
	byte* EDI = Brosse + Brosse_Hauteur*Brosse_Largeur - 1;
	// EDI pointe sur le dernier pixel de la derniere ligne
	byte tmp;
	word cx;

	while(ESI < EDI)
	{
		// On �change les deux lignes point�es par EDI et
		// ESI (Brosse_Largeur octets)
		// En m�me temps, on �change les pixels, donc EDI
		// pointe sur la FIN de sa ligne

		for(cx=Brosse_Largeur;cx>0;cx--)
		{
			tmp = *ESI;
			*ESI = *EDI;
			*EDI = tmp;
		
			EDI--; // Attention ici on recule !
			ESI++;
		}
	}
}

void Tempo_jauge(byte Vitesse)
//Boucle d'attente pour faire bouger les scrollbars � une vitesse correcte
{
        while (Vitesse!=0)
        {
            Wait_VBL();
            Vitesse--;
        }
}

byte Meilleure_couleur_sans_exclusion(byte Rouge,byte Vert,byte Bleu)
{
        UNIMPLEMENTED
        return 0;
}

void Set_color(byte Couleur, byte Rouge, byte Vert, byte Bleu)
{
	SDL_Color comp;
	comp.r=Rouge;
	comp.g=Vert;
	comp.b=Bleu;
        SDL_SetColors(Ecran_SDL,&comp,Couleur,1);
}

void Scroll_picture(short Decalage_X,short Decalage_Y)
{
	byte* esi = Ecran_backup; //Source de la copie
	byte* edi = Principal_Ecran + Decalage_Y * Principal_Largeur_image + Decalage_X;
	const word ax = Principal_Largeur_image - Decalage_X; // Nombre de pixels � copier � droite
	word dx;
	for(dx = Principal_Hauteur_image - Decalage_Y;dx>0;dx--) 
	{
	// Pour chaque ligne
		memcpy(edi,esi,ax);
		memcpy(edi - Decalage_X,esi+ax,Decalage_X);

		// On passe � la ligne suivante
		edi += Principal_Largeur_image;
		esi += Principal_Largeur_image;
	}

	// On vient de faire le traitement pour otutes les lignes au-dessous de Decalage_Y
	// Maintenant on traite celles au dessus
	edi = Decalage_X + Principal_Ecran;
	for(dx = Decalage_Y;dx>0;dx--)
	{
		memcpy(edi,esi,ax);
		memcpy(edi - Decalage_X,esi+ax,Decalage_X);

		edi += Principal_Largeur_image;
		esi += Principal_Largeur_image;
	}

	SDL_UpdateRect(Ecran_SDL,0,0,0,0);
}

word Get_key(void)
{
 	SDL_Event event;

	Attendre_fin_de_click(); // On prend le controle de la boucle d'�v�nements, donc il ne faut pas qu'on rate la fin de click !

        SDL_EnableUNICODE(SDL_ENABLE); // On a besoin du caract�re

        while(1)
        {
                SDL_PollEvent(&event);
                if(event.type == SDL_KEYDOWN)
                {
                        // On retourne en mode standard pour la gestion normale
                        SDL_EnableUNICODE(SDL_DISABLE);
                        if (event.key.keysym.unicode <= 127 && event.key.keysym.unicode > 31)
				return event.key.keysym.unicode; // Pas de souci, on est en ASCII standard
			else
			{
				// Sinon c'est une touche sp�ciale, on retourne son scancode
				return event.key.keysym.sym;
			}
                }
        }
}

void Zoomer_une_ligne(byte* Ligne_originale, byte* Ligne_zoomee, 
        word Facteur, word Largeur
)
{
        byte couleur;
        word larg;

        // Pour chaque pixel
        for(larg=0;larg<Largeur;larg++){
                couleur = *Ligne_originale;

                memset(Ligne_zoomee,couleur,Facteur);
                Ligne_zoomee+=Facteur;

                Ligne_originale++;
        }
}
