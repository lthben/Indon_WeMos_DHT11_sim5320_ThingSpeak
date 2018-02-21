#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"
#include <DHT.h>
#include <ThingSpeak.h>

// USER SETTINGS
const unsigned int LOGINTERVAL = 10000; //in milliseconds
String myMobileNum = "97974063"; //no need international code in front, just use local number
unsigned long myChannelNumber = 428538; //from own Thingspeak account
const char*  myWriteAPIKey = "1N312W3GMY8D2N6R"; //from own Thingspeak account

#define FONA_RX D6
#define FONA_TX D7
#define FONA_RST 4

SoftwareSerial fonaSS = SoftwareSerial(FONA_RX, FONA_TX);
SoftwareSerial *fonaSerial = &fonaSS;

Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);

#define DHTPIN D3     // what digital pin we're connected to (WeMos pullup pins are D3 and D4)
#define DHTTYPE DHT11 // DHT 22  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE);

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);
unsigned long lastLoggedTime;

void setup() {

  Serial.begin(115200);
  fonaSerial->begin(115200);

  fonaSS.println(F("AT+IPR=9600"));  // Set baud to 9600
  delay(100); // Let the command run
  fonaSerial->begin(9600);
  
  if (! fona.begin(*fonaSerial)) {
    Serial.println(F("Couldn't find FONA"));
    while (1);
  }
  Serial.println(F("Enabling GPS...allow up to 15min to get initial fix"));
  fona.enableGPS(true);
  Serial.println(F("FONA is ready"));

  pinMode(DHTPIN, INPUT_PULLUP);
  dht.begin();
  delay(1000);
  Serial.println("DHT Ready");

  //  ThingSpeak.begin(client); //need to change to GPRS settings

  print_menu();
}

void loop() { //choose only one of the following, comment out the others
    
//  run_menu();

  send_dht_thingspeak();

//  send_gps_thingspeak();
}


