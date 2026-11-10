//
// This file is part of the aMule Project.
//
// Copyright (c) 2003-2026 aMule Team ( https://amule-org.github.io )
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

#ifndef KAD_BUDDY_RETRY_H
#define KAD_BUDDY_RETRY_H

#include <cstdint>
#include <map>

namespace Kademlia
{

// Failed outgoing buddy IPs in network byte order. Times are monotonic milliseconds.
class CBuddyRetry
{
public:
	static constexpr uint64_t CandidateCooldown = 60 * 60 * 1000;

	bool CanRequest(uint32_t ip, uint64_t now) const
	{
		const auto entry = m_failed.find(ip);
		return entry == m_failed.end() || now >= entry->second;
	}

	void RecordFailure(uint32_t ip, uint64_t now)
	{
		for (auto it = m_failed.begin(); it != m_failed.end();) {
			if (now >= it->second) {
				it = m_failed.erase(it);
			} else {
				++it;
			}
		}
		m_failed[ip] = now + CandidateCooldown;
	}

private:
	std::map<uint32_t, uint64_t> m_failed;
};

} // namespace Kademlia

#endif // KAD_BUDDY_RETRY_H
