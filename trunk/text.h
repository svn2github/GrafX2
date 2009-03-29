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
void Init_text(void);
// Informe si texte.c a �t� compil� avec l'option de support TrueType ou pas.
int TrueType_is_supported(void);
// Ajout d'une fonte � la liste.
void Add_font(char *name);
// Cr�e une brosse � partir des param�tres de texte demand�s.
byte *Render_text(const char *str, int font_number, int size, int antialias, int bold, int italic, int *width, int *height);
// Trouve le libell� d'affichage d'une fonte par son num�ro
char * Font_label(int index);
// Trouve le nom d'une fonte par son num�ro
char * Font_name(int index);
// V�rifie si une fonte donn�e est TrueType
char * TrueType_font(int index);
// Nombre de fontes d�clar�es
extern int Nb_fonts;
