#include <iostream>
#include <cstdlib>
#include <fstream>
#include<vector>
#include <string>
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
/* switch (nom) {
        case "/MNT.asc": std::cout << "1";
        case "/Pente.asc": std::cout << "2";   //execution starts at this case label
        case "/Rg.asc": std::cout << "3";
        case "":
        case 5: std::cout << "45";
                break;              //execution of subsequent statements is terminated
        case 6: std::cout << "6";
    } */
    file>>val;
    tab1D[i] = val;
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
