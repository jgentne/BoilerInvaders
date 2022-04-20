#include "stm32f0xx.h"
#include <stdint.h>
#include <stdlib.h>
#include "lcd.h"
#include "midi.h"
#include "midiplay.h"

static void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}


// Copy a subset of a large source picture into a smaller destination.
// sx,sy are the offset into the source picture.
void pic_subset(Picture *dst, const Picture *src, int sx, int sy)
{
    int dw = dst->width;
    int dh = dst->height;
    for(int y=0; y<dh; y++) {
        if (y+sy < 0)
            continue;
        if (y+sy >= src->height)
            break;
        for(int x=0; x<dw; x++) {
            if (x+sx < 0)
                continue;
            if (x+sx >= src->width)
                break;
            dst->pix2[dw * y + x] = src->pix2[src->width * (y+sy) + x + sx];
        }
    }
}

// Overlay a picture onto a destination picture.
// xoffset,yoffset are the offset into the destination picture that the
// source picture is placed.
// Any pixel in the source that is the 'transparent' color will not be
// copied.  This defines a color in the source that can be used as a
// transparent color.
void pic_overlay(Picture *dst, int xoffset, int yoffset, const Picture *src, int transparent)
{
    for(int y=0; y<src->height; y++) {
        int dy = y+yoffset;
        if (dy < 0)
            continue;
        if (dy >= dst->height)
            break;
        for(int x=0; x<src->width; x++) {
            int dx = x+xoffset;
            if (dx < 0)
                continue;
            if (dx >= dst->width)
                break;
            unsigned short int p = src->pix2[y*src->width + x];
            if (p != transparent)
                dst->pix2[dy*dst->width + dx] = p;
        }
    }
}

extern const Picture background; // A 240x320 background image
extern const Picture player;
extern const Picture goodBullet;
extern const Picture bulletShadow;
extern const Picture badGuy;

// This C macro will create an array of Picture elements.
// Really, you'll just use it as a pointer to a single Picture
// element with an internal pix2[] array large enough to hold
// an image of the specified size.
// BE CAREFUL HOW LARGE OF A PICTURE YOU TRY TO CREATE:
// A 100x100 picture uses 20000 bytes.  You have 32768 bytes of SRAM.
#define TempPicturePtr(name,width,height) Picture name[(width)*(height)/6+2] = { {width,height,2} }

void erase(int x, int y, int value)
{
    if(value ==0){
    TempPicturePtr(tmp,29,29); // Create a temporary 29x29 image.
    pic_subset(tmp, &background, x-tmp->width/2, y-tmp->height/2); // Copy the background
    //pic_overlay(tmp, 5,5, &player, 0xffff); // Overlay the ball
    LCD_DrawPicture(x-tmp->width/2,y-tmp->height/2, tmp); // Draw
    }
    else if(value == 1){
        TempPicturePtr(tmp,12,18); // Create a temporary 29x29 image.
            pic_subset(tmp, &background, x-tmp->width/2, y-tmp->height/2); // Copy the background
            //pic_overlay(tmp, 5,5, &player, 0xffff); // Overlay the ball
            LCD_DrawPicture(x-tmp->width/2,y-tmp->height/2, tmp); // Draw
    }

}

void update(int x, int y, int Value)
{
    if (x < 228 && x > 15 && Value == 2) {
        LCD_DrawPicture(x-player.width/2,y-player.height/2, &player); // Draw the player
    }
    else if(Value == 1) {
        LCD_DrawPicture(x-goodBullet.width/2,y-goodBullet.height/2, &goodBullet);
    }
    else if(Value == 0) {
        LCD_DrawPicture(x-bulletShadow.width/2,(y)-bulletShadow.height/2, &bulletShadow);
        }
    else if(Value == 3){
        LCD_DrawPicture(x-badGuy.width/2,(y)-badGuy.height/2, &badGuy);
    }
}

void update2(int x, int y)
{
    TempPicturePtr(tmp,29,29); // Create a temporary 29x29 image.
    pic_subset(tmp, &background, x-tmp->width/2, y-tmp->height/2); // Copy the background
    pic_overlay(tmp, 5,5, &player, 0xffff); // Overlay the ball
    LCD_DrawPicture(x-tmp->width/2,y-tmp->height/2, tmp); // Draw
}

