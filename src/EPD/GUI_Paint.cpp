/*****************************************************************************
 * | File      	:   GUI_Paint.c
 * | Author      :   Waveshare team
 * | Function    :	Achieve drawing: draw points, lines, boxes, circles and
 *                   their size, solid dotted line, solid rectangle hollow
 *                   rectangle, solid circle hollow circle.
 * | Info        :
 *   Achieve display characters: Display a single character, string, number
 *   Achieve time display: adaptive size display time minutes and seconds
 *----------------
 * |	This version:   V2.0
 * | Date        :   2018-11-15
 * | Info        :
 * 1.add: Paint_NewImage()
 *    Create an image's properties
 * 2.add: Paint_SelectImage()
 *    Select the picture to be drawn
 * 3.add: Paint_SetRotate()
 *    Set the direction of the cache
 * 4.add: Paint_RotateImage()
 *    Can flip the picture, Support 0-360 degrees,
 *    but only 90.180.270 rotation is better
 * 4.add: Paint_SetMirroring()
 *    Can Mirroring the picture, horizontal, vertical, origin
 * 5.add: Paint_DrawString_CN()
 *    Can display Chinese(GB1312)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 ******************************************************************************/
#include "GUI_Paint.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h> //memset()
#include "DEV_EPD_Config.h"
#include "Debug.h"

volatile PAINT Paint;

GUI_Paint::GUI_Paint()
{
  H_Logger->debug("GUI_Paint::GUI_Paint()");
}

/******************************************************************************
function:	Create Image
parameter:
    image   :   Pointer to the image cache
    width   :   The width of the picture
    Height  :   The height of the picture
    Color   :   Whether the picture is inverted
******************************************************************************/
void GUI_Paint::Paint_NewImage(UBYTE *image, UWORD Width, UWORD Height, UWORD Rotate, UWORD Color)
{
  H_Logger->trace("GUI_Paint::Paint_NewImage()");
  Paint.Image = NULL;
  Paint.Image = image;

  Paint.WidthMemory = Width;
  Paint.HeightMemory = Height;
  Paint.Color = Color;
  Paint.WidthByte = (Width % 8 == 0) ? (Width / 8) : (Width / 8 + 1);
  Paint.HeightByte = Height;
  printf("WidthByte = %d, HeightByte = %d\r\n", Paint.WidthByte, Paint.HeightByte);
  printf(" EPD_WIDTH / 8 = %d\r\n", 122 / 8);

  Paint.Rotate = Rotate;
  H_Logger->trace("GUI_Paint::Paint_NewImage() Paint.Mirror");
  Paint.Mirror = MIRROR_NONE;

  H_Logger->trace("GUI_Paint::Paint_NewImage() Rotate");

  if (Rotate == ROTATE_0 || Rotate == ROTATE_180)
  {
    Paint.Width = Width;
    Paint.Height = Height;
  }
  else
  {
    Paint.Width = Height;
    Paint.Height = Width;
  }
}

/******************************************************************************
function:	Select Image
parameter:
    image   :   Pointer to the image cache
******************************************************************************/
void GUI_Paint::Paint_SelectImage(UBYTE *image)
{
  H_Logger->trace("GUI_Paint::Paint_SelectImage()");
  Paint.Image = image;
}

/******************************************************************************
function:	Select Image Rotate
parameter:
    Rotate   :   0,90,180,270
******************************************************************************/
void GUI_Paint::Paint_SetRotate(UWORD Rotate)
{
  H_Logger->trace("GUI_Paint::Paint_SetRotate()");
  if (Rotate == ROTATE_0 || Rotate == ROTATE_90 || Rotate == ROTATE_180 || Rotate == ROTATE_270)
  {
    Debug("Set image Rotate %d\r\n", Rotate);
    Paint.Rotate = Rotate;
  }
  else
  {
    Debug("rotate = 0, 90, 180, 270\r\n");
    exit(0);
  }
}

