#include <cstdlib>
#include <iostream>
#include <math.h>
#include <fstream>
#include "header.h"
#define NB_CHAR				200
#define PI 3.1415926535897932384626433832795
using namespace std;




// CORRECTION A L AIDE DE LA METHODE D8
// CALCUL D'UN MNT SANS PUIT NI PLAT---------------------------------------------
//Dans le cas présent cette technique utilise l'algorthime D8 pour calculer les directions d'écoulement
// et par la suite corriger les puits et les plats
void mntsanspuit(float **tabMnt,float **tabMntSansPuit, int ligne,int colonne, float cPixel,
                 float resolutionVert)
{

int compteurPuits = 0, compteurPlat = 0, compteurPuitsPlat = 0;
int *pointCompteurPuits = &compteurPuits;
int *pointCompteurPlat = &compteurPlat;
int *pointCompteurPuitsPlat = &compteurPuitsPlat;


// 1
//Premiere étape: calcul des directions d'écoulements avec la méthode D8

// Ouverture du tab des directions de D8
float** dirD8 = NULL;

dirD8 = new float*[ligne];

for(int i=0; i<ligne; i++)
{
    dirD8[i] = new float[colonne];
}

for(int i=0; i<ligne; i++)
{
    for(int j=0; j<colonne; j++)
{
    dirD8[i][j] = 0;
}
}


float** dAlti = tab2DAlloc(ligne, colonne);


// 1
// Calcul la direction de l ecoulement et localisation des puits et plats.
// Calcul les differences altimetriques
 D8 ( tabMnt, dirD8, dAlti, pointCompteurPuits, pointCompteurPlat,
        pointCompteurPuitsPlat, ligne, colonne); //donne les directions D8 en degre non corrigé :
    // Modifie dirD8 ET
    // -1 indique un puits
    // -2 indique un plat
    // -3 indique un puitsplat


//Ecriture des problemes rencontrer lors du traitement hydro
printf("\nTraitement avec l'algo D8........................................\n");
printf("    Nombre de puits =       %d \n", *pointCompteurPuits);
printf("    Nombre de plats =       %d \n", *pointCompteurPlat);
printf("    Nombre de puits-plats = %d \n", *pointCompteurPuitsPlat);



// 2
//Correction des puits par remplissage
CorrectPuits (tabMnt, dirD8, ligne, colonne);



//on refait tourner D8 pour prendre en compte les variations apporter au MNT
// (marque les valeur -2 ou -3 pour les nouveau plats et puits plats creer

D8 ( tabMnt, dirD8, dAlti, pointCompteurPuits, pointCompteurPlat,
        pointCompteurPuitsPlat, ligne, colonne);



//Test pour savoir si les puits sont bien comblés

testPuitPlat (tabMnt, dirD8, pointCompteurPuits,pointCompteurPlat,
                pointCompteurPuitsPlat, ligne, colonne );

if (*pointCompteurPuits != 0)
printf("Probleme dans la correction des puits\n\n");


// 3
// Correction du MNT et du dirD8 à l'aide de la méthode de Martz-Garbrecht

printf("\nCorrection des plats et des puits-plat a l'aide");
printf(" de la methode de Martz-Garbrecht");


NoPlaneMartz (tabMnt, dirD8, dAlti, pointCompteurPuits,pointCompteurPlat,
                pointCompteurPuitsPlat, ligne, colonne, resolutionVert);

// Creation du fichier Mntsanspuit---------------------------------------------------
for (int a = 0; a < ligne; a++)
for (int b = 0; b < colonne; b++)
    tabMntSansPuit[a][b] = tabMnt[a][b];

//tab2DToFileMatrix (tabMntSansPuit, "Geol/mntsanspuit.asc", ligne, colonne);


tab2DDesAlloc(dirD8, ligne); //Désallocation
tab2DDesAlloc(dAlti, ligne); //Désallocation
}




//Calcul de la direction d'écoulement avec la technique D8 (sans traitement des puits)
void D8 (float **tabMnt,float **dirD8, float **dAlti,int *pointCompteurPuits,
        int *pointCompteurPlat, int *pointCompteurPuitsPlat,
        int ligne,int colonne )
{

float alt0=0,alt1=0,alt2=0,alt3=0,alt4=0,alt5=0,alt6=0,alt7=0,alt8=0;
float dAlt01=0,dAlt02=0,dAlt03=0,dAlt04=0,dAlt05=0,dAlt06=0,dAlt07=0,dAlt08=0;

// Mise à zéro des compteurs
*pointCompteurPuits     = 0;
*pointCompteurPlat      = 0;
*pointCompteurPuitsPlat = 0;


// 1
// Calcul dans la matrice///////////////////////////////////////////////////////

    for (int i=1; i<ligne-1;i++)
        {
      for (int j=1; j <colonne-1;j++ )
            {

alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt2 = tabMnt[i-1][j+1];
alt3 = tabMnt[i-1][j];
alt4 = tabMnt[i-1][j-1];
alt5 = tabMnt[i][j-1];
alt6 = tabMnt[i+1][j-1];
alt7 = tabMnt[i+1][j];
alt8 = tabMnt[i+1][j+1];


// Calcul des descentes altimétriques (pour simplifier les calcul dAlt01 = (alt0 - alt1)/r, on suppose que r est égale à 1
dAlt01 = (alt0 - alt1);
dAlt02 = (alt0 - alt2)/sqrt(2);
dAlt03 = (alt0 - alt3);
dAlt04 = (alt0 - alt4)/sqrt(2);
dAlt05 = (alt0 - alt5);
dAlt06 = (alt0 - alt6)/sqrt(2);
dAlt07 = (alt0 - alt7);
dAlt08 = (alt0 - alt8)/sqrt(2);



// Comparaison des descentes altimétriques
if (dAlt01 == 0 && dAlt02 == 0 && dAlt03 == 0 && dAlt04 == 0 && dAlt05 == 0 &&
    dAlt06 == 0 && dAlt07 == 0 && dAlt08 == 0)
        {
            dirD8 [i][j] = -2;// C'est une zone plane (pas de varaition d'altitude)
            dAlti [i][j] = -2;
            *pointCompteurPlat = *pointCompteurPlat + 1;
        }




else if (dAlt01 > 0 || dAlt02 > 0 || dAlt03 > 0 || dAlt04 > 0 || dAlt05 > 0
            || dAlt06 > 0 || dAlt07 > 0 || dAlt08 > 0)
        {

            DirFctDalt(dirD8, dAlti, dAlt01,dAlt02,dAlt03,dAlt04, dAlt05,
                        dAlt06, dAlt07, dAlt08, i, j);

            //Renvoi dirD8 (0, 45, 90,...)
            //Renvoi (dAlti) la difference alti pour l'orientation choisi
            //Si égalité entre pixel choisi au hasard la direction d'écoulement ou non (cas de trois pixel collé)...
        }



else if (dAlt01 < 0 && dAlt02 < 0 && dAlt03 < 0 && dAlt04 < 0 && dAlt05 < 0 &&
            dAlt06 < 0 && dAlt07 < 0 && dAlt08 < 0)
        {
            dirD8 [i][j] = -1;// C'est un puits
            dAlti [i][j] = -1;
            *pointCompteurPuits = *pointCompteurPuits + 1;

        }

else    {
            dirD8 [i][j] = -3;  // C'est une zone de puits - plats
            dAlti [i][j] = -3;
            *pointCompteurPuitsPlat = *pointCompteurPuitsPlat + 1;
        }


}} //fin de la boucle Calcul dans la matrice



// 2
// Calcul pour les bordures/////////////////////////////////////////////////////
{
//Bordure Nord

int i = 0;
for (int j=1; j <colonne-1;j++ )
    {
alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt5 = tabMnt[i][j-1];
alt6 = tabMnt[i+1][j-1];
alt7 = tabMnt[i+1][j];
alt8 = tabMnt[i+1][j+1];


// Calcul des descentes altimétriques (pour simplifier les calcul dAlt01 = (alt0 - alt1)/r, on suppose que r est égale à 1
dAlt01 = (alt0 - alt1);
dAlt05 = (alt0 - alt5);
dAlt06 = (alt0 - alt6)/sqrt(2);
dAlt07 = (alt0 - alt7);
dAlt08 = (alt0 - alt8)/sqrt(2);

// Comparaison des descentes altimétriques
if (dAlt01 == 0 && dAlt05 == 0 && dAlt06 == 0 && dAlt07 == 0 && dAlt08 == 0)
        {
            dirD8 [i][j] = -2;// C'est une zone plane (pas de varaition d'altitude)
            dAlti [i][j] = -2;
            *pointCompteurPlat = *pointCompteurPlat + 1;
        }


else if (dAlt01 > 0 || dAlt05 > 0 || dAlt06 > 0 || dAlt07 > 0 || dAlt08 > 0)
        {
            DirFctDalt(dirD8, dAlti, dAlt01, -1, -1, -1, dAlt05, dAlt06,
                                            dAlt07, dAlt08, i, j);                    //Renvoi une valeur d'angle en degré (0, 45, 90,...)
        }


else if (dAlt01 < 0 && dAlt05 < 0 && dAlt06 < 0 && dAlt07 < 0 && dAlt08 < 0)
        {
            dirD8 [i][j] = -1;// C'est un puits
            dAlti [i][j] = -1;
            *pointCompteurPuits = *pointCompteurPuits + 1;
        }

else    {
            dirD8 [i][j] = -3;  // C'est une zone de puits - plats
            dAlti [i][j] = -3;
            *pointCompteurPuitsPlat = *pointCompteurPuitsPlat + 1;
        }




    }

//Bordure Sud

i = ligne - 1;
for (int j=1; j <colonne-1;j++ )
    {

alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt2 = tabMnt[i-1][j+1];
alt3 = tabMnt[i-1][j];
alt4 = tabMnt[i-1][j-1];
alt5 = tabMnt[i][j-1];

// Calcul des descentes altimétriques (pour simplifier les calcul dAlt01 = (alt0 - alt1)/r, on suppose que r est égale à 1
dAlt01 = (alt0 - alt1);
dAlt02 = (alt0 - alt2)/sqrt(2);
dAlt03 = (alt0 - alt3);
dAlt04 = (alt0 - alt4)/sqrt(2);
dAlt05 = (alt0 - alt5);

// Comparaison des descentes altimétriques
if (dAlt01 == 0 && dAlt02 == 0 && dAlt03 == 0 && dAlt04 == 0 && dAlt05 == 0)
        {
            dirD8 [i][j] = -2;// C'est une zone plane (pas de varaition d'altitude)
            dAlti [i][j] = -2;
            *pointCompteurPlat = *pointCompteurPlat + 1;
        }


else if (dAlt01 > 0 || dAlt02 > 0 || dAlt03 > 0 || dAlt04 > 0 || dAlt05 > 0)
        {
            DirFctDalt(dirD8, dAlti, dAlt01, dAlt02, dAlt03, dAlt04, dAlt05,
                                -1, -1, -1, i, j);                    //Renvoi une valeur d'angle en degré (0, 45, 90,...)
        }


else if (dAlt01 < 0 && dAlt02 < 0 && dAlt03 < 0 && dAlt04 < 0 && dAlt05 < 0)
        {
            dirD8 [i][j] = -1;// C'est un puits
            dAlti [i][j] = -1;
            *pointCompteurPuits = *pointCompteurPuits + 1;
        }

else    {
            dirD8 [i][j] = -3;  // C'est une zone de puits - plats
            dAlti [i][j] = -3;
            *pointCompteurPuitsPlat = *pointCompteurPuitsPlat + 1;
        }

}

//Bordure Est

int j = colonne -1;
for (int i=1; i<ligne-1;i++)
    {
alt0 = tabMnt [i][j];
alt3 = tabMnt[i-1][j];
alt4 = tabMnt[i-1][j-1];
alt5 = tabMnt[i][j-1];
alt6 = tabMnt[i+1][j-1];
alt7 = tabMnt[i+1][j];

// Calcul des descentes altimétriques (pour simplifier les calcul dAlt01 = (alt0 - alt1)/r, on suppose que r est égale à 1
dAlt03 = (alt0 - alt3);
dAlt04 = (alt0 - alt4)/sqrt(2);
dAlt05 = (alt0 - alt5);
dAlt06 = (alt0 - alt6)/sqrt(2);
dAlt07 = (alt0 - alt7);

// Comparaison des descentes altimétriques
if (dAlt03 == 0 && dAlt04 == 0 && dAlt05 == 0 && dAlt06 == 0 && dAlt07 == 0)
        {
            dirD8 [i][j] = -2;// C'est une zone plane (pas de varaition d'altitude)
            dAlti [i][j] = -2;
            *pointCompteurPlat = *pointCompteurPlat + 1;
        }


else if (dAlt03 > 0 || dAlt04 > 0 || dAlt05 > 0 || dAlt06 > 0 || dAlt07 > 0 )
        {
            DirFctDalt(dirD8, dAlti, -1,-1,dAlt03,dAlt04,dAlt05,
                                dAlt06, dAlt07, -1, i, j);                    //Renvoi une valeur d'angle en degré (0, 45, 90,...)
        }


else if (dAlt03 < 0 && dAlt04 < 0 && dAlt05 < 0 && dAlt06 < 0 && dAlt07 < 0)
        {
            dirD8 [i][j] = -1;// C'est un puits
            dAlti [i][j] = -1;
            *pointCompteurPuits = *pointCompteurPuits + 1;
        }

else    {
            dirD8 [i][j] = -3;  // C'est une zone de puits - plats
            dAlti [i][j] = -3;
            *pointCompteurPuitsPlat = *pointCompteurPuitsPlat + 1;
        }


}

//Bordure West

j = 0;
for (int i=1; i<ligne-1;i++)
{
alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt2 = tabMnt[i-1][j+1];
alt3 = tabMnt[i-1][j];
alt7 = tabMnt[i+1][j];
alt8 = tabMnt[i+1][j+1];


// Calcul des descentes altimétriques (pour simplifier les calcul dAlt01 = (alt0 - alt1)/r, on suppose que r est égale à 1
dAlt01 = (alt0 - alt1);
dAlt02 = (alt0 - alt2)/sqrt(2);
dAlt03 = (alt0 - alt3);
dAlt07 = (alt0 - alt7);
dAlt08 = (alt0 - alt8)/sqrt(2);

// Comparaison des descentes altimétriques
if (dAlt01 == 0 && dAlt02 == 0 && dAlt03 == 0 && dAlt07 == 0 && dAlt08 == 0)
        {
            dirD8 [i][j] = -2;// C'est une zone plane (pas de varaition d'altitude)
            dAlti [i][j] = -2;
            *pointCompteurPlat = *pointCompteurPlat + 1;
        }


else if (dAlt01 > 0 || dAlt02 > 0 || dAlt03 > 0 || dAlt07 > 0 || dAlt08 > 0)
        {
            DirFctDalt(dirD8, dAlti, dAlt01,dAlt02,dAlt03,-1,-1,
                                -1, dAlt07, dAlt08, i, j);                    //Renvoi une valeur d'angle en degré (0, 45, 90,...)
        }


else if (dAlt01 < 0 && dAlt02 < 0 && dAlt03 < 0 && dAlt07 < 0 && dAlt08 < 0)
        {
            dirD8 [i][j] = -1;// C'est un puits
            dAlti [i][j] = -1;
            *pointCompteurPuits = *pointCompteurPuits + 1;
        }

else    {
            dirD8 [i][j] = -3;  // C'est une zone de puits - plats
            dAlti [i][j] = -3;
            *pointCompteurPuitsPlat = *pointCompteurPuitsPlat + 1;
        }


}





//3
// Calcul pour les coins////////////////////////////////////////////////////////

// Coin NE
{
i = 0;
j = colonne -1;

alt0 = tabMnt [i][j];
alt5 = tabMnt[i][j-1];
alt6 = tabMnt[i+1][j-1];
alt7 = tabMnt[i+1][j];

// Calcul des descentes altimétriques (pour simplifier les calcul dAlt01 = (alt0 - alt1)/r, on suppose que r est égale à 1
dAlt05 = (alt0 - alt5);
dAlt06 = (alt0 - alt6)/sqrt(2);
dAlt07 = (alt0 - alt7);

// Comparaison des descentes altimétriques
if (dAlt05 == 0 && dAlt06 == 0 && dAlt07 == 0)
        {
            dirD8 [i][j] = -2;// C'est une zone plane (pas de varaition d'altitude)
            dAlti [i][j] = -2;
            *pointCompteurPlat = *pointCompteurPlat + 1;
        }


else if (dAlt05 > 0 || dAlt06 > 0 || dAlt07 > 0)
        {
            DirFctDalt(dirD8, dAlti, -1,-1,-1,-1,dAlt05,dAlt06, dAlt07, -1,
             i, j);                    //Renvoi une valeur d'angle en degré (0, 45, 90,...)
        }


else if (dAlt05 < 0 && dAlt06 < 0 && dAlt07 < 0)
        {
            dirD8 [i][j] = -1;// C'est un puits
            dAlti [i][j] = -1;
            *pointCompteurPuits = *pointCompteurPuits + 1;
        }

else    {
            dirD8 [i][j] = -3;  // C'est une zone de puits - plats
            dAlti [i][j] = -3;
            *pointCompteurPuitsPlat = *pointCompteurPuitsPlat + 1;
        }

}

// Coin NW
{
    i=0;
j=0;

alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt7 = tabMnt[i+1][j];
alt8 = tabMnt[i+1][j+1];


// Calcul des descentes altimétriques (pour simplifier les calcul dAlt01 = (alt0 - alt1)/r, on suppose que r est égale à 1
dAlt01 = (alt0 - alt1);
dAlt07 = (alt0 - alt7);
dAlt08 = (alt0 - alt8)/sqrt(2);

// Comparaison des descentes altimétriques
if (dAlt01 == 0 && dAlt07 == 0 && dAlt08 == 0)
        {
            dirD8 [i][j] = -2;// C'est une zone plane (pas de varaition d'altitude)
            dAlti [i][j] = -2;
            *pointCompteurPlat = *pointCompteurPlat + 1;
        }


else if (dAlt01 > 0 || dAlt07 > 0 || dAlt08 > 0)
        {
            DirFctDalt(dirD8, dAlti, dAlt01,-1,-1,-1,-1, -1, dAlt07, dAlt08,
                        i, j);                    //Renvoi une valeur d'angle en degré (0, 45, 90,...)
        }


else if (dAlt01 < 0 && dAlt07 < 0 && dAlt08 < 0)
        {
            dirD8 [i][j] = -1;// C'est un puits
            dAlti [i][j] = -1;
            *pointCompteurPuits = *pointCompteurPuits + 1;
        }

else    {
            dirD8 [i][j] = -3;  // C'est une zone de puits - plats
            dAlti [i][j] = -3;
            *pointCompteurPuitsPlat = *pointCompteurPuitsPlat + 1;
        }

}


// Coin SW
{
i = ligne - 1;
j = 0;

alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt2 = tabMnt[i-1][j+1];
alt3 = tabMnt[i-1][j];

// Calcul des descentes altimétriques (pour simplifier les calcul dAlt01 = (alt0 - alt1)/r, on suppose que r est égale à 1
dAlt01 = (alt0 - alt1);
dAlt02 = (alt0 - alt2)/sqrt(2);
dAlt03 = (alt0 - alt3);

// Comparaison des descentes altimétriques
if (dAlt01 == 0 && dAlt02 == 0 && dAlt03 == 0)
        {
            dirD8 [i][j] = -2;// C'est une zone plane (pas de varaition d'altitude)
            dAlti [i][j] = -2;
            *pointCompteurPlat = *pointCompteurPlat + 1;
        }


else if (dAlt01 > 0 || dAlt02 > 0 || dAlt03 > 0)
        {
            DirFctDalt(dirD8, dAlti, dAlt01,dAlt02,dAlt03,-1,-1,-1, -1, -1,
                            i, j);                    //Renvoi une valeur d'angle en degré (0, 45, 90,...)
        }


else if (dAlt01 < 0 && dAlt02 < 0 && dAlt03 < 0)
        {
            dirD8 [i][j] = -1;// C'est un puits
            dAlti [i][j] = -1;
            *pointCompteurPuits = *pointCompteurPuits + 1;
        }

else    {
            dirD8 [i][j] = -3;  // C'est une zone de puits - plats
            dAlti [i][j] = -3;
            *pointCompteurPuitsPlat = *pointCompteurPuitsPlat + 1;
        }
}


// Coin SE
{
i = ligne - 1;
j = colonne - 1;

alt0 = tabMnt [i][j];
alt3 = tabMnt[i-1][j];
alt4 = tabMnt[i-1][j-1];
alt5 = tabMnt[i][j-1];

// Calcul des descentes altimétriques (pour simplifier les calcul dAlt01 = (alt0 - alt1)/r, on suppose que r est égale à 1
dAlt03 = (alt0 - alt3);
dAlt04 = (alt0 - alt4)/sqrt(2);
dAlt05 = (alt0 - alt5);

// Comparaison des descentes altimétriques
if (dAlt03 == 0 && dAlt04 == 0 && dAlt05 == 0)
        {
            dirD8 [i][j] = -2;// C'est une zone plane (pas de varaition d'altitude)
            dAlti [i][j] = -2;
            *pointCompteurPlat = *pointCompteurPlat + 1;
        }


else if (dAlt03 > 0 || dAlt04 > 0 || dAlt05 > 0)
        {
            DirFctDalt(dirD8, dAlti, -1,-1,dAlt03,dAlt04,dAlt05,-1, -1, -1,
             i, j);                    //Renvoi une valeur d'angle en degré (0, 45, 90,...)
        }


else if (dAlt03 < 0 && dAlt04 < 0 && dAlt05 < 0)
        {
            dirD8 [i][j] = -1;// C'est un puits
            dAlti [i][j] = -1;
            *pointCompteurPuits = *pointCompteurPuits + 1;
        }

else    {
            dirD8 [i][j] = -3;  // C'est une zone de puits - plats
            dAlti [i][j] = -3;
            *pointCompteurPuitsPlat = *pointCompteurPuitsPlat + 1;
        }

}

}// fin de bordure








}



