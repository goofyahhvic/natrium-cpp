#if !defined(NA_ASSET_REGISTRY_HPP)
#define NA_ASSET_REGISTRY_HPP

#include "Natrium/Assets/Asset.hpp"
#include "Natrium/Assets/ShaderAsset.hpp"
#include "Natrium/Graphics/ShaderModule.hpp"

namespace Na {
	class AssetRegistry {
	public:
		AssetRegistry(const std::filesystem::path& asset_dir, const std::filesystem::path& shader_output_dir);
		inline ~AssetRegistry(void) { this->destroy(); }

		void destroy(void);

		inline void free_asset(const std::string_view& name) { m_Assets.erase(name); }
		inline void free_all(void) { m_Assets.clear(); }

		template<LoadableAsset T>
		inline AssetHandle<T> load_asset(const std::string_view& path)
		{
			auto it = m_Assets.find(path);
			if (it != m_Assets.end())
				return std::dynamic_pointer_cast<T>(it->second);

			AssetHandle<T> asset = T::Load(m_AssetDir / path);
			m_Assets[path] = asset;
			return asset;
		}

		ShaderModule create_shader_module_from_src(
			const std::string_view& src_path,
			ShaderStageBits stage,
			const std::string_view& entry_point = "main"
		) const;

		ShaderModule create_shader_module_from_str(
			const std::string_view& name,
			const std::string_view& src,
			ShaderStageBits stage,
			const std::string_view& entry_point = "main"
		) const;

		[[nodiscard]] inline std::filesystem::path& asset_dir(void) { return m_AssetDir; }
		[[nodiscard]] inline const std::filesystem::path& asset_dir(void) const { return m_AssetDir; }
		inline void set_asset_dir(const std::filesystem::path& asset_dir) { m_AssetDir = asset_dir; }
	private:
		std::unordered_map<std::string_view, AssetHandle<>> m_Assets;
		std::filesystem::path m_AssetDir;
		std::filesystem::path m_ShaderOutputDir;
	};
} // namespace Na

#endif // NA_ASSET_REGISTRY_HPP