#define GL_SILENCE_DEPRECATION

#include <windows.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <math.h>

float angle = 0.0f;
struct Point {
    float x;
    float y;

    Point(float ax = 0.0f, float ay = 0.0f) {
        x = ax;
        y = ay;
    }
};

float scene = 0.0f;
float targetScene = 0.0f;
float carMove = 23.0f;
int frameCount = 0;

// Project keyboard flow: D = next disaster stage, A = previous disaster stage, X = exit.

float limit01(float v) {
    if(v < 0.0f) return 0.0f;
    if(v > 1.0f) return 1.0f;
    return v;
}

float earthquakeValue() {
    if(scene <= 1.0f) {
        return limit01(scene);
    }

    if(scene <= 2.0f) {
        return limit01(2.0f - scene);
    }

    return 0.0f;
}

float fireValue() {
    if(scene <= 1.0f) {
        return 0.0f;
    }

    if(scene <= 2.0f) {
        return limit01(scene - 1.0f);
    }

    return limit01(3.0f - scene);
}

float floodValue() {
    if(scene <= 2.0f) {
        return 0.0f;
    }

    if(scene <= 3.0f) {
        return limit01(scene - 2.0f);
    }

    return limit01(4.0f - scene);
}

float stormValue() {
    return limit01(scene - 3.0f);
}

