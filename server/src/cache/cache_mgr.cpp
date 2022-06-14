#include "cache/cache_mgr.h"

namespace jwx::cache {
	CacheMgr::CacheMgr() : CacheMgr(std::filesystem::absolute(std::filesystem::path("."))) {

	}

	CacheMgr::CacheMgr(std::filesystem::path content_root_path) {

	}
}