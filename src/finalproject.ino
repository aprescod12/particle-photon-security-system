/* Project Final Project
* Description: The goal is to design a burglar alarm system with a variety of features. 
* Author: Amiri Prescod
* Completed Date: 05/01/24 
 */

#define LED0 D7                                                                                             // pin D7 is connected to the internal LED => call this pin 'LED0'
#define LED1 D0                                                                                             // pin D0 is connected to the external LED => call this pin 'LED1'
#define BTN1 A1                                                                                             // pin A1 is connected to a push button => call this pin 'BTN1'
#define RLS A4                                                                                              // pin A4 is connected to the Light Sensor
#define WKP A7                                                                                              // pin A7 is connected to the buzzer

volatile bool BtnPressed = false;                                                                           // declare volatile variable to store the when the btn is pressed and set to false
volatile int tickFlag = 0;                                                                                  // declare vairable to flag when the FSM tick is due
volatile bool publishFlag = 0;                                                                              // declare variable to flag to send email
volatile bool flashEnabled = false;                                                                         // declare variable to flag the LED1 flash state
volatile bool BuzzerTone = false;                                                                           // declare variable to flag the state of the BuzzerTone
volatile bool flashEnabled0 = false;                                                                        // declare variable to flag the LED0 flash state

Timer tickTimer(200, setTickFlag);                                                                          // Create a timer that calls setTickFlag function every 200ms

void toggleBtnPressed() {                                                                                   // function to change the state of the BTN1
    BtnPressed = true;                                                                                      // change the flag to high when the button is pressed
    tone(A7, 500, 500);                                                                                     // buzzer sound when the button is pressed
}

int cloudtoggleBtn(String cmd) {                                                                            // function to activate system from the cloud
    if (cmd == "1010") {                                                                                    // set the digital key to activate the system from the cloud
        BtnPressed = true;                                                                                  // change the flag to high when the button is pressed
        tone(A7, 500, 500);                                                                                 // buzzer sound when the button is pressed
        return 1;
    }
    else return -1;
}

void setTickFlag(){                                                                                         // function to set tick flag
    tickFlag=1;                                                                                             // set tick flag to 1                                        
}                   

void setup() {
    pinMode(LED0, OUTPUT);                                                                                  // sets LED0 to be an Output pin
    pinMode(LED1, OUTPUT);                                                                                  // sets LED1 to be an Output pin
    pinMode(BTN1, INPUT_PULLDOWN);                                                                          // sets BTN1 to be an Input pin
    pinMode(RLS, INPUT_PULLUP);                                                                             // sets RLS to be an Input pin  
    pinMode(WKP, OUTPUT);                                                                                   // sets WKP to be an Output pin
    tickTimer.start();                                                                                      // start the tick timer
    attachInterrupt(BTN1, toggleBtnPressed, RISING);                                                        // Attach an interrupt to BTN1 for the rising edge
    Particle.function("Activate Alarm", cloudtoggleBtn);                                                    // register variable to activate system from the cloud
} 

