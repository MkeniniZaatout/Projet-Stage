#include "calcul.h"
#include "gestionfichiers.h"
#include <iostream>
#include <QApplication>
#include <QtGui>
#include <QString>
#include <QList>
//#include <QMessageBox>

Calcul::Calcul()
{
}

void Calcul::start(){
    qDebug() << "Lancement ChargerConf()";
    QString zone;
    QString scenario;
    QString racine = QCoreApplication::applicationDirPath();

    int typePluie = 1;
    float* tabPluie;
    float* newTabPluie;
    int duree = 0;
    int x=0;

    // Je creer le fichier Config.txt
    // qui se trouve dans le dossier Entree
    // C:/Users/Ismael/Documents/Periode de Stage/Projet-Code-Noyau/Olpac_Web/Entree

    QFile config(racine + "/Entree/Config.txt");
    // qDebug() << "Ouverture ? : " << config.open(QIODevice::ReadOnly | QIODevice::Text);
    // Je creer les fichier pluie et zonolp
    QFile pluie;
    QFile zonolp;

    // qDebug() << "Ouvertur ?" << config.open(QIODevice::ReadOnly);
    // << " " << config.errorString() << " " << config.error() ;
    // qDebug() << "Ouvertur ?" << config.open(QIODevice::ReadWrite | QIODevice::Text);
    // J'ouvre le fichier et j'indique que je vais lire et ecrire
    // sur ce fichier et que les fin de ligne sont traduis en /n
    if(config.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Je suis ouvert !";
        //
        QTextStream out(&config);
        // QTextStream tutu(config&);
        //
        zone = out.readLine();
        scenario = out.readLine();
        QString tutu = out.readLine();
        racine += tutu;

        typePluie = out.readLine().toInt();
        config.close();
    }
    qDebug() << "Afficher le contenue de zone , scenario , racine :";
    qDebug() << zone;
    qDebug() << scenario;
    qDebug() << racine;

    setCheminRacine(racine);
    setNomScenario(scenario);
    setNomZone(zone);
    pluie.setFileName(QString(racine+"/"+zone+"/"+scenario+"/Pluies/Pluvio.asc"));
    zonolp.setFileName(QString("%1/%2/%3/%4.scenolp").arg(racine).arg(zone).arg(scenario).arg(scenario));

    // qDebug() << "Ouverture pluie , zonolp : " <<pluie.open(QIODevice::ReadOnly | QIODevice::Text) << " " <<zonolp.open(QIODevice::ReadOnly | QIODevice::Text);
    //if(pluie.open(QIODevice::ReadWrite | QIODevice::Text)){
    if(pluie.open(QIODevice::ReadOnly | QIODevice::Text)){
        //if(zonolp.open(QIODevice::ReadWrite | QIODevice::Text)){
        if(zonolp.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream pluieOut(&pluie);
            QTextStream zonolpOut(&zonolp);

            zonolpOut >> duree;
            tabPluie = new float[duree];

            for(int i=0; i < duree; i++){
                // qDebug() << pluieOut;
                pluieOut >> tabPluie[i];
            }

            if(typePluie == 1){
                newTabPluie = new float[duree/5];
                for(int i=0; i < duree; i++){
                    if(i%5==0 && i+4 < duree){
                        newTabPluie[x] = tabPluie[i] + tabPluie[i+1] + tabPluie[i+2] + tabPluie[i+3] + tabPluie[i+4];
                        x++;
                    }
                }
            }else if(typePluie == 5){
                newTabPluie = new float[duree*5];
                for(int i=0; i < duree*5; i++){
                        newTabPluie[i] = tabPluie[x]/5;
                        if(i%5 == 0)
                            x++;
                }
            }
        }

        QFile::copy(pluie.fileName(),QString("%1\\%2\\%3\\Pluies\\Old_Pluvio.asc").arg(racine).arg(zone).arg(scenario));
        pluie.close();
        if(pluie.remove()){
            if(pluie.open(QIODevice::ReadWrite | QIODevice::Text)){
                QTextStream in(&pluie);
                if(typePluie == 1){
                    for(int i=0; i < duree/5; i++){
                        qDebug() << newTabPluie[i];
                        // in << fixed << std::setprecision(2) << newTabPluie[i] << "\n";
                        in << newTabPluie[i] << "\n";
                    }
                }else if(typePluie == 5){
                    for(int i=0; i < duree*5; i++){
                        in << newTabPluie[i] << "\n";
                    }
                }
            }
        }
    }

    ChargerConf();
    //SupprAnciensResu();
    Charger_ZONE(racine);
    QString brooksPath = QCoreApplication::applicationDirPath()+"/brooks.exe";
    qDebug() << brooksPath;
    QProcess::startDetached(brooksPath);

}

