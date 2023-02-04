#include <arduino-timer.h>

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

uint8_t WATER_OUTLET_PINS[4];
int WATER_OUTLET_STATES[4];
String WATER_OUTLET_IDS[4];

String command;

auto timer = timer_create_default();

void setup() {
  analogReference(EXTERNAL);

  WATER_OUTLET_PINS[0] = 5;
  WATER_OUTLET_PINS[1] = 6;
  WATER_OUTLET_PINS[2] = 7;
  WATER_OUTLET_PINS[3] = 8;

  WATER_OUTLET_IDS[0] = "1";
  WATER_OUTLET_IDS[1] = "2";
  WATER_OUTLET_IDS[2] = "3";
  WATER_OUTLET_IDS[3] = "4";
  
  for (int i = 0; i < sizeof(WATER_OUTLET_PINS); i = i + 1) {
    pinMode(WATER_OUTLET_PINS[i], OUTPUT);
    digitalWrite(WATER_OUTLET_PINS[i], HIGH);
  }
  
  Serial.begin(9600);

  timer.every(10000, sendHeartbeat);

  sendHeartbeat();
}

void sendSensorReading(String id, uint8_t pin) {
  int readingRaw = analogRead(pin);
  Serial.print("MS:" + id + ":");
  Serial.print(readingRaw);
  Serial.print(":");
  Serial.println(translateToMoistureReadingPercentage(readingRaw));
}

void sendWaterOutletState(int outletIndex) {
  String state;
  
  if (digitalRead(WATER_OUTLET_PINS[outletIndex])) {
    state = "1";    
  } else {
    state = "0";
  }

  Serial.print("WO:" + WATER_OUTLET_IDS[outletIndex] + ":" + state);
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

bool sendHeartbeat() {
  Serial.println("HEARTBEAT");
  return true;
}

void loop() {  
  timer.tick();

  for (int i = 0; i < sizeof(WATER_OUTLET_PINS); i++) {
    digitalWrite(
      WATER_OUTLET_PINS[i], 
      WATER_OUTLET_STATES[i]
    );
  }

  while (Serial.available()) {
    int rlen = Serial.readBytes(buf, BUFFER_SIZE);

    for(int i = 0; i < rlen; i++) {
      command = buf[i];

      if (command.equals("a")) { // Water off
        WATER_OUTLET_STATES[0] = HIGH;
        WATER_OUTLET_STATES[1] = HIGH;
        WATER_OUTLET_STATES[2] = HIGH;
        WATER_OUTLET_STATES[3] = HIGH;
      } else if (command.equals("b")) { // Water on
        WATER_OUTLET_STATES[0] = LOW;
        WATER_OUTLET_STATES[1] = LOW;
        WATER_OUTLET_STATES[2] = LOW;
        WATER_OUTLET_STATES[3] = LOW;
      } else if (command.equals("c")) {
        WATER_OUTLET_STATES[0] = LOW;
      } else if (command.equals("d")) {
        WATER_OUTLET_STATES[0] = HIGH;
      } else if (command.equals("e")) {
        WATER_OUTLET_STATES[1] = LOW;
      } else if (command.equals("f")) {
        WATER_OUTLET_STATES[1] = HIGH;
      } else if (command.equals("g")) {
        WATER_OUTLET_STATES[2] = LOW;
      } else if (command.equals("h")) {
        WATER_OUTLET_STATES[2] = HIGH;
      } else if (command.equals("i")) {
        WATER_OUTLET_STATES[3] = LOW;
      } else if (command.equals("j")) {
        WATER_OUTLET_STATES[3] = HIGH;
      } else if (command.equals("k")) {
        Serial.println("READINGS_START");
        sendSensorReading("1", MOISTURE_SENSOR_1_PIN);
        sendSensorReading("2", MOISTURE_SENSOR_2_PIN);
        sendSensorReading("3", MOISTURE_SENSOR_3_PIN);
        sendSensorReading("4", MOISTURE_SENSOR_4_PIN);
        sendSensorReading("5", MOISTURE_SENSOR_5_PIN);
        sendSensorReading("6", MOISTURE_SENSOR_6_PIN);
        Serial.println("READINGS_END");
      } else if (command.equals("l")) { // Send water outlet states
        Serial.println("READINGS_START");
        sendWaterOutletState(0);
        sendWaterOutletState(1);
        sendWaterOutletState(2);
        sendWaterOutletState(3);
        Serial.println("READINGS_END");
      }
    }
  }

  delay(50);
}
