#if !defined(SHADER_ATTRIBUTE_LAYOUT_HPP)
#define SHADER_ATTRIBUTE_LAYOUT_HPP

#include "Natrium-Core/Core.hpp"

namespace Na {
	enum class ShaderAttributeType : u8 {
		None = 0,
		Vec2, Vec3, Vec4,
		Mat4x4
	};
	inline u32 SizeOf(ShaderAttributeType type)
	{
		switch (type)
		{
		case ShaderAttributeType::Vec2:   return sizeof(float) * 2;
		case ShaderAttributeType::Vec3:   return sizeof(float) * 3;
		case ShaderAttributeType::Vec4:   return sizeof(float) * 4;
		case ShaderAttributeType::Mat4x4: return sizeof(float) * 4 * 4;
		}
		return 0;
	}

	struct ShaderAttribute {
		u8 location;
		ShaderAttributeType type;
	};

	class ShaderAttributeLayout {
	public:
		using const_iterator = Na::ArrayList<Na::ArrayList<ShaderAttribute>>::const_iterator;
		using const_reverse_iterator = Na::ArrayList<Na::ArrayList<ShaderAttribute>>::const_reverse_iterator;
	public:
		inline void reserve(u8 binding_count) { m_Bindings.reallocate(binding_count); }

		void set_binding(u8 binding, const std::initializer_list<ShaderAttribute>& attributes);
		void set_attribute(u8 binding, u8 location, ShaderAttributeType type);

		[[nodiscard]] inline u64 size(void) const { return m_Bindings.size(); }
		[[nodiscard]] inline u64 capacity(void) const { return m_Bindings.capacity(); }
		[[nodiscard]] inline u64 free_space(void) const { return m_Bindings.free_space(); }
		[[nodiscard]] inline bool empty(void) const { return m_Bindings.empty(); }

		[[nodiscard]] inline const_iterator begin(void)  const { return m_Bindings.cbegin(); }
		[[nodiscard]] inline const_iterator cbegin(void) const { return m_Bindings.cbegin(); }
		[[nodiscard]] inline const_iterator end(void)    const { return m_Bindings.cend(); }
		[[nodiscard]] inline const_iterator cend(void)   const { return m_Bindings.cend(); }

		[[nodiscard]] inline const_reverse_iterator rbegin(void)  const { return m_Bindings.crbegin(); }
		[[nodiscard]] inline const_reverse_iterator crbegin(void) const { return m_Bindings.crbegin(); }
		[[nodiscard]] inline const_reverse_iterator rend(void)    const { return m_Bindings.crend(); }
		[[nodiscard]] inline const_reverse_iterator crend(void)   const { return m_Bindings.crend(); }
	private:
		Na::ArrayList<Na::ArrayList<ShaderAttribute>> m_Bindings;
	};
} // namespace Na

#endif // SHADER_ATTRIBUTE_LAYOUT_HPP