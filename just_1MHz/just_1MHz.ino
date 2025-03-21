#define SQUARE_WAVE_PIN     14  // output for 1 MHz square wave
// #define test_pin 13
// bool pinState = LOW;

void setup() {
  Serial.begin(115200);

  pinMode(SQUARE_WAVE_PIN, OUTPUT);
  // pinMode(test_pin, OUTPUT);

  ledcDetach(SQUARE_WAVE_PIN);
  ledcAttach(SQUARE_WAVE_PIN, 1000000, 1);
  ledcWrite(SQUARE_WAVE_PIN, 1);
}

void loop() {
  // pinState = !pinState;
  // digitalWrite(test_pin, pinState);
  // Serial.println("Hi");
  delay(1000);
}