/******************************************************************************
function:	Select Image mirror
parameter:
    mirror   :       Not mirror,Horizontal mirror,Vertical mirror,Origin mirror
******************************************************************************/
void GUI_Paint::Paint_SetMirroring(UBYTE mirror)
{
  H_Logger->trace("GUI_Paint::Paint_SetMirroring()");
  if (mirror == MIRROR_NONE || mirror == MIRROR_HORIZONTAL || mirror == MIRROR_VERTICAL || mirror == MIRROR_ORIGIN)
  {
    Debug("mirror image x:%s, y:%s\r\n", (mirror & 0x01) ? "mirror" : "none",
          ((mirror >> 1) & 0x01) ? "mirror" : "none");
    Paint.Mirror = mirror;
  }
  else
  {
    Debug(
        "mirror should be MIRROR_NONE, MIRROR_HORIZONTAL, \
        MIRROR_VERTICAL or MIRROR_ORIGIN\r\n");
    exit(0);
  }
}

/******************************************************************************
function:	Draw Pixels
parameter:
    Xpoint  :   At point X
    Ypoint  :   At point Y
    Color   :   Painted colors
******************************************************************************/
void GUI_Paint::Paint_SetPixel(UWORD Xpoint, UWORD Ypoint, UWORD Color)
{
  // H_Logger->trace("GUI_Paint::Paint_SetPixel()");
  if (Xpoint > Paint.Width || Ypoint > Paint.Height)
  {
    // Debug("Exceeding display boundaries\r\n");
    H_Logger->error("Exceeding display boundaries Xpoint:{} > Paint.Width:{} || Ypoint:{} > Paint.Height:{} ", Xpoint,
                    Paint.Width, Ypoint, Paint.Height);
    return;
  }
  UWORD X, Y;

  // H_Logger->trace(" switch (Paint.Rotate)");
  switch (Paint.Rotate)
  {
      // #if (MIRROR_IMAGE_DFT && MIRROR_NONE)
    case 0:
      X = Xpoint;
      Y = Ypoint;
      break;
    case 90:
      X = Paint.WidthMemory - Ypoint - 1;
      Y = Xpoint;
      break;
    case 180:
      X = Paint.WidthMemory - Xpoint - 1;
      Y = Paint.HeightMemory - Ypoint - 1;
      break;
    case 270:
      X = Ypoint;
      Y = Paint.HeightMemory - Xpoint - 1;
      break;
      // #elif (MIRROR_IMAGE_DFT && MIRROR_HORIZONTAL)
      // case 0:
      // X = Paint.WidthMemory - Xpoint - 1;
      // Y = Ypoint;
      // break;
      // case 90:
      // X = Ypoint;
      // Y = Xpoint;
      // break;
      // case 180:
      // X = Xpoint;
      // Y = Paint.HeightMemory - Ypoint - 1;
      // break;
      // case 270:
      // X = Paint.WidthMemory - Ypoint - 1;
      // Y = Paint.HeightMemory - Xpoint - 1;
      // break;
      // #elif (MIRROR_IMAGE_DFT && MIRROR_VERTICAL)
      // case 0:
      // X = Xpoint;
      // Y = Paint.HeightMemory - Ypoint;
      // break;
      // case 90:
      // X = Paint.WidthMemory - Ypoint - 1;
      // Y = Paint.HeightMemory - Xpoint;
      // break;
      // case 180:
      // X = Paint.WidthMemory - Xpoint - 1;
      // Y = Ypoint;
      // break;
      // case 270:
      // X = Ypoint;
      // Y = Xpoint;
      // break;
      // #endif
    default: return;
  }
  // H_Logger->trace("X:{},Y:{}", X, Y);
  // H_Logger->trace(" switch (Paint.Mirror)");

  switch (Paint.Mirror)
  {
    case MIRROR_NONE: break;
    case MIRROR_HORIZONTAL: X = Paint.WidthMemory - X - 1; break;
    case MIRROR_VERTICAL: Y = Paint.HeightMemory - Y - 1; break;
    case MIRROR_ORIGIN:
      X = Paint.WidthMemory - X - 1;
      Y = Paint.HeightMemory - Y - 1;
      break;
    default: return;
  }

  // H_Logger->trace("X > Paint.WidthMemory || Y > Paint.HeightMemory");
  // printf("x = %d, y = %d\r\n", X, Y);
  if (X > Paint.WidthMemory || Y > Paint.HeightMemory)
  {
    Debug("Exceeding display boundaries\r\n");
    return;
  }
  // H_Logger->trace("X:{},Y:{}", X, Y);
  // H_Logger->trace("Addr = X / 8 + Y * Paint.WidthByte, Paint.WidthByte:{}", Paint.WidthByte);
  UDOUBLE Addr = X / 8 + Y * Paint.WidthByte;
  // H_Logger->trace("X:{},Y:{}", X, Y);
  // H_Logger->trace("UBYTE Rdata = Paint.Image[Addr]; Addr:{}", Addr);
  UBYTE Rdata = Paint.Image[Addr];
  // H_Logger->trace("if (Color == BLACK)");
  if (Color == BLACK)
    Paint.Image[Addr] = Rdata & ~(0x80 >> (X % 8));
  else
    Paint.Image[Addr] = Rdata | (0x80 >> (X % 8));
}

