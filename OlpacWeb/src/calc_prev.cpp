#include "calc_prev.h"

calc_prev::calc_prev()
{

}

void calc_prev::start(){

    /*
     * vals[0] ==> Debut calcul previsionnel
     * vals[1] ==> Temps du calcul previsionnel
     */
    float* vals = new float[2];
    int duree = 0;

    QString fileConfPrev = ".\\Entree\\Config_prev.txt";
    QString fileConfReel = ".\\Entree\\Config.txt";
    QString fileConfImg = ".\\Entree\\Config_img.txt";

    QString scenario;
    QString zone;
    QString racine;
    QString chemin;

    QFile configReel(fileConfReel);
    QFile configImg(fileConfImg);

    if(configReel.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream out(&configReel);

            zone = out.readLine();
            scenario = out.readLine();
            racine = out.readLine();

            if(configImg.open(QIODevice::ReadOnly | QIODevice::Text)){

                QTextStream outImg(&configImg);
                QString row;

                row = outImg.readLine();
                duree = row.toInt();

                getVals(fileConfPrev,vals);

                copierScenario(zone,scenario,racine);

                chemin = QString("%1\\%2\\%3").arg(racine).arg(zone).arg(scenario);
                calc(chemin,QString("%1_prev").arg(chemin),vals,duree);
                setMax(chemin,QString("%1_prev").arg(chemin),vals[1]);

                configImg.close();
                configReel.close();
            }
    }
}

void calc_prev::getVals(QString file,float* vals){

    QFile config(file);
    if(config.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream out(&config);
        QString row;

        row = out.readLine();
        vals[0] = row.toFloat();

        row = out.readLine();
        vals[1] = row.toFloat();

        config.close();
    }
}

void calc_prev::copierScenario(QString zone,QString scenario,QString racine){

    QString dossier = QString("%1\\%2\\%3_prev").arg(racine).arg(zone).arg(scenario);
    QString source = QString("%1\\%2\\%3").arg(racine).arg(zone).arg(scenario);
    QString fichierS;
    QString fichierD;

    QDir dir(dossier);

    dir.mkpath(dossier);

    dir.mkdir("Sortie");
    dir.mkdir("Pluies");
    dir.mkdir("Valeurs");

    fichierS = QString("%1.scenolp").arg(scenario);
    fichierD = QString("%1_prev.scenolp").arg(scenario);

    copierFichier(source,dossier,fichierS,fichierD);

    fichierS = QString("c");
    fichierD = fichierS;

    copierFichier(source,dossier,fichierS,fichierD);

    fichierS = QString("Valeurs\\valeurs-Litho.asc");
    fichierD = fichierS;

    copierFichier(source,dossier,fichierS,fichierD);

    fichierS = QString("Valeurs\\valeurs-ODS.asc");
    fichierD = fichierS;

    copierFichier(source,dossier,fichierS,fichierD);

    fichierS = QString("Valeurs\\valeurs-R.asc");
    fichierD = fichierS;

    copierFichier(source,dossier,fichierS,fichierD);

    fichierS = QString("Pluies\\Pluvio-Prop.asc");
    fichierD = fichierS;

    copierFichier(source,dossier,fichierS,fichierD);
}

void calc_prev::copierFichier(QString source,QString dest,QString nomFichierS,QString nomFichierD){

    QFile file;
    file.setFileName(QString("%1\\%2").arg(source).arg(nomFichierS));
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        file.copy(QString("%1\\%2").arg(dest).arg(nomFichierD));
        file.close();
    }
}

void calc_prev::calc(QString source,QString dest,float* vals,int max){

    QString pluiesS = QString("%1\\Pluies\\Pluvio.asc").arg(source);
    QString pluiesD = QString("%1\\Pluies\\Pluvio.asc").arg(dest);
    QString ligne;

    QFile pluies(pluiesS);
    QFile pluiesPrev(pluiesD);

    int x=0;
    float total=0;
    float moyenne;

    /*
     * vals[0] ==> Debut calcul previsionnel
     * vals[1] ==> Temps du calcul previsionnel
     */

    if(pluies.open(QIODevice::ReadOnly | QIODevice::Text)){
        if(pluiesPrev.open(QIODevice::ReadWrite | QIODevice::Text)){
            QTextStream out(&pluies);
            QTextStream in(&pluiesPrev);

            if(vals[0]+vals[1] < max){
                while(x < vals[0]){
                    ligne = out.readLine();
                    x++;
                }

                for(int i = 0;i<vals[1];i++){
                   ligne = out.readLine();
                   total+=ligne.toFloat();
                }

                moyenne = total/vals[1];
                for(int i=0;i<vals[1];i++){
                    in << moyenne << "\n";
                }
             }else if(vals[0]<max){

                while(x < vals[0]){
                    ligne = out.readLine();
                    x++;
                }

                for(int i = 0;i<max-vals[0];i++){
                   ligne = out.readLine();
                   total+=ligne.toFloat();
                }

                moyenne = total/(max-vals[0]);
                for(int i=0;i<max-vals[0];i++){
                    in << moyenne << "\n";
                }
            }

            pluiesPrev.close();
            pluies.close();
        }
    }
}

void calc_prev::setMax(QString source,QString dest,int max){

    QFile fileS(source);
    QFile fileD(dest);

    QString ligne;

    if(fileS.open(QIODevice::ReadOnly | QIODevice::Text)){
        if(fileD.open(QIODevice::ReadWrite | QIODevice::Text)){
            QTextStream out(&fileS);
            QTextStream in(&fileD);

            ligne = out.readLine();
            in << max;

            for(int i=0;i<3;i++){
                ligne = out.readLine();
                in << ligne;
            }

            fileD.close();
            fileS.close();
        }
    }
}
