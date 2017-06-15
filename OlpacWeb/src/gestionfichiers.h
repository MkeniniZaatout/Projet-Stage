#ifndef GESTIONFICHIERS_H
#define GESTIONFICHIERS_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QVector>

class GestionFichiers
{
public:
    GestionFichiers();
    static QVector< QVector<float> > LireMatrice(QString cheminFichier, bool reduire);
    static void TradMNT(QString entree, QString sortie);
    static void CopierArboresence(QString cheminRacine, QString dossierDepart, QString dossierArrivee);
};

#endif // GESTIONFICHIERS_H
