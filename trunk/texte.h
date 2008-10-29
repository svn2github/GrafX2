/*  Grafx2 - The Ultimate 256-color bitmap paint program

    Copyright 2008 Yves Rizoud
    Copyright 2008 Adrien Destugues
    Copyright 1996-2001 Sunset Design (Guillaume Dorme & Karl Maritaud)

    Grafx2 is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; version 2
    of the License.

    Grafx2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Grafx2; if not, see <http://www.gnu.org/licenses/> or
    write to the Free Software Foundation, Inc.,
    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

// Initialisation � faire une fois au d�but du programme
void Initialisation_Texte(void);
// Informe si texte.c a �t� compil� avec l'option de support TrueType ou pas.
int Support_TrueType(void);
// Ajout d'une fonte � la liste.
void Ajout_fonte(char *Nom, int EstTrueType, int EstImage);
// Cr�e une brosse � partir des param�tres de texte demand�s.
byte *Rendu_Texte(const char *Chaine, int Numero_fonte, int Taille, int AntiAlias, int *Largeur, int *Hauteur);
// Trouve le libell� d'affichage d'une fonte par son num�ro
char * Libelle_fonte(int Indice);
// Trouve le nom d'une fonte par son num�ro
char * Nom_fonte(int Indice);
// Nombre de fontes d�clar�es
extern int Fonte_nombre;
