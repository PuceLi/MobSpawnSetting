**MobSpawnSettings** 是专为生存模式开发的插件，其提供了生物自然生成控制功能，允许管理员自定义生物生成的黑白名单、突破原版区块密度限制等。

[![English](https://img.shields.io/badge/English-informational?style=for-the-badge)](README.md)

---

## 主要功能

1. **黑/白名单控制**
    
    - 设置只允许或只禁止某些生物的生成

2. **区块密度倍增**
    
    - 修改每个区块允许容纳的怪物的数量

3. **修改全局生物生成上限**
    
    - 突破自然生成生物的全局的上限 200

4. **生物 ID / 族控制**
    
    - 可选择控制生物族或控制单个生物的生成

5. **修改生物生成速度**

    - 修改检查速度来增快生物生存速度

6. **正则表达式的支持**

    - 用正则表达式更高效的配置生成规则，方便配置 Addon 中的生物

7. **独立维度控制**

    - 自定义每个维度的自然生成设置

8. **独立生物控制**

    - 控制单独一个生物的生成规则

9. **独立生物群系控制**

    - 控制某些生物群系里的生成规则

10. **群体数量控制**

    - 修改生成群体时的最低数量和最高数量

---

## 配置文件

配置文件优先级：生物群系特定配置 > 全局生物配置 > 维度配置 > 全局默认配置
首次加载后会在 `plugins/MobSpawnSettings/config.json` 生成配置文件

### 配置介绍

#### 主配置 - config.json

``` json
{
    "version": 12,
    "whitelistMode": false,             // 工作模式：true=白名单 false=黑名单
    "enableFamilyFilter": true,         // 是否启用族过滤
    "targetFamilies": [
        "zombie"                        // 目标族 可参考 https://minecraft.fandom.com/zh/wiki/%E6%97%8F
    ],
    "enableIdentifierFilter": false,    // 是否启用 ID 过滤
    "targetMonsterIds": [
        "minecraft:creeper"             // 目标 ID
    ],
    "densityMultiplier": 4.0,           // 局部密度倍率 (大于 1.0 增加密度 小于则减少)
    "useRegex": true,                   // 是否启用正则表达式 启用后支持 ^minecraft: 这样的填写方式
    "globalCapMultiplier": 4.0,         // 全局上限倍率 (大于 1.0 为提高 小于则减少)
    "spawnSpeed": 2,                    // 尝试生成的速度（整数 代表每刻尝试生成的次数）
    "maxGroupSize": 20,                 // 群体生成时的最低数量（-1 为默认值）
    "minGroupSize": 19,                 // 群体生成时的最高数量（-1 为默认值）
    "enableDimensionConfig": false,     // 是否启用独立维度设置
    "enableMobConfig": false,           // 是否启用独立生物设置
    "enableBiomeConfig": false          // 是否启用独立生物群系设置
}
```

#### 生物群系配置 - biomes.json

``` json
{
  "version": 1,
  "biomes": [
    {
      "biomeName": "plains",                 // 生物群系 ID
      "enabled": true,                       // 是否启用此配置
      "densityMultiplier": 2.0,              // 生物群系局部密度倍率 (大于 1.0 增加密度 小于则减少)
      "spawnProbabilityMultiplier": 1.0,     // 尚未实现 无用
      "mobConfigs": [                        // 生物群系内生物配置
        {
          "identifier": "minecraft:cow",     // 生物 ID （未来会加入族支持）
          "enabled": true,                   // 是否生成该生物：true=生成生物 false=阻止生成
          "spawnProbabilityMultiplier": 1.0, // 尚未实现 无用
          "densityMultiplier": 3.0           // 生成密度倍率 (大于 1.0 增加密度 小于则减少)
        },
        {
          "identifier": "minecraft:sheep",
          "enabled": false,
          "spawnProbabilityMultiplier": 1.0,
          "densityMultiplier": 1.0
        }
      ]
    }
  ]
}
```

#### 维度配置 - dimensions.json

```json
{
  "version": 1,
  "dimensions": [
    {
      "dimensionId": 0,                   // 维度 ID (0=主世界 1=下界 2=末地 支持非原版维度)
      "enabled": true,                    // 是否启用此配置
      "densityMultiplier": 2.0,           // 维度内密度倍率 (大于 1.0 增加密度 小于则减少)
      "globalCapMultiplier": 2.0,         // 该维度的全局上限倍率
      "spawnSpeed": 2                     // 尝试生成的速度（整数 代表每刻尝试生成的次数）
    },
    {
      "dimensionId": 1,                   // 下界配置
      "enabled": true,
      "densityMultiplier": 5.0,
      "globalCapMultiplier": 5.0,
      "spawnSpeed": 5
    }
  ]
}
```

#### 生物配置 - mobs.json

```json
{
  "version": 1,
  "mobs": [
    {
      "identifier": "minecraft:zombie",   // 生物 ID
      "enabled": true,                    // 是否允许生成：true=允许生成 false=禁止生成
      "spawnProbabilityMultiplier": 1.0,  // 尚未实现 无用
      "densityMultiplier": 2.0            // 该生物的密度倍率
    },
    {
      "identifier": "minecraft:creeper", 
      "enabled": false,                   // 禁止生成
      "spawnProbabilityMultiplier": 1.0,
      "densityMultiplier": 1.0
    }
  ]
}
```

## 配置文件示例

### 主配置示例

#### 禁止苦力怕生成

不想生成苦力怕去炸家

``` json
{
    "version": 12,
    "whitelistMode": false,
    "enableFamilyFilter": true,
    "targetFamilies": [
        "creeper"
    ],
    "enableIdentifierFilter": false,
    "targetMonsterIds": [],
    "useRegex": false,
    "densityMultiplier": 1.0,
    "globalCapMultiplier": 1.0,
    "spawnSpeed": 1,
    "maxGroupSize": -1,
    "minGroupSize": -1,
    "enableDimensionConfig": false,
    "enableMobConfig": false,
    "enableBiomeConfig": false
}
```

### 刷怪塔优化（高密度）

允许所有怪生成 提高生成上限和速度

``` json
{
    "version": 12,
    "whitelistMode": false,
    "enableFamilyFilter": false,
    "targetFamilies": [],
    "enableIdentifierFilter": false,
    "targetMonsterIds": [],
    "useRegex": false,
    "densityMultiplier": 5.0,
    "globalCapMultiplier": 5.0,
    "spawnSpeed": 10,
    "maxGroupSize": 10,
    "minGroupSize": 10,
    "enableDimensionConfig": false,
    "enableMobConfig": false,
    "enableBiomeConfig": false
}
```

### 整活（别在正式服里玩）

会卡死服务端

``` json
{
    "version": 12,
    "whitelistMode": false,
    "enableFamilyFilter": false,
    "targetFamilies": [],
    "enableIdentifierFilter": false,
    "targetMonsterIds": [],
    "useRegex": false,
    "densityMultiplier": 5.0,
    "globalCapMultiplier": 5.0,
    "spawnSpeed": 10,
    "maxGroupSize": 10,
    "minGroupSize": 10,
    "enableDimensionConfig": false,
    "enableMobConfig": false,
    "enableBiomeConfig": false
}
```

> 主配置中黑名单模式下列表为空 = 允许自然生成所有可生成的生物

---

## ⚠️ 性能警告

- **将 `globalCapMultiplier` 和 `spawnSpeed` 同时调高时会让世界上瞬间生成几百个实体**

---

## 安装（服务端）

### 使用 LIP

`lip install github.com/PuceLi/MobSpawnSetting`

### 手动安装

从 **Releases** 下载该插件，解压到 `plugins` 内。

---

## MineBBS

[MobSpawnSettings](https://www.minebbs.com/resources/mobspawnsettings.14280/)