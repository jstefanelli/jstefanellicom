#pragma once
#include "cached_file.h"
#include <unordered_map>
#include <mutex>
#include <memory>
#include <string>
#include <set>

namespace jwx::cache {

	struct CacheTimeEntry {
		std::shared_ptr<Cache> target;

		CacheTimeEntry(const std::shared_ptr<Cache>& target) : target(target) {

		} 

		bool operator==(const CacheTimeEntry& other) const {
			return target == other.target;
		}

		auto operator<=>(const CacheTimeEntry& other) const {
			if (target == other.target) {
				return target <=> other.target;
			} else {
				return target->LastAccessTime() <=> other.target->LastAccessTime();
			}
		}

	};

	class CacheMgr {
	protected:
		std::mutex lock;
		std::unordered_map<std::string, std::shared_ptr<Cache>> files;
		std::set<CacheTimeEntry> time_entries;
		std::filesystem::path root_content_dir;
		size_t current_overall_size;

		void RemoveCache(const std::shared_ptr<Cache>& entry);
		void SubmitCache(const std::shared_ptr<Cache>& entry);
	public:
		CacheMgr();
		explicit CacheMgr(std::filesystem::path root_content_dir);

		const std::shared_ptr<Cache> Request(std::string request);
		void Cleanup();
	};	
}

template<>
struct std::hash<jwx::cache::CacheTimeEntry> {
	std::size_t operator()(jwx::cache::CacheTimeEntry const& item) const noexcept {
		return std::hash<std::shared_ptr<jwx::cache::Cache>>()(item.target);
	}
};