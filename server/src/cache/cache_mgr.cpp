#include "cache/cache_mgr.h"
#include <exception>
#include <iostream>

#define JWX_CACHE_MGR_MAX_SIZE 2LL * 1024LL * 1024LL * 1024LL
//#define JWX_CACHE_MGR_MAX_SIZE 0

namespace jwx::cache {
	CacheMgr::CacheMgr() : CacheMgr(std::filesystem::absolute(std::filesystem::path("."))) {

	}

	CacheMgr::CacheMgr(std::filesystem::path content_root_path) : root_content_dir(std::filesystem::canonical(content_root_path)) {
		
	}

	void CacheMgr::RemoveCache(const std::shared_ptr<Cache>& entry) {
		if (entry == nullptr)
			return;
		
		files.erase(entry->Path());
		auto any = time_entries.erase(entry);
		if (any > 0 && entry->Data() != nullptr) {
			current_overall_size -= entry->Size();
		}
	}

	void CacheMgr::SubmitCache(const std::shared_ptr<Cache>& entry) {
		if (entry == nullptr)
			return;

		entry->UpdateAccessTime();
		files[entry->Path()] = entry;
		if (entry->Data() != nullptr) {
			time_entries.insert(entry);
			current_overall_size += entry->Size();
		}
	}

	const std::shared_ptr<Cache> CacheMgr::Request(std::string request) {
		auto noncanonical_path = std::filesystem::path(root_content_dir).concat(request);
		if (!std::filesystem::exists(noncanonical_path)) {
			std::cerr << "[CacheMgr] Path '" << noncanonical_path << "' does not exits." << std::endl;
			return nullptr;
		}
		auto path = std::filesystem::canonical(noncanonical_path);
		auto tmp = path;
		bool found = false;

		lock.lock();
		do {
			if (tmp == root_content_dir) {
				found = true;
				break;
			}
			tmp = tmp.parent_path();
		} while(tmp.has_parent_path());

		if (!found) {
			std::cerr << "[CacheMgr] Path '" << path << "' is outside of the root_content_dir '" << root_content_dir << "'" << std::endl;
			lock.unlock();
			return nullptr;
		}
		
		std::shared_ptr<Cache> p = nullptr;
		try {
			if (files.count(path) > 0) {
				p = files[path];
				if (p != nullptr) {
					RemoveCache(p);
					if (p->UpToDate()) {
						SubmitCache(p);

						lock.unlock();
						return p;
					}
				}
			}
			p = Cache::Generate(path);
			if (p != nullptr) {
				SubmitCache(p);
			}
		} catch(std::runtime_error e) {
			std::cerr << "[CacheMgr] Error '" << e.what() << "'" << std::endl;
		}
		lock.unlock();

		return p;
	}

	void CacheMgr::Cleanup() {
		if (current_overall_size <= JWX_CACHE_MGR_MAX_SIZE)
			return;

		lock.lock();
		size_t cleaned = 0;
		size_t cleaned_size = 0;
		while (current_overall_size > JWX_CACHE_MGR_MAX_SIZE && time_entries.size() > 0) {
			auto it = *time_entries.begin();
			RemoveCache(it.target);
		}
		lock.unlock();
	}
}