#include "SerializableObject.h"

namespace aurora {
	SerializableObject::Array::Array() {
		ref();
	}

	SerializableObject::Array* SerializableObject::Array::copy() const {
		Array* arr = new Array();
		arr->value.resize(this->value.size());
		uint32_t idx = 0;
		for (auto& e : this->value) arr->value[idx++].set(e, Flag::COPY);
		return arr;
	}

	bool SerializableObject::Array::isContentEqual(Array* data) const {
		auto size = this->value.size();
		if (size == data->value.size()) {
			for (uint32_t i = 0; i < size; ++i) {
				if (!this->value[i].isContentEqual(data->value[i])) return false;
			}

			return true;
		} else {
			return false;
		}
	}


	SerializableObject::Map::Map() {
		ref();
	}

	SerializableObject::Map* SerializableObject::Map::copy() const {
		Map* map = new Map();
		for (auto& itr : this->value) map->value.emplace(SerializableObject(itr.first, Flag::COPY), SerializableObject(itr.second, Flag::COPY));
		return map;
	}

	bool SerializableObject::Map::isContentEqual(Map* data) const {
		int size = this->value.size();
		if (size == data->value.size()) {
			for (auto& itr : this->value) {
				if (auto itr2 = data->value.find(itr.first); itr2 == data->value.end() || !itr.second.isContentEqual(itr2->second)) return false;
			}

			return true;
		} else {
			return false;
		}
	}

	void SerializableObject::Map::unpack(ByteArray& ba, uint32_t size, Flag flag) {
		value.clear();

		SerializableObject k, v;
		for (uint32_t i = 0; i < size; ++i) {
			k.unpack(ba, flag);
			v.unpack(ba, flag);
			value.emplace(k, v);
		}
	}


	SerializableObject::Str::Str(const char* data, size_t size) :
		size(size) {
		ref();
		this->data = new char[size + 1];
		this->data[size] = 0;
		memcpy(this->data, data, size);
	}

	SerializableObject::Str::~Str() {
		delete[] data;
	}


	SerializableObject::SerializableObject() :
		_type(Type::INVALID),
		_flag(Flag::NONE) {
	}

	SerializableObject::SerializableObject(Type value) :
		_type(value),
		_flag(Flag::NONE) {
		switch (_type) {
		case Type::STRING:
		case Type::SHORT_STRING:
		{
			_value[0] = 0;
			_type = Type::SHORT_STRING;

			break;
		}
		case Type::STRING_VIEW:
		{
			auto& sv = _getValue<StrView>();
			sv.data = nullptr;
			sv.size = 0;

			break;
		}
		case Type::ARRAY:
			_getValue<Array*>() = new Array();
			break;
		case Type::MAP:
			_getValue<Map*>() = new Map();
			break;
		case Type::BYTES:
			_getValue<Bytes<false>*>() = new Bytes<false>();
			break;
		case Type::EXT_BYTES:
			_getValue<Bytes<true>*>() = new Bytes<true>();
			break;
		default:
			_getValue<uint64_t>() = 0;
			break;
		}
	}

	SerializableObject::SerializableObject(bool value) :
		_type(Type::BOOL),
		_flag(Flag::NONE) {
		_getValue<bool>() = value;
	}

	SerializableObject::SerializableObject(int8_t value) :
		_type(Type::INT),
		_flag(Flag::NONE) {
		_getValue<int64_t>() = value;
	}

	SerializableObject::SerializableObject(uint8_t value) :
		_type(Type::UINT),
		_flag(Flag::NONE) {
		_getValue<uint64_t>() = value;
	}

	SerializableObject::SerializableObject(int16_t value) :
		_type(Type::INT),
		_flag(Flag::NONE) {
		_getValue<int64_t>() = value;
	}

	SerializableObject::SerializableObject(uint16_t value) :
		_type(Type::UINT),
		_flag(Flag::NONE) {
		_getValue<uint64_t>() = value;
	}

	SerializableObject::SerializableObject(int32_t value) :
		_type(Type::INT),
		_flag(Flag::NONE) {
		_getValue<int64_t>() = value;
	}

	SerializableObject::SerializableObject(uint32_t value) :
		_type(Type::UINT),
		_flag(Flag::NONE) {
		_getValue<uint64_t>() = value;
	}

	SerializableObject::SerializableObject(const int64_t& value) :
		_type(Type::INT),
		_flag(Flag::NONE) {
		_getValue<int64_t>() = value;
	}

	SerializableObject::SerializableObject(const uint64_t& value) :
		_type(Type::UINT),
		_flag(Flag::NONE) {
		_getValue<uint64_t>() = value;
	}

	SerializableObject::SerializableObject(float32_t value) :
		_type(Type::FLOAT32),
		_flag(Flag::NONE) {
		_getValue<float32_t>() = value;
	}

