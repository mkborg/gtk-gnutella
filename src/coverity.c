/*
 * Copyright (c) 2014 Raphael Manfredi
 *
 *----------------------------------------------------------------------
 * This file is part of gtk-gnutella.
 *
 *  gtk-gnutella is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  gtk-gnutella is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with gtk-gnutella; if not, write to the Free Software
 *  Foundation, Inc.:
 *      59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *----------------------------------------------------------------------
 */

/**
 * @ingroup core
 * @file
 *
 * Coverity model for key functions.
 *
 * @author Raphael Manfredi
 * @date 2014
 *
 * This file is to help Coverity understand our code better, to limit
 * the potential for false positives.
 */

typedef int bool;

bool
__builtin_expect(bool expr, bool hint)
{
	return expr;
}

void
assertion_abort(void)
{
	__coverity_panic__();
}

void
log_abort(void)
{
	__coverity_panic__();
}

void *
vmm_valloc(void *hint, size_t size)
{
	__coverity_negative_sink__(size);
	return __coverity_alloc__(size);
}

int
munmap(void *addr, size_t size)
{
	__coverity_negative_sink__(size);
	__coverity_free__(addr);
}

struct cevent { int x; };
struct cqueue { int x; };

typedef void (*cq_service_t)(struct cqueue *cq, void *udata);

struct cevent *
cq_main_insert(int delay, cq_service_t fn, void *arg)
{
	static struct cqueue cq;
	static struct cevent e;

	fn(&cq, arg);
	return &e;
}

const char *
get_variable(const char *s, const char **end)
{
	__coverity_tainted_data_sanitize__(s);
	return s;
}

/* vi: set ts=4 sw=4 cindent: */
