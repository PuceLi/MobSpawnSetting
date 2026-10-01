**MobSpawnSettings** is a plugin designed for survival mode that provides natural mob spawn control features, allowing administrators to customize mob spawn blacklists/whitelists, break through vanilla chunk density limits, and more.

[![中文](https://img.shields.io/badge/中文-informational?style=for-the-badge)](README_zh.md)

---

## Main Features

1. **Blacklist/Whitelist Control**
    
    - Set to allow only or block only certain mobs from spawning

2. **Chunk Density Multiplier**
    
    - Modify the number of monsters allowed in each chunk

3. **Modify Global Mob Spawn Cap**
    
    - Break through the natural spawn global limit of 200

4. **Mob ID / Family Control**
    
    - Choose to control mob families or individual mob spawning

5. **Modify Mob Spawn Speed**

    - Modify check speed to increase mob spawn rate

6. **Regular Expression Support**

    - Use regular expressions to configure spawn rules more efficiently, convenient for configuring addon mobs

7. **Independent Dimension Control**

    - Customize natural spawn settings for each dimension

8. **Independent Mob Control**

    - Control spawn rules for individual mobs

9. **Independent Biome Control**

    - Control spawn rules in specific biomes

10. **Group Size Control**

    - Modify minimum and maximum numbers when spawning in groups

---

## Configuration Files (config.json)

Configuration priority: Biome-specific config > Global mob config > Dimension config > Global default config
After first load, configuration files will be generated in `plugins/MobSpawnSettings/config.json`

### Configuration Guide

#### Main Config - config.json

``` json
{
    "version": 12,
    "whitelistMode": false,             // Working mode: true=whitelist false=blacklist
    "enableFamilyFilter": true,         // Enable family filtering
    "targetFamilies": [
        "zombie"                        // Target family, see https://minecraft.fandom.com/wiki/Family
    ],
    "enableIdentifierFilter": false,    // Enable ID filtering
    "targetMonsterIds": [
        "minecraft:creeper"             // Target ID
    ],
    "densityMultiplier": 4.0,           // Local density multiplier (>1.0 increases, <1.0 decreases)
    "useRegex": true,                   // Enable regex (supports patterns like ^minecraft:)
    "globalCapMultiplier": 4.0,         // Global cap multiplier (>1.0 increases, <1.0 decreases)
    "spawnSpeed": 2,                    // Spawn attempt speed (integer, attempts per tick)
    "maxGroupSize": 20,                 // Maximum group size when spawning (-1 for default)
    "minGroupSize": 19,                 // Minimum group size when spawning (-1 for default)
    "enableDimensionConfig": false,     // Enable independent dimension settings
    "enableMobConfig": false,           // Enable independent mob settings
    "enableBiomeConfig": false          // Enable independent biome settings
}
```

#### Biome Config - biomes.json

``` json
{
  "version": 1,
  "biomes": [
    {
      "biomeName": "plains",                 // Biome ID
      "enabled": true,                       // Enable this config (false=use default)
      "densityMultiplier": 2.0,              // Biome density multiplier (>1.0 increases, <1.0 decreases)
      "spawnProbabilityMultiplier": 1.0,     // Not yet implemented, no effect
      "mobConfigs": [                        // Mob configs within this biome
        {
          "identifier": "minecraft:cow",     // Mob ID (family support planned)
          "enabled": true,                   // Allow spawning: true=spawn false=block spawn
          "spawnProbabilityMultiplier": 1.0, // Not yet implemented, no effect
          "densityMultiplier": 3.0           // Spawn density multiplier (>1.0 increases, <1.0 decreases)
        },
        {
          "identifier": "minecraft:sheep",
          "enabled": false,                  // Block sheep spawning in plains
          "spawnProbabilityMultiplier": 1.0,
          "densityMultiplier": 1.0
        }
      ]
    }
  ]
}
```

#### Dimension Config - dimensions.json

```json
{
  "version": 1,
  "dimensions": [
    {
      "dimensionId": 0,                   // Dimension ID (0=Overworld 1=Nether 2=End, supports custom dimensions)
      "enabled": true,                    // Enable this config (false=use default)
      "densityMultiplier": 2.0,           // Dimension density multiplier (>1.0 increases, <1.0 decreases)
      "globalCapMultiplier": 2.0,         // Global cap multiplier for this dimension
      "spawnSpeed": 2                     // Spawn attempt speed (integer, attempts per tick)
    },
    {
      "dimensionId": 1,                   // Nether config
      "enabled": true,
      "densityMultiplier": 5.0,
      "globalCapMultiplier": 5.0,
      "spawnSpeed": 5
    }
  ]
}
```

#### Mob Config - mobs.json

```json
{
  "version": 1,
  "mobs": [
    {
      "identifier": "minecraft:zombie",   // Mob ID
      "enabled": true,                    // Allow spawning: true=allow false=block
      "spawnProbabilityMultiplier": 1.0,  // Not yet implemented, no effect
      "densityMultiplier": 2.0            // Density multiplier for this mob
    },
    {
      "identifier": "minecraft:creeper", 
      "enabled": false,                   // Block creeper spawning
      "spawnProbabilityMultiplier": 1.0,
      "densityMultiplier": 1.0
    }
  ]
}
```

## Configuration Examples

### Main Config Examples

#### Disable Creeper Spawning

Don't want creepers blowing up your house

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

#### Mob Farm Optimization (High Density)

Allow all mobs to spawn with increased cap and speed

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

#### Extreme Mode (Don't use on production servers)

Will lag the server

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

> Blacklist mode with empty lists = allow all naturally spawnable mobs

---

## ⚠️ Performance Warning

- **Setting both `globalCapMultiplier` and `spawnSpeed` to high values will instantly spawn hundreds of entities in the world**

---

## Installation (Server)

### Using LIP

`lip install github.com/PuceLi/MobSpawnSetting`

### Manual Installation

Download the plugin from **Releases** and extract it into the `plugins` folder.

---

## MineBBS

[MobSpawnSettings](https://www.minebbs.com/resources/mobspawnsettings.14280/)
