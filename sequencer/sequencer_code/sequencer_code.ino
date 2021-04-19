#include <math.h>

/* CONSTANTS */

// pinouts
#define PIN_STRT_STP 2
#define PIN_CLK_IN_INV 3
#define PIN_MUX_SEL_0 4
#define PIN_MUX_SEL_1 5
#define PIN_MUX_SEL_2 6
#define PIN_GATE_OUT 8
#define PIN_1VPO_OUT 9
#define PIN_PITCH_MUX A0
#define PIN_GATE_LEN_POT A1
#define PIN_SEQ_LEN_POT A2

// conversion factors
const int AV_TO_DUTY_CYCLE_FACTOR = 51; // factor needed to convert AV from range [0,5] to duty cycle range [0,255]
const float ONE_STEP_VOLT = 1 / 12.0;     // the voltage difference between notes
const int MAX_STEPS = 8;

/* Serial Monitor debugging mode, active when uncommented */
//#define SERMON_DEBUG // the serial monitor will be activated when SERMON_DEBUG is defined

/* VARIABLES */
volatile unsigned long clk_period_strt;           // time (in millis since program start) that the current clock period started
volatile unsigned long last_clk_period_strt;      // time the previous clock period started, used to determine clock frequency
float clk_period;              // clock period (in millis)
uint16_t step_time_elapsed;    // time (in millis) that the current step has been playing for
bool last_iter_gate_high;      // determines if gate was high in the last iteration; used to avoid excessive calls to digitalWrite()
unsigned long step_cnt;        // number of steps elapsed since start of program
volatile bool new_step;        // becomes active if the step transitions in the current loop iteration
volatile bool stop_stepping;   // flag to indicate if the sequencer should stop sequencing; controlled by the start/stop button
int num_steps;                 // number of steps in the sequence


void setup() {

#ifdef SERMON_DEBUG
  Serial.begin(9600);
#endif

  pinMode(PIN_MUX_SEL_0, OUTPUT);
  pinMode(PIN_MUX_SEL_1, OUTPUT);
  pinMode(PIN_MUX_SEL_2, OUTPUT);
  pinMode(PIN_GATE_OUT, OUTPUT);
  pinMode(PIN_1VPO_OUT, OUTPUT);
  pinMode(PIN_STRT_STP, INPUT);
  pinMode(PIN_CLK_IN_INV, INPUT);

  attachInterrupt(digitalPinToInterrupt(PIN_STRT_STP), start_stop_handle, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_CLK_IN_INV), clk_handle, FALLING);

  // initiliaze variables
  clk_period_strt = 0;
  last_clk_period_strt = 0;
  step_time_elapsed = 0;
  clk_period = 0;
  step_cnt = 0;
  new_step = false;
  last_iter_gate_high = false;
  stop_stepping = false;

  // set pin 9 PWM frequency to ~31kHz so DAC low-pass is more precise
  TCCR1B = TCCR1B & B11111000 | B00000001;
}

void loop() {
  // Wait if "stop" button pressed or haven't received clock trigger yet
  while (stop_stepping || (last_clk_period_strt == 0));

  // determine clock period
  clk_period = clk_period_strt - last_clk_period_strt;

  if (new_step) {

    new_step = false;

    /* DETERMINE STEP NUMBER */
    float seq_len_v = get_wiper_voltage(PIN_SEQ_LEN_POT);
    seq_len_v *= (7.0 / 5); // convert range from [0,5] to [0,7]
    num_steps = ((int) seq_len_v) + 1;

    if (num_steps > MAX_STEPS) {
      num_steps = MAX_STEPS;
    }

    uint8_t step_num = step_cnt % num_steps;
    step_cnt++;
        
    /* READ PITCH POTS */
    // set up the mux
    byte s0 = step_num & 0x01;
    byte s1 = (step_num >> 1) & 0x01;
    byte s2 = (step_num >> 2) & 0x01;
    digitalWrite(PIN_MUX_SEL_0, s0);
    digitalWrite(PIN_MUX_SEL_1, s1);
    digitalWrite(PIN_MUX_SEL_2, s2);
    float pitch_v = get_wiper_voltage(PIN_PITCH_MUX);

    /* QUANTIZE PITCH */
    pitch_v = ONE_STEP_VOLT * floor(pitch_v / ONE_STEP_VOLT);
  
    /* SEND PITCH OUT */
    analogWrite(PIN_1VPO_OUT, av_to_duty_cycle(pitch_v));
     
    /* SEND GATE OUT */
    set_gate(true);
  }

  /* READ GATE LENGTH POT */
  float gate_len_v = get_wiper_voltage(PIN_GATE_LEN_POT);
  float gate_duty_cycle = gate_len_v * 20;  // convert range from [0,5] to [0,100]
  float gate_dur = clk_period * (gate_duty_cycle / 100);

  // Determine if step has been activated longer than the gate length
  step_time_elapsed = millis() - clk_period_strt;
  if ((step_time_elapsed > gate_dur) && last_iter_gate_high) { 
    set_gate(false);
  }  

  delay(10);
}

/**********************************/
/*          HELPERS               */
/**********************************/

/* Reads the voltage at analog pin 'pin_name' */
float get_wiper_voltage(int pin) {
  int val = analogRead(pin);
  val = 1023 - val;   // invert due to inverted pots
  return val / 204.6; // convert range from [0,1023] to [0,5]
}

/* Convert a 1V/octave analog voltage to a PWM duty cycle */
int av_to_duty_cycle(float av) {
  return (int) (av * AV_TO_DUTY_CYCLE_FACTOR);
}

void set_gate(bool hi_lo) {
  digitalWrite(PIN_GATE_OUT, hi_lo);
  last_iter_gate_high = hi_lo;
}

/**********************************/
/*            ISRs                */
/**********************************/
void start_stop_handle() {
  static unsigned long last_press_time = 0;
  unsigned long curr_int_time = millis();
  if (curr_int_time - last_press_time > 200) {  // avoid switch debounce
    stop_stepping = !stop_stepping;
    if (stop_stepping) {
      set_gate(false);
    }
  }
  last_press_time = curr_int_time;
}

void clk_handle() {
  last_clk_period_strt = clk_period_strt;
  clk_period_strt = millis();
  new_step = true;
}
