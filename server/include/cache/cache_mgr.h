#pragma once
#include "cached_file.h"
#include <unordered_map>
#include <mutex>

namespace jwx::cache {

	class CacheMgr {
	protected:
		std::mutex lock;
		std::unordered_map<std::filesystem::path, Cache> files;
		std::filesystem::path root_content_dir;

	public:
		CacheMgr();
		explicit CacheMgr(std::filesystem::path root_content_dir);

		const Cache& Request(std::string request);
	};
}