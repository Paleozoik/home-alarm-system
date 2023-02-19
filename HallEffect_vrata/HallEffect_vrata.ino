#include <Arduino.h>


//###########################################################
//###                                                     ###
//###                 VARIJABLE                           ###
//###                                                     ###
//###########################################################


//kada ne diramo pin, ESP vidi vrijednost 70 na njemu
//kada ga dotaknemo, ESP vidi vrijednost oko 10 na njemu
//granica dodira je vrijednost koju pin mora prijeći da bi dodir bio zabilježen
const int granica_dodira = 40;
const int ledica = 2;

//PRIVREMENE VARIJABLE (stalno se koriste i mjenjaju)
int vrijednost; //ovdje spremamo vrijednost magnetskog polja koju očitavamo sa hall senzora
bool dodir = false; //kada dotaknemo pin 4, poziva se funkcija zabiljezi_dodir() koja postavi dodir na true

//TRAJNE VARIJABLE (određujemo ih u setup funkciji)
int prosjek_otvorenih, odstupanje_otvorenih;
int prosjek_zatvorenih, odstupanje_zatvorenih;
int dozvoljena_greska;


//###########################################################
//###                                                     ###
//###                 POMOĆNE FUNKCIJE                    ###
//###                                                     ###
//###########################################################

void zabiljezi_dodir()
{
  dodir = true;
}

void trepni(int broj_treptaja, int diley)
{
  for (int i = 0; i < broj_treptaja; i++)
  {
     digitalWrite(ledica, HIGH);
     delay(diley);
     digitalWrite(ledica, LOW);
     delay(diley);
  }
}

//vrijednosti koje hall senzor daje, odstupaju dosta
//zato uzimamo prosjek na 10 mjerenja, da smanjimo odstupanja
int ocitaj_hall_senzor()
{
  int suma = 0;
  for (int i = 0; i < 10; i++)
  {
    suma += hallRead();
  }
  return suma / 10;
}


int odredi_prosjek()
{
  int suma = 0;
  for (int i = 0; i < 100; i++)
  {
    vrijednost = hallRead();
    suma += vrijednost;
    delay(1);
  }
  return suma / 100;
}

int odredi_odstupanje(int prosjek)
{
  int max_odstupanje = 0;
  int trenutno_odstupanje;
  for (int i = 0; i < 100; i++)
  {
    vrijednost = ocitaj_hall_senzor();
    trenutno_odstupanje = abs(vrijednost - prosjek);
    if (trenutno_odstupanje > max_odstupanje)
    {
      max_odstupanje = trenutno_odstupanje;
    }
  }
  return max_odstupanje;
}

bool vrata_zatvorena()
{
  vrijednost = ocitaj_hall_senzor();
  return abs(vrijednost - prosjek_zatvorenih) < dozvoljena_greska;
}


//###########################################################
//###                                                     ###
//###                 POČETAK PROGRAMA                    ###
//###                                                     ###
//###########################################################

