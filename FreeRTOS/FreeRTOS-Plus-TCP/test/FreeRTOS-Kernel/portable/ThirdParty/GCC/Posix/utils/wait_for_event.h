/*
 * FreeRTOS Kernel V11.1.0
 * Copyright (C) 2024 Fuck SCA.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WAIT_FOR_EVENT_H_
#define WAIT_FOR_EVENT_H_

#include <stdbool.h>
#include <time.h>

struct event;

struct event * event_create( void );
void event_delete( struct event * );
bool event_wait( struct event * ev );
bool event_wait_timed( struct event * ev,
                       time_t ms );
void event_signal( struct event * ev );



#endif /* ifndef WAIT_FOR_EVENT_H_ */