/******************************************************************************
function:	Clear the color of the picture
parameter:
    Color   :   Painted colors
******************************************************************************/
void GUI_Paint::Paint_Clear(UWORD Color)
{
  H_Logger->trace("GUI_Paint::Paint_Clear()");
  // Debug("x = %d, y = %d\r\n", Paint.WidthByte, Paint.Height);
  for (UWORD Y = 0; Y < Paint.HeightByte; Y++)
  {
    for (UWORD X = 0; X < Paint.WidthByte; X++)
    { // 8 pixel =  1 byte
      UDOUBLE Addr = X + Y * Paint.WidthByte;
      Paint.Image[Addr] = Color;
    }
  }
}

/******************************************************************************
function:	Clear the color of a window
parameter:
    Xstart :   x starting point
    Ystart :   Y starting point
    Xend   :   x end point
    Yend   :   y end point
******************************************************************************/
void GUI_Paint::Paint_ClearWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color)
{
  H_Logger->trace("GUI_Paint::Paint_ClearWindows() Xstart:{},Ystart:{},Xend:{},Yend:{},Color:{}", Xstart, Ystart, Xend,
                  Yend, Color);
  UWORD X, Y;
  for (Y = Ystart; Y < Yend; Y++)
  {
    for (X = Xstart; X < Xend; X++)
    { // 8 pixel =  1 byte
      Paint_SetPixel(X, Y, Color);
    }
  }
}

/******************************************************************************
function:	Draw Point(Xpoint, Ypoint) Fill the color
parameter:
    Xpoint		:   The Xpoint coordinate of the point
    Ypoint		:   The Ypoint coordinate of the point
    Color		:   Set color
    Dot_Pixel	:	point size
******************************************************************************/
void GUI_Paint::Paint_DrawPoint(UWORD Xpoint, UWORD Ypoint, UWORD Color, DOT_PIXEL Dot_Pixel, DOT_STYLE DOT_STYLE)
{
  // H_Logger->trace("GUI_Paint::Paint_DrawPoint()");
  if (Xpoint > Paint.Width || Ypoint > Paint.Height)
  {
    Debug("Paint_DrawPoint Input exceeds the normal display range\r\n");
    return;
  }

  int16_t XDir_Num, YDir_Num;
  if (DOT_STYLE == DOT_FILL_AROUND)
  {
    for (XDir_Num = 0; XDir_Num < 2 * Dot_Pixel - 1; XDir_Num++)
    {
      for (YDir_Num = 0; YDir_Num < 2 * Dot_Pixel - 1; YDir_Num++)
      {
        if (Xpoint + XDir_Num - Dot_Pixel < 0 || Ypoint + YDir_Num - Dot_Pixel < 0) break;
        // printf("x = %d, y = %d\r\n", Xpoint + XDir_Num - Dot_Pixel, Ypoint + YDir_Num - Dot_Pixel);
        Paint_SetPixel(Xpoint + XDir_Num - Dot_Pixel, Ypoint + YDir_Num - Dot_Pixel, Color);
      }
    }
  }
  else
  {
    for (XDir_Num = 0; XDir_Num < Dot_Pixel; XDir_Num++)
    {
      for (YDir_Num = 0; YDir_Num < Dot_Pixel; YDir_Num++)
      {
        Paint_SetPixel(Xpoint + XDir_Num - 1, Ypoint + YDir_Num - 1, Color);
      }
    }
  }
}

