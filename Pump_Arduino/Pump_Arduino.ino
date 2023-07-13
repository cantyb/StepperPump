int sensor1 = 2;
int sensor2 = 3;
int stepperPin = 4;
int directionPin = 5; //High toward Sensor 1, Low toward sensor 2 (subject to change)
int stepsPerRep = 0;
int steps = 0;
int n = 0;
int safeSteps = 20;
int startupSpeed = 9803; // sets the speed to min for start up
int stepsFromEnd;
bool dir = true; //true is HIGH
int i;
int slowSteps = 30;
int speedAtTurn = 10000;


unsigned long pps; //pulses per second
unsigned long microsDelay;
unsigned long lastPulseTime = 0;
unsigned long currentTime;
unsigned long minStepsSec = 100;  // 0.25 rev/sec
unsigned long maxStepsSec = 400;  // 1 rev/sec
unsigned long stepsPerRev = 400;
unsigned long speedVal = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(stepperPin, OUTPUT);
  pinMode(directionPin, OUTPUT);

  Serial.println("Set up done!");
  startup();
}

void takeStep() {  //takes a step
  digitalWrite(stepperPin, HIGH); // Set the pin to HIGH
  delayMicroseconds(5);
  digitalWrite(stepperPin, LOW); // Set the pin to LOW
}

void changeDirection() {

  if (dir == false) {
    dir = true;
    digitalWrite(directionPin, HIGH);
    Serial.println("Direction changed to high");
    return;
  }
  if (dir == true) {
    digitalWrite(directionPin, LOW);
    dir = false;
    Serial.println("Direction changed to low");
  }
}

void startup() {
  Serial.println("Doing startup!");
  steps = 0;

  digitalWrite(directionPin, HIGH);  //go toward sensor 1
  while (digitalRead(sensor1) == HIGH) { 
    currentTime = micros();
      if (currentTime - lastPulseTime > startupSpeed) {
        takeStep();
        lastPulseTime = currentTime;
      }
  }
  Serial.println("part one done!");
  digitalWrite(directionPin, LOW);  //go toward sensor two and count how far it is
  while (digitalRead(sensor2) == HIGH) {
    currentTime = micros();
    if (currentTime - lastPulseTime > startupSpeed) {
      takeStep();
      steps++;
      lastPulseTime = currentTime;
    }
  }
  stepsPerRep = steps - 2*safeSteps;
  Serial.println(stepsPerRep );
  Serial.println("steps per rep");

  digitalWrite(directionPin, HIGH);
  i = 0;
  while(i < safeSteps) {
    currentTime = micros();
    if (currentTime - lastPulseTime > startupSpeed) {
      takeStep();
      i++;
      lastPulseTime = currentTime;
    }
  }
  Serial.print(i);
  Serial.println(" Safe steps taken");
  Serial.println("Startup done!");
}

void loop() {
  // put your main code here, to run repeatedly:
  //delay(10);
  currentTime = micros();
  speedVal = analogRead(A0);
  pps = ((speedVal-(unsigned long)23)*(maxStepsSec - minStepsSec))/1000 + minStepsSec; 
  microsDelay =  1000000 / pps;

  if (speedVal > 24){
      
    if (digitalRead(sensor1) == LOW  || digitalRead(sensor2) == LOW) {
    delay(500);
    n = 0;
    dir = true;
    Serial.println("Whoa there! Sensor activated, recalibrating");
    startup();
  }
  
  if(n > stepsPerRep - slowSteps){
    microsDelay = microsDelay + ((n-(stepsPerRep-slowSteps)) * (unsigned long)((speedAtTurn-microsDelay)/((float)slowSteps)));
  }

  if(n <= slowSteps){
    microsDelay = microsDelay + (slowSteps - n) * (unsigned long)((speedAtTurn-microsDelay)/((float)slowSteps));
  }
  // int j;
  // if (j = 100) {
  //   Serial.println(microsDelay);
  // }
  // j++;
  if (n < stepsPerRep) {
    if (currentTime - lastPulseTime > microsDelay) {
    takeStep();
    // if (n % 10 == 0) 
    //   Serial.println(microsDelay);
    n++;
    lastPulseTime = currentTime;
    }
  }
  else {
    changeDirection();
    //Serial.println(n);
    //Serial.printl("steps taken");
    n = 0;
  }

  }
  else {
    Serial.println("Speed is too low");
    delay(500);
  }
}