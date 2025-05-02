#if !defined(NA_MODEL_ASSET_HPP)
#define NA_MODEL_ASSET_HPP

#include "Natrium/Assets/Asset.hpp"

namespace Na {
	struct Vertex {
		glm::vec3 position;
		glm::vec2 uv_coord;

		[[nodiscard]] inline bool operator==(const Vertex& other) const { return this->position == other.position && this->uv_coord == other.uv_coord; }
	};

	class ModelAsset : public Asset {
	public:
		ModelAsset(void) = default;
		~ModelAsset(void) = default;

		static AssetHandle<ModelAsset> Load(const std::filesystem::path& path);

		[[nodiscard]] inline u64 vertex_data_size(void) const { return m_Vertices.size() * sizeof(Vertex); }
		[[nodiscard]] inline u64 index_data_size(void) const { return m_Indices.size() * sizeof(u32); }

		[[nodiscard]] inline u64 vertex_count(void) const { return m_Vertices.size(); }
		[[nodiscard]] inline u64 index_count(void) const { return m_Indices.size(); }

		[[nodiscard]] inline Na::ArrayList<Vertex>& vertices(void) { return m_Vertices; }
		[[nodiscard]] inline const Na::ArrayList<Vertex>& vertices(void) const { return m_Vertices; }

		[[nodiscard]] inline Na::ArrayList<u32>& indices(void) { return m_Indices; }
		[[nodiscard]] inline const Na::ArrayList<u32>& indices(void) const { return m_Indices; }

		[[nodiscard]] inline operator bool(void) const override { return !m_Vertices.empty() && !m_Indices.empty(); };

	private:
		Na::ArrayList<Vertex> m_Vertices;
		Na::ArrayList<u32> m_Indices;
	};
	using Model = ModelAsset;
}

namespace std {
	template<> struct hash<Na::Vertex> {
		size_t operator()(Na::Vertex const& vertex) const
		{
			return ((
				hash<glm::vec3>()(vertex.position) ^
				(hash<glm::vec2>()(vertex.uv_coord) << 1)
			));
		}
	};
} // namespace std

#endif // NA_MODEL_ASSET_HPP