// Obtention de la valeur min-----------------------------------------------------------------
// La fonction valeur min renvoi la valeur minimale rencontré mais n'indique pas si il y des pixels adjacents de meme altitude
float valeurMin(float alt1,float alt2,float alt3, float alt4, float alt5, float alt6,float alt7, float alt8)
{


if (alt1<=alt2 && alt1<=alt3 && alt1<=alt4 && alt1<=alt5 && alt1<=alt6
 && alt1<=alt7 && alt1<=alt8)
    {return alt1;}

else if (alt2<=alt1 && alt2<=alt3 && alt2<=alt4 && alt2<=alt5 && alt2<=alt6
        && alt2<=alt7 && alt2<=alt8)
    {return alt2;}

else if (alt3<=alt2 && alt3<=alt1 && alt3<=alt4 && alt3<=alt5 && alt3<=alt6
        && alt3<=alt7 && alt3<=alt8)
    {return alt3;}

else if (alt4<=alt2 && alt4<=alt3 && alt4<=alt1 && alt4<=alt5 && alt4<=alt6
        && alt4<=alt7 && alt4<=alt8)
    {return alt4;}

else if (alt5<=alt2 && alt5<=alt3 && alt5<=alt4 && alt5<=alt1 && alt5<=alt6
        && alt5<=alt7 && alt5<=alt8)
    {return alt5;}

else if (alt6<=alt2 && alt6<=alt3 && alt6<=alt4 && alt6<=alt5 && alt6<=alt1
        && alt6<=alt7 && alt6<=alt8)
    {return alt6;}

else if (alt7<=alt2 && alt7<=alt3 && alt7<=alt4 &&  alt7<=alt5 && alt7<=alt6
        && alt7<=alt1 && alt7<=alt8)
    {return alt7;}

else if (alt8<=alt2 && alt8<=alt3 && alt8<=alt4 && alt8<=alt5 && alt8<=alt6
        && alt8<=alt7 && alt8<=alt1)
    {return alt8;}

else
    {
        printf ("problème dans la comparaison des valeurs min \n");
    }

}



