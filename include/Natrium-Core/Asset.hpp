#if !defined(NA_ASSET_HPP)
#define NA_ASSET_HPP

#include "./Core.hpp"

namespace Na {
	class Asset {
	public:
		Asset(void) = default;
		virtual ~Asset(void) = default;

		[[nodiscard]] virtual inline operator bool(void) const = 0;
	};

	template<typename t_Asset = Asset>
	using AssetHandle = std::shared_ptr<t_Asset>;

	template<typename t_Asset = Asset>
	using WeakAssetHandle = std::weak_ptr<t_Asset>;
} // namespace Na

#endif // NA_ASSET_HPP