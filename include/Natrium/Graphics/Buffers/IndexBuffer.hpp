#if !defined(NA_INDEX_BUFFER_HPP)
#define NA_INDEX_BUFFER_HPP

#include "Natrium/Graphics/Buffers/DeviceBuffer.hpp"

namespace Na {
	class IndexBuffer {
	public:
		IndexBuffer(void) = default;
		IndexBuffer(u32 count, const u32* data);
		void destroy(void);

		IndexBuffer(const IndexBuffer& other) = delete;
		IndexBuffer& operator=(const IndexBuffer& other) = delete;

		IndexBuffer(IndexBuffer&& other);
		IndexBuffer& operator=(IndexBuffer&& other);

		void set_data(const u32* data);

		[[nodiscard]] inline u64 size(void) const { return m_Buffer.size; }
		[[nodiscard]] inline u32 count(void) const { return m_Count; }

		[[nodiscard]] inline operator bool(void) const { return m_Count; }
		[[nodiscard]] inline vk::Buffer native(void) const { return m_Buffer.buffer; }
	private:
		DeviceBuffer m_Buffer;
		u32 m_Count = 0;
	};
} // namespace Na

#endif // NA_INDEX_BUFFER_HPP