
byte Readline(word Pos_X,word Pos_Y,char * Chaine,byte Taille_affichee,byte Type_saisie);
// Param�tres:
//   Pos_X, Pos_Y : Coordonn�es de la saisie dans la fen�tre
//   Chaine       : Cha�ne recevant la saisie (et contenant �ventuellement une valeur initiale)
//   Taille_maxi  : Nombre de caract�res logeant dans la zone de saisie
//   Type_saisie  : 0=Cha�ne, 1=Nombre, 2=Nom de fichier (255 caract�res r�els)
// Sortie:
//   0: Sortie par annulation (Esc.) / 1: sortie par acceptation (Return)
