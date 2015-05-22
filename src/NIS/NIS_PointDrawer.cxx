// File:      NIS_PointDrawer.cpp
// Created:   11.05.12
// Author:    Natalia Kopnova
// Copyright: LPKF Laser and Electronics AG 2012

#include <NIS_PointDrawer.hxx>
#include <NIS_Point.hxx>
#include <Precision.hxx>
#ifdef _DEBUG
#include <Standard_ProgramError.hxx>
#endif
#ifdef WNT
#include <windows.h>
#include <GL/gl.h>
#endif

//=======================================================================
//purpose  : Marker definition
//=======================================================================
#define NUM_TYPES 20
#define NUM_SIZES 5

#define COUNT_PLUS_1 9*2
#define COUNT_PLUS_2 11*2
#define COUNT_PLUS_3 13*2
#define COUNT_PLUS_4 15*2
#define COUNT_PLUS_5 17*3

#define COUNT_STAR_1 9
#define COUNT_STAR_2 11*2
#define COUNT_STAR_3 13*2
#define COUNT_STAR_4 15*2
#define COUNT_STAR_5 17*2

#define COUNT_CROSS_1 7
#define COUNT_CROSS_2 9*2
#define COUNT_CROSS_3 11*2
#define COUNT_CROSS_4 13*2
#define COUNT_CROSS_5 15*2

#define COUNT_CIRCLE_1 7
#define COUNT_CIRCLE_2 8
#define COUNT_CIRCLE_3 9*2
#define COUNT_CIRCLE_4 11*2
#define COUNT_CIRCLE_5 13*2

#define COUNT_FILLED_CIRCLE_1 7
#define COUNT_FILLED_CIRCLE_2 8
#define COUNT_FILLED_CIRCLE_3 9*2
#define COUNT_FILLED_CIRCLE_4 11*2
#define COUNT_FILLED_CIRCLE_5 13*2

#define COUNT_ANGLE0_1 7
#define COUNT_ANGLE0_2 8
#define COUNT_ANGLE0_3 11*2
#define COUNT_ANGLE0_4 13*2
#define COUNT_ANGLE0_5 15*2

#define COUNT_ANGLE90_1 7
#define COUNT_ANGLE90_2 8
#define COUNT_ANGLE90_3 10*2
#define COUNT_ANGLE90_4 13*2
#define COUNT_ANGLE90_5 16*2

#define COUNT_ANGLE180_1 7
#define COUNT_ANGLE180_2 8
#define COUNT_ANGLE180_3 11*2
#define COUNT_ANGLE180_4 13*2
#define COUNT_ANGLE180_5 15*2

#define COUNT_ANGLE270_1 7
#define COUNT_ANGLE270_2 8
#define COUNT_ANGLE270_3 10*2
#define COUNT_ANGLE270_4 13*2
#define COUNT_ANGLE270_5 16*2

#define COUNT_ANGLE45_1 8
#define COUNT_ANGLE45_2 8
#define COUNT_ANGLE45_3 11*2
#define COUNT_ANGLE45_4 14*2
#define COUNT_ANGLE45_5 16*2

#define COUNT_ANGLE135_1 7
#define COUNT_ANGLE135_2 8
#define COUNT_ANGLE135_3 11*2
#define COUNT_ANGLE135_4 14*2
#define COUNT_ANGLE135_5 16*2

#define COUNT_ANGLE225_1 8
#define COUNT_ANGLE225_2 8
#define COUNT_ANGLE225_3 11*2
#define COUNT_ANGLE225_4 14*2
#define COUNT_ANGLE225_5 16*2

#define COUNT_ANGLE315_1 7
#define COUNT_ANGLE315_2 8
#define COUNT_ANGLE315_3 11*2
#define COUNT_ANGLE315_4 14*2
#define COUNT_ANGLE315_5 16*2

#define COUNT_SQUARE_17x17 17*3

#define COUNT_TRIANGLE_17x15 15*3

#define COUNT_CIRCLE_BOLD_19x19 19*3

#define COUNT_CROSS_BOLD_17x17 17*3

#define COUNT_ROMB_19x19 19*3

#define COUNT_CROSS_CIRCLE_19x19 19*3

#define COUNT_LINE 24*3

#define OFFSET_PLUS_1 0
#define OFFSET_PLUS_2 OFFSET_PLUS_1 + COUNT_PLUS_1
#define OFFSET_PLUS_3 OFFSET_PLUS_2 + COUNT_PLUS_2
#define OFFSET_PLUS_4 OFFSET_PLUS_3 + COUNT_PLUS_3
#define OFFSET_PLUS_5 OFFSET_PLUS_4 + COUNT_PLUS_4

#define OFFSET_STAR_1 OFFSET_PLUS_5 + COUNT_PLUS_5
#define OFFSET_STAR_2 OFFSET_STAR_1 + COUNT_STAR_1
#define OFFSET_STAR_3 OFFSET_STAR_2 + COUNT_STAR_2
#define OFFSET_STAR_4 OFFSET_STAR_3 + COUNT_STAR_3
#define OFFSET_STAR_5 OFFSET_STAR_4 + COUNT_STAR_4

#define OFFSET_CROSS_1 OFFSET_STAR_5 + COUNT_STAR_5
#define OFFSET_CROSS_2 OFFSET_CROSS_1 + COUNT_CROSS_1
#define OFFSET_CROSS_3 OFFSET_CROSS_2 + COUNT_CROSS_2
#define OFFSET_CROSS_4 OFFSET_CROSS_3 + COUNT_CROSS_3
#define OFFSET_CROSS_5 OFFSET_CROSS_4 + COUNT_CROSS_4

#define OFFSET_CIRCLE_1 OFFSET_CROSS_5 + COUNT_CROSS_5
#define OFFSET_CIRCLE_2 OFFSET_CIRCLE_1 + COUNT_CIRCLE_1
#define OFFSET_CIRCLE_3 OFFSET_CIRCLE_2 + COUNT_CIRCLE_2
#define OFFSET_CIRCLE_4 OFFSET_CIRCLE_3 + COUNT_CIRCLE_3
#define OFFSET_CIRCLE_5 OFFSET_CIRCLE_4 + COUNT_CIRCLE_4

#define OFFSET_FILLED_CIRCLE_1 OFFSET_CIRCLE_5 + COUNT_CIRCLE_5
#define OFFSET_FILLED_CIRCLE_2 OFFSET_FILLED_CIRCLE_1 + COUNT_FILLED_CIRCLE_1
#define OFFSET_FILLED_CIRCLE_3 OFFSET_FILLED_CIRCLE_2 + COUNT_FILLED_CIRCLE_2
#define OFFSET_FILLED_CIRCLE_4 OFFSET_FILLED_CIRCLE_3 + COUNT_FILLED_CIRCLE_3
#define OFFSET_FILLED_CIRCLE_5 OFFSET_FILLED_CIRCLE_4 + COUNT_FILLED_CIRCLE_4

