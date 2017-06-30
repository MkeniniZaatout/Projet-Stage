#include <iostream>
#include <cstdlib>
#include <fstream>
#include <math.h>

using namespace std;

void quBrooks (float*** tabquZ, float*** tabTeta, float** tetaMax, float** tetaMaxSub, float** tetaMin,
                float** tetaMinSub,float** n, float** nSub, float** kSat, float** kSatSub, int ligne, int colonne, int subst)
{
for (int a = 0; a < ligne; a++)
for (int b = 0; b < colonne; b++)
    {
        for (int i = 0; i < subst; i++)   tabquZ[a][b][i] = kSat[a][b] * pow((tabTeta[a][b][i] - tetaMin[a][b])/(tetaMax[a][b] - tetaMin[a][b]), n[a][b]);                  // colonne
                                          tabquZ[a][b][subst] = kSatSub[a][b] * pow((tabTeta[a][b][subst] - tetaMinSub[a][b])/(tetaMaxSub[a][b] - tetaMinSub[a][b]), nSub[a][b]);  //substratum
    }

}


void tetaHw (float*** tabTetaHw, float*** tabquZ, float*** tabquIn, float*** tabquOut, float** tabPluie, float** tetaHwMax, float** pluieMax,
                float** kSat, int subst, int i, float** rTab, int t, int nbreMin, int a, int b)
{
 float quPluie = 0;

// substratum // ne prend pas en compte de quInt quOut

    if ( i == subst )
        {
            tabTetaHw[a][b][i] = tabTetaHw[a][b][i] + tabquZ[a][b][i-1] - tabquZ[a][b][i];

            if (tabTetaHw[a][b][i] > tetaHwMax[a][b])
                {
                    tabquZ[a][b][i-1] = tabquZ[a][b][i-1] - (tabTetaHw[a][b][i] - tetaHwMax[a][b]);
                    tabTetaHw[a][b][i] = tetaHwMax[a][b];
                }
        }


// Premiere case ////

    else if (i == 0)
        {
            if(t < nbreMin)
                {
                    if (tabPluie[a][b]*rTab[a][b] > pluieMax[a][b]) quPluie = pluieMax[a][b];
                    else quPluie = tabPluie[a][b]*rTab[a][b];

                    tabTetaHw[a][b][i] = tabTetaHw[a][b][i] + quPluie - tabquZ[a][b][i] - tabquOut[a][b][i] + tabquIn[a][b][i];
                }

            else    tabTetaHw[a][b][i] = tabTetaHw[a][b][i] - tabquZ[a][b][i] - tabquOut[a][b][i] + tabquIn[a][b][i];


            if (tabTetaHw[a][b][i] > tetaHwMax[a][b])      tabTetaHw[a][b][i] = tetaHwMax[a][b];
        }


//  reste de la colonne
    else
        {
            tabTetaHw[a][b][i] = tabTetaHw[a][b][i] + tabquZ[a][b][i-1] - tabquZ[a][b][i] - tabquOut[a][b][i] + tabquIn[a][b][i];

            if (tabTetaHw[a][b][i] > tetaHwMax[a][b])
                {
                    tabquZ[a][b][i-1] = tabquZ[a][b][i-1] - (tabTetaHw[a][b][i] - tetaHwMax[a][b]);
                    tabTetaHw[a][b][i] = tetaHwMax[a][b];
                }
        }




}



float teta (float*** tabTetaHw, float hauteurCase, int i, int a, int b)
{
    float teta = 0;
    teta = tabTetaHw[a][b][i] / hauteurCase ;

    return teta;
}



float hwTest (float*** tabTetaHw, float** tetaHwMax, float** tetaHwMin, float hauteurCase, int nbrCase, int a, int b)
{
    float hw = 0;

    for (int i = nbrCase-1; i > -1; i--)
        {
            if (i == nbrCase-1 && tabTetaHw[a][b][i] == tetaHwMax[a][b])
                hw = hw + hauteurCase;

            else if (tabTetaHw[a][b][i] == tetaHwMax[a][b] && tabTetaHw[a][b][i+1] == tetaHwMax[a][b])
                hw = hw + hauteurCase;

            else if (tabTetaHw[a][b][i] < tetaHwMax[a][b] && tabTetaHw[a][b][i+1] == tetaHwMax[a][b])
                {
                    hw = hw + (tabTetaHw[a][b][i] - tetaHwMin[a][b]) / (tetaHwMax[a][b] - tetaHwMin[a][b]) * hauteurCase;
                    i = -1;
                }



        }

    return hw;
}












