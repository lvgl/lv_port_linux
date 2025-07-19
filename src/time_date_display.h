#ifndef TIME_DISPLAY_H
#define TIME_DISPLAY_H

/**
 * Initializes the time and date display.
 * Creates labels to display the current time and date at the top-left of the screen.
 * @note Must be called after lvgl initialization.
 */
void init_time_and_date_display(void);

/**
 * Updates the time and date display.
 * Refreshes the labels with the current time and date based on the system clock.
 * @note Should be called periodically (e.g., every minute) to keep the display up-to-date.
 */
void update_time_and_date_display(void);

#endif