	SerializableObject::SerializableObject(const float64_t& value) :
		_type(Type::FLOAT64),
		_flag(Flag::NONE) {
		_getValue<float64_t>() = value;
	}

	SerializableObject::SerializableObject(const char* value, Flag flag) : SerializableObject(std::string_view(value), flag) {
	}

	SerializableObject::SerializableObject(const std::string& value, Flag flag) : SerializableObject(std::string_view(value), flag) {
	}

	SerializableObject::SerializableObject(const std::string_view& value, Flag flag) {
		if ((flag & (Flag::COPY | Flag::TO_COPY)) == Flag::COPY) {
			if (auto size = value.size(); size < VALUE_SIZE) {
				_writeShortString(value.data(), size);
				_type = Type::SHORT_STRING;
				_flag = Flag::NONE;
			} else {
				_getValue<Str*>() = new Str(value.data(), size);
				_type = Type::STRING;
				_flag = Flag::NONE;
			}
		} else {
			_writeStringView(value.data(), value.size());
			_type = Type::STRING_VIEW;
			_flag = flag & Flag::SV_STORE_MASK;
		}
	}

	SerializableObject::SerializableObject(const uint8_t* value, size_t size, Flag flag) {
		if ((flag & Flag::COPY) == Flag::COPY) {
			_getValue<Bytes<false>*>() = new Bytes<false>(value, size);
			_type = Type::BYTES;
		} else {
			_getValue<Bytes<true>*>() = new Bytes<true>(value, size);
			_type = Type::EXT_BYTES;
		}
	}

	SerializableObject::SerializableObject(ByteArray& ba, Flag flag) : SerializableObject(ba.getSource(), ba.getLength(), flag) {
	}

	SerializableObject::SerializableObject(const SerializableObject& value) : SerializableObject(value, Flag::NONE) {
	}

	SerializableObject::SerializableObject(const SerializableObject& value, Flag flag) :
		_type(value._type),
		_flag(Flag::NONE) {
		switch (_type) {
		case Type::STRING:
			_getValue<Str*>() = (value._getValue<Str*>())->ref<Str>();
			break;
		case Type::STRING_VIEW:
		{
			auto& sv = value._getValue<StrView>();
			if ((flag & Flag::COPY) != Flag::NONE || (value._flag & Flag::TO_COPY) != Flag::NONE) {
				if (sv.size < VALUE_SIZE) {
					_writeShortString(sv.data, sv.size);
					_type = Type::SHORT_STRING;
				} else {
					_getValue<Str*>() = new Str(sv.data, sv.size);
					_type = Type::STRING;
				}
			} else {
				_getValue<StrView>() = sv;
				_flag = flag & Flag::SV_STORE_MASK;
			}

			break;
		}
		case Type::ARRAY:
		{
			auto arr = value._getValue<Array*>();
			_getValue<Array*>() = (flag & Flag::COPY) == Flag::COPY ? arr->copy() : arr->ref<Array>();

			break;
		}
		case Type::MAP:
		{
			auto map = value._getValue<Map*>();
			_getValue<Map*>() = (flag & Flag::COPY) == Flag::COPY ? map->copy() : map->ref<Map>();

			break;
		}
		case Type::BYTES:
		{
			auto bytes = value._getValue<Bytes<false>*>();
			_getValue<Bytes<false>*>() = (flag & Flag::COPY) == Flag::COPY ? bytes->copy() : bytes->ref<Bytes<false>>();

			break;
		}
		case Type::EXT_BYTES:
		{
			auto bytes = value._getValue<Bytes<true>*>();
			_getValue<Bytes<true>*>() = (flag & Flag::COPY) == Flag::COPY ? bytes->copy() : bytes->ref<Bytes<true>>();

			break;
		}
		default:
			memcpy(_value, value._value, VALUE_SIZE);
			break;
		}
	}

	SerializableObject::SerializableObject(SerializableObject&& value) noexcept :
		_type(value._type),
		_flag(value._flag) {
		memcpy(_value, value._value, VALUE_SIZE);
		value._type = Type::INVALID;
		value._flag = Flag::NONE;
	}

	SerializableObject::~SerializableObject() {
		_freeValue();
	}

