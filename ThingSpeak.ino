void send_thingspeak(float data1, float data2) {
    //This is for wifi version
    
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
    //This is for 3g version
    
  String data = "";
  data += "/update?api_key=";
  data += myWriteAPIKey;
  if (typeOfDataToSend==0) {
    data += "&field1="; //temperature
  } else {
    data += "&field3="; //latitude
  }
  data += data1;
   if (typeOfDataToSend==0) {
    data += "&field2="; //humidity
  } else {
    data += "&field4="; //longitude
  }
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

void read_cloud_data() {
  char buffer[100];

  sprintf(buffer, "AT+CHTTPACT=\"%s\",%i", "api.thingspeak.com", 80);
  Serial.print("Connecting to server.. ");
  if (sendATCommandWaitResponse(buffer, "REQUEST", 20000)) {
    Serial.println("OK!");
  }
  else {
    Serial.println("FAILED!");
    return;
  }

  String data = "";
  data += "/channels/";
  data += myChannelNumber;
  data += "/fields/";
  data += whichField;
  data += ".json?api_key="; 
  data += myReadAPIKey;
  data += "&results=1"; //default to 1 data point only
  data.toCharArray(buffer, sizeof(buffer));

  Serial.print("Sending data.. ");

  char endChar[2];
  endChar[0] = 0x1A;
  endChar[1] = '\0';

  fonaSS.print("GET ");
  fonaSS.print(buffer);
  fonaSS.print(" HTTP/1.1\r\nHost: api.thingspeak.com");
  fonaSS.print("\r\n");
  fonaSS.print("User-Agent: Arduino");
  fonaSS.print("\r\n\r\n");
  fonaSS.print(endChar);

  long startTime = millis();
  int pointer = 0;
  bool collectData = false;

  // if not timed out, check for reply
  while (millis() - startTime < 5000) {
    if (fonaSS.available() && pointer < 100) {
      char c = fonaSS.read();

      if (c == '[') {
        collectData = true;
      }
      else if (c == ']') {
        break;
      }

      if (collectData) {
        buffer[pointer++] = c;
        buffer[pointer] = '\0';
      }
    }

    yield();
  }

  if (collectData) {
    Serial.println("OK!");

    char *index = strchr((char *)(buffer), ',');
    index++;
    char *index1 = strchr((char *)(index), ',');
    index1++;
    index = strchr((char *)(index1), ':');
    index += 2;
    index1 = strchr((char *)(index), '"');
    if (index1 != NULL) {
      *index1 = 0;
      strcpy(index, (char *)(index));
      cloudDataValue = atof(index);
    }
    Serial.print("cloudDataValue from field ");
    Serial.print(whichField);
    Serial.print(": ");
    Serial.println(cloudDataValue);
  }
  else {
    Serial.println("FAILED!");
  }
}


