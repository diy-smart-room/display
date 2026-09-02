#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUTTON 14

//SPI komunikacija kar pomeni da imamo 5V, GND, SDA/MOSI -> za transefart data, DC -> pove kaksna data
//CS/SS -> pove kateri chip (lahko je vec SPI komunikacij), CLK/SKC -> ura, RST -> reset

U8G2_ST7305_300X400_F_4W_SW_SPI display(
    U8G2_R1,11,12,40,5,41
);

struct _podatki{
  int x;        //x pa y sta koordinati za ciscenje
  int y;
  const char* enota;
  const char* stvar;
};

char* string;
int trenuten_cas;
int debounce_cas = 0;   //potrebno za debounce
struct _podatki** arrPodatkov;

void setup() {
  Serial.begin(9200);
  display.begin();
  display.clear();
  display.setFont(u8g2_font_ncenB14_tr);
  
  pinMode(BUTTON,INPUT);
  
  arrPodatkov = (struct _podatki**)calloc(sizeof(struct _podatki*),2);
  
  for(int i = 0; i < 2; i++){
    arrPodatkov[i] = (struct _podatki*)calloc(sizeof(struct _podatki),1);
  }

  arrPodatkov[0]->enota = "C";
  arrPodatkov[0]->stvar = "Temperatura: ";
  arrPodatkov[0]->x = 185;
  arrPodatkov[0]->y = 32;
}

char* frankenSteinnanjeStringov(int id, int st){
  string = (char*)calloc(sizeof(char),(strlen(arrPodatkov[id]->enota) + strlen(arrPodatkov[id]->stvar) + 10 ));
  strcat(string,arrPodatkov[id]->stvar);
  char* temp = (char*)calloc(sizeof(char),5);
  sprintf(temp,"%d ",st);
  strcat(string,temp);
  strcat(string,arrPodatkov[id]->enota);
  free(temp);
  return string;
}

void pocistiStevilko(int id){   //indeks structa da dobimo koordinate brisanja
  display.setDrawColor(0);
  display.drawBox(arrPodatkov[id]->x,arrPodatkov[id]->y,70,20);
  display.setDrawColor(1);            //barvo damo najprej na nic nato pa na 1, s tem simuliramo raderko
}

void narisi(int id, int st){      //id pove kateri struct vzamemo, st pa na katero cifro ga damo               
  pocistiStevilko(id);   //najprej pocistimo stvari, nato pa na tisto mesto narisemo stavri
  char* a = frankenSteinnanjeStringov(id,st);
  display.drawStr(50, 50, a);
  free(a);
  display.sendBuffer();       //s tem naeknrat damo gor celo sliko, ki smo jo prej ustvarili
}

void loop() {
  if(digitalRead(BUTTON) == 1){
    trenuten_cas = millis();
    if(trenuten_cas > debounce_cas){
      narisi(0,23);
      debounce_cas = 500 + trenuten_cas;
    }
  }
}