/**************************************************************************************
    Charger les données d'un fichier ZONOLP
**************************************************************************************/
/*!
 * \brief Calcul::Charger_ZONE
 */
void Calcul::Charger_ZONE(QString racine)
{
    qDebug() << "Lancement de charger Zone";
    float xMin;
    float yMin;
    int nodata;

    // --- On vide toutes les listes
    //
    this->noms_LIT.clear();
    this->couleurs_LIT.clear();
    this->noms_OCS.clear();
    this->couleurs_OCS.clear();
    this->anglesMax.clear();

    this->m_temps = 0;

    qDebug() << "Charger_ZONE : Renitialisation fichiers";

    // --- On ouvre le fichier en lecture seule
    //
    QFile fichier(getCheminFichierExp());
    if(fichier.open(QIODevice::ReadWrite | QIODevice::Text)){
        qDebug() << "Charger_ZONE : Ouverture fichier";
        QTextStream flux(&fichier);

        // --- Propriétés MNT
        //
        this->m_nbLignes = flux.readLine().toInt();
        this->m_nbColonnes = flux.readLine().toInt();
        this->m_cPixel = flux.readLine().toFloat();
        qDebug() << "Charger_ZONE : Recup props MNT";

        // --- Propriétés LIT
        //
        int nb_LIT = flux.readLine().toInt();

        for(int i=0; i<nb_LIT; i++)
        {
            QStringList liste = flux.readLine().split("|");
            noms_LIT.append(liste.at(0));

            int r = liste.at(1).toInt();
            int v = liste.at(2).toInt();
            int b = liste.at(3).toInt();
            QColor couleur(r, v, b);
            couleurs_LIT.append(couleur);
        }
        qDebug() << "Charger_ZONE : Recup prop LIT";

        // --- Propriétés OCS
        //
        int nb_OCS = flux.readLine().toInt();

        for(int i=0; i<nb_OCS; i++)
        {
            QStringList liste = flux.readLine().split("|");
            noms_OCS.append(liste.at(0));

            int r = liste.at(1).toInt();
            int v = liste.at(2).toInt();
            int b = liste.at(3).toInt();
            QColor couleur(r, v, b);
            couleurs_OCS.append(couleur);
        }
        qDebug() << "Charger_ZONE : Recup prop OCS";

        xMin = flux.readLine().toFloat();
        yMin = flux.readLine().toFloat();
        nodata = flux.readLine().toInt();

        fichier.close();
    }else{
        qDebug() << "Echec ouverture Lit , Mnt , Ocs";
    }

    // --- On change la configuration de Brooks.exe
    //
    qDebug() << "Changement de la configuration de Brooks.exe ...";
    qDebug() << "chemin brooks :"<< racine;
    QFile fichierConf(QCoreApplication::applicationDirPath() + "/brooks.conf");
    if(fichierConf.open(QIODevice::ReadWrite | QIODevice::Text)){
        QTextStream fluxConf(&fichierConf);
        qDebug() << "Charger_ZONE : Ouverture brooks.conf";
        qDebug() << m_cheminRacine << m_nomZone << m_nomScenario;

        fluxConf << m_cheminRacine << "\n";
        fluxConf << m_nomZone << "\n";
        fluxConf << m_nomScenario;
        fichierConf.close();
    }else{
        qDebug() << "Echec lors du Changement de la configuration de Brooks.exe";
    }


    // --- Lecture des angles max
    //
    QFile fichierAM(Chemin_Valeurs_LIT());
    if(fichierAM.open(QIODevice::ReadWrite | QIODevice::Text)){
        QTextStream fluxAM(&fichierAM);
        qDebug() << "Charger_ZONE : Ouverture valeurs LIT";

        for(int i=0; i<noms_LIT.size(); i++)
        {
            float val;
            for(int j=0; j<13; j++) fluxAM >> val;
            anglesMax.append(val);
        }

        fichierAM.close();
        qDebug() << "Charger_ZONE : Fermeture valeurs LIT";
    }else{
        qDebug() << "Echec ouverture valeur LIT pour anglesMax";
    }

    // --- On lit les cartes de la zone
    //
    this->matrice_MNT = GestionFichiers::LireMatrice(Fichier_MNT(), true);
    this->matrice_LIT = GestionFichiers::LireMatrice(Fichier_LIT(), false);
    this->matrice_OCS = GestionFichiers::LireMatrice(Fichier_OCS(), false);
    this->matrice_Masque = GestionFichiers::LireMatrice(Fichier_MSQ(), false);
    this->matrice_Pente = GestionFichiers::LireMatrice(Fichier_Pente(), true);
    qDebug() << "Charger_ZONE : Lecture des fichiers matrice";

    // --- On crée le fichier en-tête
    //
    QFile fichierET("EN-TETE");
    if(fichierET.open(QIODevice::ReadWrite | QIODevice::Text)){
        QTextStream fluxET(&fichierET);
        qDebug() << "Charger_ZONE : Creation EN TETE";
        fluxET << m_nbColonnes << "\n";
        fluxET << m_nbLignes << "\n";
        fluxET << xMin << "\n";
        fluxET << yMin << "\n";
        fluxET << m_cPixel << "\n";
        fluxET << nodata << "\n";
        qDebug() << m_nbColonnes << m_nbLignes << xMin << m_cPixel;
        fichierET.close();
        qDebug() << "Charger_ZONE : Fermeture EN TETE";
    }
}

