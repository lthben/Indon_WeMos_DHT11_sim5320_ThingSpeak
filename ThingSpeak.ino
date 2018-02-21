void send_thingspeak(float data1, float data2) {
  Serial.println("Posting results to ThinkSpeak");
  // Update the 2 ThingSpeak fields with the new data
  ThingSpeak.setField(1, data1);
  ThingSpeak.setField(2, data2);

  int result = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (result = 200) {
    Serial.println(F("Results Posted, closing connection"));
  }
  else
  {
    Serial.print(F("Unable to post result, closing connection : "));
    Serial.println(result);
  }
}

