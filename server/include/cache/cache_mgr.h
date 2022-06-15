#pragma once
#include "cached_file.h"
#include <unordered_map>
#include <mutex>
#include <memory>
#include <string>

namespace jwx::cache {

	class CacheMgr {
	protected:
		std::mutex lock;
		std::unordered_map<std::string, std::shared_ptr<Cache>> files;
		std::filesystem::path root_content_dir;

	public:
		CacheMgr();
		explicit CacheMgr(std::filesystem::path root_content_dir);

		const std::shared_ptr<Cache> Request(std::string request);
	};
}