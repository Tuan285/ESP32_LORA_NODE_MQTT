void lcd_node1() {
  lcd.setCursor(0, 2);
  lcd.print(h1);
  lcd.setCursor(7, 2);
  lcd.print(t1);
  lcd.setCursor(14, 2);
  lcd.print(fT1);
}
void lcd_node2() {
  lcd.setCursor(0, 3);
  lcd.print(h2);
  lcd.setCursor(7, 3);
  lcd.print(t2);
  lcd.setCursor(14, 3);
  lcd.print(fT2);
}
void print_data_to_lcd() {
  if (ttClearLcd == 1) {
    lcd.clear();
    ttClearLcd = 0;
    //checkDevices = 1;
  }
  lcd.setCursor(0, 0);
  lcd.print(dayStamp);
  lcd.setCursor(10, 0);
  lcd.print(timeStamp);
  //lcd.print()
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
  if (checkDevice == 0) {
    if (dataErr == 0) {
      lcd_node1();
      lcd_node2();
    }
    if (dataErr == 1) {
      lcd.setCursor(0, 2);
      lcd.print("DATA ERROR !!");
      lcd.setCursor(0, 3);
      lcd_node2();
    }
    if (dataErr == 2) {
      lcd.setCursor(0, 3);
      lcd.print("DATA ERROR !!");
      lcd_node1();
    }
  }
  if (checkDevice == 1) {
    lcd.setCursor(0, 2);
    lcd.print("NO CONNECT DEVICE 1");
    lcd_node2();
  }
  if (checkDevice == 2) {
    lcd.setCursor(0, 3);
    lcd.print("NO CONNECT DEVICE 2");
    lcd_node1();
  }
  if (checkDevice == 3) {
    lcd.setCursor(0, 2);
    lcd.print("NO CONNECT DEVICE 1");
    lcd.setCursor(0, 3);
    lcd.print("NO CONNECT DEVICE 2");
  }
}