Point mixPoint(Point a, Point b, float t) {
    return Point(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
}

Point leanPoint(Point p, float pivotY, float leanAmount, float t) {
    return Point(p.x + (p.y - pivotY) * leanAmount * t, p.y);
}

void drawCircle(float cx, float cy, float r) {
    glBegin(GL_POLYGON);
    for(int i = 0; i < 60; i++) {
        float theta = i * 2.0f * 3.1416f / 60.0f;
        glVertex2f(cx + r * cos(theta), cy + r * sin(theta));
    }
    glEnd();
}


// Project Requirement 1: DDA Line Algorithm
// Used for rain drops so the rain is drawn by a line drawing algorithm.
void drawWindowWatcher(float x, float y, float shirtR, float shirtG, float shirtB);
void drawDDA(float x1, float y1, float x2, float y2) {
    float scale = 100.0f;
    float X1 = x1 * scale;
    float Y1 = y1 * scale;
    float X2 = x2 * scale;
    float Y2 = y2 * scale;

    float dx = X2 - X1;
    float dy = Y2 - Y1;
    float steps = fabs(dx);

    if(fabs(dy) > steps) {
        steps = fabs(dy);
    }

    if(steps == 0) {
        glBegin(GL_POINTS);
            glVertex2f(x1, y1);
        glEnd();
        return;
    }

    float xInc = dx / steps;
    float yInc = dy / steps;
    float x = X1;
    float y = Y1;

    glBegin(GL_POINTS);
    for(int i = 0; i <= steps; i++) {
        glVertex2f(x / scale, y / scale);
        x = x + xInc;
        y = y + yInc;
    }
    glEnd();
}

void drawMidLinePoints(int x, int y, int xCenter, int yCenter, float scale) {
    glVertex2f((xCenter + x) / scale, (yCenter + y) / scale);
    glVertex2f((xCenter - x) / scale, (yCenter + y) / scale);
    glVertex2f((xCenter + x) / scale, (yCenter - y) / scale);
    glVertex2f((xCenter - x) / scale, (yCenter - y) / scale);
    glVertex2f((xCenter + y) / scale, (yCenter + x) / scale);
    glVertex2f((xCenter - y) / scale, (yCenter + x) / scale);
    glVertex2f((xCenter + y) / scale, (yCenter - x) / scale);
    glVertex2f((xCenter - y) / scale, (yCenter - x) / scale);
}

// Project Requirement 3: Midpoint Circle Algorithm
// Used for wheel/cloud/umbrella outlines.
void drawMidPointCircle(float cx, float cy, float radius) {
    float scale = 100.0f;
    int xCenter = (int)(cx * scale);
    int yCenter = (int)(cy * scale);
    int r = (int)(radius * scale);

    int x = 0;
    int y = r;
    int p = 1 - r;

    glBegin(GL_POINTS);
    drawMidLinePoints(x, y, xCenter, yCenter, scale);

    while(x < y) {
        x++;

        if(p < 0) {
            p = p + 2 * x + 1;
        } else {
            y--;
            p = p + 2 * (x - y) + 1;
        }

        drawMidLinePoints(x, y, xCenter, yCenter, scale);
    }
    glEnd();
}

// Project Requirement 2: Midpoint Line Algorithm
// Used for lightning and selected straight outlines.
void drawMidPointLine(float x1, float y1, float x2, float y2) {
    float scale = 100.0f;
    int X1 = (int)(x1 * scale);
    int Y1 = (int)(y1 * scale);
    int X2 = (int)(x2 * scale);
    int Y2 = (int)(y2 * scale);

    int dx = abs(X2 - X1);
    int dy = abs(Y2 - Y1);
    int sx = (X1 < X2) ? 1 : -1;
    int sy = (Y1 < Y2) ? 1 : -1;
    int err = dx - dy;

    glBegin(GL_POINTS);
    while(true) {
        glVertex2f(X1 / scale, Y1 / scale);

        if(X1 == X2 && Y1 == Y2) {
            break;
        }

        int e2 = 2 * err;
        if(e2 > -dy) {
            err = err - dy;
            X1 = X1 + sx;
        }
        if(e2 < dx) {
            err = err + dx;
            Y1 = Y1 + sy;
        }
    }
    glEnd();
}

// Project Requirement 5: 1 3D Object
// Small 3D debris cube appears with the earthquake crack.
void draw3DDebrisCube(float show) {
    if(show <= 0.01f) {
        return;
    }

    glPushMatrix();
    glTranslatef(13.2f, -2.0f, 0.0f);
    glRotatef(18.0f * show, 0.0f, 0.0f, 1.0f);
    glScalef(0.75f * show, 0.75f * show, 0.75f * show);

    glBegin(GL_QUADS);
        glColor3f(0.34f, 0.34f, 0.34f);
        glVertex3f(-0.6f, -0.4f, 0.5f);
        glVertex3f(0.6f, -0.4f, 0.5f);
        glVertex3f(0.6f, 0.4f, 0.5f);
        glVertex3f(-0.6f, 0.4f, 0.5f);

        glColor3f(0.22f, 0.22f, 0.22f);
        glVertex3f(0.6f, -0.4f, 0.5f);
        glVertex3f(0.95f, -0.10f, -0.2f);
        glVertex3f(0.95f, 0.70f, -0.2f);
        glVertex3f(0.6f, 0.4f, 0.5f);

        glColor3f(0.48f, 0.48f, 0.48f);
        glVertex3f(-0.6f, 0.4f, 0.5f);
        glVertex3f(0.6f, 0.4f, 0.5f);
        glVertex3f(0.95f, 0.70f, -0.2f);
        glVertex3f(-0.25f, 0.70f, -0.2f);
    glEnd();

    glPopMatrix();
}

void drawQuad(Point a, Point b, Point c, Point d) {
    glBegin(GL_POLYGON);
        glVertex2f(a.x, a.y);
        glVertex2f(b.x, b.y);
        glVertex2f(c.x, c.y);
        glVertex2f(d.x, d.y);
    glEnd();
}

void drawMorphQuad(Point a1, Point a2, Point a3, Point a4, Point b1, Point b2, Point b3, Point b4, float t) {
    drawQuad(mixPoint(a1, b1, t), mixPoint(a2, b2, t), mixPoint(a3, b3, t), mixPoint(a4, b4, t));
}

void drawLeanQuad(Point a, Point b, Point c, Point d, float pivotY, float leanAmount, float t) {
    drawQuad(leanPoint(a, pivotY, leanAmount, t),
             leanPoint(b, pivotY, leanAmount, t),
             leanPoint(c, pivotY, leanAmount, t),
             leanPoint(d, pivotY, leanAmount, t));
}

void drawBackground() {
    glColor3f(0.45f, 0.78f, 1.0f);
    glBegin(GL_QUADS);
        glVertex2f(-4.0f, -3.9f);
        glVertex2f(-4.0f, 12.0f);
        glVertex2f(22.0f, 12.0f);
        glVertex2f(22.0f, -3.9f);
    glEnd();

    glColor3f(0.15f, 0.17f, 0.18f);
    glBegin(GL_QUADS);
        glVertex2f(-4.0f, -3.9f);
        glVertex2f(-4.0f, 0.6f);
        glVertex2f(22.0f, 0.6f);
        glVertex2f(22.0f, -3.9f);
    glEnd();

    glColor3f(0.55f, 0.55f, 0.55f);
    glBegin(GL_QUADS);
        glVertex2f(-4.0f, 0.6f);
        glVertex2f(-4.0f, 1.0f);
        glVertex2f(22.0f, 1.0f);
        glVertex2f(22.0f, 0.6f);
    glEnd();

    glColor3f(1.0f, 0.86f, 0.20f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for(float x = -4.0f; x < 22.0f; x += 2.0f) {
        glVertex2f(x, -1.6f);
        glVertex2f(x + 1.0f, -1.6f);
    }
    glEnd();
}

void drawRoadCrack() {
    float q = earthquakeValue();

    if(q <= 0.01f) {
        return;
    }

    Point center(9.4f, -1.2f);
    const int n = 18;

    Point crack[n] = {
        Point(8.30f, 0.25f),
        Point(8.05f, -0.25f),
        Point(8.75f, -0.65f),
        Point(7.85f, -0.95f),
        Point(8.65f, -1.35f),
        Point(7.65f, -2.05f),
        Point(8.80f, -2.35f),
        Point(7.90f, -3.15f),
        Point(9.35f, -2.55f),
        Point(10.95f, -2.95f),
        Point(12.25f, -2.85f),
        Point(11.25f, -2.35f),
        Point(12.05f, -1.95f),
        Point(10.35f, -1.45f),
        Point(11.35f, -1.00f),
        Point(10.25f, -0.55f),
        Point(10.80f, 0.05f),
        Point(9.75f, -0.20f)
    };

    Point outer[n];
    Point inner[n];
    Point shadow[n];

    for(int i = 0; i < n; i++) {
        outer[i] = mixPoint(center, crack[i], q * 1.18f);
        inner[i] = mixPoint(center, crack[i], q * 0.78f);
        inner[i].y -= 0.55f * q;
        shadow[i] = outer[i];
        shadow[i].x += 0.18f * q;
        shadow[i].y -= 0.25f * q;
    }

    glColor3f(0.10f, 0.10f, 0.10f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < n; i++) {
        glVertex2f(shadow[i].x, shadow[i].y);
    }
    glEnd();

    for(int i = 0; i < n; i++) {
        int j = (i + 1) % n;

        if(outer[i].x < center.x && outer[j].x < center.x) {
            glColor3f(0.38f, 0.38f, 0.38f);
        } else if(outer[i].x > center.x && outer[j].x > center.x) {
            glColor3f(0.18f, 0.18f, 0.18f);
        } else {
            glColor3f(0.25f, 0.25f, 0.25f);
        }

        glBegin(GL_QUADS);
            glVertex2f(outer[i].x, outer[i].y);
            glVertex2f(outer[j].x, outer[j].y);
            glVertex2f(inner[j].x, inner[j].y);
            glVertex2f(inner[i].x, inner[i].y);
        glEnd();
    }

    glColor3f(0.04f, 0.04f, 0.04f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < n; i++) {
        glVertex2f(inner[i].x, inner[i].y);
    }
    glEnd();

    glColor3f(0.70f, 0.70f, 0.70f);
    glLineWidth(3.5f);
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < n; i++) {
        glVertex2f(outer[i].x, outer[i].y);
    }
    glEnd();

    glColor3f(0.16f, 0.16f, 0.16f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < n; i++) {
        glVertex2f(inner[i].x, inner[i].y);
    }
    glEnd();

    glColor3f(0.82f, 0.82f, 0.82f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
        glVertex2f(outer[0].x, outer[0].y);
        glVertex2f(outer[1].x, outer[1].y);
        glVertex2f(outer[2].x, outer[2].y);
        glVertex2f(outer[3].x, outer[3].y);
        glVertex2f(outer[4].x, outer[4].y);
        glVertex2f(outer[5].x, outer[5].y);
        glVertex2f(outer[6].x, outer[6].y);
        glVertex2f(outer[7].x, outer[7].y);
    glEnd();

    glColor3f(0.01f, 0.01f, 0.01f);
    glLineWidth(5.0f);
    glBegin(GL_LINE_STRIP);
        Point p1 = mixPoint(center, Point(8.55f, 0.05f), q * 0.80f);
        Point p2 = mixPoint(center, Point(9.05f, -0.65f), q * 0.80f);
        Point p3 = mixPoint(center, Point(8.55f, -1.25f), q * 0.80f);
        Point p4 = mixPoint(center, Point(9.00f, -1.95f), q * 0.80f);
        Point p5 = mixPoint(center, Point(8.55f, -2.70f), q * 0.80f);
        p1.y -= 0.40f * q;
        p2.y -= 0.45f * q;
        p3.y -= 0.52f * q;
        p4.y -= 0.58f * q;
        p5.y -= 0.64f * q;
        glVertex2f(p1.x, p1.y);
        glVertex2f(p2.x, p2.y);
        glVertex2f(p3.x, p3.y);
        glVertex2f(p4.x, p4.y);
        glVertex2f(p5.x, p5.y);
    glEnd();

    glBegin(GL_LINE_STRIP);
        Point p6 = mixPoint(center, Point(9.75f, -0.10f), q * 0.80f);
        Point p7 = mixPoint(center, Point(10.35f, -0.85f), q * 0.80f);
        Point p8 = mixPoint(center, Point(10.10f, -1.45f), q * 0.80f);
        Point p9 = mixPoint(center, Point(10.95f, -2.05f), q * 0.80f);
        Point p10 = mixPoint(center, Point(11.65f, -2.55f), q * 0.80f);
        p6.y -= 0.42f * q;
        p7.y -= 0.48f * q;
        p8.y -= 0.55f * q;
        p9.y -= 0.61f * q;
        p10.y -= 0.66f * q;
        glVertex2f(p6.x, p6.y);
        glVertex2f(p7.x, p7.y);
        glVertex2f(p8.x, p8.y);
        glVertex2f(p9.x, p9.y);
        glVertex2f(p10.x, p10.y);
    glEnd();
}

void drawWindow(Point a, Point b, Point c, Point d, float lean) {
    float q = earthquakeValue();

    glColor3f(0.65f, 0.85f, 0.92f);
    drawLeanQuad(a, b, c, d, 1.0f, lean, q);

    glColor3f(0.20f, 0.35f, 0.45f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
        Point p1 = leanPoint(a, 1.0f, lean, q);
        Point p2 = leanPoint(b, 1.0f, lean, q);
        Point p3 = leanPoint(c, 1.0f, lean, q);
        Point p4 = leanPoint(d, 1.0f, lean, q);
        glVertex2f(p1.x, p1.y);
        glVertex2f(p2.x, p2.y);
        glVertex2f(p3.x, p3.y);
        glVertex2f(p4.x, p4.y);
    glEnd();
}
//------Fire in window-------
void drawFireInWindow(float cx, float cy, float size, float show) {
    if(show <= 0.01f) {
        return;
    }

    float wave1 = sin(frameCount * 0.35f + cx) * 0.12f * show;
    float wave2 = cos(frameCount * 0.42f + cy) * 0.10f * show;

    // Project Requirement 4: Transformation - translate and scale the fire.
    glPushMatrix();
    glTranslatef(cx, cy, 0.0f);
    glScalef(size * show, size * show, 1.0f);

    glColor3f(1.0f, 0.05f, 0.00f);
    glBegin(GL_POLYGON);
        glVertex2f(-0.45f, -0.45f);
        glVertex2f(-0.32f, 0.12f);
        glVertex2f(-0.18f, -0.04f);
        glVertex2f(-0.05f, 0.52f + wave1);
        glVertex2f(0.10f, 0.05f);
        glVertex2f(0.25f, 0.62f + wave2);
        glVertex2f(0.42f, 0.10f);
        glVertex2f(0.48f, -0.45f);
    glEnd();

    glColor3f(1.0f, 0.55f, 0.00f);
    glBegin(GL_POLYGON);
        glVertex2f(-0.32f, -0.42f);
        glVertex2f(-0.20f, 0.02f);
        glVertex2f(-0.08f, -0.06f);
        glVertex2f(0.05f, 0.38f + wave2);
        glVertex2f(0.18f, -0.02f);
        glVertex2f(0.32f, 0.26f + wave1);
        glVertex2f(0.34f, -0.42f);
    glEnd();

    glColor3f(1.0f, 0.95f, 0.08f);
    glBegin(GL_POLYGON);
        glVertex2f(-0.14f, -0.38f);
        glVertex2f(-0.06f, 0.02f);
        glVertex2f(0.04f, 0.22f + wave1);
        glVertex2f(0.14f, -0.02f);
        glVertex2f(0.20f, -0.38f);
    glEnd();

    glPopMatrix();
}
//------Help needed human -----
void drawHelpHumanInWindow6() {
    float f = fireValue();

    if(f <= 0.02f) {
        return;
    }

    float show = limit01(f);
    float handWave = sin(frameCount * 0.18f) * 0.12f * show;

    // Window 6 boundary: x = 11 to 12, y = 4 to 5
    GLdouble clipLeft[]   = { 1.0,  0.0, 0.0, -11.0}; // x >= 11
    GLdouble clipRight[]  = {-1.0,  0.0, 0.0,  12.0}; // x <= 12
    GLdouble clipBottom[] = { 0.0,  1.0, 0.0,  -4.0}; // y >= 4
    GLdouble clipTop[]    = { 0.0, -1.0, 0.0,   5.0}; // y <= 5

    glClipPlane(GL_CLIP_PLANE0, clipLeft);
    glEnable(GL_CLIP_PLANE0);

    glClipPlane(GL_CLIP_PLANE1, clipRight);
    glEnable(GL_CLIP_PLANE1);

    glClipPlane(GL_CLIP_PLANE2, clipBottom);
    glEnable(GL_CLIP_PLANE2);

    glClipPlane(GL_CLIP_PLANE3, clipTop);
    glEnable(GL_CLIP_PLANE3);

    // ---------------- BODY ----------------
    glColor3f(1.0f, 0.0f, 0.0f);   // red shirt/body
    glBegin(GL_POLYGON);
        glVertex2f(11.4946f, 4.39331f);
        glVertex2f(11.7623f, 4.32168f);
        glVertex2f(11.7185f, 4.0148f);
        glVertex2f(11.27318f, 4.00788f);
        glVertex2f(11.2224f, 4.31014f);
    glEnd();

    // ---------------- RIGHT HAND (moving) ----------------
    glColor3f(0.95f, 0.70f, 0.48f);   // skin tone
    glBegin(GL_TRIANGLES);
        glVertex2f(11.7623f, 4.32168f);                         // shoulder
       glVertex2f(11.96f, 4.60779f + handWave);          // moving hand tip
        glVertex2f(11.7462f, 4.20894f);                        // arm lower point
    glEnd();

    // ---------------- LEFT HAND ----------------
    glBegin(GL_TRIANGLES);
        glVertex2f(11.03f, 4.6f);         // hand tip
        glVertex2f(11.2224f, 4.31014f);    // shoulder side
        glVertex2f(11.2404f, 4.20279f);    // lower point
    glEnd();

    // ---------------- HEAD ----------------
    glColor3f(0.95f, 0.70f, 0.48f);   // skin tone head
    float centerX = 11.48776f;
    float centerY = 4.57318f;
    float radius  = 0.18f;

    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        float X = centerX + radius * cos(angle);
        float Y = centerY + radius * sin(angle);
        glVertex2f(X, Y);
    }
    glEnd();

    // ---------------- HAIR ----------------
    glColor3f(0.08f, 0.04f, 0.03f);
    glBegin(GL_POLYGON);
        glVertex2f(11.34f, 4.60f);
        glVertex2f(11.63f, 4.60f);
        glVertex2f(11.59f, 4.73f);
        glVertex2f(11.39f, 4.73f);
    glEnd();

    // ---------------- EYES ----------------
    glColor3f(0.1f, 0.1f, 0.1f);
    glPointSize(2.0f);
    glBegin(GL_POINTS);
        glVertex2f(11.44f, 4.58f);
        glVertex2f(11.54f, 4.58f);
    glEnd();

    // ---------------- Disable Clipping ----------------
    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);
    glDisable(GL_CLIP_PLANE2);
    glDisable(GL_CLIP_PLANE3);

    // ---------------- Redraw Window Border ----------------
    glColor3f(0.20f, 0.35f, 0.45f);
    glLineWidth(1.8f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(11.0f, 4.0f);
        glVertex2f(11.0f, 5.0f);
        glVertex2f(12.0f, 5.0f);
        glVertex2f(12.0f, 4.0f);
    glEnd();
}
void drawBuildings() {
    float q = earthquakeValue();
    float f = fireValue();

    glColor3f(0.45f, 0.58f, 0.38f);
    drawMorphQuad(Point(1.0f, 1.0f), Point(1.0f, 9.0f), Point(6.0f, 9.0f), Point(6.0f, 1.0f),
                  Point(1.0f, 1.0f), Point(0.36f, 9.0f), Point(5.36f, 9.0f), Point(6.0f, 1.0f), q);

    glColor3f(0.35f, 0.48f, 0.30f);
    drawMorphQuad(Point(1.0f, 9.0f), Point(1.0f, 9.35f), Point(6.0f, 9.35f), Point(6.0f, 9.0f),
                  Point(0.36f, 9.0f), Point(0.33f, 9.35f), Point(5.33f, 9.35f), Point(5.36f, 9.0f), q);

    glColor3f(0.62f, 0.45f, 0.28f);
    drawMorphQuad(Point(8.0f, 1.0f), Point(8.0f, 10.0f), Point(13.0f, 10.0f), Point(13.0f, 1.0f),
                  Point(8.0f, 1.0f), Point(6.92f, 10.0f), Point(11.92f, 10.0f), Point(13.0f, 1.0f), q);

    glColor3f(0.50f, 0.36f, 0.20f);
    drawMorphQuad(Point(8.0f, 10.0f), Point(8.0f, 10.35f), Point(13.0f, 10.35f), Point(13.0f, 10.0f),
                  Point(6.92f, 10.0f), Point(6.88f, 10.35f), Point(11.88f, 10.35f), Point(11.92f, 10.0f), q);

    glColor3f(0.25f, 0.50f, 0.65f);
    drawMorphQuad(Point(15.0f, 1.0f), Point(15.0f, 9.0f), Point(20.0f, 9.0f), Point(20.0f, 1.0f),
                  Point(15.0f, 1.0f), Point(15.56f, 9.0f), Point(20.56f, 9.0f), Point(20.0f, 1.0f), q);

    glColor3f(0.18f, 0.38f, 0.52f);
    drawMorphQuad(Point(15.0f, 9.0f), Point(15.0f, 9.35f), Point(20.0f, 9.35f), Point(20.0f, 9.0f),
                  Point(15.56f, 9.0f), Point(15.59f, 9.35f), Point(20.59f, 9.35f), Point(20.56f, 9.0f), q);

    glColor3f(0.25f, 0.18f, 0.12f);
    drawMorphQuad(Point(2.8f, 1.0f), Point(2.8f, 2.4f), Point(4.2f, 2.4f), Point(4.2f, 1.0f),
                  Point(2.8f, 1.0f), Point(2.69f, 2.4f), Point(4.09f, 2.4f), Point(4.2f, 1.0f), q);

    drawMorphQuad(Point(9.8f, 1.0f), Point(9.8f, 2.5f), Point(11.2f, 2.5f), Point(11.2f, 1.0f),
                  Point(9.8f, 1.0f), Point(9.62f, 2.5f), Point(11.02f, 2.5f), Point(11.2f, 1.0f), q);

    drawMorphQuad(Point(16.8f, 1.0f), Point(16.8f, 2.4f), Point(18.2f, 2.4f), Point(18.2f, 1.0f),
                  Point(16.8f, 1.0f), Point(16.9f, 2.4f), Point(18.3f, 2.4f), Point(18.2f, 1.0f), q);

    drawWindow(Point(2.0f, 7.2f), Point(2.0f, 8.2f), Point(3.0f, 8.2f), Point(3.0f, 7.2f), -0.08f);
    drawWindow(Point(4.0f, 7.2f), Point(4.0f, 8.2f), Point(5.0f, 8.2f), Point(5.0f, 7.2f), -0.08f);
    drawWindow(Point(2.0f, 5.2f), Point(2.0f, 6.2f), Point(3.0f, 6.2f), Point(3.0f, 5.2f), -0.08f);
    drawWindow(Point(4.0f, 5.2f), Point(4.0f, 6.2f), Point(5.0f, 6.2f), Point(5.0f, 5.2f), -0.08f);
    drawWindow(Point(2.0f, 3.2f), Point(2.0f, 4.2f), Point(3.0f, 4.2f), Point(3.0f, 3.2f), -0.08f);
    drawWindow(Point(4.0f, 3.2f), Point(4.0f, 4.2f), Point(5.0f, 4.2f), Point(5.0f, 3.2f), -0.08f);

    drawWindow(Point(9.0f, 8.0f), Point(9.0f, 9.0f), Point(10.0f, 9.0f), Point(10.0f, 8.0f), -0.12f);
    drawWindow(Point(11.0f, 8.0f), Point(11.0f, 9.0f), Point(12.0f, 9.0f), Point(12.0f, 8.0f), -0.12f);
    drawWindow(Point(9.0f, 6.0f), Point(9.0f, 7.0f), Point(10.0f, 7.0f), Point(10.0f, 6.0f), -0.12f);
    drawWindow(Point(11.0f, 6.0f), Point(11.0f, 7.0f), Point(12.0f, 7.0f), Point(12.0f, 6.0f), -0.12f);
    drawWindow(Point(9.0f, 4.0f), Point(9.0f, 5.0f), Point(10.0f, 5.0f), Point(10.0f, 4.0f), -0.12f);
    drawWindow(Point(11.0f, 4.0f), Point(11.0f, 5.0f), Point(12.0f, 5.0f), Point(12.0f, 4.0f), -0.12f);

    drawWindow(Point(16.0f, 7.2f), Point(16.0f, 8.2f), Point(17.0f, 8.2f), Point(17.0f, 7.2f), 0.07f);
    drawWindow(Point(18.0f, 7.2f), Point(18.0f, 8.2f), Point(19.0f, 8.2f), Point(19.0f, 7.2f), 0.07f);
    drawWindow(Point(16.0f, 5.2f), Point(16.0f, 6.2f), Point(17.0f, 6.2f), Point(17.0f, 5.2f), 0.07f);
    drawWindow(Point(18.0f, 5.2f), Point(18.0f, 6.2f), Point(19.0f, 6.2f), Point(19.0f, 5.2f), 0.07f);
    drawWindow(Point(16.0f, 3.2f), Point(16.0f, 4.2f), Point(17.0f, 4.2f), Point(17.0f, 3.2f), 0.07f);
    drawWindow(Point(18.0f, 3.2f), Point(18.0f, 4.2f), Point(19.0f, 4.2f), Point(19.0f, 3.2f), 0.07f);

    drawFireInWindow(9.5f, 8.45f, 0.90f, f);
    drawFireInWindow(11.5f, 8.45f, 0.90f, f);
    drawFireInWindow(9.5f, 6.45f, 0.90f, f);
    drawFireInWindow(11.5f, 6.45f, 0.90f, f);
    drawFireInWindow(9.5f, 4.45f, 0.80f, f);
    //drawFireInWindow(11.5f, 4.45f, 0.80f, f);
}

void drawTree() {
    float q = earthquakeValue();
    float st = stormValue();
    float swing = sin(frameCount * 0.18f) * 15.0f * st;

    glColor3f(0.42f, 0.24f, 0.10f);
    drawQuad(Point(-1.6f, 0.7f), Point(-1.6f, 1.1f), Point(0.2f, 1.1f), Point(0.2f, 0.7f));

    glPushMatrix();
    glTranslatef(-0.7f, 1.1f, 0.0f);
    glRotatef((-14.0f * q) + swing, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.7f, -1.1f, 0.0f);

    glColor3f(0.36f, 0.20f, 0.08f);
    glBegin(GL_POLYGON);
        glVertex2f(-0.82f, 1.1f);
        glVertex2f(-0.913760f, 2.859693f);
        glVertex2f(-0.68f, 3.35f);
        glVertex2f(-0.53f, 4.0f);
        glVertex2f(-0.33f, 3.0f);
        glVertex2f(-0.18f, 3.55f);
        glVertex2f(0.02f, 4.05f);
        glVertex2f(0.18f, 3.35f);
        glVertex2f(0.04f, 2.8f);
        glVertex2f(0.04f, 1.1f);
        glVertex2f(-0.32f, 1.1f);
    glEnd();

    glColor3f(0.20f, 0.45f, 0.08f);
    glBegin(GL_POLYGON);
        glVertex2f(-2.226095f, 4.047655f);
        glVertex2f(-2.400470f, 4.471136f);
        glVertex2f(-2.288372f, 4.969349f);
        glVertex2f(-2.076632f, 5.343008f);
        glVertex2f(-1.790159f, 5.729123f);
        glVertex2f(-1.245659f, 5.893294f);
        glVertex2f(-0.483783f, 6.165321f);
        glVertex2f(0.0f, 6.0f);
        glVertex2f(0.622714f, 5.853783f);
        glVertex2f(1.095393f, 5.552987f);
        glVertex2f(1.320989f, 5.316648f);
        glVertex2f(1.28f, 4.90f);
        glVertex2f(1.369670f, 4.408811f);
        glVertex2f(1.147896f, 3.743489f);
        glVertex2f(0.472316f, 2.878055f);
        glVertex2f(-0.526754f, 2.598745f);
        glVertex2f(-1.665480f, 3.114395f);
        glVertex2f(-2.019989f, 3.522617f);
    glEnd();

    glPopMatrix();
}

void drawLamp() {
    float q = earthquakeValue();

    // Project Requirement 4: Transformation - rotate lamp around the bottom point.
    glPushMatrix();
    glTranslatef(14.18f, 1.0f, 0.0f);
    glRotatef(16.0f * q, 0.0f, 0.0f, 1.0f);
    glTranslatef(-14.18f, -1.0f, 0.0f);

    glColor3f(0.05f, 0.08f, 0.10f);
    glBegin(GL_QUADS);
        glVertex2f(14.1f, 1.0f);
        glVertex2f(14.25f, 1.0f);
        glVertex2f(14.25f, 3.5f);
        glVertex2f(14.1f, 3.5f);
    glEnd();

    glColor3f(1.0f, 0.85f, 0.30f);
    glBegin(GL_QUADS);
        glVertex2f(13.7f, 3.2f);
        glVertex2f(14.1f, 3.2f);
        glVertex2f(14.1f, 3.5f);
        glVertex2f(13.7f, 3.5f);
    glEnd();

    glPopMatrix();
}

void drawRedCar() {
    float q = earthquakeValue();
    float f = fireValue();
    float fl = floodValue();
    float st = stormValue();

    if(fl > 0.02f) {
        return;
    }

    float carX = 0.0f;
    float carY = 0.0f;
    float carAngle = 0.0f;

    if(st > 0.02f) {
        carX = 6.0f;
        carY = 1.26f;
        carAngle = 0.0f;
    } else {
        float normalCarX = carMove * (1.0f - f) + 6.0f * fl;
        carX = normalCarX * (1.0f - q) + 7.0f * q;
        carY = -1.55f * q;
        carAngle = -28.0f * q;
    }

    glPushMatrix();
    glTranslatef(carX, carY, 0.0f);
    glRotatef(carAngle, 0.0f, 0.0f, 1.0f);

    glColor3f(0.85f, 0.05f, 0.05f);
    glBegin(GL_POLYGON);
        glVertex2f(0.992592f, -0.601850f);
        glVertex2f(1.0f, -0.25f);
        glVertex2f(1.25f, 0.0f);
        glVertex2f(1.75f, 0.2f);
        glVertex2f(2.402458f, 0.215704f);
        glVertex2f(2.936372f, 0.541058f);
        glVertex2f(4.15f, 0.55f);
        glVertex2f(4.6f, 0.2f);
        glVertex2f(5.0f, 0.08f);
        glVertex2f(5.3f, -0.2f);
        glVertex2f(5.297271f, -0.576823f);
    glEnd();

    glColor3f(0.60f, 0.80f, 0.90f);
    glBegin(GL_QUADS);
        glVertex2f(3.07819f, 0.18233f);
        glVertex2f(3.395163f, 0.452668f);
        glVertex2f(4.11652f, 0.45399f);
        glVertex2f(4.32955f, 0.19068f);

        glVertex2f(2.458990f, 0.178237f);
        glVertex2f(2.950555f, 0.468980f);
        glVertex2f(3.292252f, 0.471977f);
        glVertex2f(3.07819f, 0.18233f);
    glEnd();

    glColor3f(1.0f, 0.95f, 0.45f);
    glBegin(GL_POLYGON);
        glVertex2f(1.2f, -0.2f);
        glVertex2f(1.0954f, -0.27504f);
        glVertex2f(1.39433f, -0.26383f);
        glVertex2f(1.4f, -0.2f);
    glEnd();

    glColor3f(0.02f, 0.02f, 0.02f);
    drawCircle(1.790571f, -0.602406f, 0.25f);
    drawCircle(4.406276f, -0.558444f, 0.25f);

    glColor3f(0.65f, 0.65f, 0.65f);
    drawCircle(1.790571f, -0.602406f, 0.12f);
    drawCircle(4.406276f, -0.558444f, 0.12f);

    // Project Requirement 3: Midpoint Circle Algorithm used for wheel outlines.
    glPointSize(2.0f);
    glColor3f(0.95f, 0.95f, 0.95f);
    drawMidPointCircle(1.790571f, -0.602406f, 0.25f);
    drawMidPointCircle(4.406276f, -0.558444f, 0.25f);

    glPopMatrix();
}

void drawPerson(Point start, Point end, float show, float direction, float r, float g, float b) {
    if(show <= 0.02f) {
        return;
    }

    Point p = mixPoint(start, end, show);
    float armSwing = sin(frameCount * 0.25f + p.x) * 0.16f;
    float legSwing = cos(frameCount * 0.25f + p.x) * 0.18f;

    glColor3f(0.95f, 0.70f, 0.48f);
    drawCircle(p.x, p.y + 0.95f, 0.16f);

    glColor3f(0.08f, 0.05f, 0.03f);
    glBegin(GL_POLYGON);
        glVertex2f(p.x - 0.12f, p.y + 0.98f);
        glVertex2f(p.x + 0.12f, p.y + 0.98f);
        glVertex2f(p.x + 0.10f, p.y + 1.09f);
        glVertex2f(p.x - 0.10f, p.y + 1.09f);
    glEnd();

    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_QUADS);
        glVertex2f(p.x - 0.045f, p.y + 0.79f);
        glVertex2f(p.x + 0.045f, p.y + 0.79f);
        glVertex2f(p.x + 0.045f, p.y + 0.69f);
        glVertex2f(p.x - 0.045f, p.y + 0.69f);
    glEnd();

    glColor3f(r, g, b);
    glBegin(GL_QUADS);
        glVertex2f(p.x - 0.18f, p.y + 0.69f);
        glVertex2f(p.x + 0.18f, p.y + 0.69f);
        glVertex2f(p.x + 0.18f, p.y + 0.22f);
        glVertex2f(p.x - 0.18f, p.y + 0.22f);
    glEnd();

    glColor3f(0.95f, 0.70f, 0.48f);
    glLineWidth(4.0f);
    glBegin(GL_LINES);
        glVertex2f(p.x - 0.16f, p.y + 0.60f);
        glVertex2f(p.x - 0.34f * direction, p.y + 0.38f + armSwing);
        glVertex2f(p.x + 0.16f, p.y + 0.60f);
        glVertex2f(p.x + 0.34f * direction, p.y + 0.38f - armSwing);
    glEnd();

    glColor3f(0.08f, 0.15f, 0.48f);
    glBegin(GL_QUADS);
        glVertex2f(p.x - 0.17f, p.y + 0.22f);
        glVertex2f(p.x + 0.17f, p.y + 0.22f);
        glVertex2f(p.x + 0.17f, p.y + 0.08f);
        glVertex2f(p.x - 0.17f, p.y + 0.08f);
    glEnd();

    glLineWidth(6.0f);
    glBegin(GL_LINES);
        glVertex2f(p.x - 0.07f, p.y + 0.08f);
        glVertex2f(p.x - 0.07f - 0.18f * direction, p.y - 0.30f - legSwing);
        glVertex2f(p.x + 0.07f, p.y + 0.08f);
        glVertex2f(p.x + 0.07f + 0.18f * direction, p.y - 0.30f + legSwing);
    glEnd();

    glColor3f(0.02f, 0.02f, 0.02f);
    glLineWidth(5.0f);
    glBegin(GL_LINES);
        glVertex2f(p.x - 0.07f - 0.18f * direction, p.y - 0.30f - legSwing);
        glVertex2f(p.x - 0.20f - 0.18f * direction, p.y - 0.31f - legSwing);
        glVertex2f(p.x + 0.07f + 0.18f * direction, p.y - 0.30f + legSwing);
        glVertex2f(p.x + 0.20f + 0.18f * direction, p.y - 0.31f + legSwing);
    glEnd();
}

void drawPeople() {
    float q = earthquakeValue();
    float f = fireValue();

    drawPerson(Point(3.5f, 1.2f), Point(6.2f, -1.55f), q, 1.0f, 0.95f, 0.10f, 0.10f);
    drawPerson(Point(10.5f, 1.2f), Point(12.2f, -1.15f), q, 1.0f, 0.10f, 0.60f, 0.95f);
    drawPerson(Point(17.5f, 1.2f), Point(15.55f, -1.65f), q, -1.0f, 0.98f, 0.85f, 0.10f);

    drawPerson(Point(13.5f, 1.1f), Point(8.8f, -2.25f), f, -1.0f, 0.10f, 0.85f, 0.30f);
    drawPerson(Point(11.2f, 1.1f), Point(13.8f, -2.05f), f, 1.0f, 0.75f, 0.20f, 0.90f);
    drawPerson(Point(16.0f, 1.1f), Point(17.4f, -2.45f), f, 1.0f, 0.95f, 0.40f, 0.10f);
}
//Fire truck
void drawFireTruck() {
    float f = fireValue();

    if(f <= 0.01f) {
        return;
    }

    float truckX = 10.0f * (1.0f - f);

    glPushMatrix();
    glTranslatef(truckX, 0.0f, 0.0f);

    glColor3f(0.85f, 0.02f, 0.02f);
    glBegin(GL_POLYGON);
        glVertex2f(10.0f, -1.0f);
        glVertex2f(12.0f, -1.0f);
        glVertex2f(15.98964f, -1.20576f);
        glVertex2f(16.00119f, -2.78821f);
        glVertex2f(9.8f, -2.8f);
        glVertex2f(9.8f, -2.0f);
    glEnd();

    glColor3f(0.95f, 0.10f, 0.10f);
    glBegin(GL_QUADS);
        glVertex2f(10.0f, -1.0f);
        glVertex2f(12.0f, -1.0f);
        glVertex2f(12.0f, -2.8f);
        glVertex2f(10.0f, -2.8f);
    glEnd();

    glColor3f(0.65f, 0.85f, 0.95f);
    drawQuad(Point(11.06741f, -1.25867f), Point(11.664331f, -1.262708f), Point(11.67643f, -1.95643f), Point(11.07547f, -1.96046f));

    glColor3f(0.35f, 0.35f, 0.35f);
    drawQuad(Point(12.29339f, -1.30971f), Point(12.90558f, -1.32127f), Point(12.904542f, -2.598512f), Point(12.292382f, -2.585418f));

    glColor3f(1.0f, 0.90f, 0.10f);
    glBegin(GL_POLYGON);
        glVertex2f(10.70478f, -0.79873f);
        glVertex2f(11.13313f, -0.79873f);
        glVertex2f(11.16608f, -0.95841f);
        glVertex2f(10.65662f, -0.95588f);
    glEnd();

    glColor3f(0.02f, 0.02f, 0.02f);
    drawCircle(10.926243f, -2.803416f, 0.36f);
    drawCircle(14.199488f, -2.793518f, 0.36f);
    drawCircle(15.19264f, -2.76511f, 0.36f);

    glColor3f(0.70f, 0.70f, 0.70f);
    drawCircle(10.926243f, -2.803416f, 0.16f);
    drawCircle(14.199488f, -2.793518f, 0.16f);
    drawCircle(15.19264f, -2.76511f, 0.16f);

    glColor3f(0.95f, 0.95f, 0.75f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
        glVertex2f(11.6497f, -0.75443f);
        glVertex2f(15.81789f, -1.13434f);
        glVertex2f(11.65513f, -0.8684f);
        glVertex2f(15.82331f, -1.0258f);
        glVertex2f(11.65513f, -0.35281f);
        glVertex2f(15.87216f, -0.60789f);
        glVertex2f(11.66055f, -0.47764f);
        glVertex2f(15.8613f, -0.70016f);
    glEnd();

    glColor3f(0.30f, 0.60f, 1.0f);
    glLineWidth(4.0f);
    glBegin(GL_LINES);
        glVertex2f(12.0f, -1.0f);
        glVertex2f(10.2f, 6.2f);
    glEnd();

    glColor3f(0.70f, 0.90f, 1.0f);
    glBegin(GL_TRIANGLES);
        glVertex2f(10.2f, 6.2f);
        glVertex2f(9.8f, 6.8f);
        glVertex2f(10.6f, 6.8f);
    glEnd();

    glPopMatrix();
}

//Flood scene

void drawFloodWater() {
    float fl = floodValue();

    if(fl <= 0.01f) {
        return;
    }

    float waterLevel = -3.9f + 5.5f * fl;
    float wave = sin(frameCount * 0.12f) * 0.08f * fl;

    float r = 0.05f + (fl * 0.35f);
        float g = 0.40f + (fl * 0.15f);
        float b = 0.82f - (fl * 0.40f);

    glColor3f(0.05f, 0.48f, 0.82f);
    glRectf(-4.0f, -3.9f, 22.0f, waterLevel);
    glBegin(GL_POLYGON);
        glVertex2f(-4.0f, -3.9f);
        glVertex2f(22.0f, -3.9f);
        for(float x = 22.0f; x >= -4.0f; x -= 1.0f) {
            float y = waterLevel + sin(x * 0.9f + frameCount * 0.08f) * 0.10f * fl + wave;
            glVertex2f(x, y);
        }
    glEnd();

    glColor3f(r + 0.1f, g + 0.1f, b + 0.1f);
    glLineWidth(2.0f);
    for(int row = 0; row < 4; row++) {
        float y = waterLevel - 0.28f - row * 0.58f;
        if(y < -3.7f) {
            continue;
        }

        glBegin(GL_LINE_STRIP);
            for(float x = -4.0f; x <= 22.0f; x += 0.45f) {
                glVertex2f(x, y + sin(x * 1.4f + frameCount * 0.10f + row) * 0.06f);
            }
        glEnd();
    }

    glColor3f(r + 0.2f, g + 0.2f, b + 0.1f);
    glBegin(GL_QUADS);
        glVertex2f(-4.0f, waterLevel - 0.12f);
        glVertex2f(22.0f, waterLevel - 0.12f);
        glVertex2f(22.0f, waterLevel + 0.12f);
        glVertex2f(-4.0f, waterLevel + 0.12f);
    glEnd();
}

void drawFloodedCar() {
    float fl = floodValue();

    if(fl <= 0.04f) {
        return;
    }
    float waterLevel = -3.9f + 5.5f * fl;
    float bob = sin(frameCount * 0.10f) * 0.05f * fl;
    float carX = 4.0f;
    float carY = (fl > 0.4f) ? (waterLevel - 0.1f + bob) : (0.5f + bob);
    float localClipY = waterLevel - carY;
    double plane[] = {0.0, 1.0, 0.0, -localClipY};

    glPushMatrix();
    glTranslatef(carX, carY, 0.0f);
    // Bonus Requirement: Clipping - only the part of the car above water is visible.
    glClipPlane(GL_CLIP_PLANE0, plane);
    glEnable(GL_CLIP_PLANE0);

    glColor3f(0.85f, 0.05f, 0.05f);
    glBegin(GL_POLYGON);
        glVertex2f(0.992592f, -0.601850f);
        glVertex2f(1.0f, -0.25f);
        glVertex2f(1.25f, 0.0f);
        glVertex2f(1.75f, 0.2f);
        glVertex2f(2.402458f, 0.215704f);
        glVertex2f(2.936372f, 0.541058f);
        glVertex2f(4.15f, 0.55f);
        glVertex2f(4.6f, 0.2f);
        glVertex2f(5.0f, 0.08f);
        glVertex2f(5.3f, -0.2f);
        glVertex2f(5.297271f, -0.576823f);
    glEnd();

    glColor3f(0.60f, 0.80f, 0.90f);
    glBegin(GL_QUADS);
        glVertex2f(3.07819f, 0.18233f);
        glVertex2f(3.395163f, 0.452668f);
        glVertex2f(4.11652f, 0.45399f);
        glVertex2f(4.32955f, 0.19068f);

        glVertex2f(2.458990f, 0.178237f);
        glVertex2f(2.950555f, 0.468980f);
        glVertex2f(3.292252f, 0.471977f);
        glVertex2f(3.07819f, 0.18233f);
    glEnd();

    glColor3f(1.0f, 0.95f, 0.45f);
    glBegin(GL_POLYGON);
        glVertex2f(1.2f, -0.2f);
        glVertex2f(1.0954f, -0.27504f);
        glVertex2f(1.39433f, -0.26383f);
        glVertex2f(1.4f, -0.2f);
    glEnd();

    glColor3f(0.02f, 0.02f, 0.02f);
    drawCircle(1.790571f, -0.602406f, 0.25f);
    drawCircle(4.406276f, -0.558444f, 0.25f);

    glColor3f(0.65f, 0.65f, 0.65f);
    drawCircle(1.790571f, -0.602406f, 0.12f);
    drawCircle(4.406276f, -0.558444f, 0.12f);

    // Project Requirement 3: Midpoint Circle Algorithm used for wheel outlines.
    glPointSize(2.0f);
    glColor3f(0.95f, 0.95f, 0.95f);
    drawMidPointCircle(1.790571f, -0.602406f, 0.25f);
    drawMidPointCircle(4.406276f, -0.558444f, 0.25f);

    glDisable(GL_CLIP_PLANE0);
    glPopMatrix();
}

void drawRescueText(float x, float y) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);

    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(0.12f, 0.12f, 1.0f);

    /*glBegin(GL_LINE_STRIP);
        glVertex2f(0, 0); glVertex2f(0, 2); glVertex2f(1, 2);
        glVertex2f(1, 1); glVertex2f(0, 1); glVertex2f(1, 0);
    glEnd();*/

    glPopMatrix();
}
void renderText(float x, float y, const char* text) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
}

