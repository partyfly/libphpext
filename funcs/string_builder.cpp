#include "../vendor.h"
#include "../phpext.h"

namespace php {
	php::string build_str(php::array& array, const std::string& prefix, const std::string& sep) {
		php::string s;
		smart_str   r = {0};
		if(php_url_encode_hash_ex(
			static_cast<zend_array*>(array), &r,
			prefix.c_str(), prefix.length(),
			prefix.c_str(), prefix.length(),
			nullptr, 0,
			nullptr,
			const_cast<char*>(sep.c_str()),
			PHP_QUERY_RFC3986) == FAILURE) {

			if(r.s) {
				smart_str_free(&r);
			}
			return php::string(std::size_t(0));
		}
		if(!r.s) {
			return php::string(std::size_t(0));
		}
		smart_str_0(&r);
		ZVAL_STR(s, r.s);
		return std::move(s);
	}
}
