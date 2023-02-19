# home-alarm-system

Door attachable alarm system using ESP32 and a magnet

Ideja je staviti magnet na vrata, i ESP na štok tako da je metalno kućište od ESP-a kraj magneta kada su vrata zatvorena. ESP osjeti kada je magnet blizu i kada nije. Tako će ESP znati kada netko otvori vrata i moći nas obavjestiti o tome.

Ovdje ćeš naći sav kod za svoj završni i izvore. Ako samo prevedeš što je napisano na ovim linkovima imaš cijeli tekstualni dio završnog riješen.

Da bi sam mogao pokrenuti program, prvo želiš instalirati Arduino IDE, i napraviti da radi s ESP mikroupravljačima.
Kako to napraviti možeš naći ovdje:
https://randomnerdtutorials.com/installing-esp32-arduino-ide-2-0/
Board name pločice koju ti imaš je ESP32 Dev Module.

Želiš se pozvati na prve konzultacije kod profesora i demonstrirati mu kod koji se nalazi u HallEffect_vrata mapi. 
Nije teško. Isprobaj ga prije nego ga počneš čitati.
Dio koji ti želiš dirati je onaj na dnu, u loop() funkciji.
Predlažem da spojiš ESP i uploadaš kod na njega. 
Otvoriš Serial Monitor i pratiš što ti kaže.
Sjeti se postaviti BAUD na 115200
Pin D4 je onaj koji reagira na dodir.
Ako ti fali teksta za završni, možeš pisati o pinovima na ESP-u. Više o njima možeš naći ovdje:
https://randomnerdtutorials.com/esp32-pinout-reference-gpios/

Prepravi komentare da se ne vidi da sam ih ja pisao.
Ako ne znaš kako ih prepraviti, najbolje da obrišeš. Oni su tu da tebi kažu što je ideja iza svake linije koda

Kako se koristi hall effekt senzor na ESP-u možeš naći ovdje:
https://randomnerdtutorials.com/esp32-hall-effect-sensor/

Za druge konzultacije par tjedana nakon prvih mu želiš pokazati ovo i predložiti da kada se vrata otvore, ESP pošalje poruku na WhatsApp.
Kako se to radi, možeš naći ovdje:
https://randomnerdtutorials.com/esp32-send-messages-whatsapp/

Ako profesor kaže da je ideja dobra, onda ću ti složiti taj kod.

# na kraju

Nisam htjeo učvrstiti magnet i ESP na vrata tako da možeš mjenjati mjesto gdje je na vratima kako ti paše.
Isto tako ako, to sam napraviš onda si sigurno završni napravio ti.
Ja bih 3D printao kućište za magnet i ESP koje se može ušarafiti u vrata. Ali možeš ih i zaljepiti.

# good luck