void AlarmSystem() {                                                                                        // Finite State Machine for the System
    static enum StateNames {OFF, ARMING, ARMED, DETECTION, ALARM, RESET} State = OFF;                       // names of the states in the FSM
    static int detcounter;                                                                                  // declare counter for detection state
    static int armcounter;                                                                                  // declare counter for arming state
    static int rescounter;                                                                                  // declare counter for reset state
    switch (State) {
        case OFF:                                                                                           // OFF state
            digitalWrite(LED1, LOW);                                                                        // LED1 turns off
            noTone(A7);                                                                                     // no tone from buzzer
            if (BtnPressed == true) {                                                                       // check if BtnPressed flag is true, if so execute the following     
                State = ARMING;                                                                             // change state to ARMING
                armcounter = 0;                                                                             // set arming counter to 0 
            }
            break;
            
        case ARMING:                                                                                        // ARMING state
            flashEnabled = true;                                                                            // set flashEnabled flag to true
            armcounter++;                                                                                   // arming counter start counting
            if (BtnPressed == true) {                                                                       // check if BtnPressed flag to true, if so execute the following 
                State = OFF;                                                                                // change state to OFF
                flashEnabled = false;                                                                       // set flashEnabled flag back to false
            }
            else if (armcounter >= 12) {                                                                    // check if counter has counted 5 or more seconds, if so execute the following 
                State = ARMED;                                                                              // change state to ARM
                flashEnabled = false;                                                                       // set flashEnabled state back to false
            }
            break;
            
        case ARMED:                                                                                         // ARMED state
            digitalWrite(LED1, HIGH);                                                                       // LED1 turns on
            noTone(A7);                                                                                     // no tone from buzzer
            if (BtnPressed == true){                                                                        // check if BtnPressed flag is true, if so execute the following
                State = OFF;                                                                                // change state to OFF
            }
            else if (digitalRead(RLS) == LOW) {                                                             // check if sensor detects movement, if so execute the following
                State = DETECTION;                                                                          // change state to DETECTION
                detcounter = 0;                                                                             // set detection counter to 0
            }
            break;
            
        case DETECTION:                                                                                     // DETECTION state
            digitalWrite(LED1, HIGH);                                                                       // LED1 turns on
            noTone(A7);                                                                                     // no tone from buzzer
            detcounter++;                                                                                   // detection counter starts counting
            if (BtnPressed == true){                                                                        // check if BtnPressed flag is true, if so execute the following
                State = OFF;                                                                                // change state to OFF
            }
            else if ((digitalRead(RLS) == LOW) && (detcounter >= 15)) {                                     // check if sensor detects movement for 3 or more seconds, if so execute the following
                State = ALARM;                                                                              // change state to ALARM
                publishFlag = true;                                                                         // set flag to send email to be true
            }
            else if (digitalRead(RLS) == HIGH) {                                                            // check if sensor does not detect movement, if so execute the following
                State = ARMED;                                                                              // change state to ARMED
            }
            break;
            
            
        case ALARM:                                                                                         // ALARM state
            BuzzerTone = true;                                                                              // set BuzzerTone flag to be true
            digitalWrite(LED1, HIGH);                                                                       // LED1 turns on
            if (digitalRead(RLS) == HIGH) {                                                                 // check if the sensor does not detect movement, if so execute the following
                State = RESET;                                                                              // change state to RESET
                rescounter = 0;                                                                             // set reset counter to 0;
                flashEnabled0 = true;                                                                       // set flashEnabled0 flag to true
            }
            else if (BtnPressed == true) {                                                                  // check if BtnPressed flag is true, if so execute to following 
                State = OFF;                                                                                // change state to OFF
                BuzzerTone = false;                                                                         // set BuzzerTone flag to false
            }
            break;
        
        case RESET:                                                                                         // RESET state
            flashEnabled0 = true;                                                                           // set flashEnabed0 flag to be true
            rescounter++;                                                                                   // reset counter starts counting
            if (BtnPressed == true) {                                                                       // check if BtnPressed flag is true, if so execute the following
                State = OFF;                                                                                // change state to OFF
                BuzzerTone = false;                                                                         // set BuzzerTone flag to false
                flashEnabled0 = false;                                                                      // set flashEnabled flag to false
            }
            else if (digitalRead(RLS) == LOW) {                                                             // check if the sensor detects movement, if so execute the following 
                State = ALARM;                                                                              // change state to ALARM
                flashEnabled0 = false;                                                                      // set flashEnabled0 flag to false
            }
            else if (rescounter >= 33) {                                                                    // check is the counter has counted 20 or more seconds, if so execute the following 
                State = ARMED;                                                                              // change state to ARMED
                BuzzerTone = false;                                                                         // set BuzzerTone flag to false
                flashEnabled0 = false;                                                                      // set flashEnabled0 flag to false
            }
            break;
            
    }
    BtnPressed = false;                                                                                     // set BtnPressed flag back to false
}
    
    
void loop() {
    if (tickFlag == 1) {                                                                                    // if tickflag is 1, execute the following
        AlarmSystem();                                                                                      // call AlarmSystem FSM
        tickFlag = 0;                                                                                       // clear tickflag
    }
    if (publishFlag == true) {                                                                              // if publishFlag is true, execute the following
        Particle.publish("Motion Detected");                                                                // send 'Motion Detected' event to the cloud
        publishFlag = false;                                                                                // clear publishFlag
    }
     if (flashEnabled == true) {                                                                            // if flashEnabled is true, execute the following
        digitalWrite(LED1, HIGH);                                                                           // turn LED1 on
        delay(200);                                                                                         // wait 200ms
        digitalWrite(LED1, LOW);                                                                            // turn LED1 off
        delay(200);                                                                                         // wait 200ms
    }
    if (BuzzerTone == true) {                                                                               // if BuzzerTone is true, execute the following
        tone(A7, 2000);                                                                                     // play buzzer at frequency of 2000Hz
        delay(100);                                                                                         // wait 100ms
        tone(A7, 1000);                                                                                     // play buzzer at frequency of 1000Hz
        delay(100);                                                                                         // wait 100ms
    }
    if (flashEnabled0 == true) {                                                                            // if flashEnabled0 is true, execute the following
        digitalWrite(LED0, HIGH);                                                                           // turn LED0 on
        delay(200);                                                                                         // wait 200ms
        digitalWrite(LED0, LOW);                                                                            // turn LED0 off
        delay(200);                                                                                         // wait 200ms
    }
}
