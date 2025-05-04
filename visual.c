#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include "shared_defs.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#define WIDTH 1280
#define HEIGHT 720


SharedMemory* shm_ptr = NULL;

GLuint bg_texture;

//function to attach to shared memory
void attach_shared_memory() {
    int shmid = shmget(SHM_KEY, sizeof(SharedMemory), 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    shm_ptr = (SharedMemory*) shmat(shmid, NULL, 0);
    if (shm_ptr == (void*)-1) {
        perror("shmat failed");
        exit(1);
    }
}


// Load an image and return the OpenGL texture ID
GLuint loadTexture(const char* filename) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
    if (!data) {
        printf("Failed to load image %s\n", filename);
        exit(1);
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return texture;
}

void drawStreetU() {
    float margin = 210.0f;
    float thickness = HEIGHT / 15.0f;
    float verticalHeight = HEIGHT * 0.6;
    float horizontalY = 140;

    glColor3f(0.59, 0.29, 0.0);  // Brown

    // Bottom horizontal line
    glBegin(GL_QUADS);
        glVertex2f(margin, horizontalY);
        glVertex2f(WIDTH - margin, horizontalY);
        glVertex2f(WIDTH - margin, horizontalY + thickness);
        glVertex2f(margin, horizontalY + thickness);
    glEnd();

    // Left vertical line
    glBegin(GL_QUADS);
        glVertex2f(margin, horizontalY + thickness);
        glVertex2f(margin + thickness, horizontalY + thickness);
        glVertex2f(margin + thickness, horizontalY + thickness + verticalHeight);
        glVertex2f(margin, horizontalY + thickness + verticalHeight);
    glEnd();

    // Right vertical line
    glBegin(GL_QUADS);
        glVertex2f(WIDTH - margin - thickness, horizontalY + thickness);
        glVertex2f(WIDTH - margin, horizontalY + thickness);
        glVertex2f(WIDTH - margin, horizontalY + thickness + verticalHeight);
        glVertex2f(WIDTH - margin - thickness, horizontalY + thickness + verticalHeight);
    glEnd();
}

void drawStreetLinesU() {
    float margin = 210.0f;
    float thickness = HEIGHT / 15.0f;
    float lineLength = 50;
    float gap = 30;
    float horizontalY = 140;
    float verticalHeight = HEIGHT * 0.6;

    glColor3f(1, 1, 1);  // White

    // Bottom horizontal dashed line
    for (float x = margin + 10; x < WIDTH - margin - lineLength; x += lineLength + gap) {
        glBegin(GL_QUADS);
            glVertex2f(x, horizontalY + thickness / 2 - 3);
            glVertex2f(x + lineLength, horizontalY + thickness / 2 - 3);
            glVertex2f(x + lineLength, horizontalY + thickness / 2 + 3);
            glVertex2f(x, horizontalY + thickness / 2 + 3);
        glEnd();
    }

    // Left vertical dashed line
    float leftX = margin + thickness / 2;
    for (float y = horizontalY + thickness + 10; y < horizontalY + thickness + verticalHeight - lineLength; y += lineLength + gap) {
        glBegin(GL_QUADS);
            glVertex2f(leftX - 3, y);
            glVertex2f(leftX + 3, y);
            glVertex2f(leftX + 3, y + lineLength);
            glVertex2f(leftX - 3, y + lineLength);
        glEnd();
    }

    // Right vertical dashed line
    float rightX = WIDTH - margin - thickness / 2;
    for (float y = horizontalY + thickness + 10; y < horizontalY + thickness + verticalHeight - lineLength; y += lineLength + gap) {
        glBegin(GL_QUADS);
            glVertex2f(rightX - 3, y);
            glVertex2f(rightX + 3, y);
            glVertex2f(rightX + 3, y + lineLength);
            glVertex2f(rightX - 3, y + lineLength);
        glEnd();
    }
}

void drawBackground() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, bg_texture);

    glColor4f(1.0, 1.0, 1.0, 1.0);  // RGB = أبيض، Alpha = 10%

    glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex2f(0, 0);
        glTexCoord2f(1, 1); glVertex2f(WIDTH, 0);
        glTexCoord2f(1, 0); glVertex2f(WIDTH, HEIGHT);
        glTexCoord2f(0, 0); glVertex2f(0, HEIGHT);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0, 1.0, 1.0, 0.7);  // 10% أبيض

    glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(WIDTH, 0);
        glVertex2f(WIDTH, HEIGHT);
        glVertex2f(0, HEIGHT);
    glEnd();

    glDisable(GL_BLEND);
}

