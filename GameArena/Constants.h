#pragma once

inline const char* windowTitle = "raylib [core] example - mouse input";

const unsigned SCREEN_WIDTH = 1800;
const unsigned SCREEN_HEIGHT = 900;

const float VELOCITY = 0.3f;

const unsigned SPRITE_WIDTH = 29;
const unsigned SPRITE_HEIGHT = 24;

const unsigned CHARACTER_PLACEHOLDER_WIDTH = 200;
const unsigned CHARACTER_PLACEHOLDER_HEIGHT = 200;

const unsigned CHARACTER_WIDTH = 100;
const unsigned CHARACTER_HEIGHT = 200;

const unsigned ARENA_BORDER = 200;

//Animations
inline unsigned MOVE_ANIM_TIME = 600;
inline unsigned MOVE_ANIM_FRAMES = 4;

inline unsigned ATTACK_X_ANIM_TIME = 300;
inline unsigned ATTACK_X_ANIM_FRAMES = 3;

inline unsigned ATTACK_Y_ANIM_TIME = 400;
inline unsigned ATTACK_Y_ANIM_FRAMES = 4;

inline float TOP_DOWN_VELOCITY_LOSS = 0.2f;
inline float TOP_DOWN_VELOCITY_LOW_LIMIT = 0.05f;