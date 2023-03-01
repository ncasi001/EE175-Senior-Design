#include <YDLidar.h>

YDLidar lidar;

bool isScanning = false;   

#define YDLIDAR_MOTOR_SCTP 7 // The PWM pin for control the speed of YDLIDAR's motor. 
#define YDLIDAR_MOTRO_EN   3 // The ENABLE PIN for YDLIDAR's motor                  
                    
#define ledPin 4
                        
void setup() {
  lidar.begin(Serial, 128000);
//output mode
  pinMode(YDLIDAR_MOTOR_SCTP, OUTPUT);
  pinMode(YDLIDAR_MOTRO_EN, OUTPUT);


  while(Serial.read() >= 0){};

}

void loop() {
    if(isScanning){
      if (lidar.waitScanDot() == RESULT_OK) {
          float distance = lidar.getCurrentScanPoint().distance; //distance value in mm unit
          float angle    = lidar.getCurrentScanPoint().angle; //anglue value in degree
          byte  quality  = lidar.getCurrentScanPoint().quality; //quality of the current measurement
	  bool  startBit = lidar.getCurrentScanPoint().startBit;
          if(angle > 30 && angle < 100) {
            Serial.print("current angle:");
            Serial.println(angle, DEC); 
          }
          Serial.print("\ncurrent distance:");
          Serial.println(distance, DEC);
          handleData(distance, angle);
//          delay(1000);  // TAKE READ EVERY 1 SECONDS
      }
    }else{
        //stop motor
	    digitalWrite(YDLIDAR_MOTRO_EN, LOW);
	    setMotorSpeed(0);
	    restartScan();
    }


}

void handleData(float distance, float angle) {
  if(angle > 30 && angle < 100 && distance < 609.6) { // 2 feet in test case
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}

void setMotorSpeed(float vol){
  uint8_t PWM = (uint8_t)(51*vol);
  analogWrite(YDLIDAR_MOTOR_SCTP, PWM);
}


void restartScan(){
    device_info deviceinfo;
    if (lidar.getDeviceInfo(deviceinfo, 100) == RESULT_OK) {
         int _samp_rate=4;
         String model;
         float freq = 7.0f;
         switch(deviceinfo.model){
            case 1:
                model="F4";
                _samp_rate=4;
                freq = 7.0;
                break;
            case 4:
                model="S4";
                _samp_rate=4;
                freq = 7.0;
                break;
            case 5:
                model="G4";
                _samp_rate=9;
                freq = 7.0;
                break;
            case 6:
                model="X4";
                _samp_rate=5;   // 5 original do not change
                freq = 7.0;     // 7 original (6-12)
                break;
            default:
                model = "Unknown";
          }

          uint16_t maxv = (uint16_t)(deviceinfo.firmware_version>>8);
          uint16_t midv = (uint16_t)(deviceinfo.firmware_version&0xff)/10;
          uint16_t minv = (uint16_t)(deviceinfo.firmware_version&0xff)%10;
          if(midv==0){
            midv = minv;
            minv = 0;
          }

          
          Serial.print("Firmware version:");
          Serial.print(maxv,DEC);
          Serial.print(".");
          Serial.print(midv,DEC);
          Serial.print(".");
          Serial.println(minv,DEC);

          Serial.print("Hardware version:");
          Serial.println((uint16_t)deviceinfo.hardware_version,DEC);

          Serial.print("Model:");
          Serial.println(model);

          Serial.print("Serial:");
          for (int i=0;i<16;i++){
            Serial.print(deviceinfo.serialnum[i]&0xff, DEC);
          }
          Serial.println("");

          Serial.print("[YDLIDAR INFO] Current Sampling Rate:");
          Serial.print(_samp_rate,DEC);
          Serial.println("K");

          Serial.print("[YDLIDAR INFO] Current Scan Frequency:");
          Serial.print(freq,DEC);
          Serial.println("Hz");
          delay(100);
          device_health healthinfo;
          if (lidar.getHealth(healthinfo, 100) == RESULT_OK){
             // detected...
              Serial.print("[YDLIDAR INFO] YDLIDAR running correctly! The health status:");
              Serial.println( healthinfo.status==0?"well":"bad");
              if(lidar.startScan() == RESULT_OK){
                isScanning = true;
                //start motor in 1.8v
		        setMotorSpeed(1.8);
		digitalWrite(YDLIDAR_MOTRO_EN, HIGH);
                Serial.println("Now YDLIDAR is scanning ......");
              //delay(1000);
              }else{
                  Serial.println("start YDLIDAR is failed!  Continue........");
              }
          }else{
              Serial.println("cannot retrieve YDLIDAR health");
          }
  
      
       }else{
             Serial.println("YDLIDAR get DeviceInfo Error!!!");
       }
}
