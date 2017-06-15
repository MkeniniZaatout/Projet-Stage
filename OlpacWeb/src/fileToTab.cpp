#include <iostream>
#include <cstdlib>
#include <fstream>
#include <math.h>
#include "header.h"
using namespace std;


/*---------------------------------------------------------------------
    Fichier contenant toutes les fonctions permettant de strocker les valeures d'un fichier texte dans un tableau
    ainsi que tous les traitements intermédiaires
---------------------------------------------------------------------*/

/**---------------------------------------------------------------------
    Recupere les données du fichier passer en parametre possédant un en-tete de la manière suivante :
        - parametres reçoit le nom de chaque ligne de l'en-tete
        - values contiendra les valeurs associées
        - Une matrice de float sera retournée, elle contiendra les valeurs du fichier situées apres l'en-tete
    Entrée :
        - string* parametres : Tableau de string contenant le nom du parametre correspondant a la ligne i-1
        - float* values : Le tableau permettant de recuperer les valeurs des 5 premières lignes
        - char* nom : Un tableau de caractere contenant le chemin relatif du fichier
        - int nbParam : Nombre de lignes de l'en-tete

    Sortie : return data, qui est une matrice de float (float**) contenant les valeurs du fichier
---------------------------------------------------------------------**/
float** getData (QString* parametres,float* values, QString nom,int nbParam){

    float *tab1D = NULL;
    float** data = NULL;
    float val=0;
    int i=0, j=0, k=0, maxVal;

    QFile file(nom);

    if (file.open(QIODevice::ReadWrite | QIODevice::Text)){
        QTextStream in(&file);

         for(i=0;i<nbParam;i++){
            in >> parametres[i];
            in >> values[i];
         }

            //Values[0] correspond au nombre de colonnes et values[1] au nombre de lignes
            maxVal = values[0] * values[1];
            tab1D = new float[maxVal];
            data = tab2DAlloc(values[1],values[0]);
            qDebug() << "ligne : " << values[0];
            qDebug() << "colonne : " << values[1];
            printf("Lecture du fichier\n");
            i=0;
            while (i < maxVal)
            {
                in >> val;
                tab1D[i] = val;
                i++;
                if(i%1000000 == 0)
                    cout << i << " elements traitees" << endl << endl;
            }
            printf("Lecture terminee \n");
            file.close();

            printf("Stockage des donnees dans une matrice\n");

            // Passage de tab1D à data
                k =0;
                for (i=0; i<values[1] ; i++)
                {
                    for (j=0; j<values[0] ; j++)
                    {
                        data[i][j]=  roundf(tab1D[k]*100)/ 100.0f;
                        // data[i][j]=  tab1D [k];
                        k++;

                    }
                }
    }

                return data;
}

void getEntete(QString* parametres,float* values, QString nom,int nbParam){

        QFile file(nom);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream in(&file);
        for(int i=0;i<nbParam;i++){
            in >> parametres[i];
            in >> values[i];
        }
        }
}

/**---------------------------------------------------------------------
    Cette fonction sert a convertir les données d'un tableau en leurs classe correspondante
    Par exemple si la valeur 2 correspond a la classe 5, tous les 2 seront convertis en 5.

    Entrée :
        - float** classes : Une matrice contenant les classes ainsi que les valeurs associées, la ligne correspond au numero de la classe et chaque colonne contiendra une des valeurs associées.
          Exemple : Les coordonées [2][4] , corrspondent a la classe 3  (i+1) et a la valeurs stocké dans la colonne 4.
        - float** data : La matrice corrspondant aux valeurs du fichier a traiter
        - int ligne : Le nombre de lignes de data
        - int colonne : Le nombre de colonnes de data
        - float* classDim : Un tableau de 2 parametres contenant les dimensions de la matrice classes.

    Sortie : float** resultat, la matrice correspondant aux valeurs traitées.
---------------------------------------------------------------------**/

float** traitementClass(float** classes,float** data,int ligne,int colonne,float* classDim){

    float val;
    float ** resultat = data;

    for(int i=0;i<ligne;i++){
        for(int j=0;j<colonne;j++){
            val = resultat[i][j];
            if(-1 != getClass(val,classes,classDim))    //Si la valeur correspondante a une classe associée, on lui affecte cette classe.
                resultat[i][j] = getClass(val,classes,classDim);
        }
    }

    return resultat;

}

/**---------------------------------------------------------------------
    Fonction retournant la classe associée a la valeur passée en parametre

    Entrée:
        - float val : Un float correspondant a la valeur a convertir
        - float** classes : La matrice des classes
        - float* classDim : Les dimensions de la matrice classes

    Sortie :
        - Un float correspondant a la classe associée a val.
        - return -1 si la valeur passée en parametre n'est pas dans la matrice de classe.
---------------------------------------------------------------------**/

