#ifndef CALCUL_H
#define CALCUL_H
#include "header.h"

class Calcul
{
protected:

    // --- Fichiers
    //
    QString m_cheminRacine;
    QString m_nomZone;
    QString m_nomScenario;

    int m_nbLignes;
    int m_nbColonnes;
    float m_cPixel;

    // --- L�gendes
    //
    QList<QList<float> > m_legendeFS;
    int m_typeLegendeFS;
    QList<QList<float> > m_legendeHW;
    int m_typeLegendeHW;

    // --- Buffers
    //
    unsigned int idBufSommets;
    unsigned int idBufTextures;
    unsigned int idBufCouleursFS;
    unsigned int idBufCouleursHW;
    unsigned int idBufCouleursGeol;
    unsigned int idBufCouleursOccupSol;

    int m_temps;
    int m_isAtemp;

public:
        Calcul();
        void start();

    // --- Cartes
    //
    QVector< QVector<float> > matrice_MNT;
    QVector< QVector<float> > matrice_LIT;
    QVector< QVector<float> > matrice_OCS;
    QVector< QVector<float> > matrice_Masque;
    QVector< QVector<float> > matrice_Pente;
    QVector< QVector<float> > matrice_FS;
    QVector< QVector<float> > matrice_HW;

    QList<float> anglesMax;

    QVector< QVector<float> > m_tabsMontana;

    void InitialiserCamera();

    void set3D(bool is3d);
    bool get3D();

    // --- Pour placer la cam avec gluLookAt
    //
    float getXOeil();
    float getYOeil();
    float getZOeil();

    float getXCentre();
    float getYCentre();
    float getZCentre();

    float getXVectHaut();
    float getYVectHaut();
    float getZVectHaut();

    // --- Evenements
    //
    void setEvtCtrlCam(bool ctrlCam);
    void setEvtVersHaut(bool versHaut);
    void setEvtVersBas(bool versBas);
    void setEvtVersGauche(bool versGauche);
    void setEvtVersDroite(bool versDroite);
    void setEvtZoomPlus(bool zoomPlus);
    void setEvtZoomMoins(bool zoomMoins);

    bool getEvtCtrlCam();
    bool getEvtVersHaut();
    bool getEvtVersBas();
    bool getEvtVersGauche();
    bool getEvtVersDroite();

    void SeDeplacer();

    enum TypeCarte { MNT, kSat, kSatSub, c, gammaHum, gammaSec, phi, r, n, nSub, litho, pente, rg, tetaMin, tetaMinSub, tetaMax, tetaMaxSub };
    enum TypePluie { Aucun, Fichier, Montana };
    enum TypeLegende { FS, HW, Geol, OccupSol };

    float getValLegendeFS(int idCl, int idVal);
    void setValLegendeFS(int idCl, int idVal, float val);
    int getNbClassesFS();

    float getValLegendeHW(int idCl, int idVal);
    void setValLegendeHW(int idCl, int idVal, float val);
    int getNbClassesHW();

    float getFS(int i, int j);

    void setTemps(int temps);
    int getTemps();
    bool getIsAtemp();

    // -2- Valeurs g�ol
    //
    QList<QColor> couleurs_LIT;
    QList<QString> noms_LIT;

    QList<QColor> couleurs_OCS;
    QList<QString> noms_OCS;

    void InitaliserDefaut();
    void InitialiserFichier(QString nomFichier);

    /***********************************************
        GETTEURS
    ***********************************************/

    // --- Terrain
    //
    QString getCheminDossierCartes();
    QString getFichierCarte(int index);

    // --- Pluies
    //
    float getMontana_a();
    float getMontana_b();
    float getMontana_D();
    bool getMontana_save();
    bool getPluies_a_lire();
    QString getMontana_dossier();
    QString getExistant_dossier();

    // --- Autres
    //
    float getCPixel();
    int getNbLignes();
    int getNbColonnes();
    int getNbCasesFormSup();
    float getHauteurCase();
    int getDureeCalc();
    float getK();
    float getResVert();

    QColor getCouleurPixelCarte(int idCarte, int i, int j);

