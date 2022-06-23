#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <concepts>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>


namespace floah
{
    /**
     * \brief Hash an uint32_t. (Thomas Wang, Jan 1997)
     * \param s Value.
     * \return Hash.
     */
    [[nodiscard]] constexpr uint32_t hash(const uint32_t& s) noexcept
    {
        const auto a = s ^ 61 ^ s >> 16;
        const auto b = a * 9;
        const auto c = b ^ b >> 4;
        const auto d = c * 0x27d4eb2d;
        const auto e = d ^ d >> 15;
        return e;
    }

    /**
     * \brief Hash a static string.
     * \tparam N String length.
     * \param str String.
     * \return Hash.
     */
    template<size_t N>
    [[nodiscard]] consteval uint32_t hash(const char (&str)[N]) noexcept
    {
        auto value = std::numeric_limits<uint32_t>::max();

        // Hash characters in multiples of 4.
        for (size_t i = 0; i < N / 4; i++)
        {
            const auto c0 = static_cast<uint32_t>(*(str + i * 4 + 0)) << 24;
            const auto c1 = static_cast<uint32_t>(*(str + i * 4 + 1)) << 16;
            const auto c2 = static_cast<uint32_t>(*(str + i * 4 + 2)) << 8;
            const auto c3 = static_cast<uint32_t>(*(str + i * 4 + 3)) << 0;
            value         = value ^ hash(c0 | c1 | c2 | c3);
        }

        // Hash 0-3 remaining characters.
        const auto remainder = N % 4;
        if constexpr (remainder > 0)
        {
            const auto c0 = static_cast<uint32_t>(*(str + N - remainder)) << 24;
            const auto c1 = remainder > 1 ? static_cast<uint32_t>(*(str + N - remainder + 1)) << 16 : 0;
            const auto c2 = remainder > 2 ? static_cast<uint32_t>(*(str + N - remainder + 2)) << 8 : 0;
            value         = value ^ hash(c0 | c1 | c2);
        }

        return value;
    }

    /**
     * \brief Hash a static string.
     * \param str String.
     * \return Hash.
     */
    consteval uint32_t hash(const char* str)
    {
        size_t N = 0;
        while (str[N] != '\0') N++;

        auto value = std::numeric_limits<uint32_t>::max();

        // Hash characters in multiples of 4.
        for (size_t i = 0; i < N / 4; i++)
        {
            const auto c0 = static_cast<uint32_t>(*(str + i * 4 + 0)) << 24;
            const auto c1 = static_cast<uint32_t>(*(str + i * 4 + 1)) << 16;
            const auto c2 = static_cast<uint32_t>(*(str + i * 4 + 2)) << 8;
            const auto c3 = static_cast<uint32_t>(*(str + i * 4 + 3)) << 0;
            value         = value ^ hash(c0 | c1 | c2 | c3);
        }

        // Hash 0-3 remaining characters.
        if (const auto remainder = N % 4; remainder > 0)
        {
            const auto c0 = static_cast<uint32_t>(*(str + N - remainder)) << 24;
            const auto c1 = remainder > 1 ? static_cast<uint32_t>(*(str + N - remainder + 1)) << 16 : 0;
            const auto c2 = remainder > 2 ? static_cast<uint32_t>(*(str + N - remainder + 2)) << 8 : 0;
            value         = value ^ hash(c0 | c1 | c2);
        }

        return value;
    }

    /**
     * \brief Hash a type name.
     * \tparam T Type.
     * \return Hash.
     */
    template<typename T>
    [[nodiscard]] constexpr uint32_t hashParameter() noexcept
    {
#if defined _MSC_VER
        return hash(__FUNCSIG__);  // NOLINT(clang-diagnostic-language-extension-token)
#elif defined __clang__ || (defined __GNUC__)
        return hash(__PRETTY_FUNCTION__);  // NOLINT(clang-diagnostic-language-extension-token)
#endif
    }

    class Stylesheet
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct BaseMap
        {
            BaseMap() = default;

            BaseMap(const BaseMap&) = default;

            BaseMap(BaseMap&&) = default;

            virtual ~BaseMap() noexcept = default;

            BaseMap& operator=(const BaseMap&) = default;

            BaseMap& operator=(BaseMap&&) = default;

            [[nodiscard]] virtual std::unique_ptr<BaseMap> clone() const = 0;
        };

        using BaseMapPtr = std::unique_ptr<BaseMap>;

        template<typename T>
        struct Map : BaseMap
        {
            std::unordered_map<std::string, T> map;

            Map() = default;

            Map(const Map&) = default;

            Map(Map&&) = default;

            ~Map() noexcept override = default;

            Map& operator=(const Map&) = default;

            Map& operator=(Map&&) = default;

            [[nodiscard]] BaseMapPtr clone() const override { return std::make_unique<Map<T>>(*this); }
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Stylesheet();

        Stylesheet(const Stylesheet&);

        Stylesheet(Stylesheet&&) noexcept;

        ~Stylesheet() noexcept;

        Stylesheet& operator=(const Stylesheet&);

        Stylesheet& operator=(Stylesheet&&) noexcept;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the parent stylesheet.
         * \return Parent stylesheet or nullptr.
         */
        [[nodiscard]] Stylesheet* getParent() noexcept;

        /**
         * \brief Get the parent stylesheet.
         * \return Parent stylesheet or nullptr.
         */
        [[nodiscard]] const Stylesheet* getParent() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Set the parent stylesheet from which values will be retrieved if this stylesheet does not have it.
         * \param stylesheet Parent or nullptr.
         */
        void setParent(Stylesheet* stylesheet) noexcept;

        ////////////////////////////////////////////////////////////////
        // Access.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Set a value.
         * \tparam T Value type.
         * \param name Value name.
         * \param value Value.
         */
        template<std::copyable T>
        void set(const std::string& name, T&& value)
        {
            constexpr auto key = hashParameter<T>();

            auto it = maps.find(key);
            if (it == maps.end()) it = maps.try_emplace(key, std::make_unique<Map<T>>()).first;

            auto& map = static_cast<Map<T>&>(*it->second).map;
            map.insert_or_assign(name, std::forward<T>(value));
        }

        /**
         * \brief Retrieve a value.
         * \tparam T Value type.
         * \param name Value name.
         * \return Value or empty if it does not exist.
         */
        template<std::copyable T>
        [[nodiscard]] std::optional<T> get(const std::string& name) const
        {
            constexpr auto key = hashParameter<T>();

            auto it = maps.find(key);
            if (it == maps.end())
            {
                if (parent) return parent->get<T>(name);
                return {};
            }

            const auto& map = static_cast<Map<T>&>(*it->second).map;

            auto it2 = map.find(name);
            if (it2 == map.end())
            {
                if (parent) return parent->get<T>(name);
                return {};
            }

            return it2->second;
        }

        /**
         * \brief Retrieve a value.
         * \tparam T Value type.
         * \param name Value name.
         * \param defaultValue Value to return if a value with the given name does not exist.
         * \return Value.
         */
        template<std::copyable T>
        [[nodiscard]] T get(const std::string& name, T&& defaultValue) const
        {
            const auto val = get<T>(name);
            if (val) return *val;
            return std::forward<T>(defaultValue);
        }

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        std::unordered_map<uint32_t, BaseMapPtr> maps;

        Stylesheet* parent = nullptr;
    };
}  // namespace floah
