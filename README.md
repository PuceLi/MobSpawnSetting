**MobSpawnSettings** is a plugin specifically developed for Survival Mode. It provides comprehensive control over natural mob spawning, allowing administrators to customize white/blacklists, bypass vanilla chunk density limits, and more.

[![中文](https://img.shields.io/badge/简体中文-informational?style=for-the-badge)](README_zh.md)

---

## Features

1. **White/Blacklist Control**

    - Whitelist Mode: Only allows mobs belonging to specific families (e.g., `zombie`) to spawn; all other natural spawns are prohibited.
    
    - Blacklist Mode: Prevents specific mobs in the configuration list from spawning while allowing others normally. Ideal for removing annoying mobs like Creepers.

2. **Chunk Density Multiplier**
   
   - Setting this to `2.0` or higher doubles the number of monsters allowed per chunk.

   - Setting it to `0.5` halves the limit, effectively solving over-spawning issues.

3. **Global Spawning Cap Adjustment**
   
   - In Bedrock Edition, the natural global spawn cap is fixed at 200 regardless of difficulty. This plugin allows you to lower or raise this cap via the config.

4. **Mob ID & Family Control**
   
   - Choose to control spawning based on broad biological families or individual entity IDs.

5. **Spawning Speed Modification**
   
   - Increase the frequency of spawn attempts to make mobs appear faster.

6. **Regex Support**
   
   - Use Regular Expressions for more efficient and flexible spawning rule configurations.

---

## Configuration (`config.json`)

The configuration file is automatically generated at `plugins/MobSpawnSettings/config.json` after the first load.

### Default Configuration

```json5
{
    "version": 9,
    "whitelistMode": false,             // Work Mode: true=Whitelist, false=Blacklist
    "enableFamilyFilter": true,         // Enable filtering by mob family
    "targetFamilies": [
        "zombie"                        // Target families (Ref: https://minecraft.fandom.com/wiki/Family)
    ],
    "enableIdentifierFilter": false,    // Enable filtering by entity ID
    "targetMonsterIds": [
        "minecraft:creeper"             // Target Entity IDs
    ],
    "densityMultiplier": 4.0,           // Local density multiplier (>1.0 increases density)
    "useRegex": true,                   // Enable Regex (supports patterns like ^minecraft:)
    "globalCapMultiplier": 4.0,         // Global cap multiplier (>1.0 increases max mobs)
    "spawnSpeed": 2                     // Spawning attempt speed (e.g., 2 = two attempts per tick)
}

```

## Configuration Examples

### Disable Creepers

Prevents Creepers from spawning to protect player structures.

```json
{
    "version": 9,
    "whitelistMode": false,
    "enableFamilyFilter": true,
    "targetFamilies": ["creeper"],
    "enableIdentifierFilter": false,
    "targetMonsterIds": [],
    "densityMultiplier": 1.0,
    "useRegex": false,
    "globalCapMultiplier": 1.0,
    "spawnSpeed": 1
}

```

### Mob Farm Optimization (High Density)

Allows all mobs to spawn while significantly increasing the cap and spawn frequency.

```json
{
    "version": 9,
    "whitelistMode": false,
    "enableFamilyFilter": false,
    "targetFamilies": [],
    "enableIdentifierFilter": false,
    "targetMonsterIds": [],
    "densityMultiplier": 4.0,
    "useRegex": false,
    "globalCapMultiplier": 4.0,
    "spawnSpeed": 100
}

```

> In Blacklist mode, leaving the list empty allows all naturally spawnable mobs to appear.

---

## ⚠️ Performance Warning

- **Setting `globalCapMultiplier` and `spawnSpeed` to high values simultaneously can cause hundreds of entities to spawn instantly, potentially crashing the server.**

---

## Installation(Server)

### Using LIP

`lip install github.com/PuceLi/MobSpawnSetting`

### Manual Installation

Download the latest version from **Releases** and extract it into the `plugins` folder.

---

## MineBBS

[MobSpawnSettings](https://www.minebbs.com/resources/mobspawnsettings.14280/)