#define OFFSET_ANGLE0_1 OFFSET_FILLED_CIRCLE_5 + COUNT_FILLED_CIRCLE_5
#define OFFSET_ANGLE0_2 OFFSET_ANGLE0_1 + COUNT_ANGLE0_1
#define OFFSET_ANGLE0_3 OFFSET_ANGLE0_2 + COUNT_ANGLE0_2
#define OFFSET_ANGLE0_4 OFFSET_ANGLE0_3 + COUNT_ANGLE0_3
#define OFFSET_ANGLE0_5 OFFSET_ANGLE0_4 + COUNT_ANGLE0_4

#define OFFSET_ANGLE90_1 OFFSET_ANGLE0_5 + COUNT_ANGLE0_5
#define OFFSET_ANGLE90_2 OFFSET_ANGLE90_1 + COUNT_ANGLE90_1
#define OFFSET_ANGLE90_3 OFFSET_ANGLE90_2 + COUNT_ANGLE90_2
#define OFFSET_ANGLE90_4 OFFSET_ANGLE90_3 + COUNT_ANGLE90_3
#define OFFSET_ANGLE90_5 OFFSET_ANGLE90_4 + COUNT_ANGLE90_4

#define OFFSET_ANGLE180_1 OFFSET_ANGLE90_5 + COUNT_ANGLE90_5
#define OFFSET_ANGLE180_2 OFFSET_ANGLE180_1 + COUNT_ANGLE180_1
#define OFFSET_ANGLE180_3 OFFSET_ANGLE180_2 + COUNT_ANGLE180_2
#define OFFSET_ANGLE180_4 OFFSET_ANGLE180_3 + COUNT_ANGLE180_3
#define OFFSET_ANGLE180_5 OFFSET_ANGLE180_4 + COUNT_ANGLE180_4

#define OFFSET_ANGLE270_1 OFFSET_ANGLE180_5 + COUNT_ANGLE180_5
#define OFFSET_ANGLE270_2 OFFSET_ANGLE270_1 + COUNT_ANGLE270_1
#define OFFSET_ANGLE270_3 OFFSET_ANGLE270_2 + COUNT_ANGLE270_2
#define OFFSET_ANGLE270_4 OFFSET_ANGLE270_3 + COUNT_ANGLE270_3
#define OFFSET_ANGLE270_5 OFFSET_ANGLE270_4 + COUNT_ANGLE270_4

#define OFFSET_ANGLE45_1 OFFSET_ANGLE270_5 + COUNT_ANGLE270_5
#define OFFSET_ANGLE45_2 OFFSET_ANGLE45_1 + COUNT_ANGLE45_1
#define OFFSET_ANGLE45_3 OFFSET_ANGLE45_2 + COUNT_ANGLE45_2
#define OFFSET_ANGLE45_4 OFFSET_ANGLE45_3 + COUNT_ANGLE45_3
#define OFFSET_ANGLE45_5 OFFSET_ANGLE45_4 + COUNT_ANGLE45_4

#define OFFSET_ANGLE135_1 OFFSET_ANGLE45_5 + COUNT_ANGLE45_5
#define OFFSET_ANGLE135_2 OFFSET_ANGLE135_1 + COUNT_ANGLE135_1
#define OFFSET_ANGLE135_3 OFFSET_ANGLE135_2 + COUNT_ANGLE135_2
#define OFFSET_ANGLE135_4 OFFSET_ANGLE135_3 + COUNT_ANGLE135_3
#define OFFSET_ANGLE135_5 OFFSET_ANGLE135_4 + COUNT_ANGLE135_4

#define OFFSET_ANGLE225_1 OFFSET_ANGLE135_5 + COUNT_ANGLE135_5
#define OFFSET_ANGLE225_2 OFFSET_ANGLE225_1 + COUNT_ANGLE225_1
#define OFFSET_ANGLE225_3 OFFSET_ANGLE225_2 + COUNT_ANGLE225_2
#define OFFSET_ANGLE225_4 OFFSET_ANGLE225_3 + COUNT_ANGLE225_3
#define OFFSET_ANGLE225_5 OFFSET_ANGLE225_4 + COUNT_ANGLE225_4

#define OFFSET_ANGLE315_1 OFFSET_ANGLE225_5 + COUNT_ANGLE225_5
#define OFFSET_ANGLE315_2 OFFSET_ANGLE315_1 + COUNT_ANGLE315_1
#define OFFSET_ANGLE315_3 OFFSET_ANGLE315_2 + COUNT_ANGLE315_2
#define OFFSET_ANGLE315_4 OFFSET_ANGLE315_3 + COUNT_ANGLE315_3
#define OFFSET_ANGLE315_5 OFFSET_ANGLE315_4 + COUNT_ANGLE315_4

#define OFFSET_SQUARE_17x17       OFFSET_ANGLE315_5 + COUNT_ANGLE315_5

#define OFFSET_TRIANGLE_17x15     OFFSET_SQUARE_17x17  + COUNT_SQUARE_17x17

#define OFFSET_CIRCLE_BOLD_19x19  OFFSET_TRIANGLE_17x15 + COUNT_TRIANGLE_17x15

#define OFFSET_CROSS_BOLD_17x17   OFFSET_CIRCLE_BOLD_19x19 + COUNT_CIRCLE_BOLD_19x19

#define OFFSET_ROMB_19x19         OFFSET_CROSS_BOLD_17x17 + COUNT_CROSS_BOLD_17x17

#define OFFSET_CROSS_CIRCLE_19x19 OFFSET_ROMB_19x19 + COUNT_ROMB_19x19

#define OFFSET_LINE               OFFSET_CROSS_CIRCLE_19x19 + COUNT_CROSS_CIRCLE_19x19

struct markerbase {
    int width;
    int height;
    int offset;
};