	bool SerializableObject::isEqual(const SerializableObject& target) const {
		switch (_type) {
		case Type::INVALID:
			return target._type == Type::INVALID;
		case Type::BOOL:
			return target._type == Type::BOOL && _getValue<bool>() == target._getValue<bool>();
		case Type::INT:
		{
			if (target._type == Type::INT) {
				return _getValue<int64_t>() == target._getValue<int64_t>();
			} else if (target._type == Type::UINT) {
				auto v = _getValue<int64_t>();
				return v < 0 ? false : v == target._getValue<uint64_t>();
			} else {
				return false;
			}
		}
		case Type::UINT:
		{
			if (target._type == Type::UINT) {
				return _getValue<uint64_t>() == target._getValue<uint64_t>();
			} else if (target._type == Type::INT) {
				auto v = target._getValue<int64_t>();
				return v < 0 ? false : v == _getValue<uint64_t>();
			} else {
				return false;
			}
		}
		case Type::FLOAT32:
			return target._type == Type::FLOAT32 && _getValue<float32_t>() == target._getValue<float32_t>();
		case Type::FLOAT64:
			return target._type == Type::FLOAT64 && _getValue<float64_t>() == target._getValue<float64_t>();
		case Type::STRING:
		{
			switch (target._type) {
			case Type::STRING:
			{
				auto s1 = _getValue<Str*>();
				auto s2 = target._getValue<Str*>();
				return _isContentEqual(s1->data, s1->size, s2->data, s2->size);
			}
			case Type::SHORT_STRING:
			{
				auto s = _getValue<Str*>();
				return _isContentEqual(s->data, s->size, target._value, strlen((char*)target._value));
			}
			case Type::STRING_VIEW:
			{
				auto s = _getValue<Str*>();
				auto& sv = target._getValue<StrView>();
				return _isContentEqual(s->data, s->size, sv.data, sv.size);
			}
			default:
				return false;
			}
		}
		case Type::SHORT_STRING:
		{
			switch (target._type) {
			case Type::STRING:
			{
				auto s = target._getValue<Str*>();
				return _isContentEqual(s->data, s->size, _value, strlen((char*)_value));
			}
			case Type::SHORT_STRING:
				return _isContentEqual(_value, strlen((char*)_value), target._value, strlen((char*)target._value));
			case Type::STRING_VIEW:
			{
				auto& sv = target._getValue<StrView>();
				return _isContentEqual(_value, strlen((char*)_value), sv.data, sv.size);
			}
			default:
				return false;
			}
		}
		case Type::STRING_VIEW:
		{
			switch (target._type) {
			case Type::STRING:
			{
				auto s = target._getValue<Str*>();
				auto& sv = _getValue<StrView>();
				return _isContentEqual(s->data, s->size, sv.data, sv.size);
			}
			case Type::SHORT_STRING:
			{
				auto sv = _getValue<StrView>();
				return _isContentEqual(sv.data, sv.size, target._value, strlen((char*)target._value));
			}
			case Type::STRING_VIEW:
			{
				auto& sv1 = _getValue<StrView>();
				auto& sv2 = target._getValue<StrView>();
				return _isContentEqual(sv1.data, sv1.size, sv2.data, sv2.size);
			}
			default:
				return false;
			}
		}
		case Type::ARRAY:
			return target._type == Type::ARRAY && _getValue<Array*>() == target._getValue<Array*>();
		case Type::MAP:
			return target._type == Type::MAP && _getValue<Map*>() == target._getValue<Map*>();
		case Type::BYTES:
			return target._type == Type::BYTES && _getValue<Bytes<false>*>() == target._getValue<Bytes<false>*>();
		case Type::EXT_BYTES:
			return target._type == Type::EXT_BYTES && _getValue<Bytes<true>*>()->getValue() == target._getValue<Bytes<true>*>()->getValue() && _getValue<Bytes<true>*>()->getSize() == target._getValue<Bytes<true>*>()->getSize();
		default:
			return false;
		}
	}

