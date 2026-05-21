const char* secretKey = "osaid"; // Your 10-byte string

void setup() {
  // Standard baud rate for ESP32
  Serial.begin(115200);
  pinMode(2, OUTPUT); // Built-in LED for visual feedback
}

void loop() {
  // Check if the PC has sent a request byte
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();

    // If PC sends 'R', ESP32 responds with the key
    if (incomingByte == 'R') {
      digitalWrite(2, HIGH); // Turn on LED
      Serial.print(secretKey);
      delay(500);
      digitalWrite(2, LOW);  // Turn off LED
    }
  }
}