static markerbase data[NUM_TYPES][NUM_SIZES] = {
    { // Plus
        { 9, 9, OFFSET_PLUS_1 },
        { 11, 11, OFFSET_PLUS_2 },
        { 13, 13, OFFSET_PLUS_3 },
        { 15, 15, OFFSET_PLUS_4 },
        { 17, 17, OFFSET_PLUS_5 }
    },
    { // Star
        { 7, 9, OFFSET_STAR_1 },
        { 9, 11, OFFSET_STAR_2 },
        { 11, 13, OFFSET_STAR_3 },
        { 13, 15, OFFSET_STAR_4 },
        { 13, 17, OFFSET_STAR_5 }
    },
    { // Cross
        { 7, 7, OFFSET_CROSS_1 },
        { 9, 9, OFFSET_CROSS_2 },
        { 11, 11, OFFSET_CROSS_3 },
        { 13, 13, OFFSET_CROSS_4 },
        { 15, 15, OFFSET_CROSS_5 }
    },
    { // Circle
        { 7, 7, OFFSET_CIRCLE_1 },
        { 8, 8, OFFSET_CIRCLE_2 },
        { 9, 9, OFFSET_CIRCLE_3 },
        { 11, 11, OFFSET_CIRCLE_4 },
        { 13, 13, OFFSET_CIRCLE_5 }
    },
    { // Filled Circle
        { 7, 7, OFFSET_FILLED_CIRCLE_1 },
        { 8, 8, OFFSET_FILLED_CIRCLE_2 },
        { 9, 9, OFFSET_FILLED_CIRCLE_3 },
        { 11, 11, OFFSET_FILLED_CIRCLE_4 },
        { 13, 13, OFFSET_FILLED_CIRCLE_5 }
    },
    { // Arrow right
        { 7, 7, OFFSET_ANGLE0_1 },
        { 8, 8, OFFSET_ANGLE0_2 },
        { 10, 11, OFFSET_ANGLE0_3 },
        { 13, 13, OFFSET_ANGLE0_4 },
        { 16, 15, OFFSET_ANGLE0_5 }
    },
    { // Arrow up
        { 7, 7, OFFSET_ANGLE90_1 },
        { 8, 8, OFFSET_ANGLE90_2 },
        { 11, 10, OFFSET_ANGLE90_3 },
        { 13, 13, OFFSET_ANGLE90_4 },
        { 15, 16, OFFSET_ANGLE90_5 }
    },
    { // Arrow left
        { 7, 7, OFFSET_ANGLE180_1 },
        { 8, 8, OFFSET_ANGLE180_2 },
        { 10, 11, OFFSET_ANGLE180_3 },
        { 13, 13, OFFSET_ANGLE180_4 },
        { 16, 15, OFFSET_ANGLE180_5 }
    },
    { // Arrow down
        { 7, 7, OFFSET_ANGLE270_1 },
        { 8, 8, OFFSET_ANGLE270_2 },
        { 11, 10, OFFSET_ANGLE270_3 },
        { 13, 13, OFFSET_ANGLE270_4 },
        { 15, 16, OFFSET_ANGLE270_5 }
    },
    { // Arrow up-right
        { 8, 7, OFFSET_ANGLE45_1 },
        { 8, 8, OFFSET_ANGLE45_2 },
        { 11, 11, OFFSET_ANGLE45_3 },
        { 14, 14, OFFSET_ANGLE45_4 },
        { 16, 16, OFFSET_ANGLE45_5 }
    },
    { // Arrow up-left
        { 7, 8, OFFSET_ANGLE135_1 },
        { 8, 8, OFFSET_ANGLE135_2 },
        { 11, 11, OFFSET_ANGLE135_3 },
        { 14, 14, OFFSET_ANGLE135_4 },
        { 16, 16, OFFSET_ANGLE135_5 }
    },
    { // Arrow down-left
        { 8, 7, OFFSET_ANGLE225_1 },
        { 8, 8, OFFSET_ANGLE225_2 },
        { 11, 11, OFFSET_ANGLE225_3 },
        { 14, 14, OFFSET_ANGLE225_4 },
        { 16, 16, OFFSET_ANGLE225_5 }
    },
    { // Arrow down-right
        { 7, 8, OFFSET_ANGLE315_1 },
        { 8, 8, OFFSET_ANGLE315_2 },
        { 11, 11, OFFSET_ANGLE315_3 },
        { 14, 14, OFFSET_ANGLE315_4 },
        { 16, 16, OFFSET_ANGLE315_5 }
    },
    {
        { 17, 17, OFFSET_SQUARE_17x17 },
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 }
    },
    {
        { 17, 15, OFFSET_TRIANGLE_17x15 },
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 }
    },
    {
        { 19, 19, OFFSET_CIRCLE_BOLD_19x19 },
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 }
    },
    {
        { 17, 17, OFFSET_CROSS_BOLD_17x17 },
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 }
    },
    {
        { 19, 19, OFFSET_ROMB_19x19 },
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 }
    },
    {
        { 19, 19, OFFSET_CROSS_CIRCLE_19x19 },
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 }
    },
    {
        { 24, 24, OFFSET_LINE },
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 }
    }
};

