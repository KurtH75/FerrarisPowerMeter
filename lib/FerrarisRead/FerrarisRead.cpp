#include <FerrarisRead.h>

#define CNY70_2 2
#define CNY70_1 0

#define DEBUG false

#define sensePeak 35

int sensor1, sensor2;
float avg_sensor1, avg_sensor2;
float weight1 = 0.005;
float weight2 = 0.005;

static enum { IDLE, S1HF, S2HS, MARK, S2HF, S1HS, S1LF, S2LF, S2LS, S1LS , ERR } state;
static enum { sL, sLH, sH, sHL } s1state, s2state;
int armedUp, armedDown =0;


void ferrarisReadInit() {
  state = IDLE;
  s1state = sL;
  s2state = sL;
  pinMode(CNY70_1, OUTPUT);
  pinMode(CNY70_2, OUTPUT);
  digitalWrite(CNY70_2, LOW);
  digitalWrite(CNY70_1, HIGH);
  delay(5);
  analogRead(A0);
  avg_sensor1 =  analogRead(A0);
  digitalWrite(CNY70_1, LOW);
  digitalWrite(CNY70_2, HIGH);
  delay(5);
  analogRead(A0);
  avg_sensor2 = analogRead(A0);
  digitalWrite(CNY70_2, LOW);
}

// the loop routine runs over and over again forever:
int ferrarisLoop() {
  // read the input on analog pin 0:
  digitalWrite(CNY70_1, HIGH);
  delay(5);
  analogRead(A0);
  sensor1 =  analogRead(A0);
  digitalWrite(CNY70_1, LOW);
  //Serial.print(sensor1);
  //Serial.print(" ");
  avg_sensor1 = weight1*sensor1 + (1-weight1) * avg_sensor1;
  //Serial.print(avg_sensor1);
  //Serial.print(" ");
  
  if ((avg_sensor1-sensor1)>=sensePeak) {
    switch (s1state)
    {
    case sL: 
      s1state = sLH;
      break;

    case sLH:
      s1state = sH;
      break;
    }
    //digitalWrite(LED_BUILTIN, LOW);
  } else {
    switch (s1state)
    {
    case sH:
      s1state = sHL;
      break;
    
    case sHL:
      s1state = sL;
      break;

    default:
      s1state = sL;
      break;
    }
    //digitalWrite(LED_BUILTIN, HIGH);
  }
  
  digitalWrite(CNY70_2, HIGH);
  delay(5);
  analogRead(A0);
  sensor2 = analogRead(A0);
  digitalWrite(CNY70_2, LOW);
  
  //Serial.print(" ");
  avg_sensor2 = weight2*sensor2 + (1-weight2) * avg_sensor2;
  //Serial.print(avg_sensor2);
  //Serial.print(" ");
  //Serial.print("");
  //Serial.print(" ");
  //Serial.println("");
  if ((avg_sensor2-sensor2)>=sensePeak) {
    switch (s2state)
    {
    case sL: 
      s2state = sLH;
      break;

    case sLH:
      s2state = sH;
      break;
    }
    //digitalWrite(LED_BUILTIN, LOW);
  } else {
    switch (s2state)
    {
    case sH:
      s2state = sHL;
      break;
    
    case sHL:
      s2state = sL;
      break;

    default:
      s2state = sL;
      break;
    }
    //digitalWrite(LED_BUILTIN, HIGH);
  }
  if (DEBUG) {
  Serial.print(" S1: ");
  Serial.print(sensor1);
  Serial.print(" S1A: ");
  Serial.print(avg_sensor1 );
  Serial.print(" S2: ");
  Serial.print(sensor2);
  Serial.print(" S2A: ");
  Serial.println(avg_sensor2);
  }

  delay(50);
  
  return statemach();
  
  
}

int statemach(){
  int pulsecount = 0;


  switch (state)
  {
    case IDLE: 
      //Serial.println("S: Idle");
      armedUp = 1;
      armedDown = 1;
      if (s1state==sLH)state = S1HF;
      if (s2state==sLH)state = S2HF;
      break;

    case S1HF:
      //Serial.println("S: S1HF");
      if (s2state==sLH)state = S2HS;
      break;

    case S2HS:
      //Serial.println("S: S2HS");
      if (armedUp) pulsecount=1; //a positive pulse is detected
      state = MARK;
      break;

    case MARK:
      //Serial.println("S: MARK");
      if (s2state==sHL)state = S2LF;
      if (s1state==sHL)state = S1LF;
      break;

    case S2LF:
      //Serial.println("S: S2LF");
      if ((s2state==sH)||(s2state==sLH))state = MARK;
      if (s1state==sHL)state = S1LS;
      break;

    case S1LS:
      //Serial.println("S: S1LS");
      if (armedDown) pulsecount= -1; //a negative pulse is detected
      state = IDLE;
      break;
      
    case S2HF:
      //Serial.println("S: S2HF");
      if (s1state==sLH)state = S1HS;
      break;

    case S1HS:
      //Serial.println("S: S1HS");
      if (armedDown) pulsecount= -1; //a negative pulse is detected
      state = MARK;
      break;

    case S1LF:
      //Serial.println("S: S1LF");
      if ((s1state==sH)||(s1state==sLH))state = MARK;
      if (s2state==sHL)state = S2LS;
      break;

    case S2LS:
      //Serial.println("S: S2LS");
      if (armedUp) pulsecount= 1; //a positive pulse is detected
      state = IDLE;
      break;
         
    default:
      state = IDLE;
      break;
  }
  if (pulsecount==1) armedUp=0;
  if (pulsecount==-1) armedDown=0;
  return pulsecount;
}