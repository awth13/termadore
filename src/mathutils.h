/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef MATHUTILS_H
#define MATHUTILS_H

#define bool char
#define false 0
#define true 1

#ifndef CSCLIP
int line_clip(int *x0_out, int *y0_out, int *x1_out, int *y1_out,
              int xmin, int xmax, int ymin, int ymax)
{

    return false;
}
#else
typedef int outcode;

const int INSIDE = 0; // 0000
const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000

// compute_ the bit code for a point (x, y) using the clip
// bounded diagonally by (xmin, ymin), and (xmax, ymax)

// ASSUME THAT xmax, xmin, ymax and ymin are global constants.

outcode compute_outcode(int x, int y, int xmin, int xmax, int ymin, int ymax)
{
    outcode code;
    code = INSIDE;          // initialised as being inside of [[clip window]]

    if (x < xmin)           // to the left of clip window
        code |= LEFT;
    else if (x > xmax)      // to the right of clip window
        code |= RIGHT;
    if (y < ymin)           // below the clip window
        code |= BOTTOM;
    else if (y > ymax)      // above the clip window
        code |= TOP;

    return code;
}

// Cohen–Sutherland clipping algorithm clips a line from
// P0 = (x0, y0) to P1 = (x1, y1) against a rectangle with 
// diagonal from (xmin, ymin) to (xmax, ymax).
int line_clip(int *x0_out, int *y0_out, int *x1_out, int *y1_out,
              int xmin, int xmax, int ymin, int ymax)
{
    int x0, y0, x1, y1;
    x0 = *x0_out;
    y0 = *y0_out;
    x1 = *x1_out;
    y1 = *y1_out;
    // compute outcodes for P0, P1, and whatever point lies outside the clip rectangle
    outcode outcode0 = compute_outcode(x0, y0, xmin, xmax, ymin, ymax);
    outcode outcode1 = compute_outcode(x1, y1, xmin, xmax, ymin, ymax);
    bool accept = false;

    while (true) {
        if (!(outcode0 | outcode1)) {
            // bitwise OR is 0: both points inside window; trivially accept and exit loop
            accept = true;
            break;
        } else if (outcode0 & outcode1) {
            // bitwise AND is not 0: both points share an outside zone (LEFT, RIGHT, TOP,
            // or BOTTOM), so both must be outside window; exit loop (accept is false)
            break;
        } else {
            // failed both tests, so calculate the line segment to clip
            // from an outside point to an intersection with clip edge
            int x, y;

            // At least one endpoint is outside the clip rectangle; pick it.
            outcode outcode_out = outcode1 > outcode0 ? outcode1 : outcode0;

            // Now find the intersection point;
            // use formulas:
            //   slope = (y1 - y0) / (x1 - x0)
            //   x = x0 + (1 / slope) * (ym - y0), where ym is ymin or ymax
            //   y = y0 + slope * (xm - x0), where xm is xmin or xmax
            // No need to worry about divide-by-zero because, in each case, the
            // outcode bit being tested guarantees the denominator is non-zero
            if (outcode_out & TOP) {           // point is above the clip window
                x = x0 + (x1 - x0) * (ymax - y0) / (y1 - y0);
                y = ymax;
            } else if (outcode_out & BOTTOM) { // point is below the clip window
                x = x0 + (x1 - x0) * (ymin - y0) / (y1 - y0);
                y = ymin;
            } else if (outcode_out & RIGHT) {  // point is to the right of clip window
                y = y0 + (y1 - y0) * (xmax - x0) / (x1 - x0);
                x = xmax;
            } else if (outcode_out & LEFT) {   // point is to the left of clip window
                y = y0 + (y1 - y0) * (xmin - x0) / (x1 - x0);
                x = xmin;
            }

            // Now we move outside point to intersection point to clip
            // and get ready for next pass.
            if (outcode_out == outcode0) {
                x0 = x;
                y0 = y;
                outcode0 = compute_outcode(x0, y0, xmin, xmax, ymin, ymax);
            } else {
                x1 = x;
                y1 = y;
                outcode1 = compute_outcode(x1, y1, xmin, xmax, ymin, ymax);
            }
        }
    }

    *x0_out = x0;
    *y0_out = y0;
    *x1_out = x1;
    *y1_out = y1;

    return accept;
}
#endif // CSCLIP

#endif /* MATHUTILS_H */