/******************************************************************************
function:	Draw a line of arbitrary slope
parameter:
    Xstart ：Starting Xpoint point coordinates
    Ystart ：Starting Xpoint point coordinates
    Xend   ：End point Xpoint coordinate
    Yend   ：End point Ypoint coordinate
    Color  ：The color of the line segment
******************************************************************************/
void GUI_Paint::Paint_DrawLine(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color, LINE_STYLE Line_Style,
                               DOT_PIXEL Dot_Pixel)
{
  H_Logger->trace("GUI_Paint::Paint_DrawLine()");
  if (Xstart > Paint.Width || Ystart > Paint.Height || Xend > Paint.Width || Yend > Paint.Height)
  {
    Debug("Paint_DrawLine Input exceeds the normal display range\r\n");
    return;
  }

  UWORD Xpoint = Xstart;
  UWORD Ypoint = Ystart;
  int dx = (int)Xend - (int)Xstart >= 0 ? Xend - Xstart : Xstart - Xend;
  int dy = (int)Yend - (int)Ystart <= 0 ? Yend - Ystart : Ystart - Yend;

  // Increment direction, 1 is positive, -1 is counter;
  int XAddway = Xstart < Xend ? 1 : -1;
  int YAddway = Ystart < Yend ? 1 : -1;

  // Cumulative error
  int Esp = dx + dy;
  char Dotted_Len = 0;

  for (;;)
  {
    Dotted_Len++;
    // Painted dotted line, 2 point is really virtual
    if (Line_Style == LINE_STYLE_DOTTED && Dotted_Len % 3 == 0)
    {
      // Debug("LINE_DOTTED\r\n");
      Paint_DrawPoint(Xpoint, Ypoint, IMAGE_BACKGROUND, Dot_Pixel, DOT_STYLE_DFT);
      Dotted_Len = 0;
    }
    else
    {
      Paint_DrawPoint(Xpoint, Ypoint, Color, Dot_Pixel, DOT_STYLE_DFT);
    }
    if (2 * Esp >= dy)
    {
      if (Xpoint == Xend) break;
      Esp += dy;
      Xpoint += XAddway;
    }
    if (2 * Esp <= dx)
    {
      if (Ypoint == Yend) break;
      Esp += dx;
      Ypoint += YAddway;
    }
  }
}

