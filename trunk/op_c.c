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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>

#include "op_c.h"
#include "errors.h"

void RGB_to_HSL(int r,int g,int b,byte * hr,byte * sr,byte* lr)
{
  double rd,gd,bd,h,s,l,max,min;

  // convert RGB to HSV
  rd = r / 255.0;            // rd,gd,bd range 0-1 instead of 0-255
  gd = g / 255.0;
  bd = b / 255.0;

  // compute L
//  l=(rd*0.30)+(gd*0.59)+(bd*0.11);

  // compute maximum of rd,gd,bd
  if (rd>=gd)
  {
    if (rd>=bd)
      max = rd;
    else
      max = bd;
  }
  else
  {
    if (gd>=bd)
      max = gd;
    else
      max = bd;
  }

  // compute minimum of rd,gd,bd
  if (rd<=gd)
  {
    if (rd<=bd)
      min = rd;
    else
      min = bd;
  }
  else
  {
    if (gd<=bd)
      min = gd;
    else
      min = bd;
  }

  l = (max + min) / 2.0;

  if(max==min)
      s = h = 0;
  else
  {
    if (l<=0.5)
        s = (max - min) / (max + min);
    else
        s = (max - min) / (2 - (max + min));

    if (max == rd)
        h = 42.5 * (gd-bd)/(max-min);
    else if (max == gd)
        h = 42.5 * (bd-rd)/(max-min)+85;
    else
        h = 42.5 * (rd-gd)/(max-min)+170;
    if (h<0) h+=255;
  }

  *hr = h;
  *lr = (l*255.0);
  *sr = (s*255.0);
}

void HSL_to_RGB(byte h,byte s,byte l, byte* r, byte* g, byte* b)
{
    float rf =0 ,gf = 0,bf = 0;
    float hf,lf,sf;
    float p,q;

    if(s==0)
    {
        *r=*g=*b=l;
        return;
    }

    hf = h / 255.0;
    lf = l / 255.0;
    sf = s / 255.0;

    if (lf<=0.5)
        q = lf*(1+sf);
    else
        q = lf+sf-lf*sf;
    p = 2*lf-q;

    rf = hf + (1 / 3.0);
    gf = hf;
    bf = hf - (1 / 3.0);

    if (rf < 0) rf+=1;
    if (rf > 1) rf-=1;
    if (gf < 0) gf+=1;
    if (gf > 1) gf-=1;
    if (bf < 0) bf+=1;
    if (bf > 1) bf-=1;

    if (rf < 1/6.0)
        rf = p + ((q-p)*6*rf);
    else if(rf < 0.5)
        rf = q;
    else if(rf < 2/3.0)
        rf = p + ((q-p)*6*(2/3.0-rf));
    else
        rf = p;

    if (gf < 1/6.0)
        gf = p + ((q-p)*6*gf);
    else if(gf < 0.5)
        gf = q;
    else if(gf < 2/3.0)
        gf = p + ((q-p)*6*(2/3.0-gf));
    else
        gf = p;

    if (bf < 1/6.0)
        bf = p + ((q-p)*6*bf);
    else if(bf < 0.5)
        bf = q;
    else if(bf < 2/3.0)
        bf = p + ((q-p)*6*(2/3.0-bf));
    else
        bf = p;

    *r = rf * (255);
    *g = gf * (255);
    *b = bf * (255);
}

/////////////////////////////////////////////////////////////////////////////
///////////////////////////// M�thodes de gestion des tables de conversion //
/////////////////////////////////////////////////////////////////////////////

T_Conversion_table * CT_new(int nbb_r,int nbb_g,int nbb_b)
{
  T_Conversion_table * n;
  int size;

  n=(T_Conversion_table *)malloc(sizeof(T_Conversion_table));
  if (n!=NULL)
  {
    // On recopie les param�tres demand�s
    n->nbb_r=nbb_r;
    n->nbb_g=nbb_g;
    n->nbb_b=nbb_b;

    // On calcule les autres
    n->rng_r=(1<<nbb_r);
    n->rng_g=(1<<nbb_g);
    n->rng_b=(1<<nbb_b);
    n->dec_r=nbb_g+nbb_b;
    n->dec_g=nbb_b;
    n->dec_b=0;
    n->red_r=8-nbb_r;
    n->red_g=8-nbb_g;
    n->red_b=8-nbb_b;

    // On tente d'allouer la table
    size=(n->rng_r)*(n->rng_g)*(n->rng_b);
    n->table=(byte *)malloc(size);
    if (n->table!=NULL)
      // C'est bon!
      memset(n->table,0,size); // Inutile, mais plus propre
    else
    {
      // Table impossible � allouer
      free(n);
      n=NULL;
    }
  }

  return n;
}

void CT_delete(T_Conversion_table * t)
{
  free(t->table);
  free(t);
}

