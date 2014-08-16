#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "phage-ui.h"
#include "phage-radio.h"
#include "ledDriver.h"
#include "effects.h"

#include "phage.h"

static THD_WORKING_AREA(waUiThread, 32);
static msg_t ui_thread(void *arg) {
  (void)arg;
  uint8_t rxbuf[3];
  enum pattern oldpat;
  uint8_t s;

  int keyleft = 0;
  int keyright = 1;
  int keydown = 1;

  chRegSetThreadName("ui polling thread");

  while (1) {
    // check everything else here
    if(!palReadPad(GPIOA, PA13)) {
      palWritePad(GPIOA, PA2, PAL_HIGH);
      palWritePad(GPIOA, PA3, PAL_HIGH);
      //ledDriverPause();
      //        chprintf(stream, "x");
#if 0
      radioSend('x');
      chThdSleepMilliseconds(50);  // an extra sleep here to prevent spamming
#endif
      //ledDriverResume();
    } else {
      palWritePad(GPIOA, PA2, PAL_LOW);
      palWritePad(GPIOA, PA3, PAL_LOW);
    }

    if( (palReadPad(GPIOB, PB5) == PAL_HIGH) ) {
      if (keyleft == 0) {
	chThdSleepMilliseconds(20); // debounce time
	keyleft = 1;

	effectsPrevPattern();
      } else {
	keyleft = 1;
      }
    } else {
      keyleft = 0;
    }

    // opposite polarity
    if( (palReadPad(GPIOB, PB4) == PAL_LOW) ) {
      if (keyright == 1) {
	chThdSleepMilliseconds(20); // debounce time
	keyright = 0;

	effectsNextPattern();
      } else {
	keyright = 0;
      }
    } else {
      keyright = 1;
    }

    if( (palReadPad(GPIOA, PA15) == PAL_LOW) ) {
      if (keydown == 1) {
	chThdSleepMilliseconds(20); // debounce time
	keydown = 0;

	s = getShift();
	if (s > 3)
	  s = 0;
	s++;
	setShift(s);
	
      } else {
	keydown = 0;
      }
    } else {
      keydown = 1;
    }


    chThdSleepMilliseconds(30);
  }
  return MSG_OK;
}

void phageUiInit(void) {

  chThdCreateStatic(waUiThread, sizeof(waUiThread),
                    NORMALPRIO-20, ui_thread, NULL);
}