	bool SerializableObject::isContentEqual(const SerializableObject& target) const {
		switch (_type) {
		case Type::INVALID:
			return target._type == Type::INVALID;
		case Type::BOOL:
			return target._type == Type::BOOL && _getValue<bool>() == target._getValue<bool>();
		case Type::INT:
			return _isEqual<int64_t>(target);
		case Type::UINT:
			return _isEqual<uint64_t>(target);
		case Type::FLOAT32:
			return _isEqual<float32_t>(target);
		case Type::FLOAT64:
			return _isEqual<float64_t>(target);
		case Type::STRING:
		{
			switch (target._type) {
			case Type::STRING:
			{
				auto s1 = _getValue<Str*>();
				auto s2 = target._getValue<Str*>();
				return _isContentEqual(s1->data, s1->size, s2->data, s2->size);
			}
			case Type::SHORT_STRING:
			{
				auto s = _getValue<Str*>();
				return _isContentEqual(s->data, s->size, target._value, strlen((char*)target._value));
			}
			case Type::STRING_VIEW:
			{
				auto s = _getValue<Str*>();
				auto& sv = target._getValue<StrView>();
				return _isContentEqual(s->data, s->size, sv.data, sv.size);
			}
			default:
				return false;
			}
		}
		case Type::SHORT_STRING:
		{
			switch (target._type) {
			case Type::STRING:
			{
				auto s = target._getValue<Str*>();
				return _isContentEqual(s->data, s->size, _value, strlen((char*)_value));
			}
			case Type::SHORT_STRING:
				return _isContentEqual(_value, strlen((char*)_value), target._value, strlen((char*)target._value));
			case Type::STRING_VIEW:
			{
				auto& sv = target._getValue<StrView>();
				return _isContentEqual(_value, strlen((char*)_value), sv.data, sv.size);
			}
			default:
				return false;
			}
		}
		case Type::STRING_VIEW:
		{
			switch (target._type) {
			case Type::STRING:
			{
				auto s = target._getValue<Str*>();
				auto& sv = _getValue<StrView>();
				return _isContentEqual(s->data, s->size, sv.data, sv.size);
			}
			case Type::SHORT_STRING:
			{
				auto sv = _getValue<StrView>();
				return _isContentEqual(sv.data, sv.size, target._value, strlen((char*)target._value));
			}
			case Type::STRING_VIEW:
			{
				auto& sv1 = _getValue<StrView>();
				auto& sv2 = target._getValue<StrView>();
				return _isContentEqual(sv1.data, sv1.size, sv2.data, sv2.size);
			}
			default:
				return false;
			}
		}
		case Type::ARRAY:
			return target._type == Type::ARRAY && _getValue<Array*>()->isContentEqual(target._getValue<Array*>());
		case Type::MAP:
			return target._type == Type::MAP && _getValue<Map*>()->isContentEqual(target._getValue<Map*>());
		case Type::BYTES:
		{
			if (target._type == Type::BYTES) {
				return _getValue<Bytes<false>*>()->isContentEqual(*target._getValue<Bytes<false>*>());
			} else if (target._type == Type::EXT_BYTES) {
				return _getValue<Bytes<false>*>()->isContentEqual(*target._getValue<Bytes<true>*>());
			} else {
				return false;
			}
		}
		case Type::EXT_BYTES:
		{
			if (target._type == Type::BYTES) {
				return _getValue<Bytes<true>*>()->isContentEqual(*target._getValue<Bytes<false>*>());
			} else if (target._type == Type::EXT_BYTES) {
				return _getValue<Bytes<true>*>()->isContentEqual(*target._getValue<Bytes<true>*>());
			} else {
				return false;
			}
		}
		default:
			return false;
		}
	}

	size_t SerializableObject::getSize() const {
		switch (_type) {
		case Type::ARRAY:
			return _getValue<Array*>()->value.size();
		case Type::MAP:
			return _getValue<Map*>()->value.size();
		case Type::BYTES:
			return _getValue<Bytes<false>*>()->getSize();
		case Type::EXT_BYTES:
			return _getValue<Bytes<true>*>()->getSize();
		case Type::STRING:
			return _getValue<Str*>()->size;
		case Type::SHORT_STRING:
			return strlen((char*)_value);
		case Type::STRING_VIEW:
			return _getValue<StrView>().size;
		default:
			return 0;
		}
	}

	void SerializableObject::clear() {
		switch (_type) {
		case Type::ARRAY:
			_getValue<Array*>()->value.clear();
			break;
		case Type::MAP:
			_getValue<Map*>()->value.clear();
			break;
		case Type::BYTES:
			_getValue<Bytes<false>*>()->clear();
			break;
		case Type::EXT_BYTES:
			_getValue<Bytes<true>*>()->clear();
			break;
		case Type::STRING:
		{
			_getValue<Str*>()->unref();
			_value[0] = 0;
			_type = Type::SHORT_STRING;

			break;
		}
		case Type::SHORT_STRING:
			_value[0] = 0;
			break;
		case Type::STRING_VIEW:
		{
			auto& sv = _getValue<StrView>();
			sv.data = nullptr;
			sv.size = 0;

			break;
		}
		default:
			_getValue<uint64_t>() = 0;
			break;
		}
	}

	bool SerializableObject::setInvalid() {
		if (_type == Type::INVALID) {
			return false;
		} else {
			_freeValue();
			_type = Type::INVALID;

			return true;
		}
	}

	bool SerializableObject::setArray() {
		if (_type == Type::ARRAY) {
			return false;
		} else {
			_freeValue();
			_getValue<Array*>() = new Array();
			_type = Type::ARRAY;

			return true;
		}
	}

	bool SerializableObject::setMap() {
		if (_type == Type::MAP) {
			return false;
		} else {
			_freeValue();
			_getValue<Map*>() = new Map();
			_type = Type::MAP;

			return true;
		}
	}

	bool SerializableObject::toBool(bool defaultValue) const {
		switch (_type) {
		case Type::BOOL:
			return _getValue<bool>();
		case Type::INT:
			return _getValue<int64_t>();
		case Type::UINT:
			return _getValue<uint64_t>();
		case Type::FLOAT32:
			return _getValue<float32_t>() != 0.0f;
		case Type::FLOAT64:
			return _getValue<float64_t>() != 0.0;
		case Type::INVALID:
			return false;
		default:
			return defaultValue;
		}
	}