// ---------------------------------------- Functions to draw the storage station -----------------------------------
void drawStorageStation(float x, float y, float w, float h) {
    // خلفية رمادي فاتح
    glColor3f(0.85, 0.85, 0.85);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();

    // حدود سوداء
    glColor3f(0, 0, 0);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();

    // العنوان
    float textWidth = glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)"STORAGE");
    float centerX = x + (w / 2) - (textWidth / 2);
    glRasterPos2f(centerX, y + h + 10);
    glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)"STORAGE");
}

void drawStorageItem(float x, float y, float w, float h, const char* name, int amount) {
    // مستطيل بني
    glColor3f(0.7, 0.5, 0.3);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();

    // حدود
    glColor3f(0, 0, 0);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();

    // نص: اسم العنصر والعدد
    char label[64];
    sprintf(label, "%s: %d", name, amount);
    glRasterPos2f(x + 10, y + h / 2 - 6);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)label);
}

void drawStorageBlock() {
    float margin = 40;
    float storageW = WIDTH / 3.2;
    float storageH = HEIGHT / 3.2;

    float startX = margin;
    float startY = HEIGHT - margin - storageH;

    drawStorageStation(startX, startY, storageW, storageH);

    const char* names[8] = {
        "Wheat", "Yeast", "Butter", "Milk",
        "Sugar/Salt", "SweetItems", "Cheese", "Salami"
    };

    int values[8] = {
        shm_ptr->store_data.wheat,
        shm_ptr->store_data.yeast,
        shm_ptr->store_data.butter,
        shm_ptr->store_data.milk,
        shm_ptr->store_data.sugar_salt,
        shm_ptr->store_data.sweet_items,
        shm_ptr->store_data.cheese,
        shm_ptr->store_data.salami
    };
    

    float boxW = (storageW - 50) / 2.0;
    float boxH = (storageH - 60) / 4.0;
    float offsetX = startX + 15;
    float offsetY = startY + storageH - boxH - 20;

    for (int i = 0; i < 8; i++) {
        float x = offsetX + (i % 2) * (boxW + 10);
        float y = offsetY - (i / 2) * (boxH + 10);
        drawStorageItem(x, y, boxW, boxH, names[i], values[i]);
    }
}

// ---------------------------------------- Functions to draw the kitchen block -----------------------------------
void drawCircle(float cx, float cy, float r, int segments) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; ++i) {
        float theta = 2.0f * 3.1415926f * i / segments;
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

void drawEllipse(float cx, float cy, float rx, float ry, int segments) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; ++i) {
        float theta = 2.0f * 3.1415926f * i / segments;
        float x = rx * cosf(theta);
        float y = ry * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

void drawKitchenCell(float x, float y, float w, float h, const char* teamName) {
    // اسم الفريق
    glColor3f(0, 0, 0);
    glRasterPos2f(x + 5, y + h - 20);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)teamName);

    // الطباخ (مستطيل + طاقية)
    glColor3f(1.0, 0.9, 0.6);  // لون بشرة
    glBegin(GL_QUADS);  // جسم الطباخ
        glVertex2f(x + 10, y + 10);
        glVertex2f(x + 30, y + 10);
        glVertex2f(x + 30, y + 40);
        glVertex2f(x + 10, y + 40);
    glEnd();

    glColor3f(1, 1, 1); // طاقية
    drawCircle(x + 20, y + 45, 8, 20);

    // طاولة (دائرتين)
    glColor3f(0.5, 0.3, 0.1); // الطاولة
    drawCircle(x + w - 30, y + 25, 15, 20);
    glColor3f(0.3, 0.2, 0.1); // القاعدة
    drawCircle(x + w - 30, y + 15, 7, 20);
}

