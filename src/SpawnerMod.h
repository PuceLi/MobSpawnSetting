#pragma once

#include "ll/api/mod/NativeMod.h"
#include "Config.h"

namespace SpawnerSetting {

class SpawnerMod {

public:
    static SpawnerMod& getInstance();

    SpawnerMod() : mSelf(*ll::mod::NativeMod::current()) {}

    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return mSelf; }

    [[nodiscard]] Config const& getConfig() const { return mConfig; }
    [[nodiscard]] DimensionConfigs const& getDimensionConfigs() const { return mDimensionConfigs; }
    [[nodiscard]] MobConfigs const& getMobConfigs() const { return mMobConfigs; }
    [[nodiscard]] BiomeConfigs const& getBiomeConfigs() const { return mBiomeConfigs; }

    bool load();
    bool enable();
    bool disable();
    bool unload();

    static void clearCache();

private:
    ll::mod::NativeMod& mSelf;
    Config              mConfig;
    DimensionConfigs    mDimensionConfigs;
    MobConfigs          mMobConfigs;
    BiomeConfigs        mBiomeConfigs;
};

} // namespace SpawnerSetting