    /***********************************************
        ACCESSEURS CHEMINS FICHIERS ET DOSSIERS
    ***********************************************/

    QString getCheminRacine();
    void setCheminRacine(QString cheminRacine);

    QString getNomZone();
    void setNomZone(QString nomZone);

    QString getNomScenario();
    void setNomScenario(QString nomScenario);

    QString getCheminFichierSim();
    QString getCheminDossierSim();

    QString getCheminFichierExp();
    QString getCheminDossierExp();

    /*******************************************************************
        OBTENIR LES CHEMINS VERS LES FICHIERS CONCERNANT LA ZONE
    *******************************************************************/

    QString Fichier_CAD();
    QString Fichier_PHO();
    QString Fichier_MNT();
    QString Fichier_LIT();
    QString Fichier_OCS();
    QString Fichier_MSQ();
    QString Fichier_Pente();
    QString Fichier_ZON();
    QString Fichier_SCEN();

    QString getCheminPluieCalc();

    QString getCheminFS();
    QString getCheminHW();
    QString getCheminFS(int idFS);
    QString getCheminHW(int idHW);
    QString getCheminFSAtemp();
    QString getCheminFSTime();
    QString getCheminHWAtemp();
    QString getCheminHWTime();

    // -3- Pluies
    //
    int getTypePluie();
    void setTypePluie(int typePluie);

    QString getCommentairePluie();
    void setCommentairePluie(QString commentairePluie);

    QString getCheminDataPluie();
    void setCheminDataPluie(QString cheminPluie);

    QDateTime getDebutPluie();
    void setDebutPluie(QDateTime debutPluie);
    QDateTime getFinPluie();
    void setFinPluie(QDateTime finPluie);

    float getMontanaA();
    void setMontanaA(float montanaA);
    float getMontanaB();
    void setMontanaB(float montanaB);
    int getMontanaDPic();
    void setMontanaDPic(int montanaDPic);
    int getMontanaDTot();
    void setMontanaDTot(int montanaDTot);

    /***********************************************
        SETTEURS
    ***********************************************/

    // --- Terrain
    //
    void setCheminDossierCartes(QString chemin);
    void setFichierCarte(int index, QString str);

    // --- Pluies
    //
    void setMontana_a(float a);
    void setMontana_b(float b);
    void setMontana_D(float D);
    void setMontana_save(bool save);
    void setMontana_dossier(QString chemin);
    void setExistant_dossier(QString chemin);
    void setPluies_a_lire(bool pluies);

    // --- Autres
    //
    void setCPixel(float val);
    void setNbLignes(int val);
    void setNbColonnes(int val);
    void setNbCasesFormSup(int val);
    void setHauteurCase(float val);
    void setDureeCalc(int val);
    void setK(float val);
    void setResVert(float val);

    // --- Sortie
    //
    void setCheminDossierFsHw(int index, QString str);
    void setIsSavedDossierFsHw(int index, bool b);
    void setCheminFichierAtempTime(int index, QString str);
    void setIsSavedFichierAtempTime(int index, bool b);

    // --- 3D
    //
    void setCheminCarteTopo(QString cheminCarteTopo);
    void setCheminOrthophoto(QString cheminOrthophoto);

    // --- Save
    //
    void Sauvegarder();
    void Charger_ZONE(QString racine);

    void ChargerConf();
    void SauvegarderConf();

    QString Chemin_Valeurs_LIT();
    QString Chemin_Valeurs_OCS();
    QString Chemin_Valeurs_R();

    QString Chemin_Pluie();
    QString Chemin_Hyeto();
    QString Chemin_Prop_Pluie();
    QString Chemin_Phase_Pluie(int phase);

    // --- Gestion des fichiers que Brooks.exe va lire
    //
    void GenererMatricesGeol();
    void GenererMatriceRuissel();
    void GenererFichierConfig();
    void SupprAnciensResu();

    // --- Pour calculer le placement de la cam
    //
    int getMulTeta();
    int getMulPhi();
    int getMulRho();
    float getAngleInc();
};

#endif // CALCUL_H
