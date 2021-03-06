#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "floah-common/length.h"
#include "math/include_all.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "floah-viz/generators/generator.h"

namespace floah
{
    class RectangleGenerator : public Generator
    {
    public:
        enum class FillMode
        {
            Outline = 1,
            Fill    = 2,
            Both    = 3
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        RectangleGenerator();

        RectangleGenerator(const RectangleGenerator&) = delete;

        RectangleGenerator(RectangleGenerator&&) noexcept = delete;

        ~RectangleGenerator() noexcept override;

        RectangleGenerator& operator=(const RectangleGenerator&) = delete;

        RectangleGenerator& operator=(RectangleGenerator&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] sol::IMesh& generate(Params& params) override;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Lower bounds of the rectangle.
         */
        math::float2 lower;

        /**
         * \brief Upper bounds of the rectangle.
         */
        math::float2 upper;

        /**
         * \brief FillMode.
         */
        FillMode fillMode = FillMode::Both;

        /**
         * \brief Width of the outline.
         */
        Length margin;

        /**
         * \brief Vertex color.
         */
        math::float4 color = {1, 1, 1, 1};
    };
}  // namespace floah