void drawKitchenBlock() {
    float margin = 40;
    float kitchenW = WIDTH / 2.5;
    float kitchenH = HEIGHT / 2.5;

    float startX = margin;
    float startY = margin;

    const char* teamNames[6] = {
        "Paste Team",
        "Cakes Team",
        "Sandwiches Team",
        "Sweets Team",
        "Sweet Patisseries Team",
        "Savory Patisseries Team"
    };

    int producedCounts[6] = {
        shm_ptr->kitchen_data.paste_ready,
        shm_ptr->kitchen_data.cakes_ready,
        shm_ptr->kitchen_data.sandwiches_ready,
        shm_ptr->kitchen_data.sweets_ready,
        shm_ptr->kitchen_data.sweet_patisseries_ready,
        shm_ptr->kitchen_data.savory_patisseries_ready
    };
    

    // خلفية المطبخ
    glColor3f(0.75, 0.85, 0.95);
    glBegin(GL_QUADS);
        glVertex2f(startX, startY);
        glVertex2f(startX + kitchenW, startY);
        glVertex2f(startX + kitchenW, startY + kitchenH);
        glVertex2f(startX, startY + kitchenH);
    glEnd();

    // حدود
    glColor3f(0, 0, 0);
    glBegin(GL_LINE_LOOP);
        glVertex2f(startX, startY);
        glVertex2f(startX + kitchenW, startY);
        glVertex2f(startX + kitchenW, startY + kitchenH);
        glVertex2f(startX, startY + kitchenH);
    glEnd();

    // عنوان "Chefs"
    const char* title = "Chefs";
    float titleWidth = glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)title);
    float centerX = startX + (kitchenW / 2) - (titleWidth / 2);
    glRasterPos2f(centerX, startY + kitchenH + 10);
    glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)title);

    float cellW = (kitchenW - 30) / 2.0;
    float cellH = (kitchenH - 30) / 3.0;
    float offsetX = startX + 10;
    float offsetY = startY + kitchenH - cellH - 10;

    for (int i = 0; i < 6; i++) {
        int col = i % 2;
        int row = i / 2;

        float x = offsetX + col * (cellW + 10);
        float y = offsetY - row * (cellH + 10);

        // اسم الفريق
        const char* label = teamNames[i];
        int labelWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)label);
        float labelX = x + (cellW / 2) - (labelWidth / 2);
        glColor3f(0, 0, 0);
        glRasterPos2f(labelX, y + cellH - 18);
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)label);

        // الطباخ
        float px = x + 60;
        float py = y + cellH - 60;
        float bodyW = 16;
        float bodyH = 35;

        // طاقية
        glColor3f(1, 1, 1);
        drawCircle(px, py + bodyH / 2 + 18, 6, 20);

        // رأس
        glColor3f(1.0, 0.85, 0.6);
        drawCircle(px, py + bodyH / 2 + 10, 8, 20);

        // جسم
        glColor3f(0.3, 0.3, 0.3);
        glBegin(GL_QUADS);
            glVertex2f(px - bodyW / 2, py + bodyH / 2);
            glVertex2f(px + bodyW / 2, py + bodyH / 2);
            glVertex2f(px + bodyW / 2, py - bodyH / 2);
            glVertex2f(px - bodyW / 2, py - bodyH / 2);
        glEnd();

        // يدين على الجوانب
        glColor3f(0.2, 0.2, 0.2);
        glBegin(GL_QUADS);
            glVertex2f(px - bodyW / 2 - 5, py + bodyH / 4);
            glVertex2f(px - bodyW / 2, py + bodyH / 4);
            glVertex2f(px - bodyW / 2, py - bodyH / 4);
            glVertex2f(px - bodyW / 2 - 5, py - bodyH / 4);

            glVertex2f(px + bodyW / 2, py + bodyH / 4);
            glVertex2f(px + bodyW / 2 + 5, py + bodyH / 4);
            glVertex2f(px + bodyW / 2 + 5, py - bodyH / 4);
            glVertex2f(px + bodyW / 2, py - bodyH / 4);
        glEnd();

        // الطاولة
        float tx = x + cellW - 140;
        float ty = y + 35;
        float tw = 120;
        float th = 20;

        // القاعدة
        float baseW = tw * 0.3;
        float baseH = 5;
        float bx = tx + (tw - baseW) / 2;
        float by = ty - 30;

        glColor3f(0.3, 0.2, 0.1);
        glBegin(GL_QUADS);
            glVertex2f(bx, by);
            glVertex2f(bx + baseW, by);
            glVertex2f(bx + baseW, by + baseH);
            glVertex2f(bx, by + baseH);
        glEnd();

        float columnHeight = ty - (by + baseH / 2);
        float newColumnHeight = columnHeight * (2.0 / 3.0);
        ty = by + baseH / 2 + newColumnHeight;

        // سطح الطاولة
        glColor3f(0.5, 0.3, 0.1);
        glBegin(GL_QUADS);
            glVertex2f(tx, ty);
            glVertex2f(tx + tw, ty);
            glVertex2f(tx + tw, ty + th);
            glVertex2f(tx, ty + th);
        glEnd();

        // العمود
        glColor3f(0.2, 0.2, 0.2);
        glBegin(GL_LINES);
            glVertex2f(tx + tw / 2, ty);
            glVertex2f(tx + tw / 2, by + baseH / 2);
        glEnd();

        // ✅ صندوق فوق الطاولة فيه عدد المنتجات
        float boxW = 40, boxH = 20;
        float boxX = tx + (tw - boxW) / 2;
        float boxY = ty + th - 10;

        glColor3f(0.9, 0.9, 0.9);  // رمادي فاتح
        glBegin(GL_QUADS);
            glVertex2f(boxX, boxY);
            glVertex2f(boxX + boxW, boxY);
            glVertex2f(boxX + boxW, boxY + boxH);
            glVertex2f(boxX, boxY + boxH);
        glEnd();

        glColor3f(0, 0, 0);
        char countStr[8];
        sprintf(countStr, "%d", producedCounts[i]);
        glRasterPos2f(boxX + 10, boxY + 4);
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)countStr);
    }
}

