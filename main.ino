// Partly uses: https://www.hackster.io/Elite_Worm/build-a-laser-tachometer-c5a320

const byte RPM_SENSOR_PIN = 2;
const byte DRILL_PIN = 3;
const double pulse_threshold = 0.99;

volatile unsigned long t_pulse_started_volatile = 0;
volatile unsigned long t_pulse_duration_volatile = 0;
unsigned long t_pulse_started = 0;
unsigned long t_pulse_duration = 0;
unsigned long t_pulse_duration_l = 2000000;

/* Not using rpm, using the inverse (rotation period) instead.
Easier to calculate, and arithmetic mean of periods (which would be the harmonic mean of rpm) 
is more correct than having arithmetic mean of rpm (which is the harmonic mean of the periods.) */

volatile bool timeout = 1;
volatile bool newpulse = 0;

void setup() {
  Serial.begin(9600);
  
  pinMode(DRILL_PIN, OUTPUT);
  analogWrite(DRILL_PIN, 255);
  
  pinMode(RPM_SENSOR_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(RPM_SENSOR_PIN), ISR_sensor, RISING); // or falling
}

void loop() {
  t_pulse_duration_l = t_pulse_duration;
  noInterrupts();
  t_pulse_started = t_pulse_started_volatile;
  t_pulse_duration = t_pulse_duration_volatile;
  interrupts();

  if (((micros() - t_pulse_started) > 2000000) && timeout == 0 && newpulse == 0) {
    timeout = 1;
    t_pulse_duration = 2000000;
  };

  if (timeout == 0 && newpulse) {
    Serial.print(t_pulse_duration); // for data collection stage only, may affect timing significantly
    newpulse = 0;
    if (static_cast<double>(t_pulse_duration_l) / t_pulse_duration < pulse_threshold) {
      analogWrite(DRILL_PIN, 0);
    }
    
    
  }
}

void ISR_sensor() {
  unsigned long micros_temp = micros();
  t_pulse_duration_volatile = micros_temp - t_pulse_started_volatile;
  t_pulse_started_volatile = micros_temp;
  timeout = 0;
  newpulse = 1;

}
