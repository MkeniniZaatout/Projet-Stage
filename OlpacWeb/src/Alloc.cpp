#include <cstdlib>
#include <iostream>
#include <math.h>
#include <fstream>


// tableau = new int[20];
// delete[] tableau;



float** tab2DAlloc(int sizeI,int sizeJ)
{
    int i,j;
    float **tab;

    tab = new float* [sizeI];

    for(i=0;i<sizeI;i++)
    {
        tab[i] = new float[sizeJ];
    }

    for (i=0; i<sizeI ; i++)
  {
    for (j=0; j<sizeJ ; j++)
  {
         tab[i][j] = 0;
}}

    return tab;
}

void tab2DDesAlloc(float **tab,int sizeI)
{
    int i;

    for(i=0;i<sizeI;i++)
        delete[] tab[i];

    delete[] tab;
}

float*** tab3DAlloc(int sizeX, int sizeY, int sizeZ)
{
    int i,j,k;
    float ***tab;

    tab = new float** [sizeX];

    for(i=0;i<sizeX;i++)
        tab[i] = new float* [sizeY];

    for (i=0; i<sizeX ; i++)
        for (j=0; j<sizeY ; j++)
            tab[i][j] = new float [sizeZ];

    for (i=0; i<sizeX ; i++)
        for (j=0; j<sizeY ; j++)
            for (k=0; k<sizeZ ; k++)
                tab[i][j][k] = 0;

    return tab;
}

void tab3DDesAlloc(float ***tab, int sizeX, int sizeY)
{
    int i,j;

    for(i = 0; i < sizeX; i++)
        for (j = 0; j < sizeY; j++)
            delete[] tab[i][j];

    for(i=0;i<sizeX;i++)
        delete[] tab[i];

    delete[] tab;
}

//Pour utiliser les fonctions
//int** tab = NULL;
//int hauteur = 3, largeur = 2;
//tab = tab2DAlloc(hauteur, largeur); // Allocation d'un tableau de int de 3 sur 2
//tab2DDesAlloc(tab, hauteur); //Désallocation
