float dht_humid, dht_temp;

String get_a_dht_reading() { //do not run more often than once every few seconds
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  dht_humid = dht.readHumidity();
  // Read temperature as Celsius (the default)
  dht_temp = dht.readTemperature();

  //  delay(3000); //at least 3 second interval between readings

  // Check if any reads failed and exit early (to try again).
  if (isnan(dht_humid) || isnan(dht_temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
  }

  String myString = "";
  myString = "Temperature: " + String(dht_temp) + " *C " + " | Humidity: " + String(dht_humid) + " %";
  Serial.println(myString);

  return myString;
}

void send_dht_thingspeak() {
  //Send dht11 readings continuously via Thingspeak
  if (millis() - lastLoggedTime > LOGINTERVAL) {
    get_a_dht_reading();
    Serial.println(F("Sending a DHT11 reading via ThingSpeak ... "));
    send_thingspeak_3g(dht_temp, dht_humid);
    lastLoggedTime = millis();
  }
}