byte CT_get(T_Conversion_table * t,int r,int g,int b)
{
  int index;

  // On r�duit le nombre de bits par couleur
  r=(r>>t->red_r);
  g=(g>>t->red_g);
  b=(b>>t->red_b);
  
  // On recherche la couleur la plus proche dans la table de conversion
  index=(r<<t->dec_r) | (g<<t->dec_g) | (b<<t->dec_b);

  return t->table[index];
}

void CT_set(T_Conversion_table * t,int r,int g,int b,byte i)
{
  int index;

  index=(r<<t->dec_r) | (g<<t->dec_g) | (b<<t->dec_b);
  t->table[index]=i;
}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////// M�thodes de gestion des tables d'occurence //
/////////////////////////////////////////////////////////////////////////////

void OT_init(T_Occurrence_table * t)
{
  int size;

  size=(t->rng_r)*(t->rng_g)*(t->rng_b)*sizeof(int);
  memset(t->table,0,size); // On initialise � 0
}

T_Occurrence_table * OT_new(int nbb_r,int nbb_g,int nbb_b)
{
  T_Occurrence_table * n;
  int size;

  n=(T_Occurrence_table *)malloc(sizeof(T_Occurrence_table));
  if (n!=0)
  {
    // On recopie les param�tres demand�s
    n->nbb_r=nbb_r;
    n->nbb_g=nbb_g;
    n->nbb_b=nbb_b;

    // On calcule les autres
    n->rng_r=(1<<nbb_r);
    n->rng_g=(1<<nbb_g);
    n->rng_b=(1<<nbb_b);
    n->dec_r=nbb_g+nbb_b;
    n->dec_g=nbb_b;
    n->dec_b=0;
    n->red_r=8-nbb_r;
    n->red_g=8-nbb_g;
    n->red_b=8-nbb_b;

    // On tente d'allouer la table
    size=(n->rng_r)*(n->rng_g)*(n->rng_b)*sizeof(int);
    n->table=(int *)malloc(size);
    if (n->table!=0)
      // C'est bon! On initialise � 0
      OT_init(n);
    else
    {
      // Table impossible � allouer
      free(n);
      n=0;
    }
  }

  return n;
}

void OT_delete(T_Occurrence_table * t)
{
  free(t->table);
  free(t);
}

int OT_get(T_Occurrence_table * t,int r,int g,int b)
{
  int index;

  index=(r<<t->dec_r) | (g<<t->dec_g) | (b<<t->dec_b);
  return t->table[index];
}

void OT_set(T_Occurrence_table * t,int r,int g,int b,int i)
{
  int index;

  r=(r>>t->red_r);
  g=(g>>t->red_g);
  b=(b>>t->red_b);
  index=(r<<t->dec_r) | (g<<t->dec_g) | (b<<t->dec_b);
  t->table[index]=i;
}

void OT_inc(T_Occurrence_table * t,int r,int g,int b)
{
  int index;

  r=(r>>t->red_r);
  g=(g>>t->red_g);
  b=(b>>t->red_b);
  index=(r<<t->dec_r) | (g<<t->dec_g) | (b<<t->dec_b);
  t->table[index]++;
}

void OT_count_occurrences(T_Occurrence_table * t,T_Bitmap24B image,int size)
{
  T_Bitmap24B ptr;
  int index;

  for (index=size,ptr=image;index>0;index--,ptr++)
    OT_inc(t,ptr->R,ptr->G,ptr->B);
}

int OT_count_colors(T_Occurrence_table * t)
{
  int val; // Valeur de retour
  int nb;  // Nombre de couleurs � tester
  int i;   // Compteur de couleurs test�es

  val=0;
  nb=(t->rng_r)*(t->rng_g)*(t->rng_b);
  for (i=0;i<nb;i++)
    if (t->table[i]>0)
      val++;

  return val;
}



/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// M�thodes de gestion des clusters //
/////////////////////////////////////////////////////////////////////////////

