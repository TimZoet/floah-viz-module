#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <filesystem>
#include <string>
#include <unordered_map>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "math/include_all.h"
#include "sol/texture/fwd.h"

namespace floah
{
    class FontMap
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Character metrics for rendering.
         */
        struct Character
        {
            math::uint2  size;
            math::int2   bearing;
            math::float2 uv0;
            math::float2 uv1;
            int32_t      advance;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        FontMap();

        /**
         * \brief Construct a new FontMap from a list of characters. Does not yet generate the actual texture.
         * \param fontPath Path to the font file.
         * \param characters List of characters that must be loaded into the map.
         * \param fontSize Size of the loaded font face. If one component is 0, it is derived from the other.
         */
        FontMap(std::filesystem::path fontPath, const char* characters, math::uint2 fontSize);

        /**
         * \brief Construct a new FontMap from a range of character codes. Does not yet generate the actual texture.
         * \param fontPath Path to the font file.
         * \param characterRange Character range that must be loaded into the map.. Note: upper bound is inclusive.
         * \param fontSize Size of the loaded font face. If one component is 0, it is derived from the other.
         */
        FontMap(std::filesystem::path fontPath, std::pair<uint32_t, uint32_t> characterRange, math::uint2 fontSize);

        FontMap(const FontMap&) = delete;

        FontMap(FontMap&&) noexcept;

        ~FontMap() noexcept;

        FontMap& operator=(const FontMap&) = delete;

        FontMap& operator=(FontMap&&) noexcept;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the path to the font file.
         * \return Path.
         */
        [[nodiscard]] const std::filesystem::path& getPath() const noexcept;

        /**
         * \brief Get the list of characters in this map.
         * \return Character list.
         */
        [[nodiscard]] const std::string& getCharacterList() const noexcept;

        /**
         * \brief Get the font size.
         * \return Font size.
         */
        [[nodiscard]] math::uint2 getFontSize() const noexcept;

        /**
         * \brief Get image object.
         * \return Image (or nullptr if image was not generated yet).
         */
        [[nodiscard]] sol::Image2D* getImage() const noexcept;

        /**
         * \brief Get texture object.
         * \return Texture (or nullptr if texture was not generated yet).
         */
        [[nodiscard]] sol::Texture2D* getTexture() const noexcept;

        /**
         * \brief Retrieve the metrics for a character.
         * \param c Character code.
         * \return Character metrics.
         */
        [[nodiscard]] const Character& getCharacter(uint32_t c) const;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Generate the image and texture objects.
         * \param textureManager TextureManager used to create the image and texture objects.
         */
        void generateTexture(sol::TextureManager& textureManager);

        // TODO: Add support for updating FontMap? New characters, different font, font size, etc.

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Path to font file.
         */
        std::filesystem::path path;

        /**
         * \brief List of characters.
         */
        std::string chars;

        /**
         * \brief Font size.
         */
        math::uint2 size;

        /**
         * \brief Image.
         */
        sol::Image2D* image = nullptr;

        /**
         * \brief Texture.
         */
        sol::Texture2D* texture = nullptr;

        // TODO: If we only allowed (a) contiguous range(s) of characters, we wouldn't need this silly map, just a vector.
        // Would sure make constructing text geometry a lot faster.
        /**
         * \brief Map with per-character metrics.
         */
        std::unordered_map<uint32_t, Character> characterMap;
    };
}  // namespace floah
