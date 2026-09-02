#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUTTON 0

//SPI komunikacija kar pomeni da imamo 5V, GND, SDA/MOSI -> za transefart data, DC -> pove kaksna data
//CS/SS -> pove kateri chip (lahko je vec SPI komunikacij), CLK/SKC -> ura, RST -> reset

U8G2_ST7305_300X400_F_4W_SW_SPI display(
    U8G2_R1,11,12,40,5,41
);

void setup() {
    Serial.begin(9200);
    display.begin();
    display.clear();
    
    pinMode(13,OUTPUT);
}

int x = 10;
int y = 100;
char* string;

char* frankenSteinnanjeStringov(const char* stvar, int podatek, const char* enota){
  string = (char*)calloc(sizeof(char),(strlen(stvar) + strlen(enota) + 10 ));
  strcat(string,stvar);
  char* temp = (char*)calloc(sizeof(char),5);
  sprintf(temp,"%d ",podatek);
  strcat(string,temp);
  strcat(string,enota);
  free(temp);
  return string;
}

void pocistiStevilko(){
  display.setDrawColor(0);
  display.drawBox(185,32,70,20);
  display.setDrawColor(1);
}

bool on = true;

void loop() {
  char* a = frankenSteinnanjeStringov("Temperatura: ",23,"C");
  display.setFont(u8g2_font_ncenB14_tr);
  display.drawStr(50, 50, a);
  free(a);
  display.drawStr(50, 150, "Vlaznost:");
  display.sendBuffer();
  
  delay(250);
  digitalWrite(13,on);
  on = !on;

  display.sendBuffer();
}