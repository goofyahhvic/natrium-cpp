#if !defined(NA_PIPELINE_HPP)
#define NA_PIPELINE_HPP

#include "./Renderer.hpp"
#include "./Shader.hpp"

namespace Na {
	using PipelineShaderInfos = std::initializer_list<vk::PipelineShaderStageCreateInfo>;

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

	using ShaderAttributeBinding = std::initializer_list<ShaderAttribute>;
	using ShaderAttributeLayout = std::initializer_list<ShaderAttributeBinding>;

	enum class ShaderUniformType : u8 {
		None = 0,
		UniformBuffer, TextureSampler
	};

	struct ShaderUniform {
		u8 binding;
		ShaderUniformType type;
		ShaderStageBits shader_stage;
	};
	using ShaderUniformLayout = std::initializer_list<ShaderUniform>;

	struct PipelineData {
		vk::Pipeline pipeline;

		vk::DescriptorSetLayout descriptor_set_layout;
		vk::PipelineLayout layout;

		vk::DescriptorPool descriptor_pool;
		Na::ArrayVector<vk::DescriptorSet> descriptor_sets;
	};

	class Pipeline {
	public:
		Pipeline(
			Renderer& renderer,
			const PipelineShaderInfos& handles = {},
			const ShaderAttributeLayout& vertex_buffer_layout = {},
			const ShaderUniformLayout& uniform_data_layout = {}
		);
		void destroy(void);

		[[nodiscard]] inline u64 handle(void) const { return m_Handle; }
		[[nodiscard]] inline operator bool(void) const { return m_Handle != NA_INVALID_HANDLE; }
	private:
		u64 m_Handle = NA_INVALID_HANDLE;
	};
} // namespace Na

#endif // NA_PIPELINE_HPP