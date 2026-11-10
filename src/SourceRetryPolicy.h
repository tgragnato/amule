//
// This file is part of the aMule Project.
//
// Copyright (c) 2003-2026 aMule Team ( https://amule-org.github.io )
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
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
//

#ifndef SOURCERETRYPOLICY_H
#define SOURCERETRYPOLICY_H

#include <cstdint>

// Keep retry policy independent of clients, sockets and the application clock.
// Shareaza's source lifecycle inspired this separation; these are aMule's
// existing eD2k intervals, not Shareaza's exponential backoff.
namespace SourceRetryPolicy
{
inline bool IsDue(uint64_t now, uint64_t lastAsked, uint64_t delay)
{
	// Zero retains the caller's explicit "ask on the next process tick" meaning.
	return lastAsked == 0 || now - lastAsked > delay;
}

inline uint64_t DeadSourceDelay(bool global, bool lowID)
{
	return static_cast<uint64_t>((global ? 30 : 45) + (lowID ? 15 : 0)) * 60000;
}

inline uint64_t DeadSourceDeadline(uint64_t now, bool global, bool lowID)
{
	return now + DeadSourceDelay(global, lowID);
}
} // namespace SourceRetryPolicy
#endif
