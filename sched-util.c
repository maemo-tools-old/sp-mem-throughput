/* This file is part of sp-mem-throughput.
 *
 * Copyright (C) 2010 by Nokia Corporation
 *
 * Authors: Tommi Rantala
 * Contact: Eero Tamminen <eero.tamminen@nokia.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "sched-util.h"

#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>

const char *
sched_policy_to_str(int policy)
{
	static char buf[128];
#ifdef SCHED_OTHER
	if (policy == SCHED_OTHER) return "SCHED_OTHER";
#endif
#ifdef SCHED_BATCH
	if (policy == SCHED_BATCH) return "SCHED_BATCH";
#endif
#ifdef SCHED_IDLE
	if (policy == SCHED_IDLE)  return "SCHED_IDLE";
#endif
#ifdef SCHED_FIFO
	if (policy == SCHED_FIFO)  return "SCHED_FIFO";
#endif
#ifdef SCHED_RR
	if (policy == SCHED_RR)    return "SCHED_RR";
#endif
	snprintf(buf, sizeof(buf), "SCHED_(%d)", policy);
	buf[sizeof(buf)-1] = '\0';
	return buf;
}

void
raise_priority(void)
{
	struct sched_param param;
	(void) setpriority(PRIO_PROCESS, 0, -20);
	(void) memset(&param, 0, sizeof(struct sched_param));
#ifdef SCHED_FIFO
	param.sched_priority = sched_get_priority_max(SCHED_FIFO);
	if (sched_setscheduler(0, SCHED_FIFO, &param) == 0) return;
	param.sched_priority = sched_get_priority_min(SCHED_FIFO);
	if (sched_setscheduler(0, SCHED_FIFO, &param) == 0) return;
#endif /* SCHED_FIFO */
#ifdef SCHED_RR
	param.sched_priority = sched_get_priority_max(SCHED_RR);
	if (sched_setscheduler(0, SCHED_RR, &param) == 0) return;
	param.sched_priority = sched_get_priority_min(SCHED_RR);
	if (sched_setscheduler(0, SCHED_RR, &param) == 0) return;
#endif /* SCHED_RR */
}

void
print_sched_info(FILE *stream)
{
	int prio, sched_pol;
	struct sched_param sched_p;
	const char *sched_pol_str;
	prio = getpriority(PRIO_PROCESS, 0);
	(void) memset(&sched_p, 0, sizeof(struct sched_param));
	(void) sched_getparam(0, &sched_p);
	sched_pol = sched_getscheduler(0);
	sched_pol_str = sched_policy_to_str(sched_pol);
	fprintf(stream,
"Scheduling information:\n"
"    Priority: %d [highest: -20, lowest: 19]\n"
"    Scheduling policy: %s\n"
"    Scheduling priority: %d [highest: %d, lowest:%d]\n"
"\n",
	       prio, sched_pol_str, sched_p.sched_priority,
	       sched_get_priority_max(sched_pol),
	       sched_get_priority_min(sched_pol));
}