void drawRescueBoat() {
    float fl = floodValue();
    if(fl <= 0.3f) return;

    float waterLevel = -3.9f + 5.5f * fl;
    float boatX = 10.0f + sin(frameCount * 0.01f) * 3.0f;
    glPushMatrix();
    glTranslatef(boatX, waterLevel - 0.2f, 0.0f);

    glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.5f, 0.0f); glVertex2f(-2.0f, 0.0f);
        glColor3f(1.0f, 0.4f, 0.0f); glVertex2f(2.0f, 0.0f);
        glColor3f(0.6f, 0.2f, 0.0f); glVertex2f(1.5f, -0.7f);
        glColor3f(0.6f, 0.2f, 0.0f); glVertex2f(-1.5f, -0.7f);
    glEnd();

    renderText(-0.8f, -0.35f, "RESCUE TEAM");

//people on boat
    drawWindowWatcher(-0.8f, 0.2f, 0.2f, 0.2f, 0.2f);
    drawWindowWatcher(0.0f, 0.1f, 0.0f, 0.5f, 0.0f);
    drawWindowWatcher(0.8f, 0.2f, 0.2f, 0.2f, 0.2f);

    glPopMatrix();
}
void drawDebris(float x, float offsetZ) {
    float fl = floodValue();
    if(fl <= 0.2f) return;

    float waterLevel = -3.9f + 5.0f * fl;
    float bob = sin(frameCount * 0.05f + offsetZ) * 0.1f;

    glPushMatrix();
    glTranslatef(x + sin(frameCount*0.01f), waterLevel + bob, 0.0f);

    //wood
    glColor3f(0.4f, 0.2f, 0.0f);
    glBegin(GL_QUADS);
            glColor3f(0.5f, 0.3f, 0.1f); glVertex2f(-0.5f, 0.05f);
            glColor3f(0.5f, 0.3f, 0.1f); glVertex2f(0.5f, 0.05f);
            glColor3f(0.2f, 0.1f, 0.0f); glVertex2f(0.5f, -0.05f);
            glColor3f(0.2f, 0.1f, 0.0f); glVertex2f(-0.5f, -0.05f);
    glEnd();
    glPopMatrix();
}
void drawFloatingDebris() {
    float fl = floodValue();
    if (fl <= 0.15f) return;

    float waterLevel = -3.9f + 5.5f * fl;
    float waveOffset = sin(frameCount * 0.12f) * 0.08f * fl;

    auto drawBottle = [&](float x, float yOffset, float r, float g, float b) {
        float y = (waterLevel - yOffset) + waveOffset;
        glColor3f(r, g, b);
        glBegin(GL_QUADS);
            glVertex2f(x, y);
            glVertex2f(x + 0.3f, y);
            glVertex2f(x + 0.3f, y + 0.15f);
            glVertex2f(x, y + 0.15f);
        glEnd();

        glColor3f(0.2f, 0.2f, 0.2f);
        glRectf(x + 0.3f, y + 0.05f, x + 0.4f, y + 0.1f);
    };

    drawBottle(-3.0f, 1.5f, 0.0f, 0.7f, 0.9f);
        drawBottle(8.0f, 2.2f, 0.1f, 0.8f, 0.2f);
        drawBottle(18.0f, 1.0f, 0.9f, 0.2f, 0.2f);

    auto drawPaper = [&](float x) {
        float y = waterLevel - 0.02f + waveOffset;
        glColor3f(0.85f, 0.85f, 0.85f);
        glBegin(GL_QUADS);
            glVertex2f(x, y);
            glVertex2f(x + 0.5f, y + 0.05f);
            glVertex2f(x + 0.45f, y + 0.25f);
            glVertex2f(x - 0.05f, y + 0.2f);
        glEnd();
    };

    drawPaper(15.0f);
    drawPaper(-1.0f);
}
void drawHeavyRain() {
    float st = floodValue();
   if(st <= 0.1f) return;

    glColor3f(0.8f, 0.8f, 1.0f);
    glLineWidth(1.5f);
    for(int i = 0; i < 100; i++) {
        float rx = -4.0f + (float)(rand() % 260) / 10.0f;
        float ry = (float)(rand() % 160) / 10.0f - 4.0f;
        // DDA Line
        drawDDA(rx, ry, rx - 0.1f, ry - 0.4f);
    }
}