static unsigned char raster[] = {
    0x08,0x00,
    0x08,0x00,
    0x08,0x00,
    0x08,0x00,
    0xff,0x80,
    0x08,0x00,
    0x08,0x00,
    0x08,0x00,
    0x08,0x00,  // Plus 9x9

    0x04,0x00,
    0x04,0x00,
    0x04,0x00,
    0x04,0x00,
    0x04,0x00,
    0xff,0xe0,
    0x04,0x00,
    0x04,0x00,
    0x04,0x00,
    0x04,0x00,
    0x04,0x00,  // Plus 11x11

    0x02,0x00,
    0x02,0x00,
    0x02,0x00,
    0x02,0x00,
    0x02,0x00,
    0x02,0x00,
    0xff,0xf8,
    0x02,0x00,
    0x02,0x00,
    0x02,0x00,
    0x02,0x00,
    0x02,0x00,
    0x02,0x00,  // Plus 13x13

    0x01,0x00,
    0x01,0x00,
    0x01,0x00,
    0x01,0x00,
    0x01,0x00,
    0x01,0x00,
    0x01,0x00,
    0xff,0xfe,
    0x01,0x00,
    0x01,0x00,
    0x01,0x00,
    0x01,0x00,
    0x01,0x00,
    0x01,0x00,
    0x01,0x00, // Plus 15x15

    0x00,0x80,0x00,
    0x00,0x80,0x00,
    0x00,0x80,0x00,
    0x00,0x80,0x00,
    0x00,0x80,0x00,
    0x00,0x80,0x00,
    0x00,0x80,0x00,
    0x00,0x80,0x00,
    0xff,0xff,0x80,
    0x00,0x80,0x00,
    0x00,0x80,0x00,
    0x00,0x80,0x00,
    0x00,0x80,0x00,
    0x00,0x80,0x00,
    0x00,0x80,0x00,
    0x00,0x80,0x00,
    0x00,0x80,0x00, // Plus 17x17

    0x10,
    0x10,
    0xd6,
    0x38,
    0x10,
    0x38,
    0xd6,
    0x10,
    0x10,  // Star 7x9

    0x08,0x00,
    0x08,0x00,
    0x08,0x00,
    0xc9,0x80,
    0x3e,0x00,
    0x08,0x00,
    0x3e,0x00,
    0xc9,0x80,
    0x08,0x00,
    0x08,0x00,
    0x08,0x00,  // Star 9x11

    0x04,0x00,
    0x04,0x00,
    0x04,0x00,
    0x84,0x20,
    0x64,0xc0,
    0x1f,0x00,
    0x04,0x00,
    0x1f,0x00,
    0x64,0xc0,
    0x84,0x20,
    0x04,0x00,
    0x04,0x00,
    0x04,0x00,  // Star 11x13

    0x02,0x00,
    0x02,0x00,
    0x02,0x00,
    0x02,0x00,
    0x82,0x18,
    0x62,0x60,
    0x1b,0x80,
    0x06,0x00,
    0x1b,0x80,
    0x62,0x60,
    0x82,0x18,
    0x02,0x00,
    0x02,0x00,
    0x02,0x00,
    0x02,0x00,  // Start 13x15

    0x02,0x00,
    0x02,0x00,
    0x02,0x00,
    0x02,0x00,
    0x82,0x08,
    0x62,0x30,
    0x12,0x40,
    0x0f,0x80,
    0x02,0x00,
    0x0f,0x80,
    0x12,0x40,
    0x62,0x30,
    0x82,0x08,
    0x02,0x00,
    0x02,0x00,
    0x02,0x00,
    0x02,0x00,  // Star 13x17

    0x82,
    0x44,
    0x28,
    0x10,
    0x28,
    0x44,
    0x82,  // Cross 7x7

    0x80,0x80,
    0x41,0x00,
    0x22,0x00,
    0x14,0x00,
    0x08,0x00,
    0x14,0x00,
    0x22,0x00,
    0x41,0x00,
    0x80,0x80,  // Cross 9x9

    0x80,0x20,
    0x40,0x40,
    0x20,0x80,
    0x11,0x00,
    0x0a,0x00,
    0x04,0x00,
    0x0a,0x00,
    0x11,0x00,
    0x20,0x80,
    0x40,0x40,
    0x80,0x20,  // Cross 11x11

    0x80,0x08,
    0x40,0x10,
    0x20,0x20,
    0x10,0x40,
    0x08,0x80,
    0x05,0x00,
    0x02,0x00,
    0x05,0x00,
    0x08,0x80,
    0x10,0x40,
    0x20,0x20,
    0x40,0x10,
    0x80,0x08,  // Cross 13x13

    0x80,0x02,
    0x40,0x04,
    0x20,0x08,
    0x10,0x10,
    0x08,0x20,
    0x04,0x40,
    0x02,0x80,
    0x01,0x00,
    0x02,0x80,
    0x04,0x40,
    0x08,0x20,
    0x10,0x10,
    0x20,0x08,
    0x40,0x04,
    0x80,0x02,  // Cross 15x15

    0x38,
    0x44,
    0x82,
    0x82,
    0x82,
    0x44,
    0x38,  // Circle 7x7

    0x3c,
    0x42,
    0x81,
    0x81,
    0x81,
    0x81,
    0x42,
    0x3c,  // Circle 8x8

    0x3e,0x00,
    0x41,0x00,
    0x81,0x80,
    0x80,0x80,
    0x80,0x80,
    0x80,0x80,
    0x81,0x80,
    0x41,0x00,
    0x3e,0x00,  // Circle 9x9

    0x1f,0x00,
    0x20,0x80,
    0x40,0x40,
    0x80,0x20,
    0x80,0x20,
    0x80,0x20,
    0x80,0x20,
    0x80,0x20,
    0x40,0x40,
    0x20,0x80,
    0x1f,0x00,  // Circle 11x11

    0x0f,0x80,
    0x10,0x40,
    0x20,0x20,
    0x40,0x10,
    0x80,0x08,
    0x80,0x08,
    0x80,0x08,
    0x80,0x08,
    0x80,0x08,
    0x40,0x10,
    0x20,0x20,
    0x10,0x40,
    0x0f,0x80,   // Circle 13x13

    0x38,
    0x7c,
    0xfe,
    0xfe,
    0xfe,
    0x7c,
    0x38,  // Filled Circle 7x7

    0x3c,
    0x7e,
    0xff,
    0xff,
    0xff,
    0xff,
    0x7e,
    0x3c,  // Filled Circle 8x8

    0x3e,0x00,
    0x7f,0x00,
    0xff,0x80,
    0xff,0x80,
    0xff,0x80,
    0xff,0x80,
    0xff,0x80,
    0x7f,0x00,
    0x3e,0x00,  // Filled Circle 9x9

    0x1f,0x00,
    0x3f,0x80,
    0x7f,0xc0,
    0xff,0xe0,
    0xff,0xe0,
    0xff,0xe0,
    0xff,0xe0,
    0xff,0xe0,
    0x7f,0xc0,
    0x3f,0x80,
    0x1f,0x00,  // Filled Circle 11x11

    0x0f,0x80,
    0x1f,0xc0,
    0x3f,0xe0,
    0x7f,0xf0,
    0xff,0xf8,
    0xff,0xf8,
    0xff,0xf8,
    0xff,0xf8,
    0xff,0xf8,
    0x7f,0xf0,
    0x3f,0xe0,
    0x1f,0xc0,
    0x0f,0x80,   // Filled Circle 13x13

    0xc0,
    0xf0,
    0x3c,
    0x3e,
    0x3c,
    0xf0,
    0xc0,  // Arrow right 7x7

    0xc0,
    0x70,
    0x7c,
    0x3f,
    0x3f,
    0x7c,
    0x70,
    0xc0,  // Arrow right 8x8

    0x80, 0x00,
    0xe0, 0x00,
    0x78, 0x00,
    0x7e, 0x00,
    0x3f, 0x80,
    0x3f, 0xc0,
    0x3f, 0x80,
    0x7e, 0x00,
    0x78, 0x00,
    0xe0, 0x00,
    0x80, 0x00,  // Arrow right 11x10

    0x80, 0x00,
    0xe0, 0x00,
    0x78, 0x00,
    0x7e, 0x00,
    0x3f, 0x80,
    0x3f, 0xe0,
    0x1f, 0xf8,
    0x3f, 0xe0,
    0x3f, 0x80,
    0x7e, 0x00,
    0x78, 0x00,
    0xe0, 0x00,
    0x80, 0x00,  // Arrow right 13x13

    0xc0, 0x00,
    0x70, 0x00,
    0x7c, 0x00,
    0x3f, 0x00,
    0x3f, 0xc0,
    0x1f, 0xf0,
    0x1f, 0xfc,
    0x0f, 0xff,
    0x1f, 0xfc,
    0x1f, 0xf0,
    0x3f, 0xc0,
    0x3f, 0x00,
    0x7c, 0x00,
    0x70, 0x00,
    0xc0, 0x00,  // Arrow right 16x15

    0xc6,
    0xc6,
    0x7c,
    0x7c,
    0x38,
    0x38,
    0x10,  // Arrow up 7x7

    0x81,
    0xe7,
    0x7e,
    0x7e,
    0x3c,
    0x3c,
    0x18,
    0x18,  // Arrow up 8x8

    0xc0, 0x60,
    0x71, 0xc0,
    0x7f, 0xc0,
    0x3f, 0x80,
    0x3f, 0x80,
    0x1f, 0x00,
    0x1f, 0x00,
    0x0e, 0x00,
    0x0e, 0x00,
    0x04, 0x00,  // Arrow up 10x11

    0xc0, 0x18,
    0x70, 0x70,
    0x7d, 0xf0,
    0x3f, 0xe0,
    0x3f, 0xe0,
    0x1f, 0xc0,
    0x1f, 0xc0,
    0x1f, 0xc0,
    0x0f, 0x80,
    0x0f, 0x80,
    0x07, 0x00,
    0x07, 0x00,
    0x02, 0x00,  // Arrow up 13x13

    0x80, 0x02,
    0xe0, 0x0e,
    0x78, 0x3c,
    0x7e, 0xfc,
    0x3f, 0xf8,
    0x3f, 0xf8,
    0x1f, 0xf0,
    0x1f, 0xf0,
    0x0f, 0xe0,
    0x0f, 0xe0,
    0x07, 0xc0,
    0x07, 0xc0,
    0x03, 0x80,
    0x03, 0x80,
    0x01, 0x00,
    0x01, 0x00,  // Arrow down 15x16

    0x06,
    0x1e,
    0x78,
    0xf8,
    0x78,
    0x1e,
    0x06,  // Arrow left 7x7

    0x03,
    0x0e,
    0x3e,
    0xfc,
    0xfc,
    0x3e,
    0x0e,
    0x03,  // Arrow left 8x8

    0x00, 0x40,
    0x01, 0xc0,
    0x07, 0x80,
    0x1f, 0x80,
    0x7f, 0x00,
    0xff, 0x00,
    0x7f, 0x00,
    0x1f, 0x80,
    0x07, 0x80,
    0x01, 0xc0,
    0x00, 0x40,  // Arrow left 11x10

    0x00, 0x08,
    0x00, 0x38,
    0x00, 0xf0,
    0x03, 0xf0,
    0x0f, 0xe0,
    0x3f, 0xe0,
    0xff, 0xc0,
    0x3f, 0xe0,
    0x0f, 0xe0,
    0x03, 0xf0,
    0x00, 0xf0,
    0x00, 0x38,
    0x00, 0x08,  // Arrow left 13x13

    0x00, 0x03,
    0x00, 0x0e,
    0x00, 0x3e,
    0x00, 0xfc,
    0x03, 0xfc,
    0x0f, 0xf8,
    0x3f, 0xf8,
    0xff, 0xf0,
    0x3f, 0xf8,
    0x0f, 0xf8,
    0x03, 0xfc,
    0x00, 0xfc,
    0x00, 0x3e,
    0x00, 0x0e,
    0x00, 0x03,  // Arrow left 16x15

    0x10,
    0x38,
    0x38,
    0x7c,
    0x7c,
    0xc6,
    0xc6,  // Arrow down 7x7

    0x18,
    0x18,
    0x3c,
    0x3c,
    0x7e,
    0x7e,
    0xe7,
    0x81,  // Arrow down 8x8

    0x04, 0x00,
    0x0e, 0x00,
    0x0e, 0x00,
    0x1f, 0x00,
    0x1f, 0x00,
    0x3f, 0x80,
    0x3f, 0x80,
    0x7f, 0xc0,
    0x71, 0xc0,
    0xc0, 0x60,  // Arrow down 10x11

    0x02, 0x00,
    0x02, 0x00,
    0x07, 0x00,
    0x07, 0x00,
    0x0f, 0x80,
    0x0f, 0x80,
    0x1f, 0xc0,
    0x1f, 0xc0,
    0x3f, 0xe0,
    0x3f, 0xe0,
    0x7d, 0xf0,
    0x70, 0x70,
    0xc0, 0x18,  // Arrow down 13x13

    0x01, 0x00,
    0x01, 0x00,
    0x03, 0x80,
    0x03, 0x80,
    0x07, 0xc0,
    0x07, 0xc0,
    0x0f, 0xe0,
    0x0f, 0xe0,
    0x1f, 0xf0,
    0x1f, 0xf0,
    0x3f, 0xf8,
    0x3f, 0xf8,
    0x7e, 0xfc,
    0x78, 0x3c,
    0xe0, 0x0e,
    0x80, 0x02,  // Arrow down 15x16

    0x08,
    0x08,
    0x18,
    0x1c,
    0x7c,
    0xfc,
    0x1e,
    0x02,  // Arrow up-right 7x8

    0x0c,
    0x0c,
    0x0c,
    0x1e,
    0xfe,
    0xfe,
    0x1f,
    0x03,  // Arrow up-right 8x8

    0x02, 0x00,
    0x03, 0x00,
    0x03, 0x00,
    0x07, 0x00,
    0x07, 0x80,
    0x1f, 0x80,
    0xff, 0x80,
    0x7f, 0xc0,
    0x0f, 0xc0,
    0x01, 0xc0,
    0x00, 0x20,  // Arrow up-right 11x11

    0x00, 0x40,
    0x00, 0x40,
    0x00, 0xc0,
    0x00, 0xe0,
    0x00, 0xe0,
    0x01, 0xe0,
    0x01, 0xf0,
    0x07, 0xf0,
    0x3f, 0xf0,
    0xff, 0xf8,
    0x1f, 0xf8,
    0x03, 0xf8,
    0x00, 0x7c,
    0x00, 0x0c,  // Arrow up-right 14x14

    0x00, 0x20,
    0x00, 0x20,
    0x00, 0x30,
    0x00, 0x70,
    0x00, 0x70,
    0x00, 0x78,
    0x00, 0xf8,
    0x00, 0xf8,
    0x03, 0xfc,
    0x1f, 0xfc,
    0xff, 0xfc,
    0x3f, 0xfe,
    0x0e, 0xfe,
    0x00, 0xfe,
    0x00, 0x1f,
    0x00, 0x03,  // Arrow up-right 16x16

    0x20,
    0x30,
    0x30,
    0x7c,
    0x7f,
    0x78,
    0xc0,  // Arrow up-left 8x7

    0x30,
    0x30,
    0x30,
    0x78,
    0x7f,
    0x7f,
    0xf8,
    0xc0,  // Arrow up-left 8x8

    0x08, 0x00,
    0x18, 0x00,
    0x18, 0x00,
    0x1c, 0x00,
    0x3c, 0x00,
    0x3f, 0x00,
    0x3f, 0xe0,
    0x7f, 0xc0,
    0x7e, 0x00,
    0x70, 0x00,
    0x80, 0x00,  // Arrow up-left 11x11

    0x08, 0x00,
    0x08, 0x00,
    0x0c, 0x00,
    0x1c, 0x00,
    0x1c, 0x00,
    0x1e, 0x00,
    0x3e, 0x00,
    0x3f, 0x80,
    0x3f, 0xf0,
    0x7f, 0xfc,
    0x7f, 0xe0,
    0x7f, 0x00,
    0xf8, 0x00,
    0xc0, 0x00,  // Arrow up-left 14x14

    0x04, 0x00,
    0x04, 0x00,
    0x0c, 0x00,
    0x0e, 0x00,
    0x0e, 0x00,
    0x1e, 0x00,
    0x1f, 0x00,
    0x1f, 0x00,
    0x3f, 0xc0,
    0x3f, 0xf8,
    0x3f, 0xff,
    0x7f, 0xfc,
    0x7f, 0xe0,
    0x7f, 0x00,
    0xf8, 0x00,
    0xc0, 0x00,  // Arrow up-left 16x16

    0x80,
    0xf0,
    0x7e,
    0x7c,
    0x70,
    0x30,
    0x20,
    0x20,  // Arrow down-left 7x8

    0xc0,
    0xf8,
    0x7f,
    0x7f,
    0x78,
    0x30,
    0x30,
    0x30,  // Arrow down-left 8x8

    0x80, 0x00,
    0x70, 0x00,
    0x7e, 0x00,
    0x7f, 0xc0,
    0x3f, 0xe0,
    0x3f, 0x00,
    0x3c, 0x00,
    0x1c, 0x00,
    0x18, 0x00,
    0x18, 0x00,
    0x08, 0x00,  // Arrow down-left 11x11

    0xc0, 0x00,
    0xf8, 0x00,
    0x7f, 0x00,
    0x7f, 0xe0,
    0x7f, 0xfc,
    0x3f, 0xf0,
    0x3f, 0x80,
    0x3e, 0x00,
    0x1e, 0x00,
    0x1c, 0x00,
    0x1c, 0x00,
    0x0c, 0x00,
    0x08, 0x00,
    0x08, 0x00,  // Arrow down-left 14x14

    0xc0, 0x00,
    0xf8, 0x00,
    0x7f, 0x00,
    0x7f, 0xe0,
    0x7f, 0xfc,
    0x3f, 0xff,
    0x3f, 0xf8,
    0x3f, 0xc0,
    0x1f, 0x00,
    0x1f, 0x00,
    0x1e, 0x00,
    0x0e, 0x00,
    0x0e, 0x00,
    0x0c, 0x00,
    0x07, 0x00,
    0x07, 0x00,  // Arrow down-left 16x16

    0x03,
    0x1e,
    0xfe,
    0x3e,
    0x0c,
    0x0c,
    0x04,  // Arrow down-right 8x7

    0x03,
    0x1f,
    0xfe,
    0xfe,
    0x1e,
    0x0c,
    0x0c,
    0x0c,  // Arrow down-right 8x8

    0x00, 0x20,
    0x01, 0xc0,
    0x0f, 0xc0,
    0x7f, 0xc0,
    0xff, 0x80,
    0x1f, 0x80,
    0x07, 0x80,
    0x07, 0x00,
    0x03, 0x00,
    0x03, 0x00,
    0x02, 0x00,  // Arrow down-right 11x11

    0x00, 0x0c,
    0x00, 0x7c,
    0x03, 0xf8,
    0x1f, 0xf8,
    0xff, 0xf8,
    0x3f, 0xf0,
    0x07, 0xf0,
    0x01, 0xf0,
    0x01, 0xe0,
    0x00, 0xe0,
    0x00, 0xe0,
    0x00, 0xc0,
    0x00, 0x40,
    0x00, 0x40,  // Arrow down-right 14x14

    0x00, 0x03,
    0x00, 0x1f,
    0x00, 0xfe,
    0x07, 0xfe,
    0x3f, 0xfe,
    0xff, 0xfc,
    0x1f, 0xfc,
    0x03, 0xfc,
    0x00, 0xf8,
    0x00, 0xf8,
    0x00, 0x78,
    0x00, 0x70,
    0x00, 0x70,
    0x00, 0x30,
    0x00, 0x20,
    0x00, 0x20,  // Arrow down-right 16x16

    0xff, 0xff, 0x80,
    0xff, 0xff, 0x80,
    0xc0, 0x01, 0x80,
    0xc0, 0x01, 0x80,
    0xc0, 0x01, 0x80,
    0xc0, 0x01, 0x80,
    0xc0, 0x01, 0x80,
    0xc0, 0x01, 0x80,
    0xc0, 0x01, 0x80,
    0xc0, 0x01, 0x80,
    0xc0, 0x01, 0x80,
    0xc0, 0x01, 0x80,
    0xc0, 0x01, 0x80,
    0xc0, 0x01, 0x80,
    0xc0, 0x01, 0x80,
    0xff, 0xff, 0x80,
    0xff, 0xff, 0x80,  // Square 17x17

    0x00,0x00,0x00,
    0xff,0xff,0x80,
    0xff,0xff,0x80,
    0xc0,0x01,0x80,
    0x60,0x03,0x00,
    0x70,0x07,0x00,
    0x38,0x0e,0x00,
    0x18,0x0c,0x00,
    0x1c,0x1c,0x00,
    0x0c,0x18,0x00,
    0x06,0x30,0x00,
    0x07,0x70,0x00,
    0x03,0x60,0x00,
    0x01,0xc0,0x00,
    0x01,0xc0,0x00,  // Triangle 17x15

    0x01,0xf0,0x00,
    0x07,0xfc,0x00,
    0x1e,0x0f,0x00,
    0x38,0x03,0x80,
    0x30,0x01,0x80,
    0x60,0x00,0xc0,
    0x60,0x00,0xc0,
    0xc0,0x00,0x60,
    0xc0,0x00,0x60,
    0xc0,0x00,0x60,
    0xc0,0x00,0x60,
    0xc0,0x00,0x60,
    0x60,0x00,0xc0,
    0x60,0x00,0xc0,
    0x30,0x01,0x80,
    0x38,0x03,0x80,
    0x1e,0x0f,0x00,
    0x07,0xfc,0x00,
    0x01,0xf0,0x00,  // Circle bold 19x19

    0xc0,0x01,0x80,
    0x60,0x03,0x00,
    0x30,0x06,0x00,
    0x18,0x0c,0x00,
    0x0c,0x18,0x00,
    0x06,0x30,0x00,
    0x03,0x60,0x00,
    0x01,0xc0,0x00,
    0x01,0xc0,0x00,
    0x03,0x60,0x00,
    0x06,0x30,0x00,
    0x0c,0x18,0x00,
    0x18,0x0c,0x00,
    0x30,0x06,0x00,
    0x60,0x03,0x00,
    0xc0,0x01,0x80,
    0x80,0x00,0x80,  // Cross bold 17x17

    0x00, 0x40, 0x00,
    0x00, 0xe0, 0x00,
    0x01, 0xb0, 0x00,
    0x03, 0x18, 0x00,
    0x06, 0x0c, 0x00,
    0x0c, 0x06, 0x00,
    0x18, 0x03, 0x00,
    0x30, 0x01, 0x80,
    0x60, 0x00, 0xc0,
    0xc0, 0x00, 0x60,
    0x60, 0x00, 0xc0,
    0x30, 0x01, 0x80,
    0x18, 0x03, 0x00,
    0x0c, 0x06, 0x00,
    0x06, 0x0c, 0x00,
    0x03, 0x18, 0x00,
    0x01, 0xb0, 0x00,
    0x00, 0xe0, 0x00,
    0x00, 0x40, 0x00,  // Romb 19x19

    0x01,0xf0,0x00,
    0x07,0xfc,0x00,
    0x1e,0x4f,0x00,
    0x38,0x43,0x80,
    0x30,0x41,0x80,
    0x60,0x40,0xc0,
    0x60,0x40,0xc0,
    0xc0,0x40,0x60,
    0xc0,0x40,0x60,
    0xff,0xff,0xe0,
    0xc0,0x40,0x60,
    0xc0,0x40,0x60,
    0x60,0x40,0xc0,
    0x60,0x40,0xc0,
    0x30,0x41,0x80,
    0x38,0x43,0x80,
    0x1e,0x4f,0x00,
    0x07,0xfc,0x00,
    0x01,0xf0,0x00,  // Cross circle 19x19

    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x38, 0x00,
    0x00, 0x38, 0x00,
    0x00, 0x38, 0x00,
    0x00, 0x38, 0x00,
    0x00, 0x38, 0x00,
    0x00, 0x38, 0x00,
    0x00, 0x38, 0x00,
    0x00, 0x38, 0x00,
    0x00, 0x38, 0x00,
    0x00, 0x38, 0x00,
    0x00, 0x38, 0x00,
    0x00, 0x38, 0x00,
    0x00, 0x38, 0x00,
    0x00, 0x38, 0x00,
    0x00, 0x38, 0x00,
    0x00, 0x38, 0x00,
    0x00, 0x38, 0x00,
    0x00, 0x38, 0x00,
    0x00, 0x38, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00  //Line 3x19
};

