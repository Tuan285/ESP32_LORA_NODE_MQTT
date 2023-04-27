void print_data_to_lcd() {
  if (ttClearLcd == 1) {
    lcd.clear();
    ttClearLcd = 0;
    //checkDevices = 1;
  }
  lcd.setCursor(7, 0);
  lcd.print("HELLO");
  lcd.setCursor(0, 1);
  lcd.print("%H");
  lcd.setCursor(7, 1);
  lcd.print("T:");
  lcd.write(1);
  lcd.print("C");
  lcd.setCursor(14, 1);
  lcd.print("T:");
  lcd.write(1);
  lcd.print("F");
  if (checkDevices == 1) {
    lcd.setCursor(0, 2);
    lcd.print("NO CONNECT DEVICE 1");
    if (countSend == 5) {
      lcd.clear();
    }
  }
  if (checkDevices == 0) {
    if (dataErr == 0) {
      lcd.setCursor(0, 2);
      lcd.print(h1);
      lcd.setCursor(7, 2);
      lcd.print(t1);
      lcd.setCursor(14, 2);
      lcd.print(fT1);
    }
    if (dataErr == 1) {
      lcd.setCursor(0, 2);
      lcd.print("DATA ERROR !!");
    }
  }
}
