#include "floah-viz/stylesheet.h"

namespace floah
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Stylesheet::Stylesheet() = default;

    Stylesheet::Stylesheet(const Stylesheet& other) { *this = other; }

    Stylesheet::Stylesheet(Stylesheet&&) noexcept = default;

    Stylesheet::~Stylesheet() noexcept = default;

    Stylesheet& Stylesheet::operator=(const Stylesheet& other)
    {
        for (const auto& [k, v] : other.maps) maps.try_emplace(k, v->clone());
        return *this;
    }

    Stylesheet& Stylesheet::operator=(Stylesheet&&) noexcept = default;
}  // namespace floah