void UpFlow (float **tabMnt, float **dirD8, float **dAlti,
               float **tabUpFlow, int ligne, int colonne)
{
//Variable modifiable en fonction du nombre de pixel coller entre
// Normalement la valeur 100 est largement suffisante
int nbreIteration = 100;

// Variable pour le calcul
float alt0=0,alt3=0,alt4=0,alt5=0;



// 1
// On marque tout les pixels pluitsplats accolés car ils sont accolés au moins un pixel plus haut
// Calcul pour l'ensemble de la carte
for (int i=0; i<ligne;i++)
  {
      for (int j=0; j <colonne;j++ )
        {
            if (dirD8[i][j] == -3)
            tabUpFlow[i][j] = 1;
        }
  }



// 2: DEUXIEME ETAPE
// ON CONNECTE TOUS LES PIXELS AU PIXEL QUI SONT EN CONTACT avec les pixels pluits plats

////////////////////////////////////////////////////////////////////////////////
for (int x=0; x < nbreIteration; x++) //debut de la boucle x iteration
                            // 100 est un chiffre pris au hasard qui doit etre suffisant pour coller tout les pixels
{

//Dans un premier temps on incrémente de 1 tout les pixels == à 1
for (int i=0; i<ligne;i++)
        {
      for (int j=0; j <colonne;j++ )
            {

                if(tabUpFlow[i][j] >= 1)

                tabUpFlow[i][j] ++;

            }
        }




// Calcul dans la matrice //
//------------------------//

for (int i=1; i<ligne-1;i++)
        {
      for (int j=1; j <colonne-1;j++ )
            {


if (dirD8[i][j] == -2 && tabUpFlow[i][j]==0)          //si pixel 0 est un plat
    {

//ALT1//////////////////////////////////////////////////////////////////////////

if (tabUpFlow[i][j+1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT2 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i-1][j+1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT3 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i-1][j] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT4 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i-1][j-1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT5 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i][j-1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT6 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i+1][j-1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT7 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i+1][j] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT8 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i+1][j+1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }

    }


}} //fin de la boucle Calcul dans la matrice





// Calcul sur les bordures //
//-------------------------//
{
//Bordure Nord //

int i = 0;
for (int j=1; j <colonne-1;j++ )
{


if (dirD8[i][j] == -2 && tabUpFlow[i][j]==0)          //si pixel 0 est un plat
    {

//ALT1//////////////////////////////////////////////////////////////////////////

if (tabUpFlow[i][j+1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }
//ALT5 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i][j-1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT6 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i+1][j-1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT7 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i+1][j] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT8 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i+1][j+1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }

    }


}



//Bordure Sud //

i = ligne - 1;
for (int j=1; j <colonne-1;j++ )
{

if (dirD8[i][j] == -2 && tabUpFlow[i][j]==0)          //si pixel 0 est un plat
    {

//ALT1//////////////////////////////////////////////////////////////////////////

if (tabUpFlow[i][j+1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT2 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i-1][j+1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT3 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i-1][j] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT4 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i-1][j-1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT5 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i][j-1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


    }


}


//Bordure Est //

int j = colonne -1;
for (int i=1; i<ligne-1;i++)
{


if (dirD8[i][j] == -2 && tabUpFlow[i][j]==0)          //si pixel 0 est un plat
    {

//ALT3 //////////////////////////////////////////////////////////////////////////

if (tabUpFlow[i-1][j] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT4 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i-1][j-1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT5 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i][j-1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT6 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i+1][j-1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT7 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i+1][j] == 2)
        {
            tabUpFlow[i][j] = 1;
        }

    }
}


//Bordure West //

j = 0;
for (int i=1; i<ligne-1;i++)
{

if (dirD8[i][j] == -2 && tabUpFlow[i][j]==0)          //si pixel 0 est un plat
    {
//ALT1//////////////////////////////////////////////////////////////////////////

if (tabUpFlow[i][j+1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT2 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i-1][j+1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT3 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i-1][j] == 2)
        {
            tabUpFlow[i][j] = 1;
        }

//ALT7 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i+1][j] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT8 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i+1][j+1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


    }
}



// Calcul pour les coins////////////////////////////////////////////////////////

// Coin NE //
{
i = 0;
j = colonne -1;

if (dirD8[i][j] == -2 && tabUpFlow[i][j]==0)          //si pixel 0 est un plat
    {

//ALT5 //////////////////////////////////////////////////////////////////////////

if (tabUpFlow[i][j-1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT6 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i+1][j-1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT7 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i+1][j] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


    }

}


// Coin NW //
{
i=0;
j=0;

if (dirD8[i][j] == -2 && tabUpFlow[i][j]==0)          //si pixel 0 est un plat
    {

//ALT1//////////////////////////////////////////////////////////////////////////

if (tabUpFlow[i][j+1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT7 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i+1][j] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT8 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i+1][j+1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


    }
}


// Coin SW //
{
i = ligne - 1;
j = 0;


if (dirD8[i][j] == -2 && tabUpFlow[i][j]==0)          //si pixel 0 est un plat
    {

//ALT1//////////////////////////////////////////////////////////////////////////

if (tabUpFlow[i][j+1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT2 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i-1][j+1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT3 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i-1][j] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


    }
}


// Coin SE //
{
i = ligne - 1;
j = colonne - 1;

alt0 = tabMnt [i][j];
alt3 = tabMnt[i-1][j];
alt4 = tabMnt[i-1][j-1];
alt5 = tabMnt[i][j-1];

if (dirD8[i][j] == -2 && tabUpFlow[i][j]==0)          //si pixel 0 est un plat
    {


//ALT3 //////////////////////////////////////////////////////////////////////////

if (tabUpFlow[i-1][j] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT4 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i-1][j-1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }


//ALT5 //////////////////////////////////////////////////////////////////////////

else if (tabUpFlow[i][j-1] == 2)
        {
            tabUpFlow[i][j] = 1;
        }



    }
}




}//Fin du calcul sur les bordures




}// fin de la boucle x




// 3 On traite tabFlowDown pour avoir le nombre correct de sommation
float valMinTemp = 200;
for (int i=0; i<ligne;i++)
        {
      for (int j=0; j <colonne;j++ )
            {
                if(tabUpFlow[i][j] !=0)
                if(tabUpFlow[i][j] <  valMinTemp)

                valMinTemp = tabUpFlow[i][j];

            }
        }

// 4
// On corrige tabFlowDown
for (int i=0; i<ligne;i++)
        {
      for (int j=0; j <colonne;j++ )
            {

                if (tabUpFlow[i][j] !=0)
                tabUpFlow[i][j] = tabUpFlow[i][j] - (valMinTemp - 1);

            }
        }









/*
// Creation du fichier Mntsanspuit---------------------------------------------------

FILE* upflow = NULL;
        upflow = fopen("upflow.txt", "w");

      for (int i=0; i<ligne;i++)
  {
      for (int j=0; j <colonne;j++ )
  {
         fprintf(upflow,"%d %d %.2lf\n",i,j, tabUpFlow[i][j]);

}
}
fclose(upflow);



cout<<"\n\nfichier ecrit\n\n"<<endl;
system("pause");
*/








}


void DownFlow (float **tabMnt, float **dirD8, float **dAlti,
               float **tabDownFlow, float *pointExutoire,
               int ligne, int colonne)
{

// Variable pour le calcul
float alt0=0,alt1=0,alt2=0,alt3=0,alt4=0,alt5=0,alt6=0,alt7=0,alt8=0;


//On creer un tableau pour connaitre les dAlt du pixel exutoire auquel sont associé les pixels plat ou platpuits
float** dAltAssocie = NULL;
dAltAssocie = tab2DAlloc(ligne, colonne);




// 1: PREMIERE ITERATION
// Premiere étape on cherche les pixels en contact avec les pixels exutoires


// Calcul dans la matrice
for (int i=1; i<ligne-1;i++)
        {
      for (int j=1; j <colonne-1;j++ )
            {

alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt2 = tabMnt[i-1][j+1];
alt3 = tabMnt[i-1][j];
alt4 = tabMnt[i-1][j-1];
alt5 = tabMnt[i][j-1];
alt6 = tabMnt[i+1][j-1];
alt7 = tabMnt[i+1][j];
alt8 = tabMnt[i+1][j+1];

if (dirD8[i][j] == -2 || dirD8[i][j] == -3) //si le pixel 0 est un plat ou un puitsplat
    {

//ALT1//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt1)                               // si il a meme altitude que le pixel 1
if (dirD8[i][j+1] != -2 && dirD8[i][j+1] != -3) // et que ce pixel voisin 1 possede un gradient vers le bas
        {
            tabDownFlow[i][j] = 1;                // alors le pixel central et en contact avec un pixel exutoire
            dAltAssocie[i][j] = dAlti[i][j+1];      // On associe la valeur dAlt du pixel exutoire au pixel plat pliutsplat
        }
}

//ALT2 //////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt2)
if (dirD8[i-1][j+1] != -2 && dirD8[i-1][j+1] != -3)
    {

// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
    if (tabDownFlow[i][j] == 1)
        {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i-1][j+1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i-1][j+1];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
        }

// B) le pixel 0 n'a pas encore un autre exutoire
    else
        {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j+1];
        }

    }
}

//ALT3//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt3)
if (dirD8[i-1][j] != -2 && dirD8[i-1][j]!= -3)
{
// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i-1][j] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i-1][j];     // On remplace l'ancienne dAlt par la nouvelle

            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j];
    }
}
    }

//ALT4//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt4)
if (dirD8[i-1][j-1] != -2 && dirD8[i-1][j-1]!= -3)
{

// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i-1][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i-1][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j-1];
    }
}
}

//ALT5//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt5)
if (dirD8[i][j-1] != -2 && dirD8[i][j-1]!= -3)
{
// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i][j-1];
    }
}
}

//ALT6//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt6)
if (dirD8[i+1][j-1] != -2 && dirD8[i+1][j-1]!= -3)
{
// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;
        dAltAssocie[i][j] = dAlti[i+1][j-1];                   // alors le pixel central et en contact avec un pixel exutoire
    }
}
}

//ALT7//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt7)
if (dirD8[i+1][j] != -2 && dirD8[i+1][j]!= -3)
{
// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j];
    }
}
}

//ALT8//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt8)
if (dirD8[i+1][j+1] != -2 && dirD8[i+1][j+1]!= -3)
{
// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j+1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j+1];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j+1];
    }
}
}

    }

}} //fin de la boucle Calcul dans la matrice






// Calcul sur les bordures
{
//Bordure Nord //

int i = 0;
for (int j=1; j <colonne-1;j++ )
    {
alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt5 = tabMnt[i][j-1];
alt6 = tabMnt[i+1][j-1];
alt7 = tabMnt[i+1][j];
alt8 = tabMnt[i+1][j+1];

if (dirD8[i][j] == -2 || dirD8[i][j] == -3)
        {

//ALT1//////////////////////////////////////////////////////////////////////////
{
    if (alt0 == alt1) //si il a meme altitude que le pixel 1
if (dirD8[i][j+1] != -2 && dirD8[i][j+1] != -3) //et que le pixel voisin 1 possede un gradient vers le bas
        {
            tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
            dAltAssocie[i][j] = dAlti[i][j+1];      // On associe la valeur dAlt du pixel exutoire au pixel plat pliutsplat
        }
}


//ALT5//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt5)
if (dirD8[i][j-1] != -2 && dirD8[i][j-1]!= -3)

// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i][j-1];
    }

}

//ALT6//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt6)
if (dirD8[i+1][j-1] != -2 && dirD8[i+1][j-1]!= -3)

// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j-1];
    }

}

//ALT7//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt7)
if (dirD8[i+1][j] != -2 && dirD8[i+1][j]!= -3)

// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j];
    }

}

//ALT8//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt8)
if (dirD8[i+1][j+1] != -2 && dirD8[i+1][j+1]!= -3)

// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j+1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j+1];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j+1];
    }

}

        }

    }


//Bordure Sud //

i = ligne - 1;
for (int j=1; j <colonne-1;j++ )
    {

alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt2 = tabMnt[i-1][j+1];
alt3 = tabMnt[i-1][j];
alt4 = tabMnt[i-1][j-1];
alt5 = tabMnt[i][j-1];

if (dirD8[i][j] == -2 || dirD8[i][j] == -3)
        {


//ALT1//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt1)                               // si il a meme altitude que le pixel 1
if (dirD8[i][j+1] != -2 && dirD8[i][j+1] != -3) // et que ce pixel voisin 1 possede un gradient vers le bas
        {
            tabDownFlow[i][j] = 1;                // alors le pixel central et en contact avec un pixel exutoire
            dAltAssocie[i][j] = dAlti[i][j+1];      // On associe la valeur dAlt du pixel exutoire au pixel plat pliutsplat
        }
}

//ALT2 //////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt2)
if (dirD8[i-1][j+1] != -2 && dirD8[i-1][j+1] != -3)
    {

// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
    if (tabDownFlow[i][j] == 1)
        {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i-1][j+1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i-1][j+1];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
        }

// B) le pixel 0 n'a pas encore un autre exutoire
    else
        {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j+1];
        }


    }
}

//ALT3//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt3)
if (dirD8[i-1][j] != -2 && dirD8[i-1][j]!= -3)
{
// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i-1][j] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i-1][j];     // On remplace l'ancienne dAlt par la nouvelle

            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j];
    }
}
    }

//ALT4//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt4)
if (dirD8[i-1][j-1] != -2 && dirD8[i-1][j-1]!= -3)
{

// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i-1][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i-1][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j-1];
    }
}
}

//ALT5//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt5)
if (dirD8[i][j-1] != -2 && dirD8[i][j-1]!= -3)
{
// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i][j-1];
    }
}
}


        }

    }



//Bordure Est

int j = colonne -1;
for (int i=1; i<ligne-1;i++)
    {
alt0 = tabMnt [i][j];
alt3 = tabMnt[i-1][j];
alt4 = tabMnt[i-1][j-1];
alt5 = tabMnt[i][j-1];
alt6 = tabMnt[i+1][j-1];
alt7 = tabMnt[i+1][j];

if (dirD8[i][j] == -2 || dirD8[i][j] == -3)
        {

//ALT3Moifier//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt3)
if (dirD8[i-1][j] != -2 && dirD8[i-1][j]!= -3)
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j];
    }

}

//ALT4//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt4)
if (dirD8[i-1][j-1] != -2 && dirD8[i-1][j-1]!= -3)

// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i-1][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i-1][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j-1];
    }

}

//ALT5//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt5)
if (dirD8[i][j-1] != -2 && dirD8[i][j-1]!= -3)

// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i][j-1];
    }

}

//ALT6//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt6)
if (dirD8[i+1][j-1] != -2 && dirD8[i+1][j-1]!= -3)

// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j-1];
    }

}

//ALT7//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt7)
if (dirD8[i+1][j] != -2 && dirD8[i+1][j]!= -3)

// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j];
    }

}


        }
    }

//Bordure West

j = 0;
for (int i=1; i<ligne-1;i++)
{
alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt2 = tabMnt[i-1][j+1];
alt3 = tabMnt[i-1][j];
alt7 = tabMnt[i+1][j];
alt8 = tabMnt[i+1][j+1];

if (dirD8[i][j] == -2 || dirD8[i][j] == -3)
    {


//ALT1//////////////////////////////////////////////////////////////////////////
{
    if (alt0 == alt1) //si il a meme altitude que le pixel 1
if (dirD8[i][j+1] != -2 && dirD8[i][j+1] != -3) //et que le pixel voisin 1 possede un gradient vers le bas
        {
            tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
            dAltAssocie[i][j] = dAlti[i][j+1];      // On associe la valeur dAlt du pixel exutoire au pixel plat pliutsplat
        }
}

//ALT2 //////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt2)
if (dirD8[i-1][j+1] != -2 && dirD8[i-1][j+1] != -3)

// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i-1][j+1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i-1][j+1];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j+1];
    }


}

//ALT3//////////////////////////////////////////////////////////////////////////
{
    if (alt0 == alt3)
if (dirD8[i-1][j] != -2 && dirD8[i-1][j]!= -3)

// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i-1][j] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i-1][j];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j];
    }

    }

//ALT7//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt7)
if (dirD8[i+1][j] != -2 && dirD8[i+1][j]!= -3)

// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j];
    }

}

//ALT8//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt8)
if (dirD8[i+1][j+1] != -2 && dirD8[i+1][j+1]!= -3)

// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j+1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j+1];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j+1];
    }

}

    }

}



// Calcul pour les coins////////////////////////////////////////////////////////

// Coin NE
{
i = 0;
j = colonne -1;

alt0 = tabMnt [i][j];
alt5 = tabMnt[i][j-1];
alt6 = tabMnt[i+1][j-1];
alt7 = tabMnt[i+1][j];

if (dirD8[i][j] != -2 || dirD8[i][j] != -3)
    {

//ALT5//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt5)
if (dirD8[i][j-1] != -2 && dirD8[i][j-1]!= -3)
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i][j-1];
    }

}

//ALT6//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt6)
if (dirD8[i+1][j-1] != -2 && dirD8[i+1][j-1]!= -3)

// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j-1];
    }

}

//ALT7//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt7)
if (dirD8[i+1][j] != -2 && dirD8[i+1][j]!= -3)

// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j];
    }

}




    }



}


// Coin NW
{
i=0;
j=0;

alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt7 = tabMnt[i+1][j];
alt8 = tabMnt[i+1][j+1];

if (dirD8[i][j] != -2 || dirD8[i][j] != -3)
    {

//ALT1//////////////////////////////////////////////////////////////////////////
{
    if (alt0 == alt1) //si il a meme altitude que le pixel 1
if (dirD8[i][j+1] != -2 && dirD8[i][j+1] != -3) //et que le pixel voisin 1 possede un gradient vers le bas
        {
            tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
            dAltAssocie[i][j] = dAlti[i][j+1];      // On associe la valeur dAlt du pixel exutoire au pixel plat pliutsplat
        }
}

//ALT7//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt7)
if (dirD8[i+1][j] != -2 && dirD8[i+1][j]!= -3)

// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j];
    }

}

//ALT8//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt8)
if (dirD8[i+1][j+1] != -2 && dirD8[i+1][j+1]!= -3)

// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j+1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j+1];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j+1];
    }

}


    }

}


// Coin SW
{
i = ligne - 1;
j = 0;

alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt2 = tabMnt[i-1][j+1];
alt3 = tabMnt[i-1][j];

if (dirD8[i][j] != -2 || dirD8[i][j] != -3)
    {


//ALT1//////////////////////////////////////////////////////////////////////////
{
    if (alt0 == alt1) //si il a meme altitude que le pixel 1
if (dirD8[i][j+1] != -2 && dirD8[i][j+1] != -3) //et que le pixel voisin 1 possede un gradient vers le bas
        {
            tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
            dAltAssocie[i][j] = dAlti[i][j+1];      // On associe la valeur dAlt du pixel exutoire au pixel plat pliutsplat
        }
}

//ALT2 //////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt2)
if (dirD8[i-1][j+1] != -2 && dirD8[i-1][j+1] != -3)

// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i-1][j+1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i-1][j+1];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j+1];
    }


}

