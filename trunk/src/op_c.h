/* vim:expandtab:ts=2 sw=2:
*/
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
    along with Grafx2; if not, see <http://www.gnu.org/licenses/>
*/

//////////////////////////////////////////////////////////////////////////////
///@file op_c.h
/// Color reduction and color conversion (24b->8b, RGB<->HSL).
/// This is called op_c because half of the process was originally 
/// coded in op_asm, in assembler.
//////////////////////////////////////////////////////////////////////////////

#ifndef _OP_C_H_
#define _OP_C_H_

#include "struct.h"
#include "colorred.h"

//////////////////////////////////////////////// D�finition des types de base

typedef T_Components * T_Bitmap24B;
typedef byte * T_Bitmap256;


///////////////////////////////////////// D�finition d'une table d'occurences

typedef struct
{
  int nbb_r; // Nb de bits de pr�cision sur les rouges
  int nbb_g; // Nb de bits de pr�cision sur les verts
  int nbb_b; // Nb de bits de pr�cision sur les bleu

  int rng_r; // Nb de valeurs sur les rouges (= 1<<nbb_r)
  int rng_g; // Nb de valeurs sur les verts  (= 1<<nbb_g)
  int rng_b; // Nb de valeurs sur les bleus  (= 1<<nbb_b)

  int dec_r; // Coefficient multiplicateur d'acc�s dans la table (= nbb_g+nbb_b)
  int dec_g; // Coefficient multiplicateur d'acc�s dans la table (= nbb_b)
  int dec_b; // Coefficient multiplicateur d'acc�s dans la table (= 0)

  int red_r; // Coefficient r�ducteur de traduction d'une couleur rouge (= 8-nbb_r)
  int red_g; // Coefficient r�ducteur de traduction d'une couleur verte (= 8-nbb_g)
  int red_b; // Coefficient r�ducteur de traduction d'une couleur bleue (= 8-nbb_b)

  int * table;
} T_Occurrence_table;



///////////////////////////////////////// D�finition d'un ensemble de couleur

typedef struct S_Cluster
{
  int occurences; // Nb total d'occurences des couleurs de l'ensemble

  // Grande couverture
  byte Rmin,Rmax;
  byte Gmin,Vmax;
  byte Bmin,Bmax;

  // Couverture minimale
  byte rmin,rmax;
  byte vmin,vmax;
  byte bmin,bmax;

  byte plus_large; // Composante ayant la plus grande variation (0=red,1=green,2=blue)
  byte r,g,b;      // color synth�tisant l'ensemble
  byte h;          // Chrominance
  byte l;          // Luminosit�

  struct S_Cluster* next;
} T_Cluster;



//////////////////////////////////////// D�finition d'un ensemble de clusters

typedef struct
{
  int       nb;
  int       nb_max;
  T_Cluster * clusters;
} T_Cluster_set;



///////////////////////////////////////////////////// D�finition d'un d�grad�

typedef struct
{
  int   nb_colors; // Nombre de couleurs dans le d�grad�
  float min;        // Chrominance minimale du d�grad�
  float max;        // Chrominance maximale du d�grad�
  float hue;        // Chrominance moyenne du d�grad�
} T_Gradient;



///////////////////////////////////////// D�finition d'un ensemble de d�grad�

typedef struct
{
  int nb;             // Nombre de d�grad�s dans l'ensemble
  int nb_max;          // Nombre maximum de d�grad�s
  T_Gradient * gradients; // Les d�grad�s
} T_Gradient_set;

void RGB_to_HSL(int r, int g,int b, byte* h, byte*s, byte* l);
void HSL_to_RGB(byte h, byte s, byte l, byte* r, byte* g, byte* b);

long Perceptual_lightness(T_Components *color);

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////// M�thodes de gestion des tables d'occurence //
/////////////////////////////////////////////////////////////////////////////

void OT_init(T_Occurrence_table * t);
T_Occurrence_table * OT_new(int nbb_r,int nbb_g,int nbb_b);
void OT_delete(T_Occurrence_table * t);
int OT_get(T_Occurrence_table * t,int r,int g,int b);
void OT_inc(T_Occurrence_table * t,int r,int g,int b);
void OT_count_occurrences(T_Occurrence_table * t,T_Bitmap24B image,int size);



/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// M�thodes de gestion des clusters //
/////////////////////////////////////////////////////////////////////////////

void Cluster_pack(T_Cluster * c,T_Occurrence_table * to);
void Cluster_split(T_Cluster * c,T_Cluster * c1,T_Cluster * c2,int hue,T_Occurrence_table * to);
void Cluster_compute_hue(T_Cluster * c,T_Occurrence_table * to);



/////////////////////////////////////////////////////////////////////////////
//////////////////////////// M�thodes de gestion des ensembles de clusters //
/////////////////////////////////////////////////////////////////////////////

void CS_Init(T_Cluster_set * cs,T_Occurrence_table * to);
T_Cluster_set * CS_New(int nbmax,T_Occurrence_table * to);
void CS_Delete(T_Cluster_set * cs);
void CS_Get(T_Cluster_set * cs,T_Cluster * c);
void CS_Set(T_Cluster_set * cs,T_Cluster * c);
void CS_Generate(T_Cluster_set * cs,T_Occurrence_table * to, CT_Node** colorTree);
void CS_Compute_colors(T_Cluster_set * cs,T_Occurrence_table * to);
void CS_Generate_color_table_and_palette(T_Cluster_set * cs,CT_Node** tc,T_Components * palette);

/////////////////////////////////////////////////////////////////////////////
//////////////////////////// M�thodes de gestion des ensembles de d�grad�s //
/////////////////////////////////////////////////////////////////////////////

void GS_Init(T_Gradient_set * ds,T_Cluster_set * cs);
T_Gradient_set * GS_New(T_Cluster_set * cs);
void GS_Delete(T_Gradient_set * ds);
void GS_Generate(T_Gradient_set * ds,T_Cluster_set * cs);



// Convertie avec le plus de pr�cision possible une image 24b en 256c
// Renvoie s'il y a eu une erreur ou pas..
int Convert_24b_bitmap_to_256(T_Bitmap256 dest,T_Bitmap24B source,int width,int height,T_Components * palette);



#endif
