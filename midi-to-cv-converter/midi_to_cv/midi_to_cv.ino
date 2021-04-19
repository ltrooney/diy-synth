#include <MIDI.h>
#include <LiquidCrystal.h>

/* Note that in order to upload this sketch nothing should be connected to the Arduino Uno UART RX channel. */


/* CONSTANTS */

// pinouts
#define LED 13
#define PWM_OUT 9
#define GATE_PIN 7

// magic numbers
const int START_PITCH = 36;     // MIDI pitch corresponding to lowest piano key
const int AV_TO_DUTY_CYCLE_FACTOR = 51; // factor needed to convert AV from range [0,5] to duty cycle range [0,255]
const int NO_PITCH = 0;

// conversion factors
const int NOTES_PER_OCTAVE = 12;
const int BPM = 120;             // beats per minute
const float BPS = ((float) BPM) / 60; // beats per second
const int NOTES_PER_BEAT = 4;   // number of notes per beat: a value of 8 would correspond to 1/8th notes
const float NOTE_DURATION = 1000 / ((float) (BPS * NOTES_PER_BEAT));  // in millis

// array sizes
const int NOTE_STACK_SIZE = 5;  // size of note_stack array
const int HOLD_PATTERN_MAX_LEN = 16;  // size of hold_pattern

// lcd
const int LCD_REFRESH_RATE = 5; // LCD refresh rate (in Hz)

/* Serial Monitor debugging mode, active when uncommented */
//#define SERMON_DEBUG true // when defined, we can debug program logic from the serial monitor, must comment out in order to use the MIDI library

/* LCD debugging modes, at most 1 of these should be active */
#define LCD_MODE

/* DATA STRUCTURES */

/* Enumeration to track different modes */
enum modes {
  MODE_NORMAL,              // Normal mode
  MODE_ARP_ORDR,            // Arpeggiator mode, ordered
  MODE_ARP_FWD_BKWD,        // Arpeggiator mode, forwards/backwards
  MODE_ARP_RNDM,            // Arpeggiator mode, random order
  MODE_SEQ_ORDR,            // Sequencer mode, ordered
  MODE_SEQ_FWD_BKWD,        // Sequencer mode, forwards/backwards
  MODE_SEQ_RNDM,            // Sequencer mode, random order
  MODE_NORMAL_HOLD = 100,   // Normal mode with hold
  MODE_ARP_ORDR_HOLD,       // Arpeggiator mode, ordered with hold
  MODE_ARP_FWD_BKWD_HOLD,   // Arpeggiator mode, forwards/backwards with hold
  MODE_ARP_RNDM_HOLD,       // Arpeggiator mode, random order with hold
  MODE_SEQ_ORDR_HOLD,       // Sequencer mode, ordered with hold
  MODE_SEQ_FWD_BKWD_HOLD,   // Sequencer mode, forwards/backwards with hold
  MODE_SEQ_RNDM_HOLD,       // Sequencer mode, random order with hold
};

/* Enumeration to track different mode transition FSM states */
enum mode_transition_fsm_states {MODE_TRANS_START, MODE_TRANS_ARM_1, MODE_TRANS_ARM_2};

byte note_stack[NOTE_STACK_SIZE]; // an array-based LIFO stack to keep track of notes that are currently pressed down; it's assumed that all items in the stack are unique */
byte hold_pattern[HOLD_PATTERN_MAX_LEN]; // an array that holds the pattern in ARP/SEQ mode

/* VARIABLES */
float av_out;
int8_t note_stack_top_idx;      // keep track of where the top of the stack is in note_av_stack; -1 implies stack is empty
int8_t note_stack_pos_idx;        // keeps track of the index in the note stack to be played
int8_t hold_pattern_len;          // keeps track of the number of notes in the hold pattern
int8_t hold_pos_idx;              // keeps track of the index in the hold pattern to be played
enum modes current_mode;
enum mode_transition_fsm_states mode_trans_state;
unsigned long last_note_start_time; // time (in millis since program start) that the current note started playing
uint16_t last_note_time_elapsed;
unsigned long lcd_last_refresh_time;  // time (in millis since program start) that lcd last refreshed
int8_t arp_dir;   // arpeggiator direction: 1 for forwards, -1 for backwards
bool end_hit;     // determines if the arpeggiator has hit either the top or bottom end
float hold_gate_duty_cycle;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
MIDI_CREATE_DEFAULT_INSTANCE(); /* create MIDI instance, must be called globally */