//ALT3//////////////////////////////////////////////////////////////////////////
{
    if (alt0 == alt3)
if (dirD8[i-1][j] != -2 && dirD8[i-1][j]!= -3)

// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i-1][j] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i-1][j];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j];
    }

    }


    }
}


// Coin SE
{
i = ligne - 1;
j = colonne - 1;

alt0 = tabMnt [i][j];
alt3 = tabMnt[i-1][j];
alt4 = tabMnt[i-1][j-1];
alt5 = tabMnt[i][j-1];


if (dirD8[i][j] != -2 || dirD8[i][j] != -3)
    {

//ALT3Moifier//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt3)
if (dirD8[i-1][j] != -2 && dirD8[i-1][j]!= -3)
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j];
    }

}

//ALT4//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt4)
if (dirD8[i-1][j-1] != -2 && dirD8[i-1][j-1]!= -3)

// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i-1][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i-1][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j-1];
    }

}

//ALT5//////////////////////////////////////////////////////////////////////////
{
if (alt0 == alt5)
if (dirD8[i][j-1] != -2 && dirD8[i][j-1]!= -3)

// A) Si le pixel 0 a deja un exutoire on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 1)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }
// Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else
    {
        tabDownFlow[i][j] = 1;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i][j-1];
    }

}


    }

}



}//fin du calcul des bordures




//test pour savoir s'il ya des exutoires
int cont = 0;
*pointExutoire = 0;

for (int i=0; i<ligne;i++)
        {
      for (int j=0; j <colonne;j++ )
            {

     if (tabDownFlow[i][j] == 1)
    cont++;

            }
        }

 if (cont == 0)
    {
        *pointExutoire = 1;
    }








// 2: DEUXIEME ETAPE
// ON CONNECTE TOUS LES PIXELS AU PIXEL QUI SONT UN CONTACT AVEC UN EXUTOIRE

// Debut de la boucle d'itération pour connaitre la connexion entre les pixels plats
// et les pixels collé aux pixels exutoires

////////////////////////////////////////////////////////////////////////////////
for (int x=0; x < 100; x++) //debut de la boucle x iteration
                            // 100 est un chiffre pris au hasard qui doit etre suffisant pour coller tout les pixels
{


// Calcul dans la matrice
for (int i=1; i<ligne-1;i++)
        {
      for (int j=1; j <colonne-1;j++ )
            {


if (dirD8[i][j] == -2 || dirD8[i][j] == -3)
if (tabDownFlow[i][j] != 1) //si le pixel 0 est un plat ou un puitsplat n'est pas le premier pixel sortie collé au pixel exutoire
    {

//ALT1//////////////////////////////////////////////////////////////////////////
{

if (tabDownFlow[i][j+1] == (1 + x) && tabDownFlow[i][j] == 0)      //si le pixel voisin 1 (pixel sortie) est en contact avec un pixel exutoire
        {
            tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel sortie
            dAltAssocie[i][j] = dAlti[i][j+1];      // On associe la valeur dAlt du pixel exutoire au pixel plat pliutsplat
        }
}


//ALT2 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i-1][j+1] == 1 + x)                      //si le pixel voisin 1 est deja en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i-1][j+1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i-1][j+1];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j+1];
    }
}

}


//ALT3 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i-1][j] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{

// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i-1][j] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i-1][j];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j];
    }
}

}


//ALT4 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i-1][j-1] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i-1][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i-1][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j-1];
    }
}

}


//ALT5 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i][j-1] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i][j-1];
    }


}
}


//ALT6 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i+1][j-1] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j-1];
    }
}

}


//ALT7 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i+1][j] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j];
    }

}
}


//ALT8 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i+1][j+1] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j+1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j+1];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j+1];
    }
}

}








    }




}} //fin de la boucle Calcul dans la matrice





// Calcul sur les bordures
{
//Bordure Nord //

int i = 0;
for (int j=1; j <colonne-1;j++ )
    {


if (dirD8[i][j] == -2 || dirD8[i][j] == -3)
if (tabDownFlow[i][j] != 1) //si le pixel 0 est un plat ou un puitsplat et pas un pixel sortie
        {


//ALT1//////////////////////////////////////////////////////////////////////////
{

if (tabDownFlow[i][j+1] == (1 + x) && tabDownFlow[i][j] == 0)                        //si le pixel voisin 1 (pixel sortie) est en contact avec un pixel exutoire
        {
            tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel sortie
            dAltAssocie[i][j] = dAlti[i][j+1];      // On associe la valeur dAlt du pixel exutoire au pixel plat pliutsplat
        }
}


//ALT5 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i][j-1] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i][j-1];
    }
}

}


//ALT6 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i+1][j-1] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j-1];
    }
}

}


//ALT7 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i+1][j] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j];
    }

}
}


//ALT8 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i+1][j+1] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j+1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j+1];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j+1];
    }
}

}




        }

    }


//Bordure Sud //

i = ligne - 1;
for (int j=1; j <colonne-1;j++ )
    {



if (dirD8[i][j] == -2 || dirD8[i][j] == -3)
if (tabDownFlow[i][j] != 1) //si le pixel 0 est un plat ou un puitsplat et pas un pixel sortie
        {

//ALT1//////////////////////////////////////////////////////////////////////////
{

if (tabDownFlow[i][j+1] == (1 + x) && tabDownFlow[i][j] == 0)                   //si le pixel voisin 1 (pixel sortie) est en contact avec un pixel exutoire
        {
            tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel sortie
            dAltAssocie[i][j] = dAlti[i][j+1];      // On associe la valeur dAlt du pixel exutoire au pixel plat pliutsplat
        }
}


//ALT2 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i-1][j+1] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i-1][j+1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i-1][j+1];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j+1];
    }
}

}


//ALT3 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i-1][j] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i-1][j] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i-1][j];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j];
    }
}

}


//ALT4 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i-1][j-1] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i-1][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i-1][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j-1];
    }
}

}


//ALT5 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i][j-1] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i][j-1];
    }
}

}



        }

    }


//Bordure Est //

int j = colonne -1;
for (int i=1; i<ligne-1;i++)
    {


if (dirD8[i][j] == -2 || dirD8[i][j] == -3)
if (tabDownFlow[i][j] != 1) //si le pixel 0 est un plat ou un puitsplat et pas un pixel sortie
        {

//ALT3 Modifié //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i-1][j] == (1 + x) && tabDownFlow[i][j] == 0)                      //si le pixel voisin 1 est en contact avec un pixel sortie
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j];
    }


}


//ALT4 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i-1][j-1] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i-1][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i-1][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j-1];
    }

}
}


//ALT5 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i][j-1] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i][j-1];
    }
}

}


//ALT6 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i+1][j-1] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j-1];
    }
}

}


//ALT7 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i+1][j] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j];
    }
}

}






        }
    }


//Bordure West //

j = 0;
for (int i=1; i<ligne-1;i++)
{


if (dirD8[i][j] == -2 || dirD8[i][j] == -3)
if (tabDownFlow[i][j] != 1) //si le pixel 0 est un plat ou un puitsplat et pas un pixel sortie
    {


//ALT1//////////////////////////////////////////////////////////////////////////
{

if (tabDownFlow[i][j+1] == (1 + x) && tabDownFlow[i][j] == 0)                       //si le pixel voisin 1 (pixel sortie) est en contact avec un pixel exutoire
        {
            tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel sortie
            dAltAssocie[i][j] = dAlti[i][j+1];      // On associe la valeur dAlt du pixel exutoire au pixel plat pliutsplat
        }
}


//ALT2 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i-1][j+1] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i-1][j+1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i-1][j+1];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j+1];
    }
}

}


//ALT3 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i-1][j] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i-1][j] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i-1][j];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j];
    }
}

}


//ALT7 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i+1][j] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j];
    }
}

}


//ALT8 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i+1][j+1] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j+1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j+1];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j+1];
    }

}
}



    }


}



// Calcul pour les coins////////////////////////////////////////////////////////

// Coin NE //
{
i = 0;
j = colonne -1;



if (dirD8[i][j] == -2 || dirD8[i][j] == -3)
if (tabDownFlow[i][j] != 1) //si le pixel 0 est un plat ou un puitsplat et pas un pixel sortie
    {


//ALT5 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i][j-1] == (1 + x) && tabDownFlow[i][j] == 0)        //si le pixel voisin 1 est en contact avec un pixel sortie
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i][j-1];
    }

}


//ALT6 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i+1][j-1] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j-1];
    }

}
}


//ALT7 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i+1][j] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j];
    }

}
}




    }



}


// Coin NW //
{
i=0;
j=0;



if (dirD8[i][j] == -2 || dirD8[i][j] == -3)
if (tabDownFlow[i][j] != 1) //si le pixel 0 est un plat ou un puitsplat et pas un pixel sortie
    {
//ALT1//////////////////////////////////////////////////////////////////////////
{

if (tabDownFlow[i][j+1] == (1 + x) && tabDownFlow[i][j] == 0)    //si le pixel voisin 1 (pixel sortie) est en contact avec un pixel exutoire
        {
            tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel sortie
            dAltAssocie[i][j] = dAlti[i][j+1];      // On associe la valeur dAlt du pixel exutoire au pixel plat pliutsplat
        }
}


//ALT7 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i+1][j] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j];
    }

}
}


//ALT8 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i+1][j+1] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i+1][j+1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i+1][j+1];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i+1][j+1];
    }
}

}


    }

}

// Coin SW //
{
i = ligne - 1;
j = 0;



if (dirD8[i][j] == -2 || dirD8[i][j] == -3)
if (tabDownFlow[i][j] != 1) //si le pixel 0 est un plat ou un puitsplat et pas un pixel sortie
    {


//ALT1//////////////////////////////////////////////////////////////////////////
{

if (tabDownFlow[i][j+1] == (1 + x) && tabDownFlow[i][j] == 0)                      //si le pixel voisin 1 (pixel sortie) est en contact avec un pixel exutoire
        {
            tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel sortie
            dAltAssocie[i][j] = dAlti[i][j+1];      // On associe la valeur dAlt du pixel exutoire au pixel plat pliutsplat
        }
}


//ALT2 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i-1][j+1] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i-1][j+1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i-1][j+1];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j+1];
    }
}

}


//ALT3 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i-1][j] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i-1][j] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i-1][j];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j];
    }
}

}




    }
}


// Coin SE //
{
i = ligne - 1;
j = colonne - 1;




if (dirD8[i][j] == -2 || dirD8[i][j] == -3)
if (tabDownFlow[i][j] != 1) //si le pixel 0 est un plat ou un puitsplat et pas un pixel sortie
    {


//ALT3 Modifié //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i-1][j] == (1 + x) && tabDownFlow[i][j] == 0)                     //si le pixel voisin 1 est en contact avec un pixel sortie
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j];
    }


}


//ALT4 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i-1][j-1] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i-1][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i-1][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i-1][j-1];
    }
}

}


//ALT5 //////////////////////////////////////////////////////////////////////////
{
if (tabDownFlow[i][j-1] == 1 + x)                      //si le pixel voisin 1 est en contact avec un pixel sortie
{
// A) Si le pixel 0 est deja en contact avec un pixel sortie on compare les dAlt des exutoires
if (tabDownFlow[i][j] == 2 + x)
    {
//On compare les dAlt des exutoires: on choisit le pixel exutoire avec le plus fort dAlt
        if (dAlti[i][j-1] > dAltAssocie[i][j])
            {
                dAltAssocie[i][j] = dAlti[i][j-1];     // On remplace l'ancienne dAlt par la nouvelle
            }

// ELSE Si dAlt0 du l'exutoire deja enregistrer et PLUS GRAND OU EGALE
// on le garde commme exutoire (on change rien)
    }

// B) le pixel 0 n'a pas encore un autre exutoire
else if (tabDownFlow[i][j] == 0)
    {
        tabDownFlow[i][j] = 2 + x;                   // alors le pixel central et en contact avec un pixel exutoire
        dAltAssocie[i][j] = dAlti[i][j-1];
    }
}

}



    }

}




}//Fin du calcul sur les bordures







} // fin de la boucle x ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/*
// 3
//On regarde quel sont les zones planes sans exutoire et on remplace leur valeur par -1

for (int i=0; i<ligne;i++)
        {
      for (int j=0; j <colonne;j++ )
            {

                if (dirD8[i][j] == -2 || dirD8[i][j] == -3)
                if (tabDownFlow[i][j]==0)
                    {
                        tabDownFlow[i][j]=-1;
                    }

            }
        }

// Ces pixels seront considérés comme inutilisable dans la suite du programme hydro
// Ils serviront juste de zones d'accumulation des eaux météoriques
*/



tab2DDesAlloc(dAltAssocie, ligne);
}


