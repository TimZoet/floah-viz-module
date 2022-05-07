#include "floah-viz/generators/circle_generator.h"

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

    CircleGenerator::CircleGenerator() = default;

    CircleGenerator::~CircleGenerator() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Generate.
    ////////////////////////////////////////////////////////////////

    sol::IMesh& CircleGenerator::generate(Params& params)
    {
        // Circle will consist of an outer rim of quads and an inner triangle fan.
        std::vector<Vertex>   vertices(vertexCount * 3 + 1);
        std::vector<uint32_t> indices(vertexCount * 3 * 3, 0);

        const auto innerRadius = radius - static_cast<float>(margin.get(static_cast<int32_t>(radius)));
        const auto rel         = innerRadius / radius;

        for (uint32_t i = 0; i < vertexCount; i++)
        {
            const float a       = static_cast<float>(i) / static_cast<float>(vertexCount) * math::m_pi * 2;
            const float x       = math::cos(a);
            const float y       = math::sin(a);
            const auto  uv      = math::float2(x, y) * 0.5f + 0.5f;
            const auto  uvInner = uv * rel;

            // Outside of rim.
            vertices[i].position = math::float4(center.x, center.y) + math::float4(x, y, 0, 0) * radius;
            vertices[i].color    = math::float4(1, 1, 1, 1);
            vertices[i].uv       = uv;

            // Inside of rim.
            vertices[vertexCount + i].position =
              math::float4(center.x, center.y) + math::float4(x, y, 0, 0) * innerRadius;
            vertices[vertexCount + i].color = math::float4(1, 1, 1, 1);
            vertices[vertexCount + i].uv    = uvInner;

            // Outside of triangle fan.
            vertices[vertexCount * 2 + i].position =
              math::float4(center.x, center.y) + math::float4(x, y, 0, 0) * innerRadius;
            vertices[vertexCount * 2 + i].color = math::float4(1, 1, 1, 1);
            vertices[vertexCount * 2 + i].uv    = uvInner;
        }

        // Center vertex.
        vertices.back().position = math::float4(center.x, center.y);
        vertices.back().color    = 1;
        vertices.back().uv       = 0.5f;

        for (uint32_t i = 0; i < vertexCount - 1; i++)
        {
            // Quad of rim.
            indices[i * 6 + 0] = i;
            indices[i * 6 + 1] = i + 1;
            indices[i * 6 + 2] = vertexCount + i;
            indices[i * 6 + 3] = i + 1;
            indices[i * 6 + 4] = vertexCount + i;
            indices[i * 6 + 5] = vertexCount + i + 1;

            // Triangle of fan.
            indices[vertexCount * 6 + i * 3 + 0] = vertexCount * 2 + i;
            indices[vertexCount * 6 + i * 3 + 1] = vertexCount * 2 + i + 1;
            indices[vertexCount * 6 + i * 3 + 2] = vertexCount * 3;
        }

        // Last quad and triangle wrap around.
        indices[(vertexCount - 1) * 6 + 0] = vertexCount - 1;
        indices[(vertexCount - 1) * 6 + 1] = 0;
        indices[(vertexCount - 1) * 6 + 2] = vertexCount;
        indices[(vertexCount - 1) * 6 + 3] = vertexCount - 1;
        indices[(vertexCount - 1) * 6 + 4] = vertexCount * 2 - 1;
        indices[(vertexCount - 1) * 6 + 5] = vertexCount;
        indices[vertexCount * 9 - 3]       = vertexCount * 2;
        indices[vertexCount * 9 - 2]       = vertexCount * 3 - 1;
        indices[vertexCount * 9 - 1]       = vertexCount * 3;

        // We generate a description that contains all data, even if e.g. fill is disabled. Makes updating a lot easier.
        auto desc = params.meshManager.createMeshDescription();
        desc->addVertexBuffer(sizeof(Vertex), static_cast<uint32_t>(vertices.size()));
        desc->setVertexData(0, 0, vertices.size(), vertices.data());
        desc->addIndexBuffer(sizeof(uint32_t), static_cast<uint32_t>(indices.size()));
        desc->setIndexData(0, indices.size(), indices.data());

        // Create mesh and set indices based on fillMode.
        auto& mesh = params.meshManager.createIndexedMesh(std::move(desc));
        if (fillMode == FillMode::Outline) { mesh.setIndexCount(vertexCount * 6); }
        else if (fillMode == FillMode::Fill)
        {
            mesh.setFirstIndex(vertexCount * 6);
            mesh.setIndexCount(vertexCount * 3);
        }

        return mesh;
#if 0
        const auto hMargin   = static_cast<float>(margin.get(static_cast<int32_t>(upper.x - lower.x)));
        const auto vMargin   = static_cast<float>(margin.get(static_cast<int32_t>(upper.y - lower.y)));
        const auto hUvMargin = hMargin / (upper.x - lower.x);
        const auto vUvMargin = vMargin / (upper.y - lower.y);

        std::array<Vertex, 8> vertices;

        // Outer quad vertices.
        vertices[0].position = math::float4(lower.x, lower.y, 0, 0);
        vertices[0].color    = math::float4(1, 1, 1, 1);
        vertices[0].uv       = math::float2(0, 0);
        vertices[1].position = math::float4(lower.x, upper.y, 0, 0);
        vertices[1].color    = math::float4(1, 1, 1, 1);
        vertices[1].uv       = math::float2(0, 1);
        vertices[2].position = math::float4(upper.x, upper.y, 0, 0);
        vertices[2].color    = math::float4(1, 1, 1, 1);
        vertices[2].uv       = math::float2(1, 1);
        vertices[3].position = math::float4(upper.x, lower.y, 0, 0);
        vertices[3].color    = math::float4(1, 1, 1, 1);
        vertices[3].uv       = math::float2(1, 0);

        // Inner quad vertices.
        vertices[4].position = math::float4(lower.x + hMargin, lower.y + vMargin, 0, 0);
        vertices[4].color    = math::float4(1, 1, 1, 1);
        vertices[4].uv       = math::float2(hUvMargin, vUvMargin);
        vertices[5].position = math::float4(lower.x + hMargin, upper.y - vMargin, 0, 0);
        vertices[5].color    = math::float4(1, 1, 1, 1);
        vertices[5].uv       = math::float2(hUvMargin, 1 - vUvMargin);
        vertices[6].position = math::float4(upper.x - hMargin, upper.y - vMargin, 0, 0);
        vertices[6].color    = math::float4(1, 1, 1, 1);
        vertices[6].uv       = math::float2(1 - hUvMargin, 1 - vUvMargin);
        vertices[7].position = math::float4(upper.x - hMargin, lower.y + vMargin, 0, 0);
        vertices[7].color    = math::float4(1, 1, 1, 1);
        vertices[7].uv       = math::float2(1 - hUvMargin, vUvMargin);

        constexpr std::array<uint32_t, 30> indices = {0, 1, 4, 1, 5, 4, 1, 2, 5, 2, 6, 5, 2, 3, 6,
                                                      3, 7, 6, 0, 4, 7, 0, 7, 3, 4, 5, 7, 5, 6, 7};

        // We generate a description that contains all data, even if e.g. fill is disabled. Makes updating a lot easier.
        auto desc = meshManager.createMeshDescription();
        desc->addVertexBuffer(sizeof(Vertex), 8);
        desc->setVertexData(0, 0, 8, vertices.data());
        desc->addIndexBuffer(sizeof(uint32_t), 30);
        desc->setIndexData(0, 30, indices.data());

        // Create mesh and set indices based on fillMode.
        auto& mesh = meshManager.createIndexedMesh(std::move(desc));
        if (fillMode == FillMode::Outline) { mesh.setIndexCount(24); }
        else if (fillMode == FillMode::Fill)
        {
            mesh.setFirstIndex(24);
            mesh.setIndexCount(6);
        }

        return mesh;
#endif
    }
}  // namespace floah