void Cluster_analyser(T_Cluster * c,T_Occurrence_table * to)
{
  int rmin,rmax,vmin,vmax,bmin,bmax;
  int r,g,b;

  // On cherche les mins et les maxs de chaque composante sur la couverture

  // int nbocc;

  // On pr�d�cale tout pour �viter de faire trop de bazar en se for�ant � utiliser OT_get, plus rapide
  rmin=c->rmax <<16; rmax=c->rmin << 16;
  vmin=c->vmax << 8; vmax=c->vmin << 8;
  bmin=c->bmax; bmax=c->bmin;
  c->occurences=0;
  /*
  for (r=c->rmin<<16;r<=c->rmax<<16;r+=1<<16)
    for (g=c->vmin<<8;g<=c->vmax<<8;g+=1<<8)
      for (b=c->bmin;b<=c->bmax;b++)
      {
        nbocc=to->table[r + g + b]; // OT_get
        if (nbocc)
        {
          if (r<rmin) rmin=r;
          else if (r>rmax) rmax=r;
          if (g<vmin) vmin=g;
          else if (g>vmax) vmax=g;
          if (b<bmin) bmin=b;
          else if (b>bmax) bmax=b;
          c->occurences+=nbocc;
        }
      }
  */

  // On recherche le minimum et le maximum en parcourant le cluster selon chaque composante, 
  // �a �vite des acc�s m�moires inutiles, de plus chaque boucle est plus petite que la 
  // pr�c�dente puisqu'on connait une borne suppl�mentaire

  for(r=c->rmin<<16;r<=c->rmax<<16;r+=1<<16)
      for(g=c->vmin<<8;g<=c->vmax<<8;g+=1<<8)
          for(b=c->bmin;b<=c->bmax;b++)
          {
            if(to->table[r + g + b]) // OT_get
            {
                rmin=r;
                goto RMAX;
            }
          }
RMAX:
  for(r=c->rmax<<16;r>=rmin;r-=1<<16)
      for(g=c->vmin<<8;g<=c->vmax<<8;g+=1<<8)
          for(b=c->bmin;b<=c->bmax;b++)
          {
            if(to->table[r + g + b]) // OT_get
            {
                rmax=r;
                goto VMIN;
            }
          }
VMIN:
  for(g=c->vmin<<8;g<=c->vmax<<8;g+=1<<8)
      for(r=rmin;r<=rmax;r+=1<<16)
          for(b=c->bmin;b<=c->bmax;b++)
          {
            if(to->table[r + g + b]) // OT_get
            {
                vmin=g;
                goto VMAX;
            }
          }
VMAX:
  for(g=c->vmax<<8;g>=vmin;g-=1<<8)
      for(r=rmin;r<=rmax;r+=1<<16)
          for(b=c->bmin;b<=c->bmax;b++)
          {
            if(to->table[r + g + b]) // OT_get
            {
                vmax=g;
                goto BMIN;
            }
          }
BMIN:
  for(b=c->bmin;b<=c->bmax;b++)
      for(r=rmin;r<=rmax;r+=1<<16)
          for(g=vmin;g<=vmax;g+=1<<8)
          {
            if(to->table[r + g + b]) // OT_get
            {
                bmin=b;
                goto BMAX;
            }
          }
BMAX:
  for(b=c->bmax;b>=bmin;b--)
      for(r=rmin;r<=rmax;r+=1<<16)
          for(g=vmin;g<=vmax;g+=1<<8)
          {
            if(to->table[r + g + b]) // OT_get
            {
                bmax=b;
                goto ENDCRUSH;
            }
          }
ENDCRUSH:
  // Il faut quand m�me parcourir la partie utile du cluster, pour savoir combien il y a d'occurences
  for(r=rmin;r<=rmax;r+=1<<16)
      for(g=vmin;g<=vmax;g+=1<<8)
          for(b=bmin;b<=bmax;b++)
          {
            c->occurences+=to->table[r + g + b]; // OT_get
          }

  c->rmin=rmin>>16; c->rmax=rmax>>16;
  c->vmin=vmin>>8;  c->vmax=vmax>>8;
  c->bmin=bmin;     c->bmax=bmax;

  // On regarde la composante qui a la variation la plus grande
  r=(c->rmax-c->rmin)*299;
  g=(c->vmax-c->vmin)*587;
  b=(c->bmax-c->bmin)*114;

  if (g>=r)
  {
    // G>=R
    if (g>=b)
    {
      // G>=R et G>=B
      c->plus_large=1;
    }
    else
    {
      // G>=R et G<B
      c->plus_large=2;
    }
  }
  else
  {
    // R>G
    if (r>=b)
    {
      // R>G et R>=B
      c->plus_large=0;
    }
    else
    {
      // R>G et R<B
      c->plus_large=2;
    }
  }
}