void drawWindowWatcher(float x, float y, float shirtR, float shirtG, float shirtB) {
    float fl = floodValue();

    if(fl <= 0.02f) {
        return;
    }

    float show = limit01(fl * 1.6f);
    float move = sin(frameCount * 0.09f + x) * 0.02f * show;

    glColor3f(shirtR, shirtG, shirtB);
    glBegin(GL_QUADS);
        glVertex2f(x - 0.16f + move, y - 0.28f);
        glVertex2f(x + 0.16f + move, y - 0.28f);
        glVertex2f(x + 0.16f + move, y + 0.03f);
        glVertex2f(x - 0.16f + move, y + 0.03f);
    glEnd();

    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_QUADS);
        glVertex2f(x - 0.035f + move, y + 0.03f);
        glVertex2f(x + 0.035f + move, y + 0.03f);
        glVertex2f(x + 0.035f + move, y + 0.10f);
        glVertex2f(x - 0.035f + move, y + 0.10f);
    glEnd();

    glColor3f(0.95f, 0.70f, 0.48f);
    drawCircle(x + move, y + 0.18f, 0.11f);

    glColor3f(0.06f, 0.04f, 0.03f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.10f + move, y + 0.20f);
        glVertex2f(x + 0.10f + move, y + 0.20f);
        glVertex2f(x + 0.08f + move, y + 0.30f);
        glVertex2f(x - 0.08f + move, y + 0.30f);
    glEnd();

    glColor3f(0.95f, 0.70f, 0.48f);
    glLineWidth(2.5f);
    glBegin(GL_LINES);
        glVertex2f(x - 0.12f + move, y - 0.04f);
        glVertex2f(x - 0.25f + move, y - 0.16f);
        glVertex2f(x + 0.12f + move, y - 0.04f);
        glVertex2f(x + 0.25f + move, y - 0.16f);
    glEnd();

    glColor3f(0.18f, 0.18f, 0.18f);
    glPointSize(2.0f);
    glBegin(GL_POINTS);
        glVertex2f(x - 0.03f + move, y + 0.18f);
        glVertex2f(x + 0.03f + move, y + 0.18f);
    glEnd();
}

