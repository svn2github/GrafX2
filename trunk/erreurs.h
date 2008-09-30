// Affcihenom fichier, n� ligne, nom fonction, une chaine et un entier.
#define DEBUG(y,z) printf("%s %d %s | %s : %d###\n",__FILE__,__LINE__,__func__,y,(unsigned int)z)

// DEBUG en hexad�cimal
#define DEBUGX(y,z) printf("%s %d %s | %s : %X###\n",__FILE__,__LINE__,__func__,y,(unsigned int)z)

#define UNIMPLEMENTED printf("%s %d %s non impl�ment� !\n",__FILE__,__LINE__,__func__);

#define UNTESTED printf("%s %d %s � tester !\n",__FILE__,__LINE__,__func__);

void Erreur_fonction(int, const char *, int, const char *);
#define Erreur(n) Erreur_fonction(n, __FILE__,__LINE__,__func__)