void setup() {

#ifdef SERMON_DEBUG
  Serial.begin(9600);
#endif

  // initiliaze variables
  av_out = 0;
  note_stack_top_idx = -1;
  note_stack_pos_idx = 0;
  hold_pos_idx = 0;
  last_note_start_time = 0;
  last_note_time_elapsed = 0;
  lcd_last_refresh_time = 0;
  arp_dir = 1;
  end_hit = false;

  // start in normal state
  current_mode = MODE_NORMAL;
  mode_trans_state = MODE_TRANS_START;

  // setup MIDI library, which only works when we aren't using the serial monitor
#ifndef SERMON_DEBUG
  pinMode(LED, OUTPUT);
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.setHandleNoteOn(handle_note_on);
  MIDI.setHandleNoteOff(handle_note_off);
  MIDI.setHandleControlChange(handle_control_change);
  MIDI.setHandleProgramChange(handle_program_change);
#endif

  // set pin 9 PWM frequency to ~31kHz so DAC low-pass is more precise
  TCCR1B = TCCR1B & B11111000 | B00000001;

  // set up the LCD screen
#ifdef LCD_MODE
  lcd.begin(16, 2);
  lcd.print("press a key");
#endif

#ifdef SERMON_DEBUG
  /* THIS CODE IS ONLY USED TO RUN TESTS ON PROGRAM LOGIC */
  run_test_1();
  run_test_2();
  reset_note_stack();
#endif

}

void loop() {
  MIDI.read();

  // update timer since last new note transition
  if (last_note_start_time == 0) {
    last_note_time_elapsed = 0;
    last_note_start_time = millis();
  } else {
    last_note_time_elapsed = millis() - last_note_start_time;
  }
  
  // update the note stack and hold pattern positions if needed
  if (last_note_time_elapsed >= NOTE_DURATION) {
    last_note_start_time = millis();
    update_note_positions();
  }

  // update the gate signal
  byte gate_out = 0;
  if (in_hold_mode()) {
    if (hold_pattern_len > 0) {
      gate_out = 1;
    }
  } else {
    if (note_stack_top_idx > -1) {
      gate_out = 1;
    }
  }
  digitalWrite(GATE_PIN, gate_out);


  // determine which note to play next
  byte note_to_play;

  /********************************************************************************************************************/
  /*  -------------------------------------------------- CAUTION: --------------------------------------------------- */
  /*  Without mutex we can't be sure the user won't release keys after we've already made a decision to play them     */
  /*  This can cause some buggy out of bounds behaviors when accessing note_stack and hold_pattern.                   */
  /*  Check bounds before accessing an array in order to decrease the odds of an illegal access,                      */
  /********************************************************************************************************************/
  
    
  switch (current_mode) {
    case MODE_NORMAL:
      // play the last note on the note stack
      note_to_play = note_stack_peek();
      break; 
    case MODE_NORMAL_HOLD:
      note_to_play = hold_pattern[hold_pattern_len - 1];
      break;
    case MODE_ARP_ORDR:
    case MODE_ARP_FWD_BKWD:
    case MODE_ARP_RNDM:
      if (note_stack_pos_idx > note_stack_top_idx)
        note_stack_pos_idx = note_stack_top_idx;
      if (note_stack_pos_idx < 0)
        note_stack_pos_idx = 0;
      note_to_play = note_stack[note_stack_pos_idx];
      break;
    case MODE_ARP_ORDR_HOLD:
    case MODE_ARP_FWD_BKWD_HOLD:
    case MODE_ARP_RNDM_HOLD:
      if (hold_pos_idx > (hold_pattern_len - 1))
        hold_pos_idx = hold_pattern_len - 1;
      if (hold_pos_idx < 0)
        hold_pos_idx = 0;
      note_to_play = hold_pattern[hold_pos_idx];
      break;
    case MODE_SEQ_ORDR:
      break;
    case MODE_SEQ_ORDR_HOLD:
      break;
    default:
      note_to_play = NO_PITCH;
      break;
  }

  // convert the note to a PWM duty cycle and output it
  if (note_to_play == NO_PITCH) {
    av_out = 0;
  } else {
    av_out = pitch_to_av(note_to_play);
  }
  analogWrite(PWM_OUT, av_to_duty_cycle(av_out));

  // print to the lcd screen
  #ifdef LCD_MODE
    if (lcd_last_refresh_time == 0 || (millis() - lcd_last_refresh_time) > 100) {
      lcd_last_refresh_time = millis();
      lcd.clear();
      lcd.setCursor(0,0);

      for (int i = 0; i < hold_pattern_len; i++) {
        byte pitch = hold_pattern[i];
        lcd.print(pitch);
        lcd.print(";");
      }
      lcd.setCursor(0, 1);
      lcd.print("T");
      lcd.print(current_mode);
      lcd.print(";");
      lcd.print(note_to_play);
    }
  #endif
  
//  delay(10);
}


