# ue5-pikmin

A **Pikmin-inspired gameplay prototype** built in **Unreal Engine 5** using C++.  
This project focuses on small-squad AI behavior, throwing mechanics, whistle-based control, carryable objects, and clean system-driven architecture.

The goal of this prototype is **mechanical clarity and code quality**, not visual polish.

## Core Gameplay Features

### Player

- Third-person player character
- Rooted aiming state with independent facing rotation
- Pikmin throwing with trajectory targeting
- Whistle-based Pikmin command system
- Upper-body animation layering (throw / whistle over locomotion)

### Pikmin Creatures

- AI-driven following behavior
- State-based logic (Idle, Following, Carrying, Thrown, Busy)
- Whistle attraction and task assignment
- Collision-safe spawning near target locations
- Selection & prioritization logic for throwing

### Carryable Objects

- Weight-based multi-Pikmin carrying
- Navigation-aware movement using `CharacterMovementComponent`
- Delivery point interaction (Onions, goals, etc.)
- AI controller for coordinated movement

### AI Systems

- Custom AI controllers
- Behavior helpers and state logic
- Interaction-driven task system
- Clean separation between Player logic and Pikmin autonomy

## Architecture Overview

The project is structured around **feature-based folders**, not Unreal templates.

```bash
Source/
├── Pikmin/              # Pikmin character, sprout actor, anim instance
├── AI/                  # Pikmin AI controllers, behaviors, states
├── Interfaces/          # Gameplay interaction interfaces
├── Player/              # Player character, controller, anim instance
├── Systems/             # Core gameplay systems (Pikmin, Carrying, Tasks)
├── PikminGameMode
└── PikminGameInstance

Content/
├── Core/
│   ├── Input/
│   └── GameModes/
│
├── Characters/
│   ├── Player/
│   │   ├── BP_PlayerCharacter.uasset
│   │   ├── BP_PlayerController.uasset
│   │   └── Animations/
│   └── Pikmin/
│       ├── BP_PikminCharacter.uasset
│       ├── BP_PikminSprout.uasset
│       └── Animations/
│
├── Gameplay/
│   ├── Carryables/
│   │   ├── BP_CarryableObject_Base.uasset
│   │   └── BP_Carryable_*.uasset
│   ├── Delivery/
│   │   ├── BP_DeliveryPoint_Base.uasset
│   │   ├── BP_OnionDeliveryPoint.uasset
│   │   └── BP_TreasureDeliveryPoint.uasset
│   └── Systems/
│       └── BP_ItemDropOff_Base.uasset
│
├── UI/
│   ├── HUD/
│   │   └── BP_HUD.uasset
│   └── Widgets/
│       └── WBP_GameScreen.uasset
│
├── Art/
│   ├── Characters/
│   ├── Props/
│   ├── Environment/
│   ├── Materials/
│   └── Textures/
│
└── Maps/
    └── Lvl_Pikmin.umap
```

## Key Systems

### PikminManagerSubsystem

- Central authority for Pikmin spawning & tracking
- Enforces global Pikmin limits
- Handles sprout → Pikmin lifecycle
- Provides utility queries (e.g. nearest throwable Pikmin)

### PikminWhistleComponent

- Emits a radial command area
- Selects nearby Pikmin
- Assigns follow or task behavior
- Distance clamping & directional control

### PikminThrowTargetComponent

- Gamepad-aimable target indicator
- World-space positioning with distance limits
- Drives throw trajectory & animation alignment

### CarryableObject

- Character-based actor using `CharacterMovementComponent`
- Accepts Pikmin contributors
- Transitions between idle, carried, and delivered states

## Animation

### Player Character

- Layered animation system
- Upper-body slot for throw / whistle
- Lower-body locomotion preserved
- Rooted aiming with character rotation driven by target direction

### Pikmin

- Lightweight animation blueprint
- State-driven playback
- Reference pose used for non-animated upper body where applicable

## Development Goals

- System-driven gameplay
- Readable, modular C++ code
- Reusable subsystem
- Clean animation layering
- Minimal focus on graphics, VFX, and UI polish

## Future Improvements

- Group-based Pikmin formations
- Smarter obstacle-aware carrying
- Contextual Pikmin task priorities
- Save/load support
- Add update events for data to be shown on UI (currently using binding events)
- Update Carryables and Pikmin (and related systems) to use data-driven design

## Engine & Tools

- Engine: Unreal Engine 5.x
- Language: C++
- Input: Keyboard / Gamepad
- AI: Custom controllers & logic (no Behavior Trees yet)

## Asset Attribution

This project uses free asset packs created by Quaternius and released under the CC0 (Public Domain) license.

The following asset packs were used:

- [Ultimate Animated Character Pack](https://quaternius.com/packs/ultimatedanimatedcharacter.html)
- [Ultimate Food Pack](https://quaternius.com/packs/ultimatefood.html)
- [Ultimate Space Kit](https://quaternius.com/packs/ultimatespacekit.html)
- [Ultimate Stylized Nature Pack](https://quaternius.com/packs/ultimatestylizednature.html)

All assets are free to use, modify, and redistribute under the CC0 license.
Attribution is not required, but is provided here out of appreciation for the creator’s work.

## Disclaimer – Nintendo-Inspired Gameplay

This project is **inspired by gameplay concepts from Nintendo’s _Pikmin_ series**, including ideas such as small-creature squad control, carrying mechanics, and whistle-based commands.

I do **not** claim ownership of any Nintendo intellectual property, characters, designs, mechanics, or trademarks.  
All referenced concepts are used **solely for educational, prototyping, and portfolio purposes**.

This project:

- Is **non-commercial**
- Uses **original code and assets**
- Does **not** attempt to replicate or distribute Nintendo-owned content
- Exists to demonstrate **systems design, AI behavior, and Unreal Engine C++ architecture**

All rights to the _Pikmin_ franchise and related IP belong to **Nintendo Co., Ltd.**
