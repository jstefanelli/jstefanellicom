#pragma once
#include <filesystem>
#include <vector>
#include <optional>
#include <fstream>
#include <memory>
#include <chrono>

#define JWX_CACHE_MAX_FILE_SIZE 20ULL * 1024ULL * 1024ULL
//#define JWX_CACHE_MAX_FILE_SIZE 64

namespace jwx::cache {
	class Cache {
	protected:
		std::filesystem::path local_path;
		std::shared_ptr<std::vector<uint8_t>> data;
		std::filesystem::file_time_type file_time;
		std::chrono::high_resolution_clock::time_point last_access_time;
		size_t size;

		explicit Cache(std::filesystem::path path);
	public:
		Cache() = delete;
		static std::shared_ptr<Cache> Generate(std::filesystem::path path);

		const size_t Size() const;
		const std::shared_ptr<const std::vector<uint8_t>> Data() const;
		std::filesystem::file_time_type Time() const;
		std::filesystem::path Path() const;

		bool UpToDate() const;
		void UpdateAccessTime();
		const std::chrono::high_resolution_clock::time_point LastAccessTime() const;
	};
}