//=======================================================================
//function : GenBitmap
//purpose  : 
//=======================================================================
static void GenBitmap(const unsigned char*   theOldBitmap,
                      unsigned char*         theNewBitmap,
                      const Standard_Integer theWidth,
                      const Standard_Integer theHeight,
                      const gp_Pnt2d&        theCenter,
                      const Standard_Real    theAngle)
{
    Standard_Integer aWidthBitmap = (Standard_Integer)Ceiling(theWidth / 8.0);
    Standard_Integer aHeightBitmap = theHeight;
    memset (theNewBitmap, 0, aWidthBitmap * aHeightBitmap);
    // Compute rotate transformation
    gp_Trsf2d aTrsf;
    aTrsf.SetRotation(theCenter, theAngle);
    for (Standard_Integer anIterY = 0; anIterY < aHeightBitmap; anIterY++)
    {
        for (Standard_Integer anIterX = 0; anIterX < aWidthBitmap; anIterX++)
        {
            unsigned char aByte = theOldBitmap[anIterY * aWidthBitmap + anIterX];
            for (Standard_Integer anIterBit = 0; anIterBit < 8; anIterBit++)
            {
                if (aByte & (1<<anIterBit))
                {
                    // Rotate each bit in bitmap
                    gp_Pnt2d aPos(anIterX * 8 + (7 - anIterBit), anIterY);
                    aPos.Transform(aTrsf.Inverted());
                    // Round the computed position to integer
                    aPos.SetXY(gp_XY(Round(aPos.X()), Round(aPos.Y())));

                    unsigned char aNewByte = theNewBitmap[(Standard_Integer)aPos.Y() * aWidthBitmap +
                                                          (Standard_Integer)aPos.X() / 8];
                    aNewByte |= 1<<(7 - (Standard_Integer)aPos.X() % 8);
                    theNewBitmap[(Standard_Integer)aPos.Y() * aWidthBitmap +
                                 (Standard_Integer)aPos.X() / 8] = aNewByte;
                }
            }
        }
    }
}


