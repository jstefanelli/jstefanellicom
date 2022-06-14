#pragma once
#include <filesystem>
#include <vector>
#include <optional>
#include <fstream>

namespace jwx::cache {
	class Cache {
	protected:
		std::filesystem::path local_path;
		std::vector<uint8_t> data;
		std::filesystem::file_time_type file_time;

		explicit Cache(std::filesystem::path path);
	public:
		Cache() = delete;
		static std::optional<Cache> Generate(std::filesystem::path path);

		const std::vector<uint8_t>& Data() const;
		std::filesystem::file_time_type Time() const;
		std::filesystem::path Path() const;

		bool UpToDate() const;
	};
}