	std::string SerializableObject::toString(const std::string& defaultValue) const {
		switch (_type) {
		case Type::BOOL:
			return _getValue<bool>() ? "true" : "false";
		case Type::INT:
			return String::toString<int64_t>(_getValue<int64_t>());
		case Type::UINT:
			return String::toString<uint64_t>(_getValue<uint64_t>());
		case Type::FLOAT32:
			return String::toString<float32_t>(_getValue<float32_t>());
		case Type::FLOAT64:
			return String::toString<float64_t>(_getValue<float64_t>());
		case Type::STRING:
		{
			auto s = _getValue<Str*>();
			return std::string(s->data, s->size);
		}
		case Type::SHORT_STRING:
			return (char*)_value;
		case Type::STRING_VIEW:
		{
			auto& sv = _getValue<StrView>();
			return std::string(sv.data, sv.size);
		}
		case Type::INVALID:
			return "";
		default:
			return defaultValue;
		}
	}

	std::string_view SerializableObject::toStringView() const {
		switch (_type) {
		case Type::STRING:
		{
			auto s = _getValue<Str*>();
			return std::string_view(s->data, s->size);
		}
		case Type::SHORT_STRING:
			return std::string_view((char*)_value);
		case Type::STRING_VIEW:
		{
			auto& sv = _getValue<StrView>();
			return std::string_view(sv.data, sv.size);
		}
		default:
			return std::string_view();
		}
	}

	const uint8_t* SerializableObject::toBytes() const {
		if (_type == Type::BYTES) {
			return _getValue<Bytes<false>*>()->getValue();
		} else if (_type == Type::EXT_BYTES) {
			return _getValue<Bytes<true>*>()->getValue();
		} else {
			return nullptr;
		}
	}

	void SerializableObject::_toJson(std::string& json) const {
		switch (_type) {
		case Type::BOOL:
		case Type::INT:
		case Type::UINT:
		case Type::FLOAT32:
		case Type::FLOAT64:
			json += toString();
			break;
		case Type::STRING:
		case Type::SHORT_STRING:
		case Type::STRING_VIEW:
		case Type::INVALID:
		{
			json += '"';
			json += toStringView();
			json += '"';

			break;
		}
		case Type::ARRAY:
		{
			json += '[';

			auto& arr = _getValue<Array*>()->value;
			bool first = true;
			for (auto& i : arr) {
				if (first) {
					first = false;
				} else {
					json += ',';
				}
				i._toJson(json);
			}

			json += ']';

			break;
		}
		case Type::MAP:
		{
			json += '{';

			auto& map = _getValue<Map*>()->value;
			bool first = true;
			for (auto& itr : map) {
				if (first) {
					first = false;
				} else {
					json += ',';
				}
				itr.first._toJson(json);
				json += ':';
				itr.second._toJson(json);
			}

			json += '}';

			break;
		}
		case Type::BYTES:
		{
			auto bin = _getValue<Bytes<false>*>();
			json += "\"" + String::toString(bin->getValue(), bin->getSize()) + "\"";

			break;
		}
		case Type::EXT_BYTES:
		{
			auto bin = _getValue<Bytes<true>*>();
			json += "\"" + String::toString(bin->getValue(), bin->getSize()) + "\"";

			break;
		}
		default:
			break;
		}
	}

	void SerializableObject::set(const std::string_view& value, Flag flag) {
		_flag = Flag::NONE;
		switch (_type) {
		case Type::STRING:
		{
			_getValue<Str*>()->unref();

			if ((flag & Flag::COPY) == Flag::COPY) {
				if (auto size = value.size(); size < VALUE_SIZE) {
					_writeShortString(value.data(), size);
					_type = Type::SHORT_STRING;
				} else {
					_getValue<Str*>() = new Str(value.data(), size);
				}
			} else {
				_writeStringView(value.data(), value.size());
				_type = Type::STRING_VIEW;
				_flag = flag & Flag::SV_STORE_MASK;
			}

			break;
		}
		case Type::SHORT_STRING:
		{
			if ((flag & Flag::COPY) == Flag::COPY) {
				if (auto size = value.size(); size < VALUE_SIZE) {
					_writeShortString(value.data(), size);
				} else {
					_getValue<Str*>() = new Str(value.data(), size);
					_type = Type::STRING;
				}
			} else {
				_writeStringView(value.data(), value.size());
				_type = Type::STRING_VIEW;
				_flag = flag & Flag::SV_STORE_MASK;
			}

			break;
		}
		case Type::STRING_VIEW:
		{
			if ((flag & Flag::COPY) == Flag::COPY) {
				if (auto size = value.size(); size < VALUE_SIZE) {
					_writeShortString(value.data(), size);
					_type = Type::SHORT_STRING;
				} else {
					_getValue<Str*>() = new Str(value.data(), size);
					_type = Type::STRING;
				}
			} else {
				_writeStringView(value.data(), value.size());
				_flag = flag & Flag::SV_STORE_MASK;
			}

			break;
		}
		default:
		{
			_freeValue();

			if ((flag & Flag::COPY) == Flag::COPY) {
				if (auto size = value.size(); size < VALUE_SIZE) {
					_writeShortString(value.data(), size);
					_type = Type::SHORT_STRING;
				} else {
					_getValue<Str*>() = new Str(value.data(), size);
					_type = Type::STRING;
				}
			} else {
				_writeStringView(value.data(), value.size());
				_type = Type::STRING_VIEW;
				_flag = flag & Flag::SV_STORE_MASK;
			}

			break;
		}
		}
	}

