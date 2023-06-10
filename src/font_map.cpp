#include "floah-viz/font_map.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "freetype2/ft2build.h"
#include FT_FREETYPE_H
#include "unicode/unistr.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "floah-common/floah_error.h"
#include "sol/texture/image2d.h"
#include "sol/texture/texture_manager.h"

namespace
{
    /**
     * \brief Calculate the required image size to fit all characters. Tries to fill a square image, starting at
     * 256x256, and repeatedly doubles size until all characters fit.
     * \param face Face.
     * \param chars List of characters.
     * \return Image size (in pixels).
     */
    [[nodiscard]] math::uint2 calculateImageSize(const FT_Face face, const icu::UnicodeString& chars)
    {
        std::vector<math::uint2> sizes;
        for (int32_t i = 0; i < chars.countChar32(); i++)
        {
            // TODO: What to do if loading fails?
            if (FT_Load_Char(face, chars.char32At(i), FT_LOAD_COMPUTE_METRICS)) continue;

            // Skip size 0 chars.
            if (face->glyph->metrics.width == 0 || face->glyph->metrics.height == 0) continue;

            sizes.emplace_back(face->glyph->metrics.width >> 6, face->glyph->metrics.height >> 6);
        }

        // Try to fit all characters into a square. Start with 256x256 and keep expanding until all chars fit.
        math::uint2 imageSize{256, 256};
        bool        fit = false;
        while (!fit)
        {
            fit = true;

            uint32_t x = 0, y = 0, ymax = 0;
            for (const auto& s : sizes)
            {
                // Glyph does not fit on current row anymore. Reset and move to next row.
                if (x + s.x >= imageSize.x)
                {
                    x = 0;
                    y += ymax;
                    ymax = 0;
                }

                // Glyph does not fit in image anymore.
                if (y + s.y >= imageSize.y)
                {
                    // TODO: Limit size by max texture size?
                    imageSize *= static_cast<uint32_t>(2);
                    fit = false;
                    break;
                }

                x += s.x;
                ymax = std::max(ymax, s.y);
            }
        }

        return imageSize;
    }

    /**
     * \brief Fill an image with all rendered characters and store in character map.
     * \param face Face.
     * \param chars List of characters.
     * \param image Image to fill.
     * \param characterMap Character map to store metrics in.
     */
    void fillImage(const FT_Face                                            face,
                   const icu::UnicodeString&                                chars,
                   sol::Image2D&                                            image,
                   std::unordered_map<uint32_t, floah::FontMap::Character>& characterMap)
    {
        image.createStagingBuffer();

        uint32_t x = 0, y = 0, ymax = 0;
        for (int32_t i = 0; i < chars.countChar32(); i++)
        {
            const auto uchar = chars.char32At(i);
            // TODO: What if loading fails?
            if (FT_Load_Char(face, uchar, FT_LOAD_RENDER)) continue;

            // Glyph does not fit on current row anymore. Reset and move to next row.
            if (x + face->glyph->bitmap.width >= image.getWidth())
            {
                x = 0;
                y += ymax;
                ymax = 0;
            }

            // Copy bitmap.
            if (face->glyph->bitmap.width != 0 && face->glyph->bitmap.rows != 0)
                image.setData(
                  face->glyph->bitmap.buffer, {x, y}, {face->glyph->bitmap.width, face->glyph->bitmap.rows}, 0);

            // Store character.
            const auto size    = math::uint2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
            const auto bearing = math::int2(face->glyph->bitmap_left, face->glyph->bitmap_top);
            const auto uv0     = math::float2(static_cast<float>(x) / static_cast<float>(image.getWidth()),
                                          static_cast<float>(y) / static_cast<float>(image.getHeight()));
            const auto uv1     = uv0 + math::float2(static_cast<float>(size.x) / static_cast<float>(image.getWidth()),
                                                static_cast<float>(size.y) / static_cast<float>(image.getHeight()));
            const auto advance = face->glyph->advance.x;
            characterMap.try_emplace(
              static_cast<uint32_t>(uchar),
              floah::FontMap::Character{.size = size, .bearing = bearing, .uv0 = uv0, .uv1 = uv1, .advance = advance});

            // Move right.
            x += face->glyph->bitmap.width;
            // Keep track of highest glyph in current row.
            ymax = std::max(ymax, face->glyph->bitmap.rows);
        }
    }
}  // namespace

namespace floah
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    FontMap::FontMap() = default;

    FontMap::FontMap(std::filesystem::path fontPath, const char* characters, math::uint2 fontSize) :
        path(std::move(fontPath)), chars(characters), size(std::move(fontSize))
    {
    }

    FontMap::FontMap(std::filesystem::path               fontPath,
                     const std::pair<uint32_t, uint32_t> characterRange,
                     math::uint2                         fontSize) :
        path(std::move(fontPath)), size(std::move(fontSize))
    {
        icu::UnicodeString ustr;
        for (uint32_t i = characterRange.first; i <= characterRange.second; i++) ustr.append(static_cast<UChar32>(i));
        ustr.toUTF8String(chars);
    }

    FontMap::FontMap(FontMap&&) noexcept = default;

    FontMap::~FontMap() noexcept
    {
        // TODO: Destroy image and texture.
    }

    FontMap& FontMap::operator=(FontMap&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const std::filesystem::path& FontMap::getPath() const noexcept { return path; }

    const std::string& FontMap::getCharacterList() const noexcept { return chars; }

    math::uint2 FontMap::getFontSize() const noexcept { return size; }

    int32_t FontMap::getFontAscender() const noexcept { return ascender; }

    int32_t FontMap::getFontDescender() const noexcept { return descender; }

    sol::Image2D* FontMap::getImage() const noexcept { return image; }

    sol::Texture2D* FontMap::getTexture() const noexcept { return texture; }

    const FontMap::Character& FontMap::getCharacter(const uint32_t c) const
    {
        const auto it = characterMap.find(c);
        // TODO: Return 'missing' character?
        if (it == characterMap.end()) throw FloahError(std::format("Unknown character {}.", c));

        return it->second;
    }

    ////////////////////////////////////////////////////////////////
    // Generate.
    ////////////////////////////////////////////////////////////////

    void FontMap::generateTexture(sol::TextureManager& textureManager)
    {
        // Texture was already generated.
        if (image) return;

        const icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(chars);

        // Load library.
        FT_Library ft;
        auto       err = FT_Init_FreeType(&ft);
        if (err) throw FloahError(std::format("Failed to initialize FreeType Library. Error code: {}", err));

        // Load face.
        FT_Face face;
        err = FT_New_Face(ft, path.string().c_str(), 0, &face);
        if (err) throw FloahError(std::format("Failed to load font file {}. Error code: {}", path.string(), err));

        FT_Set_Pixel_Sizes(face, size.x, size.y);
        ascender  = face->ascender >> 6;
        descender = face->descender >> 6;

        // Create image and texture object.
        const auto imageSize = calculateImageSize(face, ustr);
        image                = &textureManager.createImage2D(VK_FORMAT_R8_UINT, {imageSize.x, imageSize.y});
        texture              = &textureManager.createTexture2D(*image);

        // Fill image.
        fillImage(face, ustr, *image, characterMap);

        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }
}  // namespace floah