/*********************************/
/*    MIDI Callback Functions    */
/*********************************/


void handle_note_on(byte channel, byte pitch, byte velocity) {  
  // clear the hold pattern if no notes were being pressed before this one
  if (note_stack_is_empty()) {
    hold_pattern_clear();
  }

  // add the note to the current set of notes being pressed
  note_stack_push(pitch);
  
  // add the note to the hold pattern, if it's not in there already
  if (hold_pattern_len < HOLD_PATTERN_MAX_LEN) { 
    bool already_exists = false;
    for (int i = 0; i < hold_pattern_len; i++) {
      if (hold_pattern[i] == pitch) {
        already_exists = true;
        break;
      }
    }

    if (!already_exists) {
      hold_pattern[hold_pattern_len] = pitch;
      hold_pattern_len += 1;
    }
  }
}

void handle_note_off(byte channel, byte pitch, byte velocity) {
  // remove the note from the set of notes currently being pressed
  byte removed_pitch = note_stack_remove(pitch);
}

void handle_control_change(byte channel, byte data1, byte data2) {
  // Update mode transition FSM - assumes that first two control messages are sent subsequently
  if (data1 == 0x00 && data2 == 0x01) {
    // Received first of 3 total messages in the mode transition sequence
    if (mode_trans_state == MODE_TRANS_START) {
      mode_trans_state = MODE_TRANS_ARM_1;
    } else {
      mode_trans_state = MODE_TRANS_START;
    }
  } else if (data1 == 0x20 && data2 == 0x00) {
    // Received second of 3 total messages in the mode transition sequence
    if (mode_trans_state == MODE_TRANS_ARM_1) {
      mode_trans_state = MODE_TRANS_ARM_2;
    } else {
      mode_trans_state = MODE_TRANS_START;
    }
  } else {
    mode_trans_state = MODE_TRANS_START;
  }
}

void handle_program_change(byte channel, byte data1) {
  if (mode_trans_state == MODE_TRANS_ARM_2 && is_valid_mode(data1)) {
      // Received last of 3 total messages in the mode transition sequence
      current_mode = data1;

      // reset indices
      arp_dir = 1;
      note_stack_pos_idx = 0;
      hold_pos_idx = 0;
      end_hit = false;
  }
  mode_trans_state = MODE_TRANS_START;  // always go back to start state whether successful mode transition or not
}

/**********************************/
/*          HELPERS               */
/**********************************/


/* Returns true if the given mode is valid */
bool is_valid_mode(byte mode) {
  switch (mode) {
    case MODE_NORMAL:
    case MODE_NORMAL_HOLD:
    case MODE_ARP_ORDR:
    case MODE_ARP_ORDR_HOLD:
    case MODE_ARP_FWD_BKWD:
    case MODE_ARP_FWD_BKWD_HOLD:
    case MODE_ARP_RNDM:
    case MODE_ARP_RNDM_HOLD:
    case MODE_SEQ_ORDR:
    case MODE_SEQ_ORDR_HOLD:
      return true;
  }
  return false;
}