void Cluster_split(T_Cluster * c,T_Cluster * c1,T_Cluster * c2,int hue,T_Occurrence_table * to)
{
  int limit;
  int cumul;
  int r,g,b;

  limit=(c->occurences)/2;
  cumul=0;
  if (hue==0)
  {
    for (r=c->rmin<<16;r<=c->rmax<<16;r+=1<<16)
    {
      for (g=c->vmin<<8;g<=c->vmax<<8;g+=1<<8)
      {
        for (b=c->bmin;b<=c->bmax;b++)
        {
          cumul+=to->table[r + g + b];
          if (cumul>=limit)
            break;
        }
        if (cumul>=limit)
          break;
      }
      if (cumul>=limit)
        break;
    }

    r>>=16;
    g>>=8;

    if (r==c->rmin)
      r++;
    // R est la valeur de d�but du 2nd cluster

    c1->Rmin=c->Rmin; c1->Rmax=r-1;
    c1->rmin=c->rmin; c1->rmax=r-1;
    c1->Gmin=c->Gmin; c1->Vmax=c->Vmax;
    c1->vmin=c->vmin; c1->vmax=c->vmax;
    c1->Bmin=c->Bmin; c1->Bmax=c->Bmax;
    c1->bmin=c->bmin; c1->bmax=c->bmax;
    c2->Rmin=r;       c2->Rmax=c->Rmax;
    c2->rmin=r;       c2->rmax=c->rmax;
    c2->Gmin=c->Gmin; c2->Vmax=c->Vmax;
    c2->vmin=c->vmin; c2->vmax=c->vmax;
    c2->Bmin=c->Bmin; c2->Bmax=c->Bmax;
    c2->bmin=c->bmin; c2->bmax=c->bmax;
  }
  else
  if (hue==1)
  {

    for (g=c->vmin<<8;g<=c->vmax<<8;g+=1<<8)
    {
      for (r=c->rmin<<16;r<=c->rmax<<16;r+=1<<16)
      {
        for (b=c->bmin;b<=c->bmax;b++)
        {
          cumul+=to->table[r + g + b];
          if (cumul>=limit)
            break;
        }
        if (cumul>=limit)
          break;
      }
      if (cumul>=limit)
        break;
    }

    r>>=16; g>>=8;

    if (g==c->vmin)
      g++;
    // G est la valeur de d�but du 2nd cluster

    c1->Rmin=c->Rmin; c1->Rmax=c->Rmax;
    c1->rmin=c->rmin; c1->rmax=c->rmax;
    c1->Gmin=c->Gmin; c1->Vmax=g-1;
    c1->vmin=c->vmin; c1->vmax=g-1;
    c1->Bmin=c->Bmin; c1->Bmax=c->Bmax;
    c1->bmin=c->bmin; c1->bmax=c->bmax;
    c2->Rmin=c->Rmin; c2->Rmax=c->Rmax;
    c2->rmin=c->rmin; c2->rmax=c->rmax;
    c2->Gmin=g;       c2->Vmax=c->Vmax;
    c2->vmin=g;       c2->vmax=c->vmax;
    c2->Bmin=c->Bmin; c2->Bmax=c->Bmax;
    c2->bmin=c->bmin; c2->bmax=c->bmax;
  }
  else
  {

    for (b=c->bmin;b<=c->bmax;b++)
    {
      for (g=c->vmin<<8;g<=c->vmax<<8;g+=1<<8)
      {
        for (r=c->rmin<<16;r<=c->rmax<<16;r+=1<<16)
        {
          cumul+=to->table[r + g + b];
          if (cumul>=limit)
            break;
        }
        if (cumul>=limit)
          break;
      }
      if (cumul>=limit)
        break;
    }

    r>>=16; g>>=8;

    if (b==c->bmin)
      b++;
    // B est la valeur de d�but du 2nd cluster

    c1->Rmin=c->Rmin; c1->Rmax=c->Rmax;
    c1->rmin=c->rmin; c1->rmax=c->rmax;
    c1->Gmin=c->Gmin; c1->Vmax=c->Vmax;
    c1->vmin=c->vmin; c1->vmax=c->vmax;
    c1->Bmin=c->Bmin; c1->Bmax=b-1;
    c1->bmin=c->bmin; c1->bmax=b-1;
    c2->Rmin=c->Rmin; c2->Rmax=c->Rmax;
    c2->rmin=c->rmin; c2->rmax=c->rmax;
    c2->Gmin=c->Gmin; c2->Vmax=c->Vmax;
    c2->vmin=c->vmin; c2->vmax=c->vmax;
    c2->Bmin=b;       c2->Bmax=c->Bmax;
    c2->bmin=b;       c2->bmax=c->bmax;
  }
}

void Cluster_compute_hue(T_Cluster * c,T_Occurrence_table * to)
{
  int cumul_r,cumul_g,cumul_b;
  int r,g,b;
  int nbocc;

  byte s=0;

  cumul_r=cumul_g=cumul_b=0;
  for (r=c->rmin;r<=c->rmax;r++)
    for (g=c->vmin;g<=c->vmax;g++)
      for (b=c->bmin;b<=c->bmax;b++)
      {
        nbocc=OT_get(to,r,g,b);
        if (nbocc)
        {
          cumul_r+=r*nbocc;
          cumul_g+=g*nbocc;
          cumul_b+=b*nbocc;
        }
      }
  
  c->r=(cumul_r<<to->red_r)/c->occurences;
  c->g=(cumul_g<<to->red_g)/c->occurences;
  c->b=(cumul_b<<to->red_b)/c->occurences;
  RGB_to_HSL(c->r,c->g,c->b,&c->h,&s,&c->l);
}



/////////////////////////////////////////////////////////////////////////////
//////////////////////////// M�thodes de gestion des ensembles de clusters //
/////////////////////////////////////////////////////////////////////////////