	void SerializableObject::set(const uint8_t* value, size_t size, Flag flag) {
		_flag = Flag::NONE;
		if ((flag & Flag::COPY) == Flag::COPY) {
			setBytes<false>();
			_getValue<Bytes<false>*>()->setValue(value, size);
		} else {
			setBytes<true>();
			_getValue<Bytes<true>*>()->setValue(value, size);
		}
	}

	void SerializableObject::set(const SerializableObject& value, Flag flag) {
		_flag = Flag::NONE;
		switch (value._type) {
		case Type::STRING:
		{
			_freeValue();
			_type = Type::STRING;

			_getValue<Str*>() = (value._getValue<Str*>())->ref<Str>();

			break;
		}
		case Type::SHORT_STRING:
		{
			_freeValue();
			_type = Type::SHORT_STRING;

			memcpy(_value, value._value, strlen((const char*)value._value));

			break;
		}
		case Type::STRING_VIEW:
		{
			auto& sv = value._getValue<StrView>();
			set(std::string_view(sv.data, sv.size), flag | ((value._flag & Flag::TO_COPY) != Flag::NONE ? Flag::COPY : Flag::NONE));

			break;
		}
		case Type::ARRAY:
		{
			_freeValue();
			_type = Type::ARRAY;

			auto arr = value._getValue<Array*>();
			_getValue<Array*>() = (flag & Flag::COPY) == Flag::COPY ? arr->copy() : arr->ref<Array>();

			break;
		}
		case Type::MAP:
		{
			_freeValue();
			_type = Type::MAP;

			auto map = value._getValue<Map*>();
			_getValue<Map*>() = (flag & Flag::COPY) == Flag::COPY ? map->copy() : map->ref<Map>();

			break;
		}
		case Type::BYTES:
		{
			_freeValue();
			_type = Type::BYTES;

			auto bytes = value._getValue<Bytes<false>*>();
			_getValue<Bytes<false>*>() = (flag & Flag::COPY) == Flag::COPY ? bytes->copy() : bytes->ref<Bytes<false>>();

			break;
		}
		case Type::EXT_BYTES:
		{
			_freeValue();
			_type = Type::EXT_BYTES;

			auto bytes = value._getValue<Bytes<true>*>();
			_getValue<Bytes<true>*>() = (flag & Flag::COPY) == Flag::COPY ? bytes->copy() : bytes->ref<Bytes<true>>();

			break;
		}
		default:
		{
			memcpy(_value, value._value, VALUE_SIZE);
			_type = value._type;

			break;
		}
		}
	}

	SerializableObject& SerializableObject::at(size_t index) {
		Array* arr = _getArray();
		if (index >= arr->value.size()) arr->value.resize(index + 1);
		return arr->value[index];
	}

	SerializableObject SerializableObject::tryAt(size_t index) const {
		if (_type == Type::ARRAY) {
			Array* arr = _getValue<Array*>();
			return index < arr->value.size() ? arr->value[index] : std::move(SerializableObject());
		} else {
			return std::move(SerializableObject());
		}
	}

	SerializableObject* SerializableObject::tryAtPtr(size_t index) const {
		if (_type == Type::ARRAY) {
			Array* arr = _getValue<Array*>();
			return index < arr->value.size() ? &arr->value[index] : nullptr;
		} else {
			return nullptr;
		}
	}

	SerializableObject& SerializableObject::push() {
		return _getArray()->value.emplace_back();
	}

	SerializableObject& SerializableObject::push(const SerializableObject& value) {
		return _getArray()->value.emplace_back(value);
	}

	SerializableObject SerializableObject::removeAt(size_t index) {
		if (_type == Type::ARRAY) {
			Array* arr = _getValue<Array*>();

			if (index < arr->value.size()) {
				SerializableObject v = arr->value[index];
				arr->value.erase(arr->value.begin() + index);
				return std::move(v);
			} else {
				return std::move(SerializableObject());
			}
		} else {
			return std::move(SerializableObject());
		}
	}

	void SerializableObject::insertAt(size_t index, const SerializableObject& value) {
		Array* arr = _getArray();
		if (index < arr->value.size()) {
			arr->value.emplace(arr->value.begin() + index, value);
		} else {
			at(index) = value;
		}
	}

