#include "generateur.h"


Generateur::Generateur() : matriceHw(5,std::vector<float>(5)), matriceFs(5,std::vector<float>(5))
{
}

void Generateur::calculerHwFinal(getLigne(), getColonne(), std::vector<std::vector<float> > matriceMnt)
{   // colonne -> 250
    // ligne -> 169
    // nbrCase -> 15

    float*** tabTetaHw = tab3DAlloc(169,250,15); // Normalement getLigne() getColonne(), getNbrCase() , O --- Interne
    float** tetaHwMax = tab2DAlloc(169, 250);
    float** tetaHwMin = tab2DAlloc(169, 250);


    // --- Calcul du nouveau HW

    for (int a = 0; a < getLigne(); a++)
     {
        for (int b = 0; b < getColonne(); b++)
        {
            if(matriceMnt[a][b]>1)
            {
                tetaHw (tabTetaHw, tabquZ,tabquIn, tabquOut, tabPluie, tetaHwMaxSub, pluieMax, kSat, subst, subst, rTab, t, nbreMinPluie, a, b);
                for (int i = nbrCase-1; i > -1; i--)
                {
                    tetaHw (tabTetaHw, tabquZ,tabquIn, tabquOut, tabPluie, tetaHwMax, pluieMax, kSat, subst, i, rTab, t, nbreMinPluie, a, b);
                }
            }
        }
    }

    // Calcul du Hw et Fs
    for(int a = 0; a < getLigne(); a++){
            for(int b = 0; b< getColonne(); b++){
                if(matriceMnt[a][b] !=-9999){
                    matriceHw[a][b] = float calculeHw(tabTetaHw, tetaHwMax, tetaHwMin, hauteurCase, nbrCase, a, b);
                    matriceFs[a][b] = float calculeFs(a , b);
                }
            }
    }
}

float Generateur::calculeHw(float*** tabTetaHw, float** tetaHwMax, float** tetaHwMin, float hauteurCase, int nbrCase, int a, int b)
{
    float  hw = NULL;
    for (int i = nbrCase-1; i > -1; i--)
        {
            if (i == nbrCase-1 && tabTetaHw[a][b][i] == tetaHwMax[a][b])  hw = hw + hauteurCase;

            else if (tabTetaHw[a][b][i] == tetaHwMax[a][b] && tabTetaHw[a][b][i+1] == tetaHwMax[a][b])  hw = hw + hauteurCase;

            else if (tabTetaHw[a][b][i] < tetaHwMax[a][b] && tabTetaHw[a][b][i+1] == tetaHwMax[a][b])
                {
                    hw = hw + (tabTetaHw[a][b][i] - tetaHwMin[a][b]) / (tetaHwMax[a][b] - tetaHwMin[a][b]) * hauteurCase;
                    i = -1;
                }
        }
    return hw;
}

int Generateur::getNbrCase() const
{
    return nbrCase;
}

float Generateur::calculeFs(int a , int b)
{
    float fs = NULL;
    double** tabC = tab2DAlloc(getLigne(), getColonne());
    double** tabGamaSec = tab2DAlloc(getLigne(), getColonne());
    double** tabGamaHum = tab2DAlloc(getLigne(), getColonne());
    double** tabHw = tab2DAlloc(getLigne(), getColonne());
    double** tabPente = tab2DAlloc(getLigne(), getColonne());
    double** tabPhi = tab2DAlloc(getLigne(), getColonne());

    // Gros calcul du fs
    fs = (  tabC[a][b] + ((   (tabGamaSec[a][b]*H) - (tabGamaSec[a][b] + 10 - tabGamaHum[a][b])*(tabHw[a][b]/100))*((cos((2*tabPente[a][b]))+1)/2)*tan(tabPhi[a][b]*DEGRE)))
                        /
                        (((tabGamaSec[a][b]*(H-tabHw[a][b]/100))+ (tabGamaHum[a][b]*(tabHw[a][b]/100)))  * ( sin(tabPente[a][b]) + k ) * cos(tabPente[a][b]));
    return fs;
}

