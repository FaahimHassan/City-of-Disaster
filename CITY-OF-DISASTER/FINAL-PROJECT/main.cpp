#define GL_SILENCE_DEPRECATION

#include <windows.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <math.h>

float angle = 0.0f;


float scene = 0.0f;
float targetScene = 0.0f;
float carMove = 23.0f;
int frameCount = 0;

float doorRunnerProgress = 0.0f;

int doorRunnerWait = 0;

int waterOn = 0;
float waterEndX = 10.2f;

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



void drawCircle(float cx, float cy, float r) {
    glBegin(GL_POLYGON);
    for(int i = 0; i < 60; i++) {
        float theta = i * 2.0f * 3.1416f / 60.0f;
        glVertex2f(cx + r * cos(theta), cy + r * sin(theta));
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
//road line
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

    float cx = 9.4f;
    float cy = -1.2f;

    float outerQ = q * 1.18f;
    float innerQ = q * 0.78f;

    // ---------------- Shadow ----------------
    glColor3f(0.10f, 0.10f, 0.10f);
    glBegin(GL_POLYGON);
        glVertex2f(cx + (8.30f - cx) * outerQ + 0.18f * q, cy + (0.25f - cy) * outerQ - 0.25f * q);
        glVertex2f(cx + (8.05f - cx) * outerQ + 0.18f * q, cy + (-0.25f - cy) * outerQ - 0.25f * q);
        glVertex2f(cx + (8.75f - cx) * outerQ + 0.18f * q, cy + (-0.65f - cy) * outerQ - 0.25f * q);
        glVertex2f(cx + (7.85f - cx) * outerQ + 0.18f * q, cy + (-0.95f - cy) * outerQ - 0.25f * q);
        glVertex2f(cx + (8.65f - cx) * outerQ + 0.18f * q, cy + (-1.35f - cy) * outerQ - 0.25f * q);
        glVertex2f(cx + (7.65f - cx) * outerQ + 0.18f * q, cy + (-2.05f - cy) * outerQ - 0.25f * q);
        glVertex2f(cx + (8.80f - cx) * outerQ + 0.18f * q, cy + (-2.35f - cy) * outerQ - 0.25f * q);
        glVertex2f(cx + (7.90f - cx) * outerQ + 0.18f * q, cy + (-3.15f - cy) * outerQ - 0.25f * q);
        glVertex2f(cx + (9.35f - cx) * outerQ + 0.18f * q, cy + (-2.55f - cy) * outerQ - 0.25f * q);
        glVertex2f(cx + (10.95f - cx) * outerQ + 0.18f * q, cy + (-2.95f - cy) * outerQ - 0.25f * q);
        glVertex2f(cx + (12.25f - cx) * outerQ + 0.18f * q, cy + (-2.85f - cy) * outerQ - 0.25f * q);
        glVertex2f(cx + (11.25f - cx) * outerQ + 0.18f * q, cy + (-2.35f - cy) * outerQ - 0.25f * q);
        glVertex2f(cx + (12.05f - cx) * outerQ + 0.18f * q, cy + (-1.95f - cy) * outerQ - 0.25f * q);
        glVertex2f(cx + (10.35f - cx) * outerQ + 0.18f * q, cy + (-1.45f - cy) * outerQ - 0.25f * q);
        glVertex2f(cx + (11.35f - cx) * outerQ + 0.18f * q, cy + (-1.00f - cy) * outerQ - 0.25f * q);
        glVertex2f(cx + (10.25f - cx) * outerQ + 0.18f * q, cy + (-0.55f - cy) * outerQ - 0.25f * q);
        glVertex2f(cx + (10.80f - cx) * outerQ + 0.18f * q, cy + (0.05f - cy) * outerQ - 0.25f * q);
        glVertex2f(cx + (9.75f - cx) * outerQ + 0.18f * q, cy + (-0.20f - cy) * outerQ - 0.25f * q);
    glEnd();

    // ---------------- Outer Crack Body ----------------
    glColor3f(0.28f, 0.28f, 0.28f);
    glBegin(GL_POLYGON);
        glVertex2f(cx + (8.30f - cx) * outerQ, cy + (0.25f - cy) * outerQ);
        glVertex2f(cx + (8.05f - cx) * outerQ, cy + (-0.25f - cy) * outerQ);
        glVertex2f(cx + (8.75f - cx) * outerQ, cy + (-0.65f - cy) * outerQ);
        glVertex2f(cx + (7.85f - cx) * outerQ, cy + (-0.95f - cy) * outerQ);
        glVertex2f(cx + (8.65f - cx) * outerQ, cy + (-1.35f - cy) * outerQ);
        glVertex2f(cx + (7.65f - cx) * outerQ, cy + (-2.05f - cy) * outerQ);
        glVertex2f(cx + (8.80f - cx) * outerQ, cy + (-2.35f - cy) * outerQ);
        glVertex2f(cx + (7.90f - cx) * outerQ, cy + (-3.15f - cy) * outerQ);
        glVertex2f(cx + (9.35f - cx) * outerQ, cy + (-2.55f - cy) * outerQ);
        glVertex2f(cx + (10.95f - cx) * outerQ, cy + (-2.95f - cy) * outerQ);
        glVertex2f(cx + (12.25f - cx) * outerQ, cy + (-2.85f - cy) * outerQ);
        glVertex2f(cx + (11.25f - cx) * outerQ, cy + (-2.35f - cy) * outerQ);
        glVertex2f(cx + (12.05f - cx) * outerQ, cy + (-1.95f - cy) * outerQ);
        glVertex2f(cx + (10.35f - cx) * outerQ, cy + (-1.45f - cy) * outerQ);
        glVertex2f(cx + (11.35f - cx) * outerQ, cy + (-1.00f - cy) * outerQ);
        glVertex2f(cx + (10.25f - cx) * outerQ, cy + (-0.55f - cy) * outerQ);
        glVertex2f(cx + (10.80f - cx) * outerQ, cy + (0.05f - cy) * outerQ);
        glVertex2f(cx + (9.75f - cx) * outerQ, cy + (-0.20f - cy) * outerQ);
    glEnd();

    // ---------------- Inner Dark Hole ----------------
    glColor3f(0.04f, 0.04f, 0.04f);
    glBegin(GL_POLYGON);
        glVertex2f(cx + (8.30f - cx) * innerQ, cy + (0.25f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (8.05f - cx) * innerQ, cy + (-0.25f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (8.75f - cx) * innerQ, cy + (-0.65f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (7.85f - cx) * innerQ, cy + (-0.95f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (8.65f - cx) * innerQ, cy + (-1.35f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (7.65f - cx) * innerQ, cy + (-2.05f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (8.80f - cx) * innerQ, cy + (-2.35f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (7.90f - cx) * innerQ, cy + (-3.15f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (9.35f - cx) * innerQ, cy + (-2.55f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (10.95f - cx) * innerQ, cy + (-2.95f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (12.25f - cx) * innerQ, cy + (-2.85f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (11.25f - cx) * innerQ, cy + (-2.35f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (12.05f - cx) * innerQ, cy + (-1.95f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (10.35f - cx) * innerQ, cy + (-1.45f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (11.35f - cx) * innerQ, cy + (-1.00f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (10.25f - cx) * innerQ, cy + (-0.55f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (10.80f - cx) * innerQ, cy + (0.05f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (9.75f - cx) * innerQ, cy + (-0.20f - cy) * innerQ - 0.55f * q);
    glEnd();

    // ---------------- Outer Border ----------------
    glColor3f(0.70f, 0.70f, 0.70f);
    glLineWidth(3.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(cx + (8.30f - cx) * outerQ, cy + (0.25f - cy) * outerQ);
        glVertex2f(cx + (8.05f - cx) * outerQ, cy + (-0.25f - cy) * outerQ);
        glVertex2f(cx + (8.75f - cx) * outerQ, cy + (-0.65f - cy) * outerQ);
        glVertex2f(cx + (7.85f - cx) * outerQ, cy + (-0.95f - cy) * outerQ);
        glVertex2f(cx + (8.65f - cx) * outerQ, cy + (-1.35f - cy) * outerQ);
        glVertex2f(cx + (7.65f - cx) * outerQ, cy + (-2.05f - cy) * outerQ);
        glVertex2f(cx + (8.80f - cx) * outerQ, cy + (-2.35f - cy) * outerQ);
        glVertex2f(cx + (7.90f - cx) * outerQ, cy + (-3.15f - cy) * outerQ);
        glVertex2f(cx + (9.35f - cx) * outerQ, cy + (-2.55f - cy) * outerQ);
        glVertex2f(cx + (10.95f - cx) * outerQ, cy + (-2.95f - cy) * outerQ);
        glVertex2f(cx + (12.25f - cx) * outerQ, cy + (-2.85f - cy) * outerQ);
        glVertex2f(cx + (11.25f - cx) * outerQ, cy + (-2.35f - cy) * outerQ);
        glVertex2f(cx + (12.05f - cx) * outerQ, cy + (-1.95f - cy) * outerQ);
        glVertex2f(cx + (10.35f - cx) * outerQ, cy + (-1.45f - cy) * outerQ);
        glVertex2f(cx + (11.35f - cx) * outerQ, cy + (-1.00f - cy) * outerQ);
        glVertex2f(cx + (10.25f - cx) * outerQ, cy + (-0.55f - cy) * outerQ);
        glVertex2f(cx + (10.80f - cx) * outerQ, cy + (0.05f - cy) * outerQ);
        glVertex2f(cx + (9.75f - cx) * outerQ, cy + (-0.20f - cy) * outerQ);
    glEnd();

    // ---------------- Inner Border ----------------
    glColor3f(0.16f, 0.16f, 0.16f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(cx + (8.30f - cx) * innerQ, cy + (0.25f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (8.05f - cx) * innerQ, cy + (-0.25f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (8.75f - cx) * innerQ, cy + (-0.65f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (7.85f - cx) * innerQ, cy + (-0.95f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (8.65f - cx) * innerQ, cy + (-1.35f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (7.65f - cx) * innerQ, cy + (-2.05f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (8.80f - cx) * innerQ, cy + (-2.35f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (7.90f - cx) * innerQ, cy + (-3.15f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (9.35f - cx) * innerQ, cy + (-2.55f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (10.95f - cx) * innerQ, cy + (-2.95f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (12.25f - cx) * innerQ, cy + (-2.85f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (11.25f - cx) * innerQ, cy + (-2.35f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (12.05f - cx) * innerQ, cy + (-1.95f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (10.35f - cx) * innerQ, cy + (-1.45f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (11.35f - cx) * innerQ, cy + (-1.00f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (10.25f - cx) * innerQ, cy + (-0.55f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (10.80f - cx) * innerQ, cy + (0.05f - cy) * innerQ - 0.55f * q);
        glVertex2f(cx + (9.75f - cx) * innerQ, cy + (-0.20f - cy) * innerQ - 0.55f * q);
    glEnd();

    // ---------------- Highlight Edge ----------------
    glColor3f(0.82f, 0.82f, 0.82f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
        glVertex2f(cx + (8.30f - cx) * outerQ, cy + (0.25f - cy) * outerQ);
        glVertex2f(cx + (8.05f - cx) * outerQ, cy + (-0.25f - cy) * outerQ);
        glVertex2f(cx + (8.75f - cx) * outerQ, cy + (-0.65f - cy) * outerQ);
        glVertex2f(cx + (7.85f - cx) * outerQ, cy + (-0.95f - cy) * outerQ);
        glVertex2f(cx + (8.65f - cx) * outerQ, cy + (-1.35f - cy) * outerQ);
        glVertex2f(cx + (7.65f - cx) * outerQ, cy + (-2.05f - cy) * outerQ);
        glVertex2f(cx + (8.80f - cx) * outerQ, cy + (-2.35f - cy) * outerQ);
        glVertex2f(cx + (7.90f - cx) * outerQ, cy + (-3.15f - cy) * outerQ);
    glEnd();

    // ---------------- Main Crack Lines ----------------
    glColor3f(0.01f, 0.01f, 0.01f);
    glLineWidth(5.0f);

    glBegin(GL_LINE_STRIP);
        glVertex2f(cx + (8.55f - cx) * q * 0.80f, cy + (0.05f - cy) * q * 0.80f - 0.40f * q);
        glVertex2f(cx + (9.05f - cx) * q * 0.80f, cy + (-0.65f - cy) * q * 0.80f - 0.45f * q);
        glVertex2f(cx + (8.55f - cx) * q * 0.80f, cy + (-1.25f - cy) * q * 0.80f - 0.52f * q);
        glVertex2f(cx + (9.00f - cx) * q * 0.80f, cy + (-1.95f - cy) * q * 0.80f - 0.58f * q);
        glVertex2f(cx + (8.55f - cx) * q * 0.80f, cy + (-2.70f - cy) * q * 0.80f - 0.64f * q);
    glEnd();

    glBegin(GL_LINE_STRIP);
        glVertex2f(cx + (9.75f - cx) * q * 0.80f, cy + (-0.10f - cy) * q * 0.80f - 0.42f * q);
        glVertex2f(cx + (10.35f - cx) * q * 0.80f, cy + (-0.85f - cy) * q * 0.80f - 0.48f * q);
        glVertex2f(cx + (10.10f - cx) * q * 0.80f, cy + (-1.45f - cy) * q * 0.80f - 0.55f * q);
        glVertex2f(cx + (10.95f - cx) * q * 0.80f, cy + (-2.05f - cy) * q * 0.80f - 0.61f * q);
        glVertex2f(cx + (11.65f - cx) * q * 0.80f, cy + (-2.55f - cy) * q * 0.80f - 0.66f * q);
    glEnd();

    glLineWidth(1.0f);
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
        glVertex2f(11.7623f, 4.32168f);
       glVertex2f(11.96f, 4.60779f + handWave);
        glVertex2f(11.7462f, 4.20894f);
    glEnd();

    // ---------------- LEFT HAND ----------------
    glBegin(GL_TRIANGLES);
        glVertex2f(11.03f, 4.6f);
        glVertex2f(11.2224f, 4.31014f);
        glVertex2f(11.2404f, 4.20279f);
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

    // ---------------- First Building ----------------
    glColor3f(0.45f, 0.58f, 0.38f);
    glBegin(GL_POLYGON);
        glVertex2f(1.0f, 1.0f);
        glVertex2f(1.0f - 0.64f * q, 9.0f);
        glVertex2f(6.0f - 0.64f * q, 9.0f);
        glVertex2f(6.0f, 1.0f);
    glEnd();

    // first building top shade
    glColor3f(0.35f, 0.48f, 0.30f);
    glBegin(GL_POLYGON);
        glVertex2f(1.0f - 0.64f * q, 9.0f);
        glVertex2f(1.0f - 0.67f * q, 9.35f);
        glVertex2f(6.0f - 0.67f * q, 9.35f);
        glVertex2f(6.0f - 0.64f * q, 9.0f);
    glEnd();

    // ---------------- Second Building ----------------
    glColor3f(0.62f, 0.45f, 0.28f);
    glBegin(GL_POLYGON);
        glVertex2f(8.0f, 1.0f);
        glVertex2f(8.0f - 1.08f * q, 10.0f);
        glVertex2f(13.0f - 1.08f * q, 10.0f);
        glVertex2f(13.0f, 1.0f);
    glEnd();

    // second building top shade
    glColor3f(0.50f, 0.36f, 0.20f);
    glBegin(GL_POLYGON);
        glVertex2f(8.0f - 1.08f * q, 10.0f);
        glVertex2f(8.0f - 1.12f * q, 10.35f);
        glVertex2f(13.0f - 1.12f * q, 10.35f);
        glVertex2f(13.0f - 1.08f * q, 10.0f);
    glEnd();

    // ---------------- Third Building ----------------
    glColor3f(0.25f, 0.50f, 0.65f);
    glBegin(GL_POLYGON);
        glVertex2f(15.0f, 1.0f);
        glVertex2f(15.0f + 0.56f * q, 9.0f);
        glVertex2f(20.0f + 0.56f * q, 9.0f);
        glVertex2f(20.0f, 1.0f);
    glEnd();

    // third building top shade
    glColor3f(0.18f, 0.38f, 0.52f);
    glBegin(GL_POLYGON);
        glVertex2f(15.0f + 0.56f * q, 9.0f);
        glVertex2f(15.0f + 0.59f * q, 9.35f);
        glVertex2f(20.0f + 0.59f * q, 9.35f);
        glVertex2f(20.0f + 0.56f * q, 9.0f);
    glEnd();

    // ---------------- Doors ----------------
    glColor3f(0.25f, 0.18f, 0.12f);

    // first building door
    glBegin(GL_POLYGON);
        glVertex2f(2.8f, 1.0f);
        glVertex2f(2.8f - 0.11f * q, 2.4f);
        glVertex2f(4.2f - 0.11f * q, 2.4f);
        glVertex2f(4.2f, 1.0f);
    glEnd();

    // second building door
    glBegin(GL_POLYGON);
        glVertex2f(9.8f, 1.0f);
        glVertex2f(9.8f - 0.18f * q, 2.5f);
        glVertex2f(11.2f - 0.18f * q, 2.5f);
        glVertex2f(11.2f, 1.0f);
    glEnd();

    // third building door
    glBegin(GL_POLYGON);
        glVertex2f(16.8f, 1.0f);
        glVertex2f(16.8f + 0.10f * q, 2.4f);
        glVertex2f(18.2f + 0.10f * q, 2.4f);
        glVertex2f(18.2f, 1.0f);
    glEnd();

    // ---------------- First Building Windows ----------------

    // window 1
    glColor3f(0.65f, 0.85f, 0.92f);
    glBegin(GL_POLYGON);
        glVertex2f(2.0f + (7.2f - 1.0f) * (-0.08f) * q, 7.2f);
        glVertex2f(2.0f + (8.2f - 1.0f) * (-0.08f) * q, 8.2f);
        glVertex2f(3.0f + (8.2f - 1.0f) * (-0.08f) * q, 8.2f);
        glVertex2f(3.0f + (7.2f - 1.0f) * (-0.08f) * q, 7.2f);
    glEnd();

    glColor3f(0.20f, 0.35f, 0.45f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(2.0f + (7.2f - 1.0f) * (-0.08f) * q, 7.2f);
        glVertex2f(2.0f + (8.2f - 1.0f) * (-0.08f) * q, 8.2f);
        glVertex2f(3.0f + (8.2f - 1.0f) * (-0.08f) * q, 8.2f);
        glVertex2f(3.0f + (7.2f - 1.0f) * (-0.08f) * q, 7.2f);
    glEnd();

    // window 2
    glColor3f(0.65f, 0.85f, 0.92f);
    glBegin(GL_POLYGON);
        glVertex2f(4.0f + (7.2f - 1.0f) * (-0.08f) * q, 7.2f);
        glVertex2f(4.0f + (8.2f - 1.0f) * (-0.08f) * q, 8.2f);
        glVertex2f(5.0f + (8.2f - 1.0f) * (-0.08f) * q, 8.2f);
        glVertex2f(5.0f + (7.2f - 1.0f) * (-0.08f) * q, 7.2f);
    glEnd();

    glColor3f(0.20f, 0.35f, 0.45f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(4.0f + (7.2f - 1.0f) * (-0.08f) * q, 7.2f);
        glVertex2f(4.0f + (8.2f - 1.0f) * (-0.08f) * q, 8.2f);
        glVertex2f(5.0f + (8.2f - 1.0f) * (-0.08f) * q, 8.2f);
        glVertex2f(5.0f + (7.2f - 1.0f) * (-0.08f) * q, 7.2f);
    glEnd();

    // window 3
    glColor3f(0.65f, 0.85f, 0.92f);
    glBegin(GL_POLYGON);
        glVertex2f(2.0f + (5.2f - 1.0f) * (-0.08f) * q, 5.2f);
        glVertex2f(2.0f + (6.2f - 1.0f) * (-0.08f) * q, 6.2f);
        glVertex2f(3.0f + (6.2f - 1.0f) * (-0.08f) * q, 6.2f);
        glVertex2f(3.0f + (5.2f - 1.0f) * (-0.08f) * q, 5.2f);
    glEnd();

    glColor3f(0.20f, 0.35f, 0.45f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(2.0f + (5.2f - 1.0f) * (-0.08f) * q, 5.2f);
        glVertex2f(2.0f + (6.2f - 1.0f) * (-0.08f) * q, 6.2f);
        glVertex2f(3.0f + (6.2f - 1.0f) * (-0.08f) * q, 6.2f);
        glVertex2f(3.0f + (5.2f - 1.0f) * (-0.08f) * q, 5.2f);
    glEnd();

    // window 4
    glColor3f(0.65f, 0.85f, 0.92f);
    glBegin(GL_POLYGON);
        glVertex2f(4.0f + (5.2f - 1.0f) * (-0.08f) * q, 5.2f);
        glVertex2f(4.0f + (6.2f - 1.0f) * (-0.08f) * q, 6.2f);
        glVertex2f(5.0f + (6.2f - 1.0f) * (-0.08f) * q, 6.2f);
        glVertex2f(5.0f + (5.2f - 1.0f) * (-0.08f) * q, 5.2f);
    glEnd();

    glColor3f(0.20f, 0.35f, 0.45f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(4.0f + (5.2f - 1.0f) * (-0.08f) * q, 5.2f);
        glVertex2f(4.0f + (6.2f - 1.0f) * (-0.08f) * q, 6.2f);
        glVertex2f(5.0f + (6.2f - 1.0f) * (-0.08f) * q, 6.2f);
        glVertex2f(5.0f + (5.2f - 1.0f) * (-0.08f) * q, 5.2f);
    glEnd();

    // window 5
    glColor3f(0.65f, 0.85f, 0.92f);
    glBegin(GL_POLYGON);
        glVertex2f(2.0f + (3.2f - 1.0f) * (-0.08f) * q, 3.2f);
        glVertex2f(2.0f + (4.2f - 1.0f) * (-0.08f) * q, 4.2f);
        glVertex2f(3.0f + (4.2f - 1.0f) * (-0.08f) * q, 4.2f);
        glVertex2f(3.0f + (3.2f - 1.0f) * (-0.08f) * q, 3.2f);
    glEnd();

    glColor3f(0.20f, 0.35f, 0.45f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(2.0f + (3.2f - 1.0f) * (-0.08f) * q, 3.2f);
        glVertex2f(2.0f + (4.2f - 1.0f) * (-0.08f) * q, 4.2f);
        glVertex2f(3.0f + (4.2f - 1.0f) * (-0.08f) * q, 4.2f);
        glVertex2f(3.0f + (3.2f - 1.0f) * (-0.08f) * q, 3.2f);
    glEnd();

    // window 6
    glColor3f(0.65f, 0.85f, 0.92f);
    glBegin(GL_POLYGON);
        glVertex2f(4.0f + (3.2f - 1.0f) * (-0.08f) * q, 3.2f);
        glVertex2f(4.0f + (4.2f - 1.0f) * (-0.08f) * q, 4.2f);
        glVertex2f(5.0f + (4.2f - 1.0f) * (-0.08f) * q, 4.2f);
        glVertex2f(5.0f + (3.2f - 1.0f) * (-0.08f) * q, 3.2f);
    glEnd();

    glColor3f(0.20f, 0.35f, 0.45f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(4.0f + (3.2f - 1.0f) * (-0.08f) * q, 3.2f);
        glVertex2f(4.0f + (4.2f - 1.0f) * (-0.08f) * q, 4.2f);
        glVertex2f(5.0f + (4.2f - 1.0f) * (-0.08f) * q, 4.2f);
        glVertex2f(5.0f + (3.2f - 1.0f) * (-0.08f) * q, 3.2f);
    glEnd();

    // ---------------- Second Building Windows ----------------

    // window 1
    glColor3f(0.65f, 0.85f, 0.92f);
    glBegin(GL_POLYGON);
        glVertex2f(9.0f + (8.0f - 1.0f) * (-0.12f) * q, 8.0f);
        glVertex2f(9.0f + (9.0f - 1.0f) * (-0.12f) * q, 9.0f);
        glVertex2f(10.0f + (9.0f - 1.0f) * (-0.12f) * q, 9.0f);
        glVertex2f(10.0f + (8.0f - 1.0f) * (-0.12f) * q, 8.0f);
    glEnd();

    glColor3f(0.20f, 0.35f, 0.45f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(9.0f + (8.0f - 1.0f) * (-0.12f) * q, 8.0f);
        glVertex2f(9.0f + (9.0f - 1.0f) * (-0.12f) * q, 9.0f);
        glVertex2f(10.0f + (9.0f - 1.0f) * (-0.12f) * q, 9.0f);
        glVertex2f(10.0f + (8.0f - 1.0f) * (-0.12f) * q, 8.0f);
    glEnd();

    // window 2
    glColor3f(0.65f, 0.85f, 0.92f);
    glBegin(GL_POLYGON);
        glVertex2f(11.0f + (8.0f - 1.0f) * (-0.12f) * q, 8.0f);
        glVertex2f(11.0f + (9.0f - 1.0f) * (-0.12f) * q, 9.0f);
        glVertex2f(12.0f + (9.0f - 1.0f) * (-0.12f) * q, 9.0f);
        glVertex2f(12.0f + (8.0f - 1.0f) * (-0.12f) * q, 8.0f);
    glEnd();

    glColor3f(0.20f, 0.35f, 0.45f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(11.0f + (8.0f - 1.0f) * (-0.12f) * q, 8.0f);
        glVertex2f(11.0f + (9.0f - 1.0f) * (-0.12f) * q, 9.0f);
        glVertex2f(12.0f + (9.0f - 1.0f) * (-0.12f) * q, 9.0f);
        glVertex2f(12.0f + (8.0f - 1.0f) * (-0.12f) * q, 8.0f);
    glEnd();

    // window 3
    glColor3f(0.65f, 0.85f, 0.92f);
    glBegin(GL_POLYGON);
        glVertex2f(9.0f + (6.0f - 1.0f) * (-0.12f) * q, 6.0f);
        glVertex2f(9.0f + (7.0f - 1.0f) * (-0.12f) * q, 7.0f);
        glVertex2f(10.0f + (7.0f - 1.0f) * (-0.12f) * q, 7.0f);
        glVertex2f(10.0f + (6.0f - 1.0f) * (-0.12f) * q, 6.0f);
    glEnd();

    glColor3f(0.20f, 0.35f, 0.45f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(9.0f + (6.0f - 1.0f) * (-0.12f) * q, 6.0f);
        glVertex2f(9.0f + (7.0f - 1.0f) * (-0.12f) * q, 7.0f);
        glVertex2f(10.0f + (7.0f - 1.0f) * (-0.12f) * q, 7.0f);
        glVertex2f(10.0f + (6.0f - 1.0f) * (-0.12f) * q, 6.0f);
    glEnd();

    // window 4
    glColor3f(0.65f, 0.85f, 0.92f);
    glBegin(GL_POLYGON);
        glVertex2f(11.0f + (6.0f - 1.0f) * (-0.12f) * q, 6.0f);
        glVertex2f(11.0f + (7.0f - 1.0f) * (-0.12f) * q, 7.0f);
        glVertex2f(12.0f + (7.0f - 1.0f) * (-0.12f) * q, 7.0f);
        glVertex2f(12.0f + (6.0f - 1.0f) * (-0.12f) * q, 6.0f);
    glEnd();

    glColor3f(0.20f, 0.35f, 0.45f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(11.0f + (6.0f - 1.0f) * (-0.12f) * q, 6.0f);
        glVertex2f(11.0f + (7.0f - 1.0f) * (-0.12f) * q, 7.0f);
        glVertex2f(12.0f + (7.0f - 1.0f) * (-0.12f) * q, 7.0f);
        glVertex2f(12.0f + (6.0f - 1.0f) * (-0.12f) * q, 6.0f);
    glEnd();

    // window 5
    glColor3f(0.65f, 0.85f, 0.92f);
    glBegin(GL_POLYGON);
        glVertex2f(9.0f + (4.0f - 1.0f) * (-0.12f) * q, 4.0f);
        glVertex2f(9.0f + (5.0f - 1.0f) * (-0.12f) * q, 5.0f);
        glVertex2f(10.0f + (5.0f - 1.0f) * (-0.12f) * q, 5.0f);
        glVertex2f(10.0f + (4.0f - 1.0f) * (-0.12f) * q, 4.0f);
    glEnd();

    glColor3f(0.20f, 0.35f, 0.45f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(9.0f + (4.0f - 1.0f) * (-0.12f) * q, 4.0f);
        glVertex2f(9.0f + (5.0f - 1.0f) * (-0.12f) * q, 5.0f);
        glVertex2f(10.0f + (5.0f - 1.0f) * (-0.12f) * q, 5.0f);
        glVertex2f(10.0f + (4.0f - 1.0f) * (-0.12f) * q, 4.0f);
    glEnd();

    // window 6
    glColor3f(0.65f, 0.85f, 0.92f);
    glBegin(GL_POLYGON);
        glVertex2f(11.0f + (4.0f - 1.0f) * (-0.12f) * q, 4.0f);
        glVertex2f(11.0f + (5.0f - 1.0f) * (-0.12f) * q, 5.0f);
        glVertex2f(12.0f + (5.0f - 1.0f) * (-0.12f) * q, 5.0f);
        glVertex2f(12.0f + (4.0f - 1.0f) * (-0.12f) * q, 4.0f);
    glEnd();

    glColor3f(0.20f, 0.35f, 0.45f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(11.0f + (4.0f - 1.0f) * (-0.12f) * q, 4.0f);
        glVertex2f(11.0f + (5.0f - 1.0f) * (-0.12f) * q, 5.0f);
        glVertex2f(12.0f + (5.0f - 1.0f) * (-0.12f) * q, 5.0f);
        glVertex2f(12.0f + (4.0f - 1.0f) * (-0.12f) * q, 4.0f);
    glEnd();

    // ---------------- Third Building Windows ----------------

    // window 1
    glColor3f(0.65f, 0.85f, 0.92f);
    glBegin(GL_POLYGON);
        glVertex2f(16.0f + (7.2f - 1.0f) * 0.07f * q, 7.2f);
        glVertex2f(16.0f + (8.2f - 1.0f) * 0.07f * q, 8.2f);
        glVertex2f(17.0f + (8.2f - 1.0f) * 0.07f * q, 8.2f);
        glVertex2f(17.0f + (7.2f - 1.0f) * 0.07f * q, 7.2f);
    glEnd();

    glColor3f(0.20f, 0.35f, 0.45f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(16.0f + (7.2f - 1.0f) * 0.07f * q, 7.2f);
        glVertex2f(16.0f + (8.2f - 1.0f) * 0.07f * q, 8.2f);
        glVertex2f(17.0f + (8.2f - 1.0f) * 0.07f * q, 8.2f);
        glVertex2f(17.0f + (7.2f - 1.0f) * 0.07f * q, 7.2f);
    glEnd();

    // window 2
    glColor3f(0.65f, 0.85f, 0.92f);
    glBegin(GL_POLYGON);
        glVertex2f(18.0f + (7.2f - 1.0f) * 0.07f * q, 7.2f);
        glVertex2f(18.0f + (8.2f - 1.0f) * 0.07f * q, 8.2f);
        glVertex2f(19.0f + (8.2f - 1.0f) * 0.07f * q, 8.2f);
        glVertex2f(19.0f + (7.2f - 1.0f) * 0.07f * q, 7.2f);
    glEnd();

    glColor3f(0.20f, 0.35f, 0.45f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(18.0f + (7.2f - 1.0f) * 0.07f * q, 7.2f);
        glVertex2f(18.0f + (8.2f - 1.0f) * 0.07f * q, 8.2f);
        glVertex2f(19.0f + (8.2f - 1.0f) * 0.07f * q, 8.2f);
        glVertex2f(19.0f + (7.2f - 1.0f) * 0.07f * q, 7.2f);
    glEnd();

    // window 3
    glColor3f(0.65f, 0.85f, 0.92f);
    glBegin(GL_POLYGON);
        glVertex2f(16.0f + (5.2f - 1.0f) * 0.07f * q, 5.2f);
        glVertex2f(16.0f + (6.2f - 1.0f) * 0.07f * q, 6.2f);
        glVertex2f(17.0f + (6.2f - 1.0f) * 0.07f * q, 6.2f);
        glVertex2f(17.0f + (5.2f - 1.0f) * 0.07f * q, 5.2f);
    glEnd();

    glColor3f(0.20f, 0.35f, 0.45f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(16.0f + (5.2f - 1.0f) * 0.07f * q, 5.2f);
        glVertex2f(16.0f + (6.2f - 1.0f) * 0.07f * q, 6.2f);
        glVertex2f(17.0f + (6.2f - 1.0f) * 0.07f * q, 6.2f);
        glVertex2f(17.0f + (5.2f - 1.0f) * 0.07f * q, 5.2f);
    glEnd();

    // window 4
    glColor3f(0.65f, 0.85f, 0.92f);
    glBegin(GL_POLYGON);
        glVertex2f(18.0f + (5.2f - 1.0f) * 0.07f * q, 5.2f);
        glVertex2f(18.0f + (6.2f - 1.0f) * 0.07f * q, 6.2f);
        glVertex2f(19.0f + (6.2f - 1.0f) * 0.07f * q, 6.2f);
        glVertex2f(19.0f + (5.2f - 1.0f) * 0.07f * q, 5.2f);
    glEnd();

    glColor3f(0.20f, 0.35f, 0.45f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(18.0f + (5.2f - 1.0f) * 0.07f * q, 5.2f);
        glVertex2f(18.0f + (6.2f - 1.0f) * 0.07f * q, 6.2f);
        glVertex2f(19.0f + (6.2f - 1.0f) * 0.07f * q, 6.2f);
        glVertex2f(19.0f + (5.2f - 1.0f) * 0.07f * q, 5.2f);
    glEnd();

    // window 5
    glColor3f(0.65f, 0.85f, 0.92f);
    glBegin(GL_POLYGON);
        glVertex2f(16.0f + (3.2f - 1.0f) * 0.07f * q, 3.2f);
        glVertex2f(16.0f + (4.2f - 1.0f) * 0.07f * q, 4.2f);
        glVertex2f(17.0f + (4.2f - 1.0f) * 0.07f * q, 4.2f);
        glVertex2f(17.0f + (3.2f - 1.0f) * 0.07f * q, 3.2f);
    glEnd();

    glColor3f(0.20f, 0.35f, 0.45f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(16.0f + (3.2f - 1.0f) * 0.07f * q, 3.2f);
        glVertex2f(16.0f + (4.2f - 1.0f) * 0.07f * q, 4.2f);
        glVertex2f(17.0f + (4.2f - 1.0f) * 0.07f * q, 4.2f);
        glVertex2f(17.0f + (3.2f - 1.0f) * 0.07f * q, 3.2f);
    glEnd();

    // window 6
    glColor3f(0.65f, 0.85f, 0.92f);
    glBegin(GL_POLYGON);
        glVertex2f(18.0f + (3.2f - 1.0f) * 0.07f * q, 3.2f);
        glVertex2f(18.0f + (4.2f - 1.0f) * 0.07f * q, 4.2f);
        glVertex2f(19.0f + (4.2f - 1.0f) * 0.07f * q, 4.2f);
        glVertex2f(19.0f + (3.2f - 1.0f) * 0.07f * q, 3.2f);
    glEnd();

    glColor3f(0.20f, 0.35f, 0.45f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(18.0f + (3.2f - 1.0f) * 0.07f * q, 3.2f);
        glVertex2f(18.0f + (4.2f - 1.0f) * 0.07f * q, 4.2f);
        glVertex2f(19.0f + (4.2f - 1.0f) * 0.07f * q, 4.2f);
        glVertex2f(19.0f + (3.2f - 1.0f) * 0.07f * q, 3.2f);
    glEnd();

    // ---------------- Fire in Second Building Windows ----------------
    // ---------------- Fire in Second Building Windows ----------------
if(f > 0.01f) {
    float show = f;
    float cx, cy, size;
    float wave1, wave2;

    // ---------------- Fire Window 1 ----------------
    cx = 9.5f;
    cy = 8.45f;
    size = 0.90f;

    wave1 = sin(frameCount * 0.35f + cx) * 0.12f * show;
    wave2 = cos(frameCount * 0.42f + cy) * 0.10f * show;

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


    // ---------------- Fire Window 2 ----------------
    cx = 11.5f;
    cy = 8.45f;
    size = 0.90f;

    wave1 = sin(frameCount * 0.35f + cx) * 0.12f * show;
    wave2 = cos(frameCount * 0.42f + cy) * 0.10f * show;

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


    // ---------------- Fire Window 3 ----------------
    cx = 9.5f;
    cy = 6.45f;
    size = 0.90f;

    wave1 = sin(frameCount * 0.35f + cx) * 0.12f * show;
    wave2 = cos(frameCount * 0.42f + cy) * 0.10f * show;

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


    // ---------------- Fire Window 4 ----------------
    cx = 11.5f;
    cy = 6.45f;
    size = 0.90f;

    wave1 = sin(frameCount * 0.35f + cx) * 0.12f * show;
    wave2 = cos(frameCount * 0.42f + cy) * 0.10f * show;

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


    // ---------------- Fire Window 5 ----------------
    cx = 9.5f;
    cy = 4.45f;
    size = 0.80f;

    wave1 = sin(frameCount * 0.35f + cx) * 0.12f * show;
    wave2 = cos(frameCount * 0.42f + cy) * 0.10f * show;

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
}

void drawTree() {
    float q = earthquakeValue();
    float st = stormValue();
    float swing = sin(frameCount * 0.18f) * 15.0f * st;


   // ---------------- Tree Base ----------------
glColor3f(0.42f, 0.24f, 0.10f);
glBegin(GL_POLYGON);
    glVertex2f(-1.60f, 0.70f);   // TG1
    glVertex2f(-1.60f, 1.10f);   // TG2
    glVertex2f( 0.20f, 1.10f);   // TG3
    glVertex2f( 0.20f, 0.70f);   // TG4
glEnd();

glPushMatrix();
glTranslatef(-0.7f, 1.1f, 0.0f);
glRotatef((-14.0f * q) + swing, 0.0f, 0.0f, 1.0f);
glTranslatef(0.7f, -1.1f, 0.0f);

// ---------------- Main Tree Trunk ----------------
// Coordinates followed from your screenshot:
// TT1, TT2, TT3, TT4, TT6, TT7, TT8, TT9, TT10, TT11, TT12
glColor3f(0.36f, 0.20f, 0.08f);
glBegin(GL_POLYGON);
    glVertex2f(-0.82f, 1.10f);   // TT1
    glVertex2f(-0.91f, 2.86f);   // TT2
    glVertex2f(-0.68f, 3.35f);   // TT3
    glVertex2f(-0.53f, 4.00f);   // TT4

    glVertex2f(-0.33f, 3.00f);   // TT6
    glVertex2f(-0.18f, 3.55f);   // TT7
    glVertex2f( 0.02f, 4.05f);   // TT8

    glVertex2f( 0.18f, 3.35f);   // TT9
    glVertex2f( 0.04f, 2.80f);   // TT10
    glVertex2f( 0.04f, 1.10f);   // TT11
    glVertex2f(-0.32f, 1.10f);   // TT12
glEnd();

// ---------------- Dark left side shade ----------------
glColor3f(0.25f, 0.13f, 0.05f);
glBegin(GL_POLYGON);
    glVertex2f(-0.82f, 1.10f);
    glVertex2f(-0.91f, 2.86f);
    glVertex2f(-0.75f, 2.23f);
    glVertex2f(-0.77f, 1.70f);
    glVertex2f(-0.65f, 1.10f);
glEnd();

// ---------------- Right side shade ----------------
glColor3f(0.43f, 0.24f, 0.09f);
glBegin(GL_POLYGON);
    glVertex2f(-0.08f, 1.70f);
    glVertex2f(-0.09f, 2.20f);
    glVertex2f( 0.04f, 2.80f);
    glVertex2f( 0.04f, 1.10f);
    glVertex2f(-0.20f, 1.10f);
glEnd();

// ---------------- Left branch ----------------
glColor3f(0.36f, 0.20f, 0.08f);
glBegin(GL_POLYGON);
    glVertex2f(-0.68f, 3.35f);   // branch base
    glVertex2f(-1.02f, 3.90f);
    glVertex2f(-0.95f, 4.12f);
    glVertex2f(-0.63f, 3.56f);   // O
glEnd();

// ---------------- Middle branch ----------------
glBegin(GL_POLYGON);
    glVertex2f(-0.33f, 3.00f);   // branch base
    glVertex2f(-0.63f, 3.56f);   // O
    glVertex2f(-0.53f, 3.75f);
    glVertex2f(-0.18f, 3.55f);   // TT7
glEnd();

// ---------------- Right branch ----------------
glBegin(GL_POLYGON);
    glVertex2f(-0.18f, 3.55f);   // TT7
    glVertex2f( 0.02f, 4.05f);   // TT8
    glVertex2f( 0.18f, 3.35f);   // TT9
    glVertex2f( 0.05f, 3.15f);
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

    //  lamp around the bottom point.
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

    // black tires
    glColor3f(0.02f, 0.02f, 0.02f);
    drawCircle(1.790571f, -0.602406f, 0.25f);
    drawCircle(4.406276f, -0.558444f, 0.25f);

    // inner hub
    glColor3f(0.65f, 0.65f, 0.65f);
    drawCircle(1.790571f, -0.602406f, 0.12f);
    drawCircle(4.406276f, -0.558444f, 0.12f);

    // Midpoint Circle Algorithm
    glColor3f(0.95f, 0.95f, 0.95f);
    glPointSize(2.0f);

    float scale = 300.0f;
    int xCenter = (int)(1.790571f * scale);
    int yCenter = (int)(-0.602406f * scale);
    int r = (int)(0.28f * scale);

    int x = 0;
    int y = r;
    int p = 1 - r;

    glBegin(GL_POINTS);

        glVertex2f((xCenter + x) / scale, (yCenter + y) / scale);
        glVertex2f((xCenter - x) / scale, (yCenter + y) / scale);
        glVertex2f((xCenter + x) / scale, (yCenter - y) / scale);
        glVertex2f((xCenter - x) / scale, (yCenter - y) / scale);
        glVertex2f((xCenter + y) / scale, (yCenter + x) / scale);
        glVertex2f((xCenter - y) / scale, (yCenter + x) / scale);
        glVertex2f((xCenter + y) / scale, (yCenter - x) / scale);
        glVertex2f((xCenter - y) / scale, (yCenter - x) / scale);

        while(x < y) {
            x++;

            if(p < 0) {
                p = p + 2 * x + 1;
            } else {
                y--;
                p = p + 2 * (x - y) + 1;
            }

            glVertex2f((xCenter + x) / scale, (yCenter + y) / scale);
            glVertex2f((xCenter - x) / scale, (yCenter + y) / scale);
            glVertex2f((xCenter + x) / scale, (yCenter - y) / scale);
            glVertex2f((xCenter - x) / scale, (yCenter - y) / scale);
            glVertex2f((xCenter + y) / scale, (yCenter + x) / scale);
            glVertex2f((xCenter - y) / scale, (yCenter + x) / scale);
            glVertex2f((xCenter + y) / scale, (yCenter - x) / scale);
            glVertex2f((xCenter - y) / scale, (yCenter - x) / scale);
        }

    glEnd();

    // right wheel outline drawn normally
    glColor3f(0.95f, 0.95f, 0.95f);
    glLineWidth(2.0f);

    float centerX = 4.406276f;
    float centerY = -0.558444f;
    float radius = 0.28f;

    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        float X = centerX + radius * cos(angle);
        float Y = centerY + radius * sin(angle);
        glVertex2f(X, Y);
    }
    glEnd();

    glPointSize(1.0f);
    glLineWidth(1.0f);

    glPopMatrix();
}


//-------------------draw------------ People-------------

void drawPeople() {
    float q = earthquakeValue();
    float f = fireValue();

    float x, y;
    float armSwing, legSwing;

    // =====================================================
    // Earthquake Person 1
    // =====================================================
    if(q > 0.02f) {
        x = 3.5f + (6.2f - 3.5f) * q;
        y = 1.2f + (-1.55f - 1.2f) * q;

        armSwing = sin(frameCount * 0.25f + x) * 0.16f;
        legSwing = cos(frameCount * 0.25f + x) * 0.18f;

        // head
        glColor3f(0.95f, 0.70f, 0.48f);
        glBegin(GL_POLYGON);
        for(int i = 0; i < 360; i++) {
            float angle = i * 3.1416f / 180.0f;
            glVertex2f(x + 0.16f * cos(angle), y + 0.95f + 0.16f * sin(angle));
        }
        glEnd();

        // hair
        glColor3f(0.08f, 0.05f, 0.03f);
        glBegin(GL_POLYGON);
            glVertex2f(x - 0.12f, y + 0.98f);
            glVertex2f(x + 0.12f, y + 0.98f);
            glVertex2f(x + 0.10f, y + 1.09f);
            glVertex2f(x - 0.10f, y + 1.09f);
        glEnd();

        // neck
        glColor3f(0.95f, 0.70f, 0.48f);
        glBegin(GL_POLYGON);
            glVertex2f(x - 0.045f, y + 0.79f);
            glVertex2f(x + 0.045f, y + 0.79f);
            glVertex2f(x + 0.045f, y + 0.69f);
            glVertex2f(x - 0.045f, y + 0.69f);
        glEnd();

        // body
        glColor3f(0.95f, 0.10f, 0.10f);
        glBegin(GL_POLYGON);
            glVertex2f(x - 0.18f, y + 0.69f);
            glVertex2f(x + 0.18f, y + 0.69f);
            glVertex2f(x + 0.18f, y + 0.22f);
            glVertex2f(x - 0.18f, y + 0.22f);
        glEnd();

        // hands
        glColor3f(0.95f, 0.70f, 0.48f);
        glLineWidth(4.0f);
        glBegin(GL_LINES);
            glVertex2f(x - 0.16f, y + 0.60f);
            glVertex2f(x - 0.34f, y + 0.38f + armSwing);

            glVertex2f(x + 0.16f, y + 0.60f);
            glVertex2f(x + 0.34f, y + 0.38f - armSwing);
        glEnd();

        // pant
        glColor3f(0.08f, 0.15f, 0.48f);
        glBegin(GL_POLYGON);
            glVertex2f(x - 0.17f, y + 0.22f);
            glVertex2f(x + 0.17f, y + 0.22f);
            glVertex2f(x + 0.17f, y + 0.08f);
            glVertex2f(x - 0.17f, y + 0.08f);
        glEnd();

        // legs
        glLineWidth(6.0f);
        glBegin(GL_LINES);
            glVertex2f(x - 0.07f, y + 0.08f);
            glVertex2f(x - 0.25f, y - 0.30f - legSwing);

            glVertex2f(x + 0.07f, y + 0.08f);
            glVertex2f(x + 0.25f, y - 0.30f + legSwing);
        glEnd();

        // shoes
        glColor3f(0.02f, 0.02f, 0.02f);
        glLineWidth(5.0f);
        glBegin(GL_LINES);
            glVertex2f(x - 0.25f, y - 0.30f - legSwing);
            glVertex2f(x - 0.38f, y - 0.31f - legSwing);

            glVertex2f(x + 0.25f, y - 0.30f + legSwing);
            glVertex2f(x + 0.38f, y - 0.31f + legSwing);
        glEnd();
    }

    // =====================================================
    // Earthquake Person 2
    // =====================================================
    if(q > 0.02f) {
        x = 10.5f + (12.2f - 10.5f) * q;
        y = 1.2f + (-1.15f - 1.2f) * q;

        armSwing = sin(frameCount * 0.25f + x) * 0.16f;
        legSwing = cos(frameCount * 0.25f + x) * 0.18f;

        // head
        glColor3f(0.95f, 0.70f, 0.48f);
        glBegin(GL_POLYGON);
        for(int i = 0; i < 360; i++) {
            float angle = i * 3.1416f / 180.0f;
            glVertex2f(x + 0.16f * cos(angle), y + 0.95f + 0.16f * sin(angle));
        }
        glEnd();

        // hair
        glColor3f(0.08f, 0.05f, 0.03f);
        glBegin(GL_POLYGON);
            glVertex2f(x - 0.12f, y + 0.98f);
            glVertex2f(x + 0.12f, y + 0.98f);
            glVertex2f(x + 0.10f, y + 1.09f);
            glVertex2f(x - 0.10f, y + 1.09f);
        glEnd();

        // neck
        glColor3f(0.95f, 0.70f, 0.48f);
        glBegin(GL_POLYGON);
            glVertex2f(x - 0.045f, y + 0.79f);
            glVertex2f(x + 0.045f, y + 0.79f);
            glVertex2f(x + 0.045f, y + 0.69f);
            glVertex2f(x - 0.045f, y + 0.69f);
        glEnd();

        // body
        glColor3f(0.10f, 0.60f, 0.95f);
        glBegin(GL_POLYGON);
            glVertex2f(x - 0.18f, y + 0.69f);
            glVertex2f(x + 0.18f, y + 0.69f);
            glVertex2f(x + 0.18f, y + 0.22f);
            glVertex2f(x - 0.18f, y + 0.22f);
        glEnd();

        // hands
        glColor3f(0.95f, 0.70f, 0.48f);
        glLineWidth(4.0f);
        glBegin(GL_LINES);
            glVertex2f(x - 0.16f, y + 0.60f);
            glVertex2f(x - 0.34f, y + 0.38f + armSwing);

            glVertex2f(x + 0.16f, y + 0.60f);
            glVertex2f(x + 0.34f, y + 0.38f - armSwing);
        glEnd();

        // pant
        glColor3f(0.08f, 0.15f, 0.48f);
        glBegin(GL_POLYGON);
            glVertex2f(x - 0.17f, y + 0.22f);
            glVertex2f(x + 0.17f, y + 0.22f);
            glVertex2f(x + 0.17f, y + 0.08f);
            glVertex2f(x - 0.17f, y + 0.08f);
        glEnd();

        // legs
        glLineWidth(6.0f);
        glBegin(GL_LINES);
            glVertex2f(x - 0.07f, y + 0.08f);
            glVertex2f(x - 0.25f, y - 0.30f - legSwing);

            glVertex2f(x + 0.07f, y + 0.08f);
            glVertex2f(x + 0.25f, y - 0.30f + legSwing);
        glEnd();

        // shoes
        glColor3f(0.02f, 0.02f, 0.02f);
        glLineWidth(5.0f);
        glBegin(GL_LINES);
            glVertex2f(x - 0.25f, y - 0.30f - legSwing);
            glVertex2f(x - 0.38f, y - 0.31f - legSwing);

            glVertex2f(x + 0.25f, y - 0.30f + legSwing);
            glVertex2f(x + 0.38f, y - 0.31f + legSwing);
        glEnd();
    }

    // =====================================================
    // Earthquake Person 3
    // =====================================================
    if(q > 0.02f) {
        x = 17.5f + (15.55f - 17.5f) * q;
        y = 1.2f + (-1.65f - 1.2f) * q;

        armSwing = sin(frameCount * 0.25f + x) * 0.16f;
        legSwing = cos(frameCount * 0.25f + x) * 0.18f;

        // head
        glColor3f(0.95f, 0.70f, 0.48f);
        glBegin(GL_POLYGON);
        for(int i = 0; i < 360; i++) {
            float angle = i * 3.1416f / 180.0f;
            glVertex2f(x + 0.16f * cos(angle), y + 0.95f + 0.16f * sin(angle));
        }
        glEnd();

        // hair
        glColor3f(0.08f, 0.05f, 0.03f);
        glBegin(GL_POLYGON);
            glVertex2f(x - 0.12f, y + 0.98f);
            glVertex2f(x + 0.12f, y + 0.98f);
            glVertex2f(x + 0.10f, y + 1.09f);
            glVertex2f(x - 0.10f, y + 1.09f);
        glEnd();

        // neck
        glColor3f(0.95f, 0.70f, 0.48f);
        glBegin(GL_POLYGON);
            glVertex2f(x - 0.045f, y + 0.79f);
            glVertex2f(x + 0.045f, y + 0.79f);
            glVertex2f(x + 0.045f, y + 0.69f);
            glVertex2f(x - 0.045f, y + 0.69f);
        glEnd();

        // body
        glColor3f(0.98f, 0.85f, 0.10f);
        glBegin(GL_POLYGON);
            glVertex2f(x - 0.18f, y + 0.69f);
            glVertex2f(x + 0.18f, y + 0.69f);
            glVertex2f(x + 0.18f, y + 0.22f);
            glVertex2f(x - 0.18f, y + 0.22f);
        glEnd();

        // hands, direction left
        glColor3f(0.95f, 0.70f, 0.48f);
        glLineWidth(4.0f);
        glBegin(GL_LINES);
            glVertex2f(x - 0.16f, y + 0.60f);
            glVertex2f(x + 0.34f, y + 0.38f + armSwing);

            glVertex2f(x + 0.16f, y + 0.60f);
            glVertex2f(x - 0.34f, y + 0.38f - armSwing);
        glEnd();

        // pant
        glColor3f(0.08f, 0.15f, 0.48f);
        glBegin(GL_POLYGON);
            glVertex2f(x - 0.17f, y + 0.22f);
            glVertex2f(x + 0.17f, y + 0.22f);
            glVertex2f(x + 0.17f, y + 0.08f);
            glVertex2f(x - 0.17f, y + 0.08f);
        glEnd();

        // legs, direction left
        glLineWidth(6.0f);
        glBegin(GL_LINES);
            glVertex2f(x - 0.07f, y + 0.08f);
            glVertex2f(x + 0.11f, y - 0.30f - legSwing);

            glVertex2f(x + 0.07f, y + 0.08f);
            glVertex2f(x - 0.11f, y - 0.30f + legSwing);
        glEnd();

        // shoes
        glColor3f(0.02f, 0.02f, 0.02f);
        glLineWidth(5.0f);
        glBegin(GL_LINES);
            glVertex2f(x + 0.11f, y - 0.30f - legSwing);
            glVertex2f(x + 0.24f, y - 0.31f - legSwing);

            glVertex2f(x - 0.11f, y - 0.30f + legSwing);
            glVertex2f(x - 0.24f, y - 0.31f + legSwing);
        glEnd();
    }

    // =====================================================
    // Fire Person 1
    // =====================================================
    if(f > 0.02f) {
        x = 13.5f + (8.8f - 13.5f) * f;
        y = 1.1f + (-2.25f - 1.1f) * f;

        float bodyMove = sin(frameCount * 0.22f + x) * 0.10f * f;
        float bodyShakeY = fabs(sin(frameCount * 0.40f + x)) * 0.05f * f;

        armSwing = sin(frameCount * 0.35f + x) * 0.08f * f;
        legSwing = cos(frameCount * 0.35f + x) * 0.10f * f;

        x = x + bodyMove;
        y = y + bodyShakeY;

        // head
        glColor3f(0.95f, 0.70f, 0.48f);
        glBegin(GL_POLYGON);
        for(int i = 0; i < 360; i++) {
            float angle = i * 3.1416f / 180.0f;
            glVertex2f(x + 0.16f * cos(angle), y + 0.95f + 0.16f * sin(angle));
        }
        glEnd();

        // hair
        glColor3f(0.08f, 0.05f, 0.03f);
        glBegin(GL_POLYGON);
            glVertex2f(x - 0.12f, y + 0.98f);
            glVertex2f(x + 0.12f, y + 0.98f);
            glVertex2f(x + 0.10f, y + 1.09f);
            glVertex2f(x - 0.10f, y + 1.09f);
        glEnd();

        // neck
        glColor3f(0.95f, 0.70f, 0.48f);
        glBegin(GL_POLYGON);
            glVertex2f(x - 0.045f, y + 0.79f);
            glVertex2f(x + 0.045f, y + 0.79f);
            glVertex2f(x + 0.045f, y + 0.69f);
            glVertex2f(x - 0.045f, y + 0.69f);
        glEnd();

        // body
        glColor3f(0.10f, 0.85f, 0.30f);
        glBegin(GL_POLYGON);
            glVertex2f(x - 0.18f, y + 0.69f);
            glVertex2f(x + 0.18f, y + 0.69f);
            glVertex2f(x + 0.18f, y + 0.22f);
            glVertex2f(x - 0.18f, y + 0.22f);
        glEnd();

        // hands, moving in fear
        glColor3f(0.95f, 0.70f, 0.48f);
        glLineWidth(4.0f);
        glBegin(GL_LINES);
            glVertex2f(x - 0.16f, y + 0.60f);
            glVertex2f(x - 0.34f, y + 0.44f + armSwing);

            glVertex2f(x + 0.16f, y + 0.60f);
            glVertex2f(x + 0.26f, y + 0.44f - armSwing);
        glEnd();

        // pant
        glColor3f(0.08f, 0.15f, 0.48f);
        glBegin(GL_POLYGON);
            glVertex2f(x - 0.17f, y + 0.22f);
            glVertex2f(x + 0.17f, y + 0.22f);
            glVertex2f(x + 0.17f, y + 0.08f);
            glVertex2f(x - 0.17f, y + 0.08f);
        glEnd();

        // legs, small fear movement
        glLineWidth(6.0f);
        glBegin(GL_LINES);
            glVertex2f(x - 0.07f, y + 0.08f);
            glVertex2f(x - 0.24f, y - 0.30f - legSwing);

            glVertex2f(x + 0.07f, y + 0.08f);
            glVertex2f(x + 0.10f, y - 0.30f + legSwing);
        glEnd();

        // shoes
        glColor3f(0.02f, 0.02f, 0.02f);
        glLineWidth(5.0f);
        glBegin(GL_LINES);
            glVertex2f(x - 0.24f, y - 0.30f - legSwing);
            glVertex2f(x - 0.38f, y - 0.30f - legSwing);

            glVertex2f(x + 0.10f, y - 0.30f + legSwing);
            glVertex2f(x + 0.22f, y - 0.30f + legSwing);
        glEnd();
    }

    // =====================================================
    // Fire Person 2
    // =====================================================
    if(f > 0.02f) {
        x = 11.2f + (13.8f - 11.2f) * f;
        y = 1.1f + (-2.05f - 1.1f) * f;

        float bodyMove = sin(frameCount * 0.22f + x) * 0.10f * f;
        float bodyShakeY = fabs(sin(frameCount * 0.40f + x)) * 0.05f * f;

        armSwing = sin(frameCount * 0.35f + x) * 0.08f * f;
        legSwing = cos(frameCount * 0.35f + x) * 0.10f * f;
        x = x + bodyMove;
        y = y + bodyShakeY;

        // head
        glColor3f(0.95f, 0.70f, 0.48f);
        glBegin(GL_POLYGON);
        for(int i = 0; i < 360; i++) {
            float angle = i * 3.1416f / 180.0f;
            glVertex2f(x + 0.16f * cos(angle), y + 0.95f + 0.16f * sin(angle));
        }
        glEnd();

        // hair
        glColor3f(0.08f, 0.05f, 0.03f);
        glBegin(GL_POLYGON);
            glVertex2f(x - 0.12f, y + 0.98f);
            glVertex2f(x + 0.12f, y + 0.98f);
            glVertex2f(x + 0.10f, y + 1.09f);
            glVertex2f(x - 0.10f, y + 1.09f);
        glEnd();

        // neck
        glColor3f(0.95f, 0.70f, 0.48f);
        glBegin(GL_POLYGON);
            glVertex2f(x - 0.045f, y + 0.79f);
            glVertex2f(x + 0.045f, y + 0.79f);
            glVertex2f(x + 0.045f, y + 0.69f);
            glVertex2f(x - 0.045f, y + 0.69f);
        glEnd();

        // body
        glColor3f(0.75f, 0.20f, 0.90f);
        glBegin(GL_POLYGON);
            glVertex2f(x - 0.18f, y + 0.69f);
            glVertex2f(x + 0.18f, y + 0.69f);
            glVertex2f(x + 0.18f, y + 0.22f);
            glVertex2f(x - 0.18f, y + 0.22f);
        glEnd();

        // hands, moving in fear
        glColor3f(0.95f, 0.70f, 0.48f);
        glLineWidth(4.0f);
        glBegin(GL_LINES);
            glVertex2f(x - 0.16f, y + 0.60f);
            glVertex2f(x - 0.26f, y + 0.44f + armSwing);

            glVertex2f(x + 0.16f, y + 0.60f);
            glVertex2f(x + 0.34f, y + 0.44f - armSwing);
        glEnd();

        // pant
        glColor3f(0.08f, 0.15f, 0.48f);
        glBegin(GL_POLYGON);
            glVertex2f(x - 0.17f, y + 0.22f);
            glVertex2f(x + 0.17f, y + 0.22f);
            glVertex2f(x + 0.17f, y + 0.08f);
            glVertex2f(x - 0.17f, y + 0.08f);
        glEnd();

        // legs, small fear movement
        glLineWidth(6.0f);
        glBegin(GL_LINES);
            glVertex2f(x - 0.07f, y + 0.08f);
            glVertex2f(x - 0.10f, y - 0.30f - legSwing);

            glVertex2f(x + 0.07f, y + 0.08f);
            glVertex2f(x + 0.24f, y - 0.30f + legSwing);
        glEnd();

        // shoes
        glColor3f(0.02f, 0.02f, 0.02f);
        glLineWidth(5.0f);
        glBegin(GL_LINES);
            glVertex2f(x - 0.10f, y - 0.30f - legSwing);
            glVertex2f(x - 0.22f, y - 0.30f - legSwing);

            glVertex2f(x + 0.24f, y - 0.30f + legSwing);
            glVertex2f(x + 0.38f, y - 0.30f + legSwing);
        glEnd();
    }

    // =====================================================
    // Fire Person 3
    // =====================================================
    if(f > 0.02f) {
        x = 16.0f + (17.4f - 16.0f) * f;
        y = 1.1f + (-2.45f - 1.1f) * f;

        float bodyMove = sin(frameCount * 0.22f + x) * 0.10f * f;
        float bodyShakeY = fabs(sin(frameCount * 0.40f + x)) * 0.05f * f;

        armSwing = sin(frameCount * 0.35f + x) * 0.08f * f;
        legSwing = cos(frameCount * 0.35f + x) * 0.10f * f;

        x = x + bodyMove;
        y = y + bodyShakeY;

        // head
        glColor3f(0.95f, 0.70f, 0.48f);
        glBegin(GL_POLYGON);
        for(int i = 0; i < 360; i++) {
            float angle = i * 3.1416f / 180.0f;
            glVertex2f(x + 0.16f * cos(angle), y + 0.95f + 0.16f * sin(angle));
        }
        glEnd();

        // hair
        glColor3f(0.08f, 0.05f, 0.03f);
        glBegin(GL_POLYGON);
            glVertex2f(x - 0.12f, y + 0.98f);
            glVertex2f(x + 0.12f, y + 0.98f);
            glVertex2f(x + 0.10f, y + 1.09f);
            glVertex2f(x - 0.10f, y + 1.09f);
        glEnd();

        // neck
        glColor3f(0.95f, 0.70f, 0.48f);
        glBegin(GL_POLYGON);
            glVertex2f(x - 0.045f, y + 0.79f);
            glVertex2f(x + 0.045f, y + 0.79f);
            glVertex2f(x + 0.045f, y + 0.69f);
            glVertex2f(x - 0.045f, y + 0.69f);
        glEnd();

        // body
        glColor3f(0.95f, 0.40f, 0.10f);
        glBegin(GL_POLYGON);
            glVertex2f(x - 0.18f, y + 0.69f);
            glVertex2f(x + 0.18f, y + 0.69f);
            glVertex2f(x + 0.18f, y + 0.22f);
            glVertex2f(x - 0.18f, y + 0.22f);
        glEnd();

        // hands, moving in fear
        glColor3f(0.95f, 0.70f, 0.48f);
        glLineWidth(4.0f);
        glBegin(GL_LINES);
            glVertex2f(x - 0.16f, y + 0.60f);
            glVertex2f(x - 0.26f, y + 0.44f + armSwing);

            glVertex2f(x + 0.16f, y + 0.60f);
            glVertex2f(x + 0.34f, y + 0.44f - armSwing);
        glEnd();

        // pant
        glColor3f(0.08f, 0.15f, 0.48f);
        glBegin(GL_POLYGON);
            glVertex2f(x - 0.17f, y + 0.22f);
            glVertex2f(x + 0.17f, y + 0.22f);
            glVertex2f(x + 0.17f, y + 0.08f);
            glVertex2f(x - 0.17f, y + 0.08f);
        glEnd();

        // legs, small fear movement
        glLineWidth(6.0f);
        glBegin(GL_LINES);
            glVertex2f(x - 0.07f, y + 0.08f);
            glVertex2f(x - 0.10f, y - 0.30f - legSwing);

            glVertex2f(x + 0.07f, y + 0.08f);
            glVertex2f(x + 0.24f, y - 0.30f + legSwing);
        glEnd();

        // shoes
        glColor3f(0.02f, 0.02f, 0.02f);
        glLineWidth(5.0f);
        glBegin(GL_LINES);
            glVertex2f(x - 0.10f, y - 0.30f - legSwing);
            glVertex2f(x - 0.22f, y - 0.30f - legSwing);

            glVertex2f(x + 0.24f, y - 0.30f + legSwing);
            glVertex2f(x + 0.38f, y - 0.30f + legSwing);
        glEnd();
    }
}
// ---------------- Human near second building ----------------
void drawSecondBuildingDoorHuman() {
    float f = fireValue();

    // Person will appear only in Fire scene
    if(f <= 0.02f) {
        return;
    }

    float handWave = sin(frameCount * 0.18f) * 0.15f;
    float leftHandWave = -handWave;

    // ---------------- Body ----------------
    glColor3f(1.0f, 0.0f, 0.0f); // red body/shirt
    glBegin(GL_POLYGON);
        glVertex2f(11.47668f, 1.96f);     // A1
        glVertex2f(11.64663f, 1.81302f);  // C1
        glVertex2f(11.65817f, 1.4033f);   // D1
        glVertex2f(11.63798f, 1.04264f);  // K1
        glVertex2f(11.57738f, 1.03975f);  // J1
        glVertex2f(11.55142f, 1.40042f);  // I1
        glVertex2f(11.4f, 1.4f);          // H1
        glVertex2f(11.37541f, 1.03687f);  // G1
        glVertex2f(11.31193f, 1.03398f);  // F1
        glVertex2f(11.2802f, 1.39465f);   // E1
        glVertex2f(11.27443f, 1.80148f);  // B1
    glEnd();

    // ---------------- Left hand ----------------
    glColor3f(0.9f, 0.7f, 0.6f); // skin colour
   glColor3f(0.9f, 0.7f, 0.6f);
glBegin(GL_POLYGON);
    glVertex2f(10.91664f, 1.97749f + leftHandWave);  // O1 moving
    glVertex2f(11.27443f, 1.80148f);                 // B1 fixed
    glVertex2f(11.27758f, 1.7093f);                  // Q1 fixed
    glVertex2f(10.91664f, 1.87749f + leftHandWave);  // P1 moving
glEnd();

    // ---------------- Right waving hand ----------------
    glColor3f(0.9f, 0.7f, 0.6f);
    glBegin(GL_POLYGON);
        glVertex2f(11.64663f, 1.81302f);              // C1
        glVertex2f(11.97267f, 2.06404f + handWave);   // L1 moving
        glVertex2f(11.97556f, 2.02942f + handWave);   // M1 moving
        glVertex2f(11.64663f, 1.71302f);              // N1
    glEnd();

    // ---------------- Head ----------------
    glColor3f(0.9f, 0.7f, 0.6f); // skin colour
    float centerX = 11.45f;
    float centerY = 2.15f;
    float radius = 0.18f;

    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        float X = centerX + radius * cos(angle);
        float Y = centerY + radius * sin(angle);
        glVertex2f(X, Y);
    }
    glEnd();

    // ---------------- Hair ----------------
    glColor3f(0.05f, 0.03f, 0.02f);
    glBegin(GL_POLYGON);
        glVertex2f(11.31f, 2.20f);
        glVertex2f(11.59f, 2.20f);
        glVertex2f(11.56f, 2.32f);
        glVertex2f(11.34f, 2.32f);
    glEnd();

    // ---------------- Eyes ----------------
    glColor3f(0.0f, 0.0f, 0.0f);
    glPointSize(3.0f);
    glBegin(GL_POINTS);
        glVertex2f(11.39f, 2.15f);
        glVertex2f(11.51f, 2.15f);
    glEnd();
}


//newly added running man from second building
void drawDoorRunnerBehindTruck() {
    float f = fireValue();

    if(f <= 0.02f) {
        return;
    }
    if(doorRunnerWait > 0) {
        return;
    }

    float startX = 10.50f;   // second building door
    float startY = 0.95f;

    float endX = 11.25f;     // near fire truck
    float endY = -1.75f;

    float x = startX + (endX - startX) * doorRunnerProgress;
    float y = startY + (endY - startY) * doorRunnerProgress;

    float bodyShake = fabs(sin(frameCount * 0.35f + x)) * 0.04f;
    float armSwing = sin(frameCount * 0.55f + x) * 0.12f;
    float legSwing = cos(frameCount * 0.55f + x) * 0.14f;

    y = y + bodyShake;

    // head
    glColor3f(0.95f, 0.70f, 0.48f);
    drawCircle(x, y + 0.95f, 0.15f);

    // hair
    glColor3f(0.08f, 0.05f, 0.03f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.11f, y + 0.98f);
        glVertex2f(x + 0.11f, y + 0.98f);
        glVertex2f(x + 0.09f, y + 1.08f);
        glVertex2f(x - 0.09f, y + 1.08f);
    glEnd();

    // neck
    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.04f, y + 0.80f);
        glVertex2f(x + 0.04f, y + 0.80f);
        glVertex2f(x + 0.04f, y + 0.70f);
        glVertex2f(x - 0.04f, y + 0.70f);
    glEnd();

    // body
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.17f, y + 0.70f);
        glVertex2f(x + 0.17f, y + 0.70f);
        glVertex2f(x + 0.17f, y + 0.25f);
        glVertex2f(x - 0.17f, y + 0.25f);
    glEnd();

    // hands
    glColor3f(0.95f, 0.70f, 0.48f);
    glLineWidth(4.0f);
    glBegin(GL_LINES);
        glVertex2f(x - 0.15f, y + 0.60f);
        glVertex2f(x - 0.32f, y + 0.42f + armSwing);

        glVertex2f(x + 0.15f, y + 0.60f);
        glVertex2f(x + 0.30f, y + 0.42f - armSwing);
    glEnd();

    // pant
    glColor3f(0.08f, 0.15f, 0.48f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.16f, y + 0.25f);
        glVertex2f(x + 0.16f, y + 0.25f);
        glVertex2f(x + 0.16f, y + 0.10f);
        glVertex2f(x - 0.16f, y + 0.10f);
    glEnd();

    // legs
    glLineWidth(6.0f);
    glBegin(GL_LINES);
        glVertex2f(x - 0.06f, y + 0.10f);
        glVertex2f(x - 0.22f, y - 0.30f - legSwing);

        glVertex2f(x + 0.06f, y + 0.10f);
        glVertex2f(x + 0.18f, y - 0.30f + legSwing);
    glEnd();

    // shoes
    glColor3f(0.02f, 0.02f, 0.02f);
    glLineWidth(5.0f);
    glBegin(GL_LINES);
        glVertex2f(x - 0.22f, y - 0.30f - legSwing);
        glVertex2f(x - 0.36f, y - 0.30f - legSwing);

        glVertex2f(x + 0.18f, y - 0.30f + legSwing);
        glVertex2f(x + 0.32f, y - 0.30f + legSwing);
    glEnd();
}

//Fire truck
//changing
void drawFireTruck() {
    float f = fireValue();

    if(f <= 0.01f) {
        return;
    }

    float truckX = 10.0f * (1.0f - f);

    glPushMatrix();
    glTranslatef(truckX, 0.0f, 0.0f);

    // ---------------- Main truck body ----------------
    glColor3f(0.85f, 0.02f, 0.02f);
    glBegin(GL_POLYGON);
        glVertex2f(10.0f, -1.0f);
        glVertex2f(12.0f, -1.0f);
        glVertex2f(15.98964f, -1.20576f);
        glVertex2f(16.00119f, -2.78821f);
        glVertex2f(9.8f, -2.8f);
        glVertex2f(9.8f, -2.0f);
    glEnd();

    // ---------------- Front cabin ----------------
    glColor3f(0.95f, 0.10f, 0.10f);
    glBegin(GL_POLYGON);
        glVertex2f(10.0f, -1.0f);
        glVertex2f(12.0f, -1.0f);
        glVertex2f(12.0f, -2.8f);
        glVertex2f(10.0f, -2.8f);
    glEnd();

    // ---------------- Driver window ----------------
    glColor3f(0.65f, 0.85f, 0.95f);
    glBegin(GL_POLYGON);
        glVertex2f(11.06741f, -1.25867f);
        glVertex2f(11.664331f, -1.262708f);
        glVertex2f(11.67643f, -1.95643f);
        glVertex2f(11.07547f, -1.96046f);
    glEnd();

    // ---------------- Side equipment box ----------------
    glColor3f(0.35f, 0.35f, 0.35f);
    glBegin(GL_POLYGON);
        glVertex2f(12.29339f, -1.30971f);
        glVertex2f(12.90558f, -1.32127f);
        glVertex2f(12.904542f, -2.598512f);
        glVertex2f(12.292382f, -2.585418f);
    glEnd();

    // ---------------- Siren light ----------------
    glColor3f(1.0f, 0.90f, 0.10f);
    glBegin(GL_POLYGON);
        glVertex2f(10.70478f, -0.79873f);
        glVertex2f(11.13313f, -0.79873f);
        glVertex2f(11.16608f, -0.95841f);
        glVertex2f(10.65662f, -0.95588f);
    glEnd();

    // ---------------- Wheel 1 black ----------------
    glColor3f(0.02f, 0.02f, 0.02f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        glVertex2f(10.926243f + 0.36f * cos(angle), -2.803416f + 0.36f * sin(angle));
    }
    glEnd();

    // ---------------- Wheel 2 black ----------------
    glColor3f(0.02f, 0.02f, 0.02f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        glVertex2f(14.199488f + 0.36f * cos(angle), -2.793518f + 0.36f * sin(angle));
    }
    glEnd();

    // ---------------- Wheel 3 black ----------------
    glColor3f(0.02f, 0.02f, 0.02f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        glVertex2f(15.19264f + 0.36f * cos(angle), -2.76511f + 0.36f * sin(angle));
    }
    glEnd();

    // ---------------- Wheel 1 inner grey ----------------
    glColor3f(0.70f, 0.70f, 0.70f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        glVertex2f(10.926243f + 0.16f * cos(angle), -2.803416f + 0.16f * sin(angle));
    }
    glEnd();

    // ---------------- Wheel 2 inner grey ----------------
    glColor3f(0.70f, 0.70f, 0.70f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        glVertex2f(14.199488f + 0.16f * cos(angle), -2.793518f + 0.16f * sin(angle));
    }
    glEnd();

    // ---------------- Wheel 3 inner grey ----------------
    glColor3f(0.70f, 0.70f, 0.70f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        glVertex2f(15.19264f + 0.16f * cos(angle), -2.76511f + 0.16f * sin(angle));
    }
    glEnd();

    // ---------------- Ladder ----------------
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

    // ---------------- Water pipe and spray ----------------
    if(waterOn == 1)
        {
    glColor3f(0.30f, 0.60f, 1.0f);
    glLineWidth(4.0f);
    glBegin(GL_LINES);
        glVertex2f(12.0f, -1.0f);
        glVertex2f(waterEndX, 6.2f);
    glEnd();

    glColor3f(0.70f, 0.90f, 1.0f);
    glBegin(GL_TRIANGLES);
        glVertex2f(waterEndX, 6.2f);
        glVertex2f(waterEndX - 0.4f, 6.8f);
        glVertex2f(waterEndX + 0.4f, 6.8f);
    glEnd();
    }

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
    // Wheel circles drawn normally
glColor3f(0.95f, 0.95f, 0.95f);

float centerX = 1.790571f;
float centerY = -0.602406f;
float radius = 0.25f;

glBegin(GL_LINE_LOOP);
for(int i = 0; i < 360; i++) {
    float angle = i * 3.1416f / 180.0f;
    float X = centerX + radius * cos(angle);
    float Y = centerY + radius * sin(angle);
    glVertex2f(X, Y);
}
glEnd();

centerX = 4.406276f;
centerY = -0.558444f;
radius = 0.25f;

glBegin(GL_LINE_LOOP);
for(int i = 0; i < 360; i++) {
    float angle = i * 3.1416f / 180.0f;
    float X = centerX + radius * cos(angle);
    float Y = centerY + radius * sin(angle);
    glVertex2f(X, Y);
}
glEnd();

    glDisable(GL_CLIP_PLANE0);
    glPopMatrix();
}

void drawRescueText(float x, float y) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);

    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(0.12f, 0.12f, 1.0f);



    glPopMatrix();
}
void drawText(float x, float y, const char *string) {
    glRasterPos2f(x, y);

    while(*string) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *string);
        string++;
    }
}

void drawRescueBoat() {
    float fl = floodValue();

    if(fl <= 0.3f) {
        return;
    }

    float waterLevel = -3.9f + 5.5f * fl;
    float boatX = 10.0f + sin(frameCount * 0.01f) * 3.0f;

    glPushMatrix();
    glTranslatef(boatX, waterLevel - 0.2f, 0.0f);

    // ---------------- Rescue Boat ----------------
    glColor3f(1.0f, 0.5f, 0.0f);
    glBegin(GL_POLYGON);
        glVertex2f(-2.0f, 0.0f);
        glVertex2f(2.0f, 0.0f);
        glVertex2f(1.5f, -0.7f);
        glVertex2f(-1.5f, -0.7f);
    glEnd();

    // boat lower shade
    glColor3f(0.6f, 0.2f, 0.0f);
    glBegin(GL_POLYGON);
        glVertex2f(-1.5f, -0.35f);
        glVertex2f(1.5f, -0.35f);
        glVertex2f(1.5f, -0.7f);
        glVertex2f(-1.5f, -0.7f);
    glEnd();

    // ---------------- Rescue Team Text ----------------
    // ---------------- Rescue Team Text ----------------
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(-0.85f, -0.35f, "RESCUE TEAM");

    // =====================================================
    // Person 1 on boat
    // =====================================================

    float x = -0.8f;
    float y = 0.2f;

    // body
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.12f, y - 0.20f);
        glVertex2f(x + 0.12f, y - 0.20f);
        glVertex2f(x + 0.12f, y + 0.05f);
        glVertex2f(x - 0.12f, y + 0.05f);
    glEnd();

    // neck
    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.03f, y + 0.05f);
        glVertex2f(x + 0.03f, y + 0.05f);
        glVertex2f(x + 0.03f, y + 0.10f);
        glVertex2f(x - 0.03f, y + 0.10f);
    glEnd();

    // head
    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        glVertex2f(x + 0.10f * cos(angle), y + 0.20f + 0.10f * sin(angle));
    }
    glEnd();

    // hair
    glColor3f(0.05f, 0.03f, 0.02f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.09f, y + 0.22f);
        glVertex2f(x + 0.09f, y + 0.22f);
        glVertex2f(x + 0.07f, y + 0.30f);
        glVertex2f(x - 0.07f, y + 0.30f);
    glEnd();

    // =====================================================
    // Person 2 on boat
    // =====================================================

    x = 0.0f;
    y = 0.1f;

    // body
    glColor3f(0.0f, 0.5f, 0.0f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.12f, y - 0.20f);
        glVertex2f(x + 0.12f, y - 0.20f);
        glVertex2f(x + 0.12f, y + 0.05f);
        glVertex2f(x - 0.12f, y + 0.05f);
    glEnd();

    // neck
    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.03f, y + 0.05f);
        glVertex2f(x + 0.03f, y + 0.05f);
        glVertex2f(x + 0.03f, y + 0.10f);
        glVertex2f(x - 0.03f, y + 0.10f);
    glEnd();

    // head
    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        glVertex2f(x + 0.10f * cos(angle), y + 0.20f + 0.10f * sin(angle));
    }
    glEnd();

    // hair
    glColor3f(0.05f, 0.03f, 0.02f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.09f, y + 0.22f);
        glVertex2f(x + 0.09f, y + 0.22f);
        glVertex2f(x + 0.07f, y + 0.30f);
        glVertex2f(x - 0.07f, y + 0.30f);
    glEnd();


    // Person 3 on boat


    x = 0.8f;
    y = 0.2f;

    // body
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.12f, y - 0.20f);
        glVertex2f(x + 0.12f, y - 0.20f);
        glVertex2f(x + 0.12f, y + 0.05f);
        glVertex2f(x - 0.12f, y + 0.05f);
    glEnd();

    // neck
    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.03f, y + 0.05f);
        glVertex2f(x + 0.03f, y + 0.05f);
        glVertex2f(x + 0.03f, y + 0.10f);
        glVertex2f(x - 0.03f, y + 0.10f);
    glEnd();

    // head
    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        glVertex2f(x + 0.10f * cos(angle), y + 0.20f + 0.10f * sin(angle));
    }
    glEnd();

    // hair
    glColor3f(0.05f, 0.03f, 0.02f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.09f, y + 0.22f);
        glVertex2f(x + 0.09f, y + 0.22f);
        glVertex2f(x + 0.07f, y + 0.30f);
        glVertex2f(x - 0.07f, y + 0.30f);
    glEnd();

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
            glColor3f(0.5f, 0.3f, 0.1f);
            glVertex2f(-0.5f, 0.05f);
            glColor3f(0.5f, 0.3f, 0.1f);
            glVertex2f(0.5f, 0.05f);
            glColor3f(0.2f, 0.1f, 0.0f);
            glVertex2f(0.5f, -0.05f);
            glColor3f(0.2f, 0.1f, 0.0f);
            glVertex2f(-0.5f, -0.05f);
    glEnd();
    glPopMatrix();
}
void drawFloatingDebris() {
    float fl = floodValue();

    if(fl <= 0.15f) {
        return;
    }

    float waterLevel = -3.9f + 5.5f * fl;
    float waveOffset = sin(frameCount * 0.12f) * 0.08f * fl;
    float y;

    // ---------------- Bottle 1 ----------------
    y = waterLevel - 1.5f + waveOffset;

    glColor3f(0.0f, 0.7f, 0.9f);
    glBegin(GL_QUADS);
        glVertex2f(-3.0f, y);
        glVertex2f(-2.7f, y);
        glVertex2f(-2.7f, y + 0.15f);
        glVertex2f(-3.0f, y + 0.15f);
    glEnd();

    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
        glVertex2f(-2.7f, y + 0.05f);
        glVertex2f(-2.6f, y + 0.05f);
        glVertex2f(-2.6f, y + 0.10f);
        glVertex2f(-2.7f, y + 0.10f);
    glEnd();

    // ---------------- Bottle 2 ----------------
    y = waterLevel - 2.2f + waveOffset;

    glColor3f(0.1f, 0.8f, 0.2f);
    glBegin(GL_QUADS);
        glVertex2f(8.0f, y);
        glVertex2f(8.3f, y);
        glVertex2f(8.3f, y + 0.15f);
        glVertex2f(8.0f, y + 0.15f);
    glEnd();

    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
        glVertex2f(8.3f, y + 0.05f);
        glVertex2f(8.4f, y + 0.05f);
        glVertex2f(8.4f, y + 0.10f);
        glVertex2f(8.3f, y + 0.10f);
    glEnd();

    // ---------------- Bottle 3 ----------------
    y = waterLevel - 1.0f + waveOffset;

    glColor3f(0.9f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
        glVertex2f(18.0f, y);
        glVertex2f(18.3f, y);
        glVertex2f(18.3f, y + 0.15f);
        glVertex2f(18.0f, y + 0.15f);
    glEnd();

    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
        glVertex2f(18.3f, y + 0.05f);
        glVertex2f(18.4f, y + 0.05f);
        glVertex2f(18.4f, y + 0.10f);
        glVertex2f(18.3f, y + 0.10f);
    glEnd();

    // ---------------- Paper 1 ----------------
    y = waterLevel - 0.02f + waveOffset;

    glColor3f(0.85f, 0.85f, 0.85f);
    glBegin(GL_QUADS);
        glVertex2f(15.0f, y);
        glVertex2f(15.5f, y + 0.05f);
        glVertex2f(15.45f, y + 0.25f);
        glVertex2f(14.95f, y + 0.20f);
    glEnd();

    // ---------------- Paper 2 ----------------
    glColor3f(0.85f, 0.85f, 0.85f);
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, y);
        glVertex2f(-0.5f, y + 0.05f);
        glVertex2f(-0.55f, y + 0.25f);
        glVertex2f(-1.05f, y + 0.20f);
    glEnd();
}
void drawHeavyRain() {
    float st = floodValue();

    if(st <= 0.1f) {
        return;
    }

    glColor3f(0.8f, 0.8f, 1.0f);
    glLineWidth(1.5f);

    for(int i = 0; i < 100; i++) {
        float rx = -4.0f + (float)(rand() % 260) / 10.0f;
        float ry = (float)(rand() % 160) / 10.0f - 4.0f;

        glBegin(GL_LINES);
            glVertex2f(rx, ry);
            glVertex2f(rx - 0.1f, ry - 0.4f);
        glEnd();
    }

    glLineWidth(1.0f);
}

//----

void drawFloodWindowPeople() {
    float fl = floodValue();

    if(fl <= 0.02f) {
        return;
    }

    float x, y, move;


    // Person 1

    x = 2.50f;
    y = 7.58f;
    move = sin(frameCount * 0.09f + x) * 0.02f;

    // shirt
    glColor3f(0.95f, 0.15f, 0.15f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.16f + move, y - 0.28f);
        glVertex2f(x + 0.16f + move, y - 0.28f);
        glVertex2f(x + 0.16f + move, y + 0.03f);
        glVertex2f(x - 0.16f + move, y + 0.03f);
    glEnd();

    // neck
    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.035f + move, y + 0.03f);
        glVertex2f(x + 0.035f + move, y + 0.03f);
        glVertex2f(x + 0.035f + move, y + 0.10f);
        glVertex2f(x - 0.035f + move, y + 0.10f);
    glEnd();

    // head
    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        float X = x + move + 0.11f * cos(angle);
        float Y = y + 0.18f + 0.11f * sin(angle);
        glVertex2f(X, Y);
    }
    glEnd();

    // hair
    glColor3f(0.06f, 0.04f, 0.03f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.10f + move, y + 0.20f);
        glVertex2f(x + 0.10f + move, y + 0.20f);
        glVertex2f(x + 0.08f + move, y + 0.30f);
        glVertex2f(x - 0.08f + move, y + 0.30f);
    glEnd();

    // hands
    glColor3f(0.95f, 0.70f, 0.48f);
    glLineWidth(2.5f);
    glBegin(GL_LINES);
        glVertex2f(x - 0.12f + move, y - 0.04f);
        glVertex2f(x - 0.25f + move, y - 0.16f);
        glVertex2f(x + 0.12f + move, y - 0.04f);
        glVertex2f(x + 0.25f + move, y - 0.16f);
    glEnd();

    // eyes
    glColor3f(0.18f, 0.18f, 0.18f);
    glPointSize(2.0f);
    glBegin(GL_POINTS);
        glVertex2f(x - 0.03f + move, y + 0.18f);
        glVertex2f(x + 0.03f + move, y + 0.18f);
    glEnd();


    // Person 2

    x = 4.50f;
    y = 7.58f;
    move = sin(frameCount * 0.09f + x) * 0.02f;

    glColor3f(0.15f, 0.60f, 0.95f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.16f + move, y - 0.28f);
        glVertex2f(x + 0.16f + move, y - 0.28f);
        glVertex2f(x + 0.16f + move, y + 0.03f);
        glVertex2f(x - 0.16f + move, y + 0.03f);
    glEnd();

    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.035f + move, y + 0.03f);
        glVertex2f(x + 0.035f + move, y + 0.03f);
        glVertex2f(x + 0.035f + move, y + 0.10f);
        glVertex2f(x - 0.035f + move, y + 0.10f);
    glEnd();

    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        float X = x + move + 0.11f * cos(angle);
        float Y = y + 0.18f + 0.11f * sin(angle);
        glVertex2f(X, Y);
    }
    glEnd();

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


    // Person 3

    x = 9.50f;
    y = 8.35f;
    move = sin(frameCount * 0.09f + x) * 0.02f;

    glColor3f(0.95f, 0.80f, 0.10f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.16f + move, y - 0.28f);
        glVertex2f(x + 0.16f + move, y - 0.28f);
        glVertex2f(x + 0.16f + move, y + 0.03f);
        glVertex2f(x - 0.16f + move, y + 0.03f);
    glEnd();

    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.035f + move, y + 0.03f);
        glVertex2f(x + 0.035f + move, y + 0.03f);
        glVertex2f(x + 0.035f + move, y + 0.10f);
        glVertex2f(x - 0.035f + move, y + 0.10f);
    glEnd();

    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        float X = x + move + 0.11f * cos(angle);
        float Y = y + 0.18f + 0.11f * sin(angle);
        glVertex2f(X, Y);
    }
    glEnd();

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


    // Person 4

    x = 11.50f;
    y = 8.35f;
    move = sin(frameCount * 0.09f + x) * 0.02f;

    glColor3f(0.25f, 0.85f, 0.30f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.16f + move, y - 0.28f);
        glVertex2f(x + 0.16f + move, y - 0.28f);
        glVertex2f(x + 0.16f + move, y + 0.03f);
        glVertex2f(x - 0.16f + move, y + 0.03f);
    glEnd();

    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.035f + move, y + 0.03f);
        glVertex2f(x + 0.035f + move, y + 0.03f);
        glVertex2f(x + 0.035f + move, y + 0.10f);
        glVertex2f(x - 0.035f + move, y + 0.10f);
    glEnd();

    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        float X = x + move + 0.11f * cos(angle);
        float Y = y + 0.18f + 0.11f * sin(angle);
        glVertex2f(X, Y);
    }
    glEnd();

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


    // Person 5
    x = 9.50f;
    y = 6.35f;
    move = sin(frameCount * 0.09f + x) * 0.02f;

    glColor3f(0.75f, 0.25f, 0.90f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.16f + move, y - 0.28f);
        glVertex2f(x + 0.16f + move, y - 0.28f);
        glVertex2f(x + 0.16f + move, y + 0.03f);
        glVertex2f(x - 0.16f + move, y + 0.03f);
    glEnd();

    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.035f + move, y + 0.03f);
        glVertex2f(x + 0.035f + move, y + 0.03f);
        glVertex2f(x + 0.035f + move, y + 0.10f);
        glVertex2f(x - 0.035f + move, y + 0.10f);
    glEnd();

    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        float X = x + move + 0.11f * cos(angle);
        float Y = y + 0.18f + 0.11f * sin(angle);
        glVertex2f(X, Y);
    }
    glEnd();

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


    // Person 6

    x = 11.50f;
    y = 6.35f;
    move = sin(frameCount * 0.09f + x) * 0.02f;

    glColor3f(0.95f, 0.40f, 0.10f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.16f + move, y - 0.28f);
        glVertex2f(x + 0.16f + move, y - 0.28f);
        glVertex2f(x + 0.16f + move, y + 0.03f);
        glVertex2f(x - 0.16f + move, y + 0.03f);
    glEnd();

    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.035f + move, y + 0.03f);
        glVertex2f(x + 0.035f + move, y + 0.03f);
        glVertex2f(x + 0.035f + move, y + 0.10f);
        glVertex2f(x - 0.035f + move, y + 0.10f);
    glEnd();

    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        float X = x + move + 0.11f * cos(angle);
        float Y = y + 0.18f + 0.11f * sin(angle);
        glVertex2f(X, Y);
    }
    glEnd();

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

    // =====================================================
    // Person 7
    // =====================================================
    x = 16.50f;
    y = 7.58f;
    move = sin(frameCount * 0.09f + x) * 0.02f;

    glColor3f(0.20f, 0.75f, 0.95f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.16f + move, y - 0.28f);
        glVertex2f(x + 0.16f + move, y - 0.28f);
        glVertex2f(x + 0.16f + move, y + 0.03f);
        glVertex2f(x - 0.16f + move, y + 0.03f);
    glEnd();

    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.035f + move, y + 0.03f);
        glVertex2f(x + 0.035f + move, y + 0.03f);
        glVertex2f(x + 0.035f + move, y + 0.10f);
        glVertex2f(x - 0.035f + move, y + 0.10f);
    glEnd();

    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        float X = x + move + 0.11f * cos(angle);
        float Y = y + 0.18f + 0.11f * sin(angle);
        glVertex2f(X, Y);
    }
    glEnd();

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


    // Person 8

    x = 18.50f;
    y = 7.58f;
    move = sin(frameCount * 0.09f + x) * 0.02f;

    glColor3f(0.95f, 0.20f, 0.35f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.16f + move, y - 0.28f);
        glVertex2f(x + 0.16f + move, y - 0.28f);
        glVertex2f(x + 0.16f + move, y + 0.03f);
        glVertex2f(x - 0.16f + move, y + 0.03f);
    glEnd();

    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.035f + move, y + 0.03f);
        glVertex2f(x + 0.035f + move, y + 0.03f);
        glVertex2f(x + 0.035f + move, y + 0.10f);
        glVertex2f(x - 0.035f + move, y + 0.10f);
    glEnd();

    glColor3f(0.95f, 0.70f, 0.48f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i++) {
        float angle = i * 3.1416f / 180.0f;
        float X = x + move + 0.11f * cos(angle);
        float Y = y + 0.18f + 0.11f * sin(angle);
        glVertex2f(X, Y);
    }
    glEnd();

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

float centerX = x;
float centerY = y;
float radius = 0.55f * s;

glBegin(GL_LINE_LOOP);
for(int i = 0; i < 360; i++) {
    float angle = i * 3.1416f / 180.0f;
    float X = centerX + radius * cos(angle);
    float Y = centerY + radius * sin(angle);
    glVertex2f(X, Y);
}
glEnd();

centerX = x + 0.55f * s;
centerY = y + 0.12f * s;
radius = 0.70f * s;

glBegin(GL_LINE_LOOP);
for(int i = 0; i < 360; i++) {
    float angle = i * 3.1416f / 180.0f;
    float X = centerX + radius * cos(angle);
    float Y = centerY + radius * sin(angle);
    glVertex2f(X, Y);
}
glEnd();

centerX = x + 1.25f * s;
centerY = y;
radius = 0.55f * s;

glBegin(GL_LINE_LOOP);
for(int i = 0; i < 360; i++) {
    float angle = i * 3.1416f / 180.0f;
    float X = centerX + radius * cos(angle);
    float Y = centerY + radius * sin(angle);
    glVertex2f(X, Y);
}
glEnd();
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

    int flash = frameCount % 80;

    if(flash > 16) {
        return;
    }

    // ---------------- Main thick yellow lightning ----------------
    glColor3f(1.0f, 1.0f, 0.25f);
    glLineWidth(6.0f);

    glBegin(GL_LINE_STRIP);
        glVertex2f(13.25f, 9.80f);
        glVertex2f(12.65f, 8.95f);
        glVertex2f(13.15f, 8.95f);
        glVertex2f(12.45f, 7.85f);
        glVertex2f(12.95f, 7.85f);
        glVertex2f(12.25f, 6.65f);
    glEnd();

    // ---------------- Midpoint Line Algorithm -----------


    glColor3f(1.0f, 1.0f, 0.45f);
    glPointSize(4.0f);

    float x1 = 12.85f;
    float y1 = 8.40f;
    float x2 = 13.50f;
    float y2 = 8.65f;

    float i, j, p;
    int dx = (int)((x2 - x1) * 100.0f);
    int dy = (int)((y2 - y1) * 100.0f);

    i = x1 * 100.0f;
    j = y1 * 100.0f;

    p = (2 * dy) - dx;

    glBegin(GL_POINTS);
        while(i <= x2 * 100.0f && j <= y2 * 100.0f) {
            glVertex2f(i / 100.0f, j / 100.0f);

            if(p >= 0) {
                i = i + 1;
                j = j + 1;
                p = p + (2 * dy) - (2 * dx);
            }
            else {
                i = i + 1;
                p = p + (2 * dy);
            }
        }
    glEnd();

    // ---------------- Other small branches drawn simply ----------------
    glColor3f(1.0f, 1.0f, 0.35f);
    glLineWidth(4.0f);

    glBegin(GL_LINES);
        glVertex2f(12.65f, 8.95f);
        glVertex2f(12.20f, 8.55f);

        glVertex2f(12.45f, 7.85f);
        glVertex2f(12.05f, 7.55f);

        glVertex2f(12.95f, 7.85f);
        glVertex2f(13.35f, 7.55f);
    glEnd();

    // ---------------- White inner highlight ----------------
    glColor3f(1.0f, 1.0f, 0.90f);
    glLineWidth(2.0f);

    glBegin(GL_LINE_STRIP);
        glVertex2f(13.25f, 9.80f);
        glVertex2f(12.65f, 8.95f);
        glVertex2f(13.15f, 8.95f);
        glVertex2f(12.45f, 7.85f);
        glVertex2f(12.95f, 7.85f);
        glVertex2f(12.25f, 6.65f);
    glEnd();

    glBegin(GL_LINES);
        glVertex2f(12.85f, 8.40f);
        glVertex2f(13.50f, 8.65f);
    glEnd();

    glLineWidth(1.0f);
    glPointSize(1.0f);
}
void drawRain() {
    float st = stormValue();

    if(st <= 0.01f) {
        return;
    }

    glColor3f(0.70f, 0.88f, 1.0f);
    glPointSize(2.0f);
    glLineWidth(1.5f);

    for(int i = 0; i < 110; i++) {
        float x = -4.0f + (i % 22) * 1.25f;
        float baseY = -3.6f + (i / 22) * 2.9f;
        float fall = fmod(frameCount * 0.20f + i * 0.37f, 3.0f);
        float y = baseY - fall;

        if(i == 0) {
            // DDA Line Algorithm used one time only
            float x1 = x + 0.25f * st;
            float y1 = y + 0.85f;
            float x2 = x - 0.22f * st;
            float y2 = y + 0.05f;

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

            float xInc = dx / steps;
            float yInc = dy / steps;

            float ddaX = X1;
            float ddaY = Y1;

            glBegin(GL_POINTS);
            for(int j = 0; j <= steps; j++) {
                glVertex2f(ddaX / scale, ddaY / scale);

                ddaX = ddaX + xInc;
                ddaY = ddaY + yInc;
            }
            glEnd();
        } else {
            // Other rain drops drawn simply
            glBegin(GL_LINES);
                glVertex2f(x + 0.25f * st, y + 0.85f);
                glVertex2f(x - 0.22f * st, y + 0.05f);
            glEnd();
        }
    }

    glLineWidth(1.0f);
}
//newly added
void drawFlyingPaperAndLeaf() {
    float st = stormValue();

    if(st <= 0.01f) {
        return;
    }

    float paperMove1 = fmod(frameCount * 0.05f, 25.0f);
    float paperMove2 = fmod(frameCount * 0.04f, 18.0f);
    float paperMove3 = -fmod(frameCount * 0.05f, 25.0f);
    float leafMove = fmod(frameCount * 0.06f, 25.0f);

    float paperWave1 = sin(frameCount * 0.10f) * 0.20f;
    float paperWave2 = cos(frameCount * 0.12f) * 0.18f;
    float paperWave3 = sin(frameCount * 0.13f) * 0.20f;
    float leafWave = sin(frameCount * 0.18f) * 0.18f;

    // ---------------- Flying Paper 1 ----------------

    // P1 = (-2.2, 8.8), P2 = (-1.5, 9.1), P3 = (-1.2, 8.6), P4 = (-1.9, 8.4)

    glPushMatrix();
    glTranslatef(paperMove1, paperWave1, 0.0f);

    glColor3f(0.95f, 0.88f, 0.75f);
    glBegin(GL_POLYGON);
        glVertex2f(-2.2f, 8.8f);
        glVertex2f(-1.5f, 9.1f);
        glVertex2f(-1.2f, 8.6f);
        glVertex2f(-1.9f, 8.4f);
    glEnd();

    glColor3f(0.55f, 0.35f, 0.20f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-2.2f, 8.8f);
        glVertex2f(-1.5f, 9.1f);
        glVertex2f(-1.2f, 8.6f);
        glVertex2f(-1.9f, 8.4f);
    glEnd();

    glBegin(GL_LINES);
        glVertex2f(-2.2f, 8.8f);
        glVertex2f(-1.2f, 8.6f);

        glVertex2f(-1.5f, 9.1f);
        glVertex2f(-1.9f, 8.4f);
    glEnd();

    glPopMatrix();


    // ---------------- Flying Paper 2 ----------------

    // Q1 = (6, 9.8), Q2 = (6.7, 10.1), Q3 = (7, 9.6), Q4 = (6.3, 9.4)

    glPushMatrix();
    glTranslatef(paperMove2, paperWave2, 0.0f);

    glColor3f(0.95f, 0.88f, 0.75f);
    glBegin(GL_POLYGON);
        glVertex2f(6.0f, 9.8f);
        glVertex2f(6.7f, 10.1f);
        glVertex2f(7.0f, 9.6f);
        glVertex2f(6.3f, 9.4f);
    glEnd();

    glColor3f(0.55f, 0.35f, 0.20f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(6.0f, 9.8f);
        glVertex2f(6.7f, 10.1f);
        glVertex2f(7.0f, 9.6f);
        glVertex2f(6.3f, 9.4f);
    glEnd();

    glBegin(GL_LINES);
        glVertex2f(6.0f, 9.8f);
        glVertex2f(7.0f, 9.6f);

        glVertex2f(6.7f, 10.1f);
        glVertex2f(6.3f, 9.4f);
    glEnd();

    glPopMatrix();


    // ---------------- Flying Paper 3 ----------------



    glPushMatrix();
    glTranslatef(paperMove3, paperWave3, 0.0f);

    glColor3f(0.95f, 0.88f, 0.75f);
    glBegin(GL_POLYGON);
        glVertex2f(19.5f, 7.8f);
        glVertex2f(20.3f, 8.1f);
        glVertex2f(20.6f, 7.5f);
        glVertex2f(19.8f, 7.3f);
    glEnd();

    glColor3f(0.55f, 0.35f, 0.20f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(19.5f, 7.8f);
        glVertex2f(20.3f, 8.1f);
        glVertex2f(20.6f, 7.5f);
        glVertex2f(19.8f, 7.3f);
    glEnd();

    glBegin(GL_LINES);
        glVertex2f(19.5f, 7.8f);
        glVertex2f(20.6f, 7.5f);

        glVertex2f(20.3f, 8.1f);
        glVertex2f(19.8f, 7.3f);
    glEnd();

    glPopMatrix();


    // ---------------- Flying Leaf ----------------


   // ---------------- Flying Leaf 1 ----------------
glPushMatrix();
glTranslatef(leafMove, leafWave, 0.0f);

glColor3f(0.35f, 0.70f, 0.10f);
glBegin(GL_POLYGON);
    glVertex2f(-2.80f, 1.80f);
    glVertex2f(-2.62f, 1.97f);
    glVertex2f(-2.43f, 1.93f);
    glVertex2f(-2.16f, 2.03f);
    glVertex2f(-2.20f, 1.80f);
    glVertex2f(-2.34f, 1.64f);
    glVertex2f(-2.54f, 1.59f);
    glVertex2f(-2.76f, 1.63f);
glEnd();

glColor3f(0.15f, 0.35f, 0.05f);
glLineWidth(2.0f);
glBegin(GL_LINE_LOOP);
    glVertex2f(-2.80f, 1.80f);
    glVertex2f(-2.62f, 1.97f);
    glVertex2f(-2.43f, 1.93f);
    glVertex2f(-2.16f, 2.03f);
    glVertex2f(-2.20f, 1.80f);
    glVertex2f(-2.34f, 1.64f);
    glVertex2f(-2.54f, 1.59f);
    glVertex2f(-2.76f, 1.63f);
glEnd();

// fixed petiole
glLineWidth(3.0f);
glBegin(GL_LINES);
    glVertex2f(-2.80f, 1.80f);
    glVertex2f(-2.96f, 1.70f);
glEnd();

glPopMatrix();


// ---------------- Flying Leaf 2 ----------------
glPushMatrix();
glTranslatef(leafMove + 1.30f, leafWave + 0.15f, 0.0f);

glColor3f(0.40f, 0.75f, 0.12f);
glBegin(GL_POLYGON);
    glVertex2f(-1.49f, 1.40f);
    glVertex2f(-1.51f, 1.52f);
    glVertex2f(-1.31f, 1.60f);
    glVertex2f(-1.10f, 1.54f);
    glVertex2f(-0.91f, 1.38f);
    glVertex2f(-1.09f, 1.27f);
    glVertex2f(-1.33f, 1.24f);
    glVertex2f(-1.51f, 1.30f);
glEnd();

glColor3f(0.15f, 0.35f, 0.05f);
glLineWidth(2.0f);
glBegin(GL_LINE_LOOP);
    glVertex2f(-1.49f, 1.40f);
    glVertex2f(-1.51f, 1.52f);
    glVertex2f(-1.31f, 1.60f);
    glVertex2f(-1.10f, 1.54f);
    glVertex2f(-0.91f, 1.38f);
    glVertex2f(-1.09f, 1.27f);
    glVertex2f(-1.33f, 1.24f);
    glVertex2f(-1.51f, 1.30f);
glEnd();

// fixed petiole
glLineWidth(3.0f);
glBegin(GL_LINES);
    glVertex2f(-1.51f, 1.40f);
    glVertex2f(-1.75f, 1.36f);
glEnd();

glPopMatrix();


// ---------------- Flying Leaf 3 ----------------
glPushMatrix();
glTranslatef(leafMove + 3.30f, leafWave + 0.30f, 0.0f);

glColor3f(0.38f, 0.72f, 0.10f);
glBegin(GL_POLYGON);
    glVertex2f(0.20f, 7.60f);
    glVertex2f(0.21f, 7.72f);
    glVertex2f(0.33f, 7.71f);
    glVertex2f(0.46f, 7.65f);
    glVertex2f(0.76f, 7.60f);
    glVertex2f(0.52f, 7.43f);
    glVertex2f(0.30f, 7.42f);
    glVertex2f(0.14f, 7.50f);
glEnd();

glColor3f(0.15f, 0.35f, 0.05f);
glLineWidth(2.0f);
glBegin(GL_LINE_LOOP);
    glVertex2f(0.20f, 7.60f);
    glVertex2f(0.21f, 7.72f);
    glVertex2f(0.33f, 7.71f);
    glVertex2f(0.46f, 7.65f);
    glVertex2f(0.76f, 7.60f);
    glVertex2f(0.52f, 7.43f);
    glVertex2f(0.30f, 7.42f);
    glVertex2f(0.14f, 7.50f);
glEnd();

// fixed petiole
glLineWidth(3.0f);
glBegin(GL_LINES);
    glVertex2f(0.14f, 7.50f);
    glVertex2f(-0.10f, 7.67f);
glEnd();

glPopMatrix();


// ---------------- Flying Leaf 4 ----------------
glPushMatrix();
glTranslatef(-leafMove, leafWave + 0.10f, 0.0f);

glColor3f(0.36f, 0.70f, 0.10f);
glBegin(GL_POLYGON);
    glVertex2f(13.98f, 6.49f);
    glVertex2f(14.12f, 6.75f);
    glVertex2f(14.29f, 6.69f);
    glVertex2f(14.52f, 6.64f);
    glVertex2f(14.88f, 6.64f);
    glVertex2f(14.52f, 6.32f);
    glVertex2f(14.20f, 6.20f);
    glVertex2f(13.99f, 6.30f);
glEnd();

glColor3f(0.15f, 0.35f, 0.05f);
glLineWidth(2.0f);
glBegin(GL_LINE_LOOP);
    glVertex2f(13.98f, 6.49f);
    glVertex2f(14.12f, 6.75f);
    glVertex2f(14.29f, 6.69f);
    glVertex2f(14.52f, 6.64f);
    glVertex2f(14.88f, 6.64f);
    glVertex2f(14.52f, 6.32f);
    glVertex2f(14.20f, 6.20f);
    glVertex2f(13.99f, 6.30f);
glEnd();

// fixed petiole
glLineWidth(3.0f);
glBegin(GL_LINES);
    glVertex2f(13.98f, 6.49f);
    glVertex2f(13.75f, 6.55f);
glEnd();

glPopMatrix();

    glLineWidth(1.0f);
}


void drawStormPeople() {
    float st = stormValue();

    if(st <= 0.01f) {
        return;
    }

    float x, y, s, dir;
    float sway;
    float umbrellaX, umbrellaY;
    float angleValue;


    // Storm Person 1 with Umbrella

    x = 3.5f + 0.55f * st;
    y = 0.92f - 0.12f * st;
    s = 0.88f;
    dir = 1.0f;

    sway = sin(frameCount * 0.12f + x) * 0.06f * st;
    x = x + sway;

    umbrellaX = x + 0.05f * dir;
    umbrellaY = y + 1.58f * s;

    // umbrella top
    glColor3f(0.85f, 0.10f, 0.16f);
    glBegin(GL_POLYGON);
        glVertex2f(umbrellaX - 0.75f * s, umbrellaY);

        for(int i = 0; i <= 24; i++) {
            angleValue = 3.1416f - i * 3.1416f / 24.0f;
            glVertex2f(umbrellaX + cos(angleValue) * 0.75f * s,
                       umbrellaY + sin(angleValue) * 0.52f * s);
        }

        glVertex2f(umbrellaX + 0.75f * s, umbrellaY);
    glEnd();

    // umbrella border
    glColor3f(0.08f, 0.08f, 0.08f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        for(int i = 0; i <= 24; i++) {
            angleValue = 3.1416f - i * 3.1416f / 24.0f;
            glVertex2f(umbrellaX + cos(angleValue) * 0.75f * s,
                       umbrellaY + sin(angleValue) * 0.52f * s);
        }
    glEnd();

    // umbrella handle
    glBegin(GL_LINES);
        glVertex2f(umbrellaX, umbrellaY);
        glVertex2f(umbrellaX, umbrellaY - 1.10f * s);

        glVertex2f(umbrellaX, umbrellaY);
        glVertex2f(umbrellaX - 0.55f * s, umbrellaY + 0.10f * s);

        glVertex2f(umbrellaX, umbrellaY);
        glVertex2f(umbrellaX + 0.55f * s, umbrellaY + 0.10f * s);
    glEnd();

    // head
    glColor3f(0.95f, 0.70f, 0.48f);
    drawCircle(x, y + 1.02f * s, 0.16f * s);

    // hair
    glColor3f(0.05f, 0.035f, 0.02f);
    drawCircle(x, y + 1.12f * s, 0.12f * s);

    // body
    glColor3f(0.95f, 0.25f, 0.18f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.24f * s, y + 0.82f * s);
        glVertex2f(x + 0.24f * s, y + 0.82f * s);
        glVertex2f(x + 0.19f * s, y + 0.30f * s);
        glVertex2f(x - 0.19f * s, y + 0.30f * s);
    glEnd();

    // hands
    glColor3f(0.95f, 0.70f, 0.48f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
        glVertex2f(x + 0.15f * s, y + 0.70f * s);
        glVertex2f(x + 0.08f * dir * s, y + 1.05f * s);

        glVertex2f(x - 0.15f * s, y + 0.70f * s);
        glVertex2f(x - 0.40f * dir * s, y + 0.48f * s);
    glEnd();

    // legs
    glColor3f(0.05f, 0.12f, 0.40f);
    glLineWidth(5.0f);
    glBegin(GL_LINES);
        glVertex2f(x - 0.08f * s, y + 0.30f * s);
        glVertex2f(x - 0.25f * s, y - 0.25f * s);

        glVertex2f(x + 0.08f * s, y + 0.30f * s);
        glVertex2f(x + 0.25f * s, y - 0.25f * s);
    glEnd();

    // shoes
    glColor3f(0.02f, 0.02f, 0.02f);
    glLineWidth(4.0f);
    glBegin(GL_LINES);
        glVertex2f(x - 0.25f * s, y - 0.25f * s);
        glVertex2f(x - 0.43f * s, y - 0.25f * s);

        glVertex2f(x + 0.25f * s, y - 0.25f * s);
        glVertex2f(x + 0.43f * s, y - 0.25f * s);
    glEnd();

    // =====================================================
    // Storm Person 2 with Umbrella
    // =====================================================
    x = 10.5f - 0.55f * st;
    y = 0.92f - 0.12f * st;
    s = 0.88f;
    dir = -1.0f;

    sway = sin(frameCount * 0.12f + x) * 0.06f * st;
    x = x + sway;

    umbrellaX = x + 0.05f * dir;
    umbrellaY = y + 1.58f * s;

    // umbrella top
    glColor3f(0.12f, 0.20f, 0.75f);
    glBegin(GL_POLYGON);
        glVertex2f(umbrellaX - 0.75f * s, umbrellaY);

        for(int i = 0; i <= 24; i++) {
            angleValue = 3.1416f - i * 3.1416f / 24.0f;
            glVertex2f(umbrellaX + cos(angleValue) * 0.75f * s,
                       umbrellaY + sin(angleValue) * 0.52f * s);
        }

        glVertex2f(umbrellaX + 0.75f * s, umbrellaY);
    glEnd();

    // umbrella border
    glColor3f(0.08f, 0.08f, 0.08f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        for(int i = 0; i <= 24; i++) {
            angleValue = 3.1416f - i * 3.1416f / 24.0f;
            glVertex2f(umbrellaX + cos(angleValue) * 0.75f * s,
                       umbrellaY + sin(angleValue) * 0.52f * s);
        }
    glEnd();

    // umbrella handle
    glBegin(GL_LINES);
        glVertex2f(umbrellaX, umbrellaY);
        glVertex2f(umbrellaX, umbrellaY - 1.10f * s);

        glVertex2f(umbrellaX, umbrellaY);
        glVertex2f(umbrellaX - 0.55f * s, umbrellaY + 0.10f * s);

        glVertex2f(umbrellaX, umbrellaY);
        glVertex2f(umbrellaX + 0.55f * s, umbrellaY + 0.10f * s);
    glEnd();

    // head
    glColor3f(0.95f, 0.70f, 0.48f);
    drawCircle(x, y + 1.02f * s, 0.16f * s);

    // hair
    glColor3f(0.05f, 0.035f, 0.02f);
    drawCircle(x, y + 1.12f * s, 0.12f * s);

    // body
    glColor3f(0.20f, 0.55f, 0.95f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.24f * s, y + 0.82f * s);
        glVertex2f(x + 0.24f * s, y + 0.82f * s);
        glVertex2f(x + 0.19f * s, y + 0.30f * s);
        glVertex2f(x - 0.19f * s, y + 0.30f * s);
    glEnd();

    // hands
    glColor3f(0.95f, 0.70f, 0.48f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
        glVertex2f(x + 0.15f * s, y + 0.70f * s);
        glVertex2f(x + 0.08f * dir * s, y + 1.05f * s);

        glVertex2f(x - 0.15f * s, y + 0.70f * s);
        glVertex2f(x - 0.40f * dir * s, y + 0.48f * s);
    glEnd();

    // legs
    glColor3f(0.05f, 0.12f, 0.40f);
    glLineWidth(5.0f);
    glBegin(GL_LINES);
        glVertex2f(x - 0.08f * s, y + 0.30f * s);
        glVertex2f(x - 0.25f * s, y - 0.25f * s);

        glVertex2f(x + 0.08f * s, y + 0.30f * s);
        glVertex2f(x + 0.25f * s, y - 0.25f * s);
    glEnd();

    // shoes
    glColor3f(0.02f, 0.02f, 0.02f);
    glLineWidth(4.0f);
    glBegin(GL_LINES);
        glVertex2f(x - 0.25f * s, y - 0.25f * s);
        glVertex2f(x - 0.43f * s, y - 0.25f * s);

        glVertex2f(x + 0.25f * s, y - 0.25f * s);
        glVertex2f(x + 0.43f * s, y - 0.25f * s);
    glEnd();
}
void drawSmoke() {
    float f = fireValue();

    if(f <= 0.02f) {
        return;
    }

    float pi = 3.1416f;

    float rise1 = fmod(frameCount * 0.035f, 1.8f);
    float rise2 = fmod(frameCount * 0.030f + 0.6f, 1.8f);
    float rise3 = fmod(frameCount * 0.025f + 1.2f, 1.8f);

    float sway1 = sin(frameCount * 0.08f) * 0.12f;
    float sway2 = sin(frameCount * 0.07f + 2.0f) * 0.15f;
    float sway3 = sin(frameCount * 0.06f + 4.0f) * 0.18f;

    float smokeSize = 1.35f;


    // Smoke from upper left window

    glColor3f(0.55f, 0.55f, 0.55f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 32; i++) {
        float ang = 2.0f * pi * i / 32.0f;
        float r = 0.22f + 0.05f * sin(3.0f * ang) + 0.03f * cos(5.0f * ang);
        glVertex2f((9.50f + sway1) + cos(ang) * r * 1.15f * smokeSize,
                   (8.95f + rise1) + sin(ang) * r * 0.85f * smokeSize);
    }
    glEnd();

    glColor3f(0.65f, 0.65f, 0.65f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 32; i++) {
        float ang = 2.0f * pi * i / 32.0f;
        float r = 0.19f + 0.04f * sin(4.0f * ang) + 0.02f * cos(6.0f * ang);
        glVertex2f((9.35f + sway2) + cos(ang) * r * 1.10f * smokeSize,
                   (9.35f + rise2) + sin(ang) * r * 0.90f * smokeSize);
    }
    glEnd();

    glColor3f(0.75f, 0.75f, 0.75f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 32; i++) {
        float ang = 2.0f * pi * i / 32.0f;
        float r = 0.15f + 0.03f * sin(3.0f * ang) + 0.02f * cos(4.0f * ang);
        glVertex2f((9.60f + sway3) + cos(ang) * r * 1.20f * smokeSize,
                   (9.80f + rise3) + sin(ang) * r * 0.85f * smokeSize);
    }
    glEnd();



    // Smoke from upper right window

    glColor3f(0.52f, 0.52f, 0.52f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 32; i++) {
        float ang = 2.0f * pi * i / 32.0f;
        float r = 0.22f + 0.05f * sin(3.0f * ang) + 0.03f * cos(5.0f * ang);
        glVertex2f((11.50f + sway2) + cos(ang) * r * 1.15f * smokeSize,
                   (8.95f + rise2) + sin(ang) * r * 0.85f * smokeSize);
    }
    glEnd();

    glColor3f(0.62f, 0.62f, 0.62f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 32; i++) {
        float ang = 2.0f * pi * i / 32.0f;
        float r = 0.19f + 0.04f * sin(4.0f * ang) + 0.02f * cos(6.0f * ang);
        glVertex2f((11.35f + sway3) + cos(ang) * r * 1.10f * smokeSize,
                   (9.35f + rise3) + sin(ang) * r * 0.90f * smokeSize);
    }
    glEnd();

    glColor3f(0.72f, 0.72f, 0.72f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 32; i++) {
        float ang = 2.0f * pi * i / 32.0f;
        float r = 0.15f + 0.03f * sin(3.0f * ang) + 0.02f * cos(4.0f * ang);
        glVertex2f((11.60f + sway1) + cos(ang) * r * 1.20f * smokeSize,
                   (9.80f + rise1) + sin(ang) * r * 0.85f * smokeSize);
    }
    glEnd();


    // =====================================================
    // Smoke from middle left window
    // =====================================================
    glColor3f(0.48f, 0.48f, 0.48f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 32; i++) {
        float ang = 2.0f * pi * i / 32.0f;
        float r = 0.20f + 0.04f * sin(3.0f * ang) + 0.025f * cos(5.0f * ang);
        glVertex2f((9.50f + sway3) + cos(ang) * r * 1.15f * smokeSize,
                   (6.95f + rise1) + sin(ang) * r * 0.85f * smokeSize);
    }
    glEnd();

    glColor3f(0.60f, 0.60f, 0.60f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 32; i++) {
        float ang = 2.0f * pi * i / 32.0f;
        float r = 0.16f + 0.03f * sin(4.0f * ang) + 0.02f * cos(6.0f * ang);
        glVertex2f((9.35f + sway1) + cos(ang) * r * 1.10f * smokeSize,
                   (7.35f + rise2) + sin(ang) * r * 0.90f * smokeSize);
    }
    glEnd();


    // =====================================================
    // Smoke from middle right window
    // =====================================================
    glColor3f(0.48f, 0.48f, 0.48f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 32; i++) {
        float ang = 2.0f * pi * i / 32.0f;
        float r = 0.20f + 0.04f * sin(3.0f * ang) + 0.025f * cos(5.0f * ang);
        glVertex2f((11.50f + sway1) + cos(ang) * r * 1.15f * smokeSize,
                   (6.95f + rise2) + sin(ang) * r * 0.85f * smokeSize);
    }
    glEnd();

    glColor3f(0.60f, 0.60f, 0.60f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 32; i++) {
        float ang = 2.0f * pi * i / 32.0f;
        float r = 0.16f + 0.03f * sin(4.0f * ang) + 0.02f * cos(6.0f * ang);
        glVertex2f((11.65f + sway2) + cos(ang) * r * 1.10f * smokeSize,
                   (7.35f + rise3) + sin(ang) * r * 0.90f * smokeSize);
    }
    glEnd();
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
    drawSmoke();
    // moving human from second building door to fire truck
    drawDoorRunnerBehindTruck();
    // human near second building door
    drawSecondBuildingDoorHuman();
    //human help
    drawHelpHumanInWindow6();
    drawTree();
    drawLamp();
    drawPeople();
    drawRedCar();
    drawFlyingPaperAndLeaf();
    drawFireTruck();
    drawFloodWater();
    drawFloodedCar();
    drawRescueBoat();
    drawDebris(5.0f, 1.0f);
    drawDebris(15.0f, 2.0f);
    drawFloatingDebris();
    drawHeavyRain();
    drawFloodWindowPeople();
    //newly
    drawFlyingPaperAndLeaf();
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

        case 'w':
        case 'W':
            if(fireValue() > 0.02f)
                {
                    if(waterOn == 0)
                        {
                            waterOn = 1;
                        }
            else
                        {
                            waterOn = 0;
                        }
                }
            break;
            case 'l':
            case 'L':
            if(fireValue() > 0.02f && waterOn == 1)
                {
                    waterEndX -= 0.2f;
                    if(waterEndX < 8.8f)
                    {
                        waterEndX = 8.8f;
                    }
                }
            break;

            case 'r':
            case 'R':
            if(fireValue() > 0.02f && waterOn == 1)
                {
                    waterEndX += 0.2f;
                    if(waterEndX > 11.6f)
                    {
                        waterEndX = 11.6f;
                    }
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
    // repeated runner from second building door to fire truck
    // repeated runner from second building door to fire truck
if(fireValue() > 0.02f) {
    if(doorRunnerWait > 0)
        {
            doorRunnerWait--;
            doorRunnerProgress = 0.0f;
        }
    else
        {
            doorRunnerProgress += 0.018f;

            if(doorRunnerProgress > 1.0f)
                {
                doorRunnerProgress = 0.0f;
                doorRunnerWait = 67;
                }
        }
    }
    else
        {
        doorRunnerProgress = 0.0f;
        doorRunnerWait = 0;
        }
    if(fireValue() <= 0.02f)
        {
            waterOn = 0;
            waterEndX = 10.2f;
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