void CS_Init(T_Cluster_set * cs,T_Occurrence_table * to)
{
  cs->clusters[0].Rmin=cs->clusters[0].rmin=0;
  cs->clusters[0].Gmin=cs->clusters[0].vmin=0;
  cs->clusters[0].Bmin=cs->clusters[0].bmin=0;
  cs->clusters[0].Rmax=cs->clusters[0].rmax=to->rng_r-1;
  cs->clusters[0].Vmax=cs->clusters[0].vmax=to->rng_g-1;
  cs->clusters[0].Bmax=cs->clusters[0].bmax=to->rng_b-1;
  Cluster_analyser(cs->clusters+0,to);
  // Et hop : le 1er ensemble de couleurs est initialis�
  cs->nb=1;
}

T_Cluster_set * CS_New(int nbmax,T_Occurrence_table * to)
{
  T_Cluster_set * n;

  n=(T_Cluster_set *)malloc(sizeof(T_Cluster_set));
  if (n!=0)
  {
    // On recopie les param�tres demand�s
    n->nb_max=OT_count_colors(to);

    // On vient de compter le nombre de couleurs existantes, s'il est plus grand que 256 on limit � 256 (nombre de couleurs voulu au final)
    if (n->nb_max>nbmax)
    {
      n->nb_max=nbmax;
    }

    // On tente d'allouer la table
    n->clusters=(T_Cluster *)malloc(nbmax*sizeof(T_Cluster));
    if (n->clusters!=NULL)
      // C'est bon! On initialise
      CS_Init(n,to);
    else
    {
      // Table impossible � allouer
      free(n);
      n=0;
    }
  }

  return n;
}

void CS_Delete(T_Cluster_set * cs)
{
  free(cs->clusters);
  free(cs);
}

void CS_Get(T_Cluster_set * cs,T_Cluster * c)
{
  int index;

  // On cherche un cluster que l'on peut couper en deux, donc avec au moins deux valeurs
  // diff�rentes sur l'une des composantes
  for (index=0;index<cs->nb;index++)
    if ( (cs->clusters[index].rmin<cs->clusters[index].rmax) ||
         (cs->clusters[index].vmin<cs->clusters[index].vmax) ||
         (cs->clusters[index].bmin<cs->clusters[index].bmax) )
      break;

  // On le recopie dans c
  *c=cs->clusters[index];

  // On d�cr�mente le nombre et on d�cale tous les clusters suivants
  // Sachant qu'on va r�ins�rer juste apr�s, il me semble que �a serait une bonne id�e de g�rer les clusters 
  // comme une liste chain�e... on n'a aucun acc�s direct dedans, que des parcours ...
  cs->nb--;
  memcpy((cs->clusters+index),(cs->clusters+index+1),(cs->nb-index)*sizeof(T_Cluster));
}

void CS_Set(T_Cluster_set * cs,T_Cluster * c)
{
  int index;
  // int decalage;

  // Le tableau des clusters est tri� par nombre d'occurences. Donc on cherche la position du premier cluster 
  // qui est plus grand que le notre
  for (index=0;index<cs->nb;index++)
    if (cs->clusters[index].occurences<c->occurences)
/*
    if (((OPTPAL_Cluster[index].rmax-OPTPAL_Cluster[index].rmin+1)*
         (OPTPAL_Cluster[index].gmax-OPTPAL_Cluster[index].gmin+1)*
         (OPTPAL_Cluster[index].bmax-OPTPAL_Cluster[index].bmin+1))
        <
        ((Set->rmax-Set->rmin+1)*
         (Set->gmax-Set->gmin+1)*
         (Set->bmax-Set->bmin+1))
       )
*/
      break;

  if (index<cs->nb)
  {
    // On distingue ici une insertion plutot qu'un placement en fin de liste.
    // On doit donc d�caler les ensembles suivants vers la fin pour se faire
    // une place dans la liste.

    //for (decalage=cs->nb;decalage>index;decalage--)
    //  memcpy((cs->clusters+decalage),(cs->clusters+decalage-1),sizeof(T_Cluster));
    memmove(cs->clusters+index+1,cs->clusters+index,(cs->nb-index)*sizeof(T_Cluster));
  }

  cs->clusters[index]=*c;
  cs->nb++;
}

