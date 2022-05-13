/*
 * MyStone.cpp
 *
 *  Created on: 02.05.2022
 *      Author: Enzo Richard
 */
#include "MyTemp.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN 21    // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22    // DHT 22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);

// On vient initialiser notre dhtt
MyTemp::MyTemp(){

    dht.begin();
};

// On récupère la température
float MyTemp::ReadTemp(){

  //read temperature and humidity
  float t = dht.readTemperature();
  
  return t;
}