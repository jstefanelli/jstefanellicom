#include "cache/cache_mgr.h"
#include <exception>
#include <iostream>

namespace jwx::cache {
	CacheMgr::CacheMgr() : CacheMgr(std::filesystem::absolute(std::filesystem::path("."))) {

	}

	CacheMgr::CacheMgr(std::filesystem::path content_root_path) : root_content_dir(std::filesystem::canonical(content_root_path)) {
		
	}

	const std::shared_ptr<Cache> CacheMgr::Request(std::string request) {
		lock.lock();
		auto path = std::filesystem::canonical(std::filesystem::path(root_content_dir).concat(request));
		auto tmp = path;
		bool found = false;

		do {
			if (tmp == root_content_dir) {
				found = true;
				break;
			}
			tmp = tmp.parent_path();
		} while(tmp.has_parent_path());

		if (!found) {
			std::cerr << "[CacheMgr] Path '" << path << "' is outside of the root_content_dir '" << root_content_dir << "'" << std::endl;
			return nullptr;
		}
		
		std::shared_ptr<Cache> p = nullptr;
		try {
			if (files.count(path) > 0) {
				p = files[path];
				if (p != nullptr && p->UpToDate()) {
					lock.unlock();
					return p;
				}
			}
			p = Cache::Generate(path);
			if (p != nullptr) {
				files[path] = p;
			}
		} catch(std::runtime_error e) {
			std::cerr << "[CacheMgr] Error '" << e.what() << "'" << std::endl;
		} 
		lock.unlock();

		return p;
	}
}