// D�termination de la meilleure palette en utilisant l'algo Median Cut :
// 1) On consid�re l'espace (R,G,B) comme 1 bo�te
// 2) On cherche les extr�mes de la bo�te en (R,G,B)
// 3) On trie les pixels de l'image selon l'axe le plus long parmi (R,G,B)
// 4) On coupe la bo�te en deux au milieu, et on compacte pour que chaque bord corresponde bien � un pixel extreme
// 5) On recommence � couper selon le plus grand axe toutes bo�tes confondues
// 6) On s'arr�te quand on a le nombre de couleurs voulu
void CS_Generate(T_Cluster_set * cs,T_Occurrence_table * to)
{
  T_Cluster current;
  T_Cluster Nouveau1;
  T_Cluster Nouveau2;

  // Tant qu'on a moins de 256 clusters
  while (cs->nb<cs->nb_max)
  {
    // On r�cup�re le plus grand cluster
    CS_Get(cs,&current);

    // On le coupe en deux
    Cluster_split(&current,&Nouveau1,&Nouveau2,current.plus_large,to);

    // On compacte ces deux nouveaux (il peut y avoir un espace entre l'endroit de la coupure et les premiers pixels du cluster)
    Cluster_analyser(&Nouveau1,to);
    Cluster_analyser(&Nouveau2,to);

    // On met ces deux nouveaux clusters dans le clusterSet... sauf s'ils sont vides
    if(Nouveau1.occurences>0)
        CS_Set(cs,&Nouveau1);
    if(Nouveau2.occurences>0)
        CS_Set(cs,&Nouveau2);
  }
}

void CS_Compute_colors(T_Cluster_set * cs,T_Occurrence_table * to)
{
  int index;
  T_Cluster * c;

  for (index=0,c=cs->clusters;index<cs->nb;index++,c++)
    Cluster_compute_hue(c,to);
}

void CS_Sort_by_chrominance(T_Cluster_set * cs)
{
  int byte_used[256];
  int decalages[256];
  int index;
  T_Cluster * nc;

  nc=(T_Cluster *)malloc(cs->nb_max*sizeof(T_Cluster));

  // Initialisation de la table d'occurence de chaque octet
  for (index=0;index<256;index++)
    byte_used[index]=0;

  // Comptage du nombre d'occurences de chaque octet
  for (index=0;index<cs->nb;index++)
    byte_used[cs->clusters[index].h]++;

  // Calcul de la table des d�calages
  decalages[0]=0;
  for (index=1;index<256;index++)
    decalages[index]=decalages[index-1]+byte_used[index-1];

  // Copie r�ordonn�e dans la nouvelle liste
  for (index=0;index<cs->nb;index++)
  {
    nc[decalages[cs->clusters[index].h]]=cs->clusters[index];
    decalages[cs->clusters[index].h]++;
  }

  // Remplacement de la liste d�sordonn�e par celle tri�e
  free(cs->clusters);
  cs->clusters=nc;
}

void CS_Sort_by_luminance(T_Cluster_set * cs)
{
  int byte_used[256];
  int decalages[256];
  int index;
  T_Cluster * nc;

  nc=(T_Cluster *)malloc(cs->nb_max*sizeof(T_Cluster));

  // Initialisation de la table d'occurence de chaque octet
  for (index=0;index<256;index++)
    byte_used[index]=0;

  // Comptage du nombre d'occurences de chaque octet
  for (index=0;index<cs->nb;index++)
    byte_used[cs->clusters[index].l]++;

  // Calcul de la table des d�calages
  decalages[0]=0;
  for (index=1;index<256;index++)
    decalages[index]=decalages[index-1]+byte_used[index-1];

  // Copie r�ordonn�e dans la nouvelle liste
  for (index=0;index<cs->nb;index++)
  {
    nc[decalages[cs->clusters[index].l]]=cs->clusters[index];
    decalages[cs->clusters[index].l]++;
  }

  // Remplacement de la liste d�sordonn�e par celle tri�e
  free(cs->clusters);
  cs->clusters=nc;
}

void CS_Generate_color_table_and_palette(T_Cluster_set * cs,T_Conversion_table * tc,T_Components * palette)
{
  int index;
  int r,g,b;

  for (index=0;index<cs->nb;index++)
  {
    palette[index].R=cs->clusters[index].r;
    palette[index].G=cs->clusters[index].g;
    palette[index].B=cs->clusters[index].b;

    for (r=cs->clusters[index].Rmin;r<=cs->clusters[index].Rmax;r++)
      for (g=cs->clusters[index].Gmin;g<=cs->clusters[index].Vmax;g++)
        for (b=cs->clusters[index].Bmin;b<=cs->clusters[index].Bmax;b++)
          CT_set(tc,r,g,b,index);
  }
}

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// M�thodes de gestion des d�grad�s //
/////////////////////////////////////////////////////////////////////////////

void GS_Init(T_Gradient_set * ds,T_Cluster_set * cs)
{
    ds->gradients[0].nb_colors=1;
    ds->gradients[0].min=cs->clusters[0].h;
    ds->gradients[0].max=cs->clusters[0].h;
    ds->gradients[0].hue=cs->clusters[0].h;
    // Et hop : le 1er ensemble de d�grad�s est initialis�
    ds->nb=1;
}

