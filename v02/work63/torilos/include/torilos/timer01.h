#ifndef TIMER01_H
#define TIMER01_H

#define PIT_CTRL    0x0043  /* PIT Control Register */
#define PIT_CNT0    0x0040  /* PIT Counter0 */

void init_pit();

#endif
