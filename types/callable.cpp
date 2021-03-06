#include "../phpext.h"

namespace php {
	value callable::__invoke(zval* cb, int argc, zval argv[], bool silent) {
		value rv;
		if(Z_OBJCE_P(cb) == class_entry<class_closure>::entry()) {
			// 内置的 closure 对象，简化调用过程以提高效率
			class_closure* c = class_wrapper<class_closure>::from_obj(Z_OBJ_P(cb));
			parameters params(argc, argv);
			rv = c->fn_(params);
		}else if(FAILURE == call_user_function_ex(EG(function_table), nullptr, cb, (zval*)&rv, argc, argv, 1, nullptr) && !silent) {
			zend_error_noreturn(E_USER_ERROR, "failed to invoke callalbe '%s'", zval_get_string(cb));
		}
		return std::move(rv);
	}
}
