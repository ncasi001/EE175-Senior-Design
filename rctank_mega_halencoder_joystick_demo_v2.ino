
// Define the pins for motor control and encoder input
#define left_motor_enable_pin 5
#define left_motor_direction_pin1 6
#define left_motor_direction_pin2 7
#define right_motor_enable_pin 11
#define right_motor_direction_pin1 9
#define right_motor_direction_pin2 10
#define encoder_pinA 2
#define encoder_pinB 4

// Define the pins for joystick control and potentiometer
#define x_pin A0
#define y_pin A1
#define pot_pin A2
#define joy_threshold 100  

// Set the current and previous encoder values
volatile long current_encoder_value = 0;
volatile long previous_encoder_value = 0;

// Set the encoder count and ticks per revolution
const int encoder_count = 2;
const int ticks_per_revolution = 12;

// Initialize the motor and encoder pins
void setup() {
  pinMode(left_motor_enable_pin, OUTPUT);
  pinMode(left_motor_direction_pin1, OUTPUT);
  pinMode(left_motor_direction_pin2, OUTPUT);
  pinMode(right_motor_enable_pin, OUTPUT);
  pinMode(right_motor_direction_pin1, OUTPUT);
  pinMode(right_motor_direction_pin2, OUTPUT);
  pinMode(encoder_pinA, INPUT_PULLUP);
  pinMode(encoder_pinB, INPUT_PULLUP);
  
  // Attach interrupts for encoder input pins
  attachInterrupt(digitalPinToInterrupt(encoder_pinA), handle_encoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoder_pinB), handle_encoder, CHANGE);
  
  // Set the motor directions
  digitalWrite(left_motor_direction_pin1, LOW);
  digitalWrite(left_motor_direction_pin2, LOW);
  digitalWrite(right_motor_direction_pin1, LOW);
  digitalWrite(right_motor_direction_pin2, LOW);
  
  // Initialize Serial Monitor
  Serial.begin(9600);
}

// Handle the encoder interrupts
void handle_encoder() {
  // Read the state of the encoder pins
  int encoder_A = digitalRead(encoder_pinA);
  int encoder_B = digitalRead(encoder_pinB);
  
  // Calculate the new encoder value
  if (encoder_A == encoder_B) {
    current_encoder_value++;
  } else {
    current_encoder_value--;
  }
}

// Main program loop
void loop() {
  //dummy val
  int temp = 0;
  // Read the joystick x and y values
  int x_value = analogRead(x_pin);
  int y_value = analogRead(y_pin);

  Serial.print("X: ");
  Serial.print(x_value);
  Serial.print("\t");
  Serial.print("Y: ");
  Serial.println(y_value);
  
  // Calculate motor directions based on joystick values
  int left_motor_direction = 1;
  int right_motor_direction = 1;
  
  // Read the potentiometer value
  int pot_value = analogRead(pot_pin);
  
  // Calculate motor speeds based on potentiometer value
  int left_motor_speed = map(pot_value, 0, 1023, 0, 255);
  int right_motor_speed = map(pot_value, 0, 1023 , 0, 255);

  // Set the motor speeds and directions
  analogWrite(left_motor_enable_pin, left_motor_speed);
  analogWrite(right_motor_enable_pin, right_motor_speed);

  if (y_value <= 200) {
    temp = 1;
    Serial.print("FORWARD");
  }
  else if (y_value >= 800) {
    temp = 2;
    Serial.print("BACKWARD");
  } 
  else if (x_value >= 800) {
    temp = 3;
    Serial.print("RIGHT");
  } 
  else if (x_value <= 200) {
    temp = 4;
    Serial.print("LEFT");
  } 
  if (x_value > 200 && x_value < 800 && y_value > 200 && y_value < 800) {
    temp = 0;
    Serial.print("NO MOVEMENT");
  }

// TRY
// initializing all pins as low (default case)
// in direction case, set needed ones high

  switch(temp){ // FIX ME
   case 0: //NO MOVEMENT
      digitalWrite(left_motor_direction_pin1, LOW);
      digitalWrite(left_motor_direction_pin2, LOW);
      digitalWrite(right_motor_direction_pin1, LOW);
      digitalWrite(right_motor_direction_pin2, LOW);
      break;
      
    case 1: //FORWARD
      digitalWrite(right_motor_direction_pin1, HIGH);
      digitalWrite(right_motor_direction_pin2, LOW);
      digitalWrite(left_motor_direction_pin1, HIGH);
      digitalWrite(left_motor_direction_pin2, LOW);
      break;
        
    case 2: //BACKWARD
      digitalWrite(left_motor_direction_pin1, LOW);   // LEFT FW
      digitalWrite(left_motor_direction_pin2, HIGH);  // LEFT BACKWARDS
      digitalWrite(right_motor_direction_pin1, LOW); // RIGHT BACKWARDS
      digitalWrite(right_motor_direction_pin2, HIGH);  // RIGHT FW
      break;
      
    case 3: //RIGHT
      digitalWrite(left_motor_direction_pin1, HIGH);
      digitalWrite(left_motor_direction_pin2, LOW);
      digitalWrite(right_motor_direction_pin1, LOW);
      digitalWrite(right_motor_direction_pin2, LOW);
      break;
      
    case 4: //LEFT
      digitalWrite(left_motor_direction_pin1, LOW);
      digitalWrite(left_motor_direction_pin2, LOW);
      digitalWrite(right_motor_direction_pin1, HIGH);
      digitalWrite(right_motor_direction_pin2, LOW);
      break;
      
//    default:
//      digitalWrite(left_motor_direction_pin1, LOW);
//      digitalWrite(left_motor_direction_pin2, LOW);
//      digitalWrite(right_motor_direction_pin1, LOW);
//      digitalWrite(right_motor_direction_pin2, LOW);
//      break;
  }
  
  // Calculate the encoder ticks and motor speed
  long encoder_ticks = current_encoder_value - previous_encoder_value;
  double motor_speed = (double)encoder_ticks / (double)ticks_per_revolution * 1000.0 / 20.0; // RPM
  
  // Print the encoder ticks and motor speed to Serial Monitor
//  Serial.print("Encoder Ticks: ");
//  Serial.print(encoder_ticks);
//  Serial.print(", Motor Speed: ");
//  Serial.print(motor_speed);
//  Serial.println(" RPM");
  
  // Update the previous encoder value
  previous_encoder_value = current_encoder_value;
  
  // Wait for a short time to allow the motor to spin up
  delay(20);
}
