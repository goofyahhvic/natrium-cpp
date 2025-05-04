#if !defined(NA_STORAGE_BUFFER_HPP)
#define NA_STORAGE_BUFFER_HPP

#include "Natrium/Graphics/Buffers/DeviceBuffer.hpp"
#include "Natrium/Graphics/Pipeline.hpp"

namespace Na {
	class StorageBuffer {
	public:
		struct Data {
			vk::Buffer buffer;
			vk::DeviceMemory memory;
			void* mapped;
		};
	public:
		StorageBuffer(void) = default;
		StorageBuffer(u64 size, const RendererSettings& renderer_settings);
		void destroy(void);
		inline ~StorageBuffer(void) { this->destroy(); }

		StorageBuffer(const StorageBuffer& other) = delete;
		StorageBuffer& operator=(const StorageBuffer& other) = delete;

		StorageBuffer(StorageBuffer&& other);
		StorageBuffer& operator=(StorageBuffer&& other);

		void bind_to_pipeline(u32 binding, GraphicsPipeline& pipeline) const;

		[[nodiscard]] inline Na::ArrayVector<Data>& datas(void) { return m_Datas; }
		[[nodiscard]] inline const Na::ArrayVector<Data>& datas(void) const { return m_Datas; }

		[[nodiscard]] inline u64 size(void) const { return m_Size; }
		[[nodiscard]] inline operator bool(void) const { return m_Size; }
	private:
		Na::ArrayVector<Data> m_Datas;

		u64 m_Size = 0;
	};
} // namespace Na

#endif // NA_STORAGE_BUFFER_HPP