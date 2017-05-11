#include "../phpext.h"

namespace php {
	bool string_parser::parse(std::streambuf& buffer, std::size_t n, php::array& data) {
		while(n > 0) {
			char c = buffer.sbumpc();
REPEAT:
			auto r = parse(c, data);
			switch(r) {
			case 1:
				--n;
				break;
			case -1:
				return false;
			case 0:
				goto REPEAT;
			}
		}
		return true;
	}
	bool string_parser::parse(const char* buffer, std::size_t n, php::array& data) {
		int i = 0;
		while(i < n) {
			auto r = parse(buffer[i], data);
			switch(r) {
			case 1:
				break;
			case -1:
				return false;
			case 0:
				continue;
			}
			++i;
		}
		return true;
	}
	int string_parser::parse(const char c, php::array& data) {
		switch(status_) {
		case FIELD_BEFORE:
			if(!std::isspace(c)) {
				status_ = FIELD;
				return 0;
			}
			break;
		case FIELD:
			if(c == sep1_) {
				status_ = SEPERATOR_1;
				return 0;
			}
			field_.push_back(c);
			break;
		case SEPERATOR_1: // =
			status_ = VALUE_BEFORE;
			break;
		case VALUE_BEFORE:
			if(!std::isspace(c)) {
				status_ = VALUE;
				return 0;
			}
			break;
		case VALUE:
			if(c == sep2_) {
				status_ = SEPERATOR_2;
				return 0;
			}
			value_.push_back(c);
			break;
		case SEPERATOR_2:
			php_url_decode(value_.data(), value_.size());
			data[field_] = php::value(value_.data(), value_.size());
			field_.clear();
			value_.clear();
			status_ = FIELD_BEFORE;
			break;
		}
		return 1;
	}
	void string_parser::flush(php::array& data) {
		if(field_.length() > 0) {
			php_url_decode(value_.data(), value_.size());
			data[field_] = php::value(value_.data(), value_.size());
			field_.clear();
			value_.clear();
			status_ = FIELD_BEFORE;
		}
	}

	php::array parse_str(const char sep2, const char* buffer, std::size_t n, const char sep1) {
		string_parser p(sep2, sep1);
		php::array data(std::size_t(0));
		p.parse(buffer, n, data);
		p.flush(data);
		return std::move(data);
	}
	php::array parse_str(const char sep2, std::streambuf& buffer, std::size_t n, const char sep1) {
		string_parser p(sep2, sep1);
		php::array data(std::size_t(0));
		p.parse(buffer, n, data);
		p.flush(data);
		return std::move(data);
	}

}