//Indique dans dirD8 la localisation des puits et plats mais ne modifie pas la direction d'écoulement
void testPuitPlat (float **tabMnt, float **dirD8, int *pointCompteurPuits,
        int *pointCompteurPlat, int *pointCompteurPuitsPlat,
        int ligne,int colonne )
{

float alt0=0,alt1=0,alt2=0,alt3=0,alt4=0,alt5=0,alt6=0,alt7=0,alt8=0;
float dAlt01=0,dAlt02=0,dAlt03=0,dAlt04=0,dAlt05=0,dAlt06=0,dAlt07=0,dAlt08=0;

// Mise à zéro des compteurs
*pointCompteurPuits     = 0;
*pointCompteurPlat      = 0;
*pointCompteurPuitsPlat = 0;


// 1
// Calcul dans la matrice///////////////////////////////////////////////////////

    for (int i=1; i<ligne-1;i++)
        {
      for (int j=1; j <colonne-1;j++ )
            {

alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt2 = tabMnt[i-1][j+1];
alt3 = tabMnt[i-1][j];
alt4 = tabMnt[i-1][j-1];
alt5 = tabMnt[i][j-1];
alt6 = tabMnt[i+1][j-1];
alt7 = tabMnt[i+1][j];
alt8 = tabMnt[i+1][j+1];


// Calcul des descentes altimétriques (pour simplifier les calcul dAlt01 = (alt0 - alt1)/r, on suppose que r est égale à 1
dAlt01 = (alt0 - alt1);
dAlt02 = (alt0 - alt2)/sqrt(2);
dAlt03 = (alt0 - alt3);
dAlt04 = (alt0 - alt4)/sqrt(2);
dAlt05 = (alt0 - alt5);
dAlt06 = (alt0 - alt6)/sqrt(2);
dAlt07 = (alt0 - alt7);
dAlt08 = (alt0 - alt8)/sqrt(2);



// Comparaison des descentes altimétriques
if (dAlt01 == 0 && dAlt02 == 0 && dAlt03 == 0 && dAlt04 == 0 && dAlt05 == 0 &&
    dAlt06 == 0 && dAlt07 == 0 && dAlt08 == 0)
        {
            dirD8 [i][j] = -2;// C'est une zone plane (pas de varaition d'altitude)
            *pointCompteurPlat = *pointCompteurPlat + 1;
        }


else if (dAlt01 > 0 || dAlt02 > 0 || dAlt03 > 0 || dAlt04 > 0 || dAlt05 > 0
            || dAlt06 > 0 || dAlt07 > 0 || dAlt08 > 0)
        {}


else if (dAlt01 < 0 && dAlt02 < 0 && dAlt03 < 0 && dAlt04 < 0 && dAlt05 < 0 &&
            dAlt06 < 0 && dAlt07 < 0 && dAlt08 < 0)
        {
            dirD8 [i][j] = -1;// C'est un puits
            *pointCompteurPuits = *pointCompteurPuits + 1;
        }

else    {
            dirD8 [i][j] = -3;  // C'est une zone de puits - plats
            *pointCompteurPuitsPlat = *pointCompteurPuitsPlat + 1;
        }


}} //fin de la boucle Calcul dans la matrice




// 2
// Calcul pour les bordures/////////////////////////////////////////////////////

//Bordure Nord

int i = 0;
for (int j=1; j <colonne-1;j++ )
    {
alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt5 = tabMnt[i][j-1];
alt6 = tabMnt[i+1][j-1];
alt7 = tabMnt[i+1][j];
alt8 = tabMnt[i+1][j+1];


// Calcul des descentes altimétriques (pour simplifier les calcul dAlt01 = (alt0 - alt1)/r, on suppose que r est égale à 1
dAlt01 = (alt0 - alt1);
dAlt05 = (alt0 - alt5);
dAlt06 = (alt0 - alt6)/sqrt(2);
dAlt07 = (alt0 - alt7);
dAlt08 = (alt0 - alt8)/sqrt(2);

// Comparaison des descentes altimétriques
if (dAlt01 == 0 && dAlt05 == 0 && dAlt06 == 0 && dAlt07 == 0 && dAlt08 == 0)
        {
            dirD8 [i][j] = -2;// C'est une zone plane (pas de varaition d'altitude)
            *pointCompteurPlat = *pointCompteurPlat + 1;
        }


else if (dAlt01 > 0 || dAlt05 > 0 || dAlt06 > 0 || dAlt07 > 0 || dAlt08 > 0)
        {}


else if (dAlt01 < 0 && dAlt05 < 0 && dAlt06 < 0 && dAlt07 < 0 && dAlt08 < 0)
        {
            dirD8 [i][j] = -1;// C'est un puits
            *pointCompteurPuits = *pointCompteurPuits + 1;
        }

else    {
            dirD8 [i][j] = -3;  // C'est une zone de puits - plats
            *pointCompteurPuitsPlat = *pointCompteurPuitsPlat + 1;
        }




    }

//Bordure Sud

i = ligne - 1;
for (int j=1; j <colonne-1;j++ )
    {

alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt2 = tabMnt[i-1][j+1];
alt3 = tabMnt[i-1][j];
alt4 = tabMnt[i-1][j-1];
alt5 = tabMnt[i][j-1];

// Calcul des descentes altimétriques (pour simplifier les calcul dAlt01 = (alt0 - alt1)/r, on suppose que r est égale à 1
dAlt01 = (alt0 - alt1);
dAlt02 = (alt0 - alt2)/sqrt(2);
dAlt03 = (alt0 - alt3);
dAlt04 = (alt0 - alt4)/sqrt(2);
dAlt05 = (alt0 - alt5);

// Comparaison des descentes altimétriques
if (dAlt01 == 0 && dAlt02 == 0 && dAlt03 == 0 && dAlt04 == 0 && dAlt05 == 0)
        {
            dirD8 [i][j] = -2;// C'est une zone plane (pas de varaition d'altitude)
            *pointCompteurPlat = *pointCompteurPlat + 1;
        }


else if (dAlt01 > 0 || dAlt02 > 0 || dAlt03 > 0 || dAlt04 > 0 || dAlt05 > 0)
        {}


else if (dAlt01 < 0 && dAlt02 < 0 && dAlt03 < 0 && dAlt04 < 0 && dAlt05 < 0)
        {
            dirD8 [i][j] = -1;// C'est un puits
            *pointCompteurPuits = *pointCompteurPuits + 1;
        }

else    {
            dirD8 [i][j] = -3;  // C'est une zone de puits - plats
            *pointCompteurPuitsPlat = *pointCompteurPuitsPlat + 1;
        }

}

//Bordure Est

int j = colonne -1;
for (int i=1; i<ligne-1;i++)
    {
alt0 = tabMnt [i][j];
alt3 = tabMnt[i-1][j];
alt4 = tabMnt[i-1][j-1];
alt5 = tabMnt[i][j-1];
alt6 = tabMnt[i+1][j-1];
alt7 = tabMnt[i+1][j];

// Calcul des descentes altimétriques (pour simplifier les calcul dAlt01 = (alt0 - alt1)/r, on suppose que r est égale à 1
dAlt03 = (alt0 - alt3);
dAlt04 = (alt0 - alt4)/sqrt(2);
dAlt05 = (alt0 - alt5);
dAlt06 = (alt0 - alt6)/sqrt(2);
dAlt07 = (alt0 - alt7);

// Comparaison des descentes altimétriques
if (dAlt03 == 0 && dAlt04 == 0 && dAlt05 == 0 && dAlt06 == 0 && dAlt07 == 0)
        {
            dirD8 [i][j] = -2;// C'est une zone plane (pas de varaition d'altitude)
            *pointCompteurPlat = *pointCompteurPlat + 1;
        }


else if (dAlt03 > 0 || dAlt04 > 0 || dAlt05 > 0 || dAlt06 > 0 || dAlt07 > 0 )
        {}


else if (dAlt03 < 0 && dAlt04 < 0 && dAlt05 < 0 && dAlt06 < 0 && dAlt07 < 0)
        {
            dirD8 [i][j] = -1;// C'est un puits
            *pointCompteurPuits = *pointCompteurPuits + 1;
        }

else    {
            dirD8 [i][j] = -3;  // C'est une zone de puits - plats
            *pointCompteurPuitsPlat = *pointCompteurPuitsPlat + 1;
        }


}

//Bordure West

j = 0;
for (int i=1; i<ligne-1;i++)
{
alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt2 = tabMnt[i-1][j+1];
alt3 = tabMnt[i-1][j];
alt7 = tabMnt[i+1][j];
alt8 = tabMnt[i+1][j+1];


// Calcul des descentes altimétriques (pour simplifier les calcul dAlt01 = (alt0 - alt1)/r, on suppose que r est égale à 1
dAlt01 = (alt0 - alt1);
dAlt02 = (alt0 - alt2)/sqrt(2);
dAlt03 = (alt0 - alt3);
dAlt07 = (alt0 - alt7);
dAlt08 = (alt0 - alt8)/sqrt(2);

// Comparaison des descentes altimétriques
if (dAlt01 == 0 && dAlt02 == 0 && dAlt03 == 0 && dAlt07 == 0 && dAlt08 == 0)
        {
            dirD8 [i][j] = -2;// C'est une zone plane (pas de varaition d'altitude)
            *pointCompteurPlat = *pointCompteurPlat + 1;
        }


else if (dAlt01 > 0 || dAlt02 > 0 || dAlt03 > 0 || dAlt07 > 0 || dAlt08 > 0)
        {}


else if (dAlt01 < 0 && dAlt02 < 0 && dAlt03 < 0 && dAlt07 < 0 && dAlt08 < 0)
        {
            dirD8 [i][j] = -1;// C'est un puits
            *pointCompteurPuits = *pointCompteurPuits + 1;
        }

else    {
            dirD8 [i][j] = -3;  // C'est une zone de puits - plats
            *pointCompteurPuitsPlat = *pointCompteurPuitsPlat + 1;
        }


}





//3
// Calcul pour les coins////////////////////////////////////////////////////////

// Coin NE
{
i = 0;
j = colonne -1;

alt0 = tabMnt [i][j];
alt5 = tabMnt[i][j-1];
alt6 = tabMnt[i+1][j-1];
alt7 = tabMnt[i+1][j];

// Calcul des descentes altimétriques (pour simplifier les calcul dAlt01 = (alt0 - alt1)/r, on suppose que r est égale à 1
dAlt05 = (alt0 - alt5);
dAlt06 = (alt0 - alt6)/sqrt(2);
dAlt07 = (alt0 - alt7);

// Comparaison des descentes altimétriques
if (dAlt05 == 0 && dAlt06 == 0 && dAlt07 == 0)
        {
            dirD8 [i][j] = -2;// C'est une zone plane (pas de varaition d'altitude)
            *pointCompteurPlat = *pointCompteurPlat + 1;
        }


else if (dAlt05 > 0 || dAlt06 > 0 || dAlt07 > 0)
        {}


else if (dAlt05 < 0 && dAlt06 < 0 && dAlt07 < 0)
        {
            dirD8 [i][j] = -1;// C'est un puits
            *pointCompteurPuits = *pointCompteurPuits + 1;
        }

else    {
            dirD8 [i][j] = -3;  // C'est une zone de puits - plats
            *pointCompteurPuitsPlat = *pointCompteurPuitsPlat + 1;
        }

}

// Coin NW
{
    i=0;
j=0;

alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt7 = tabMnt[i+1][j];
alt8 = tabMnt[i+1][j+1];


// Calcul des descentes altimétriques (pour simplifier les calcul dAlt01 = (alt0 - alt1)/r, on suppose que r est égale à 1
dAlt01 = (alt0 - alt1);
dAlt07 = (alt0 - alt7);
dAlt08 = (alt0 - alt8)/sqrt(2);

// Comparaison des descentes altimétriques
if (dAlt01 == 0 && dAlt07 == 0 && dAlt08 == 0)
        {
            dirD8 [i][j] = -2;// C'est une zone plane (pas de varaition d'altitude)
            *pointCompteurPlat = *pointCompteurPlat + 1;
        }


else if (dAlt01 > 0 || dAlt07 > 0 || dAlt08 > 0)
        {}


else if (dAlt01 < 0 && dAlt07 < 0 && dAlt08 < 0)
        {
            dirD8 [i][j] = -1;// C'est un puits
            *pointCompteurPuits = *pointCompteurPuits + 1;
        }

else    {
            dirD8 [i][j] = -3;  // C'est une zone de puits - plats
            *pointCompteurPuitsPlat = *pointCompteurPuitsPlat + 1;
        }

}


// Coin SW
{
i = ligne - 1;
j = 0;

alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt2 = tabMnt[i-1][j+1];
alt3 = tabMnt[i-1][j];

// Calcul des descentes altimétriques (pour simplifier les calcul dAlt01 = (alt0 - alt1)/r, on suppose que r est égale à 1
dAlt01 = (alt0 - alt1);
dAlt02 = (alt0 - alt2)/sqrt(2);
dAlt03 = (alt0 - alt3);

// Comparaison des descentes altimétriques
if (dAlt01 == 0 && dAlt02 == 0 && dAlt03 == 0)
        {
            dirD8 [i][j] = -2;// C'est une zone plane (pas de varaition d'altitude)
            *pointCompteurPlat = *pointCompteurPlat + 1;
        }


else if (dAlt01 > 0 || dAlt02 > 0 || dAlt03 > 0)
        {}


else if (dAlt01 < 0 && dAlt02 < 0 && dAlt03 < 0)
        {
            dirD8 [i][j] = -1;// C'est un puits
            *pointCompteurPuits = *pointCompteurPuits + 1;
        }

else    {
            dirD8 [i][j] = -3;  // C'est une zone de puits - plats
            *pointCompteurPuitsPlat = *pointCompteurPuitsPlat + 1;
        }
}


// Coin SE
{
i = ligne - 1;
j = colonne - 1;

alt0 = tabMnt [i][j];
alt3 = tabMnt[i-1][j];
alt4 = tabMnt[i-1][j-1];
alt5 = tabMnt[i][j-1];

// Calcul des descentes altimétriques (pour simplifier les calcul dAlt01 = (alt0 - alt1)/r, on suppose que r est égale à 1
dAlt03 = (alt0 - alt3);
dAlt04 = (alt0 - alt4)/sqrt(2);
dAlt05 = (alt0 - alt5);

// Comparaison des descentes altimétriques
if (dAlt03 == 0 && dAlt04 == 0 && dAlt05 == 0)
        {
            dirD8 [i][j] = -2;// C'est une zone plane (pas de varaition d'altitude)
            *pointCompteurPlat = *pointCompteurPlat + 1;
        }


else if (dAlt03 > 0 || dAlt04 > 0 || dAlt05 > 0)
        {}


else if (dAlt03 < 0 && dAlt04 < 0 && dAlt05 < 0)
        {
            dirD8 [i][j] = -1;// C'est un puits
            *pointCompteurPuits = *pointCompteurPuits + 1;
        }

else    {
            dirD8 [i][j] = -3;  // C'est une zone de puits - plats
            *pointCompteurPuitsPlat = *pointCompteurPuitsPlat + 1;
        }

}









}


//Calcul des TabS
//----------------------------------------------------------------------------------
float tabS1(float alt0,float alt1,float alt2,float tableauR1,float cPixel)
{
float tableauS1 = 0;
float D1 = 0;
float D2 = 0;
float S1 = 0;
float S2 = 0;

//calcul de D1 et D2

D1 = sqrt((cPixel*cPixel)+((alt1-alt0)*(alt1-alt0)));
D2 = sqrt((cPixel*cPixel)+((alt2-alt1)*(alt2-alt1)));

S1 = (alt0-alt1)/D1;
S2 = (alt1-alt2)/D2;

tableauS1 = sqrt((S1*S1)+(S2*S2));

if (tableauR1<=0)
{tableauS1 = S1;
//printf("cas 1");
}

if (tableauR1>=atan(D2/D1))
{tableauS1 = (alt0 -alt2)/sqrt((D1*D1)+(D2*D2));
//printf("cas 2");
}

if (alt1>alt0 && alt2>alt0)
{tableauS1=-1;}

if (alt1==alt0 && alt2>alt0)
{tableauS1=-1;}

if (alt1>alt0 && alt2==alt0)
{tableauS1=-1;}

if (alt1==alt0 && alt2==alt0)
{tableauS1=-1;}


return tableauS1;
}

