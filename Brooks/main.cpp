#include "header.h"
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <stdio.h>      /* printf */
#include <stdlib.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
double getTime() {
  LARGE_INTEGER freq, val;
  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&val);
  return (double)val.QuadPart / (double)freq.QuadPart;
}

int main()
{
    cout << "Bonjour !" << endl;

    /***************************************************
        Configuration
    ***************************************************/

    cout << "Configuration" << endl;

    //--- Configuration Fichiers
    //
    ifstream fichierConfig("brooks.conf");

    string racine;
    string nomZone;
    string nomScenar;

    getline(fichierConfig, racine);
    getline(fichierConfig, nomZone);
    getline(fichierConfig, nomScenar);

    fichierConfig.close();

    cout <<"racine : "+racine << endl;
    cout << "nom zone : "+nomZone << endl;
    cout << "nom scenario : "+nomScenar << endl;
    //--- Configuration Zone
    //
    string cheminZonolp = racine + "/" + nomZone + "/" + nomZone + ".zonolp";
    ifstream fichierZone(cheminZonolp.c_str());

    int ligne;
    int colonne;
    float cPixel;
    int nbGeol;

    fichierZone >> ligne;
    fichierZone >> colonne;
    fichierZone >> cPixel;
    fichierZone >> nbGeol;
    fichierZone.close();

    cout << "Lecture des proprietes de la zone" << endl;
    //--- Configuration Scénario
    //
    string cheminScenolp = racine + "/" + nomZone + "/" + nomScenar + "/" + nomScenar + ".scenolp";
    ifstream fichierScenar(cheminScenolp.c_str());

    /*
    cout << "cheminZonolp : "+cheminZonolp<< endl;
    cout << "cheminScenolp : "+cheminScenolp << endl;
    */
    int nbrCase;
    float hauteurCase;    // --- (en cm)
    int tpsCalcul;        // --- Temps de calcul
    float k;              // --- Facteur d'accélération sismique

    fichierScenar >> tpsCalcul;
    fichierScenar >> nbrCase;
    fichierScenar >> hauteurCase;
    fichierScenar >> k;
    fichierScenar.close();

    cout << "Lecture des proprietes du scenario" << endl;
    // --- Autres variables
    //
    int subst = nbrCase;
    char nom[20];
    char nom2[20];
    float H = (nbrCase * hauteurCase)/100; // --- Epaisseur de la formation superficielle (en m)

    /***************************************************
        Création des matrices
    ***************************************************/

    cout << "Creation des matrices" << endl;


    float** tabMnt = NULL;
    float** tabPente = NULL;
    float** tabRg = NULL;
    float** tabMasque = NULL;
    tabMnt = tab2DAlloc(ligne, colonne);
    tabPente = tab2DAlloc(206, colonne);
    tabRg = tab2DAlloc(ligne, colonne);
    tabMasque = tab2DAlloc(ligne, colonne);


    float** tabPluie = NULL;
    float** tabHw = NULL;
    float** tetaMax = NULL;
    float** tetaMin = NULL;
    float** kSat = NULL;
    float** n = NULL;
    float** tetaMaxSub = NULL;
    float** tetaMinSub = NULL;
    float** kSatSub = NULL;
    float** nSub = NULL;
    float** tetaHwMax = NULL;
    float** tetaHwMin = NULL;
    float** tetaHwMaxSub = NULL;
    float** pluieMax = NULL;
    float** rTab = NULL;
    float** tabPhi = NULL;
    float** tabC = NULL;
    float** tabGamaSec = NULL;
    float** tabGamaHum = NULL;
    float** tabFs = NULL;
    float** tabAtempFS = NULL;
    float** tabAtempHW = NULL;
    float** tabTimeFS = NULL;
    float** tabTimeHW = NULL;

    float** valeursGeol = NULL;
    float** litho = NULL;

    tabPluie = tab2DAlloc(ligne, colonne);
    tabHw = tab2DAlloc(ligne, colonne);
    tetaMax = tab2DAlloc(ligne, colonne);
    tetaMin = tab2DAlloc(ligne, colonne);
    kSat = tab2DAlloc(ligne, colonne);
    n = tab2DAlloc(ligne, colonne);
    tetaMaxSub = tab2DAlloc(ligne, colonne);
    tetaMinSub = tab2DAlloc(ligne, colonne);
    kSatSub = tab2DAlloc(ligne, colonne);
    nSub = tab2DAlloc(ligne, colonne);
    tetaHwMax = tab2DAlloc(ligne, colonne);
    tetaHwMin = tab2DAlloc(ligne, colonne);
    tetaHwMaxSub = tab2DAlloc(ligne, colonne);
    pluieMax = tab2DAlloc(ligne, colonne);
    rTab = tab2DAlloc(ligne, colonne);
    tabPhi = tab2DAlloc(ligne, colonne);
    tabC = tab2DAlloc(ligne, colonne);
    tabGamaSec = tab2DAlloc(ligne, colonne);
    tabGamaHum = tab2DAlloc(ligne, colonne);
    tabFs = tab2DAlloc(ligne, colonne);
    tabAtempFS = tab2DAlloc(ligne, colonne);
    tabAtempHW = tab2DAlloc(ligne, colonne);
    tabTimeFS = tab2DAlloc(ligne, colonne);
    tabTimeHW = tab2DAlloc(ligne, colonne);

    valeursGeol = tab2DAlloc(ligne, colonne);
    litho = tab2DAlloc(ligne, colonne);

    /***************************************************
        Création des tenseurs
    ***************************************************/

    cout << "Creation des tenseurs" << endl;

    float*** tabTeta = NULL;
    float*** tabquZ = NULL;
    float*** tabTetaHw = NULL;
    float*** tabquOut = NULL;
    float*** tabquIn = NULL;

    cout << "nbreCase : "<< endl;
    cout << nbrCase << endl;
    tabTeta = tab3DAlloc(ligne, colonne, nbrCase+1);
    tabquZ = tab3DAlloc(ligne, colonne, nbrCase+1);
    tabquIn = tab3DAlloc(ligne, colonne, nbrCase+1);
    tabquOut = tab3DAlloc(ligne, colonne, nbrCase+1);
    tabTetaHw = tab3DAlloc(ligne, colonne, nbrCase+1);

// ICI
    string dossierZone = racine + "/" + nomZone;
    /***************************************************
        Lecture des fichiers géol
    ***************************************************/

    cout << "Lecture des fichiers Zone" << endl;
    fileMatrixToTab2D(tabMnt,dossierZone,"/MNT.asc",ligne, colonne);
    cout << "- MNT" << endl;
    fileMatrixToTab2D(tabPente, dossierZone,"/Pente.asc",206, colonne);
    cout << "- Pente" << endl;
    fileMatrixToTab2D(tabRg,dossierZone,"/Rg.asc", ligne, colonne);
    cout << "- Rg" << endl;
    fileMatrixToTab2D(litho,dossierZone, "/Litho.asc", ligne, colonne);
    cout << "- Litho" << endl;
    fileMatrixToTab2D(tabMasque,dossierZone, "/Masque.asc", ligne, colonne);
    cout << "- Masque" << endl;
//
    cout << "Lecture des fichiers Scenario :" << endl;

    string dossierScenar = dossierZone + "/" + nomScenar + "/";
    cout << "- Valeurs de ruissellement " << endl;
    fileMatrixToTab2D(rTab,dossierScenar, "Valeurs/valeurs-R.asc",   ligne, colonne);

    cout << "- Valeurs lithologiques " << endl;
    fileMatrixToTab2D(valeursGeol,dossierScenar, "Valeurs/valeurs-Litho.asc", ligne, colonne);
    // cout << dossierScenar + "Valeurs/valeurs-Litho.asc" << endl;

    // nbGeol, 13

    cout << "Remplissage des matrices" << endl;
    // cout << "litho :" << litho[0][0] << " mnt : " <<tabMnt[0][0] << " Rg : " << tabRg[0][0] << " Pente : " << tabPente[0][0] << endl;

    for(int i=0; i<ligne; i++)
    {
        for(int j=0; j<colonne; j++)
        {
            int form = litho[i][j];
            if(form > 0){
            tabC[i][j] = valeursGeol[form-1][0];
            tabPhi[i][j] = valeursGeol[form-1][1];
            tabGamaSec[i][j] = valeursGeol[form-1][2];
            tabGamaHum[i][j] = valeursGeol[form-1][3];
            kSat[i][j] = valeursGeol[form-1][4];
            kSatSub[i][j] = valeursGeol[form-1][5];
            n[i][j] = valeursGeol[form-1][6];
            nSub[i][j] = valeursGeol[form-1][7];
            tetaMin[i][j] = valeursGeol[form-1][8];
            tetaMax[i][j] = valeursGeol[form-1][9];
            tetaMinSub[i][j] = valeursGeol[form-1][10];
            tetaMaxSub[i][j] = valeursGeol[form-1][11];
            }else{
            std::cerr << "Erreur fichier litho : Valeur nul ou negative\n";
            system("PAUSE");
            return 0;
            }
        }
    }
/*
    for(int i=0; i<ligne; i++)
    {
        for(int j=0; j<colonne; j++)
        {
            int form = litho[i][j];
            float angleMax = valeursGeol[form-1][12];
            if(tabPente[i][j] > angleMax || tabMasque[i][j] == 0)
                cout << angleMax << endl;
            tabC[i][j] = 200;
        }
    }
*/
    /***************************************************
        Lecture du fichier de pluie
    ***************************************************/

    //--- Configuration Zone
    //
    // cout << tabMnt[0][0] << endl;
    string cheminPropPluie = dossierScenar + "Pluies/Pluvio-Prop.asc";

    // cout << cheminPropPluie << endl;
    ifstream fichierPropPluies(cheminPropPluie.c_str());

    int nbreMinPluie;
    fichierPropPluies >> nbreMinPluie;
    fichierPropPluies.close();
    float** pluies = NULL;

    pluies = tab2DAlloc(1, 7500);
    // pluies = tab2DAlloc(nbreMinPluie, 1);

    //fileToTab1D (pluies, 7500, dossierScenar + "Pluies/Pluvio.asc");
    fileMatrixToTab2D(pluies, dossierScenar ,"Pluies/Pluvio.asc", 1, 7500);
    cout << "Lecture du fichier de pluie" << endl;
    /*****************************************************************
        Calcul des limites de chaque case
    *****************************************************************/

    cout << "Debut du Calcul hydrogeologique" << endl;

    for (int a = 0; a < ligne; a++)
    {
        for (int b = 0; b < colonne; b++)
        {
            tetaHwMax[a][b] = tetaMax[a][b] * hauteurCase;       // --- Formation superficielle
            tetaHwMin[a][b] = tetaMin[a][b] * hauteurCase;       // --- Formation superficielle
            tetaHwMaxSub[a][b] = tetaMaxSub[a][b] * hauteurCase; // --- Substratum

            pluieMax[a][b] = (tetaMax[a][b] - tetaMin[a][b]) * hauteurCase;
        }
    }

    // --- Transformation des pentes en degrés en radian
    //
    for (int a = 0; a < ligne; a++)
    {
        for (int b = 0; b < colonne; b++)
        {
            tabPente[a][b] = tabPente[a][b] * DEGRE;
        }
    }

    /*****************************************************************
        Initialisation des colonnes
    *****************************************************************/
    cout << "Initialisation des colonnes" << endl;
    for (int a = 0; a < ligne; a++)
    {
        for (int b = 0; b < colonne; b++)
        {
            // --- Teta : teneur en eau
            //
            for (int i = 0; i < nbrCase; i++)
            {
                tabTeta[a][b][i] = tetaMin[a][b];   // --- Formation superficielle
            }

            tabTeta[a][b][subst] = tetaMinSub[a][b]; // --- Substratum

            // --- Calcul de HW dans chaque case
            //
            for (int i = 0; i < nbrCase; i++)
            {
                tabTetaHw[a][b][i] = tabTeta[a][b][i] * hauteurCase; // --- Formation superficielle
            }

            tabTetaHw[a][b][subst] = tabTeta[a][b][nbrCase] * hauteurCase; // --- Substratum

            tabAtempFS[a][b] = 10000;
        }
    }

    /*****************************************************************
        Boucle de calcul
    *****************************************************************/
    cout << "Stockage des FS et des HW en cours ...\n";
    for (int t = 0; t < tpsCalcul ; t++)
    {
        if (t < nbreMinPluie)
        {
            for (int a = 0; a < ligne; a++)
            for (int b = 0; b < colonne; b++)
            tabPluie[a][b] = pluies[0][t];
        }

        else
        {
            for (int a = 0; a < ligne; a++)
            for (int b = 0; b < colonne; b++)
                tabPluie[a][b] = 0;
        }

        // --- Calcul du flux vertical en fonction de la teneur en eau
        //

        quBrooks (tabquZ, tabTeta, tetaMax, tetaMaxSub, tetaMin, tetaMinSub, n, nSub, kSat, kSatSub, ligne, colonne, subst);

        // --- Calcul du tenseur des flux latéraux quOut
        //

        quOut (tabquOut, tabTetaHw, tabRg, tabPente, kSat, tetaHwMax, ligne, colonne, subst);

        // --- Calcul des flux reçus
        //

        quIn(tabquIn, tabquOut, tabTetaHw, tabRg, tabPente, kSat, tetaHwMax, ligne, colonne, subst);

        // --- Calcul du nouveau HW
        //

        // float toto = tabMnt[0][0];
        // cout << toto << endl;

        for (int a = 0; a < ligne; a++)
        {
            for (int b = 0; b < colonne; b++)
            {

                if(tabMnt[a][b]>1)
                {
                    tetaHw (tabTetaHw, tabquZ,tabquIn, tabquOut, tabPluie, tetaHwMaxSub, pluieMax, kSat, subst, subst, rTab, t, nbreMinPluie, a, b);

                    for (int i = nbrCase-1; i>-1; i--)
                    {
                        tetaHw (tabTetaHw, tabquZ,tabquIn, tabquOut, tabPluie, tetaHwMax, pluieMax, kSat, subst, i, rTab, t, nbreMinPluie, a, b);
                    }
                }
            }
        }

        // --- Calcul du nouveau teta
        //

        for (int a = 0; a < ligne; a++)
        {
            for (int b = 0; b < colonne; b++)
            {
                if(tabMnt[a][b]>1)
                {
                    for (int i = 0; i < nbrCase; i++) // --- Formation superficielle
                    {
                        tabTeta[a][b][i] = teta (tabTetaHw, hauteurCase, i, a, b);
                    }

                    tabTeta[a][b][subst] = teta (tabTetaHw, hauteurCase, subst, a, b); // --- Substratum
                }
            }
        }

        // --- Calcul de HW
        //

        for (int a = 0; a < ligne; a++)
        {
            for (int b = 0; b < colonne; b++)
            {
                if(tabMnt[a][b]!=-9999)
                {
                    tabHw[a][b] = hwTest(tabTetaHw, tetaHwMax, tetaHwMin, hauteurCase, nbrCase, a, b);

                    tabFs[a][b] = (  tabC[a][b] + ((   (tabGamaSec[a][b]*H) - (tabGamaSec[a][b] + 10 - tabGamaHum[a][b])*(tabHw[a][b]/100))*((cos((2*tabPente[a][b]))+1)/2)*tan(tabPhi[a][b]*DEGRE))  )
                                    /
                                  ( ((tabGamaSec[a][b]*(H-tabHw[a][b]/100))+ (tabGamaHum[a][b]*(tabHw[a][b]/100)))  * ( sin(tabPente[a][b]) + k ) * cos(tabPente[a][b])          );

                // system("PAUSE");
                // cout << tabFs[a][b] << endl;
                /*
                cout << "tabC : " << tabC[1][0]  << endl;
                cout << "tabGamaSec : " << tabGamaSec[1][0]  << endl;
                cout << "tabGamaHum : " << tabGamaHum[1][0]  << endl;
                cout << "tabPente : " << tabPente[1][0]  << endl;

                cout << " tabC "<< tabC[a][b] << endl;
                cout << "tabGamaSec " << tabGamaSec[a][b] <<endl;
                cout << "tabGamaHum " << tabGamaHum[a][b]<<endl;
                cout << "tabHw " << tabHw[a][b] <<endl;
                cout << "tabPhi " << tabPhi[a][b] <<endl;
                */
                }
            }
        }

        // system("PAUSE");
        // --- Stockage des FS et des HW
        //
        // cout << "Stockage des FS et des HW\n";
        sprintf(nom, "Sortie/HW/HW-%d.asc", t);
        sprintf(nom2, "Sortie/FS/FS-%d.asc", t);

        tab2DToFileMatrix (tabHw, dossierScenar + nom, ligne, colonne);
        tab2DToFileMatrix (tabFs, dossierScenar + nom2, ligne, colonne);

        AtempTimeFS(tabFs, tabAtempFS, tabTimeFS, t, ligne, colonne);
        AtempTimeHW(tabHw, tabAtempHW, tabTimeHW, t, ligne, colonne);

        cout << t << endl;
        // cout << ".";
    }

    /*****************************************************************
        Stockage des Atemp et Time dans des fichiers
    *****************************************************************/

    cout << "Generation des fichiers :" << endl;
    tab2DToFileMatrix (tabAtempFS, dossierScenar+"Sortie/FS/FS-Atemp.asc", ligne, colonne);
    cout << "FS-Atemp.asc" << endl;
    tab2DToFileMatrix (tabTimeFS, dossierScenar +"Sortie/FS/FS-Time.asc", ligne, colonne);
    cout << "FS-Time.asc" << endl;
    tab2DToFileMatrix (tabAtempHW, dossierScenar +"Sortie/HW/HW-Atemp.asc", ligne, colonne);
    cout << "HW-Atemp.asc" << endl;
    tab2DToFileMatrix (tabTimeHW, dossierScenar +"Sortie/HW/HW-Time.asc", ligne, colonne);
    cout << "HW-Time.asc" << endl;

    /*****************************************************************
        Libération de la mémoire
    *****************************************************************/
    cout << "Programme getime : " << getTime() << endl;
    cout << "Liberation de la memoire" << endl;

    // tab2DDesAlloc(pluies, 1);
    tab2DDesAlloc(tabMnt, ligne);
    tab2DDesAlloc(tabRg, ligne);
    tab2DDesAlloc(tabPente, ligne);
    tab2DDesAlloc(tetaMax, ligne);
    tab2DDesAlloc(tetaMin, ligne);
    tab2DDesAlloc(tetaHwMax, ligne);
    tab2DDesAlloc(tetaHwMin, ligne);
    tab2DDesAlloc(tetaHwMaxSub, ligne);
    tab2DDesAlloc(pluieMax, ligne);
    tab2DDesAlloc(tetaMaxSub, ligne);
    tab2DDesAlloc(tetaMinSub, ligne);
    tab3DDesAlloc(tabTeta, ligne, colonne);
    tab3DDesAlloc(tabTetaHw, ligne, colonne);
    tab2DDesAlloc(tabMasque, ligne);

    tab2DDesAlloc(tabPluie, ligne);
    tab2DDesAlloc(tabHw, ligne);
    tab2DDesAlloc(kSat, ligne);
    tab2DDesAlloc(n, ligne);
    tab2DDesAlloc(kSatSub, ligne);
    tab2DDesAlloc(nSub, ligne);
    tab2DDesAlloc(rTab, ligne);
    tab2DDesAlloc(tabPhi, ligne);
    tab2DDesAlloc(tabC, ligne);
    tab2DDesAlloc(tabGamaSec, ligne);
    tab2DDesAlloc(tabGamaHum, ligne);
    tab2DDesAlloc(tabFs, ligne);
    tab3DDesAlloc(tabquZ, ligne, colonne);
    tab3DDesAlloc(tabquOut, ligne, colonne);
    tab3DDesAlloc(tabquIn, ligne, colonne);

    tab2DDesAlloc(tabAtempFS, ligne);
    tab2DDesAlloc(tabAtempHW, ligne);
    tab2DDesAlloc(tabTimeFS, ligne);
    tab2DDesAlloc(tabTimeHW, ligne);

    /****************************************************************/
    cout << "Programme getime : " << getTime() << endl;
    cout << "Au revoir !" << endl;

    system("PAUSE");

    return 0;
}
