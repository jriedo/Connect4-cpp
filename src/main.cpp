/**
* @brief    A connect 4 game engine based on minimax logic with qt5 rendered GUI.
* @file     main.cpp
* @date     03.12.2018
* @version  4.2
* @author   unibe/jan.riedo
*
*
* @section c++ standard 14 and 17 tested
*/

#include <iostream>
# include <QApplication>
# include "form.h"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc , argv);
    Form w;
    w.show();
    return a.exec();
}