/**************************************************************************************
    Charger les données du fichier CONF (légende)
**************************************************************************************/

void Calcul::ChargerConf()
{
    qDebug() << "ChargerConf : ";
    m_legendeFS.clear();

    // --- On ouvre le fichier en lecture seule
    //
    QFile fichier("CONFIG");
    if(fichier.open(QIODevice::ReadWrite | QIODevice::Text)){
        qDebug() << "ChargerConf : Ouverture fichier CONF";
        QTextStream flux(&fichier);
        qDebug() << "ChargerConf : Creation TextStream";

        int nbClassesFS = flux.readLine().toInt();
        qDebug() << "ChargerConf : nbClasses" << nbClassesFS;

        qDebug() << "ChargerConf : Debut boucle";
        for(int idCl=0; idCl < nbClassesFS; idCl++)
        {
            QString ligne = flux.readLine();
            QStringList classe = ligne.split("|");

            QList<float> vals;
            for(int i=0; i<classe.size(); i++)
            {
                vals.append(classe.at(i).toFloat());
            }
            this->m_legendeFS.append(vals);
        }

        fichier.close();
        qDebug() << "ChargerConf : Fermeture fichier";
    }
}

/**************************************************************************************
    Supprimer tous les FS et de HW générés avec une ancienne configuration
**************************************************************************************/

void Calcul::SupprAnciensResu()
{
    // --- On récupère les dossiers de FS et de HW
    //
    qDebug() << "SupprAnciensResu : ";
    QDir dossierFS(this->getCheminFS());
    QDir dossierHW(this->getCheminHW());
    qDebug() << "SupprAnciensResu : Recuperation dossier FS et HW";
    qDebug() << "SupprAnciensResu : FS" << this->getCheminFS();
    qDebug() << "SupprAnciensResu : HW" << this->getCheminHW();


    // --- Seuls les fichiers ".asc" nous intéressent
    //
    QStringList filtres("*.asc");

    // --- On récupére le contenu des dossiers de FS et de HW
    //
    QStringList fichiersFS = dossierFS.entryList(filtres);
    QStringList fichiersHW = dossierHW.entryList(filtres);
    qDebug() << "SupprAnciensResu : Recuperation contenu des dossiers";

    // --- On supprime chaque fichier du dossier de FS
    //
    for(int i=0; i<fichiersFS.size(); i++) dossierFS.remove(fichiersFS.at(i));
    qDebug() << "SupprAnciensResu : Suppr FS";

    // --- Idem pour le dossier de HW
    //
    for(int i=0; i<fichiersHW.size(); i++) dossierHW.remove(fichiersHW.at(i));
    qDebug() << "SupprAnciensResu : Suppr HW";
}