void drawOvenSection() {
    float margin = 40;
    float sectionW = WIDTH / 2.5;
    float sectionH = HEIGHT / 3;

    float startX = WIDTH - margin - sectionW;
    float startY = margin;

    const char* teamNames[3] = {
        "Bake Cakes/Sweets",
        "Bake Patisseries",
        "Bake Bread"
    };

    int producedCounts[3] = {
        shm_ptr->oven_data.cakes_in_oven + shm_ptr->oven_data.sweets_in_oven,
        shm_ptr->oven_data.sweet_patisseries_in_oven + shm_ptr->oven_data.savory_patisseries_in_oven,
        shm_ptr->oven_data.bread_in_oven
    };
    
    // خلفية القسم
    glColor3f(0.2, 0.2, 0.25);  // رمادي غامق
    glBegin(GL_QUADS);
        glVertex2f(startX, startY);
        glVertex2f(startX + sectionW, startY);
        glVertex2f(startX + sectionW, startY + sectionH);
        glVertex2f(startX, startY + sectionH);
    glEnd();

    // حدود
    glColor3f(0, 0, 0);
    glBegin(GL_LINE_LOOP);
        glVertex2f(startX, startY);
        glVertex2f(startX + sectionW, startY);
        glVertex2f(startX + sectionW, startY + sectionH);
        glVertex2f(startX, startY + sectionH);
    glEnd();

    // العنوان
    const char* title = "Bakers";
    float titleWidth = glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)title);
    float centerX = startX + (sectionW / 2) - (titleWidth / 2);
    glRasterPos2f(centerX, startY + sectionH + 10);
    glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)title);

    // تقسيم أفقي: 3 خلايا (أعمدة)
    float cellW = (sectionW - 40) / 3.0;
    float cellH = sectionH - 20;
    float offsetX = startX + 10;
    float offsetY = startY + 10;

    for (int i = 0; i < 3; i++) {
        float x = offsetX + i * (cellW + 10);
        float y = offsetY;

        // اسم الفريق
        const char* label = teamNames[i];
        int labelWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)label);
        float labelX = x + (cellW / 2) - (labelWidth / 2);
        glColor3f(1, 1, 1);
        glRasterPos2f(labelX, y + cellH - 18);
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)label);

        // الفرن
        float ovenW = 120;
        float ovenH = 80;
        float ovenX = x + cellW / 2 - ovenW / 2;
        float ovenY = y + 20;

        // المستطيل الرئيسي
        glColor3f(0.3, 0.3, 0.3);
        glBegin(GL_QUADS);
            glVertex2f(ovenX, ovenY);
            glVertex2f(ovenX + ovenW, ovenY);
            glVertex2f(ovenX + ovenW, ovenY + ovenH);
            glVertex2f(ovenX, ovenY + ovenH);
        glEnd();

        // الزجاج
        float glassMargin = 10;
        glColor3f(0.5, 0.7, 0.8);
        glBegin(GL_QUADS);
            glVertex2f(ovenX + glassMargin, ovenY + glassMargin);
            glVertex2f(ovenX + ovenW - glassMargin, ovenY + glassMargin);
            glVertex2f(ovenX + ovenW - glassMargin, ovenY + ovenH - glassMargin);
            glVertex2f(ovenX + glassMargin, ovenY + ovenH - glassMargin);
        glEnd();

        // رقم الإنتاج داخل الزجاج
        char numStr[8];
        sprintf(numStr, "%d", producedCounts[i]);
        glColor3f(0, 0, 0);
        glRasterPos2f(ovenX + ovenW / 2 - 5, ovenY + ovenH / 2 - 4);
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)numStr);

        // الدوائر داخل الفرن (فوق الزجاج)
        float cx = ovenX + 35;
        float cy = ovenY + ovenH - 10;
        for (int c = 0; c < 3; c++) {
            glColor3f(1, 0, 0);  // إشارة حمراء
            drawCircle(cx + c * 25, cy, 6, 20);
        }

        // الخباز - فوق الفرن مباشرة
        float px = ovenX + ovenW / 2;
        float py = ovenY + ovenH + 30;
        float bodyW = 16;
        float bodyH = 35;

        // طاقية
        glColor3f(1.0, 1.0, 0.2); 
        drawCircle(px, py + bodyH / 2 + 18, 6, 20);

        // رأس
        glColor3f(1.0, 0.85, 0.6);
        drawCircle(px, py + bodyH / 2 + 10, 8, 20);

        // جسم
        glColor3f(0.2, 0.4, 0.5);
        glBegin(GL_QUADS);
            glVertex2f(px - bodyW / 2, py + bodyH / 2);
            glVertex2f(px + bodyW / 2, py + bodyH / 2);
            glVertex2f(px + bodyW / 2, py - bodyH / 2);
            glVertex2f(px - bodyW / 2, py - bodyH / 2);
        glEnd();

        // يدين
        glColor3f(0.1, 0.2, 0.2);
        glBegin(GL_QUADS);
            // يسار
            glVertex2f(px - bodyW / 2 - 5, py + bodyH / 4);
            glVertex2f(px - bodyW / 2, py + bodyH / 4);
            glVertex2f(px - bodyW / 2, py - bodyH / 4);
            glVertex2f(px - bodyW / 2 - 5, py - bodyH / 4);

            // يمين
            glVertex2f(px + bodyW / 2, py + bodyH / 4);
            glVertex2f(px + bodyW / 2 + 5, py + bodyH / 4);
            glVertex2f(px + bodyW / 2 + 5, py - bodyH / 4);
            glVertex2f(px + bodyW / 2, py - bodyH / 4);
        glEnd();

    }
}