T_Gradient_set * GS_New(T_Cluster_set * cs)
{
    T_Gradient_set * n;

    n=(T_Gradient_set *)malloc(sizeof(T_Gradient_set));
    if (n!=NULL)
    {
        // On recopie les param�tres demand�s
        n->nb_max=cs->nb_max;

        // On tente d'allouer la table
        n->gradients=(T_Gradient *)malloc((n->nb_max)*sizeof(T_Gradient));
        if (n->gradients!=0)
            // C'est bon! On initialise
            GS_Init(n,cs);
        else
        {
            // Table impossible � allouer
            free(n);
            n=0;
        }
    }

    return n;
}

void GS_Delete(T_Gradient_set * ds)
{
    free(ds->gradients);
    free(ds);
}

void GS_Generate(T_Gradient_set * ds,T_Cluster_set * cs)
{
    int ic,id; // Les indexs de parcours des ensembles
    int best_gradient; // Meilleur d�grad�
    int best_diff; // Meilleure diff�rence de chrominance
    int diff;  // difference de chrominance courante

    // Pour chacun des clusters � traiter
    for (ic=1;ic<cs->nb;ic++)
    {
        // On recherche le d�grad� le plus proche de la chrominance du cluster
        best_gradient=-1;
        best_diff=99999999;
        for (id=0;id<ds->nb;id++)
        {
            diff=abs(cs->clusters[ic].h - ds->gradients[id].hue);
            if ((best_diff>diff) && (diff<16))
            {
                best_gradient=id;
                best_diff=diff;
            }
        }

        // Si on a trouv� un d�grad� dans lequel inclure le cluster
        if (best_gradient!=-1)
    {
      // On met � jour le d�grad�
      if (cs->clusters[ic].h < ds->gradients[best_gradient].min)
        ds->gradients[best_gradient].min=cs->clusters[ic].h;
      if (cs->clusters[ic].h > ds->gradients[best_gradient].max)
        ds->gradients[best_gradient].max=cs->clusters[ic].h;
      ds->gradients[best_gradient].hue=((ds->gradients[best_gradient].hue*
                                ds->gradients[best_gradient].nb_colors)
                               +cs->clusters[ic].h)
                              /(ds->gradients[best_gradient].nb_colors+1);
      ds->gradients[best_gradient].nb_colors++;
    }
    else
    {
      // On cr�e un nouveau d�grad�
      best_gradient=ds->nb;
      ds->gradients[best_gradient].nb_colors=1;
      ds->gradients[best_gradient].min=cs->clusters[ic].h;
      ds->gradients[best_gradient].max=cs->clusters[ic].h;
      ds->gradients[best_gradient].hue=cs->clusters[ic].h;
      ds->nb++;
    }
    cs->clusters[ic].h=best_gradient;
  }

  // On redistribue les valeurs dans les clusters
  for (ic=0;ic<cs->nb;ic++)
    cs->clusters[ic].h=ds->gradients[cs->clusters[ic].h].hue;
}




T_Conversion_table * Optimize_palette(T_Bitmap24B image,int size,T_Components * palette,int r,int g,int b)
{
  T_Occurrence_table  * to;
  T_Conversion_table * tc;
  T_Cluster_set       * cs;
  T_Gradient_set       * ds;

  // Cr�ation des �l�ments n�cessaires au calcul de palette optimis�e:
  to=0; tc=0; cs=0; ds=0;

  to=OT_new(r,g,b);
  if (to!=0)
  {
    tc=CT_new(r,g,b);
    if (tc!=0)
    {

      // Premi�re �tape : on compte les pixels de chaque couleur pour pouvoir trier l� dessus
      OT_count_occurrences(to,image,size);

      cs=CS_New(256,to);
      if (cs!=0)
      {
        // C'est bon, on a pu tout allouer

        // On g�n�re les clusters (avec l'algo du median cut)
        CS_Generate(cs,to);

        // On calcule la teinte de chaque pixel (Luminance et chrominance)
        CS_Compute_colors(cs,to);

        ds=GS_New(cs);
        if (ds!=0)
        {
          GS_Generate(ds,cs);
          GS_Delete(ds);
        }

        // Enfin on trie les clusters (donc les couleurs de la palette) dans un ordre sympa : par couleur, et par luminosit� pour chaque couleur
        CS_Sort_by_luminance(cs);
        CS_Sort_by_chrominance(cs);

        // Enfin on g�n�re la palette et la table de correspondance entre chaque couleur 24b et sa couleur palette associ�e.
        CS_Generate_color_table_and_palette(cs,tc,palette);

        CS_Delete(cs);
        OT_delete(to);
        return tc;
      }
      CT_delete(tc);
    }
    OT_delete(to);
  }
  // Si on arrive ici c'est que l'allocation n'a pas r�ussi,
  // l'appelant devra recommencer avec une pr�cision plus faible (3 derniers param�tres)
  return 0;
}

int Modified_value(int value,int modif)
{
  value+=modif;
  if (value<0)
  {
    value=0;
  }
  else if (value>255)
  {
    value=255;
  }
  return value;
}