void setup() {
  //POČETNI SETUP
  pinMode(ledica, OUTPUT); //Plava ledica na ESP neka radi kao output
  touchAttachInterrupt(4, zabiljezi_dodir, granica_dodira); //pin D4 neka radi kao dugme na touch
  Serial.begin(115200); //sjeti se postaviti BAUD na SERIAL MONITORu na 115200


  //ČEKA DODIR DA ODREDI VRIJEDNOST KOJU HALL EFEKT SENZOR VIDI KADA SU VRATA ZATVORENA
  dodir = false; //ako smo slučajno dotakli dugme nakon paljenja stavimo dodir na false
  Serial.println("Zatvorite vrata i dotaknte dugme \n(pin D4 reagira na touch)");
  while(!dodir){delay(1);} //čekaj dodir
  trepni(1, 330); //trepni da pokažeš da je dodir registriran;
  dodir = false; //resetiraj dodir

  //ODREDI VRIJEDNOST KOJU HALL EFEKT SENZOR VIDI KADA SU VRATA ZATVORENA
  prosjek_zatvorenih = odredi_prosjek();
  odstupanje_zatvorenih = odredi_odstupanje(prosjek_zatvorenih);

  //ISPIŠI VRIJEDNOSTI NA SERIAL MONITOR
  Serial.print("Prosječna vrijednost hall senzora dok su vrata zatvorena je: ");
  Serial.println(prosjek_zatvorenih);
  Serial.print("Odstupanje hall senzora dok su vrata zatvorena je: ");
  Serial.println(odstupanje_zatvorenih);
  trepni(2, 150); //trepne da pokaže da je uspješno izmjerio

  //MALI VREMENSKI RAZMAK DA SE NE POMJEŠAJU DVA PRITISKA DUGMETA
  delay(600);

  //ČEKAJ DODIR DA ODREDIŠ VRIJEDNOST KOJU HALL EFEKT SENZOR VIDI KADA SU VRATA OTVORENA
  Serial.println("Otvorite vrata i dotaknite dugme");
  while(!dodir){delay(1);} //čekaj dodir
  trepni(1, 330); //trepni da pokažeš da je dodir registriran;
  dodir = false; //resetiraj dodir

  //ODREDI VRIJEDNOST KOJU HALL EFEKT SENZOR VIDI KADA SU VRATA OTVORENA
  prosjek_otvorenih = odredi_prosjek();
  odstupanje_otvorenih = odredi_odstupanje(prosjek_otvorenih);

  //ISPIŠI VRIJEDNOSTI NA SERIAL MONITOR
  Serial.print("Prosječna vrijednost hall senzora dok su vrata otvorena je: ");
  Serial.println(prosjek_otvorenih);
  Serial.print("Odstupanje hall senzora dok su vrata otvorena je: ");
  Serial.println(odstupanje_otvorenih);
  trepni(2, 150); //trepni da pokažeš da je uspješno izmjereno
  
  int ukupno_odstupanje = odstupanje_zatvorenih + odstupanje_otvorenih;
  //PROVJERI DALI JE RAZLIKA IZMEĐU VRIJEDNOSTI OTVORENIH I ZATVORENIH VRATA DOVOLJNO VELIKA
  if (abs(prosjek_otvorenih - prosjek_zatvorenih) < ukupno_odstupanje)
  { //ako razmak nije dovoljno velik
    //onda signaliziraj grešku zauvijek
    Serial.println("!!!GREŠKA!!!");
    Serial.println("ESP ne osjeti dovoljno veliku razliku između zatvorenih i otvorenih vrata");
    while(true) //zauvijek
    {trepni(1, 500);} //signal za GREŠKU
    //ako se ovo desi onda bi ugasili ili resetirali ESP ako smo nešto pogriješili u postupku postavljanja
    //ako nismo pogriješili u postavljanju, onda bi trebali uzeti jači magnet ili približiti ovaj magnet ESPu
  }

  Serial.println("ESP osjeti dovoljno veliku razliku između otvorenih i zatvorenih vrata");
  Serial.println("ESP sada ulazi u loop() funkciju gdje će pokazivati s lampicom kada osjeti da su vrata zatvorena");

  dozvoljena_greska = abs(prosjek_otvorenih - prosjek_zatvorenih) - ukupno_odstupanje;
  //setup je završen i ulazimo u loop
}


//###########################################################
//###                                                     ###
//###            GLAVNI DIO PROGRAMA                      ###
//###                                                     ###
//###########################################################

//možda želiš spojiti mali buzzer na pin 5
//onda bi ovdje stavio ono što je zakomentirano
void loop() 
{
  if (vrata_zatvorena())
  {
    Serial.println("Vrata ZATVORENA");
    digitalWrite(ledica, HIGH);
    //digitalWrite(5, LOW); //gasi buzzer
  }
  else
  {
    Serial.println("Vrata OTVORENA");
    digitalWrite(ledica, LOW);
    //digitalWrite(5, HIGH); //pali buzzer
  }
}
