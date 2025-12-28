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
├── PikminGameMode._
└── PikminGameInstance._
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
- Readable, modular C++ cod
- Reusable subsystem
- Clean animation layering
- Minimal focus on graphics, VFX, and UI polish

## Future Improvements

- Group-based Pikmin formations
- Smarter obstacle-aware carrying
- Contextual Pikmin task priorities
- Save/load support
- Add update events for data to be shown on UI (currently using binding events)

## Engine & Tools

- Engine: Unreal Engine 5.x
- Language: C++
- Input: Keyboard / Gamepad
- AI: Custom controllers & logic (no Behavior Trees yet)

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
