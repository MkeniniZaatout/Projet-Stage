#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QRgb>
#include <QImage>
#include <QSize>
#include <Qpoint>
#include <QApplication>
#include <QProcess>
#include <QThread>

#include <QList>
#include <QVector>
#include <QVector2D>
#include <QDir>
#include <QApplication>
#include <QThread>

// Calcul AtempTime

void AtempTime (float tpsCalcul, int ligne, int colonne);

// Allocation

float** tab2DAlloc(int sizeI,int sizeJ);

void tab2DDesAlloc(float **tab,int sizeI);

float*** tab3DAlloc(int sizeX, int sizeY, int sizeZ);

void tab3DDesAlloc(float ***tab, int sizeX, int sizeY);


//Passage d'un fichier à un tab
float** getData(QString* parametres,float* values,QString nom,int nbParam);

void getEntete(QString* parametres,float* values, QString nom,int nbParam);

float** extraireTab(int xMin,int xMax,int yMin, int yMax,float** tab);

int getCoordX(float xllcorner,float x,float cellSize);

int getCoordY(float yllcorner,float y,float cellsize,float nbLigne);

float getClass(float val,float** classes,float* classDim);

float** traitementClass(float** classes,float** data,int ligne,int colonne,float* classDim);

void fileToTab1D (float *tab, char *nom);

void fileToTab2D (float **tab, char *nom, int size_X, int size_Y);

void fileMatrixToTab2D (float **tab, QString nom, int size_X, int size_Y);

void tab2DToFileMatrix (float **tab, QString nom, int ligne, int colonne);

void tab2DToFileMatrix (float **tab, QString nom,QString nom2, int ligne, int colonne);

void tab2DToFileMatrix(float **tab, char *nom,char* nom2, int ligne, int colonne);


//Traitement d'une image

void tab2DToImage(float** tab,char* nom,int ligne,int colonne);

float** getCoordImg(float coordX,float coordY,float dalleX,float dalleY,float newCoordX,float newCoordY,int bloc,int dalle);

float* conversion(int dizaine,int chiffre,int bloc,int x,int y);

void assemblerImg(float** images,char* prefixe,int taille);

int getSize(float coordX,float coordY,float dalleX,float dalleY,float newCoordX,float newCoordY);

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


