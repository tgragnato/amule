// SPDX-License-Identifier: GPL-2.0-or-later
// Isolated media parser: input is an inherited, seekable regular file on stdin.
#include "probe.h"
#include "sandbox.h"

#include <algorithm>
#include <clocale>
#include <cerrno>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <sys/resource.h>
#include <sys/types.h>
static_assert(sizeof(off_t) >= sizeof(int64_t), "Media probe requires 64-bit file offsets");

namespace
{
int seek(int64_t offset, int whence)
{
	return fseeko(stdin, offset, whence);
}

int64_t tell()
{
	return ftello(stdin);
}

class StdinSource : public probe::Source
{
public:
	explicit StdinSource(int64_t size)
	: m_size(size)
	{
	}
	int64_t size() const override { return m_size; }
	size_t readAt(int64_t offset, void *buffer, size_t n) override
	{
		if (seek(offset, SEEK_SET))
			return 0;
		return std::fread(buffer, 1, n, stdin);
	}

private:
	int64_t m_size;
};

void quoted(const std::string &s, bool json = true)
{
	std::cout << '"';
	// cleanText() already validates and bounds complete code points.
	for (unsigned char c : s) {
		if (c == '\\' || c == '"')
			std::cout << '\\' << c;
		else if (c == '\n')
			std::cout << "\\n";
		else if (c == '\r')
			std::cout << "\\r";
		else if (c == '\t')
			std::cout << (json ? "\\t" : "\t");
		else if (c >= 32 && c != 127)
			std::cout << c;
	}
	std::cout << '"';
}

uint32_t clamp32(double value)
{
	return value > 0 && std::isfinite(value) ? static_cast<uint32_t>(std::min(value, double(UINT32_MAX)))
						 : 0;
}

int failed(const char *reason, int code = 12)
{
	if (code == 12 || code == probe::sandbox::kSetupFailed)
		std::fprintf(stderr, "media-probe: %s (exit %d, errno %d)\n", reason, code, errno);
	else
		std::fprintf(stderr, "media-probe: %s (exit %d)\n", reason, code);
	return code;
}

bool limit(decltype(RLIMIT_CORE) resource, rlim_t maximum)
{
	rlimit current;
	if (getrlimit(resource, &current) != 0)
		return false;
	// Preserve any tighter limits imposed by the launcher or outer sandbox.
	rlimit value{ std::min(current.rlim_cur, maximum), std::min(current.rlim_max, maximum) };
	return setrlimit(resource, &value) == 0;
}
} // namespace

int main(int argc, char **argv)
{
	std::setlocale(LC_NUMERIC, "C");
	// stdout is a regular file the core reads, so 0 would block the reply. Four times the
	// core's 64 KiB reply cap still leaves no room for any real file write.
	if (!limit(RLIMIT_CORE, 0) || !limit(RLIMIT_CPU, 25) || !limit(RLIMIT_NOFILE, 256) ||
		!limit(RLIMIT_FSIZE, probe::sandbox::kOutputFileLimitBytes))
		return failed(
			"cannot establish CPU, descriptor, output and core-dump limits; refusing to parse",
			probe::sandbox::kSetupFailed);
#ifdef __linux__
	// The in-memory fuzz target uses sanitizers without this address-space cap.
	// Also preserve support for explicitly instrumented helper test builds.
	constexpr bool addressSanitized =
#if defined(__SANITIZE_ADDRESS__)
		true;
#elif defined(__has_feature)
		__has_feature(address_sanitizer);
#else
		false;
#endif
	if (!addressSanitized && !limit(RLIMIT_AS, probe::sandbox::kMemoryLimitBytes))
		return failed("cannot establish address-space limit; refusing to parse",
			probe::sandbox::kSetupFailed);
#endif
	bool json = false;
	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];
		if (arg == "--json")
			json = true;
		else if (arg == "--version") {
			std::cout << "amule-media-probe 1\n";
			return 0;
		} else
			return failed("unknown argument");
	}
	if (seek(0, SEEK_END))
		return failed("input is not seekable");
	const auto size = tell();
	if (size < 0 || seek(0, SEEK_SET))
		return failed("cannot determine input size or rewind input");
	if (!probe::sandbox::enter())
		return failed(
			"cannot establish OS restrictions; refusing to parse", probe::sandbox::kSetupFailed);
	if (!json) {
		// Before parsing: a crash after this line is the file's doing, not a broken install.
		std::cout << "AMMP 1\n" << std::flush;
	}
	probe::Result r;
	int rc = 12;
	try {
		StdinSource source(size);
		rc = probe::probe(source, r);
	} catch (...) {
		return failed("unexpected parser exception; discarding result");
	}
	if (std::ferror(stdin))
		return failed("input read failed; discarding result");
	if (rc)
		return failed(r.failure != nullptr ? r.failure : "parser rejected input", rc);
	// Whole seconds, rounded; the overall bitrate from the exact duration, truncated.
	const uint32_t length = clamp32(std::floor(r.seconds + 0.5));
	const uint32_t bitrate = r.seconds > 0 ? clamp32(double(size) * 8 / r.seconds / 1000) : 0;
	if (!length && r.codec.empty())
		return failed("no usable duration or codec", 10);
	if (json) {
		std::cout << "{\"length_seconds\":" << length << ",\"bitrate_kbps\":" << bitrate;
		for (const auto &item : { std::make_pair("codec", r.codec),
			     { "artist", r.artist },
			     { "album", r.album },
			     { "title", r.title } }) {
			std::cout << ",\"" << item.first << "\":";
			quoted(item.second);
		}
		std::cout << "}\n";
	} else {
		std::cout << "format.duration=" << length << "\nformat.bit_rate=" << uint64_t(bitrate) * 1000
			  << '\n';
		std::cout << "streams.stream.0.codec_type=\"audio\"\nstreams.stream.0.codec_name=";
		quoted(r.codec);
		std::cout << '\n';
		for (const auto &item :
			{ std::make_pair("artist", r.artist), { "album", r.album }, { "title", r.title } }) {
			std::cout << "format.tags." << item.first << '=';
			quoted(item.second, false);
			std::cout << '\n';
		}
		std::cout << "END\n";
	}
	return 0;
}