//----------------------------------------------------------------------------------
float tabS2(float alt0,float alt2,float alt1,float tableauR2,float cPixel)
{
float tableauS2 = 0;
float D1 = 0;
float D2 = 0;
float S1 = 0;
float S2 = 0;

//calcul de D1 et D2

D1 = sqrt((cPixel*cPixel)+((alt1-alt0)*(alt1-alt0)));
D2 = sqrt((cPixel*cPixel)+((alt2-alt1)*(alt2-alt1)));

S1 = (alt0-alt1)/D1;
S2 = (alt1-alt2)/D2;

tableauS2 = sqrt((S1*S1)+(S2*S2));//printf("\n\ncas0\n");

if (tableauR2 >=(PI/4))
{tableauS2 = S1;
//printf("cas1\n");
}

if (tableauR2 <=0)
{tableauS2 = (alt0 -alt2)/sqrt((D1*D1)+(D2*D2));
//printf("cas2\n");
}

if (alt1>alt0 && alt2>alt0)
{tableauS2=-1;
//printf("cas3\n");
}

if (alt1==alt0 && alt2>alt0)
{tableauS2=-1;
//printf("cas4\n");
}

if (alt1>alt0 && alt2==alt0)
{tableauS2=-1;
//printf("cas5\n");
}

if (alt1==alt0 && alt2==alt0)
{tableauS2=-1;}

//printf("alt0 = %lf  alt1 = %lf   alt2 = %lf\n",alt0, alt1,alt2);
//printf("D1 = %lf   D2 = %lf\n", D1,D2);
//printf("S1 = %lf   S2 = %lf\n", S1,S2);
//printf("tableauS2 = %lf \n\n\n",tableauS2);
  //system("PAUSE");

return tableauS2;
}


//Calcul des TabR
//----------------------------------------------------------------------------------
float tabR1(float alt0,float alt1,float alt2, float cPixel)
{

float tableauR1 =0;
float D1 = 0;
float D2 = 0;

//calcul de D1 et D2

D1 = sqrt((cPixel*cPixel)+((alt1-alt0)*(alt1-alt0)));
D2 = sqrt((cPixel*cPixel)+((alt2-alt1)*(alt2-alt1)));


//Evite les erreurs de calculs
if (alt0 == alt1)
{tableauR1 = atan ((D1*(alt1 - alt2))/(D2*0.01));}
else
{tableauR1 = atan ((D1*(alt1 - alt2))/(D2*(alt0 - alt1)));}

//Evite d'avoir un angle qui sort de la facette en forme de triangle
if (tableauR1<0)
{          tableauR1 = 0;}
else if (tableauR1>(PI/4))
{       tableauR1 = (PI/4);
}

//Elimine les facettes qui ne sont pas dirigées vers le bas
if (alt1>alt0 && alt2>alt0)
{tableauR1=-1;}
if (alt1==alt0 && alt2>alt0)
{tableauR1=-1;}
if (alt1>alt0 && alt2==alt0)
{tableauR1=-1;}


return tableauR1;
}

//----------------------------------------------------------------------------------
float tabR2(float alt0,float alt2,float alt1, float cPixel)
{
float tableauR2 =0;
float D1 = 0;
float D2 = 0;

//calcul de D1 et D2

D1 = sqrt((cPixel*cPixel)+((alt1-alt0)*(alt1-alt0)));
D2 = sqrt((cPixel*cPixel)+((alt2-alt1)*(alt2-alt1)));


//Evite les erreurs de calculs
if (alt0 == alt1)
{tableauR2 = (PI/4) - atan ((D1*(alt1 - alt2))/(D2*0.00001));}
else
{tableauR2 = (PI/4) - atan ((D1*(alt1 - alt2))/(D2*(alt0 - alt1)));
}

//Evite d'avoir un angle qui sort de la facette en forme de triangle
if (tableauR2 > (PI/4))
{          tableauR2 =(PI/4);
 }
else if (  tableauR2 < (PI/4) - atan(D2/D1))
{          tableauR2 =0;
}

//Elimine les facettes qui ne sont pas dirigées vers le bas
if (alt1>alt0 && alt2>alt0)
{tableauR2=-1;}
if (alt1==alt0 && alt2>alt0)
{tableauR2=-1;}
if (alt1>alt0 && alt2==alt0)
{tableauR2=-1;}


return tableauR2;
}



// "Corrige" les puits en élevant les pixels puits jusqu'au pixel adjacent le plus bas
void CorrectPuits (float **tabMnt, float **dirD8,  int ligne,int colonne )
{
float alt0=0,alt1=0,alt2=0,alt3=0,alt4=0,alt5=0,alt6=0,alt7=0,alt8=0;

// 1
// Calcul dans la matrice///////////////////////////////////////////////////////
    for (int i=1; i<ligne-1;i++)
        {
      for (int j=1; j <colonne-1;j++ )
            {

alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt2 = tabMnt[i-1][j+1];
alt3 = tabMnt[i-1][j];
alt4 = tabMnt[i-1][j-1];
alt5 = tabMnt[i][j-1];
alt6 = tabMnt[i+1][j-1];
alt7 = tabMnt[i+1][j];
alt8 = tabMnt[i+1][j+1];

                if (dirD8[i][j] == -1)
                    {
// On cherche le pixel adjacent le plus bas
// La fonction valeur min renvoi la valeur minimale rencontré mais n'indique pas
// si il y des pixels adjacents de meme altitude
        tabMnt [i][j] = valeurMin (alt1,alt2,alt3, alt4,alt5,alt6,alt7,alt8);
                    }

}} // fin de la boucle calcul dans la matrice



// 2
// Calcul pour les bordures/////////////////////////////////////////////////////

//Bordure Nord//

int i = 0;
for (int j=1; j <colonne-1;j++ )
    {
alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt5 = tabMnt[i][j-1];
alt6 = tabMnt[i+1][j-1];
alt7 = tabMnt[i+1][j];
alt8 = tabMnt[i+1][j+1];

            if (dirD8[i][j] == -1)
                    {
// On cherche le pixel adjacent le plus bas
// La fonction valeur min renvoi la valeur minimale rencontré mais n'indique pas
// si il y des pixels adjacents de meme altitude
        tabMnt [i][j] = valeurMin (alt1,10000,10000,10000,alt5,alt6,alt7,alt8);
                    }

    }//fin de la boucle bordure Nord



//Bordure Sud//

i = ligne - 1;
for (int j=1; j <colonne-1;j++ )
    {

alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt2 = tabMnt[i-1][j+1];
alt3 = tabMnt[i-1][j];
alt4 = tabMnt[i-1][j-1];
alt5 = tabMnt[i][j-1];

if (dirD8[i][j] == -1)
                    {
// On cherche le pixel adjacent le plus bas
// La fonction valeur min renvoi la valeur minimale rencontré mais n'indique pas
// si il y des pixels adjacents de meme altitude
        tabMnt [i][j] = valeurMin (alt1,alt2,alt3, alt4,alt5,10000,10000,10000);
                    }

    }// fin de la boucle bordure Sud


//Bordure Est

int j = colonne -1;
for (int i=1; i<ligne-1;i++)
    {
alt0 = tabMnt [i][j];
alt3 = tabMnt[i-1][j];
alt4 = tabMnt[i-1][j-1];
alt5 = tabMnt[i][j-1];
alt6 = tabMnt[i+1][j-1];
alt7 = tabMnt[i+1][j];

 if (dirD8[i][j] == -1)
                    {
// On cherche le pixel adjacent le plus bas
// La fonction valeur min renvoi la valeur minimale rencontré mais n'indique pas
// si il y des pixels adjacents de meme altitude
        tabMnt [i][j] = valeurMin (10000,10000,alt3, alt4,alt5,alt6,alt7,10000);
                    }

    }//fin de la boucle bordure Est



//Bordure West

j = 0;
for (int i=1; i<ligne-1;i++)
{
alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt2 = tabMnt[i-1][j+1];
alt3 = tabMnt[i-1][j];
alt7 = tabMnt[i+1][j];
alt8 = tabMnt[i+1][j+1];

 if (dirD8[i][j] == -1)
                    {
// On cherche le pixel adjacent le plus bas
// La fonction valeur min renvoi la valeur minimale rencontré mais n'indique pas
// si il y des pixels adjacents de meme altitude
        tabMnt [i][j] = valeurMin (alt1,alt2,alt3, 10000,10000,10000,alt7,alt8);
                    }

} // fin de la boucle bordure West


//3
// Calcul pour les coins////////////////////////////////////////////////////////

// Coin NE //
{
i = 0;
j = colonne -1;

alt0 = tabMnt [i][j];
alt5 = tabMnt[i][j-1];
alt6 = tabMnt[i+1][j-1];
alt7 = tabMnt[i+1][j];

            if (dirD8[i][j] == -1)
                    {
// On cherche le pixel adjacent le plus bas
// La fonction valeur min renvoi la valeur minimale rencontré mais n'indique pas
// si il y des pixels adjacents de meme altitude
    tabMnt [i][j] = valeurMin (10000,10000,10000, 10000,alt5,alt6,alt7,10000);
                    }

} // fin de la boucle calcul coin NE



// Coin NW //
{
i=0;
j=0;

alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt7 = tabMnt[i+1][j];
alt8 = tabMnt[i+1][j+1];

            if (dirD8[i][j] == -1)
                    {
// On cherche le pixel adjacent le plus bas
// La fonction valeur min renvoi la valeur minimale rencontré mais n'indique pas
// si il y des pixels adjacents de meme altitude
    tabMnt [i][j] = valeurMin (alt1,10000,10000, 10000,10000,10000,alt7,alt8);
                    }

} // fin de la boucle calcul coin NW


// Coin SW //
{
i = ligne - 1;
j = 0;

alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt2 = tabMnt[i-1][j+1];
alt3 = tabMnt[i-1][j];

            if (dirD8[i][j] == -1)
                    {
// On cherche le pixel adjacent le plus bas
// La fonction valeur min renvoi la valeur minimale rencontré mais n'indique pas
// si il y des pixels adjacents de meme altitude
    tabMnt [i][j] = valeurMin (alt1,alt2,alt3, 10000,10000,10000,10000,10000);
                    }

} // fin de la boucle calcul coin SW




// Coin SE //
{
i = ligne - 1;
j = colonne - 1;

alt0 = tabMnt [i][j];
alt3 = tabMnt[i-1][j];
alt4 = tabMnt[i-1][j-1];
alt5 = tabMnt[i][j-1];

            if (dirD8[i][j] == -1)
                    {
// On cherche le pixel adjacent le plus bas
// La fonction valeur min renvoi la valeur minimale rencontré mais n'indique pas
// si il y des pixels adjacents de meme altitude
    tabMnt [i][j] = valeurMin (10000,10000,alt3, alt4,alt5,10000,10000,10000);
                    }

} // fin de la boucle calcul coin SE











}



void NoPlaneMartz (float **tabMnt, float **dirD8, float **dAlti,
                    int *pointCompteurPuits, int *pointCompteurPlat,
                    int *pointCompteurPuitsPlat, int ligne, int colonne,
                    float resolutionVert)
{
// Variable pour le calcul
float exutoire = 0;
float *pointExutoire = &exutoire;

// 1 ///////////////////////////////////////////////////////////////////////////
// On calcul la taille de l'increment 1 et 2
float incr1 = 0.00002 * resolutionVert;
float incr2 = 0.00001 * resolutionVert;


// 2
// Calcul des incrementations pour le downflow ///////////////////////////////////////////////////////////////////////////
// Ne pas prendre en compte les depressions sans exutoires
float** tabDownFlow = NULL;
tabDownFlow = tab2DAlloc(ligne, colonne);

 DownFlow (tabMnt, dirD8, dAlti, tabDownFlow, pointExutoire, ligne, colonne);


// 3
// Calcul des incrementations pour le upflow ///////////////////////////////////////////////////////////////////////////
// Ne pas prendre en compte les plats sur des sommets
float** tabUpFlow = NULL;
tabUpFlow = tab2DAlloc(ligne, colonne);

 UpFlow (tabMnt, dirD8, dAlti, tabUpFlow, ligne, colonne);

// 4
// Addition des deux tableaux d'increment
float** tabAdd = NULL;
tabAdd = tab2DAlloc(ligne, colonne);


for (int i=0; i<ligne;i++)
      for (int j=0; j <colonne;j++ )
                tabAdd[i][j]  =   tabDownFlow[i][j] + tabUpFlow[i][j];






// 5
// On modifie le MNT avec le nombre d'itération calculée


for (int i=0; i<ligne;i++)
      for (int j=0; j <colonne;j++ )
                tabMnt[i][j]  =   tabMnt[i][j] + tabAdd[i][j] * incr1;




// 6
// On calcul les nouvelles direction //
D8 ( tabMnt, dirD8, dAlti, pointCompteurPuits, pointCompteurPlat,
        pointCompteurPuitsPlat, ligne, colonne);

// 7
// Si il y a encore des puits, des plats ou des puits plats on recommence down flow pour un incr2
//on creer un pointeur pour savoir si il reste des exutoires

int test=0;

do
{

    test++;
    cout << test << endl;

if (*pointCompteurPuits > 0)  CorrectPuits (tabMnt, dirD8, ligne, colonne);


if (*pointCompteurPlat > 0 || *pointCompteurPuitsPlat>0)
{

// mise a zéro de tab down flow
for (int i=0; i<ligne;i++)
      for (int j=0; j <colonne;j++ )
                tabDownFlow[i][j]  =   0;


// On calcul le nombre d'iétration nécessaire à l'écoulement vers le bas
DownFlow (tabMnt, dirD8, dAlti, tabDownFlow, pointExutoire,ligne, colonne);


for (int i=0; i<ligne;i++)
      for (int j=0; j <colonne;j++ )
                tabMnt[i][j]  =   tabMnt[i][j] + tabDownFlow[i][j] * incr2;


// On vérifie si il y a encore des plats //
D8 ( tabMnt, dirD8, dAlti, pointCompteurPuits, pointCompteurPlat,
        pointCompteurPuitsPlat, ligne, colonne);

}




} while ( *pointExutoire == 0);



 D8 ( tabMnt, dirD8, dAlti, pointCompteurPuits, pointCompteurPlat,
        pointCompteurPuitsPlat, ligne, colonne); //donne les directions D8 en degre non corrigé :

//Ecriture des problemes rencontrer lors du traitement hydro
printf("\nResultat du traitement........................................\n");
printf("    Nombre de puits =       %d \n", *pointCompteurPuits);
printf("    Nombre de plats =       %d \n", *pointCompteurPlat);
printf("    Nombre de puits-plats = %d \n", *pointCompteurPuitsPlat);









tab2DDesAlloc(tabDownFlow, ligne); //Désallocation
tab2DDesAlloc(tabUpFlow, ligne); //Désallocation
tab2DDesAlloc(tabAdd, ligne); //Désallocation


}