void Convert_24b_bitmap_to_256_Floyd_Steinberg(T_Bitmap256 dest,T_Bitmap24B source,int width,int height,T_Components * palette,T_Conversion_table * tc)
// Cette fonction d�grade au fur et � mesure le bitmap source, donc soit on ne
// s'en ressert pas, soit on passe � la fonction une copie de travail du
// bitmap original.
{
  T_Bitmap24B current;
  T_Bitmap24B c_plus1;
  T_Bitmap24B u_minus1;
  T_Bitmap24B next;
  T_Bitmap24B u_plus1;
  T_Bitmap256 d;
  int x_pos,y_pos;
  int red,green,blue;
  float e_red,e_green,e_blue;

  // On initialise les variables de parcours:
  current =source;      // Le pixel dont on s'occupe
  next =current+width; // Le pixel en dessous
  c_plus1 =current+1;   // Le pixel � droite
  u_minus1=next-1;   // Le pixel en bas � gauche
  u_plus1 =next+1;   // Le pixel en bas � droite
  d       =dest;

  // On parcours chaque pixel:
  for (y_pos=0;y_pos<height;y_pos++)
  {
    for (x_pos=0;x_pos<width;x_pos++)
    {
      // On prends la meilleure couleur de la palette qui traduit la couleur
      // 24 bits de la source:
      red=current->R;
      green =current->G;
      blue =current->B;
      // Cherche la couleur correspondant dans la palette et la range dans l'image de destination
      *d=CT_get(tc,red,green,blue);

      // Puis on calcule pour chaque composante l'erreur d�e � l'approximation
      red-=palette[*d].R;
      green -=palette[*d].G;
      blue -=palette[*d].B;

      // Et dans chaque pixel voisin on propage l'erreur
      // A droite:
        e_red=(red*7)/16.0;
        e_green =(green *7)/16.0;
        e_blue =(blue *7)/16.0;
        if (x_pos+1<width)
        {
          // Modified_value fait la somme des 2 params en bornant sur [0,255]
          c_plus1->R=Modified_value(c_plus1->R,e_red);
          c_plus1->G=Modified_value(c_plus1->G,e_green );
          c_plus1->B=Modified_value(c_plus1->B,e_blue );
        }
      // En bas � gauche:
      if (y_pos+1<height)
      {
        e_red=(red*3)/16.0;
        e_green =(green *3)/16.0;
        e_blue =(blue *3)/16.0;
        if (x_pos>0)
        {
          u_minus1->R=Modified_value(u_minus1->R,e_red);
          u_minus1->G=Modified_value(u_minus1->G,e_green );
          u_minus1->B=Modified_value(u_minus1->B,e_blue );
        }
      // En bas:
        e_red=(red*5/16.0);
        e_green =(green*5 /16.0);
        e_blue =(blue*5 /16.0);
        next->R=Modified_value(next->R,e_red);
        next->G=Modified_value(next->G,e_green );
        next->B=Modified_value(next->B,e_blue );
      // En bas � droite:
        if (x_pos+1<width)
        {
        e_red=(red/16.0);
        e_green =(green /16.0);
        e_blue =(blue /16.0);
          u_plus1->R=Modified_value(u_plus1->R,e_red);
          u_plus1->G=Modified_value(u_plus1->G,e_green );
          u_plus1->B=Modified_value(u_plus1->B,e_blue );
        }
      }

      // On passe au pixel suivant :
      current++;
      c_plus1++;
      u_minus1++;
      next++;
      u_plus1++;
      d++;
    }
  }

}



static const byte precision_24b[]=
{
 8,8,8,
 6,6,6,
 6,6,5,
 5,6,5,
 5,5,5,
 5,5,4,
 4,5,4,
 4,4,4,
 4,4,3,
 3,4,3,
 3,3,3,
 3,3,2};


// Convertie avec le plus de pr�cision possible une image 24b en 256c
// Renvoie s'il y a eu une erreur ou pas..

// Cette fonction utilise l'algorithme "median cut" (Optimize_palette) pour trouver la palette, et diffuse les erreurs avec floyd-steinberg.

int Convert_24b_bitmap_to_256(T_Bitmap256 dest,T_Bitmap24B source,int width,int height,T_Components * palette)
{
  T_Conversion_table * table; // table de conversion
  int                ip;    // index de pr�cision pour la conversion

  // On essaye d'obtenir une table de conversion qui loge en m�moire, avec la
  // meilleure pr�cision possible
  for (ip=0;ip<(10*3);ip+=3)
  {
    table=Optimize_palette(source,width*height,palette,precision_24b[ip+0],
                            precision_24b[ip+1],precision_24b[ip+2]);
    if (table!=0)
      break;
  }

  if (table!=0)
  {
    Convert_24b_bitmap_to_256_Floyd_Steinberg(dest,source,width,height,palette,table);
    CT_delete(table);
    return 0;
  }
  else
    return 1;
}



