#if !defined(NA_PIPELINE_HPP)
#define NA_PIPELINE_HPP

#include "./Renderer.hpp"
#include "./Assets/ShaderAsset.hpp"

namespace Na {
	using PipelineShaderInfos = std::initializer_list<vk::PipelineShaderStageCreateInfo>;

	enum class ShaderAttributeType : u8 {
		None  = (u8)vk::Format::eUndefined,
		Float = (u8)vk::Format::eR32Sfloat,
		Vec2  = (u8)vk::Format::eR32G32Sfloat,
		Vec3  = (u8)vk::Format::eR32G32B32Sfloat,
		Vec4  = (u8)vk::Format::eR32G32B32A32Sfloat
	};
	inline u32 SizeOf(ShaderAttributeType type)
	{
		switch (type)
		{
		case ShaderAttributeType::Float:  return sizeof(float);
		case ShaderAttributeType::Vec2:   return sizeof(float) * 2;
		case ShaderAttributeType::Vec3:   return sizeof(float) * 3;
		case ShaderAttributeType::Vec4:   return sizeof(float) * 4;
		}
		return 0;
	}

	struct ShaderAttribute {
		u8 location;
		ShaderAttributeType type;
	};

	enum class AttributeInputRate : u8 {
		Vertex   = (u8)vk::VertexInputRate::eVertex,
		Instance = (u8)vk::VertexInputRate::eInstance
	};

	struct ShaderAttributeBinding {
		u8 binding;
		AttributeInputRate input_rate;
		std::initializer_list<ShaderAttribute> attributes;
	};

	using ShaderAttributeLayout = std::initializer_list<ShaderAttributeBinding>;

	enum class ShaderUniformType : u8 {
		None          = 0,

		UniformBuffer = (u8)vk::DescriptorType::eUniformBuffer,
		StorageBuffer = (u8)vk::DescriptorType::eStorageBuffer,
		Texture       = (u8)vk::DescriptorType::eCombinedImageSampler,

		UBO           = UniformBuffer,
		SSBO          = StorageBuffer
	};

	struct ShaderUniform {
		u8 binding;
		ShaderUniformType type;
		ShaderStageBits shader_stage;
	};
	using ShaderUniformLayout = std::initializer_list<ShaderUniform>;

	using PushConstantLayout = std::initializer_list<PushConstant>;

	struct PipelineData {
		vk::Pipeline pipeline;

		vk::DescriptorSetLayout descriptor_set_layout;
		vk::PipelineLayout layout;

		vk::DescriptorPool descriptor_pool;
		Na::ArrayVector<vk::DescriptorSet> descriptor_sets;
	};

	class Pipeline {
	public:
		Pipeline(void) = default;
		Pipeline(
			Renderer& renderer,
			const PipelineShaderInfos& handles = {},
			const ShaderAttributeLayout& vertex_buffer_layout = {},
			const ShaderUniformLayout& uniform_data_layout = {},
			const PushConstantLayout& push_constant_layout = {}
		);
		void destroy(void);
		inline ~Pipeline(void) { this->destroy(); }

		Pipeline(const Pipeline& other) = delete;
		Pipeline& operator=(const Pipeline& other) = delete;

		Pipeline(Pipeline&& other);
		Pipeline& operator=(Pipeline&& other);

		[[nodiscard]] inline u64 handle(void) const { return m_Handle; }
		[[nodiscard]] inline operator bool(void) const { return m_Handle != NA_INVALID_HANDLE; }
	private:
		u64 m_Handle = NA_INVALID_HANDLE;
	};
} // namespace Na

#endif // NA_PIPELINE_HPP