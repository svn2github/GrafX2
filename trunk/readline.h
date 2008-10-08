/*  Grafx2 - The Ultimate 256-color bitmap paint program

    Copyright 2007 Adrien Destugues
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

byte Readline(word Pos_X,word Pos_Y,char * Chaine,byte Taille_affichee,byte Type_saisie);
// Param�tres:
//   Pos_X, Pos_Y : Coordonn�es de la saisie dans la fen�tre
//   Chaine       : Cha�ne recevant la saisie (et contenant �ventuellement une valeur initiale)
//   Taille_maxi  : Nombre de caract�res logeant dans la zone de saisie
//   Type_saisie  : 0=Cha�ne, 1=Nombre, 2=Nom de fichier (255 caract�res r�els)
// Sortie:
//   0: Sortie par annulation (Esc.) / 1: sortie par acceptation (Return)
