#include "ShaderDefine.h"

namespace aurora {
	const std::string* ShaderDefineCollection::get(const query_string& name) const {
		auto itr = _values.find(name);
		return itr == _values.end() ? nullptr : &itr->second;
	}

	const std::string* ShaderDefineGetterStack::get(const query_string& name) const {
		auto i = _stack.size();
		while (i--) {
			if (_stack[i]) {
				if (auto rst = _stack[i]->get(name); rst) return rst;
			}
		}
		return nullptr;
	}
}