void rocketMan(void)
{
    // Draw the background.
    MIDI_Player *mp = midi_init(midifile);
    init_tim2(10417);

    int x = 120;
    int y = 22;

    int right, left, shootah, gbX, gbY;
    int gbCheck = 0;
    int bg1, bg2, bg3, bg4, bg5, bg6, bg7 = 1;

    int bgX1 = 90;
    int bgY1 = 280; //initializing bad guy 1

    int bgX2 = 150;
    int bgY2 = 280; //initializing bad guy 2

    int bgX3 = 210;
    int bgY3 = 280; //initializing bad guy 3

    int bgX4 = 60;
    int bgY4 = 295; //initializing bad guy 4

    int bgX5 = 120;
    int bgY5 = 295; //initializing bad guy 5

    int bgX6 = 180;
    int bgY6 = 295; //initializing bad guy 6

    int bgX7 = 30;
    int bgY7 = 280; //initializing bad guy 7

    int alternate = 1;
    int justShot = 0;

    int MAX = 6400;
    for(;;) {
        for(int z=0; z<4; z++)
            {
            nano_wait(2500000); // wait
            right = GPIOC->IDR & 1<<6;
            left = GPIOC->IDR & 1<<7;
            shootah = GPIOC->IDR & 1<<8;

            if((shootah && 1<<8) && (gbCheck != 1)) // initializing good bullet
            {
                gbCheck = 1;
                gbX = x;
                gbY = 30;
            }

            if(gbCheck) //moving good bullet along
            {
                update(gbX-2, gbY + 15, 1);//bullet
                update(gbX-2, gbY+2, 0);//shadow
                gbY += 1;
            }

            if (right && 1<<7)
            {
                //nano_wait(5000000);
                x++;
                update(x, y, 2);
            }
            else if(left && 1<<6)
            {
                //nano_wait(5000000);
                x--;
                update(x, y, 2);
            }
            gbCheck = gbCheckVal(gbY);

            if(bg1)
            {
                moveBadGuys(&bgX1, &bgY1, alternate);
                bg1 = bgCheck(bgX1, bgY1, gbX, gbY, &gbCheck);
                if(!bg1){
                    gbCheck = 0;
                    while(gbY<325){
                        update(gbX-2, ++gbY, 0);
                    }
                }
            }

            if(bg2)
            {
                moveBadGuys(&bgX2, &bgY2, alternate);
                bg2 = bgCheck(bgX2, bgY2, gbX, gbY, &gbCheck);
                if(!bg2){
                    gbCheck = 0;
                    while(gbY<325){
                        update(gbX-2, ++gbY, 0);
                    }
                }
            }

            if(bg3)
            {
                moveBadGuys(&bgX3, &bgY3, alternate);
                bg3 = bgCheck(bgX3, bgY3, gbX, gbY, &gbCheck);
                if(!bg3){
                    gbCheck = 0;
                    while(gbY<325){
                        update(gbX-2, ++gbY, 0);
                    }
                }
            }

            if(bg4)
            {
                moveBadGuys(&bgX4, &bgY4, alternate);
                bg4 = bgCheck(bgX4, bgY4, gbX, gbY, &gbCheck);
                if(!bg4){
                    gbCheck = 0;
                    while(gbY<325){
                        update(gbX-2, ++gbY, 0);
                    }
                }
            }
            if(bg5)
            {
                moveBadGuys(&bgX5, &bgY5, alternate);
                bg5 = bgCheck(bgX5, bgY5, gbX, gbY, &gbCheck);
                if(!bg5){
                    gbCheck = 0;
                    while(gbY<325){
                        update(gbX-2, ++gbY, 0);
                    }
                }
            }

            if(bg6)
            {
                moveBadGuys(&bgX6, &bgY6, alternate);
                bg6 = bgCheck(bgX6, bgY6, gbX, gbY, &gbCheck);
                if(!bg6){
                    gbCheck = 0;
                    while(gbY<325){
                        update(gbX-2, ++gbY, 0);
                    }
                }
            }

            if(bg7)
            {
                moveBadGuys(&bgX7, &bgY7, alternate);
                bg7 = bgCheck(bgX7, bgY7, gbX, gbY, &gbCheck);
                if(!bg7){
                    gbCheck = 0;
                    while(gbY<325){
                        update(gbX-2, ++gbY, 0);
                    }
                }
            }

            alternate = altInc(alternate);

            asm("wfi");
            if (mp->nexttick == MAXTICKS) {
            mp = midi_init(midifile);
            }
        }
    }
}

int bgCheck(int, int, int, int, int*);

int bgCheck(int bgX, int bgY, int gbX, int gbY, int* gbCheck) {
    if(gbX > (bgX - 7) && gbX < (bgX + 7)) //if bgX-5 < gbX < bgX + 5
        {
        if(gbY > (bgY - 15) && gbY < (bgY)) {
            erase(bgX, bgY, 1);
            *gbCheck = 0;
            return 0;
        }
        }


    return 1;
}

int altInc(int inc) {
    nano_wait(1000);
    inc++;
    if(inc == 6400)
    {
       inc = 0;
    }
    return inc;
}

int gbCheckVal(int val) {
    if(val > 325)
    {
        return 0;
    }
    return 1;
}

