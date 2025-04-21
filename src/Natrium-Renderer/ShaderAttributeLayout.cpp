#include "Pch.hpp"
#include "Natrium-Renderer/ShaderAttributeLayout.hpp"

namespace Na {
	void ShaderAttributeLayout::set_binding(u8 binding, const std::initializer_list<ShaderAttribute>& attributes)
	{
		if (m_Bindings.capacity() < binding)
			m_Bindings.reallocate(binding);

		m_Bindings[binding] = attributes;
	}

	void ShaderAttributeLayout::set_attribute(u8 binding, u8 location, ShaderAttributeType type)
	{
		if (m_Bindings.capacity() < binding)
			m_Bindings.reallocate(binding);

		if (m_Bindings[binding].capacity() < location)
			m_Bindings[binding].reallocate(location);
		
		m_Bindings[binding][location] = { location, type };
	}

} // namespace Na
