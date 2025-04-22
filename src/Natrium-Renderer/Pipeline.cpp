#include "Pch.hpp"
#include "Natrium-Renderer/Pipeline.hpp"

#include "Natrium-Renderer/VkContext.hpp"

#include "./PipelineStates.hpp"

namespace Na {
	static vk::Format shaderAttributeTypeToVk(ShaderAttributeType type)
	{
		switch (type)
		{
		case ShaderAttributeType::Vec2: return vk::Format::eR32G32Sfloat;
		case ShaderAttributeType::Vec3: return vk::Format::eR32G32B32Sfloat;
		case ShaderAttributeType::Vec4: return vk::Format::eR32G32B32A32Sfloat;
		}
		return vk::Format::eUndefined;
	}

	static vk::DescriptorType shaderUniformTypeToVk(ShaderUniformType type)
	{
		switch (type)
		{
		case ShaderUniformType::UniformBuffer:  return vk::DescriptorType::eUniformBuffer;
		case ShaderUniformType::TextureSampler: return vk::DescriptorType::eCombinedImageSampler;
		}
		return {};
	}

	static std::tuple<
		Na::ArrayVector<vk::VertexInputBindingDescription>,
		Na::ArrayVector<vk::VertexInputAttributeDescription>
	>
		GetVertexInputInfo(
			const ShaderAttributeLayout& vertex_buffer_layout
		)
	{
		if (!vertex_buffer_layout.size())
			return { {}, {} };

		Na::ArrayVector<vk::VertexInputBindingDescription> binding_descriptions(vertex_buffer_layout.size());

		u64 attribute_count = 0;
		for (const auto& binding : vertex_buffer_layout)
			attribute_count += binding.size();

		Na::ArrayVector<vk::VertexInputAttributeDescription> attribute_descriptions(attribute_count);

		for (u32 i = 0; const auto& binding : vertex_buffer_layout)
		{
			u32 offset = 0;
			for (u64 j = 0; const auto& attribute : binding)
			{
				attribute_descriptions[i + j * binding_descriptions.size()].binding = i;
				attribute_descriptions[i + j * binding_descriptions.size()].location = attribute.location;
				attribute_descriptions[i + j * binding_descriptions.size()].format = shaderAttributeTypeToVk(attribute.type);
				attribute_descriptions[i + j * binding_descriptions.size()].offset = offset;

				offset += SizeOf(attribute.type);
				j++;
			}

			binding_descriptions[i].binding = i;
			binding_descriptions[i].stride = offset;
			binding_descriptions[i].inputRate = vk::VertexInputRate::eVertex;

			i++;
		}

		return { binding_descriptions, attribute_descriptions };
	}

	static vk::DescriptorSetLayout createDescriptorSetLayout(const ShaderUniformLayout& descriptor_layout)
	{
		Na::ArrayVector<vk::DescriptorSetLayoutBinding> bindings(descriptor_layout.size());
		for (size_t i = 0; const auto & binding : descriptor_layout)
		{
			bindings[i].binding            = binding.binding;
			bindings[i].descriptorType     = shaderUniformTypeToVk(binding.type);
			bindings[i].stageFlags         = (vk::ShaderStageFlagBits)binding.shader_stage;
			bindings[i].descriptorCount    = 1;
			bindings[i].pImmutableSamplers = nullptr;

			i++;
		}

		vk::DescriptorSetLayoutCreateInfo create_info;
		create_info.bindingCount = (u32)bindings.size();
		create_info.pBindings = bindings.ptr();

		return VkContext::GetLogicalDevice().createDescriptorSetLayout(create_info);
	}

	static vk::DescriptorPool createDescriptorPool(const ShaderUniformLayout& descriptor_layout, u32 max_frames_in_flight)
	{
		Na::ArrayVector<vk::DescriptorPoolSize> pool_sizes(descriptor_layout.size());
		for (size_t i = 0; const ShaderUniform& uniform : descriptor_layout)
		{
			pool_sizes[i].descriptorCount = max_frames_in_flight;
			pool_sizes[i].type = shaderUniformTypeToVk(uniform.type);
			i++;
		}

		vk::DescriptorPoolCreateInfo create_info;
		create_info.poolSizeCount = (u32)pool_sizes.size();
		create_info.pPoolSizes = pool_sizes.ptr();
		create_info.maxSets = max_frames_in_flight;

		return VkContext::GetLogicalDevice().createDescriptorPool(create_info);
	}

	static Na::ArrayVector<vk::DescriptorSet> createDescriptorSets(u32 count, vk::DescriptorSetLayout* layouts, vk::DescriptorPool pool)
	{
		vk::DescriptorSetAllocateInfo alloc_info;
		alloc_info.descriptorPool = pool;
		alloc_info.descriptorSetCount = count;
		alloc_info.pSetLayouts = layouts;

		Na::ArrayVector<vk::DescriptorSet> descriptor_sets(count);
		(void)VkContext::GetLogicalDevice().allocateDescriptorSets(&alloc_info, descriptor_sets.ptr());
		return descriptor_sets;
	}

