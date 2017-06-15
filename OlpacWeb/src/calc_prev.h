#ifndef CALC_PREV_H
#define CALC_PREV_H
#include "header.h"



class calc_prev
{
public:
    calc_prev();
    void start();
    void getVals(QString file,float* vals);
    void copierScenario(QString zone,QString scenario,QString racine);
    void copierFichier(QString source,QString dest,QString nomFichierS,QString nomFichierD);
    void calc(QString source,QString dest,float* vals,int max);
    void setMax(QString source,QString dest,int max);
};

#endif // CALC_PREV_H
