/*
 * 2-bit Saturating Counter Branch Predictor
 *
 * Uses the lower 14 bits of the PC to index into a Branch History Table (BHT)
 * of 16K entries. Each entry is a 2-bit saturating counter.
 *
 * States:
 *   0 - Strongly Not Taken (SN)
 *   1 - Weakly Not Taken   (WN)
 *   2 - Weakly Taken       (WT)
 *   3 - Strongly Taken     (ST)
 *
 * Prediction: Taken if counter >= 2, Not Taken if counter <= 1.
 * Training:   Increment counter on taken (saturate at 3),
 *             decrement counter on not-taken (saturate at 0).
 *
 * Budget: 16384 entries * 2 bits = 32768 bits = 32K bits.
 */

#include "predictor_2sc.h"
#include <string.h>

unsigned char bht[BHT_ENTRIES];

void init_predictor ()
{
  /* Initialize all counters to 0 (Strongly Not Taken) */
  memset (bht, 0, sizeof (bht));
}

bool make_prediction (unsigned int pc)
{
  unsigned int index = pc & INDEX_MASK;
  /* Predict taken if counter is in Weakly Taken or Strongly Taken state */
  return bht[index] >= 2;
}

void train_predictor (unsigned int pc, bool outcome)
{
  unsigned int index = pc & INDEX_MASK;
  if (outcome) {
    /* Branch was taken: increment counter, saturate at 3 */
    if (bht[index] < 3)
      bht[index]++;
  } else {
    /* Branch was not taken: decrement counter, saturate at 0 */
    if (bht[index] > 0)
      bht[index]--;
  }
}
