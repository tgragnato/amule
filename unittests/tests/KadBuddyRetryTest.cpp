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

#include <muleunit/test.h>
#include <KadBuddyRetry.h>

using namespace muleunit;
using Kademlia::CBuddyRetry;

DECLARE_SIMPLE(KadBuddyRetry)

TEST(KadBuddyRetry, FailedIpIsSkippedUntilCooldownExpires)
{
	CBuddyRetry retry;
	const uint32_t ip = 0x12345678;
	const uint64_t now = uint64_t(1) << 33;
	ASSERT_TRUE(retry.CanRequest(ip, now));
	retry.RecordFailure(ip, now);
	ASSERT_FALSE(retry.CanRequest(ip, now + 30000));
	ASSERT_FALSE(retry.CanRequest(ip, now + CBuddyRetry::CandidateCooldown - 1));
	ASSERT_TRUE(retry.CanRequest(ip, now + CBuddyRetry::CandidateCooldown));
	ASSERT_TRUE(retry.CanRequest(ip + 1, now));
}

TEST(KadBuddyRetry, AddingFailuresPreservesUnexpiredIpsAndAllowsExpiredIpsAgain)
{
	CBuddyRetry retry;
	retry.RecordFailure(1, 0);
	retry.RecordFailure(2, 1000);
	const uint64_t now = CBuddyRetry::CandidateCooldown;
	retry.RecordFailure(3, now);
	ASSERT_TRUE(retry.CanRequest(1, now));
	ASSERT_FALSE(retry.CanRequest(2, now));
	ASSERT_FALSE(retry.CanRequest(3, now));
	retry.RecordFailure(1, now);
	ASSERT_FALSE(retry.CanRequest(1, now));
	ASSERT_TRUE(retry.CanRequest(2, now + 1000));
}
