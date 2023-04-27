void onReceive(int packetSize) {
  //Serial.println("packetSize");
  if (packetSize == 0) return;  // if there's no packet, return
  // read packet header bytes:
  Den2(1);
  delay(50);
  Den2(0);
  int recipient = LoRa.read();  // recipient address
  byte sender = LoRa.read();    // sender address
  //Serial.println(sender, HEX);
  if (sender == 0xBB)
    SenderNode1 = "Node1";
  if (sender == 0xCC)
    SenderNode2 = "Node2";
  byte incomingMsgId = LoRa.read();   // incoming msg ID
  //incomingMsgId_s = incomingMsgId;
  byte incomingLength = LoRa.read();  // incoming msg length
  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }
  // if the recipient isn't this device or broadcast,
  if (incomingLength != incoming.length()) {  // check length for error
    //Serial.println("error: message length does not match length");
    ;
    return;  // skip rest of function
  }
  //Serial.println("recipient");
  if (recipient != Node1 && recipient != MasterNode) {
    // Serial.println("This message is not for me.");
    ;
    return;  // skip rest of function
  }
  if (sender == 0xBB) {  //70.00, 30.20, 86.26
    h1s = getValue(incoming, ',', 0);
    t1s = getValue(incoming, ',', 1);
    fT1s = getValue(incoming, ',', 2);
    //checkDevice =  getValue(incoming, ',', 3);
    h1 = h1s.toFloat();
    t1 = t1s.toFloat();
    fT1 = fT1s.toFloat();
    //checkDevices = checkDevice.toInt();
    incoming = "";
    dataErr = 0;
    ttClearLcd = 1;
    // Serial.print(h1);
    // Serial.print("--");
    // Serial.print(t1);
    // Serial.print("--");
    // Serial.print(fT1);
    //Serial.print("--");
    //Serial.print(checkDevices);
    Serial.println();
    if (h1s == "nan" || t1s == "nan" || fT1s == "nan") {
      dataErr = 1;
      Serial.println("Data Error !");
    }
  }
  if (sender == 0xFF) {
    h2s = getValue(incoming, ',', 0);
    t2s = getValue(incoming, ',', 1);
    fT2s = getValue(incoming, ',', 2);
    //checkDevice =  getValue(incoming, ',', 3);
    h2 = h2s.toFloat();
    t2 = t2s.toFloat();
    fT2 = fT2s.toFloat();
    //checkDevices = checkDevice.toInt();
    incoming = "";
    dataErr = 0;
    ttClearLcd = 1;
    // Serial.print(h2);
    // Serial.print("--");
    // Serial.print(t2);
    // Serial.print("--");
    // Serial.print(fT2);
    //Serial.print("--");
    //Serial.print(checkDevices);
    Serial.println();
    if (h2s == "nan" || t2s == "nan" || fT2s == "nan") {
      dataErr = 2;
      Serial.println("Data Error !");
    }
  }
}
