#include "floah-viz/generators/rectangle_generator.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol/mesh/flat_mesh.h"
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

    sol::FlatMesh& RectangleGenerator::generate(sol::MeshManager& meshManager)
    {
        auto desc = meshManager.createMeshDescription();
        desc->addVertexBuffer(sizeof(Vertex), 6);

        // 0/3  -----  5
        //     |\    |
        //     | \   |
        //     |  \  |
        //     |   \ |
        //     |    \|
        //   1  -----  2/4

        Vertex v;
        v.position = math::float4(lower.x, lower.y, 0, 0);
        v.color    = math::float4(1, 1, 1, 1);
        v.uv       = math::float2(0, 0);
        desc->setVertexData(0, 0, &v);
        desc->setVertexData(0, 3, &v);
        v.position = math::float4(lower.x, upper.y, 0, 0);
        v.color    = math::float4(1, 1, 1, 1);
        v.uv       = math::float2(0, 1);
        desc->setVertexData(0, 1, &v);
        v.position = math::float4(upper.x, upper.y, 0, 0);
        v.color    = math::float4(1, 1, 1, 1);
        v.uv       = math::float2(1, 1);
        desc->setVertexData(0, 2, &v);
        desc->setVertexData(0, 4, &v);
        v.position = math::float4(upper.x, lower.y, 0, 0);
        v.color    = math::float4(1, 1, 1, 1);
        v.uv       = math::float2(1, 0);
        desc->setVertexData(0, 5, &v);

        return meshManager.createFlatMesh(std::move(desc));
    }
}  // namespace floah
