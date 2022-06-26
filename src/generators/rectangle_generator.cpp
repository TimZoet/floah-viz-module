#include "floah-viz/generators/rectangle_generator.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/enum_classes.h"
#include "sol/mesh/indexed_mesh.h"
#include "sol/mesh/mesh_description.h"
#include "sol/mesh/mesh_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "floah-viz/vertex.h"

namespace floah
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    RectangleGenerator::RectangleGenerator() = default;

    RectangleGenerator::~RectangleGenerator() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Generate.
    ////////////////////////////////////////////////////////////////

    sol::IMesh& RectangleGenerator::generate(Params& params)
    {
        const auto hMargin   = static_cast<float>(margin.get(static_cast<int32_t>(upper.x - lower.x)));
        const auto vMargin   = static_cast<float>(margin.get(static_cast<int32_t>(upper.y - lower.y)));
        const auto hUvMargin = hMargin / (upper.x - lower.x);
        const auto vUvMargin = vMargin / (upper.y - lower.y);

        std::array<Vertex, 8> vertices;

        // Outer quad vertices.
        vertices[0].position = math::float4(lower.x, lower.y, 0, 0);
        vertices[0].color    = color;
        vertices[0].uv       = math::float2(0, 0);
        vertices[1].position = math::float4(lower.x, upper.y, 0, 0);
        vertices[1].color    = color;
        vertices[1].uv       = math::float2(0, 1);
        vertices[2].position = math::float4(upper.x, upper.y, 0, 0);
        vertices[2].color    = color;
        vertices[2].uv       = math::float2(1, 1);
        vertices[3].position = math::float4(upper.x, lower.y, 0, 0);
        vertices[3].color    = color;
        vertices[3].uv       = math::float2(1, 0);

        // Inner quad vertices.
        vertices[4].position = math::float4(lower.x + hMargin, lower.y + vMargin, 0, 0);
        vertices[4].color    = color;
        vertices[4].uv       = math::float2(hUvMargin, vUvMargin);
        vertices[5].position = math::float4(lower.x + hMargin, upper.y - vMargin, 0, 0);
        vertices[5].color    = color;
        vertices[5].uv       = math::float2(hUvMargin, 1 - vUvMargin);
        vertices[6].position = math::float4(upper.x - hMargin, upper.y - vMargin, 0, 0);
        vertices[6].color    = color;
        vertices[6].uv       = math::float2(1 - hUvMargin, 1 - vUvMargin);
        vertices[7].position = math::float4(upper.x - hMargin, lower.y + vMargin, 0, 0);
        vertices[7].color    = color;
        vertices[7].uv       = math::float2(1 - hUvMargin, vUvMargin);

        constexpr std::array<uint32_t, 30> indices = {0, 1, 4, 1, 5, 4, 1, 2, 5, 2, 6, 5, 2, 3, 6,
                                                      3, 7, 6, 0, 4, 7, 0, 7, 3, 4, 5, 7, 5, 6, 7};

        // We generate a description that contains all data, even if e.g. fill is disabled. Makes updating a lot easier.
        auto desc = params.meshManager.createMeshDescription();
        desc->addVertexBuffer(sizeof(Vertex), static_cast<uint32_t>(vertices.size()));
        desc->setVertexData(0, 0, vertices.size(), vertices.data());
        desc->addIndexBuffer(sizeof(uint32_t), static_cast<uint32_t>(indices.size()));
        desc->setIndexData(0, indices.size(), indices.data());

        // Create mesh and set indices based on fillMode.
        auto& mesh = params.meshManager.createIndexedMesh(std::move(desc));
        if (fillMode == FillMode::Outline) { mesh.setIndexCount(24); }
        else if (fillMode == FillMode::Fill)
        {
            mesh.setFirstIndex(24);
            mesh.setIndexCount(6);
        }

        return mesh;
    }
}  // namespace floah