/****************************************************************************************
    INFOS SUR LE FICHIER OUVERT
****************************************************************************************/

    /***********
        GET
    ***********/

    QString Calcul::getCheminRacine() { return m_cheminRacine; }
    QString Calcul::getNomZone() { return m_nomZone; }
    QString Calcul::getNomScenario() { return m_nomScenario; }

    /***********
        SET
    ***********/

    void Calcul::setCheminRacine(QString cheminRacine) { m_cheminRacine = cheminRacine; }
    void Calcul::setNomZone(QString nomZone) { m_nomZone = nomZone; }
    void Calcul::setNomScenario(QString nomScenario) { m_nomScenario = nomScenario; }

/****************************************************************************************
    CHEMINS DES FICHIER à CONSULTER
****************************************************************************************/

    QString Calcul::getCheminFichierSim()
    {
        return m_cheminRacine + "\\" + m_nomZone + "\\" + m_nomScenario + "\\" + m_nomScenario + ".sim.olp";
    }

    QString Calcul::getCheminDossierSim()
    {
        return m_cheminRacine + "\\" + m_nomZone + "\\" + m_nomScenario;
    }

    QString Calcul::getCheminFichierExp()
    {
        return m_cheminRacine + "\\" + m_nomZone + "\\" + m_nomZone + ".zonolp";
    }

    QString Calcul::getCheminDossierExp()
    {
        return m_cheminRacine + "\\" + m_nomZone;
    }

    QString Calcul::getCheminFS()
    {
        return m_cheminRacine + "\\" + m_nomZone + "\\" + m_nomScenario + "\\Sortie\\FS";
    }

    QString Calcul::getCheminFS(int idFS)
    {
        return m_cheminRacine + "\\" + m_nomZone + "\\" + m_nomScenario + "\\Sortie\\FS\\FS-" + QString::number(idFS) + ".asc";
    }

    QString Calcul::getCheminHW()
    {
        return m_cheminRacine + "\\" + m_nomZone + "\\" + m_nomScenario + "\\Sortie\\HW";
    }

    QString Calcul::getCheminHW(int idHW)
    {
        return m_cheminRacine + "\\" + m_nomZone + "\\" + m_nomScenario + "\\Sortie\\HW\\HW-" + QString::number(idHW) + ".asc";
    }

    QString Calcul::getCheminFSAtemp()
    {
        return m_cheminRacine + "\\" + m_nomZone + "\\" + m_nomScenario + "\\Sortie\\FS\\FS-Atemp.asc";
    }

    QString Calcul::getCheminFSTime()
    {
        return m_cheminRacine + "\\" + m_nomZone + "\\" + m_nomScenario + "\\Sortie\\FS\\FS-Time.asc";
    }

    QString Calcul::getCheminHWAtemp()
    {
        return m_cheminRacine + "\\" + m_nomZone + "\\" + m_nomScenario + "\\Sortie\\HW\\HW-Atemp.asc";
    }

    QString Calcul::getCheminHWTime()
    {
        return m_cheminRacine + "\\" + m_nomZone + "\\" + m_nomScenario + "\\Sortie\\HW\\HW-Time.asc";
    }

    QString Calcul::getCheminPluieCalc()
    {
        return m_cheminRacine + "\\" + m_nomZone + "\\" + m_nomScenario + "\\Pluies\\";
    }

    /********************************************************************
        LEGENDE
    ********************************************************************/

    float Calcul::getValLegendeFS(int idCl, int idVal)
    {
        return m_legendeFS.at(idCl).at(idVal);
    }

    float Calcul::getValLegendeHW(int idCl, int idVal)
    {
        return m_legendeHW.at(idCl).at(idVal);
    }

    void Calcul::setValLegendeFS(int idCl, int idVal, float val)
    {
        m_legendeFS[idCl].replace(idVal, val);
    }

    void Calcul::setValLegendeHW(int idCl, int idVal, float val)
    {
        m_legendeHW[idCl].replace(idVal, val);
    }

    int Calcul::getNbClassesFS()
    {
        if(this->m_legendeFS.size() == 0) return -1;
        if(this->m_legendeFS.at(0).at(0) == -1) return -1;
        return this->m_legendeFS.size();
    }

    int Calcul::getNbClassesHW()
    {
        if(this->m_legendeHW.size() == 0) return -1;
        if(this->m_legendeHW.at(0).at(0) == -1) return -1;
        return this->m_legendeHW.size();
    }

    QColor Calcul::getCouleurPixelCarte(int idCarte, int i, int j)
    {
        return QColor();

        if(idCarte == FS)
        {

        }
    }

    float Calcul::getFS(int i, int j)
    {
        if(i>matrice_FS.size()-1) return -1;
        if(j>matrice_FS.at(0).size()-1) return -1;
        return this->matrice_FS.at(i).at(j);
    }

    int Calcul::getNbLignes(){ return this->matrice_MNT.size();}

    int Calcul::getNbColonnes()
    {
        if(this->matrice_MNT.size() == 0) return 0;
        return this->matrice_MNT.at(0).size();
    }

    void Calcul::setTemps(int temps)
    {
        if(temps == -1)
        {
            this->matrice_FS = GestionFichiers::LireMatrice(this->getCheminFSAtemp(), true);
            this->m_isAtemp = true;
        }
        else
        {
            this->matrice_FS = GestionFichiers::LireMatrice(this->getCheminFS(temps), true);
            this->m_isAtemp = false;
            this->m_temps = temps;
        }
    }

    int Calcul::getTemps() { return this->m_temps; }
    bool Calcul::getIsAtemp() { return this->m_isAtemp; }

    /*******************************************************************
        Pour les widgets OGL
    *******************************************************************/

    float Calcul::getCPixel() {return this->m_cPixel; }


    /*************************************************************************************************
        Chemin vers les cartes, photos et propriétés de la zone
    *************************************************************************************************/

    QString Calcul::Fichier_MNT(){ return m_cheminRacine + "\\" + m_nomZone + "\\MNT.asc"; }
    QString Calcul::Fichier_LIT(){ return m_cheminRacine + "\\" + m_nomZone + "\\Litho.asc"; }
    QString Calcul::Fichier_OCS(){ return m_cheminRacine + "\\" + m_nomZone + "\\ODS.asc"; }
    QString Calcul::Fichier_MSQ(){ return m_cheminRacine + "\\" + m_nomZone + "\\Masque.asc"; }
    QString Calcul::Fichier_Pente(){ return m_cheminRacine + "\\" + m_nomZone + "\\Pente.asc"; }

    QString Calcul::Fichier_CAD(){ return m_cheminRacine + "\\" + m_nomZone + "\\Plan.png"; }
    QString Calcul::Fichier_PHO(){ return m_cheminRacine + "\\" + m_nomZone + "\\Photo.png"; }

    QString Calcul::Fichier_ZON(){ return m_cheminRacine + "\\" + m_nomZone + "\\" + m_nomZone + ".zonolp"; }

    QString Calcul::Fichier_SCEN(){ return m_cheminRacine + "\\" + m_nomZone + "\\" + m_nomScenario + "\\" + m_nomScenario + ".scenolp"; }

    /**************************************************
        Chemin vers les valeurs LIT, OCS et R
    **************************************************/

    QString Calcul::Chemin_Valeurs_LIT() {return m_cheminRacine + "\\" + m_nomZone + "\\" + m_nomScenario + "\\Valeurs\\valeurs-Litho.asc";}
    QString Calcul::Chemin_Valeurs_OCS() {return m_cheminRacine + "\\" + m_nomZone + "\\" + m_nomScenario + "\\Valeurs\\valeurs-ODS.asc";}
    QString Calcul::Chemin_Valeurs_R()   {return m_cheminRacine + "\\" + m_nomZone + "\\" + m_nomScenario + "\\Valeurs\\valeurs-R.asc";}

    /**************************************************
        Chemin vers la Pluie
    **************************************************/

    QString Calcul::Chemin_Pluie() {return m_cheminRacine + "\\" + m_nomZone + "\\" + m_nomScenario + "\\Pluies\\Pluvio.asc";}
    QString Calcul::Chemin_Hyeto() {return m_cheminRacine + "\\" + m_nomZone + "\\" + m_nomScenario + "\\Pluies\\Hyétogramme.png";}
    QString Calcul::Chemin_Prop_Pluie() {return m_cheminRacine + "\\" + m_nomZone + "\\" + m_nomScenario + "\\Pluies\\Pluvio-Prop.asc";}
    QString Calcul::Chemin_Phase_Pluie(int phase)
    {
        return m_cheminRacine + "\\" + m_nomZone + "\\" + m_nomScenario + "\\Pluies\\Pluvio-" + QString::number(phase) + ".asc";
    }
