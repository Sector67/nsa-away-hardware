int counter = 0;                  // counter
int sensorValue = 0;
void setup() {
  // initialize control over the keyboard:
  Keyboard.begin();
}

void loop() {
  
  // read the value from the sensor:
  sensorValue = analogRead(A0);
  if (sensorValue>512){
    Keyboard.println("DOWN");
  }
  // read the value from the sensor:
  sensorValue = analogRead(A1);
  if (sensorValue>512){
    Keyboard.println("UP");
  }
  // read the value from the sensor:
  sensorValue = analogRead(A2);
  if (sensorValue>512){
    Keyboard.println("RIGHT");
  }
  // read the value from the sensor:
  sensorValue = analogRead(A3);
  if (sensorValue>512){
    Keyboard.println("LEFT");
  }
  // read the value from the sensor:
  sensorValue = analogRead(A4);
  if (sensorValue>512){
    Keyboard.println("SD_LEFT");
  }
  // read the value from the sensor:
  sensorValue = analogRead(A5);
  if (sensorValue>512){
    Keyboard.println("SD_RIGHT");
  }
}

