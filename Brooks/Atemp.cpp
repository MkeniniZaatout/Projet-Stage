#include <iostream>
#include <cstdlib>
#include <fstream>
#include "header.h"

using namespace std;

void AtempTimeFS(float** matFS, float** matAtempFS, float** matTimeFS, int t, int ligne, int colonne)
{
    for (int i=0; i<ligne; i++)
    {
        for (int j=0; j<colonne; j++)
        {
            if(matFS[i][j] < matAtempFS[i][j])
            {
                matAtempFS[i][j] = matFS[i][j];
                matTimeFS[i][j] = t;
            }
        }
    }
}

void AtempTimeHW(float** matHW, float** matAtempHW, float** matTimeHW, int t, int ligne, int colonne)
{
    for (int i=0; i<ligne; i++)
    {
        for (int j=0; j<colonne; j++)
        {
            if(matHW[i][j] > matAtempHW[i][j])
            {
                matAtempHW[i][j] = matHW[i][j];
                matTimeHW[i][j] = t;
            }
        }
    }
}