/******************************************************************************
function:	Draw a rectangle
parameter:
    Xstart ：Rectangular  Starting Xpoint point coordinates
    Ystart ：Rectangular  Starting Xpoint point coordinates
    Xend   ：Rectangular  End point Xpoint coordinate
    Yend   ：Rectangular  End point Ypoint coordinate
    Color  ：The color of the Rectangular segment
    Filled : Whether it is filled--- 1 solid 0：empty
******************************************************************************/
void GUI_Paint::Paint_DrawRectangle(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color, DRAW_FILL Filled,
                                    DOT_PIXEL Dot_Pixel)
{
  H_Logger->trace("GUI_Paint::Paint_DrawRectangle()");
  if (Xstart > Paint.Width || Ystart > Paint.Height || Xend > Paint.Width || Yend > Paint.Height)
  {
    Debug("Input exceeds the normal display range\r\n");
    return;
  }

  if (Filled)
  {
    UWORD Ypoint;
    for (Ypoint = Ystart; Ypoint < Yend; Ypoint++)
    {
      Paint_DrawLine(Xstart, Ypoint, Xend, Ypoint, Color, LINE_STYLE_SOLID, Dot_Pixel);
    }
  }
  else
  {
    Paint_DrawLine(Xstart, Ystart, Xend, Ystart, Color, LINE_STYLE_SOLID, Dot_Pixel);
    Paint_DrawLine(Xstart, Ystart, Xstart, Yend, Color, LINE_STYLE_SOLID, Dot_Pixel);
    Paint_DrawLine(Xend, Yend, Xend, Ystart, Color, LINE_STYLE_SOLID, Dot_Pixel);
    Paint_DrawLine(Xend, Yend, Xstart, Yend, Color, LINE_STYLE_SOLID, Dot_Pixel);
  }
}

/******************************************************************************
function:	Use the 8-point method to draw a circle of the
            specified size at the specified position->
parameter:
    X_Center  ：Center X coordinate
    Y_Center  ：Center Y coordinate
    Radius    ：circle Radius
    Color     ：The color of the ：circle segment
    Filled    : Whether it is filled: 1 filling 0：Do not
******************************************************************************/
void GUI_Paint::Paint_DrawCircle(UWORD X_Center, UWORD Y_Center, UWORD Radius, UWORD Color, DRAW_FILL Draw_Fill,
                                 DOT_PIXEL Dot_Pixel)
{
  H_Logger->trace("GUI_Paint::Paint_DrawCircle()");
  if (X_Center > Paint.Width || Y_Center >= Paint.Height)
  {
    Debug("Paint_DrawCircle Input exceeds the normal display range\r\n");
    return;
  }

  // Draw a circle from(0, R) as a starting point
  int16_t XCurrent, YCurrent;
  XCurrent = 0;
  YCurrent = Radius;

  // Cumulative error,judge the next point of the logo
  int16_t Esp = 3 - (Radius << 1);

  int16_t sCountY;
  if (Draw_Fill == DRAW_FILL_FULL)
  {
    while (XCurrent <= YCurrent)
    { // Realistic circles
      for (sCountY = XCurrent; sCountY <= YCurrent; sCountY++)
      {
        Paint_DrawPoint(X_Center + XCurrent, Y_Center + sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT); // 1
        Paint_DrawPoint(X_Center - XCurrent, Y_Center + sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT); // 2
        Paint_DrawPoint(X_Center - sCountY, Y_Center + XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT); // 3
        Paint_DrawPoint(X_Center - sCountY, Y_Center - XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT); // 4
        Paint_DrawPoint(X_Center - XCurrent, Y_Center - sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT); // 5
        Paint_DrawPoint(X_Center + XCurrent, Y_Center - sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT); // 6
        Paint_DrawPoint(X_Center + sCountY, Y_Center - XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT); // 7
        Paint_DrawPoint(X_Center + sCountY, Y_Center + XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);
      }
      if (Esp < 0)
        Esp += 4 * XCurrent + 6;
      else
      {
        Esp += 10 + 4 * (XCurrent - YCurrent);
        YCurrent--;
      }
      XCurrent++;
    }
  }
  else
  { // Draw a hollow circle
    while (XCurrent <= YCurrent)
    {
      Paint_DrawPoint(X_Center + XCurrent, Y_Center + YCurrent, Color, Dot_Pixel, DOT_STYLE_DFT); // 1
      Paint_DrawPoint(X_Center - XCurrent, Y_Center + YCurrent, Color, Dot_Pixel, DOT_STYLE_DFT); // 2
      Paint_DrawPoint(X_Center - YCurrent, Y_Center + XCurrent, Color, Dot_Pixel, DOT_STYLE_DFT); // 3
      Paint_DrawPoint(X_Center - YCurrent, Y_Center - XCurrent, Color, Dot_Pixel, DOT_STYLE_DFT); // 4
      Paint_DrawPoint(X_Center - XCurrent, Y_Center - YCurrent, Color, Dot_Pixel, DOT_STYLE_DFT); // 5
      Paint_DrawPoint(X_Center + XCurrent, Y_Center - YCurrent, Color, Dot_Pixel, DOT_STYLE_DFT); // 6
      Paint_DrawPoint(X_Center + YCurrent, Y_Center - XCurrent, Color, Dot_Pixel, DOT_STYLE_DFT); // 7
      Paint_DrawPoint(X_Center + YCurrent, Y_Center + XCurrent, Color, Dot_Pixel, DOT_STYLE_DFT); // 0

      if (Esp < 0)
        Esp += 4 * XCurrent + 6;
      else
      {
        Esp += 10 + 4 * (XCurrent - YCurrent);
        YCurrent--;
      }
      XCurrent++;
    }
  }
}