void drawFloodWindowPeople() {
    float fl = floodValue();

    if(fl <= 0.02f) {
        return;
    }

    drawWindowWatcher(2.50f, 7.58f, 0.95f, 0.15f, 0.15f);
    drawWindowWatcher(4.50f, 7.58f, 0.15f, 0.60f, 0.95f);
    drawWindowWatcher(9.50f, 8.35f, 0.95f, 0.80f, 0.10f);
    drawWindowWatcher(11.50f, 8.35f, 0.25f, 0.85f, 0.30f);
    drawWindowWatcher(9.50f, 6.35f, 0.75f, 0.25f, 0.90f);
    drawWindowWatcher(11.50f, 6.35f, 0.95f, 0.40f, 0.10f);
    drawWindowWatcher(16.50f, 7.58f, 0.20f, 0.75f, 0.95f);
    drawWindowWatcher(18.50f, 7.58f, 0.95f, 0.20f, 0.35f);
}

void drawCloud(float x, float y, float s) {
    glColor3f(0.48f, 0.50f, 0.54f);
    drawCircle(x, y, 0.55f * s);
    drawCircle(x + 0.55f * s, y + 0.12f * s, 0.70f * s);
    drawCircle(x + 1.25f * s, y, 0.55f * s);
    drawCircle(x + 0.25f * s, y - 0.28f * s, 0.55f * s);
    drawCircle(x + 0.95f * s, y - 0.28f * s, 0.55f * s);

    glBegin(GL_QUADS);
        glVertex2f(x - 0.45f * s, y - 0.55f * s);
        glVertex2f(x + 1.65f * s, y - 0.55f * s);
        glVertex2f(x + 1.65f * s, y + 0.10f * s);
        glVertex2f(x - 0.45f * s, y + 0.10f * s);
    glEnd();

    glColor3f(0.62f, 0.64f, 0.68f);
    glPointSize(2.0f);
    drawMidPointCircle(x, y, 0.55f * s);
    drawMidPointCircle(x + 0.55f * s, y + 0.12f * s, 0.70f * s);
    drawMidPointCircle(x + 1.25f * s, y, 0.55f * s);
}

