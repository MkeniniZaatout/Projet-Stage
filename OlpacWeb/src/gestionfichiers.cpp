#include "gestionfichiers.h"
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

GestionFichiers::GestionFichiers()
{
}

QVector< QVector<float> > GestionFichiers::LireMatrice(QString cheminFichier, bool reduire)
{
    qDebug() << "LireMatrice : " << cheminFichier;
    // Matrice
    QVector< QVector<float> > matrice;

    QFile fichier(cheminFichier);

    // if(fichier.open(QIODevice::ReadWrite | QIODevice::Text)){
    if(fichier.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream flux(&fichier);
        qDebug() << "LireMatrice : Ouverture fichier";

        QString ligneTxt;
        QStringList liste;
        qDebug() << "LireMatrice : Creation vars";

        while(!flux.atEnd())
        {
            QVector<float> ligne;

            ligneTxt = flux.readLine();
            liste = ligneTxt.split("\t");

            for(int i=0; i<liste.size(); i++)
            {
                if(reduire == true){
                // Ajouter nombre avec deux chiffre apres la virgule
                  float x = roundf(liste.at(i).toFloat()*100)/ 100.0f;
                  ligne.append(x);
                }else{
                   ligne.append(liste.at(i).toFloat());
                }
            }

            matrice.append(ligne);
        }

        fichier.close();
        qDebug() << "LireMatrice : Fermeture fichier et retour matrice";
        return matrice;
    }else{
    qDebug() << "Erreur d'ouverture du" << cheminFichier << ", matrice vide";
    }

    return matrice;
}

void GestionFichiers::TradMNT(QString entree, QString sortie)
{
    QFile fichierIn(entree);
    if(fichierIn.open(QIODevice::ReadWrite | QIODevice::Text)){
        QTextStream fluxIn(&fichierIn);
        qDebug() << "TradMNT : Ouverture fichier entree";

        QFile fichierOut(sortie);
        if(fichierOut.open(QIODevice::ReadWrite | QIODevice::Text)){
            QTextStream fluxOut(&fichierOut);
            qDebug() << "TradMNT : Ouverture fichier sortie";

            QString ligneTxt;
            QStringList liste;
            QString ligneCourante = "";
            qDebug() << "TradMNT : Declaration vars";

            while(!fluxIn.atEnd())
            {
                ligneTxt = fluxIn.readLine();

                if(ligneTxt != "")
                {
                    liste = ligneTxt.split(",");

                    if(ligneCourante != liste.at(1) && ligneCourante != "")
                    {
                        fluxOut << "\n";
                    }
                    else if(ligneCourante != "")
                    {
                        fluxOut << "\t";
                    }

                    fluxOut << liste.at(2);

                    ligneCourante = liste.at(1);
                }
                else
                {
                    break;
                }
            }

            fichierIn.close();
        }
        fichierOut.close();
        qDebug() << "TradMNT : Fermeture fichiers";
    }
}

void GestionFichiers::CopierArboresence(QString cheminRacine, QString dossierDepart, QString dossierArrivee)
{
    // --- Création de l'arborescence
    //
    QDir dossierRacine(cheminRacine);
    dossierRacine.mkdir(dossierArrivee);
    dossierRacine.mkdir(dossierArrivee + "\\Pluies");
    dossierRacine.mkdir(dossierArrivee + "\\Valeurs");
    dossierRacine.mkdir(dossierArrivee + "\\Sortie");
    dossierRacine.mkdir(dossierArrivee + "\\Sortie\\FS");
    dossierRacine.mkdir(dossierArrivee + "\\Sortie\\HW");

    // --- Pour la copie des fichiers
    //
    QStringList filtres("*.asc");

    QString cheminDepart = cheminRacine + "\\" + dossierDepart;
    QString cheminArrivee = cheminRacine + "\\" + dossierArrivee;

    QFile::copy(cheminDepart + "\\" + dossierDepart + ".scenolp", cheminArrivee + "\\" + dossierArrivee + ".scenolp");

    // --- Copie des fichiers géol
    //
    QDir dossierGeol(cheminDepart + "\\Valeurs");
    QStringList fichiersGeol = dossierGeol.entryList(filtres);

    for(int i=0; i<fichiersGeol.count(); i++)
    {
        QFile::copy(cheminDepart + "\\Valeurs\\" + fichiersGeol.at(i), cheminArrivee + "\\Valeurs\\" + fichiersGeol.at(i));
    }

    // --- Copie des fichiers de pluie
    //
    QDir dossierPluie(cheminDepart + "\\Pluies");
    QStringList fichiersPluie = dossierPluie.entryList(filtres);

    for(int i=0; i<fichiersPluie.count(); i++)
    {
        QFile::copy(cheminDepart + "\\Pluies\\" + fichiersPluie.at(i), cheminArrivee + "\\Pluies\\" + fichiersPluie.at(i));
    }
}
