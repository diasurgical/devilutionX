#include "utils/string_or_view.hpp"

namespace devilution {
class InfoStringView : public StringOrView {

public:
	InfoStringView &operator=(StringOrView &&other) noexcept
	{
		string_view::size_type end = 0;
		for (unsigned i = 0; i < 5; ++i) {
			if (end < other.str().size()) {
				if (other.str().find('\n', end + 1) == std::string::npos)
					end = other.str().size();
				else
					end = other.str().find('\n', end + 1);
			} else
				break;
		}
		if (end < other.str().size()) {
			LogWarn("PrintInfo unable to render everything - not enough lines");
			other = other.view_.substr(0, end);
		}

		if (owned_) {
			if (other.owned_) {
				str_ = std::move(other.str_);
			} else {
				str_.~basic_string();
				owned_ = false;
				new (&view_) string_view(other.view_);
			}
		} else {
			if (other.owned_) {
				view_.~string_view();
				owned_ = true;
				new (&str_) std::string(std::move(other.str_));
			} else {
				view_ = other.view_;
			}
		}
		return *this;
	}
	void clear()
	{
		if (!this->empty()) {
			str() = "";
			view_ = "";
			owned_ = false;
		}
	}
};
} // namespace devilution
