/*
 *  --- Schéma ---
 *        _____
 *  RST -|•    |- VCC
 *  PB3 -|     |- PB2
 *  PB4 -|     |- PB1
 *  GND -|_____|- PB0
 *  
*/

/*  --- Variables déclarées de manière globale ---  */
/*
#define PB0 0
#define PB1 1
#define PB2 2
#define PB3 3
#define PB4 4
*/
long unsigned int *inter = NULL; // Pointeur destiné à stocker les intervalles de temps
unsigned int taille(0); // Taille du tableau dynamique

// Adressage des différentes broches
#define temp 0
#define prg 1
#define buzz 3
#define del 2

#define buzzfreq 440

void tempo(); // Fonction qui gère le tempo
void prog();  // Fonction qui gère la programmation du tempo
void action(bool etat = 0, int freq = 440);

void setup() {
  pinMode(temp, INPUT);
  pinMode(prg, INPUT);
  pinMode(del, OUTPUT);
  pinMode(buzz, OUTPUT);
}

void loop() {
  tempo();
  prog();
}

void tempo()  {
  static unsigned long temps = millis();
  bool etat = false;
  static unsigned int etape = -1;
  if (taille)  {
    if(etape==-1) {
      temps = millis();
      etape++;
    }

    if(millis()-temps < inter[etape]) action(etape%2, buzzfreq);
    else  {
      temps = millis();
      etape++;  
    }
    
    if(etape==taille) etape = -1;
  }
  else
    while (!taille) prog();
}

void prog() {
  static bool mode = false;
  mode = digitalRead(prg);
  action();
  
  taille = 0;
  while (mode) {  // Nombre de changements
    while (mode) {
      if(digitalRead(temp)) taille++;
      if(digitalRead(prg) && taille) mode = 0;
    }
    delete inter;
    taille *= taille;
    *inter = new long unsigned int [taille];
    
    mode = 1;
    unsigned long tmps=0;
    
    while (!tmps) if(digitalRead(temp)) tmps = millis(); // On attend l'appuie sur le bouton tempo
    unsigned short t=0;
    
    while (mode)  { // On récupère les délais (tempo,...)
      if(digitalRead(temp) && !(t%2))  { // Temps du silence/éteint
        inter[t] = millis() - tmps;
        tmps = millis();
        t++;
      }
      if(!digitalRead(temp) && (t%2))  { // Temps du son/allumé
        inter[t] = millis() - tmps;
        tmps = millis();
        t++;
      }
      if(t==taille) mode=0;
    }
  }
}

void action(bool etat = 0, int freq = 440)  {
  if (etat) {
    digitalWrite(del, 1);
    tone(buzz, freq);
  }
  else {
    digitalWrite(del, 0);
    noTone(buzz);
  }
}
