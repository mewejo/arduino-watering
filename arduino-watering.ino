//https://github.com/contrem/arduino-timer
#include <arduino-timer.h>

const int BUFFER_SIZE = 1;
char buf[BUFFER_SIZE];

uint8_t MOISTURE_SENSOR_PINS[6];
String MOISTURE_SENSOR_IDS[6];
int MOISTURE_SENSOR_DRY[6];
int MOISTURE_SENSOR_WET[6];

uint8_t WATER_OUTLET_PINS[4];
int WATER_OUTLET_STATES[4];
String WATER_OUTLET_IDS[4];

String command;

auto timer = timer_create_default();

void setup() {
  analogReference(EXTERNAL);

  // Water outlets pins
  WATER_OUTLET_PINS[0] = 5;
  WATER_OUTLET_PINS[1] = 6;
  WATER_OUTLET_PINS[2] = 7;
  WATER_OUTLET_PINS[3] = 8;

  // Water outlet IDs
  WATER_OUTLET_IDS[0] = "1";
  WATER_OUTLET_IDS[1] = "2";
  WATER_OUTLET_IDS[2] = "3";
  WATER_OUTLET_IDS[3] = "4";
  
  // Moisture sensor pins
  MOISTURE_SENSOR_PINS[0] = A0;
  MOISTURE_SENSOR_PINS[1] = A1;
  MOISTURE_SENSOR_PINS[2] = A2;
  MOISTURE_SENSOR_PINS[3] = A3;
  MOISTURE_SENSOR_PINS[4] = A4;
  MOISTURE_SENSOR_PINS[5] = A5;

  // Moisture sensor IDs
  MOISTURE_SENSOR_IDS[0] = "1";
  MOISTURE_SENSOR_IDS[1] = "2";
  MOISTURE_SENSOR_IDS[2] = "3";
  MOISTURE_SENSOR_IDS[3] = "4";
  MOISTURE_SENSOR_IDS[4] = "5";
  MOISTURE_SENSOR_IDS[5] = "6";

  // Moisture sensor dry thresholds
  MOISTURE_SENSOR_DRY[0] = 500;
  MOISTURE_SENSOR_DRY[1] = 500;
  MOISTURE_SENSOR_DRY[2] = 500;
  MOISTURE_SENSOR_DRY[3] = 500;
  MOISTURE_SENSOR_DRY[4] = 500;
  MOISTURE_SENSOR_DRY[5] = 500;

  // Moisture sensor wet thresholds
  MOISTURE_SENSOR_WET[0] = 240;
  MOISTURE_SENSOR_WET[1] = 240;
  MOISTURE_SENSOR_WET[2] = 240;
  MOISTURE_SENSOR_WET[3] = 240;
  MOISTURE_SENSOR_WET[4] = 240;
  MOISTURE_SENSOR_WET[5] = 240;

  // Set water outlet pins up
  for (int i = 0; i < sizeof(WATER_OUTLET_PINS); i = i + 1) {
    pinMode(WATER_OUTLET_PINS[i], OUTPUT);
    digitalWrite(WATER_OUTLET_PINS[i], HIGH);
  }
  
  // Comms
  Serial.begin(9600);

  timer.every(10000, sendHeartbeat);

  sendHeartbeat();
}

void sendSensorReading(int sensorIndex) {
  int readingRaw = analogRead(
    MOISTURE_SENSOR_PINS[sensorIndex]
  );

  Serial.print("MS:" + MOISTURE_SENSOR_IDS[sensorIndex] + ":");
  Serial.print(readingRaw);
  Serial.print(":");
  Serial.println(translateToMoistureReadingPercentage(sensorIndex, readingRaw));
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

void setWaterOutletState(int outletIndex, bool state) {
  if (state) {
    WATER_OUTLET_STATES[outletIndex] = LOW;
  } else {
    WATER_OUTLET_STATES[outletIndex] = HIGH;
  }
}

int translateToMoistureReadingPercentage(int sensorIndex, int readingRaw) {
  
  int percentage = map(
    readingRaw,
    MOISTURE_SENSOR_DRY[sensorIndex],
    MOISTURE_SENSOR_WET[sensorIndex],
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

  while (Serial.available()) {
    int rlen = Serial.readBytes(buf, BUFFER_SIZE);

    for(int i = 0; i < rlen; i++) {
      command = buf[i];

      if (command.equals("a")) { // Water off
        for (int i = 0; i < sizeof(WATER_OUTLET_PINS); i++) {
          setWaterOutletState(i, false);
        }
      } else if (command.equals("b")) { // Water on
        for (int i = 0; i < sizeof(WATER_OUTLET_PINS); i++) {
          setWaterOutletState(i, true);
        }
      } else if (command.equals("c")) {
        setWaterOutletState(0, true);
      } else if (command.equals("d")) {
        setWaterOutletState(0, false);
      } else if (command.equals("e")) {
        WATER_OUTLET_STATES[1] = LOW;
        setWaterOutletState(1, true);
      } else if (command.equals("f")) {
        setWaterOutletState(1, false);
      } else if (command.equals("g")) {
        setWaterOutletState(2, true);
      } else if (command.equals("h")) {
        setWaterOutletState(2, false);
      } else if (command.equals("i")) {
        setWaterOutletState(3, true);
      } else if (command.equals("j")) {
        setWaterOutletState(3, false);
      } else if (command.equals("k")) {
        Serial.println("READINGS_START");

        for (int i = 0; i < sizeof(MOISTURE_SENSOR_PINS); i++) {
          sendSensorReading(i);
        }

        Serial.println("READINGS_END");
      } else if (command.equals("l")) { // Send water outlet states
        Serial.println("OUTLETS_START");

        for (int i = 0; i < sizeof(WATER_OUTLET_PINS); i++) {
          sendWaterOutletState(i);
        }

        Serial.println("OUTLETS_END");
      }
    }
  }

  for (int i = 0; i < sizeof(WATER_OUTLET_PINS); i++) {
    digitalWrite(
      WATER_OUTLET_PINS[i], 
      WATER_OUTLET_STATES[i]
    );
  }

  delay(50);
}