	SerializableObject& SerializableObject::get(const SerializableObject& key) {
		Map* map = _getMap();

		return map->value.emplace(std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple()).first->second;
	}

	SerializableObject SerializableObject::tryGet(const SerializableObject& key) const {
		if (_type == Type::MAP) {
			Map* map = _getValue<Map*>();

			auto itr = map->value.find(key);
			return itr == map->value.end() ? std::move(SerializableObject()) : itr->second;
		} else {
			return std::move(SerializableObject());
		}
	}

	SerializableObject* SerializableObject::tryGetPtr(const SerializableObject& key) const {
		if (_type == Type::MAP) {
			Map* map = _getValue<Map*>();

			auto itr = map->value.find(key);
			return itr == map->value.end() ? nullptr : &itr->second;
		} else {
			return nullptr;
		}
	}

	SerializableObject& SerializableObject::insert(const SerializableObject& key, const SerializableObject& value) {
		Map* map = _getMap();

		return map->value.insert_or_assign(key, value).first->second;
	}

	bool SerializableObject::has(const SerializableObject& key) const {
		if (_type == Type::MAP) {
			Map* map = _getValue<Map*>();

			return map->value.find(key) != map->value.end();
		} else {
			return false;
		}
	}

	SerializableObject SerializableObject::remove(const SerializableObject& key) {
		if (_type == Type::MAP) {
			Map* map = _getValue<Map*>();

			auto itr = map->value.find(key);
			if (itr == map->value.end()) {
				return std::move(SerializableObject());
			} else {
				SerializableObject v = itr->second;
				map->value.erase(itr);
				return std::move(v);
			}
		} else {
			return std::move(SerializableObject());
		}
	}

	void SerializableObject::_packUInt(ByteArray& ba, uint64_t val, uint8_t typeBegin) const {
		if (val <= BitUInt<8>::MAX) {
			ba.write<ba_vt::UI8>(typeBegin);
			ba.write<ba_vt::UI8>(val);
		} else if (val <= BitUInt<16>::MAX) {
			ba.write<ba_vt::UI8>(typeBegin + 1);
			ba.write<ba_vt::UI16>(val);
		} else if (val <= BitUInt<24>::MAX) {
			ba.write<ba_vt::UI8>(typeBegin + 2);
			ba.write<ba_vt::UIX>(val, 3);
		} else if (val <= BitUInt<32>::MAX) {
			ba.write<ba_vt::UI8>(typeBegin + 3);
			ba.write<ba_vt::UI32>(val);
		} else if (val <= BitUInt<40>::MAX) {
			ba.write<ba_vt::UI8>(typeBegin + 4);
			ba.write<ba_vt::UIX>(val, 5);
		} else if (val <= BitUInt<48>::MAX) {
			ba.write<ba_vt::UI8>(typeBegin + 5);
			ba.write<ba_vt::UIX>(val, 6);
		} else if (val <= BitUInt<56>::MAX) {
			ba.write<ba_vt::UI8>(typeBegin + 6);
			ba.write<ba_vt::UIX>(val, 7);
		} else {
			ba.write<ba_vt::UI8>(typeBegin + 7);
			ba.write<ba_vt::UI64>(val);
		}
	}