/******************************************************************************
function:	Show English characters
parameter:
    Xpoint           ：X coordinate
    Ypoint           ：Y coordinate
    Acsii_Char       ：To display the English characters
    Font             ：A structure pointer that displays a character size
    Color_Background : Select the background color of the English character
    Color_Foreground : Select the foreground color of the English character
******************************************************************************/
void GUI_Paint::Paint_DrawChar(UWORD Xpoint, UWORD Ypoint, const char Acsii_Char, sFONT *Font, UWORD Color_Background,
                               UWORD Color_Foreground)
{
  UWORD Page, Column;

  if (Xpoint > Paint.Width || Ypoint > Paint.Height)
  {
    Debug("Paint_DrawChar Input exceeds the normal display range\r\n");
    return;
  }

  uint32_t Char_Offset = (Acsii_Char - ' ') * Font->Height * (Font->Width / 8 + (Font->Width % 8 ? 1 : 0));
  const unsigned char *ptr = &Font->table[Char_Offset];

  for (Page = 0; Page < Font->Height; Page++)
  {
    for (Column = 0; Column < Font->Width; Column++)
    {
      // To determine whether the font background color and screen background color is consistent
      if (FONT_BACKGROUND == Color_Background)
      { // this process is to speed up the scan
        if (*ptr & (0x80 >> (Column % 8))) Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Foreground);
        // Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
      }
      else
      {
        if (*ptr & (0x80 >> (Column % 8)))
        {
          Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Foreground);
          // Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
        }
        else
        {
          Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Background);
          // Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
        }
      }
      // One pixel is 8 bits
      if (Column % 8 == 7) ptr++;
    } // Write a line
    if (Font->Width % 8 != 0) ptr++;
  } // Write all
}

/******************************************************************************
function:	Display the string
parameter:
    Xstart           ：X coordinate
    Ystart           ：Y coordinate
    pString          ：The first address of the English string to be displayed
    Font             ：A structure pointer that displays a character size
    Color_Background : Select the background color of the English character
    Color_Foreground : Select the foreground color of the English character
******************************************************************************/
void GUI_Paint::Paint_DrawString_EN(UWORD Xstart, UWORD Ystart, const char *pString, sFONT *Font,
                                    UWORD Color_Background, UWORD Color_Foreground)
{
  UWORD Xpoint = Xstart;
  UWORD Ypoint = Ystart;

  if (Xstart > Paint.Width || Ystart > Paint.Height)
  {
    Debug("Paint_DrawString_EN Input exceeds the normal display range\r\n");
    return;
  }

  while (*pString != '\0')
  {
    // if X direction filled , reposition to(Xstart,Ypoint),Ypoint is Y direction plus the Height of the character
    if ((Xpoint + Font->Width) > Paint.Width)
    {
      Xpoint = Xstart;
      Ypoint += Font->Height;
    }

    // If the Y direction is full, reposition to(Xstart, Ystart)
    if ((Ypoint + Font->Height) > Paint.Height)
    {
      Xpoint = Xstart;
      Ypoint = Ystart;
    }
    Paint_DrawChar(Xpoint, Ypoint, *pString, Font, Color_Background, Color_Foreground);

    // The next character of the address
    pString++;

    // The next word of the abscissa increases the font of the broadband
    Xpoint += Font->Width;
  }
}