// Obtention de la valeur max-----------------------------------------------------------------
float valeurMaxS(float tabS1,float tabS2,float tabS3,
       float tabS4,float tabS5,float tabS6,float tabS7,
       float tabS8)
{
int p=1;

if (tabS1>tabS2 && tabS1>tabS3 && tabS1>tabS4 &&
    tabS1>tabS5 && tabS1>tabS6 && tabS1>tabS7 &&
    tabS1>tabS8)
    {return tabS1;}

else if (tabS2>tabS1 && tabS2>tabS3 && tabS2>tabS4 &&
    tabS2>tabS5 && tabS2>tabS6 && tabS2>tabS7 &&
    tabS2>tabS8)
    {return tabS2;}

else if (tabS3>tabS2 && tabS3>tabS1 && tabS3>tabS4 &&
    tabS3>tabS5 && tabS3>tabS6 && tabS3>tabS7 &&
    tabS3>tabS8)
    {return tabS3;}

else if (tabS4>tabS2 && tabS4>tabS3 && tabS4>tabS1 &&
    tabS4>tabS5 && tabS4>tabS6 && tabS4>tabS7 &&
    tabS4>tabS8)
    {return tabS4;}

else if (tabS5>tabS2 && tabS5>tabS3 && tabS5>tabS4 &&
    tabS5>tabS1 && tabS5>tabS6 && tabS5>tabS7 &&
    tabS5>tabS8)
    {return tabS5;}

else if (tabS6>tabS2 && tabS6>tabS3 && tabS6>tabS4 &&
    tabS6>tabS5 && tabS6>tabS1 && tabS6>tabS7 &&
    tabS6>tabS8)
    {return tabS6;}

else if (tabS7>tabS2 && tabS7>tabS3 && tabS7>tabS4 &&
    tabS7>tabS5 && tabS7>tabS6 && tabS7>tabS1 &&
    tabS7>tabS8)
    {return tabS7;}

else if (tabS8>tabS2 && tabS8>tabS3 && tabS8>tabS4 &&
    tabS8>tabS5 && tabS8>tabS6 && tabS8>tabS7 &&
    tabS8>tabS1)
    {return tabS8;}

else if (tabS1==tabS2 && tabS1!=-1.0)
{if (tabS8>=tabS3)
 {return tabS1;}
 else
 {return tabS2;}}

else if (tabS2==tabS3 && tabS2!=-1.0)
{if (tabS1>=tabS4)
 {return tabS2;}
 else
 {return tabS3;}}

else if (tabS3==tabS4  && tabS3!=-1.0)
{if (tabS2>=tabS5)
 {return tabS3;}
 else
 {return tabS4;}}

else if (tabS4==tabS5 && tabS4!=-1.0)
{if (tabS3>=tabS6)
 {return tabS4;}
 else
 {return tabS5;}}

else if (tabS5==tabS6 && tabS5!=-1.0)
{if (tabS4>=tabS7)
 {return tabS5;}
 else
 {return tabS6;}}

else if (tabS6==tabS7 && tabS6!=-1.0)
{if (tabS5>=tabS8)
 {return tabS6;}
 else
 {return tabS7;}}

else if (tabS7==tabS8 && tabS7!=-1.0)
{if (tabS6>=tabS1)
 {return tabS7;}
 else
 {return tabS8;}}

else if (tabS8==tabS1 && tabS8!=-1.0)
{if (tabS7>=tabS2)
 {return tabS8;}
 else
 {return tabS1;}}

else if ( tabS1==tabS3 && tabS1!=-1.0 ||
 tabS1==tabS4 && tabS1!=-1 ||
 tabS1==tabS5 && tabS1!=-1 ||
 tabS1==tabS6 && tabS1!=-1 ||
 tabS1==tabS7 && tabS1!=-1)
 {return tabS1;}

else if ( tabS2==tabS4 && tabS2!=-1.0 ||
 tabS2==tabS5 && tabS2!=-1 ||
  tabS2==tabS6 && tabS2!=-1||
  tabS2==tabS7 && tabS2!=-1 ||
  tabS2==tabS8 && tabS2!=-1)
 {return tabS2;}

else if (tabS3==tabS5 && tabS3!=-1.0 ||
 tabS3==tabS6  && tabS3!=-1||
tabS3==tabS7 && tabS3!=-1||
tabS3==tabS8 && tabS3!=-1)
 {return tabS3;}

else if ( tabS4==tabS8 && tabS4!=-1.0 ||
 tabS4==tabS6  && tabS4!=-1||
  tabS4==tabS7 && tabS4!=-1)
 {return tabS4;}

else if ( tabS5==tabS8 && tabS5!=-1.0 ||
 tabS5==tabS7 && tabS5!=-1)
 {return tabS5;}

else if ( tabS6==tabS8 && tabS6!=-1.0)
 {return tabS6;}

else
    {p++;
    //printf("%lf %lf %lf %lf %lf %lf %lf %lf\n",tabS1,tabS2,tabS3,
  //tabS4,tabS5,tabS6,tabS7,tabS8);

    //printf("nombre de points non calcule pour S = %d\n",p);
    }
return tabS1;//soit tabS = -1;
system("pause");
}




void CalculDirPente(float **tabMnt,float** tabRg, float** tabPente, int ligne, int colonne, float cPixel)

