#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "math/include_all.h"
#include "sol/scenegraph/node.h"

namespace floah
{
    class ITransformNode
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ITransformNode() = default;

        ITransformNode(const ITransformNode&) = default;

        ITransformNode(ITransformNode&&) noexcept = default;

        virtual ~ITransformNode() noexcept = default;

        ITransformNode& operator=(const ITransformNode&) = default;

        ITransformNode& operator=(ITransformNode&&) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] virtual sol::Node& getAsNode() = 0;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        virtual void setOffset(math::float3 offset) = 0;

        virtual void setZ(float z) = 0;
    };
}  // namespace floah