	Pipeline::Pipeline(
		Renderer& renderer,
		const PipelineShaderInfos& shader_infos,
		const ShaderAttributeLayout& vertex_buffer_layout,
		const ShaderUniformLayout& uniform_data_layout,
		const PushConstantLayout& push_constant_layout
	)
	: m_Handle(VkContext::GetPipelinePool().emplace())
	{
		PipelineData& pipeline = VkContext::GetPipelinePool()[m_Handle];

		Na::ArrayVector<vk::DynamicState> dynamic_states = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor
		};

		auto [binding_descriptions, attribute_descriptions] = GetVertexInputInfo(vertex_buffer_layout);

		vk::PipelineVertexInputStateCreateInfo vertex_input_info;
		vertex_input_info.vertexAttributeDescriptionCount = (u32)attribute_descriptions.size();
		vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.ptr();
		vertex_input_info.vertexBindingDescriptionCount = (u32)binding_descriptions.size();
		vertex_input_info.pVertexBindingDescriptions = binding_descriptions.ptr();

		auto dynamic_state_info = dynamicStateInfo(dynamic_states);
		auto viewport_info = viewportInfo();
		auto input_assembly_info = inputAssemblyInfo();
		auto rasterization_info = rasterizationInfo();
		auto multisample_info = multisampleInfo(false);
		auto color_blend_attachment = colorBlendAttachment(false);
		auto color_blend_info = colorBlendInfo(color_blend_attachment);
		auto depth_stencil_info = depthStencilInfo();

		if (uniform_data_layout.size())
			pipeline.descriptor_set_layout = createDescriptorSetLayout(uniform_data_layout);

		Na::ArrayVector<vk::PushConstantRange> push_constant_ranges(push_constant_layout.size());

		for (u64 i = 0; const auto& push_constant : push_constant_layout)
		{
			push_constant_ranges[i].stageFlags = (vk::ShaderStageFlagBits)push_constant.shader_stage;
			push_constant_ranges[i].offset = push_constant.offset;
			push_constant_ranges[i].size = push_constant.size;
			i++;
		}

		pipeline.layout = VkContext::GetLogicalDevice().createPipelineLayout(
			vk::PipelineLayoutCreateInfo(
				{}, // flags
				(bool)pipeline.descriptor_set_layout, uniform_data_layout.size() ? &pipeline.descriptor_set_layout : nullptr,
				(u32)push_constant_ranges.size(), push_constant_ranges.ptr()
			)
		);

		vk::GraphicsPipelineCreateInfo create_info;

		create_info.stageCount = (u32)shader_infos.size();
		create_info.pStages = shader_infos.begin();

		create_info.renderPass = renderer.m_RenderPass;
		create_info.layout = pipeline.layout;

		create_info.pDynamicState = &dynamic_state_info;
		create_info.pViewportState = &viewport_info;
		create_info.pInputAssemblyState = &input_assembly_info;
		create_info.pVertexInputState = &vertex_input_info;
		create_info.pRasterizationState = &rasterization_info;
		create_info.pMultisampleState = &multisample_info;
		create_info.pColorBlendState = &color_blend_info;
		create_info.pDepthStencilState = &depth_stencil_info;

		pipeline.pipeline = VkContext::GetLogicalDevice().createGraphicsPipeline(nullptr, create_info).value;

		if (uniform_data_layout.size())
		{
			Na::ArrayVector<vk::DescriptorSetLayout> descriptor_layouts(renderer.m_Config.max_frames_in_flight);
			for (vk::DescriptorSetLayout& descriptor_layout : descriptor_layouts)
				descriptor_layout = pipeline.descriptor_set_layout;

			pipeline.descriptor_pool = createDescriptorPool(uniform_data_layout, renderer.m_Config.max_frames_in_flight);
			pipeline.descriptor_sets = createDescriptorSets(
				renderer.m_Config.max_frames_in_flight,
				descriptor_layouts.ptr(),
				pipeline.descriptor_pool
			);
		}
	}

	void Pipeline::destroy(void)
	{
		if (m_Handle == NA_INVALID_HANDLE)
			return;

		vk::Device logical_device = VkContext::GetLogicalDevice();
		PipelineData& pipeline = VkContext::GetPipelinePool()[m_Handle];

		if (pipeline.descriptor_pool)
			logical_device.destroyDescriptorPool(pipeline.descriptor_pool);

		if (pipeline.pipeline)
			logical_device.destroyPipeline(pipeline.pipeline);

		if (pipeline.descriptor_set_layout)
			logical_device.destroyDescriptorSetLayout(pipeline.descriptor_set_layout);

		if (pipeline.layout)
			logical_device.destroyPipelineLayout(pipeline.layout);

		pipeline.descriptor_sets.~ArrayVector();
		memset(&pipeline, 0, sizeof(PipelineData));

		m_Handle = NA_INVALID_HANDLE;
	}
} // namespace Na