float getClass(float val,float** classes,float* classDim){
    int ligne = classDim[1];
    int colonne = classDim[0];

    for(int i=0;i<ligne;i++){
        for(int j=0;j<colonne;j++){
            if(val == classes[i][j])
                return i+1;
        }
    }

    return -1;

}

/**---------------------------------------------------------------------
    Fonction permettant de convertir une coordonée en un int, ce qui nous donne acces a l'emplacement correpondant dans
    la matrice de valeurs associées au fichier lut.

    Entrée:
        - float xllcorner : Coordonée a l'emplacement [0][0], sur l'axe associé.
        - float x : La coordonée a convertir.
        - float cellSize : L'ecart entre deux cases.

    Sortie :
        - return resultat + 1, un int correspondant a l'adresse associée dans la matrice ( ligne ou colonne)
---------------------------------------------------------------------**/

int getCoordX(float xllcorner,float x,float cellSize){

    int resultat = 0;
    resultat = x - xllcorner;
    resultat = resultat / cellSize;
    return resultat + 1;

}

int getCoordY(float yllcorner,float y,float cellSize,float nbLigne){

    int resultat = 0;
    resultat = y - yllcorner;
    resultat = resultat / cellSize;
    resultat = nbLigne - resultat;
    return resultat + 1;
}

/**---------------------------------------------------------------------
    Fonction retournant une sous matrice de données, extraite dans une matrice.

    Entrée:
        - int xMin : La ligne de depart
        - int xMax : La ligne d'arrivée
        - int yMin : La colonne de depart
        - int yMax : La colonne d'arrivée
        - float** tab : Le tableau a traiter

    Sortie :
        - float** result, la sous matrice extraite de tab.
---------------------------------------------------------------------**/
float** extraireTab(int xMin,int xMax,int yMin, int yMax,float** tab){

    int ligne = (yMin - yMax)+1;
    int colonne = (xMax - xMin);
    int x=0,y=0;
    int debI,debJ;
    float** result = NULL;

    if(xMin <= 0)
        debJ = 1;
    else
        debJ = xMin -1;

    if(yMax <= 0)
        debI = 1;
    else
        debI = yMax -2;

    result = tab2DAlloc(ligne,colonne);

    qDebug() << "Extraire donnée" << ligne << colonne;
    qDebug() << xMin << xMax ;
    qDebug() << yMin << yMax;

    for(int i = debI; i < yMin-1; i++){
        for(int j = debJ ;j< xMax-1 ;j++){
            qDebug() << "i" << i << "j" << j;
            result[x][y] = tab[i][j];
            y++;
        }
        x++;        
        y=0;
    }
    return result;
}

/**---------------------------------------------------------------------
    Entrée :
        - tab : Le tableau qui contiendra les valeures du fichier
        - nom : Un tableau de caractere contenant le chemin relatif du fichier

    Sortie : Void , le tableau passer en parametre sera remplit avec les valeures du fichier
---------------------------------------------------------------------**/
void fileToTab1D (float *tab, char *nom)
{

int i=0;

ifstream file (nom);
    if(!file)
        {
            cerr<<"Ouverture du fichier impossible de "<< nom <<endl;
            exit(1);
        }

      while (!file.eof ())
      {
        char ligne[256];
        file.getline (ligne, sizeof (ligne));
        tab[i] = atof(ligne);
        i++;
      }
      file.close ();


}

/**---------------------------------------------------------------------
    Entrée :
        - tab : La matrice qui contiendra les valeures du fichier
        - nom : Un tableau de caractere contenant le chemin relatif du fichier
        - size_X : Le nombre de lignes du tableau
        - size_Y : Le nombre de colonnes du tableau

    Sortie : Void , le tableau passer en parametre sera remplit avec les valeures du fichier
---------------------------------------------------------------------**/

void fileToTab2D (float **tab, char *nom, int size_X, int size_Y)
{

int i=0, j=0, k=0;

float *tab1D = new float [size_X*size_Y];

ifstream file (nom);
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


delete[] tab1D;


}

/**---------------------------------------------------------------------
    Identique a fileToTab2D sauf que le fichier passer en parametre contient des données sous forme de matrice
    Entrée :
        - tab : La matrice qui contiendra les valeures du fichier
        - nom : Un tableau de caractere contenant le chemin relatif du fichier
        - size_X : Le nombre de lignes du tableau
        - size_Y : Le nombre de colonnes du tableau

    Sortie : Void , le tableau passer en parametre sera remplit avec les valeures du fichier
---------------------------------------------------------------------**/

