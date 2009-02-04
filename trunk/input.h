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

void Gere_Evenement_SDL(SDL_Event * event);

int  Get_input(void);

#define TOUCHE_MOUSEMIDDLE     (SDLK_LAST+1)
#define TOUCHE_MOUSEWHEELUP    (SDLK_LAST+2)
#define TOUCHE_MOUSEWHEELDOWN  (SDLK_LAST+3)
#define TOUCHE_BUTTON          (SDLK_LAST+4)
