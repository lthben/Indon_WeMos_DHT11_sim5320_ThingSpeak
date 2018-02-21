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

bool send_thingspeak_3g(float data1, float data2) {
  String data = "";
  data += "/update?api_key=";
  data += myWriteAPIKey;
  data += "&field1=";
  data += data1;
  data += "&field2=";
  data += data2;
  
  char buffer[200];
  data.toCharArray(buffer, sizeof(buffer));

  return syncServer("api.thingspeak.com", buffer, 80);
}

bool syncServer(char* server, char* path, int port) {
  char dataBuffer[50];
  sprintf(dataBuffer, "AT+CHTTPACT=\"%s\",%i", server, port);

  Serial.print("Connecting to server.. ");
  if (sendATCommandWaitResponse(dataBuffer, "REQUEST", 20000)) {
    Serial.println("OK!");
  }
  else {
    Serial.println("FAILED!");
    return false;
  }

  Serial.print("Sending data.. ");

  char endChar[2];
  endChar[0] = 0x1a;
  endChar[1] = '\0';

  fonaSS.print("GET ");
  fonaSS.print(path);
  fonaSS.print(" HTTP/1.1\r\nHost: ");
  fonaSS.print(server);
  fonaSS.print("\r\n");
  fonaSS.print("User-Agent: Arduino");
  fonaSS.print("\r\n\r\n");
  fonaSS.print(endChar);

  long startTime = millis();
  int pointer = 0;
  char reply[100];

  // if not timed out, check for reply
  while (millis() - startTime < 5000) {
    if (fonaSS.available()) {
      char c = fonaSS.read();
      reply[pointer++] = c;
      reply[pointer] = '\0';
      delay(2);
    }

    // check reply for the response we want
    if (strstr(reply, "OK") != NULL) {
      Serial.println("OK!");
      return true;
    }

    yield();
  }
  Serial.println("FAILED!");
  return false;
}

bool sendATCommandWaitResponse(char* command, char* response, long timeout) {
  char reply[100];
  fonaSS.println(command);

  long startTime = millis();
  int pointer = 0;

  // if not timed out, check for reply
  while (millis() - startTime < timeout) {
    if (fonaSS.available()) {
      char c = fonaSS.read();
      reply[pointer++] = c;
      reply[pointer] = '\0';
    }

    // check reply for the response we want
    if (strstr(reply, response) != NULL) {
      return true;
    }

    yield();
  }

  return false;
}
