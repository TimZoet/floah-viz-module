#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol/mesh/fwd.h"

namespace floah
{
    class Generator
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Generator() = default;

        Generator(const Generator&) = delete;

        Generator(Generator&&) noexcept = delete;

        virtual ~Generator() noexcept = default;

        Generator& operator=(const Generator&) = delete;

        Generator& operator=(Generator&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] virtual sol::IMesh& generate(sol::MeshManager& meshManager) = 0;
    };
}  // namespace floah
