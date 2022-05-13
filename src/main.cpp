/* Copyright (C) 2022 Alain Dube
 * All rights reserved.
 *
 * Projet Stone de base
 * Ecole du Web
 * Cours Systèmes embarqués (c)2022
 *
    @file     main.cpp
    @author   Enzo Richard
    @version  1.2 12/05/15
    @description
      Faire une application qui permet d'allumer un four pour faire sécher le bois,
      avec l'aide d'un écran stone et d'un capteur de température DHT22

    platform = ESP32
    OS : Arduino
    Langage : C++

    Historique des versions
        Version    Date       Auteur       Description
        1.1        22/08/15  Alain       Première version du logiciel
        1.2        12/05/22  Enzo       Deuxieme version du logiciel

    Fonctionnalités implantées
        Lecture des evénements envoyés par l'écran
        Envoyer une commande à l'écran
          Optenir la version du Firmware de l'écran
          Lancer un compteur
          Changer des labels
 * */

#include <Arduino.h>

#define RXD2 18//16
#define TXD2 19//17
#define BAUD_RATE 115200

#include <iostream>

#include "MyButton.h"
MyButton *myButtonT4 = new MyButton();
MyButton *myButtonT5 = new MyButton();

#include "MyTemp.h"
MyTemp *myTemp;

#include "MyStone.h"
MyStone *myStone;

std::string intToHexa(int value){
  char buffer[10];
  sprintf(buffer , "0x%4X", value);
  return (buffer);
};

std::string charToString(char str, std::string formatStr){
  char buffer[10];
  sprintf(buffer , formatStr.c_str(), str);
  return (buffer);
};

std::string intToString(int value, std::string formatStr){
  char buffer[10];
  sprintf(buffer , formatStr.c_str(), value);
  return (buffer);
};


// On initialie nos variables
int btnDemmarage;
int tempsSechage = 20;
float tempBoisMin = 25.0;
int compteur = 0;


//Thread qui permet de LOOPER et lire le contenu du port serie
//avec l'aide du la fonction getValidsDatasIfExists
void readStoneData() {
  datasRead rd = myStone->getValidsDatasIfExists();
  //std::cout << "GData : " << intToHexa(abs(rd.id)) << " " << rd.command << " " << rd.name << " " << rd.type << "\n";
  switch(rd.id){

      case 0x0002: { //Version
          std::cout << "GData : " << intToHexa(abs(rd.id)) << " " << rd.command << " " << rd.name << " " << rd.type << "\n";
          std::string stoneVersion = rd.name;
          std::cout << "Version : " <<  stoneVersion.c_str() << "\n";

          //std::this_thread::sleep_for(std::chrono::milliseconds(10));

          break;
          }

      case 0x1001: { //Bouton pour le démarrage
          std::cout << "GData : " << intToHexa(abs(rd.id)) << " " << rd.command << " " << rd.name << " " << rd.type << "\n";
          std::string stoneStart = rd.name;
          std::cout << "Bouton de démarrage : " <<  stoneStart.c_str() << "\n"; 
          // Si on appuie sur le bouton du four alors on lance la cuisson
          if(strcmp(stoneStart.c_str(),"btn_demmarage_four")== 0 && rd.type == 1){
            btnDemmarage = 1;
          }
          // Si on appuie sur le bouton de retour on revient au splash
          if(strcmp(stoneStart.c_str(),"btn_retour")== 0 && rd.type == 1){
            myStone->changePage("home_page");
          }
          // Si on appuie sur le bouton de window alors on va vers le four
          if(strcmp(stoneStart.c_str(),"btn_window")== 0 && rd.type == 1){
            myStone->changePage("window1");
          }
          break;
          }  
      }



  if(rd.id<0) std::cout << "Data received ( id: : " << intToHexa(abs(rd.id)) << "  Command: " << rd.command << " Type: " << rd.type<< ")\n";
}