void moveBadGuys(int *x, int *y, int alternate, int MAX)
{
//    nano_wait(1000);
//    erase(*x,*y,1);

    switch(alternate)
    {
    case 100:
        *y += 2;
        update(*x,*y,3);
        break;
    case 200:
        *x += 2;
        update(*x,*y,3);
        break;
    case 300:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 400:
        *x += 2;
        update(*x,*y,3);
        break;
    case 500:
        *y += 2;
        update(*x,*y,3);
        break;
    case 600:
        *x += 2;
        update(*x,*y,3);
        break;
    case 700:
        *y -= 2;
        update(*x,*y,3);
        break;
    case 800:
        *x += 2;
        update(*x,*y,3);
        break;
    case (900):
        *y += 2;
        update(*x,*y,3);
        break;
    case 1000:
        *x += 2;
        update(*x,*y,3);
        break;
    case 1100:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 1200:
        *x += 2;
        update(*x,*y,3);
      break;
    case 1300:
        *y += 2;
        update(*x,*y,3);
      break;
    case 1400:
        *x += 2;
        update(*x,*y,3);
      break;
    case 1500:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 1600:
        *x += 2;
        update(*x,*y,3);
      break;
    case 1700:
        *y += 2;
        update(*x,*y,3);
      break;
    case 1800:
        *x -= 2;
        update(*x,*y,3);
        break;
    case 1900:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 2000:
        *x -= 2;
        update(*x,*y,3);
        break;
    case 2100:
        *y += 2;
        update(*x,*y,3);
      break;
    case 2200:
        *x -= 2;
        update(*x,*y,3);
      break;
    case 2300:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 2400:
        *x -= 2;
        update(*x,*y,3);
        break;
    case 2500:
        *y += 2;
        update(*x,*y,3);
      break;
    case 2600:
        *x -= 2;
        update(*x,*y,3);
      break;
    case 2700:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 2800:
        *x -= 2;
        update(*x,*y,3);
      break;
    case 2900:
        *y += 2;
        update(*x,*y,3);
      break;
    case 3000:
        *x -= 2;
        update(*x,*y,3);
      break;
    case 3100:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 3200:
        *x -= 2;
        update(*x,*y,3);
      break;
    case 3300:
        *y += 2;
        update(*x,*y,3);
      break;
    case 3400:
        *x -= 2;
        update(*x,*y,3);
        break;
    case 3500:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 3600:
        *x -= 2;
        update(*x,*y,3);
        break;
    case 3700:
        *y += 2;
        update(*x,*y,3);
      break;
    case 3800:
        *x -= 2;
        update(*x,*y,3);
      break;
    case 3900:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 4000:
        *x -= 2;
        update(*x,*y,3);
        break;
    case 4100:
        *y += 2;
        update(*x,*y,3);
      break;
    case 4200:
        *x -= 2;
        update(*x,*y,3);
      break;
    case 4300:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 4400:
        *x -= 2;
        update(*x,*y,3);
      break;
    case 4500:
        *y += 2;
        update(*x,*y,3);
      break;
    case 4600:
        *x -= 2;
        update(*x,*y,3);
      break;
    case 4700:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 4800:
        *x -= 2;
        update(*x,*y,3);
      break;
    case 4900:
        *y += 2;
        update(*x,*y,3);
        break;
    case 5000:
        *x += 2;
        update(*x,*y,3);
        break;
    case 5100:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 5200:
        *x += 2;
        update(*x,*y,3);
        break;
    case 5300:
        *y += 2;
        update(*x,*y,3);
        break;
    case 5400:
        *x += 2;
        update(*x,*y,3);
        break;
    case 5500:
        *y -= 2;
        update(*x,*y,3);
        break;
    case 5600:
        *x += 2;
        update(*x,*y,3);
        break;
    case 5700:
        *y += 2;
        update(*x,*y,3);
        break;
    case 5800:
        *x += 2;
        update(*x,*y,3);
        break;
    case 5900:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 6000:
        *x += 2;
        update(*x,*y,3);
      break;
    case 6100:
        *y += 2;
        update(*x,*y,3);
      break;
    case 6200:
        *x += 2;
        update(*x,*y,3);
      break;
    case 6300:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 0:
        *x += 2;
        update(*x,*y,3);
        break;  }
//nano_wait(1000);
//update(*x,*y,3);
}

void generateGame(void) {
    LCD_DrawPicture(0,0,&background);

    update(120,22,2); //initializing spaceship

    update(90, 280, 3); //initializing bad guys
    update(150, 280, 3);
    update(210, 280, 3);
    update(60, 295, 3); //initializing bad guys
    update(120, 295, 3);
    update(180, 295, 3);

}


void basic_drawing(void)
{
    LCD_Clear(0);
    LCD_DrawRectangle(10, 10, 30, 50, GREEN);
    LCD_DrawFillRectangle(50, 10, 70, 50, BLUE);
    LCD_DrawLine(10, 10, 70, 50, RED);
    LCD_Circle(50, 90, 40, 1, CYAN);
    LCD_DrawTriangle(90,10, 120,10, 90,30, YELLOW);
    LCD_DrawFillTriangle(90,90, 120,120, 90,120, GRAY);
    LCD_DrawFillRectangle(10, 140, 120, 159, WHITE);
    LCD_DrawString(20,141, BLACK, WHITE, "Test string!", 16, 0); // opaque background
}

