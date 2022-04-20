#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "floah-layout/properties/length.h"
#include "math/include_all.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "floah-viz/generators/generator.h"

namespace floah
{
    class CircleGenerator : public Generator
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

        CircleGenerator();

        CircleGenerator(const CircleGenerator&) = delete;

        CircleGenerator(CircleGenerator&&) noexcept = delete;

        ~CircleGenerator() noexcept override;

        CircleGenerator& operator=(const CircleGenerator&) = delete;

        CircleGenerator& operator=(CircleGenerator&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] sol::IMesh& generate(sol::MeshManager& meshManager) override;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Center of the circle.
         */
        math::float2 center;

        /**
         * \brief Radius of the circle.
         */
        float radius;

        /**
         * \brief Number of vertices.
         */
        uint32_t vertexCount = 8;

        /**
         * \brief FillMode.
         */
        FillMode fillMode = FillMode::Both;

        /**
         * \brief Width of the outline.
         */
        Length margin;
    };
}  // namespace floah
