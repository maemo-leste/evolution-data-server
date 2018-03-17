/* Evolution calendar - iCalendar file backend
 *
 * Copyright (C) 1999-2008 Novell, Inc. (www.novell.com)
 *
 * Author: Federico Mena-Quintero <federico@ximian.com>
 *
 * This library is free software you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef E_CAL_BACKEND_FILE_EVENTS_H
#define E_CAL_BACKEND_FILE_EVENTS_H

#include "e-cal-backend-file.h"

/* Standard GObject macros */
#define E_TYPE_CAL_BACKEND_FILE_EVENTS \
	(e_cal_backend_file_events_get_type ())
#define E_CAL_BACKEND_FILE_EVENTS(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST \
	((obj), E_TYPE_CAL_BACKEND_FILE_EVENTS, ECalBackendFileEvents))
#define E_CAL_BACKEND_FILE_EVENTS_CLASS(cls) \
	(G_TYPE_CHECK_CLASS_CAST \
	((cls), E_TYPE_CAL_BACKEND_FILE_EVENTS, ECalBackendFileEventsClass))
#define E_IS_CAL_BACKEND_FILE_EVENTS(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE \
	((obj), E_TYPE_CAL_BACKEND_FILE_EVENTS))
#define E_IS_CAL_BACKEND_FILE_EVENTS_CLASS(cls) \
	(G_TYPE_CHECK_CLASS_TYPE \
	((cls), E_TYPE_CAL_BACKEND_FILE_EVENTS))
#define E_CAL_BACKEND_FILE_EVENTS_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS \
	((obj), E_TYPE_CAL_BACKEND_FILE_EVENTS, ECalBackendFileEventsClass))

G_BEGIN_DECLS

typedef ECalBackendFile ECalBackendFileEvents;
typedef ECalBackendFileClass ECalBackendFileEventsClass;

GType		e_cal_backend_file_events_get_type	(void);

G_END_DECLS

#endif /* E_CAL_BACKEND_FILE_EVENTS_H */
