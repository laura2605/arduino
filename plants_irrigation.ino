const int FEW_WATER_LED = 11;
const int MEDIUM_WATER_LED = 12;
const int MUCH_WATER_LED = 13;

const int SWITCH_PUMP_PIN = 10;

const int HUMIDITY_SENSOR_PIN = 9;

const int WATER_STATUS_LED = 8;

const int HUMIDITY_SENSOR_DATA_PIN = A0;

const int WATER_SENSOR_DATA_PIN = A1;

const int FEW_WATER_HUMIDITY = 500;
const int MEDIUM_WATER_HUMIDITY = 450;
const int MUCH_WATER_HUMIDITY = 400;

const int WATER_VALUE = 600;

const int INTERVALL_TIME = 30000; //300000;

unsigned long lastMillis;

int humiditySetpoint = MEDIUM_WATER_HUMIDITY;

char bluetoothValue; 
 
void setup(){
  
  pinMode(FEW_WATER_LED, OUTPUT);
  pinMode(MEDIUM_WATER_LED, OUTPUT);
  pinMode(MUCH_WATER_LED, OUTPUT);

  changeWaterRequirementsStatusLeds(HIGH, HIGH, LOW);

  pinMode(WATER_STATUS_LED, OUTPUT);

  pinMode(HUMIDITY_SENSOR_PIN, OUTPUT);

  pinMode(SWITCH_PUMP_PIN, OUTPUT); 
  stopWaterPump();

  lastMillis = millis();
 
  Serial.begin(9600);
}
 
void loop(){
  
  if(Serial.available()) {
    bluetoothValue = Serial.read();
  }

  // Changes the water requirements status according the value received from bluetooth connection.
  switch(bluetoothValue) {
    case '1':
      changeWaterRequirementsStatusLeds(HIGH, LOW, LOW);
      humiditySetpoint = FEW_WATER_HUMIDITY;
      break;
    case '2':
      changeWaterRequirementsStatusLeds(HIGH, HIGH, LOW);
      humiditySetpoint = MEDIUM_WATER_HUMIDITY;
      break;
    case '3':
      changeWaterRequirementsStatusLeds(HIGH, HIGH, HIGH);
      humiditySetpoint = MUCH_WATER_HUMIDITY;
      break;
  }

  checkWaterStatus();

  if(isTimeToCheckHumidity()) {
    checkHumidity();
  }
}

/*
 * Changes the status of the three water requirements LEDs.
 */
void changeWaterRequirementsStatusLeds(int fewWaterLed, int mediumWaterLed, int muchWaterLed) {
  
  digitalWrite(FEW_WATER_LED, fewWaterLed);
  digitalWrite(MEDIUM_WATER_LED, mediumWaterLed);
  digitalWrite(MUCH_WATER_LED, muchWaterLed);
}

/*
 * Let the water status LED glow, if the water status is too high. 
 */
void checkWaterStatus() {

  if(isWaterLevelToLow()) {
    digitalWrite(WATER_STATUS_LED, HIGH);
  }
  else {
    digitalWrite(WATER_STATUS_LED, LOW);
  }
}

/*
 * Reads the water status from the water sensor.
 */
boolean isWaterLevelToLow() {

  return analogRead(WATER_SENSOR_DATA_PIN) > WATER_VALUE;

}

/*
 * true, if the intervall time is over
 */
boolean isTimeToCheckHumidity() {

  if((millis() - lastMillis) > INTERVALL_TIME) {
    lastMillis = millis();
    return true;  
  }
  return false;
}

/*
 * Pumps water, if the humidity value is greater than the setpoint and the water level is not to low.
 */
void checkHumidity() {

  if(readHumidity() > humiditySetpoint && !isWaterLevelToLow()) {
    startWaterPump();

    int humidity = readHumidity();
    
    while(humidity > humiditySetpoint && !isWaterLevelToLow()) {
      humidity = readHumidity();
    }
    stopWaterPump();
  }
}

/*
 * Reads the humidity from the humidity sensor.
 */
int readHumidity() {

  digitalWrite(HUMIDITY_SENSOR_PIN, HIGH);
  delay(1000);
  int humidity = analogRead(HUMIDITY_SENSOR_DATA_PIN);
  digitalWrite(HUMIDITY_SENSOR_PIN, LOW);

  return humidity;
}

/*
 * Starts the water pump.
 */
void startWaterPump() {
  
  digitalWrite(SWITCH_PUMP_PIN, LOW);
}

/*
 * Stops the water pump.
 */
void stopWaterPump() {
  
  digitalWrite(SWITCH_PUMP_PIN, HIGH);
}

