#include <QDebug>


#include "calcalerte.h"

bool testAlerte(float* tabFs,float** alertes,int nbClasses,int nbAlertes,int alerte){

    bool resultat = false;

    qDebug() << "Debut testAlerte";

    //La matrice alertes a une taille [nbAlertes][nbClasses+1]
    //Car chaque ligne contient la periode correpondante en plus des valeurs
    //des pourcentages pour les classes associées
    //Exemple : Periode | Classe1 | Classe2 ...

    for(int i = 0;i<nbAlertes;i++){
        for(int j = 1;j<nbClasses;j++){
           if(alertes[i][0] == alerte){//Si on se trouve a la bonne ligne
               qDebug() << tabFs[j-1] << "   " << alertes[i][j];
                if(tabFs[j-1] > alertes[i][j])
                    resultat = true;
           }
        }
    }

    qDebug() << "Fin testAlerte";
    return resultat;

}

float* convertirVals(int nbClasses,int nbVals,float* tabVals){

    qDebug() << "Debut convertirVals";
    //On transforme les valeurs pour obtenir un pourcentage
    //Cela permet de comparer aux données existantes
    float* result = new float[nbClasses];
    for(int i = 0;i<nbClasses;i++){
        result[i] = (tabVals[i]*100)/nbVals;
        qDebug() << result[i];
    }

    qDebug() << "Fin convertirVals";
    return result;
}