/* Returns true if the current mode is a hold mode. */
bool in_hold_mode() {
  switch (current_mode) {
    case MODE_NORMAL_HOLD:
    case MODE_ARP_ORDR_HOLD:
    case MODE_ARP_FWD_BKWD_HOLD:
    case MODE_ARP_RNDM_HOLD:
    case MODE_SEQ_ORDR_HOLD:
      return true;
  }
  return false;
}

/* Clear the current ARP/SEQ pattern */
void hold_pattern_clear() {
  for (int i = 0; i < HOLD_PATTERN_MAX_LEN; i++) {
    hold_pattern[i] = 0;
  }
  hold_pattern_len = 0;
}

/* Update the hold position index, right now it just goes in circles */
void update_note_positions() {
  if (current_mode == MODE_ARP_ORDR) {
    note_stack_pos_idx += arp_dir;
    if (note_stack_pos_idx >= (note_stack_top_idx + 1)) {    
      note_stack_pos_idx = 0; // reset position
    }
  } else if (current_mode == MODE_ARP_ORDR_HOLD) {
    hold_pos_idx += arp_dir;
    if (hold_pos_idx >= hold_pattern_len) {
      hold_pos_idx = 0;
    }
  } else if (current_mode == MODE_ARP_FWD_BKWD) {
    // check if need to replay the last note and switch directions
    if (note_stack_pos_idx == 0) {
      arp_dir = 0;
      if (end_hit && (note_stack_pos_idx < note_stack_top_idx))
        arp_dir = 1;
    } else if (note_stack_pos_idx == note_stack_top_idx) {    
      arp_dir = 0;
      if (end_hit && (note_stack_pos_idx > 0))
        arp_dir = -1;
    } 
    end_hit = (arp_dir == 0);
    note_stack_pos_idx += arp_dir;
  } else if (current_mode == MODE_ARP_FWD_BKWD_HOLD) {
    // check if need to replay the last note and switch directions
    if (hold_pos_idx == 0) {
      arp_dir = 0;
      if (end_hit && (hold_pos_idx < (hold_pattern_len - 1)))
        arp_dir = 1;
    } else if (hold_pos_idx == (hold_pattern_len - 1)) {
      arp_dir = 0;
      if (end_hit && (hold_pos_idx > 0))
        arp_dir = -1;
    }
    end_hit = (arp_dir == 0); // arp_dir will be 0 if we are currently at an end point
    hold_pos_idx += arp_dir;
  } else if (current_mode == MODE_ARP_RNDM) {
    note_stack_pos_idx = random(0, note_stack_top_idx + 1);
  } else if (current_mode == MODE_ARP_RNDM_HOLD) {
    hold_pos_idx = random(0, hold_pattern_len);
  }
}

/* Convert a given pitch to an analog voltage */
float pitch_to_av(byte pitch) {
  if (pitch == 0) {
    return 0;
  }
  return ((float) (pitch - START_PITCH)) / NOTES_PER_OCTAVE;
}

int av_to_duty_cycle(float av) {
  return (int) (av * AV_TO_DUTY_CYCLE_FACTOR);
}

/* Push a note pitch value to the note stack if there's room, then return success status */
bool note_stack_push(byte pitch) {
  if (note_stack_top_idx < NOTE_STACK_SIZE - 1) {
    note_stack_top_idx += 1;
    note_stack[note_stack_top_idx] = pitch;
#ifdef SERMON_DEBUG
    Serial.print("push ");
    Serial.println(pitch, DEC);
#endif
    return true;
  }
  return false;
}

/* Pop a note pitch value from the note stack if it's not empty */
byte note_stack_pop() {
  if (note_stack_is_empty()) {
#ifdef SERMON_DEBUG
    Serial.println("pop -> empty");
#endif
    return NO_PITCH;
  }
  byte pitch = note_stack[note_stack_top_idx];
  note_stack_top_idx -= 1;
#ifdef SERMON_DEBUG
  Serial.print("pop -> ");
  Serial.println(pitch);
#endif
  return pitch;
}

/* Returns the last note pressed */
byte note_stack_peek() {
  if (note_stack_is_empty()) {
    return NO_PITCH;
  }
  return note_stack[note_stack_top_idx];
}