void drawSellerSection() {
    float margin = 40;
    float sectionW = WIDTH / 2.5;
    float sectionH = HEIGHT / 2.7;

    float startX = WIDTH - margin - sectionW;
    float startY = HEIGHT - margin - sectionH;

    // خلفية القسم
    glColor3f(0.9, 0.95, 0.95);
    glBegin(GL_QUADS);
        glVertex2f(startX, startY);
        glVertex2f(startX + sectionW, startY);
        glVertex2f(startX + sectionW, startY + sectionH);
        glVertex2f(startX, startY + sectionH);
    glEnd();

    // حدود
    glColor3f(0, 0, 0);
    glBegin(GL_LINE_LOOP);
        glVertex2f(startX, startY);
        glVertex2f(startX + sectionW, startY);
        glVertex2f(startX + sectionW, startY + sectionH);
        glVertex2f(startX, startY + sectionH);
    glEnd();

    // العنوان "Seller"
    const char* title = "Seller";
    float titleWidth = glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)title);
    float centerX = startX + (sectionW / 2) - (titleWidth / 2);
    glRasterPos2f(centerX, startY + sectionH + 10);
    glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)title);

    // حساب منتصف الارتفاع لتحريك البائع والطاولة
    float centerY = startY + sectionH / 2;
    float tableW = sectionW / 2.8;
    float tableH = 25;
    float tableX = startX + 30;
    float tableY = centerY - tableH / 2.2;

    // البائع
    float px = tableX + tableW / 2;
    float py = tableY + tableH - 10;
    float bodyW = 16 * 3;
    float bodyH = 35 * 3;

    // طاقية
    glColor3f(1.0, 0.0, 0.0);
    drawCircle(px, py + bodyH / 2 + 24, 10, 24);

    // رأس
    glColor3f(1.0, 0.85, 0.6);
    drawCircle(px, py + bodyH / 2 + 15, 12, 24);

    // جسم
    glColor3f(0.2, 0.2, 0.3);
    glBegin(GL_QUADS);
        glVertex2f(px - bodyW / 2, py + bodyH / 2);
        glVertex2f(px + bodyW / 2, py + bodyH / 2);
        glVertex2f(px + bodyW / 2, py - bodyH / 2);
        glVertex2f(px - bodyW / 2, py - bodyH / 2);
    glEnd();

    // يدين
    glColor3f(0.1, 0.1, 0.2);
    glBegin(GL_QUADS);
        // يسار
        glVertex2f(px - bodyW / 2 - 10, py + bodyH / 4);
        glVertex2f(px - bodyW / 2, py + bodyH / 4);
        glVertex2f(px - bodyW / 2, py - bodyH / 4);
        glVertex2f(px - bodyW / 2 - 10, py - bodyH / 4);

        // يمين
        glVertex2f(px + bodyW / 2, py + bodyH / 4);
        glVertex2f(px + bodyW / 2 + 10, py + bodyH / 4);
        glVertex2f(px + bodyW / 2 + 10, py - bodyH / 4);
        glVertex2f(px + bodyW / 2, py - bodyH / 4);
    glEnd();

    // 🔶 رسم الطاولة أولاً
    glColor3f(0.4, 0.2, 0.0);
    glBegin(GL_QUADS);
        glVertex2f(tableX, tableY - 50);
        glVertex2f(tableX + tableW, tableY - 50);
        glVertex2f(tableX + tableW, tableY + tableH);
        glVertex2f(tableX, tableY + tableH);
    glEnd();

    // 💰 عرض الدخل الحالي فوق الطاولة
    char salesText[32];
    sprintf(salesText, "Sales: %d JD", shm_ptr->sales_data.daily_sales_dinar);
    glColor3f(0, 0, 0);  // أسود
    glRasterPos2f(tableX + 40, tableY + tableH - 40);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)salesText);

    // لوحة الطلبات الجانبية
    float boardX = startX + sectionW / 2 + 10; 
    float boardY = startY + 20;
    float boardW = sectionW / 2 - 30;
    float boardH = sectionH - 40;

    glColor3f(0.7, 0.6, 0.4);
    glBegin(GL_QUADS);
        glVertex2f(boardX, boardY);
        glVertex2f(boardX + boardW, boardY);
        glVertex2f(boardX + boardW, boardY + boardH);
        glVertex2f(boardX, boardY + boardH);
    glEnd();

    // عرض الطلبات كبطاقات (كحد أقصى 9)
    int max_orders = shm_ptr->sales_data.order_count;
    if (max_orders > 9) max_orders = 9;

    float boxW = 70;
    float boxH = 50;
    float spacing = 10;

    for (int i = 0; i < max_orders; i++) {
        Order o = shm_ptr->sales_data.current_orders[i];

        float cardX = boardX + (i % 3) * (boxW + spacing);
        float cardY = boardY + boardH - (i / 3 + 1) * (boxH + spacing);

        // بطاقة الطلب
        glColor3f(0.95, 0.9, 0.6);  // لون البطاقة
        glBegin(GL_QUADS);
            glVertex2f(cardX, cardY);
            glVertex2f(cardX + boxW, cardY);
            glVertex2f(cardX + boxW, cardY + boxH);
            glVertex2f(cardX, cardY + boxH);
        glEnd();

        // نقطة صغيرة للتزيين
        glColor3f(1.0, 0.0, 0.0);
        drawCircle(cardX + 5, cardY + boxH - 5, 3, 12);

        // النص
        char line1[32], line2[32];
        sprintf(line1, "#%d", o.id);
        sprintf(line2, "Cake:%d Bread:%d", o.cake_qty, o.bread_qty);

        glColor3f(0, 0, 0);
        glRasterPos2f(cardX + 10, cardY + boxH - 15);
        glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)line1);

        glRasterPos2f(cardX + 10, cardY + boxH - 30);
        glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)line2);
    }
}






void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    drawBackground();
    drawStreetU();
    drawStreetLinesU();

    drawStorageBlock();
    drawKitchenBlock();
    drawOvenSection();
    drawSellerSection();

    glutSwapBuffers();
}

void update(int val) {
    glutPostRedisplay();
    glutTimerFunc(1000, update, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Bakery Visual");
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);

    glMatrixMode(GL_MODELVIEW); // 🔴 ضروري للـ glLoadIdentity داخل display
    glLoadIdentity();

    //bg_texture = loadTexture("background.png"); 
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glutDisplayFunc(display);
    glutTimerFunc(1000, update, 0);
    attach_shared_memory();
    glutMainLoop();

    return 0;
}
