// Matrix Rain
// Control an LED matrix to emulate falling droplets - as the code in the movie "The Matrix".
// by Andy-K-To

#include "FastLED.h"
#include "timer.h"
#include "droplet.h"

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define ROW_MAJOR         1
#define COLUMN_MAJOR      2
#define SNAKE_HORIZONTAL  3
#define SNAKE_VERTICAL    4


/******************************************************************************/
/*** PROGRAM SETTINGS - ADAPT TO YOUR NEEDS! **********************************/
/******************************************************************************/

// Hardware definitions.
#define DATA_PIN    7
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB


// Effect settings.
constexpr unsigned long REFRESH_PERIOD_MS = 20;
constexpr int MAX_BRIGHTNESS = 50;
constexpr unsigned long DROPLET_MIN_RESPAWN_DELAY_MS = 500;
constexpr unsigned long DROPLET_MAX_RESPAWN_DELAY_MS = 5000;
constexpr unsigned long DROPLET_MIN_FALL_DELAY_MS = 50;
constexpr unsigned long DROPLET_MAX_FALL_DELAY_MS = 150;

// "The Matrix" color scheme
const CRGB COLOR_INITIAL(MAX_BRIGHTNESS/2, MAX_BRIGHTNESS, 0);
const CRGB COLOR_REST(0, 0, 0);
const CRGB COLOR_DECAY(20, 1, 1);

// "Fire" color scheme
// const CRGB COLOR_INITIAL(MAX_BRIGHTNESS, MAX_BRIGHTNESS, 0);
// const CRGB COLOR_REST(0, 0, 0);
// const CRGB COLOR_DECAY(1, 5, 1);


// Matrix dimensions.
#define ORDERING SNAKE_HORIZONTAL
constexpr int ROWS = 32;
constexpr int COLS = 8;
constexpr int NUM_LEDS = ROWS * COLS;
constexpr int DROPLETS_PER_COLUMN = 3;
constexpr int NUM_DROPLETS = DROPLETS_PER_COLUMN * COLS;



/******************************************************************************/
/*** GLOBAL :( VARIABLES ******************************************************/
/******************************************************************************/

// Global array with all LEDs.
CRGB leds[NUM_LEDS];

// Auxiliary function to access LEDs as a matrix.
CRGB& matrix(unsigned int row, unsigned int col) {
  unsigned int idx = 
    #if ORDERING==ROW_MAJOR
      row * COLS + col;
    #elif ORDERING==COLUMN_MAJOR
      col * ROWS + row;
    #elif ORDERING==SNAKE_HORIZONTAL
      row * COLS + (row % 2 == 0 ? col : COLS - 1 - col);
    #elif ORDERING==SNAKE_VERTICAL
      // row * COLS + (row % 2 == 0 ? COLS - 1 - col : col);
      col * ROWS + (col % 2 == 0 ? row : ROWS - 1 - row);
    #else
    #error "You must set the macro ORDERING to allow addressing LEDs as a matrix"
    #endif
  return leds[idx];
}

// Array with droplets.
Droplet* droplets[NUM_DROPLETS];

// Timer to "flush" colors when needed.
Timer global_timer(REFRESH_PERIOD_MS);

// Timer to dim colors - it has the same rate as the global one.
Timer dim_timer(REFRESH_PERIOD_MS);


/******************************************************************************/
/*** MAIN PROGRAM - NOTHING TO CHANGE HERE (IN THEORY!) ***********************/
/******************************************************************************/

void setup() {
  // 3 seconds delay for recovery.
  delay(3000);
  
  // Tell FastLED about the LED strip configuration.
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip)
    .setDither(MAX_BRIGHTNESS < 255);

  // Initialize droplets.
  for(unsigned int i=0; i<NUM_DROPLETS; i++) {
    droplets[i] = new Droplet(
      i % COLS,
      ROWS,
      DROPLET_MIN_RESPAWN_DELAY_MS,
      DROPLET_MAX_RESPAWN_DELAY_MS,
      DROPLET_MIN_FALL_DELAY_MS,
      DROPLET_MAX_FALL_DELAY_MS
    );
  }
}


void loop()
{
  animate_droplets();

  if(global_timer.tictoc())
    FastLED.show();
}


void animate_droplets() {
  if(dim_timer.tictoc()) {
    for(unsigned int i=0; i<NUM_LEDS; i++) {
      leds[i] -= COLOR_DECAY;
      leds[i] |= COLOR_REST;
    }
  }

  for(unsigned int i=0; i<NUM_DROPLETS; i++) {
    if(droplets[i]->visible()) {
        matrix(droplets[i]->row(), droplets[i]->col()) = COLOR_INITIAL;
    }
    
    droplets[i]->update();
    
    if(droplets[i]->visible()) {
      matrix(droplets[i]->row(), droplets[i]->col()) = CRGB(MAX_BRIGHTNESS, MAX_BRIGHTNESS, MAX_BRIGHTNESS);
    }
  }
}
