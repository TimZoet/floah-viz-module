#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "math/include_all.h"
#include "sol/material/fwd.h"
#include "sol/scenegraph/node.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "floah-viz/scenegraph/transform_node.h"

namespace floah
{
    class IScenegraphGenerator
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        IScenegraphGenerator() = default;

        IScenegraphGenerator(const IScenegraphGenerator&) = default;

        IScenegraphGenerator(IScenegraphGenerator&&) noexcept = default;

        virtual ~IScenegraphGenerator() noexcept = default;

        IScenegraphGenerator& operator=(const IScenegraphGenerator&) = default;

        IScenegraphGenerator& operator=(IScenegraphGenerator&&) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create root node for a widget.
         * \return Node.
         */
        [[nodiscard]] virtual sol::Node& createWidgetNode() = 0;

        [[nodiscard]] virtual sol::Node& createTextMaterialNode(sol::Node& parent, sol::ForwardMaterialInstance& mtlInstance) = 0;

        [[nodiscard]] virtual ITransformNode& createTransformNode(sol::Node& parent, math::float3 offset) = 0;

        [[nodiscard]] virtual sol::NodePtr createMaterialNode(sol::ForwardMaterialInstance& mtlInstance) = 0;
    };
}  // namespace floah
