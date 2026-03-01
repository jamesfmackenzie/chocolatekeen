#ifndef _CHOCOLATE_KEEN_VORTICONS_SPRITES_
#define _CHOCOLATE_KEEN_VORTICONS_SPRITES_

#include <stdint.h>

struct SpriteStruct {
    uint16_t type_;
    uint16_t active;
    int32_t posX, posY;
    int32_t boxX1, boxY1, boxX2, boxY2;
    int16_t delX, delY, velX, velY;
    int16_t health;
    uint16_t varA;
    uint16_t frame;
    int16_t time;
    int16_t varB;
    int16_t varC; // Seems to store the old vel_x (i.e. when keen jumps he pauses, then he continues with old vel x)
    int16_t varD; // If false, then think_13_keen_ground checks tiles
    void (*think)();
    void (*contact)(struct SpriteStruct *owner, struct SpriteStruct * contacted);
};

typedef struct SpriteStruct Sprite_T;
typedef void (*SpriteContactFunction_T)(Sprite_T *owner, Sprite_T * contacted);
typedef void (*SpriteThinkFunction_T)();

struct BodyStruct {
    int32_t tile_x, tile_y; // x and y coordinates of body map tile
    uint16_t type_; // the type of the body
    int16_t variant; // the "flavor" of the body, eg ice cannons are variant 0-3 depending on direction
    int16_t field_C, field_E, field_10, field_12, field_14, field_16, field_18, field_1A, field_1C, field_1E, field_20;
    void (*think_ptr)(struct BodyStruct * body);
};

typedef struct BodyStruct Body_T;
typedef void (*BodyThinkFunction_T)(Body_T * body);

#endif
