#include <Servo.h>
#include <Stepper.h>

const int Steps = 2048; 
const int Velocita = 15;    

#define PIN_FOTO A1
#define PIN_PANNELLO A0
#define PIN_INCLINATORE 5

int posizione = 0; 

Servo inclinatore;
Stepper giratore(Velocita, 8, 10, 9, 11);

/*
  Funzione che gira il pannello da -180 a 180 gradi e returna la posizione con il livello di luce maggiore
  IN:/
  OUT: pos(int): posizione da -180 a 180 dove la luce ha assunto il valore maggiore
*/
int cercaLuce();

/*
  funzione che posiziona lo stepper in una posizione specifica
  IN: angolo(int) valore da 0 a 2048 per girare lo stepper
  OUT: //
*/
void posiziona(int);

/*
  funzione che legge il valore del fotoresistore e calcola l'angolo tramite la funzione di tangenza
  IN: //
  out: angolo (int)in gradi
*/
int calcolaAngolazione();

/*
  funzione che ilclina il pannello solare
  IN: //
  OUT://
*/
void inclina(int);

void setup() {
  Serial.begin(9600);
  inclinatore.write(0);
  
  pinMode(PIN_PANNELLO, INPUT);
  giratore.setSpeed(Steps);
  inclinatore.attach(PIN_INCLINATORE);
  inclinatore.write(35);
}

void loop() {
  int dest = cercaLuce();
  posiziona(dest);
  inclina();
  Serial.println(String(analogRead(PIN_FOTO)*5/1023.0));
  delay(10000);
}


int cercaLuce(){
  int maxLuce = 0; 
  int posMax = 0; 
  int luceRilev = 0; 
  int retPos = 0; 
  //ripristino la posizione del pannello a quella di partena per poi iniziare a girare
  posiziona(0);
  //ciclo che fa girare il pannello
  for(int i = 0; i<36;i++){
    ruota(Steps/36);
    //attesa che il pannello riesca a restituire un valore nuovo
    delay(100);
    luceRilev = analogRead(PIN_PANNELLO); 
    //normale swap se il valore letto è maggiore del vecchio massimo
    if(luceRilev>maxLuce){
      maxLuce = luceRilev;
      retPos = posizione;
    }
  }
  return retPos;
}

void posiziona(int destinazione){
  if(destinazione<0||destinazione>2048) return; 
  //controllo se ho già superato la posizione oppure no
  if(destinazione > posizione){
    //se così allora torno alla posizione 0
    while(posizione!=0){
      ruota(1);
    }
    //ora posso mettermi alla destinazione desiderata
    ruota(destinazione);
  }else{
    //calcolo la differenza e ottengo gli step che devo compiere per raggiungere la destinazione
    ruota(destinazione-posizione);
  }
}

int calcolaAngolazione(){
  int maxLuce = 0; 
  int maxPos = 0;
  //imposto il servo a una posizione di partenza
  inclinatore.write(0);
  for(int i = 0; i<90;i+=10){
    inclinatore.write(i);
    delay(1000);
    //controllo se i livelli sono maggiori di quelli registrati in precedenza
    int currentLuce = analogRead(PIN_PANNELLO);
    if(currentLuce > maxLuce){
      maxLuce = currentLuce;
      maxPos = i; 
    }
  }
  //ritorno la posizione massima
  return maxPos;
}

void ruota(int passi){
  //faccio ruotare fisicamente lo stepper
  giratore.step(passi);
  //aggiorno la posizione logica
  if(posizione+passi>=Steps){
    posizione += passi-Steps; //aggiungo i passi e rimuovo quelli massimi (2045+10-2048 = 7)
  }else{
    posizione+=passi;
  }
}

void inclina(){
  inclinatore.write(calcolaAngolazione());
}

