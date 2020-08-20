//Setting up a WiFi connection - COMPLETE

void connectToWifi(const char ssidName[], const char pass[])
{

  WiFi.setPins(8, 7, 4, 2); // Set the WiFi pins on the Feather


  Serial.println("Starting...");
  Serial.println("Connecting to WiFi.");

  int conn = WiFi.begin(ssidName, pass);

  if (conn == WL_CONNECTED)
  {
    Serial.println("OK!");
  }
  else if (conn == WL_IDLE_STATUS)
  {
    Serial.println("Idle");
  }
  else
  {
    Serial.println("WiFi Connection Failed!");
  }

}

void haltFirmware()
{
  Serial.println("\n\nSTOPPING THE PROGRAM!");
  while(true) {}
}
