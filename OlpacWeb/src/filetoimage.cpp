#include "filetoimage.h"
#include "header.h"
#include "calcalerte.h"

fileToImage::fileToImage()
{




}

void fileToImage::trait(){

    int nbFile;
    int ligne;
    int colonne;
    int height;
    int width;
    int nbClasse = 4;
    int nbAlerte = 0;
    int alerte;
    int nbVals = 0;
    int duree = 0;
    int debut = 0;

    bool alerteT = false;

    float* comptFs;
    float* classeVals;
    float** vals = NULL;
    float** alerteVals;

    QString dossier;
    QString source;
    QString sortie;
    QString row;
    QString classes;
    QString type;
    QString demo = "false";
    QStringList coulListe;
    QStringList alerteList;

    QFile config("Entree\\Config_img.txt");
    QFile configPrev("Entree\\Config_prev.txt");

    QColor couleur;

    //Ouverture du fichier de configuration
    //
    if(config.open(QIODevice::ReadWrite | QIODevice::Text)){
        //Si le fichier de configuration initial à été ouvert on traite les autres fichiers
        //

        QTextStream out(&config);

        /*
         * Les données récupérées sont :
         * - Le nombre total de FS ( ou la durée en seconde du scenario)
         * - Le nombre de ligne des FS
         * - Le nombre de colonne des FS
         * - Le dossier ou sont stockés les FS.asc
         * - Le dossier dans lequel les images générées seront sauvegardées
         * - Le dossier dans lequel est stocker le fichier de classe
         * - Indication sur le type de calcul
         *
         * Tous les chemins de dossiers peuvent etre en absolu ou relatif ( on considère alors
         *  le dossier du .exe comme la racine)
         */

        row = out.readLine();
        nbFile = row.toInt();   //Nombre de FS a traiter

        row = out.readLine();
        ligne = row.toInt();    //Nombre de lignes dans un FS

        row = out.readLine();
        colonne = row.toInt();  //Nombre de colonnes

        nbVals = ligne*colonne; //Sert a connaitre le nombre de valeurs dans un fichiers FS

        dossier = out.readLine();   //Chemin vers les fichiers FS
        sortie = out.readLine();    //Dossier de sortie des calques obtenus
        classes = out.readLine();   //Chemin des fichiers classe.conf et alerte.conf

        type = out.readLine();  //Indique le type de traitement à effectuer
        demo = out.readLine();  //Permet de traiter le cas particulier d'une demonstration
        config.close();



        //Ouverture du fichier des classes
        //Contient les differentes couleurs qui seronts utilisées
        //
        QFile classe(QString("%1\\classes.conf").arg(classes));
        qDebug() << classe.fileName();

        if(classe.open(QIODevice::ReadWrite | QIODevice::Text)){

            qDebug() << "Lecture fichier classe";
            QTextStream in(&classe);
            //On recupere le nombre de classes enregistrées
            //
            row = in.readLine();
            nbClasse = row.toInt(); //On recupere le nombre total de classe
            qDebug() << nbClasse;

            classeVals = new float[nbClasse];

            comptFs = new float[nbClasse-1];

            //Recuperation des couleurs en hexadecimal et de la valeur max de cette classe
            //
            for(int i=0;i<nbClasse;i++){
                row = in.readLine();
                coulListe << row;

                row = in.readLine();
                classeVals[i] = row.toFloat();

                if(i < nbClasse-1){
                    comptFs[i] = 0;
                    qDebug() << i;
                }
            }
            classeVals[nbClasse-1] = classeVals[nbClasse-2] + 0.5;
            classe.close();
        }

        //Ouverture du fichier de configuration pour les alertes
        //

        QFile alertes(QString("%1\\alerte.conf").arg(classes));
        if(alertes.open(QIODevice::ReadWrite | QIODevice::Text)){
            QTextStream in(&alertes);

            //On recupere le nombre d'alertes
            //
            row = in.readLine();
            nbAlerte = row.toInt();

            //On recupere l'alerte de reference
            //
            row = in.readLine();
            alerte = row.toInt();

            alerteVals = tab2DAlloc(nbAlerte,nbClasse);

            //On lit les lignes restantes dans une matrice
            //
            for(int j=0;j<nbAlerte;j++){

                row = in.readLine();
                alerteList = row.split("|",QString::SkipEmptyParts);

                for(int i=0;i<alerteList.size();i++){
                    row = alerteList.at(i);
                    alerteVals[j][i] = row.toFloat();
                }
            }
            alertes.close();
        }
        if(type == "P"){
            if(configPrev.open(QIODevice::ReadWrite | QIODevice::Text)){
                QTextStream out(&configPrev);
                row = out.readLine();
                debut = row.toInt();
                row = out.readLine();
                duree = row.toInt();
            }
        }
    }

    //On declare une nouvelle image, que l'ont initialise en noir
    //
    QImage img(QSize(colonne,ligne),QImage::Format_ARGB32);
    img.fill(QColor(255,0,0,0));

    vals = tab2DAlloc(ligne,colonne);

    qDebug() << img.height();
    qDebug() << img.width();

    qDebug() << classeVals[0];
    qDebug() << classeVals[1];
    qDebug() << classeVals[2];
    qDebug() << classeVals[3];

    //Si les lignes/colonnes donnent une image trop petite on redefini la taille
    //

    if(ligne < 1000)
        height = ligne*10;
    else
        height = ligne;
    if(colonne < 1000)
        width = colonne*10;
    else
        width = colonne;

    //Boucle qui va créer les calques(images) correspondants a chacun des FS correspondant
    //

    //TO DO : Changer le code une fois que les données arriveront en temps réel
    //Actuellement on lit les données et on créer les fichiers à l'avance pour les calculs

    if(type == "R"){
        int startCalc = 0;
        QFile test;
        if(demo == "true")
            startCalc = 640;
        for(int i=startCalc;i<nbFile;i++){
            source = QString("%1\\FS-%2.asc").arg(dossier).arg(i);
            test.setFileName(source);

            while(!test.exists()){
                //Tant que le FS n'existe pas on attend
            }
            fileMatrixToTab2D(vals,source,ligne,colonne);
            for(int x=0;x<ligne;x++)
                for(int y=0;y<colonne;y++){
                    //En fonction de la valeur on associe une couleur, definie par les classes lue
                    /* TO DO :
                     * Changer les valeurs des if pour les faire correpondres aux limits données dans le fichier classes,
                     * Les valeurs actuelles sont les valeurs standards utilisées par olpac
                     */
                    if(vals[x][y] > 0 && vals[x][y] < 0.5){
                        couleur = QColor(coulListe.at(0));
                        img.setPixel(QPoint(y,x),couleur.rgba());
                        comptFs[0]++;
                    }else if(vals[x][y] >= 0.5 && vals[x][y] < 0.7){
                        couleur = QColor(coulListe.at(1));
                        img.setPixel(QPoint(y,x),couleur.rgba());
                        comptFs[1]++;
                    }else if(vals[x][y] >= 0.7 && vals[x][y] < 0.9){
                        couleur = QColor(coulListe.at(2));
                        img.setPixel(QPoint(y,x),couleur.rgba());
                        comptFs[2]++;
                    }else if(vals[x][y] >= 0.9 && vals[x][y] < 1.1){
                        couleur = QColor(coulListe.at(3));
                        img.setPixel(QPoint(y,x),couleur.rgba());
                        comptFs[3]++;
                    }else{
                        //Si aucune des classes correpond, on affecte une valeur de pixel transparent
                        //
                        couleur = QColor(0,0,0,0);
                        img.setPixel(QPoint(y,x),couleur.rgba());
                    }
                 }

                comptFs = convertirVals(nbClasse-1,nbVals,comptFs);
                alerteT = testAlerte(comptFs,alerteVals,nbClasse,nbAlerte,alerte);

                qDebug() << alerteT;
                for(int j=0;j<nbClasse-1;j++){
                    qDebug() << comptFs[j];
                    comptFs[j] = 0;
                }

                QString path = dossier; //On recupere le chemin vers le dossier du scenario
                path.resize(path.size()-10);
                if(alerteT){
                    QFile newAlerte(QString("%1\\newAlerte.txt").arg(path));
                    if(!newAlerte.exists()){
                        if(newAlerte.open(QIODevice::ReadWrite | QIODevice::Text)){ //Creation du fichier d'alerte
                            QTextStream out(&newAlerte);
                                out << "Alerte-true\n";
                                out << type << "\n";
                                out << alerte;
                            newAlerte.close();
                        }
                      }
                    }else if(!alerteT){
                        QFile noAlerte(QString("%1\\noAlerte.txt").arg(path));
                        QFile newAlerte(QString("%1\\newAlerte.txt").arg(path));
                        if(newAlerte.exists())
                            newAlerte.remove();
                        if(!noAlerte.exists()){
                            if(noAlerte.open(QIODevice::ReadWrite | QIODevice::Text)){ //Creation du fichier d'alerte
                                QTextStream out(&noAlerte);
                                    out << "Alerte-false\n";
                                    out << type << "\n";
                                    out << alerte;
                                noAlerte.close();
                            }
                        }
                    }
                    alerteT = false;

                qDebug() << QString("%1\\FS-%2.png").arg(sortie).arg(i);
                //On adapte l'image a la taille definit plus tot
                //

                img = img.scaled(QSize(width,height),Qt::IgnoreAspectRatio);
                qDebug() << "QImage :: scaled";

                //On enregistre l'image, et on test si cela marche ( debug )
                //
                if(img.save(QString("%1\\FS-%2.png").arg(sortie).arg(i)))
                    qDebug() << "Sauvegarde image";
                else
                    qDebug() << "Erreur sauvegarde";
                //On redefinit une nouvelle image pour le prochain FS
                //
                img = QImage(QSize(colonne,ligne),QImage::Format_ARGB32);
                img.fill(QColor(255,255,255));
        }
        //img.save(QString("%1\\FS-%2.png").arg(sortie).arg(0));
    }else if(type == "P"){

        QThread::sleep(10);
        QFile test;
        int compteur = 0;
        int startCalc = (debut+duree);

        if(demo == "true")
            startCalc = 730;

        for(int i=startCalc;i<nbFile;i++){
            source = QString("%1\\FS-%2.asc").arg(dossier).arg(i);
            test.setFileName(source);

            while(!test.exists()){
                //Tant que le FS n'existe pas on attend
            }
            fileMatrixToTab2D(vals,source,ligne,colonne);
            for(int x=0;x<ligne;x++)
                for(int y=0;y<colonne;y++){
                    //En fonction de la valeur on associe une couleur, definie par les classes lue
                    /* TO DO :
                     * Changer les valeurs des if pour les faire correpondres aux limits données dans le fichier classes,
                     * Les valeurs actuelles sont les valeurs standards utilisées par olpac
                     */
                    if(vals[x][y] > 0 && vals[x][y] < 0.5){
                        couleur = QColor(coulListe.at(0));
                        img.setPixel(QPoint(y,x),couleur.rgba());
                        comptFs[0]++;
                    }else if(vals[x][y] >= 0.5 && vals[x][y] < 0.7){
                        couleur = QColor(coulListe.at(1));
                        img.setPixel(QPoint(y,x),couleur.rgba());
                        comptFs[1]++;
                    }else if(vals[x][y] >= 0.7 && vals[x][y] < 0.9){
                        couleur = QColor(coulListe.at(2));
                        img.setPixel(QPoint(y,x),couleur.rgba());
                        comptFs[2]++;
                    }else if(vals[x][y] >= 0.9 && vals[x][y] < 1.1){
                        couleur = QColor(coulListe.at(3));
                        img.setPixel(QPoint(y,x),couleur.rgba());
                        comptFs[3]++;
                    }else{
                        //Si aucune des classes correpond, on affecte une valeur de pixel transparent
                        //
                        couleur = QColor(0,0,0,0);
                        img.setPixel(QPoint(y,x),couleur.rgba());
                    }
                 }

                comptFs = convertirVals(nbClasse-1,nbVals,comptFs);
                alerteT = testAlerte(comptFs,alerteVals,nbClasse,nbAlerte,alerte);

                qDebug() << alerteT;
                for(int j=0;j<nbClasse-1;j++){
                    qDebug() << comptFs[j];
                    comptFs[j] = 0;
                }

                QString path = dossier; //On recupere le chemin vers le dossier du scenario
                path.resize(path.size()-10);
                if(alerteT){
                    QFile newAlerte(QString("%1_prev\\newAlerte.txt").arg(path));
                    if(!newAlerte.exists()){
                        if(newAlerte.open(QIODevice::ReadWrite | QIODevice::Text)){ //Creation du fichier d'alerte
                            QTextStream out(&newAlerte);
                                out << "Alerte-true\n";
                                out << type << "\n";
                                out << alerte;
                            newAlerte.close();
                        }
                      }
                    }else if(!alerteT){
                        QFile noAlerte(QString("%1_prev\\noAlerte.txt").arg(path));
                        QFile newAlerte(QString("%1_prev\\newAlerte.txt").arg(path));
                        if(newAlerte.exists())
                            newAlerte.remove();
                        if(!noAlerte.exists()){
                            if(noAlerte.open(QIODevice::ReadWrite | QIODevice::Text)){ //Creation du fichier d'alerte
                                QTextStream out(&noAlerte);
                                    out << "Alerte-false\n";
                                    out << type << "\n";
                                    out << alerte;
                                noAlerte.close();
                            }
                        }
                    }
                    alerteT = false;

                qDebug() << QString("%1\\FS-%2.png").arg(sortie).arg(i);
                //On adapte l'image a la taille definit plus tot
                //

                img = img.scaled(QSize(width,height),Qt::IgnoreAspectRatio);
                qDebug() << "QImage :: scaled";

                //On enregistre l'image, et on test si cela marche ( debug )
                //
                if(img.save(QString("%1\\FS-%2.png").arg(sortie).arg(i)))
                    qDebug() << "Sauvegarde image";
                else
                    qDebug() << "Erreur sauvegarde";
                //On redefinit une nouvelle image pour le prochain FS
                //
                img = QImage(QSize(colonne,ligne),QImage::Format_ARGB32);
                img.fill(QColor(255,255,255));
                compteur += duree;
        }
        //img.save(QString("%1\\FS-%2.png").arg(sortie).arg(0));
    }

    qApp->quit();
}
