#include <stdint.h>

// Version with C++		--> START

//#define TIMEOUT_DELAY_US    (25000)                     // Timeout in microseconds
//#define CALC_COEFF          (340.0f/(2.0f*1000.0f))    // Coefficient for distance calculation (distance = coeff * microseconds)
//#define TICKS_RANGE_MAX     (15000)                   // Maximum reliable measurement - echo pulse width in microseconds
//#define TICKS_RANGE_MIN     (150)                    // Minimum reliable measurement - echo pulse width in microseconds
//#define TRIG_PULSE_US       (50)  

//typedef enum { IDLE, STARTED, COMPLETED, TIMEOUT, OUT_OF_RANGE_MIN, OUT_OF_RANGE_MAX, ERROR_SIG } Distance_HC_SR04_state;

//volatile uint32_t tout_us = TIMEOUT_DELAY_US;
//volatile uint32_t tmin_us = TICKS_RANGE_MIN;
//volatile uint32_t tmax_us = TICKS_RANGE_MAX;

//extern void trigger(void);
//Distance_HC_SR04_state getState(void);
//extern void reset(void);
//extern uint32_t getTicks(void);
//extern float getDistance(void);
//extern float getCoeff(void);
//extern void setCoeff(float coeff);
//extern float measureDistance(void);
//extern uint32_t measureTicks(void);

// Version with C++		-->			END



// Width of trigger pulse in microseconds
//volatile uint8_t ECHO_PIN = 2;     // Just example
//volatile uint8_t TRIGGER_PIN = 3; // Just example
volatile unsigned int a;
//volatile const long MEASURE_TIMEOUT = 25000L; //40Hz = 25 ms = ~8m - 340m/s
//volatile long duration, distance_cm, distance_inc;

//extern void setup(void);
//extern void loop(void);
//extern void pulseIn(void);
extern void HC_SR04_init(uint8_t TP, uint8_t EP);
//extern void HC_SR04_init1(void);  //------------------------------------------------------>

//extern long HC_SR04_ranging(int sys);
//extern long HC_SR04_timing(void);



