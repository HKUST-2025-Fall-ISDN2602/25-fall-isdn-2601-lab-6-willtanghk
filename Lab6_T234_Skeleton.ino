/*Change all the ? in the code, add code in ???, modify getPosition() */

//L298N Driver Pin 

#define MOTOR_ENA 14  // Replace the ? with the GPIO pin you selected to connect ENA
#define MOTOR_IN1 13  // Replace the ? with the GPIO pin you selected to connect IN2
#define MOTOR_IN2 12  // Replace the ? with the GPIO pin you selected to connect IN2

//Encoder Pin 
#define ENCODER_PINA 33 // Replace the ? with the GPIO pin you selected to connect encoder A
#define ENCODER_PINB 32 // Replace the ? with the GPIO pin you selected to connect encoder B

//Encoder Counter
volatile long encoderCount=0; 
volatile double position=0; 
String command;

//PID constants
//** Modify these value for Task 2-4
double kp = 0.45;
double ki = 0.00002;
double kd = 0.36;

//PID Varibles 
unsigned long currentTime, previousTime;
double elapsedTime;
double error;
double lastError;
double output;
double cumError, rateError;

// set desired position to 90 degrees
double setPoint=90.0;

int Task = 4;  //Change this according to the task that you are doing 

void TaskConfig(){
  if(Task == 2){
    ki = 0;
    kd = 0;
  }
  else if(Task == 3)
    kd =0;
} 

//PID Controller 
double computePID(double inp){     
  currentTime = millis();                              //get current time
  elapsedTime = (double)(currentTime - previousTime);  //compute time elapsed from previous computation
  
  error = setPoint - inp;                              // determine error
  cumError += error * elapsedTime;                     // compute integral
  rateError = (error - lastError)/elapsedTime;         // compute derivative

  double out = kp*error + ki*cumError + kd*rateError;  //PID output               

  lastError = error;                                   //remember current error
  previousTime = currentTime;                          //remember current time

  return out;                                          //have function return the PID output
}


void IRAM_ATTR encoderInterrupt() {
  if (digitalRead(ENCODER_PINA) != digitalRead(ENCODER_PINB)) {
    encoderCount++;
  } else {
    encoderCount--;
  }
}


//Serial Display Function 

void serialGraph(){
  Serial.print("Error:");
  Serial.print(error);
  Serial.print(",");
  Serial.print("Position:");
  Serial.print(getPosition());
  Serial.print(",");
  Serial.print("PID_output:");
  Serial.print(output);
  Serial.print(","); 
  Serial.print("setPoint:"); 
  Serial.print(setPoint);
  Serial.println("\t"); 
}
// To get the current position

// **Some value needs to be changed in order to use this
double getPosition() {
  // Calculate the current position based on encoder count
  position = float(encoderCount)*360.0/170; // Replace 1000.0 with the actual counts per revolution

  if (position<0)
  {position = position + 360; // Ensure position is positive
  }
  
  return position;
}

void setup() {
  
/*setup baud and pin mode */
  Serial.begin(115200); //Define baud
  Serial.println("ISDN2601 Lab 6: Task 2");

// encoder A pin mode for interrupt
  pinMode(ENCODER_PINA, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PINA), encoderInterrupt, CHANGE);

/* pin mode for pins connected with L298N driver  */
  pinMode(MOTOR_IN1, OUTPUT);  // Set IN1 as an output pin
  pinMode(MOTOR_IN2, OUTPUT);  // Set IN2 as an output pin
  //pinMode(ENCODER_PINA, INPUT);  // Set A as an input pin
  pinMode(ENCODER_PINB, INPUT);  // Set B as an input pin

  //Configure PID value for different Task 
  TaskConfig(); 
}

int count = 1;

void loop() {

/*
  if((count % 50) == 0){
    kd = kd + 0.01;
    position = 0;
    encoderCount = 0;
    Serial.println("###### STOPPED ######");
    Serial.print("Count: ");
    Serial.print(count);
    Serial.print(",kd: ");
    Serial.print(kd,5);
    Serial.println(","); 
  }
  count++;
  */
  
        error = setPoint - getPosition();         // calculate current error
        output = computePID(getPosition());       // calculate PID output 
        analogWrite(MOTOR_ENA,(67+int(abs(output))));   // Replace 128 with the the threshold value for your motor to move
        //analogWrite(MOTOR_ENA,112);   // Replace 128 with the the threshold value for your motor to move

        // control the motor based on PID output
        if(output<0){
           /*movement direction set when output<0 */
              digitalWrite(MOTOR_IN1, HIGH);
              digitalWrite(MOTOR_IN2, LOW);
          }
          else{
            /*movement direction set when output>=0*/
              digitalWrite(MOTOR_IN1, LOW);
              digitalWrite(MOTOR_IN2, HIGH);
          }
     
  //display position, setPoint and outout of controller in Serial Plotter
  serialGraph();

   // Reset encoder count
  if (position > 360 || position < 0) {
    encoderCount = 0;
  } 
delay(50);

}
