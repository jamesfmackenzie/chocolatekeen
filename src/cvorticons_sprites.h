#ifndef _CHOCOLATE_KEEN_VORTICONS_SPRITES_
#define _CHOCOLATE_KEEN_VORTICONS_SPRITES_

#include <stdint.h>

/****************************************************************
Soon we have a complicated case:
- We want to define a Sprite_T type of a struct.
- It would have a pointer to a function that accepts
two pointers to Sprite_T structs as the input.
- We want to work inside an instance of some class (CVorticons).
- Furthermore, we may store a pointer to a function that belongs
to a specific child class (say CVorticons1). A static cast should
solve the latter point, although it looks a bit hackish.
***************************************************************/
/*
 * Source for the latter point:
 * http://stackoverflow.com/questions/3920157/base-classs-function-pointer-points-to-child-classs-member-function
 */

//class CVorticons;

struct SpriteStruct {
    //typedef void (*SpriteThinkFunction_T)();
    //typedef void (*SpriteContactFunction_T)(struct SpriteStruct *owner, struct SpriteStruct * contacted);
    //typedef void (CVorticons::*SpriteThinkFunction_T)();
    //typedef void (CVorticons::*SpriteContactFunction_T)(struct SpriteStruct *owner, struct SpriteStruct * contacted);

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
    //SpriteThinkFunction_T think;
    //SpriteContactFunction_T contact;
};

typedef struct SpriteStruct Sprite_T;
typedef void (*SpriteContactFunction_T)(Sprite_T *owner, Sprite_T * contacted);
typedef void (*SpriteThinkFunction_T)();

/********************************************************
We repeat the same function pointer mess, but for Body_T.
********************************************************/

struct BodyStruct {
    //typedef void (*BodyThinkFunction_T)(struct BodyStruct * body);
    //typedef void (CVorticons::*BodyThinkFunction_T)(struct BodyStruct * body);

    int32_t tile_x, tile_y; // x and y coordinates of body map tile
    uint16_t type_; // the type of the body
    int16_t variant; // the "flavor" of the body, eg ice cannons are variant 0-3 depending on direction
    int16_t field_C, field_E, field_10, field_12, field_14, field_16, field_18, field_1A, field_1C, field_1E, field_20;
    void (*think_ptr)(struct BodyStruct * body);
    //BodyThinkFunction_T think_ptr;
};

typedef struct BodyStruct Body_T;
typedef void (*BodyThinkFunction_T)(Body_T * body);

/**********************************************************
Sadly we need static casts, to work with child classes
like CVorticons1. (We could also declare lots of functions
in CVorticons as virtual, but that may not be much better.)
**********************************************************/

//#define CAST_SPRITE_THINK_FUNC static_cast<Sprite_T::SpriteThinkFunction_T>
//#define CAST_SPRITE_CONTACT_FUNC static_cast<Sprite_T::SpriteContactFunction_T>
//#define CAST_BODY_THINK_FUNC static_cast<Body_T::BodyThinkFunction_T>

#endif
