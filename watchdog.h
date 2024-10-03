/*
 * Copyright 2017-2024 Morse Micro
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see
 * <https://www.gnu.org/licenses/>.
 */
#pragma once

#include "morse.h"

typedef int (*watchdog_callback_t)(struct morse *);

/**
 * morse_watchdog_init() - Initialize the driver's watchdog timer
 *
 * @mors: The global morse config object
 * @interval_s: The parameter holds the timeout interval in seconds
 * @ping: The pointer to ping callback function
 *
 * Return 0 if success else error code
 */
int morse_watchdog_init(struct morse *mors, int interval_s,
	watchdog_callback_t ping);

/**
 * morse_watchdog_cleanup() - Cancel an active watchdog timer and
 *                            release allocated memory.
 * @mors: The global morse config object
 *
 * Return 0 if success else error code
 */
int morse_watchdog_cleanup(struct morse *mors);

/**
 * morse_watchdog_start() - Start the watchdog timer
 *
 * @mors: The global morse config object
 *
 * Return 0 if success else error code
 */
int morse_watchdog_start(struct morse *mors);

/**
 * morse_watchdog_stop() - Stop an active watchdog timer
 *
 * @mors: The global morse config object
 *
 * Return 0 if success else error code
 */
int morse_watchdog_stop(struct morse *mors);

/**
 * morse_watchdog_refresh() - Restart a watchdog timer expiry (now + interval)
 *
 * @mors: The global morse config object
 *
 * Return 0 if success else error code
 */
int morse_watchdog_refresh(struct morse *mors);

/**
 * morse_watchdog_pause() - Temporarily pause the watchdog.
 *
 * @mors: The global morse config object
 *
 * This will suspend the watchdog timer until morse_watchdog_resume()
 * is invoked. There will be no further watchdog timeouts until the
 * watchdog is resumed. If the watchdog is stopped and restarted while
 * paused, it will still remain paused until resumed.
 *
 * Return 0 if success else error code
 */
int morse_watchdog_pause(struct morse *mors);

/**
 * morse_watchdog_resume() - Resume the watchdog, if it was paused.
 *
 * @mors: The global morse config object
 *
 * This will resume operation of the watchdog timer following
 * morse_watchdog_pause(). The watchdog timer will be scheduled
 * for (now + interval).
 *
 * Return 0 if success else error code
 */
int morse_watchdog_resume(struct morse *mors);

/**
 * morse_watchdog_get_interval() - Return a watchdog timeout interval in seconds
 *
 * @mors: The global morse config object
 *
 * Return interval (if greater than 0) else error code
 */
int morse_watchdog_get_interval(struct morse *mors);