//=======================================================================


IMPLEMENT_STANDARD_HANDLE  (NIS_PointDrawer, NIS_Drawer)
IMPLEMENT_STANDARD_RTTIEXT (NIS_PointDrawer, NIS_Drawer)

//=======================================================================
//function : NIS_PointDrawer
//purpose  : Constructor
//=======================================================================

NIS_PointDrawer::NIS_PointDrawer(const Quantity_Color& theColor)
  : myPointSize(5.f),
    myType(0),
    myInvertColor(Standard_False),
    myIsWhite(Standard_False),
    myAngle(0.0),
    myIsUpdateAngle(Standard_False),
    myRotatedRaster(NULL)
{
    SetColor(Draw_Normal, theColor);
    SetColor(Draw_Hilighted, Quantity_NOC_GRAY80);
    SetColor(Draw_DynHilighted, Quantity_NOC_CYAN1);
}

//=======================================================================
//function : ~NIS_PointDrawer
//purpose  : Destructor
//=======================================================================

NIS_PointDrawer::~NIS_PointDrawer()
{
    if (myRotatedRaster)
    {
        delete [] myRotatedRaster;
        myRotatedRaster = NULL;
    }
}

//=======================================================================
//function : Assign
//purpose  :
//=======================================================================

void NIS_PointDrawer::Assign (const Handle_NIS_Drawer& theOther)
{
    if (theOther.IsNull() == Standard_False)
    {
        NIS_Drawer::Assign(theOther);
        const Handle(NIS_PointDrawer)& anOther =
            static_cast <const Handle(NIS_PointDrawer)&> (theOther);
        myPointSize = anOther->myPointSize;
        myType = anOther->myType;
        myInvertColor = anOther->myInvertColor;
        myIsWhite = anOther->myIsWhite;
        myAngle = anOther->myAngle;
        myIsUpdateAngle = anOther->myIsUpdateAngle;
    }
}

