/**
 * @file interface.h
 *
 * Declares the functions that a backend
 * has to implement
 *
 */

/* Enters the run loop of the selected backend */
void lv_linux_run_loop(void);

/* Initializes the display */
void lv_linux_disp_init(void);