//Storm scene
void drawStormSky() {
    float st = stormValue();

    if(st <= 0.01f) {
        return;
    }

    glColor3f(0.12f, 0.15f, 0.20f);
    glBegin(GL_QUADS);
        glVertex2f(-4.0f, 1.0f);
        glVertex2f(22.0f, 1.0f);
        glVertex2f(22.0f, 12.0f);
        glVertex2f(-4.0f, 12.0f);
    glEnd();

    drawCloud(-2.5f, 10.2f, 1.55f);
    drawCloud(2.2f, 10.6f, 1.30f);
    drawCloud(6.8f, 10.1f, 1.65f);
    drawCloud(12.4f, 10.8f, 1.55f);
    drawCloud(17.0f, 10.2f, 1.50f);
}

void drawLightning() {
    float st = stormValue();

    if(st <= 0.01f) {
        return;
    }

    int flash = frameCount % 95;
    if(flash > 8) {
        return;
    }

    // Project Requirement 2: Midpoint Line Algorithm used for lightning.
    glPointSize(5.0f);
    glColor3f(1.0f, 1.0f, 0.55f);
    drawMidPointLine(13.8f, 10.5f, 12.8f, 8.8f);
    drawMidPointLine(12.8f, 8.8f, 13.6f, 8.8f);
    drawMidPointLine(13.6f, 8.8f, 12.4f, 6.6f);
    drawMidPointLine(13.0f, 7.9f, 12.2f, 7.9f);

    glPointSize(2.0f);
    glColor3f(1.0f, 1.0f, 0.90f);
    drawMidPointLine(13.8f, 10.5f, 12.8f, 8.8f);
    drawMidPointLine(12.8f, 8.8f, 13.6f, 8.8f);
    drawMidPointLine(13.6f, 8.8f, 12.4f, 6.6f);
}

