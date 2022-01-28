#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "config.h"


RF24 radio(9, 10);
const uint64_t pipe = 0xE8E8F0F0E1LL;
dataTx rData; 
driveData dData,iData;

void initiallizeRadio() {
    while(!radio.begin()) {
      Serial.print("Radio did not start: \n");
      delay(100);
    }
    Serial.print("Radio init Sucessfull \n");
    delay(100);
    radio.setChannel(100);
    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_2MBPS);
    radio.setAutoAck(true);
    radio.enableAckPayload();
    radio.enableDynamicPayloads();
    radio.openReadingPipe(1,pipe);
    radio.startListening();
    radio.setRetries(15,15);
}

void printData() {
    Serial.print(rData.imuTx.x,4);
    Serial.print(",");
    Serial.print(rData.imuTx.y,4);
    Serial.print(",");
    Serial.print(rData.imuTx.z,4);
    Serial.print(",");
    Serial.print(rData.altTx,2);
    if (rData.valid) {
    Serial.print(",");
    Serial.print(rData.lat,6);
    Serial.print(",");
    Serial.print(rData.lon,6);
    Serial.print(",");
    Serial.print(rData.velocity,2);
    Serial.print(",");
    Serial.print(rData.sat);
    }
    Serial.println("");
}

void printTransmitData(){
  Serial.print(dData.x);
  Serial.print(",");
  Serial.print(dData.y);
  Serial.print(",");
  Serial.print(dData.z);
  Serial.print(",");
  Serial.print(dData.yr);
  Serial.print(",");
  Serial.print(dData.auxA);
  Serial.print(",");
  Serial.print(dData.auxB);
  Serial.print(",");
  Serial.print(dData.auxC);
  Serial.print(",");
  Serial.print(dData.auxD);
  Serial.print(",");
  Serial.print(dData.auxE);
  Serial.print(",");
  Serial.print(dData.auxF);
  Serial.println("");

}

void recieveData(){
    if (radio.available()) { 
    radio.writeAckPayload(1,&dData,sizeof(driveData));   
    radio.read(&rData, sizeof(dataTx));
  }
}

void readJoyStick(driveData *data){
  data->x = analogRead(JOYSTICK_1_X);
  data->y = analogRead(JOYSTICK_1_Y);
  data->auxA = !digitalRead(A);
  data->auxB = !digitalRead(B);
  data->auxC = !digitalRead(C);
  data->auxD = !digitalRead(D);
  data->auxE = !digitalRead(E);
  data->auxF = !digitalRead(F);

}

void tuning(driveData *data, int step) {
  data->x = data->x+step;
}

void mapServo(driveData *data) {
  data->x = map(data->x,0,660,SERVO_MIN,SERVO_MAX);
  data->y = map(data->y,0,660,SERVO_MIN,SERVO_MAX);
}

void DeadBand(driveData *data) {
  if (abs(data->x - iData.x) <= JOYSTICK_DEADBAND) {
    data->x = 0;
  } 
  if (abs(data->y - iData.y) <= JOYSTICK_DEADBAND) {
    data->y = 0;
  } 
}

void controlData() {
   readJoyStick(&dData);
   mapServo(&dData);
   DeadBand(&dData); 
}

void initiallizeInputs() {
    pinMode(JOYSTICK_1_X,INPUT);
    pinMode(JOYSTICK_1_Y,INPUT);
    pinMode(A,INPUT_PULLUP);
    pinMode(B,INPUT_PULLUP);
    pinMode(C,INPUT_PULLUP);
    pinMode(D,INPUT_PULLUP);
    pinMode(E,INPUT_PULLUP);
    pinMode(F,INPUT_PULLUP);
}
void setup() {
    Serial.begin(115200);
    initiallizeRadio();
    initiallizeInputs();

    readJoyStick(&iData);
    mapServo(&iData);
}

void loop() {
    controlData();
    recieveData();
    printTransmitData();
}