void fileMatrixToTab2D (float **tab, QString nom, int size_X, int size_Y)
{
    float val = 0;
    int i=0, j=0, k=0;
    float *tab1D = new float [size_X*size_Y];

    int maxVal = size_X*size_Y;
    qDebug() << "Lect file declaration vars";
    qDebug() << "Lect file nom file :" << nom;


        QFile file(nom);
        qDebug() << file.fileName();
        if(file.open(QIODevice::ReadWrite | QIODevice::Text)){
            QTextStream in(&file);
            qDebug() << "Lect file Ouverture fichiers";

                //Values[0] correspond au nombre de colonnes et values[1] au nombre de lignes
                maxVal = size_Y * size_X;
                tab1D = new float[maxVal];

                qDebug() << "Lect file Lecture du fichier";
                i=0;
                while (i < maxVal)
                {
                    in >> val;
                    tab1D[i] = val;
                    i++;
                    if(i%1000000 == 0)
                        cout << i << " elements traitees" << endl << endl;
                }
                qDebug() << "Lect file Lecture terminee";

                qDebug() << "Lect file Stockage des donnees dans une matrice";

                // Passage de tab1D à tab
                    k =0;
                    for (i=0; i< size_X ; i++)
                    {
                        for (j=0; j< size_Y ; j++)
                        {
                            tab[i][j]=  tab1D [k];
                            k++;

                        }
                    }
                    file.close();
                    qDebug() << "Lect file Lecture dans une matrice";
    qDebug() << "Lect file Sauvegarde du tab";
}else
    qDebug() << "Erreur lecture";

//cout << "Lecture fichier "<<nom<< "terminee..." <<endl;
}

/**---------------------------------------------------------------------
    Fonction permettant d'enregistrer les valeurs d'un tableau dans un fichier
    Le fichier contiendra les valeurs sous forme de matrice correspondant au tableau
    Entrée :
        - tab : La matrice qui contiendra les valeures du fichier
        - nom : Un tableau de caractere contenant le chemin relatif du fichier
        - size_X : Le nombre de lignes du tableau
        - size_Y : Le nombre de colonnes du tableau

    Sortie : Void , le tableau passer en parametre sera remplit avec les valeures du fichier
---------------------------------------------------------------------**/

void tab2DToFileMatrix (float **tab, QString nom, int ligne, int colonne)

{
    QFile file(nom);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream stream;
            for (int i = 0; i < ligne; i++)
                for (int j = 0; j < colonne; j++ )
                    {
                            if (j == colonne-1 && i == ligne-1)
                            stream << tab[i][j];

                            else if (j == colonne-1)
                            stream << tab[i][j] << endl;

                            else
                            stream << tab[i][j] << "\t";
                    }
        }


}

/**---------------------------------------------------------------------
    Fonction permettant d'enregistrer les valeurs d'un tableau dans un fichier
    et d'y mettre un en-tête contenu dans un autrte fichier créer provisoirement
    Le fichier contiendra les valeurs sous forme de matrice correspondant au tableau
    Entrée :
        - tab : La matrice qui contiendra les valeures du fichier
        - nom : Un tableau de caractere contenant le chemin relatif du fichier d'en-tête
        - nom2 : Un tableau de caractere contenant le chemin relatif du fichier ou les données seront stockées
        - size_X : Le nombre de lignes du tableau
        - size_Y : Le nombre de colonnes du tableau

    Sortie : Void , le tableau passer en parametre sera remplit avec les valeures du fichier
---------------------------------------------------------------------**/

void tab2DToFileMatrix(float **tab, QString nom,QString nom2, int ligne, int colonne){

    QFile result(nom2);
    QFile enTete(nom);
    QString stock;

    if(enTete.open(QIODevice::ReadOnly | QIODevice::Text)){
        if(result.open(QIODevice::WriteOnly | QIODevice::Text)){

            QTextStream in(&result);
            QTextStream out(&enTete);

            for(int i=0;i<6;i++){
                stock = out.readLine();
                if(!stock.isEmpty())
                    in << stock << "\n";
            }
               for (int i = 0; i < ligne; i++)
                    for (int j = 0; j < colonne; j++ )
                        {
                                if (j == colonne-1 && i == ligne-1)
                                in << tab[i][j];

                                else if (j == colonne-1)
                                in << tab[i][j] << endl;

                                else
                                in << tab[i][j] << "\t";
                        }

               result.close();
               enTete.close();
        }
    }
}