/* Removes a note pitch value from the note stack */
byte note_stack_remove(byte pitch) {
#ifdef SERMON_DEBUG
  Serial.print("remove ");
  Serial.print(pitch);
  Serial.print(" -> ");
#endif

  if (note_stack_is_empty()) {
#ifdef SERMON_DEBUG
    Serial.println("empty");
#endif
    return NO_PITCH;
  }

  byte removed_pitch = NO_PITCH;
  for (int i = 0; i < NOTE_STACK_SIZE; i++) {
    if (note_stack[i] == pitch) {
      removed_pitch = pitch;

      // shift the rest of the stack one position to the left
      int j;
      for (j = i; j < NOTE_STACK_SIZE - 1; j++) {
        note_stack[j] = note_stack[j + 1];
      }
      note_stack[j] = 0;  // set the last note to 0
      note_stack_top_idx -= 1;
#ifdef SERMON_DEBUG
      Serial.println(removed_pitch);
#endif
      return removed_pitch;
    }
  }
#ifdef SERMON_DEBUG
  if (removed_pitch == NO_PITCH) {
    Serial.println("not found");
  }
#endif

  return removed_pitch;
}

/* Returns true if the note stack is empty, false otherwise */
bool note_stack_is_empty() {
  return note_stack_top_idx == -1;
}

/* Prints the stack to the serial monitor or LCD, depending on the debug mode */
void print_note_stack() {
  lcd.clear();
  lcd.setCursor(0, 0);
  for (int i = 0; i < NOTE_STACK_SIZE; i++) {
    byte pitch = note_stack[i];
#ifdef SERMON_DEBUG
    Serial.print(pitch, HEX);
    Serial.print(",");
#else
    lcd.print(pitch);
    lcd.print(";");
#endif
  }
  lcd.setCursor(0, 1);
  lcd.print("av:");
  lcd.print(av_out);

#ifdef SERMON_DEBUG
  Serial.println();
#endif
}

/* Resets the note stack */
void reset_note_stack() {
  for (int i = 0; i < NOTE_STACK_SIZE; i++) {
    note_stack[i] = NO_PITCH;
  }
  note_stack_top_idx = -1;
}

/********************************************************************************************/
/* This section is for test functions which can only be called when SERMON_DEBUG is defined */
/********************************************************************************************/

/* Tests basic note stack functionality. */
void run_test_1() {
  Serial.println("Running test 1...");
  reset_note_stack();
  print_note_stack(); // print -> should be empty
  note_stack_push(1); // push 1
  note_stack_push(2); // push 2
  note_stack_push(3); // push 3
  print_note_stack(); // print -> should be [1,2,3]
  note_stack_pop();   // pop 3
  note_stack_push(4); // push 4
  note_stack_push(5); // push 5
  print_note_stack(); // print -> should be [1,2,4,5]
  note_stack_push(6); // push 6
  print_note_stack(); // print -> should be [1,2,4,5,6]
  note_stack_push(7); // push 7 -> should fail
  print_note_stack(); // print -> should be [1,2,4,5,6]
}

void run_test_2() {
  Serial.println("Running test 2...");
  reset_note_stack();
  print_note_stack(); // print -> should be empty
  note_stack_push(1); // push 1
  note_stack_push(2); // push 2
  note_stack_push(3); // push 3
  print_note_stack(); // print -> should be [1,2,3,0,0]
  note_stack_remove(2);   // remove 2
  print_note_stack(); // print -> should be [1,3,0,0,0]
  note_stack_remove(3);   // remove 3
  print_note_stack(); // print -> should be [1,0,0,0,0]
  note_stack_push(4); // push 4
  note_stack_push(5); // push 5
  print_note_stack(); // print -> should be [1,4,5,0,0]
  note_stack_remove(3);   // remove 3 -> not found
  note_stack_push(6); // push 6
  note_stack_push(7); // push 7
  note_stack_push(8); // push 8
  note_stack_push(9); // push 9
  note_stack_push(10); // push 10
  print_note_stack(); // print -> should be [1,4,5,6,7]
  note_stack_remove(5);   // remove 5
  print_note_stack(); // print -> should be [1,4,6,7,0]
  note_stack_remove(4);   // remove 4
  print_note_stack(); // print -> should be [1,6,7,0,0]
}


