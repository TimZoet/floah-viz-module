#include "floah-viz/generators/text_generator.h"

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "unicode/unistr.h"

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

    TextGenerator::TextGenerator() = default;

    TextGenerator::~TextGenerator() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Generate.
    ////////////////////////////////////////////////////////////////

    sol::IMesh& TextGenerator::generate(Params& params)
    {
        const icu::UnicodeString ustr(text.c_str());
        std::vector<Vertex>      vertices;
        std::vector<uint32_t>    indices;
        math::float2             offset = position;

        for (int32_t i = 0; i < ustr.countChar32(); i++)
        {
            // Get current character metrics.
            const auto& character = params.fontMap.getCharacter(static_cast<uint32_t>(ustr.char32At(i)));

            // Base position and size.
            math::float2 p = offset;
            p.x += static_cast<float>(character.bearing.x);
            p.y += static_cast<float>(character.size.y) - static_cast<float>(character.bearing.y);
            p.y += static_cast<float>(params.fontMap.getFontAscender() - character.size.y);
            const auto s = math::float2(character.size);

            // Quad vertices.
            Vertex v;
            v.position = math::float4(p.x, p.y, 0.0f, 0.0f);
            v.color    = math::float4(1.0f);
            v.uv       = character.uv0;
            vertices.emplace_back(v);
            v.position = math::float4(p.x + s.x, p.y, 0.0f, 0.0f);
            v.color    = math::float4(1.0f);
            v.uv       = math::float2(character.uv1.x, character.uv0.y);
            vertices.emplace_back(v);
            v.position = math::float4(p.x + s.x, p.y + s.y, 0.0f, 0.0f);
            v.color    = math::float4(1.0f);
            v.uv       = character.uv1;
            vertices.emplace_back(v);
            v.position = math::float4(p.x, p.y + s.y, 0.0f, 0.0f);
            v.color    = math::float4(1.0f);
            v.uv       = math::float2(character.uv0.x, character.uv1.y);
            vertices.emplace_back(v);

            // Two tris.
            indices.emplace_back(i * 4 + 0);
            indices.emplace_back(i * 4 + 1);
            indices.emplace_back(i * 4 + 2);
            indices.emplace_back(i * 4 + 0);
            indices.emplace_back(i * 4 + 2);
            indices.emplace_back(i * 4 + 3);

            offset.x += static_cast<float>(character.advance >> 6);
        }

        // Create mesh.
        auto desc = params.meshManager.createMeshDescription();
        desc->addVertexBuffer(sizeof(Vertex), static_cast<uint32_t>(vertices.size()));
        desc->setVertexData(0, 0, vertices.size(), vertices.data());
        desc->addIndexBuffer(sizeof(uint32_t), static_cast<uint32_t>(indices.size()));
        desc->setIndexData(0, indices.size(), indices.data());
        auto& mesh = params.meshManager.createIndexedMesh(std::move(desc));

        return mesh;
    }
}  // namespace floah
