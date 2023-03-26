#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClient.h>

const char* SSID = "ImeWifija";
const char* PASSWORD = "lozinka";

String phoneNumber = "+385977275701"; //promjeni na svoj broj mobitela
String apiKey = "7856952"; // kada podesiš sve dobiti ćeš api key. Zaljepi ga ovdje


const int DODIR_PIN = 4;
const int DODIR_GRANICA = 40;
const int LED_PIN = 2;
const int DEBOUNCE_DELAY = 100; // Debounce delay in milliseconds
const int BUZZER_PIN = 13;

// Globalne variable
bool dodir = false;
bool securityMode = false;
bool vrataOtvorenaNaPocetku = false;
unsigned long lastTouchMillis = 0;

int vrijednost;

int prosjek_otvorenih, odstupanje_otvorenih;
int prosjek_zatvorenih, odstupanje_zatvorenih;
int dozvoljena_greska;

void zabiljezi_dodir()
{
  unsigned long currentMillis = millis();
  if (currentMillis - lastTouchMillis > DEBOUNCE_DELAY) {
    dodir = true;
    lastTouchMillis = currentMillis;
  }
}

void trepni(int broj_treptaja, int diley)
{
  for (int i = 0; i < broj_treptaja; i++)
  {
     digitalWrite(LED_PIN, HIGH);
     delay(diley);
     digitalWrite(LED_PIN, LOW);
     delay(diley);
  }
}

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

void cekaj_dodir() {
  dodir = false;
  while (!dodir) {
    delay(1);
  }
  trepni(1, 330);
  dodir = false;
}

void izmjeri_vrijednosti_zatvorenih_vrata() {
  prosjek_zatvorenih = odredi_prosjek();
  odstupanje_zatvorenih = odredi_odstupanje(prosjek_zatvorenih);

  Serial.print("Prosječna vrijednost hall senzora dok su vrata zatvorena je: ");
  Serial.println(prosjek_zatvorenih);
  Serial.print("Odstupanje hall senzora dok su vrata zatvorena je: ");
  Serial.println(odstupanje_zatvorenih);
  trepni(2, 150);
}

void izmjeri_vrijednosti_otvorenih_vrata() {
  prosjek_otvorenih = odredi_prosjek();
  odstupanje_otvorenih = odredi_odstupanje(prosjek_otvorenih);

  Serial.print("Prosječna vrijednost hall senzora dok su vrata otvorena je: ");
  Serial.println(prosjek_otvorenih);
  Serial.print("Odstupanje hall senzora dok su vrata otvorena je: ");
  Serial.println(odstupanje_otvorenih);
  trepni(2, 150);
}

void sendMessage(String message) {
  String url = "http://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&text=" + message + "&apikey=" + apiKey;
  Serial.println(url);
  WiFiClient client;
  HTTPClient http;
  http.begin(client, url);

  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpResponseCode = http.POST(url);
  if (httpResponseCode == 200) {
    Serial.print("Message sent successfully");
  } else {
    Serial.println("Error sending the message");
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void svirajAlarm()
{
  for (int i = 0; i < 3; i++)
  {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    delay(100);
  }
}


void alarm() {
  svirajAlarm();
  sendMessage("ULJEZ");
}

void podesiWifi()
{
  WiFi.begin(SSID, PASSWORD);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  touchAttachInterrupt(DODIR_PIN, zabiljezi_dodir, DODIR_GRANICA);
  Serial.begin(115200);

  Serial.println("Zatvorite vrata i dotaknte dugme \n(pin D4 reagira na touch)");
  cekaj_dodir();
  izmjeri_vrijednosti_zatvorenih_vrata();

  delay(600);

  Serial.println("Otvorite vrata i dotaknite dugme");
  cekaj_dodir();
  izmjeri_vrijednosti_otvorenih_vrata();

  int ukupno_odstupanje = odstupanje_zatvorenih + odstupanje_otvorenih;
  if (abs(prosjek_otvorenih - prosjek_zatvorenih) < ukupno_odstupanje) {
    Serial.println("!!!GREŠKA!!!");
    Serial.println("ESP ne osjeti dovoljno veliku razliku između zatvorenih i otvorenih vrata");
    while (true) {
      trepni(1, 500);
    }
  }

  Serial.println("ESP osjeti dovoljno veliku razliku između otvorenih i zatvorenih vrata");
  Serial.println("ESP sada ulazi u loop() funkciju gdje će pokazivati s lampicom kada osjeti da su vrata zatvorena");

  dozvoljena_greska = abs(prosjek_otvorenih - prosjek_zatvorenih) - ukupno_odstupanje;
  podesiWifi();
}

void loop() {
  if (dodir) {
    dodir = false;
    securityMode = !securityMode;
    if (securityMode) {
      Serial.println("Security mode ON");
      vrataOtvorenaNaPocetku = !vrata_zatvorena();
      if (vrataOtvorenaNaPocetku) {
        Serial.println("Čekam da se vrata zatvore...");
      } else {
        Serial.println("Nadzirem vrata...");
      }
    } else {
      Serial.println("Security mode OFF");
    }
  }

  if (securityMode) {
    if (vrataOtvorenaNaPocetku) {
      if (vrata_zatvorena()) {
        vrataOtvorenaNaPocetku = false;
        Serial.println("Vrata zatvorena. Nadzirem vrata...");
      }
    } else {
      if (!vrata_zatvorena()) {
        Serial.println("ALARM: Vrata otvorena!");
        alarm();
      }
    }
  }
}