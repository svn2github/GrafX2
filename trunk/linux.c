// Diverses fonctions qui existaient sous dos mais pas sous linux...
#ifdef __linux__

#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

void _splitpath(char* Buffer, char* Chemin, char* Nom_Fichier)
{
    int i=0, Position_Du_Dernier_Slash=0;
    do
    {
        Chemin[i]=Buffer[i];
        if (Buffer[i]=='/')
            Position_Du_Dernier_Slash=i;
        i++;
    }while (Buffer[i]!=0);

    i=Position_Du_Dernier_Slash+1;
    Chemin[i]=0;
    strcpy(Nom_Fichier,Buffer+i);
}

int filelength(int fichier)
{
	struct stat infos_fichier;
	fstat(fichier,&infos_fichier);
	return infos_fichier.st_size;
}

void itoa(int source,char* dest, int longueur)
{
	snprintf(dest,longueur,"%d",source);
}

#endif
