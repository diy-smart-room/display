#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUTTON_1 14
#define BUTTON_2 13
#define BUTTON_3 44
#define BUTTON_4 17

//SPI komunikacija kar pomeni da imamo 5V, GND, SDA/MOSI -> za transefart data, DC -> pove kaksna data
//CS/SS -> pove kateri chip (lahko je vec SPI komunikacij), CLK/SKC -> ura, RST -> reset

U8G2_ST7305_300X400_F_4W_SW_SPI display(
    U8G2_R1,11,12,40,5,41
);

struct _podatki{
  int x;        //x pa y sta koordinati za ciscenje
  int stevilo;
  int y;
  const char* enota;
  const char* stvar;
};

void posodobiPodatek(int,int);             //posodobi trenutni zasloncek oz. stevilko na zasloncku (samo to)
void spremeniZaslon(int);                 //spremeni zaslonec na prikazovanje nove teme (celoten zaslon)
void pocistiZaslon();               //pocisti celoten zasloncek
void pocistiStevilko(int);
char* frankenSteinnanjeStringov(int, int);
void izracunTrenutnegaID(int);
int testirnikRazlicnihCifr(int);

char* string;
int trenuten_cas;
int debounce_cas[4];   //potrebno za debounce
const int stRazlicnihPodatkov = 3;
int trenutniID = 0;
struct _podatki** arrPodatkov;

void setup() {
  Serial.begin(9200);
  display.begin();
  display.clear();
  display.setFont(u8g2_font_ncenB24_tr);
  
  pinMode(BUTTON_1,INPUT);
  pinMode(BUTTON_2,INPUT);
  pinMode(BUTTON_3,INPUT);
  pinMode(BUTTON_4,INPUT);
  
  arrPodatkov = (struct _podatki**)calloc(sizeof(struct _podatki*),stRazlicnihPodatkov);
  
  for(int i = 0; i < stRazlicnihPodatkov; i++){
    arrPodatkov[i] = (struct _podatki*)calloc(sizeof(struct _podatki),1);
  }

  arrPodatkov[0]->enota = "C";
  arrPodatkov[0]->stvar = "Temperatura: ";
  arrPodatkov[0]->stevilo = 32;
  arrPodatkov[0]->x = 255;
  arrPodatkov[0]->y = 20;


  arrPodatkov[1]->enota = "%";
  arrPodatkov[1]->stvar = "Vlaznost: ";
  arrPodatkov[1]->stevilo = 66;
  arrPodatkov[1]->x = 185;
  arrPodatkov[1]->y = 20;

  arrPodatkov[2]->enota = "%";
  arrPodatkov[2]->stvar = "Freakiness: ";
  arrPodatkov[2]->stevilo = 100;
  arrPodatkov[2]->x = 225;
  arrPodatkov[2]->y = 20;

  posodobiPodatek(trenutniID, arrPodatkov[trenutniID]->stevilo);
}

char* frankenSteinnanjeStringov(int id, int st){        //naredi EN velik string za izpis (seveda ga pol se free-am)
  string = (char*)calloc(sizeof(char),(strlen(arrPodatkov[id]->enota) + strlen(arrPodatkov[id]->stvar) + 10 ));
  strcat(string,arrPodatkov[id]->stvar);
  char* temp = (char*)calloc(sizeof(char),10);
  sprintf(temp,"%d ",st);
  strcat(string,temp);
  strcat(string,arrPodatkov[id]->enota);
  free(temp);
  return string;
}

void pocistiStevilko(int id){   //indeks structa da dobimo koordinate brisanja
  display.setDrawColor(0);
  display.drawBox(arrPodatkov[id]->x,arrPodatkov[id]->y,170,35);
  display.setDrawColor(1);            //barvo damo najprej na nic nato pa na 1, s tem simuliramo raderko
}

void posodobiPodatek(int id, int st){      //id pove kateri struct vzamemo, st pa na katero cifro ga damo               
  pocistiStevilko(id);   //najprej pocistimo stvari, nato pa na tisto mesto narisemo stavri
  arrPodatkov[id]->stevilo = st;
  char* a = frankenSteinnanjeStringov(id,st);
  display.drawStr(25, 50, a);
  free(a);
  display.sendBuffer();       //s tem naeknrat damo gor celo sliko, ki smo jo prej ustvarili
}

void pocistiZaslon(){         //popolnoma scisti zaslon in se tudi buffer, ki se je pripravljal da bi seu gor
  display.clearDisplay();
  display.clearBuffer();
}

void spremeniZaslon(int id){  //za spremembno celotnega zaslona na prikaz druge vsebine
  pocistiZaslon();
  posodobiPodatek(id,arrPodatkov[id]->stevilo);
}

void izracunTrenutnegaID(int i){    //za menjavo med razlicnimi zasloncki
  trenutniID += i;
  if(trenutniID < 0){
    trenutniID = stRazlicnihPodatkov - 1;
  }

  if(trenutniID >= stRazlicnihPodatkov){
    trenutniID = 0;
  }
  
  Serial.printf("ID: %d\n",trenutniID);

  return;
}

int testirnikIndeks = 1;

int testirnikRazlicnihCifr(){         //bilo je za testiranje ce se mogoce pri kaksnih stevilah pusti "sled"
  int a = 1;                          //na zaslonu, od prejsnjega stevila
  for(int i = 0; i < testirnikIndeks; i++){
    a = a * 10;
  }
  return random(a);
}


void loop() {
  if(digitalRead(BUTTON_1) == 1){       //trenutno daje random stevila, ki si sledijo od 0-10, 0-100, . . .
    trenuten_cas = millis();
    if(trenuten_cas > debounce_cas[0]){
      posodobiPodatek(trenutniID,testirnikRazlicnihCifr());
      testirnikIndeks++;
      debounce_cas[0] = 500 + trenuten_cas;
    }
  }

  if(digitalRead(BUTTON_2) == 1){       //pobrise celoten zaslon
    trenuten_cas = millis();
    if(trenuten_cas > debounce_cas[1] ){
      pocistiZaslon();
      debounce_cas[1] = 500 + trenuten_cas;
    }
  }
  
   if(digitalRead(BUTTON_3) == 1){      //shuffle-a workspace v levo
    trenuten_cas = millis();
    if(trenuten_cas > debounce_cas[2] ){
      testirnikIndeks = 1;
      izracunTrenutnegaID(-1);
      spremeniZaslon(trenutniID);
      debounce_cas[2] = 500 + trenuten_cas;
    }
  }

 if(digitalRead(BUTTON_4) == 1){        //shuffle-a wroskapce v  desno (aka. id+1)
    trenuten_cas = millis();
    if(trenuten_cas > debounce_cas[3] ){
      testirnikIndeks = 1;
      izracunTrenutnegaID(1);
      spremeniZaslon(trenutniID);
      debounce_cas[3] = 500 + trenuten_cas;
    }
  }
}