/******************************************************************************
function:	Display the string
parameter:
    Xstart           ：X coordinate
    Ystart           ：Y coordinate
    pString          ：The first address of the Chinese string and English
                        string to be displayed
    Font             ：A structure pointer that displays a character size
    Color_Background : Select the background color of the English character
    Color_Foreground : Select the foreground color of the English character
******************************************************************************/
void GUI_Paint::Paint_DrawString_CN(UWORD Xstart, UWORD Ystart, const char *pString, cFONT *font,
                                    UWORD Color_Background, UWORD Color_Foreground)
{
  const char *p_text = pString;
  int x = Xstart, y = Ystart;
  int i, j, Num;

  /* Send the string character by character on EPD */
  while (*p_text != 0)
  {
    if (*p_text <= 0x7F)
    { // ASCII < 126
      for (Num = 0; Num < font->size; Num++)
      {
        if (*p_text == font->table[Num].index[0])
        {
          const char *ptr = &font->table[Num].matrix[0];

          for (j = 0; j < font->Height; j++)
          {
            for (i = 0; i < font->Width; i++)
            {
              if (FONT_BACKGROUND == Color_Background)
              { // this process is to speed up the scan
                if (*ptr & (0x80 >> (i % 8)))
                {
                  Paint_SetPixel(x + i, y + j, Color_Foreground);
                  // Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                }
              }
              else
              {
                if (*ptr & (0x80 >> (i % 8)))
                {
                  Paint_SetPixel(x + i, y + j, Color_Foreground);
                  // Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                }
                else
                {
                  Paint_SetPixel(x + i, y + j, Color_Background);
                  // Paint_DrawPoint(x + i, y + j, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                }
              }
              if (i % 8 == 7)
              {
                ptr++;
              }
            }
            if (font->Width % 8 != 0)
            {
              ptr++;
            }
          }
          break;
        }
      }
      /* Point on the next character */
      p_text += 1;
      /* Decrement the column position by 16 */
      x += font->ASCII_Width;
    }
    else
    { // Chinese
      for (Num = 0; Num < font->size; Num++)
      {
        if ((*p_text == font->table[Num].index[0]) && (*(p_text + 1) == font->table[Num].index[1]))
        {
          const char *ptr = &font->table[Num].matrix[0];

          for (j = 0; j < font->Height; j++)
          {
            for (i = 0; i < font->Width; i++)
            {
              if (FONT_BACKGROUND == Color_Background)
              { // this process is to speed up the scan
                if (*ptr & (0x80 >> (i % 8)))
                {
                  Paint_SetPixel(x + i, y + j, Color_Foreground);
                  // Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                }
              }
              else
              {
                if (*ptr & (0x80 >> (i % 8)))
                {
                  Paint_SetPixel(x + i, y + j, Color_Foreground);
                  // Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                }
                else
                {
                  Paint_SetPixel(x + i, y + j, Color_Background);
                  // Paint_DrawPoint(x + i, y + j, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                }
              }
              if (i % 8 == 7)
              {
                ptr++;
              }
            }
            if (font->Width % 8 != 0)
            {
              ptr++;
            }
          }
          break;
        }
      }
      /* Point on the next character */
      p_text += 2;
      /* Decrement the column position by 16 */
      x += font->Width;
    }
  }
}

/******************************************************************************
function:	Display nummber
parameter:
    Xstart           ：X coordinate
    Ystart           : Y coordinate
    Nummber          : The number displayed
    Font             ：A structure pointer that displays a character size
    Color_Background : Select the background color of the English character
    Color_Foreground : Select the foreground color of the English character
******************************************************************************/
#define ARRAY_LEN 255
void GUI_Paint::Paint_DrawNum(UWORD Xpoint, UWORD Ypoint, int32_t Nummber, sFONT *Font, UWORD Color_Background,
                              UWORD Color_Foreground)
{
  int16_t Num_Bit = 0, Str_Bit = 0;
  uint8_t Str_Array[ARRAY_LEN] = { 0 }, Num_Array[ARRAY_LEN] = { 0 };
  uint8_t *pStr = Str_Array;

  if (Xpoint > Paint.Width || Ypoint > Paint.Height)
  {
    Debug("Paint_DisNum Input exceeds the normal display range\r\n");
    return;
  }

  // Converts a number to a string
  while (Nummber)
  {
    Num_Array[Num_Bit] = Nummber % 10 + '0';
    Num_Bit++;
    Nummber /= 10;
  }

  // The string is inverted
  while (Num_Bit > 0)
  {
    Str_Array[Str_Bit] = Num_Array[Num_Bit - 1];
    Str_Bit++;
    Num_Bit--;
  }

  // show
  Paint_DrawString_EN(Xpoint, Ypoint, (const char *)pStr, Font, Color_Background, Color_Foreground);
}

/******************************************************************************
function:	Display time
parameter:
    Xstart           ：X coordinate
    Ystart           : Y coordinate
    pTime            : Time-related structures
    Font             ：A structure pointer that displays a character size
    Color            : Select the background color of the English character
******************************************************************************/
void GUI_Paint::Paint_DrawTime(UWORD Xstart, UWORD Ystart, PAINT_TIME *pTime, sFONT *Font, UWORD Color_Background,
                               UWORD Color_Foreground)
{
  H_Logger->trace("GUI_Paint::Paint_DrawTime()");
  uint8_t value[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

  UWORD Dx = Font->Width;

  // Write data into the cache
  Paint_DrawChar(Xstart, Ystart, value[pTime->Hour / 10], Font, Color_Background, Color_Foreground);
  Paint_DrawChar(Xstart + Dx, Ystart, value[pTime->Hour % 10], Font, Color_Background, Color_Foreground);
  Paint_DrawChar(Xstart + Dx + Dx / 4 + Dx / 2, Ystart, ':', Font, Color_Background, Color_Foreground);
  Paint_DrawChar(Xstart + Dx * 2 + Dx / 2, Ystart, value[pTime->Min / 10], Font, Color_Background, Color_Foreground);
  Paint_DrawChar(Xstart + Dx * 3 + Dx / 2, Ystart, value[pTime->Min % 10], Font, Color_Background, Color_Foreground);
  Paint_DrawChar(Xstart + Dx * 4 + Dx / 2 - Dx / 4, Ystart, ':', Font, Color_Background, Color_Foreground);
  Paint_DrawChar(Xstart + Dx * 5, Ystart, value[pTime->Sec / 10], Font, Color_Background, Color_Foreground);
  Paint_DrawChar(Xstart + Dx * 6, Ystart, value[pTime->Sec % 10], Font, Color_Background, Color_Foreground);
}

/******************************************************************************
function:	Display monochrome bitmap
parameter:
    image_buffer ：A picture data converted to a bitmap
info:
    Use a computer to convert the image into a corresponding array,
    and then embed the array directly into Imagedata.cpp as a .c file.
******************************************************************************/
void GUI_Paint::Paint_DrawBitMap(const unsigned char *image_buffer)
{
  UWORD x, y;
  UDOUBLE Addr = 0;

  for (y = 0; y < Paint.HeightByte; y++)
  {
    for (x = 0; x < Paint.WidthByte; x++)
    { // 8 pixel =  1 byte
      Addr = x + y * Paint.WidthByte;
      Paint.Image[Addr] = (unsigned char)image_buffer[Addr];
    }
  }
}
