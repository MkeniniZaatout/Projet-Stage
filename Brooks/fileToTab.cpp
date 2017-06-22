#include <iostream>
#include <cstdlib>
#include <fstream>
#include<vector>
#include <string>
#include <math.h>
#include "header.h"

using namespace std;

vector<string> ReadLines(string nomFichier, int nbLignes)
{
    int i=0;
    string ligne;
    vector<string> retour(nbLignes);

    ifstream fichier(nomFichier.c_str());
    while(!fichier.eof())
    {
        fichier >> ligne;
        if(i<nbLignes)
        {
            retour[i] = ligne;
        }
        i++;
    }

    return retour;
}

void FileToTabInt1D(int* tab, string nomFichier, int taille)
{
    int val;
    int i=0;

    ifstream fichier(nomFichier.c_str());

    if(!fichier)
    {
        cerr << "Erreur : Ouverture de " << nomFichier << " impossible !" << endl;
        exit(1);
    }
    else
    {
        while(!fichier.eof())
        {
            fichier >> val;
            if(i<taille)
            {
                tab[i] = val;
            }
            i++;
        }
    }
}

void FileToTabfloat1D(float* tab, string nomFichier, int taille)
{
    float val;
    int i=0;

    ifstream fichier(nomFichier.c_str());

    if(!fichier)
    {
        cerr << "Erreur : Ouverture de " << nomFichier << " impossible !" << endl;
        exit(1);
    }
    else
    {
        while(!fichier.eof())
        {
            fichier >> val;
            if(i<taille)
            {
                tab[i] = val;
            }
            i++;
        }
    }
}

void fileToTab1D (float **tab, int nbLignes, string nom)
{
    float val = 0;
    ifstream file (nom.c_str());

    if(!file)
    {
        cerr << nom << " : Ouverture du fichier impossible." << endl;
        exit(1);
    }

    int i=0;

    while (!file.eof())
    {
        file >> val;
        tab[0][i] = val;
        i++;
    }

    file.close ();
}

void fileToTab2D (float **tab, string nom, int size_X, int size_Y)
{

int i=0, j=0, k=0;

float *tab1D = new float [size_X*size_Y];

ifstream file (nom.c_str());
    if(!file)
        {
            cerr<<"Ouverture du fichier impossible de "<< nom <<endl;
            exit(1);
        }

      while (!file.eof ())
      {
        char ligne[256];
        file.getline (ligne, sizeof (ligne));
        tab1D[i] = atof(ligne);
        i++;
      }
  file.close ();

// Passage de tab1D à tab
    	k =0;
    	for (i=0; i<size_X ; i++)
    	{
    		for (j=0; j<size_Y ; j++)
    		{
    			tab[i][j]=  tab1D [k];
    			k++;
    		}
    	}

}

void fileMatrixToTab2D (float **tab, string nom, int size_X, int size_Y)
{
float val = 0;

int i=0, j=0, k=0;

float tab1D[size_X*size_Y];

    ifstream file (nom.c_str());
        if(!file)
            {
                cerr<<"Ouverture du fichier impossible de "<< nom <<endl;
                exit(1);
            }

while (!file.eof())
{
    file>>val;
    // MNT -> 3 chiffres apres la virgule
    if(nom == "/MNT.asc" || nom == "/Rg.asc"){
       val = roundf(val*1000)/ 1000.0f;
    // Pente -> 2 chiffres apres la virgule
    }else if(nom == "/Pente.asc") {
       val = roundf(val*100)/ 100.0f;
    // Pente -> 3 chiffres apres la virgule
    }
    // file>>val;
    tab1D[i] = val;
    i++;
}
    file.close ();

    cout << size_X <<" "<< size_Y << endl;
    cout << "ligne x colonne : " << size_X*size_Y << endl;
    cout << " i : " << i << endl;

    // Passage de tab1D à tab
        k =0;

        for (i=0; i<size_X ; i++)
        {
            for (j=0; j<size_Y ; j++)
            {
                tab[i][j]=  tab1D [k];
                k++;
            }
        }
    cout << " Fin derniere boucle " << endl;
}

void tab2DToFileMatrix (float **tab, string nom, int ligne, int colonne)
{
    ofstream fichier(nom.c_str());
/*
    Afin d'ecrire sur le fichier de matrice Hw et Fs

    if (nom.find("Sortie/HW/") != std::string::npos) {

    }
    else if(nom.find("Sortie/FS") != std::string::npos){

    }
*/
    for(int i=0; i<ligne; i++)
    {
        for (int j=0; j<colonne; j++)
        {
            fichier << tab[i][j];
            if(j!=colonne-1) fichier << "\t";
        }
        if(i!=ligne-1) fichier << "\n";
    }

    fichier.close();
}
