//
// This file is part of the aMule Project.
//
// Copyright (c) 2003-2011 Alo Sarv ( madcat_@users.sourceforge.net )
// Copyright (c) 2003-2026 aMule Team ( https://amule-org.github.io )
// Copyright (c) 2002-2011 Timo Kujala ( tiku@users.sourceforge.net )
//
// Any parts of this program derived from the xMule, lMule or eMule project,
// or contributed by third-party developers are copyrighted by their
// respective authors.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301, USA
//

#include "GetTickCount.h" // Interface

// seconds from app start-up, check GetTickCount.h
std::atomic<uint32> TheTime{ 0 };

#include <time.h> // Needed for clock_gettime

// In milliseconds, not seconds: avoids the 32-bit rollover error for differences above 50 days,
// 2**32 milliseconds being roughly 50 days.
uint64 GetTickCount64(void)
{
	struct timespec ts;
	uint64 msecs;

	// Fetch time (Y2038-safe). CLOCK_MONOTONIC: the tick count is for timeouts and deltas and
	// must not jump when the wall clock is adjusted. Callers needing a Unix timestamp
	// (CClientCredits::SetLastSeen, partfile source-seeds serialization, wxCas defaults) should
	// use time(NULL) instead.
	clock_gettime(CLOCK_MONOTONIC, &ts);
	msecs = (uint64)ts.tv_sec * 1000;
	msecs += ts.tv_nsec / 1000000;
	return msecs;
}

/**
 * Copyright (c) 2002-2011 Timo Kujala ( tiku@users.sourceforge.net )
 *
 * gettimeofday() syscall based implementation. Each GetTickCount() call produces a new syscall, so
 * it grows increasingly heavy on CPU as uptime increases and more things need doing.
 */
void StartTickTimer() {}

void StopTickTimer() {}
