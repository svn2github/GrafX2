void _splitpath(char* Buffer, char* Chemin, char* Nom_Fichier);
/* S�pare dans la cha�ne pass�e dans Buffer le chemin d'acc�s du nom de fichier */

struct find_t {
  unsigned char attrib;
  char name[256];
};

int filelength(int);

void itoa(int source,char* dest, int longueur);
/* Integer TO Ascii */

#define _A_SUBDIR 1
