int WATER_1_PIN = 5;
int WATER_2_PIN = 6;
int WATER_3_PIN = 7;
int WATER_4_PIN = 8;

const int BUFFER_SIZE = 1;
char buf[BUFFER_SIZE];

#define MOISTURE_SENSOR_1_PIN A0
#define MOISTURE_SENSOR_2_PIN A1
#define MOISTURE_SENSOR_3_PIN A2
#define MOISTURE_SENSOR_4_PIN A3
#define MOISTURE_SENSOR_5_PIN A4
#define MOISTURE_SENSOR_6_PIN A5

#define MOISTURE_DRY 500
#define MOISTURE_WET 240

String command;

void sendSensorReading(String id, uint8_t pin) {
  int readingRaw = analogRead(pin);
  Serial.print("MS:" + id + ":");
  Serial.print(readingRaw);
  Serial.print(":");
  Serial.println(translateToMoistureReadingPercentage(readingRaw));
}

int translateToMoistureReadingPercentage(int readingRaw) {
  
  int percentage = map(
    readingRaw,
    MOISTURE_DRY,
    MOISTURE_WET,
    0,
    100
  );

  if (percentage > 100) {
    percentage = 100;
  }

  if (percentage < 0) {
    percentage = 0;
  }

  return percentage;
}

void setup() {
  analogReference(EXTERNAL);
  
  pinMode(WATER_1_PIN, OUTPUT);
  pinMode(WATER_2_PIN, OUTPUT);
  pinMode(WATER_3_PIN, OUTPUT);
  pinMode(WATER_4_PIN, OUTPUT);

  digitalWrite(WATER_1_PIN, HIGH);
  digitalWrite(WATER_2_PIN, HIGH);
  digitalWrite(WATER_3_PIN, HIGH);
  digitalWrite(WATER_4_PIN, HIGH);
  
  Serial.begin(9600);
  
  delay(1000);

  Serial.println("READY");
}

void loop() {  
  while (Serial.available()) {
    int rlen = Serial.readBytes(buf, BUFFER_SIZE);

    for(int i = 0; i < rlen; i++) {
      command = buf[i];

      if (command.equals("a")) { // Water off
        digitalWrite(WATER_1_PIN, HIGH);
        digitalWrite(WATER_2_PIN, HIGH);
        digitalWrite(WATER_3_PIN, HIGH);
        digitalWrite(WATER_4_PIN, HIGH);
      } else if (command.equals("b")) { // Water on
        digitalWrite(WATER_1_PIN, LOW);
        digitalWrite(WATER_2_PIN, LOW);
        digitalWrite(WATER_3_PIN, LOW);
        digitalWrite(WATER_4_PIN, LOW);
      } else if (command.equals("c")) {
        digitalWrite(WATER_1_PIN, LOW);
      } else if (command.equals("d")) {
        digitalWrite(WATER_1_PIN, HIGH);
      } else if (command.equals("e")) {
        digitalWrite(WATER_2_PIN, LOW);
      } else if (command.equals("f")) {
        digitalWrite(WATER_2_PIN, HIGH);
      } else if (command.equals("g")) {
        digitalWrite(WATER_3_PIN, LOW);
      } else if (command.equals("h")) {
        digitalWrite(WATER_3_PIN, HIGH);
      } else if (command.equals("i")) {
        digitalWrite(WATER_4_PIN, LOW);
      } else if (command.equals("j")) {
        digitalWrite(WATER_4_PIN, HIGH);
      } else if (command.equals("k")) {
        Serial.println("READINGS_START");
        sendSensorReading("1", MOISTURE_SENSOR_1_PIN);
        sendSensorReading("2", MOISTURE_SENSOR_2_PIN);
        sendSensorReading("3", MOISTURE_SENSOR_3_PIN);
        sendSensorReading("4", MOISTURE_SENSOR_4_PIN);
        sendSensorReading("5", MOISTURE_SENSOR_5_PIN);
        sendSensorReading("6", MOISTURE_SENSOR_6_PIN);
        Serial.println("READINGS_END");
      }
    }
  }

  delay(50);
}