//=======================================================================
//function : IsEqual
//purpose  : Comparison of two Drawers (for Map impementation)
//=======================================================================

Standard_Boolean NIS_PointDrawer::IsEqual
                                (const Handle_NIS_Drawer& theOther) const
{
    static const Standard_Real anEpsilon2 (1e-7);
    Standard_Boolean aResult (Standard_False);
    const Handle(NIS_PointDrawer) anOther =
        Handle(NIS_PointDrawer)::DownCast(theOther);
    if (NIS_Drawer::IsEqual(theOther))
    {
        aResult = ((anOther->myPointSize - myPointSize) *
                   (anOther->myPointSize - myPointSize) < 0.01 &&
                   anOther->myType == myType &&
                   anOther->myInvertColor == myInvertColor &&
                   anOther->myIsWhite == myIsWhite &&
                   anOther->myAngle == myAngle &&
                   anOther->myIsUpdateAngle == myIsUpdateAngle);
    }
    return aResult;
}

//=======================================================================
//function : BeforeDraw
//purpose  :
//=======================================================================

void NIS_PointDrawer::BeforeDraw (const DrawType         theType,
                                  const NIS_DrawList&    /*theDrawList*/)
{
    Quantity_Parameter   aValue[4];
    aValue[3] = 1.;                               // opaque as default
    Quantity_TypeOfColor bidTC (Quantity_TOC_RGB);
    GLfloat aPointSize (myPointSize);

    if (theType == Draw_DynHilighted)
    {
        aPointSize = myPointSize + 2.f;
    }

    if (myInvertColor)
    {
        glEnable(GL_COLOR_LOGIC_OP);
        glLogicOp(GL_XOR);
    }
    else if (theType == Draw_Transparent)
    {
        aValue[3] = 1. - myTransparency;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    GetColor(theType).Values(aValue[0], aValue[1], aValue[2], bidTC);

    // It's necessary for snap marker visualization to identify 
    // whether the input color is white, because XOR mode for white 
    // color is processed differently.
    // Note that white color is represented as (1., 1., 1.)
    if ( Abs(aValue[0] - 1.) <= Precision::Confusion() &&
         Abs(aValue[1] - 1.) <= Precision::Confusion() &&
         Abs(aValue[2] - 1.) <= Precision::Confusion())
    {
        myIsWhite = Standard_True;
    }

    glColor4d (aValue[0], aValue[1], aValue[2], aValue[3]);
    //glEnableClientState (GL_VERTEX_ARRAY);
    glDisable(GL_LIGHTING);

    if (myType > 0 && myType <= NUM_TYPES)
    {
        mySizeIndex = 0;
        for (int j = 0; j < NUM_SIZES; j++, mySizeIndex++)
        {
            if (aPointSize <= data[myType-1][j].height)
            {
                break;
            }
        }
        if (mySizeIndex >= NUM_SIZES)
        {
            mySizeIndex = NUM_SIZES - 1;
        }
    }
    else
    {
        glPointSize(aPointSize);
    }
}

//=======================================================================
//function : AfterDraw
//purpose  :
//=======================================================================

void NIS_PointDrawer::AfterDraw (const DrawType         theType,
                                 const NIS_DrawList&    /*theDrawList*/)
{
    if (myInvertColor)
    {
        glDisable(GL_COLOR_LOGIC_OP);
    }
    else if (theType == Draw_Transparent)
    {
        glDisable(GL_BLEND);
    }
    //glDisableClientState(GL_VERTEX_ARRAY);
}

//=======================================================================
//function : Draw
//purpose  :
//=======================================================================

void NIS_PointDrawer::Draw (const Handle_NIS_InteractiveObject& theObj,
                            const DrawType                      /*theType*/,
                            const NIS_DrawList&                 /*theDrawList*/)
{
    // Assertion for the type of the drawn object
#ifdef _DEBUG
    static const Handle(Standard_Type) ThisType =
        STANDARD_TYPE(NIS_Point);
    if (theObj->IsKind(ThisType) == Standard_False)
    {
        Standard_ProgramError::Raise("NIS_PointDrawer::Draw: "
                                     "irrelevant object type");
    }
#endif
    const NIS_Point* pObject = 
        static_cast <const NIS_Point*> (theObj.operator->());

    const Standard_ShortReal* aCoords = &pObject->myCoords[0];

    // Draw single point (using bitmap)
    if (myType > 0 && myType <= NUM_TYPES)
    {
        if (myIsWhite == Standard_False)
        {
            // Simple blending
            glDisable (GL_LIGHTING);
            glEnable (GL_BLEND);
            glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);

            DrawPoint ( aCoords );

            glDisable (GL_BLEND);
            glEnable (GL_LIGHTING);
        }
        else
        {
            // White color invertion should be processed differntly

            // Invert color
            glEnable(GL_COLOR_LOGIC_OP);
            glLogicOp(GL_INVERT);
            // Draw point
            DrawPoint ( aCoords );

            glDisable(GL_COLOR_LOGIC_OP);

            // Discret color
            glColor3ub (224, 224, 224);
            glEnable(GL_COLOR_LOGIC_OP);
            glLogicOp(GL_AND);

            // Draw point
            DrawPoint ( aCoords );

            glDisable(GL_COLOR_LOGIC_OP);

            // Scale up
            glEnable(GL_BLEND);
            glColor3f(1.f, 1.f, 1.f);
            glBlendFunc(GL_DST_COLOR, GL_ONE);

            // Draw point
            DrawPoint ( aCoords );

            // Scale up again
            // Draw point
            DrawPoint ( aCoords );

            // Draw point
            DrawPoint ( aCoords );
            glDisable(GL_BLEND);
        }
    }
    else
    {
        glBegin(GL_POINTS);
        glVertex3fv(aCoords);
        glEnd();
    }
}

