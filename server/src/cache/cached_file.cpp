#include "cache/cached_file.h"
#include <fstream>
#include <iterator>

namespace jwx::cache {
	Cache::Cache(std::filesystem::path path) {
		local_path = path;
		std::ifstream file(local_path, std::ios::binary);
		file.unsetf(std::ios::skipws);

		file.seekg(0, std::ios::end);
		std::streampos fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		size = fileSize;

		if (fileSize <= JWX_CACHE_MAX_FILE_SIZE) {
			data = std::make_shared<std::vector<uint8_t>>();

			data->reserve(fileSize);

			data->insert(data->end(),
				std::istream_iterator<uint8_t>(file),
				std::istream_iterator<uint8_t>());

		}

		file_time = std::filesystem::last_write_time(local_path);
	}

	const size_t Cache::Size() const {
		return size;
	}

	const std::shared_ptr<const std::vector<uint8_t>> Cache::Data() const {
		return data;
	}

	std::filesystem::file_time_type Cache::Time() const {
		return file_time;
	}

	std::filesystem::path Cache::Path() const {
		return local_path;
	}

	bool Cache::UpToDate() const {
		if (!std::filesystem::exists(local_path) || !std::filesystem::is_regular_file(local_path)) {
			return false;
		}

		auto new_time = std::filesystem::last_write_time(local_path);

		return new_time != file_time;
	}

	std::shared_ptr<Cache> Cache::Generate(std::filesystem::path path) {
		if(!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path)) {
			return nullptr;
		}

		return std::shared_ptr<Cache>(new Cache(path));
	}

	void Cache::UpdateAccessTime() {
		last_access_time = std::chrono::high_resolution_clock::now();
	}

	const std::chrono::high_resolution_clock::time_point Cache::LastAccessTime() const {
		return last_access_time;
	}
}