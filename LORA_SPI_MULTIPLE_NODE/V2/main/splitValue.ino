String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) { //data.charAt(i) xuất ra kí tự ở vị trí (i)
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;     
      // (i == maxIndex) true -> i + 1
      // (i == maxIndex) false -> i
    }
  }
  return found > index ? data.substring( strIndex[0], strIndex[1] ) : "";
}
