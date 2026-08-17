#include <EEPROM.h>
#include <string.h>

/* --- CONFIGURATION & SETTINGS SECTION --- */
#define COIL_PIN          9  
#define SENSOR_PIN        A0
#define VUSB_PIN          A1
#define BTEN_PIN          2
#define LED_PIN           8
#define SETTINGS_VERSION  4

// ZIEGLER-NICHOLS TEST 
#define DEFAULT_SETPOINT       300   
#define DEFAULT_KP             1.75  
#define DEFAULT_KD             23.5  
#define DEFAULT_KI             0.0017 

#define MAX_INTEGRAL           20000
#define DEFAULT_SENS_FILTER    2
#define DEFAULT_SENS_REVERSED  0
#define DEFAULT_COIL_FILTER    2
#define DEFAULT_CTRL_PERIOD    1
#define DEFAULT_ACTV_THRESHOLD 500   
#define DEFAULT_ACTV_TIMEOUT   2000

#define DEFAULT_COIL_MIN       0
#define DEFAULT_COIL_MAX       200  
#define DEFAULT_COIL_MID       ((DEFAULT_COIL_MAX-DEFAULT_COIL_MIN)/2)

typedef struct {
  int setPoint;
  float kp;
  float ki;
  float kd;
  unsigned int sensFilter;
  bool sensReversed;
  unsigned int coilFilter;
  unsigned int ctrlPeriod;
  unsigned int actvThreshold;
  unsigned int actvTimeout;
  byte coilMin;
  byte coilMax;
  byte coilMid;
} settings_t;

settings_t settings;

/* --- GLOBAL VARIABLES --- */
volatile bool ctrlState = false;
volatile unsigned long reportPeriod = 20; 
volatile unsigned long reportCounter = 0;
volatile bool reportFlag = false;

volatile unsigned long ctrlPeriodCounter = 0;
volatile unsigned long activeTimeoutCounter = 0;

volatile int currentSensor = 1024;
volatile int lastSensor = 1024;
volatile int integralError = 0;
volatile int coilPWM = 0;
volatile unsigned int _sensor = 0;

char inBuffer[64];
byte received = 0;

#define readSensor() _sensor

/* --- EEPROM FUNCTIONS --- */
void memcpy_to_eeprom_with_checksum(unsigned int destination, char *source, unsigned int size) {
  unsigned char checksum = 0;
  for(; size > 0; size--) { 
    checksum = (checksum << 1) | (checksum >> 7);
    checksum += *source;
    EEPROM.write(destination++, *(source++)); 
  }
  EEPROM.write(destination, checksum);
}

int memcpy_from_eeprom_with_checksum(char *destination, unsigned int source, unsigned int size) {
  unsigned char data, checksum = 0;
  for(; size > 0; size--) { 
    data = EEPROM.read(source++);
    checksum = (checksum << 1) | (checksum >> 7);
    checksum += data;     
    *(destination++) = data; 
  }
  return(checksum == EEPROM.read(source));
}

void write_settings() {
  EEPROM.write(0, SETTINGS_VERSION);
  memcpy_to_eeprom_with_checksum(1U, (char*)&settings, sizeof(settings_t));
}

bool read_settings() {
  uint8_t version = EEPROM.read(0);
  if (version == SETTINGS_VERSION) {
    return memcpy_from_eeprom_with_checksum((char*)&settings, 1U, sizeof(settings_t));
  }
  return false;
}

void default_settings() {  
  settings.setPoint = DEFAULT_SETPOINT;
  settings.kp = DEFAULT_KP;
  settings.ki = DEFAULT_KI;
  settings.kd = DEFAULT_KD;
  settings.sensFilter = DEFAULT_SENS_FILTER;
  settings.sensReversed = DEFAULT_SENS_REVERSED;
  settings.coilFilter = DEFAULT_COIL_FILTER;
  settings.ctrlPeriod = DEFAULT_CTRL_PERIOD;
  settings.actvTimeout = DEFAULT_ACTV_TIMEOUT;
  settings.actvThreshold = DEFAULT_ACTV_THRESHOLD;
  settings.coilMin = DEFAULT_COIL_MIN;
  settings.coilMid = DEFAULT_COIL_MID;
  settings.coilMax = DEFAULT_COIL_MAX;
}

/* --- HARDWARE CONTROL --- */
void writeCoilPWM(int value) {
  #if COIL_PIN == 9
    OCR1A = value;  
  #elif COIL_PIN == 10
    OCR1B = value;
  #endif
}

void changeState(bool active) {
  if(ctrlState == active) return;
  digitalWrite(LED_PIN, active ? LOW : HIGH);
  ctrlState = active;
}

void doIdle() {
  currentSensor = readSensor();
  if(settings.actvThreshold > currentSensor) {
    lastSensor = currentSensor;
    changeState(true);
  }
}

void doActive() {
  currentSensor = ((currentSensor * (settings.sensFilter - 1)) + (int)readSensor()) / settings.sensFilter;
  if(settings.actvThreshold <= currentSensor) {
    if(activeTimeoutCounter == 0) {
      coilPWM = 0;
      writeCoilPWM(0);
      changeState(false);
    }
    return; 
  } 
  activeTimeoutCounter = settings.actvTimeout;
  int error = settings.setPoint - currentSensor;
  int dError = currentSensor - lastSensor; 
  integralError = constrain(integralError + error, -MAX_INTEGRAL, MAX_INTEGRAL);
  lastSensor = currentSensor;

  // PID 
  int currentPWM = settings.coilMid - ((settings.kp*error) - (settings.kd*dError) + (settings.ki*integralError));
  coilPWM = ((coilPWM * (settings.coilFilter-1)) + currentPWM ) / settings.coilFilter;
  coilPWM = constrain(coilPWM, settings.coilMin, settings.coilMax);
  writeCoilPWM(coilPWM);
}

/* --- INTERRUPTS --- */
ISR(ADC_vect){
  unsigned int s = ADCL | (ADCH << 8);
  _sensor = settings.sensReversed ? 1024 - s : s;
}

ISR(TIMER2_OVF_vect) {
  TCNT2 = (256 - (16000/128)); 
  if(--ctrlPeriodCounter == 0) {
    if(ctrlState) doActive();
    else doIdle();
    ctrlPeriodCounter = settings.ctrlPeriod;
  }
  if(activeTimeoutCounter > 0) --activeTimeoutCounter;
  if(reportCounter > 0) {
    --reportCounter;
  } else {
    reportFlag = true;
    reportCounter = reportPeriod;
  }
}

/* --- REPORTING --- */
void sendReport() {
  Serial.print(currentSensor);
  Serial.print(",");
  Serial.print(settings.setPoint);
  Serial.print(",");
  Serial.println(coilPWM);
}

/* --- MAIN SETUP & LOOP --- */
void setup() { 
  pinMode(LED_PIN, OUTPUT);
  pinMode(COIL_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);

  
  default_settings(); 
  write_settings();

  noInterrupts();
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM10);
  TCCR1B = _BV(CS10);
  TCCR2A = 0;
  TCCR2B = (1<<CS22) | (1<<CS20); 
  TIMSK2 |= _BV(TOIE2);
  ADMUX = _BV(REFS0); 
  ADCSRA = _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
  ADCSRB = 0;
  ADCSRA |= _BV(ADSC);
  interrupts();

  Serial.begin(115200);
  ctrlPeriodCounter = settings.ctrlPeriod;
}

void loop() {
  if(reportFlag) {
    sendReport();
    reportFlag = false;
  }
}
