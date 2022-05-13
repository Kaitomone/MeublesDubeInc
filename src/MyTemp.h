/* Copyright (C) 2022 Enzo Richard
 * All rights reserved.
 *
 * Projet Domotique
 * Ecole du Web
* Cours Systèmes embarqués (c)2022
 *  
    @file     MyTemp.h
    @author   Enzo Richard
    @version  1.1 02/05/22
    @description
      Démonstration comment récupérer la température

    platform = ESP32
    OS = Arduino
    Langage : C++

    Historique des versions
        Version    Date       Auteur       Description
        1.1        02/05/22  Enzo       Première version du logiciel

**/
#ifndef MYTEMP_H
#define MYTEMP_H

#include <DHT.h>

class MyTemp {
    private:

    public:
        MyTemp();
        ~MyTemp() { };
        float ReadTemp();
   };
#endif
