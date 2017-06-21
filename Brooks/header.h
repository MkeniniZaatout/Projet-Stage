#include <stdio.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <fstream>
#define PI 3.1415926535897932384626433832795
#define DEGRE 3.141592654/180

using namespace std;

// --- Calcul AtempTime
//
void AtempTimeFS(float** matFS, float** matAtempFS, float** matTimeFS, int t, int ligne, int colonne);
void AtempTimeHW(float** matHW, float** matAtempHW, float** matTimeHW, int t, int ligne, int colonne);

int** AllocInt2D(int nbLignes, int nbColonnes);
void DesallocInt2D(int** tab, int nbLignes);

// Allocation

float** tab2DAlloc(int sizeI,int sizeJ);

void tab2DDesAlloc(float **tab,int sizeI);

float*** tab3DAlloc(int sizeX, int sizeY, int sizeZ);

void tab3DDesAlloc(float ***tab, int sizeX, int sizeY);


//Passage d'un fichier à un tab
void fileToTab1D (float **tab, int nbLignes, string nom);

void fileToTab2D (float **tab, string nom, int size_X, int size_Y);

void fileMatrixToTab2D (float **tab, string nom, int size_X, int size_Y);

void tab2DToFileMatrix (float **tab,  string nom, int ligne, int colonne);

// Calcul hydro
void quBrooks (float*** tabquZ, float*** tabTeta, float** tetaMax, float** tetaMaxSub, float** tetaMin,
                float** tetaMinSub,float** n, float** nSub, float** kSat, float** kSatSub, int ligne, int colonne, int subst);

void tetaHw (float*** tabTetaHw, float*** tabquZ, float*** tabquIn, float*** tabquOut, float** tabPluie, float** tetaHwMax, float** pluieMax,
                float** kSat, int subst, int i, float** rTab, int t, int nbreSeconde, int a, int b);

float teta (float*** tabTetaHw, float hauteurCase, int i, int a, int b);

float hwTest (float*** tabTetaHw, float** tetaHwMax, float** tetaHwMin, float hauteurCase, int nbrCase, int a, int b);

// quIn et quOut

void quOut (float*** tabquOut, float***tabTetaHw, float** tabRg, float** tabPente, float** kSat, float** tetaHwMax, int ligne, int colonne,int subst);

void quIn(float*** tabquIn, float*** tabquOut, float***tabTetaHw, float** tabRg, float** tabPente, float** kSat, float** tetaHwMax, int ligne, int colonne,int subst);


// Traitement MNT


void CalculDirPente(float **tabMnt,float ** tabRg,float ** tabPente,int ligne,int colonne,float cPixel);

void mntsanspuit(float **tabMnt, float ** tabMntSansPuit, int ligne,int colonne, float cPixel,
                     float resolutionVert);

float tabR1(float alt0,float alt1,float alt2, float cpixel);
float tabR2(float alt0,float alt2,float alt3, float cpixel);

float tabS1(float alt0,float alt1,float alt2,float tableauR1,
                    float cPixel);
float tabS2(float alt0,float alt2,float alt3,float tableauR2,
                    float cPixel);
float tabS3(float alt0,float alt3,float alt4,float tableauR3,
                    float cPixel);
float tabS4(float alt0,float alt4,float alt5,float tableauR4,
                    float cPixel);
float tabS5(float alt0,float alt5,float alt6,float tableauR5,
                    float cPixel);
float tabS6(float alt0,float alt6,float alt7,float tableauR6,
                    float cPixel);
float tabS7(float alt0,float alt7,float alt8,float tableauR7,
                    float cPixel);
float tabS8(float alt0,float alt8,float alt1,float tableauR8,
                    float cPixel);


float valeurMaxS
       (float tableauS1,float tableauS2,float tableauS3,float tableauS4,
       float tableauS5,float tableauS6,float tableauS7,float tableauS8);

float valeurMin(float alt1,float alt2,float alt3, float alt4,float alt5,
                        float alt6,float alt7, float alt8);

//Calcul des direction d'écoulement pour l'algo D8
void D8 (float **tabMnt, float **dirD8,float **dAlti,int *pointCompteurPuits,
        int *pointCompteurPlat, int *pointCompteurPuitsPlat,
        int ligne,int colonne );

float DirFctDalt  (float **dirD8, float **dAlti, float val1,float val2,
                    float val3, float val4, float val5,float val6,
                    float val7, float val8, int i, int j);

void NoPlaneMartz (float **tabMnt, float **dirD8, float **dAlti,
                    int *pointCompteurPuits, int *pointCompteurPlat,
                    int *pointCompteurPuitsPlat, int ligne, int colonne,
                    float resolutionVert);

void CorrectPuits (float **tabMnt, float **dirD8,  int ligne,int colonne );

void testPuitPlat (float **tabMnt, float **dirD8, int *pointCompteurPuits,
        int *pointCompteurPlat, int *pointCompteurPuitsPlat,
        int ligne,int colonne );

void DownFlow (float **tabMnt, float **dirD8, float **dAlti,
               float **tabDownFlow, float *pointExutoire,
               int ligne, int colonne);


void UpFlow (float **tabMnt, float **dirD8, float **dAlti,
               float **tabUpFlow, int ligne, int colonne);