	void SerializableObject::unpack(ByteArray& ba, Flag flag) {
		if (ba.getBytesAvailable() > 0) {
			InternalType type = (InternalType)ba.read<ba_vt::UI8>();
			switch (type) {
			case InternalType::UNVALID:
				setInvalid();
				break;
			case InternalType::END:
				setInvalid();
				break;
			case InternalType::FLT_0:
				set(0.0f);
				break;
			case InternalType::FLT_0_5:
				set(0.5f);
				break;
			case InternalType::FLT_1:
				set(1.0f);
				break;
			case InternalType::FLOAT:
				set(ba.read<float32_t>());
				break;
			case InternalType::DBL_0:
				set(0.0);
				break;
			case InternalType::DBL_0_5:
				set(0.5);
				break;
			case InternalType::DBL_1:
				set(1.0);
				break;
			case InternalType::DOUBLE:
				set(ba.read<ba_vt::F64>());
				break;
			case InternalType::STRING_EMPTY:
				set("");
				break;
			case InternalType::STRING:
				set(ba.read<std::string_view>(), flag);
				break;
			case InternalType::BOOL_TRUE:
				set(true);
				break;
			case InternalType::BOOL_FALSE:
				set(false);
				break;
			case InternalType::N_INT_1:
				set(-1);
				break;
			case InternalType::N_INT_8BITS:
			case InternalType::N_INT_16BITS:
			case InternalType::N_INT_24BITS:
			case InternalType::N_INT_32BITS:
			case InternalType::N_INT_40BITS:
			case InternalType::N_INT_48BITS:
			case InternalType::N_INT_56BITS:
			case InternalType::N_INT_64BITS:
			{
				set(-(int64_t)ba.read<ba_vt::UIX>((uint8_t)type - (uint8_t)InternalType::N_INT_8BITS + 1));
				break;
			}
			case InternalType::P_INT_0:
				set(0);
				break;
			case InternalType::P_INT_1:
				set(1);
				break;
			case InternalType::P_INT_8BITS:
			case InternalType::P_INT_16BITS:
			case InternalType::P_INT_24BITS:
			case InternalType::P_INT_32BITS:
			case InternalType::P_INT_40BITS:
			case InternalType::P_INT_48BITS:
			case InternalType::P_INT_56BITS:
			case InternalType::P_INT_64BITS:
			{
				set(ba.read<ba_vt::UIX>((uint8_t)type - (uint8_t)InternalType::P_INT_8BITS + 1));
				break;
			}
			case InternalType::ARRAY_0:
			case InternalType::ARRAY_8BITS:
			case InternalType::ARRAY_16BITS:
			case InternalType::ARRAY_24BITS:
			case InternalType::ARRAY_32BITS:
			case InternalType::ARRAY_40BITS:
			case InternalType::ARRAY_48BITS:
			case InternalType::ARRAY_56BITS:
			case InternalType::ARRAY_64BITS:
			{
				size_t size = ba.read<ba_vt::UIX>((uint8_t)type - (uint8_t)InternalType::ARRAY_0);
				auto& arr = _getArray()->value;
				arr.resize(size);
				for (size_t i = 0; i < size; ++i) arr[i].unpack(ba, flag);

				break;
			}
			case InternalType::ARRAY_END:
			{
				auto& arr = _getArray()->value;
				arr.clear();
				while (ba.getBytesAvailable()) {
					if ((InternalType)*(ba.getSource() + ba.getPosition()) == InternalType::END) {
						ba.skip(1);
						break;
					} else {
						arr.emplace_back().unpack(ba, flag);
					}
				}

				break;
			}
			case InternalType::MAP_0:
			case InternalType::MAP_8BITS:
			case InternalType::MAP_16BITS:
			case InternalType::MAP_24BITS:
			case InternalType::MAP_32BITS:
			case InternalType::MAP_40BITS:
			case InternalType::MAP_48BITS:
			case InternalType::MAP_56BITS:
			case InternalType::MAP_64BITS:
			{
				_getMap()->unpack(ba, ba.read<ba_vt::UIX>((uint8_t)type - (uint8_t)InternalType::MAP_0), flag);
				break;
			}
			case InternalType::MAP_END:
			{
				auto& map = _getMap()->value;
				map.clear();
				SerializableObject k, v;
				while (ba.getBytesAvailable()) {
					if ((InternalType) * (ba.getSource() + ba.getPosition()) == InternalType::END) {
						ba.skip(1);
						break;
					} else {
						k.unpack(ba, flag);
						v.unpack(ba, flag);
						map.emplace(k, v);
					}
				}

				break;
			}
			case InternalType::BYTES_0:
			case InternalType::BYTES_8BITS:
			case InternalType::BYTES_16BITS:
			case InternalType::BYTES_24BITS:
			case InternalType::BYTES_32BITS:
			case InternalType::BYTES_40BITS:
			case InternalType::BYTES_48BITS:
			case InternalType::BYTES_56BITS:
			case InternalType::BYTES_64BITS:
			{
				_unpackBytes(ba, ba.read<ba_vt::UIX>((uint8_t)type - (uint8_t)InternalType::BYTES_0), flag);
				break;
			}
			default:
				break;
			}
		} else {
			setInvalid();
		}
	}

	void SerializableObject::_unpackBytes(ByteArray& ba, size_t size, Flag flag) {
		if ((flag & Flag::COPY) == Flag::COPY) {
			_getBytes<false>()->setValue(ba.getSource() + ba.getPosition(), size);
		} else {
			_getBytes<true>()->setValue(ba.getSource() + ba.getPosition(), size);
		}
		ba.setPosition(ba.getPosition() + size);
	}

	void SerializableObject::_freeValue() {
		switch (_type) {
		case Type::STRING:
		{
			_getValue<Str*>()->unref();
			_type = Type::INVALID;

			break;
		}
		case Type::ARRAY:
		{
			_getValue<Array*>()->unref();
			_type = Type::INVALID;

			break;
		}
		case Type::MAP:
		{
			_getValue<Map*>()->unref();
			_type = Type::INVALID;

			break;
		}
		case Type::BYTES:
		{
			_getValue<Bytes<false>*>()->unref();
			_type = Type::INVALID;

			break;
		}
		case Type::EXT_BYTES:
		{
			_getValue<Bytes<true>*>()->unref();
			_type = Type::INVALID;

			break;
		}
		default:
			_type = Type::INVALID;
			break;
		}

		_flag = Flag::NONE;
	}
}