void setup() {
  
  Serial.begin(9600);

  Serial.println("Stone serial Txd is on pin: "+String(TXD2));
  Serial.println("Stone serial Rxd is on pin: "+String(RXD2));
  myStone = new MyStone(115200, SERIAL_8N1, RXD2, TXD2);


  myButtonT4->init(T4);
  myButtonT4->autoSensibilisation(); //Trouve la sensibilité automatiquement

  myButtonT5->init(T5);
  myButtonT5->autoSensibilisation(); //Trouve la sensibilité automatiquement

  myTemp = new MyTemp();

  // On vient initialisé tous nos labels
  // D'abord on fait un readStoneData()
  // Et ensuite on peut mettre nos valeurs dans leurs labels
  // On se met de base sur le splash
  readStoneData();

  myStone->changePage("home_page");

  if(myStone) myStone->setLabel("lbl_bois_carac", "Erable");

  if(myStone) myStone->setLabel("lbl_type_carac", "Dur");
  if(myStone) myStone->setLabel("lbl_type_bois", "Dur");

  if(myStone) myStone->setLabel("lbl_origine_carac", "US");

  char sechage[100];
  sprintf(sechage , "%d secondes", tempsSechage);
  if(myStone) myStone->setLabel("lbl_temps_sechage_carac", sechage);

  char tempMin[100];
  sprintf(tempMin , "%0.2f°C", tempBoisMin);
  if(myStone) myStone->setLabel("lbl_temps_min_carac", tempMin);

  char compteurTempMin[100];
  sprintf(compteurTempMin , "/%d s", tempsSechage);
  if(myStone) myStone->setLabel("lbl_temps_sechage", compteurTempMin);

  char indiceTemp[100];
  sprintf(indiceTemp , "(min : %0.2f°Celsius)", tempBoisMin);
  if(myStone) myStone->setLabel("lbl_info_temps_min", indiceTemp);

  if(myStone) myStone->setLabel("lbl_info_temp", "");

  cout << std::string("Début de l'exemple Stone de base pour le ESP32")  << "\n";
}

void loop() {
 
  readStoneData();

  // On vient récuupérer la température et l'afficher sur notre écran Stoone
  float temp = myTemp->ReadTemp();

  char label10[50];
  sprintf(label10 , "%0.2f °Celsius", temp);
  if(myStone) myStone->setLabel("lbl_temps_four", label10);

  // Speudocode
  // Lorque l'on presse le boutton "Démarrage du four"
  // On va donc venir vérifier que l'on appuie bien sur le bon boutton
  // Alors on vérifie si la température du four est en accord avec la température demandé
  // Et on peut alors démarrer le four et le compte à rebours.
  // On vérifie à chaque instant que la température est toujours correct
  if (btnDemmarage == 1)
  {
    if (temp >= tempBoisMin*0.90 && temp <= tempBoisMin*1.10){
      while (compteur<tempsSechage && (temp >= tempBoisMin*0.90 && temp <= tempBoisMin*1.10))
      {
        // On incrémente le compteur
        compteur++;

        if(myStone) myStone->setLabel("lbl_info_temp", "La température est bonne");

        // On vient afficher notre compteur et continuer a prendre la température
        // Sur notre écran Stone
        char label7[10];
        sprintf(label7 , "%d s", compteur);
        if(myStone) myStone->setLabel("lbl_temps_compteur", label7);

        temp = myTemp->ReadTemp();
        char label10[50];
        sprintf(label10 , "%0.2f °Celsius", temp);
        if(myStone) myStone->setLabel("lbl_temps_four", label10);

        delay(1000);
      }

      // On vient tout mettre à zéro pour recommencer une cuisson après
      btnDemmarage  = 0;
      
      if(myStone) myStone->setLabel("lbl_info_temp", "Cuisson terminé");

    }
    // Si la température est trop basse on le notifie
    else if (temp < tempBoisMin*0.90 )
    {
      if(myStone) myStone->setLabel("lbl_info_temp", "Température trop basse");
    }
    // Si la température est trop haute on le notifie
    else if (temp > tempBoisMin*1.10 )
    {
      if(myStone) myStone->setLabel("lbl_info_temp", "Température trop haute");
    }

    // On vient tout mettre à zéro pour recommencer une cuisson après
    char compteurSec[100];
    sprintf(compteurSec , "%d s", compteur);
    if(myStone) myStone->setLabel("lbl_temps_compteur", compteurSec);

    compteur = 0;
  }
  delay(2000);
}