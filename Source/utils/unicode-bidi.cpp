#include <fribidi/fribidi.h>

#include <string>
#include <string_view>

namespace devilution {

std::u32string ConvertLogicalToVisual(std::u32string_view input)
{
    FriBidiChar *logical = new FriBidiChar[input.size()];
    FriBidiChar *visual = new FriBidiChar[input.size()];

    for (size_t i = 0; i < input.size(); i++) {
        logical[i] = input[i];
    }

    FriBidiParType baseDirection = FRIBIDI_PAR_ON;
    fribidi_log2vis(logical, input.size(), &baseDirection, visual, nullptr, nullptr, nullptr);

    std::u32string result(reinterpret_cast<const char32_t *>(visual), input.size());

    delete[] logical;
    delete[] visual;

    return result;
}

} // namespace devilution
