/*
    --- Schéma ---
          _____
    RST -|•    |- VCC
    PB3 -|     |- PB2
    PB4 -|     |- PB1
    GND -|_____|- PB0

*/

/*  --- Variables déclarées de manière globale ---  */
/*
  #define PB0 0
  #define PB1 1
  #define PB2 2
  #define PB3 3
  #define PB4 4
*/
unsigned long inter[60];
unsigned int taille = 0; // "Taille" du tableau

// Adressage des différentes broches
#define temp 4
#define prg 1
#define buzz 0
#define del 2

#define buzzfreq 880

void tempo(); // Fonction qui gère le tempo
void prog();  // Fonction qui gère la programmation du tempo
void action(bool etat = 0, int freq = 440);

void setup() {
  pinMode(temp, INPUT_PULLUP);
  pinMode(prg, INPUT_PULLUP);
  pinMode(del, OUTPUT);
  pinMode(buzz, OUTPUT);
  action(1, buzzfreq);
  delay(1000);
  action();
}

void loop() {
  tempo();
  prog();
}

void tempo()  {
  static int etape = 0;

  if (taille > 0)  {
    action(etape % 2, buzzfreq);
    delay(inter[etape]);
    etape++;

    if (etape == taille) etape = 0;
  }

  else  {
    while (!taille) prog();
  }
}

void prog() {
  volatile bool mode = false;
  mode = !digitalRead(prg);

  while (mode) {  // Nombre de changements
    action();
    taille = 0;
    while (mode) {
      if (!digitalRead(temp)) {
        taille++;
        tone(buzz, buzzfreq);
        while (!digitalRead(temp));
        noTone(buzz);
        delay(10);
      }
      if (!digitalRead(prg) && taille) {
        mode = 0;
        action(1);
        while (!digitalRead(prg));
        action(0);
      }
    }
    
    for (int i = 0; i < taille; i++)  { // On indique le nombre de temps
      action(0);
      delay(500);
      action(1);
      delay(500);
    }
    action(0);
    
    taille *= 2;
    mode = 1;
    
    volatile unsigned long tmps = 0;

    while (!tmps) { // On attend l'appuie sur le bouton tempo
      if (!digitalRead(temp))  tmps = millis();
    }
    unsigned short t = 0;

    while (mode)  { // On récupère les délais (tempo,...)
      if (!digitalRead(temp) && !(t % 2) && mode)  { // Temps du silence/éteint
        noTone(buzz);
        while (!digitalRead(temp));
        inter[t] = millis() - tmps;
        tmps = millis();
        t++;
      }

      if (t == taille + 1) mode = 0;

      if (digitalRead(temp) && (t % 2) && mode)  { // Temps du son/allumé
        tone(buzz, buzzfreq);
        while (digitalRead(temp));
        inter[t] = millis() - tmps;
        tmps = millis();
        t++;
      }
      if (t == taille + 1) mode = 0;
    }
    noTone(buzz);
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
