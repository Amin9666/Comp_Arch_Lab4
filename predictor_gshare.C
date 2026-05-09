/*
 * GShare Branch Predictor
 *
 * Combines a 14-bit Global History Register (GHR) with the lower 14 bits of
 * the PC via XOR to index into a Branch History Table (BHT) of 16K entries.
 * Each entry is a 2-bit saturating counter.
 *
 * Index = (GHR ^ pc) & 0x3FFF
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
 *             Shift GHR left by 1, insert outcome in LSB, mask to 14 bits.
 *
 * Budget: 16384 entries * 2 bits + 14-bit GHR = (32K + 14) bits.
 */

#include "predictor_gshare.h"
#include <string.h>

unsigned char bht[BHT_ENTRIES];
unsigned int  ghr;

void init_predictor ()
{
  /* Initialize all counters to 0 (Strongly Not Taken) and GHR to 0 */
  memset (bht, 0, sizeof (bht));
  ghr = 0;
}

bool make_prediction (unsigned int pc)
{
  unsigned int index = (ghr ^ pc) & INDEX_MASK;
  return bht[index] >= 2;
}

void train_predictor (unsigned int pc, bool outcome)
{
  unsigned int index = (ghr ^ pc) & INDEX_MASK;

  /* Update the 2-bit saturating counter */
  if (outcome) {
    if (bht[index] < 3)
      bht[index]++;
  } else {
    if (bht[index] > 0)
      bht[index]--;
  }

  /* Shift GHR left, insert new outcome in LSB, keep only 14 bits */
  ghr = ((ghr << 1) | (outcome ? 1 : 0)) & GHR_MASK;
}
