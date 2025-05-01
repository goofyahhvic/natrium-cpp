#if !defined(NA_RENDERER_INTERNAL_HPP)
#define NA_RENDERER_INTERNAL_HPP

namespace Na::Internal {
	void WriteToDescriptorSet(
		vk::DescriptorSet set,
		u32 binding,
		vk::DescriptorType type,
		u32 count,
		vk::DescriptorBufferInfo* buffer_info = nullptr,
		vk::DescriptorImageInfo* image_info = nullptr,
		vk::BufferView* texel_buffer_view = nullptr
	);
} // namespace Na::Internal

#endif // NA_RENDERER_INTERNAL_HPP