{
//Variable pour le calcul
int i=0, j=0;
float tableauR1=0, tableauR2=0,tableauR3=0,tableauR4=0,tableauR5=0,tableauR6=0,tableauR7=0,tableauR8=0;
float tableauS1=0, tableauS2=0, tableauS3=0, tableauS4=0,  tableauS5=0,tableauS6=0, tableauS7=0, tableauS8=0;
float alt0=0,alt1=0, alt2=0, alt3=0, alt4=0, alt5=0, alt6=0, alt7=0, alt8=0;




//Calcul de r et s-----------------------------------------------------------------------------

// Création de tableauR, tabRg et tableauS-------------------------------------------
float** tableauR;
float** tableauS;

tableauR = tab2DAlloc(ligne, colonne);//R
tableauS = tab2DAlloc(ligne, colonne);//S


//condition et calcul-------------------------------------------------------------------
for (i=0; i<ligne ; i++)
  {
for (j=0; j<colonne ; j++)
  {

// Calcul pour les quatres coins-----------------------------------------------------------------------
if (i==0 && j==0)//************************************************************
{
//Calcul de S pour chaque triangle -------------
alt0 = tabMnt[i][j];
alt1 = tabMnt[i][j+1];
alt7 = tabMnt[i+1][j];
alt8 = tabMnt[i+1][j+1];

tableauR7 =0;
tableauR8 =0;
tableauS7 =0;
tableauS8 =0;

tableauR7 = tabR1(alt0, alt7, alt8, cPixel);
tableauR8 = tabR2(alt0, alt8, alt1, cPixel);

tableauS7 = tabS1(alt0, alt7, alt8, tableauR7, cPixel);
tableauS8 = tabS2(alt0, alt8, alt1, tableauR8, cPixel);

tableauS[i][j]= valeurMaxS(-1,-1,-1,-1,-1,-1,tableauS7,tableauS8);

//EN fonction du Smax on calcul R et R'------------

if (tableauS[i][j] == tableauS7)
{tableauR[i][j] = tableauR7;
 tabRg[i][j]=  tableauR[i][j]+(6*PI/4);}

else if (tableauS[i][j] == tableauS8)
{tableauR[i][j] = tableauR8;
 tabRg[i][j]= tableauR[i][j]+(7*PI/4);}


}

else if (i==0 && j==colonne-1)//***********************************************
{
//Calcul pour chaque triangle S-------------

alt0 = tabMnt [i][j];
alt5 = tabMnt[i][j-1];
alt6 = tabMnt[i+1][j-1];
alt7 = tabMnt[i+1][j];

tableauR5 =0;
tableauR6 =0;
tableauS5 =0;
tableauS6 =0;

tableauR5 = tabR1(alt0, alt5, alt6, cPixel);
tableauR6 = tabR2(alt0, alt6, alt7, cPixel);

tableauS5 = tabS1(alt0, alt5, alt6, tableauR5, cPixel);
tableauS6 = tabS2(alt0, alt6, alt7, tableauR6, cPixel);

tableauS[i][j]= valeurMaxS(-1,-1,-1,-1,tableauS5,tableauS6,-1,-1);

//EN fonction du Smax on calcul R = R'------------

if (tableauS[i][j] == tableauS5)
{tableauR[i][j] = tableauR5;
 tabRg[i][j]=  tableauR[i][j]+PI;}

else if (tableauS[i][j] == tableauS6)
{tableauR[i][j] = tableauR6;
 tabRg[i][j]= tableauR[i][j]+(5*PI/2);}

}

else if (i==ligne-1 && j==0)//*************************************************
{
alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt2 = tabMnt[i-1][j+1];
alt3 = tabMnt[i-1][j];

tableauR1 =0;
tableauR2 =0;
tableauS1 =0;
tableauS2 =0;

tableauR1 = tabR1(alt0, alt1, alt2, cPixel);
tableauR2 = tabR2(alt0, alt2, alt3, cPixel);

tableauS1 = tabS1(alt0, alt1, alt2, tableauR1, cPixel);
tableauS2 = tabS2(alt0, alt2, alt3, tableauR2, cPixel);

tableauS[i][j]= valeurMaxS(tableauS1,tableauS2,-1,-1,-1,-1,-1,-1);

//EN fonction du Smax on calcul R = R'------------

if (tableauS[i][j] == tableauS1)
{tableauR[i][j] = tableauR1;
 tabRg[i][j]= tableauR[i][j];}

else if (tableauS[i][j] == tableauS2)
{tableauR[i][j] = tableauR2;
 tabRg[i][j]= tableauR[i][j] +(PI/4);}

}

else if (i==ligne-1 && j==colonne-1)//******************************************
{
alt0 = tabMnt [i][j];
alt3 = tabMnt[i-1][j];
alt4 = tabMnt[i-1][j-1];
alt5 = tabMnt[i][j-1];

tableauR3 =0;
tableauR4 =0;
tableauS3 =0;
tableauS4 =0;

tableauR3 = tabR1(alt0, alt3, alt4, cPixel);
tableauR4 = tabR2(alt0, alt4, alt5, cPixel);

tableauS3 = tabS1(alt0, alt3, alt4, tableauR3, cPixel);
tableauS4 = tabS2(alt0, alt4, alt5, tableauR4, cPixel);

tableauS[i][j]= valeurMaxS(-1,-1,tableauS3,tableauS4,-1,-1,-1,-1);

//EN fonction du Smax on calcul R = R'------------

if (tableauS[i][j] == tableauS3)
{tableauR[i][j] = tableauR3;
 tabRg[i][j]= tableauR[i][j]+(PI/2);}

else if (tableauS[i][j] == tableauS4)
{tableauR[i][j] = tableauR4;
 tabRg[i][j]= tableauR[i][j]+(3*PI/4);}

}

//Calcul pour les bords----------------------------------------------------------------------
else if (i!=0 && j==0 || i!=ligne-1 && j==0)//**********************************************
{
alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt2 = tabMnt[i-1][j+1];
alt3 = tabMnt[i-1][j];
alt7 = tabMnt[i+1][j];
alt8 = tabMnt[i+1][j+1];

tableauR1 =0;
tableauR2 =0;
tableauR7 =0;
tableauR8 =0;
tableauS1 =0;
tableauS2 =0;
tableauS7 =0;
tableauS8 =0;

tableauR1 = tabR1(alt0, alt1, alt2, cPixel);
tableauR2 = tabR2(alt0, alt2, alt3, cPixel);
tableauR7 = tabR1(alt0, alt7, alt8, cPixel);
tableauR8 = tabR2(alt0, alt8, alt1, cPixel);

tableauS1 = tabS1(alt0, alt1, alt2, tableauR1, cPixel);
tableauS2 = tabS2(alt0, alt2, alt3, tableauR2, cPixel);
tableauS7 = tabS1(alt0, alt7, alt8, tableauR7, cPixel);
tableauS8 = tabS2(alt0, alt8, alt1, tableauR8, cPixel);

tableauS[i][j]= valeurMaxS(tableauS1,tableauS2,-1,-1,-1,-1,tableauS7,tableauS8);

//EN fonction du Smax on calcul R = R'------------

if (tableauS[i][j] == tableauS1)
{tableauR[i][j] = tableauR1;
 tabRg[i][j]= tableauR[i][j];}

else if (tableauS[i][j] == tableauS2)
{tableauR[i][j] = tableauR2;
 tabRg[i][j]= tableauR[i][j] +(PI/4);}

else if (tableauS[i][j] == tableauS7)
{tableauR[i][j] = tableauR7;
 tabRg[i][j]=  tableauR[i][j]+(6*PI/4);}

else if (tableauS[i][j] == tableauS8)
{tableauR[i][j] = tableauR8;
 tabRg[i][j]= tableauR[i][j]+(7*PI/4);}

}

else if (i!=0 && j==colonne-1 || i!=ligne-1 && j==colonne-1)//********************************
{
alt0 = tabMnt [i][j];
alt3 = tabMnt[i-1][j];
alt4 = tabMnt[i-1][j-1];
alt5 = tabMnt[i][j-1];
alt6 = tabMnt[i+1][j-1];
alt7 = tabMnt[i+1][j];

tableauR3 =0;
tableauR4 =0;
tableauR5 =0;
tableauR6 =0;
tableauS3 =0;
tableauS4 =0;
tableauS5 =0;
tableauS6 =0;

tableauR3 = tabR1(alt0, alt3, alt4, cPixel);
tableauR4 = tabR2(alt0, alt4, alt5, cPixel);
tableauR5 = tabR1(alt0, alt5, alt6, cPixel);
tableauR6 = tabR2(alt0, alt6, alt7, cPixel);

tableauS3 = tabS1(alt0, alt3, alt4, tableauR3, cPixel);
tableauS4 = tabS2(alt0, alt4, alt5, tableauR4, cPixel);
tableauS5 = tabS1(alt0, alt5, alt6, tableauR5, cPixel);
tableauS6 = tabS2(alt0, alt6, alt7, tableauR6, cPixel);

tableauS[i][j]= valeurMaxS(-1,-1,tableauS3,tableauS4,tableauS5,tableauS6,-1,-1);

//EN fonction du Smax on calcul R = R'------------

if (tableauS[i][j] == tableauS3)
{tableauR[i][j] = tableauR3;
 tabRg[i][j]= tableauR[i][j]+(PI/2);}

else if (tableauS[i][j] == tableauS4)
{tableauR[i][j] = tableauR4;
 tabRg[i][j]= tableauR[i][j]+(3*PI/4);}

else if (tableauS[i][j] == tableauS5)
{tableauR[i][j] = tableauR5;
 tabRg[i][j]=  tableauR[i][j]+PI;}

else if (tableauS[i][j] == tableauS6)
{tableauR[i][j] = tableauR6;
 tabRg[i][j]= tableauR[i][j]+(5*PI/2);}

}

else if (i==0 && j!=0 || i==0 && j!=colonne-1)//*****************************************************
{
alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt5 = tabMnt[i][j-1];
alt6 = tabMnt[i+1][j-1];
alt7 = tabMnt[i+1][j];
alt8 = tabMnt[i+1][j+1];

tableauR5 =0;
tableauR6 =0;
tableauR7 =0;
tableauR8 =0;
tableauS5 =0;
tableauS6 =0;
tableauS7 =0;
tableauS8 =0;

tableauR5 = tabR1(alt0, alt5, alt6, cPixel);//printf("%lf\n",tableauR5);
tableauR6 = tabR2(alt0, alt6, alt7, cPixel);//printf("%lf\n",tableauR6);
tableauR7 = tabR1(alt0, alt7, alt8, cPixel);//printf("%lf\n",tableauR7);
tableauR8 = tabR2(alt0, alt8, alt1, cPixel);//printf("%lf\n",tableauR8);

tableauS5 = tabS1(alt0, alt5, alt6, tableauR5, cPixel);//printf("%lf\n",tableauS5);
tableauS6 = tabS2(alt0, alt6, alt7, tableauR6, cPixel);//printf("%lf\n",tableauS6);
tableauS7 = tabS1(alt0, alt7, alt8, tableauR7, cPixel);//printf("%lf\n",tableauS7);
tableauS8 = tabS2(alt0, alt8, alt1, tableauR8, cPixel);//printf("%lf\n",tableauS8);


tableauS[i][j]= valeurMaxS(-1,-1,-1,-1,tableauS5,tableauS6,tableauS7,tableauS8);

//EN fonction du Smax on calcul R = R'------------

if (tableauS[i][j] == tableauS5)
{tableauR[i][j] = tableauR5;
 tabRg[i][j]=  tableauR[i][j]+PI;}

else if (tableauS[i][j] == tableauS6)
{tableauR[i][j] = tableauR6;
 tabRg[i][j]= tableauR[i][j]+(5*PI/2);}

else if (tableauS[i][j] == tableauS7)
{tableauR[i][j] = tableauR7;
 tabRg[i][j]=  tableauR[i][j]+(6*PI/4);}

else if (tableauS[i][j] == tableauS8)
{tableauR[i][j] = tableauR8;
 tabRg[i][j]= tableauR[i][j]+(7*PI/4);}

}

else if (i==ligne-1 && j!=0 || i==ligne-1 && j!=colonne-1)//***************************************
{
alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt2 = tabMnt[i-1][j+1];
alt3 = tabMnt[i-1][j];
alt4 = tabMnt[i-1][j-1];
alt5 = tabMnt[i][j-1];

tableauR1 =0;
tableauR2 =0;
tableauR3 =0;
tableauR4 =0;
tableauS1 =0;
tableauS2 =0;
tableauS3 =0;
tableauS4 =0;

tableauR1 = tabR1(alt0, alt1, alt2, cPixel);//printf("%lf\n",tableauR1);
tableauR2 = tabR2(alt0, alt2, alt3, cPixel);//printf("%lf\n",tableauR2);
tableauR3 = tabR1(alt0, alt3, alt4, cPixel);//printf("%lf\n",tableauR3);
tableauR4 = tabR2(alt0, alt4, alt5, cPixel);//printf("%lf\n",tableauR4);

tableauS1 = tabS1(alt0, alt1, alt2, tableauR1, cPixel);//printf("%lf\n",tableauS1);
tableauS2 = tabS2(alt0, alt2, alt3, tableauR2, cPixel);//printf("%lf\n",tableauS2);
tableauS3 = tabS1(alt0, alt3, alt4, tableauR3, cPixel);//printf("%lf\n",tableauS3);
tableauS4 = tabS2(alt0, alt4, alt5, tableauR4, cPixel);//printf("%lf\n",tableauS4);

tableauS[i][j]= valeurMaxS(tableauS1,tableauS2,tableauS3,tableauS4,-1,-1,-1,-1);

//EN fonction du Smax on calcul R = R'------------

if (tableauS[i][j] == tableauS1)
{tableauR[i][j] = tableauR1;
 tabRg[i][j]= tableauR[i][j];}

else if (tableauS[i][j] == tableauS2)
{tableauR[i][j] = tableauR2;
 tabRg[i][j]= tableauR[i][j] +(PI/4);}

else if (tableauS[i][j] == tableauS3)
{tableauR[i][j] = tableauR3;
 tabRg[i][j]= tableauR[i][j]+(PI/2);}

else if (tableauS[i][j] == tableauS4)
{tableauR[i][j] = tableauR4;
 tabRg[i][j]= tableauR[i][j]+(3*PI/4);}

}
}}



//Calcul dans la martice---------------------------------------------------------------
//*********************************************************************************************************
for (i=1; i<ligne-1;i++)
  {
      for (j=1; j <colonne-1;j++ )
  {


alt0 = tabMnt [i][j];
alt1 = tabMnt[i][j+1];
alt2 = tabMnt[i-1][j+1];
alt3 = tabMnt[i-1][j];
alt4 = tabMnt[i-1][j-1];
alt5 = tabMnt[i][j-1];
alt6 = tabMnt[i+1][j-1];
alt7 = tabMnt[i+1][j];
alt8 = tabMnt[i+1][j+1];
//printf("alt0=%lf\n",alt0 );
//printf("alt1=%lf\n",alt1 );
//printf("alt2=%lf\n",alt2);
//printf("alt3=%lf\n",alt3);
//printf("alt4=%lf\n",alt4 );
//printf("alt5=%lf\n",alt5 );
//printf("alt6=%lf\n",alt6 );
//printf("alt7=%lf\n",alt7 );
//printf("alt8=%lf\n",alt8 );


tableauR1 =0;
tableauR2 =0;
tableauR3 =0;
tableauR4 =0;
tableauR5 =0;
tableauR6 =0;
tableauR7 =0;
tableauR8 =0;
tableauS1 =0;
tableauS2 =0;
tableauS3 =0;
tableauS4 =0;
tableauS5 =0;
tableauS6 =0;
tableauS7 =0;
tableauS8 =0;

tableauR1 = tabR1(alt0, alt1, alt2, cPixel);//printf("%lf\n",tableauR1);
tableauR2 = tabR2(alt0, alt2, alt3, cPixel);//printf("%lf\n",tableauR2);
tableauR3 = tabR1(alt0, alt3, alt4, cPixel);//printf("%lf\n",tableauR3);
tableauR4 = tabR2(alt0, alt4, alt5, cPixel);//printf("%lf\n",tableauR4);
tableauR5 = tabR1(alt0, alt5, alt6, cPixel);//printf("%lf\n",tableauR5);
tableauR6 = tabR2(alt0, alt6, alt7, cPixel);//printf("%lf\n",tableauR6);
tableauR7 = tabR1(alt0, alt7, alt8, cPixel);//printf("%lf\n",tableauR7);
tableauR8 = tabR2(alt0, alt8, alt1, cPixel);//printf("%lf\n",tableauR8);



tableauS1 = tabS1(alt0, alt1, alt2, tableauR1, cPixel);//printf("%lf\n",tableauS1);
tableauS2 = tabS2(alt0, alt2, alt3, tableauR2, cPixel);//printf("%lf\n",tableauS2);
tableauS3 = tabS1(alt0, alt3, alt4, tableauR3, cPixel);//printf("%lf\n",tableauS3);
tableauS4 = tabS2(alt0, alt4, alt5, tableauR4, cPixel);//printf("%lf\n",tableauS4);
tableauS5 = tabS1(alt0, alt5, alt6, tableauR5, cPixel);//printf("%lf\n",tableauS5);
tableauS6 = tabS2(alt0, alt6, alt7, tableauR6, cPixel);//printf("%lf\n",tableauS6);
tableauS7 = tabS1(alt0, alt7, alt8, tableauR7, cPixel);//printf("%lf\n",tableauS7);
tableauS8 = tabS2(alt0, alt8, alt1, tableauR8, cPixel);//printf("%lf\n",tableauS8);

//printf("%lf %lf %lf %lf %lf %lf %lf %lf\n",tableauS1,tableauS2,tableauS3,
//tableauS4,tableauS5,tableauS6,tableauS7,tableauS8);

tableauS[i][j]= valeurMaxS(tableauS1,tableauS2,tableauS3,tableauS4,
                tableauS5,tableauS6,tableauS7,tableauS8);
//printf("%lf\n",tableauS[i][j]);

// Vérification des valeurs de mntsans puits------------------------------------------------

    if (alt0<alt1 && alt0<alt2 && alt0<alt3 && alt0<alt4 && alt0<alt5 &&
    alt0<alt6 && alt0<alt7 && alt0<alt8)
    {
              printf("probleme dans le traitement des puits2\n");
    }

//EN fonction du Smax on calcul Rg = R------------

if (tableauS[i][j] == -1)
{tabRg[i][j]= fabs(tableauR1+ tableauR2+ tableauR3+ tableauR4+ tableauR5+
                tableauR6+ tableauR7+ tableauR8)/8;}

else if (tableauS[i][j] == tableauS1)
{tableauR[i][j] = tableauR1;
 tabRg[i][j]= tableauR[i][j];
 }

else if (tableauS[i][j] == tableauS2)
{tableauR[i][j] = tableauR2;
 tabRg[i][j]= tableauR[i][j] +(PI/4);}

else if (tableauS[i][j] == tableauS3)
{tableauR[i][j] = tableauR3;
 tabRg[i][j]= tableauR[i][j]+(PI/2);}

else if (tableauS[i][j] == tableauS4)
{tableauR[i][j] = tableauR4;
 tabRg[i][j]= tableauR[i][j]+(3*PI/4);}

else if (tableauS[i][j] == tableauS5)
{tableauR[i][j] = tableauR5;
 tabRg[i][j]=  tableauR[i][j]+PI;}

else if (tableauS[i][j] == tableauS6)
{tableauR[i][j] = tableauR6;
 tabRg[i][j]= tableauR[i][j]+(5*PI/4);}

else if (tableauS[i][j] == tableauS7)
{tableauR[i][j] = tableauR7;
 tabRg[i][j]=  tableauR[i][j]+(6*PI/4);}

else if (tableauS[i][j] == tableauS8)
{tableauR[i][j] = tableauR8;
 tabRg[i][j]= tableauR[i][j]+(7*PI/4);}





//Fin de la boucleDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD
}}

      for (i=0; i<ligne;i++)
      for (j=0; j <colonne;j++ )
            {
                 tabPente[i][j]=tableauS[i][j]*180/PI;
                 if (tabPente[i][j]<0) tabPente[i][j]=0;
                 if (tabRg[i][j]<0) tabRg[i][j]=-9999;
            }

//tab2DToFileMatrix (tabPente, "Geol/Pente.asc", ligne, colonne);
//tab2DToFileMatrix (tabRg, "Geol/Rg.asc", ligne, colonne);


//Fermeture des tableaux-------------------------------------------------------------------------------
// on libère tableauS de la même manière qu'on l'a construit************
    tab2DDesAlloc(tableauS, ligne); //Désallocation
    tab2DDesAlloc(tableauR, ligne);



}





// Les valeurs qui arrivent ici sont que des valeurs de pixels qui ont un gradient vers le bas
float DirFctDalt  (float **dirD8, float **dAlti, float val1,float val2,
                    float val3, float val4, float val5,float val6,
                    float val7, float val8, int i, int j)

{
//Si deux valeurs sont égales se sera la premiere valeurs dans le sens inverse des aiguilles d'une montre


if (val1>=val2 && val1>=val3 && val1>=val4 && val1>=val5 && val1>=val6
            && val1>=val7 && val1>=val8)
    {
        dirD8[i][j] = 0;
        dAlti[i][j] = val1;
    }

else if (val2>=val1 && val2>=val3 && val2>=val4 && val2>=val5 && val2>=val6
        && val2>=val7 && val2>=val8)
    {
        dirD8[i][j] =  45;
        dAlti[i][j] = val2;
    }

else if (val3>=val2 && val3>=val1 && val3>=val4 && val3>=val5 && val3>=val6
            && val3>=val7 && val3>=val8)
    {
        dirD8[i][j] =  90;
        dAlti[i][j] = val3;
    }

else if (val4>=val2 && val4>=val3 && val4>=val1 && val4>=val5 && val4>=val6
            && val4>=val7 && val4>=val8)
    {
        dirD8[i][j] =  135;
        dAlti[i][j] = val4;
    }

else if (val5>=val2 && val5>=val3 && val5>=val4 && val5>=val1 && val5>=val6
            && val5>=val7 && val5>=val8)
    {
        dirD8[i][j] =  180;
        dAlti[i][j] = val5;
    }

else if (val6>=val2 && val6>=val3 && val6>=val4 && val6>=val5 && val6>=val1
            && val6>=val7 && val6>=val8)
    {
        dirD8[i][j] =  225;
        dAlti[i][j] = val6;
    }

else if (val7>=val2 && val7>=val3 && val7>=val4 && val7>=val5 && val7>=val6
            && val7>=val1 && val7>=val8)
    {
        dirD8[i][j] =  270;
        dAlti[i][j] = val7;
    }

else if (val8>=val2 && val8>=val3 && val8>=val4 && val8>=val5 && val8>=val6
            && val8>=val7 && val8>=val1)
    {
        dirD8[i][j] =  315;
        dAlti[i][j] = val8;
    }


}
