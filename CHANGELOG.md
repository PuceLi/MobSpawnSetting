# 更新日志

## 适配 LeviLamina 26.20.7 (2026-01-XX)

### 重大更新
- ✨ 适配到 LeviLamina 26.20.7 和 Minecraft 1.21.60+
- 🎯 **摆脱特征码依赖** - 使用符号化的函数 hook，无需每次更新重新定位特征码

### 技术改进
- 使用 `BedrockSpawner::$tick` 替代特征码 hook
- 使用 `BedrockSpawner::mTotalEntityCount` 成员变量替代硬编码偏移量
- 使用 `Mob::$checkSpawnRules` 符号化 hook
- 使用 `Dimension::$init` 符号化 hook
- 更新头文件引用以适配新版本 API

### 优势
- ✅ 更稳定 - 不再依赖容易变化的特征码
- ✅ 更安全 - 使用结构体成员而非内存偏移
- ✅ 易维护 - 版本更新时无需重新定位特征码

### 功能保持
所有原有功能保持不变：
- 生物生成密度控制
- 生物种类过滤（白名单/黑名单模式）
- 全局生成上限倍率
- 生成速度倍率控制