//=======================================================================
//function : DrawPoint
//purpose  :
//=======================================================================

void NIS_PointDrawer::DrawPoint (const Standard_ShortReal* thePosition)
{
    glRasterPos3f((GLfloat)(thePosition[0]),
                  (GLfloat)(thePosition[1]),
                  (GLfloat)(thePosition[2]) );
    int aWidth = data[myType-1][mySizeIndex].width;
    int aHeight = data[myType-1][mySizeIndex].height;
    const Standard_Size aNBytes = sizeof(unsigned char) *
                                 (Standard_Integer)Ceiling(aWidth / 8.0) * aHeight;

    if (std::abs(myAngle) < Precision::Confusion())
    {
        myRotatedRaster = new unsigned char[aNBytes];
        memcpy(myRotatedRaster, &raster[data[myType-1][mySizeIndex].offset], aNBytes);
    }
    else if (myIsUpdateAngle)
    {
        if (myRotatedRaster != NULL)
        {
            delete[] myRotatedRaster;
            myRotatedRaster = NULL;
        }
        // To generate the new rotated bitmap, the old bitmap should be square.
        myRotatedRaster = new unsigned char[aNBytes];
        GenBitmap((const GLubyte*) &raster[data[myType-1][mySizeIndex].offset],
                  myRotatedRaster, aWidth, aHeight, gp_Pnt2d(aWidth/2, aHeight/2), myAngle);
    }
    myIsUpdateAngle = Standard_False;
    glBitmap((GLsizei)aWidth, (GLsizei)aHeight,
             (GLfloat)aWidth/2, (GLfloat)aHeight/2, 0., 0.,
             (const GLubyte*) myRotatedRaster);
}
