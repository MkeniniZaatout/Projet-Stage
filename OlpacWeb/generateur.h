#ifndef GENERATEUR_H
#define GENERATEUR_H

#endif // GENERATEUR_H

class Generateur {

    private :
    int ligne, colonne;
    QVector< QVector<float> > matriceFs = NULL;
    QVector< QVector<float> > matriceHw = NULL;

    public:
    int getLigne(){ return ligne;}
    int getColonne(){ return colonne;}

    void calculerHwFs();

}
