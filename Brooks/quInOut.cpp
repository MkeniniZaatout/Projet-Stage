#include <math.h>
#include <cstdlib>
#include <iostream>
#define PI 3.1415926535897932384626433832795


// Calcul des flux latéraux quOut
void quOut (float*** tabquOut, float***tabTetaHw, float** tabRg, float** tabPente, float** kSat, float** tetaHwMax, int ligne, int colonne,int subst)
{
// Calcul
for (int a = 1; a < ligne-1; a++)
for (int b = 1; b < colonne-1; b++)
for (int i = 0; i < subst-1; i++)
    {
        if (tabRg[a][b] == -1 || tabPente[a][b] == 0.0)   tabquOut[a][b][i] = 0;              // si il n'y a pas d'exutoire pas de sortie d'eau
        else if (tabTetaHw[a][b][i] == tetaHwMax[a][b])   tabquOut[a][b][i] = kSat[a][b] * sin(tabPente[a][b]);
        else                                        tabquOut[a][b][i] = 0;
    }
}




// Calcul des flux latéraux quIn
void quIn(float*** tabquIn, float*** tabquOut, float***tabTetaHw, float** tabRg, float** tabPente, float** kSat, float** tetaHwMax, int ligne, int colonne,int subst)
{
float alpha1 = 0, alpha2 = 0;
float a1=0,a2=0,a3=0,a4=0,a5=0,a6=0,a7=0,a8=0,a9=0;
float Pne=0, Pnw=0, Pse=0, Psw=0, Pn =0, Pe=0, Ps=0, Pw=0;

//Calcul des angles
a1=0*PI/180; a2=45*PI/180; a3=90*PI/180; a4=135*PI/180; a5=180*PI/180; a6=225*PI/180; a7=270*PI/180; a8=315*PI/180; a9=360*PI/180;

// remise à zéro du tenseur des flux
for (int a = 0; a < ligne; a++)
for (int b = 0; b < colonne; b++)
for (int i = 0; i < subst-1; i++)   tabquIn[a][b][i] = 0;


// Calcul
for (int a = 0; a < ligne; a++)
for (int b = 0; b < colonne; b++)
for (int i = 0; i < subst-1; i++)
    {


//a l'interieur de la matrice
if (a!=0 && a!=ligne-1 && b!=0 && b!=colonne-1)
    {


   if ( a6 >= tabRg [a][b+1] && tabRg [a][b+1]>=a5 && tabTetaHw[a][b+1][i] == tetaHwMax[a][b])//------------------------------
           {
                alpha2 = tabRg [a][b+1] - PI;
                alpha1 = PI/4 - alpha2;
                Pw = alpha1 /(PI/4);

                tabquIn[a][b][i] = tabquIn[a][b][i] + Pw * tabquOut[a][b+1][i];
            }


   if ( a5 > tabRg [a][b+1] &&  tabRg [a][b+1]>=a4  && tabTetaHw[a][b+1][i] == tetaHwMax[a][b])//------------------------------
           {
                alpha2 = tabRg [a][b+1] - (3*PI/4);
                alpha1 = PI/4 - alpha2;
                Pw = alpha2 /(PI/4);

                tabquIn[a][b][i] = tabquIn[a][b][i] + Pw * tabquOut[a][b+1][i];
           }


   if ( a6 > tabRg [a-1][b+1] && tabRg [a-1][b+1]>=a5  && tabTetaHw[a-1][b+1][i] == tetaHwMax[a][b])//------------------------------
           {
                alpha2 = tabRg [a-1][b+1] - PI;
                alpha1 = PI/4 - alpha2;
                Psw = alpha2 /(PI/4);

                tabquIn[a][b][i] = tabquIn[a][b][i] + Psw * tabquOut[a-1][b+1][i];
           }


   if ( a7 >= tabRg [a-1][b+1] && tabRg [a-1][b+1]>=a6  && tabTetaHw[a-1][b+1][i] == tetaHwMax[a][b])//------------------------------
           {
                alpha2 =  tabRg [a-1][b+1] - (5*PI/4);
                alpha1 = PI/4 - alpha2;
                Psw = alpha1 /(PI/4);

                tabquIn[a][b][i] = tabquIn[a][b][i] + Psw * tabquOut[a-1][b+1][i];
           }


   if ( a8 >= tabRg [a-1][b] &&  tabRg [a-1][b]>=a7  && tabTetaHw[a-1][b][i] == tetaHwMax[a][b])//------------------------------
           {
                alpha2 = tabRg [a-1][b] - (6*PI/4);
                alpha1 = PI/4 - alpha2;
                Ps = alpha1 /(PI/4);

                tabquIn[a][b][i] = tabquIn[a][b][i] + Ps * tabquOut[a-1][b][i];
            }


   if ( a7 > tabRg [a-1][b] && tabRg [a-1][b]>=a6  && tabTetaHw[a-1][b][i] == tetaHwMax[a][b]) //------------------------------
           {
                alpha2 = tabRg [a-1][b] - (5*PI/4);
                alpha1 = PI/4 - alpha2;
                Ps = alpha2 /(PI/4);

                tabquIn[a][b][i] = tabquIn[a][b][i] + Ps * tabquOut[a-1][b][i];
           }


   if ( a9 >= tabRg [a-1][b-1] && tabRg [a-1][b-1]>=a8  && tabTetaHw[a-1][b-1][i] == tetaHwMax[a][b])//------------------------------
           {
                alpha2 = tabRg [a-1][b-1] - (7*PI/4);
                alpha1 = PI/4 - alpha2;
                Pse = alpha1 /(PI/4);

                tabquIn[a][b][i] = tabquIn[a][b][i] + Pse * tabquOut[a-1][b-1][i];
           }


   if ( a8 > tabRg [a-1][b-1] && tabRg [a-1][b-1]>=a7  && tabTetaHw[a-1][b-1][i] == tetaHwMax[a][b])//------------------------------
           {
                alpha2 = tabRg [a-1][b-1] - (6*PI/4);
                alpha1 = PI/4 - alpha2;
                Pse = alpha2 /(PI/4);

                tabquIn[a][b][i] = tabquIn[a][b][i] + Pse * tabquOut[a-1][b-1][i];
           }


   if ( a2 >= tabRg [a][b-1] && tabRg [a][b-1]>=a1 && tabTetaHw[a][b-1][i] == tetaHwMax[a][b])//------------------------------
           {
                alpha2 = tabRg [a][b-1];
                alpha1 = PI/4 - alpha2;
                Pe = alpha1 /(PI/4);

                tabquIn[a][b][i] = tabquIn[a][b][i] + Pe * tabquOut[a][b-1][i];
           }


   if ( a9 >= tabRg [a][b-1] && tabRg [a][b-1]>=a8  && tabTetaHw[a][b-1][i] == tetaHwMax[a][b])//------------------------------
           {
                alpha2 = tabRg [a][b-1] - (7*PI/4);
                alpha1 = PI/4 - alpha2;
                Pe = alpha2 /(PI/4);

                tabquIn[a][b][i] = tabquIn[a][b][i] + Pe * tabquOut[a][b-1][i];
           }

   if ( a2 > tabRg [a+1][b-1] && tabRg [a+1][b-1]>=a1  && tabTetaHw[a+1][b-1][i] == tetaHwMax[a][b])//------------------------------
           {
                alpha2 = tabRg [a+1][b-1];
                alpha1 = PI/4 - alpha2;
                Pne = alpha2 /(PI/4);

                tabquIn[a][b][i] = tabquIn[a][b][i] + Pne * tabquOut[a+1][b-1][i];
           }


   if ( a3 >= tabRg [a+1][b-1] && tabRg [a+1][b-1]>=a2  && tabTetaHw[a+1][b-1][i] == tetaHwMax[a][b])//------------------------------
           {
                alpha2 = tabRg [a+1][b-1] - (PI/4);
                alpha1 = PI/4 - alpha2;
                Pne = alpha1 /(PI/4);

                tabquIn[a][b][i] = tabquIn[a][b][i] + Pne * tabquOut[a+1][b-1][i];
           }


 if ( a3 > tabRg [a+1][b] && tabRg [a+1][b]>=a2  && tabTetaHw[a+1][b][i] == tetaHwMax[a][b])//------------------------------
           {
                alpha2 = tabRg [a+1][b] - (PI/4);
                alpha1 = PI/4 - alpha2;
                Pn = alpha2 /(PI/4);

                tabquIn[a][b][i] = tabquIn[a][b][i] + Pn * tabquOut[a+1][b][i];
           }


   if ( a4 >= tabRg [a+1][b] && tabRg [a+1][b]>=a3  && tabTetaHw[a+1][b][i] == tetaHwMax[a][b])//------------------------------
           {
                alpha2 = tabRg [a+1][b] - (PI/2);
                alpha1 = PI/4 - alpha2;
                Pn = alpha1 /(PI/4);

                tabquIn[a][b][i] = tabquIn[a][b][i] + Pn * tabquOut[a+1][b][i];
           }


   if ( a5 >= tabRg [a+1][b+1] && tabRg [a+1][b+1]>=a4  && tabTetaHw[a+1][b+1][i] == tetaHwMax[a][b])//------------------------------
           {
                alpha2 = tabRg [a+1][b+1] - (3*PI/4);
                alpha1 = PI/4 - alpha2;
                Pnw = alpha1 /(PI/4);

                tabquIn[a][b][i] = tabquIn[a][b][i] + Pnw * tabquOut[a+1][b+1][i];
           }


   if ( a4 > tabRg [a+1][b+1] && tabRg [a+1][b+1]>=a3  && tabTetaHw[a+1][b+1][i] == tetaHwMax[a][b])//------------------------------
           {
                alpha2 = tabRg [a+1][b+1] - (PI/2);
                alpha1 = PI/4 - alpha2;
                Pnw = alpha2 /(PI/4);

                tabquIn[a][b][i] = tabquIn[a][b][i] + Pnw * tabquOut[a+1][b+1][i];
           }

}



    } // fin du ballayage du tenseur













}