void drawRain() {
    float st = stormValue();

    if(st <= 0.01f) {
        return;
    }

    // Project Requirement 1: DDA Algorithm used for each rain drop.
    glColor3f(0.70f, 0.88f, 1.0f);
    glPointSize(2.0f);
    for(int i = 0; i < 110; i++) {
        float x = -4.0f + (i % 22) * 1.25f;
        float baseY = -3.6f + (i / 22) * 2.9f;
        float fall = fmod(frameCount * 0.20f + i * 0.37f, 3.0f);
        float y = baseY - fall;
        drawDDA(x + 0.25f * st, y + 0.85f, x - 0.22f * st, y + 0.05f);
    }
}

void drawUmbrella(float x, float y, float s, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for(int i = 0; i <= 24; i++) {
            float angle = 3.1416f - i * 3.1416f / 24.0f;
            glVertex2f(x + cos(angle) * 0.75f * s, y + sin(angle) * 0.52f * s);
        }
    glEnd();

    glColor3f(0.08f, 0.08f, 0.08f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        for(int i = 0; i <= 24; i++) {
            float angle = 3.1416f - i * 3.1416f / 24.0f;
            glVertex2f(x + cos(angle) * 0.75f * s, y + sin(angle) * 0.52f * s);
        }
    glEnd();

    glBegin(GL_LINES);
        glVertex2f(x, y);
        glVertex2f(x, y - 1.10f * s);
        glVertex2f(x, y);
        glVertex2f(x - 0.55f * s, y + 0.10f * s);
        glVertex2f(x, y);
        glVertex2f(x + 0.55f * s, y + 0.10f * s);
    glEnd();
}

void drawUmbrellaPerson(float x, float y, float s, float dir, float shirtR, float shirtG, float shirtB, float umbrellaR, float umbrellaG, float umbrellaB) {
    float st = stormValue();

    if(st <= 0.01f) {
        return;
    }

    float sway = sin(frameCount * 0.12f + x) * 0.06f * st;
    x += sway;

    drawUmbrella(x + 0.05f * dir, y + 1.58f * s, s, umbrellaR, umbrellaG, umbrellaB);

    glColor3f(0.95f, 0.70f, 0.48f);
    drawCircle(x, y + 1.02f * s, 0.16f * s);

    glColor3f(0.05f, 0.035f, 0.02f);
    drawCircle(x, y + 1.12f * s, 0.12f * s);

    glColor3f(shirtR, shirtG, shirtB);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.24f * s, y + 0.82f * s);
        glVertex2f(x + 0.24f * s, y + 0.82f * s);
        glVertex2f(x + 0.19f * s, y + 0.30f * s);
        glVertex2f(x - 0.19f * s, y + 0.30f * s);
    glEnd();

    glColor3f(0.95f, 0.70f, 0.48f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
        glVertex2f(x + 0.15f * s, y + 0.70f * s);
        glVertex2f(x + 0.08f * dir * s, y + 1.05f * s);
        glVertex2f(x - 0.15f * s, y + 0.70f * s);
        glVertex2f(x - 0.40f * dir * s, y + 0.48f * s);
    glEnd();

    glColor3f(0.05f, 0.12f, 0.40f);
    glLineWidth(5.0f);
    glBegin(GL_LINES);
        glVertex2f(x - 0.08f * s, y + 0.30f * s);
        glVertex2f(x - 0.25f * s, y - 0.25f * s);
        glVertex2f(x + 0.08f * s, y + 0.30f * s);
        glVertex2f(x + 0.25f * s, y - 0.25f * s);
    glEnd();

    glColor3f(0.02f, 0.02f, 0.02f);
    glLineWidth(4.0f);
    glBegin(GL_LINES);
        glVertex2f(x - 0.25f * s, y - 0.25f * s);
        glVertex2f(x - 0.43f * s, y - 0.25f * s);
        glVertex2f(x + 0.25f * s, y - 0.25f * s);
        glVertex2f(x + 0.43f * s, y - 0.25f * s);
    glEnd();
}

void drawStormPeople() {
    float st = stormValue();

    if(st <= 0.01f) {
        return;
    }

    float x1 = 3.5f + 0.55f * st;
    float y1 = 0.92f - 0.12f * st;
    float x2 = 10.5f - 0.55f * st;
    float y2 = 0.92f - 0.12f * st;

    drawUmbrellaPerson(x1, y1, 0.88f, 1.0f, 0.95f, 0.25f, 0.18f, 0.85f, 0.10f, 0.16f);
    drawUmbrellaPerson(x2, y2, 0.88f, -1.0f, 0.20f, 0.55f, 0.95f, 0.12f, 0.20f, 0.75f);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float q = earthquakeValue();
    float shakeX = sin(frameCount * 0.45f) * 0.04f * q;
    float shakeY = cos(frameCount * 0.39f) * 0.025f * q;

    glTranslatef(shakeX, shakeY, 0.0f);

    drawBackground();
    drawStormSky();
    drawRoadCrack();
    draw3DDebrisCube(earthquakeValue());
    drawBuildings();
    //human help
    drawHelpHumanInWindow6();
    drawTree();
    drawLamp();
    drawPeople();
    drawRedCar();
    drawFireTruck();
    drawFloodWater();
    drawFloodedCar();
    drawRescueBoat();
    drawDebris(5.0f, 1.0f);
    drawDebris(15.0f, 2.0f);
    drawFloatingDebris();
    drawHeavyRain();
    drawFloodWindowPeople();
    drawStormPeople();
    drawRain();
    drawLightning();

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    // Project Requirement 6: Keyboard Controlling
    switch(key) {
        case 'd':
        case 'D':
            if(targetScene < 4.0f) {
                targetScene += 1.0f;
            }
            break;

        case 'a':
        case 'A':
            if(targetScene > 0.0f) {
                targetScene -= 1.0f;
            }
            break;

        case 'x':
        case 'X':
            exit(0);
    }

    glutPostRedisplay();
}

void update(int value) {
    frameCount++;

    if(scene < targetScene) {
        scene += 0.02f;
        if(scene > targetScene) {
            scene = targetScene;
        }
    }

    if(scene > targetScene) {
        scene -= 0.02f;
        if(scene < targetScene) {
            scene = targetScene;
        }
    }

    if(targetScene == 0.0f && scene < 0.05f) {
        carMove -= 0.04f;
        if(carMove < -10.0f) {
            carMove = 23.0f;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(30, update, 0);
}

void init() {
    glClearColor(0.45f, 0.78f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-4.0f, 22.0f, -3.9f, 12.0f);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(900, 600);
    glutCreateWindow("City of Disaster");

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(30, update, 0);

    glutMainLoop();
    return 0;
}
