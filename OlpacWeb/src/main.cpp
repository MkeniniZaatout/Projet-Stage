#include <QCoreApplication>
#include <QtWidgets>
#include "filetoimage.h"
#include "calcul.h"
#include "main.h"


int main(int argc, char *argv[])

{
    // fonction pour la memoire -> getrusage(RUSAGE_SELF, &usage);
    QCoreApplication app(argc, argv);
    // return 0;

    Calcul calc;
    calc.start();
    /*
    fileToImage fsImg;
    fsImg.trait();
    */
}
