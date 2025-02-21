// Weapon ID #42. Used by weapons:
// Monster vial 3
#include "weapon_private.h"
#include "shared.h"

// data
extern AnimationFrame D_12A000_8017A604[];
extern AnimationFrame D_12A000_8017A620[];
extern AnimationFrame D_12A000_8017A684[];
extern AnimationFrame D_12A000_8017A6B4[];
extern AnimationFrame D_12A000_8017A6AC[];
extern AnimationFrame D_12A000_8017A6BC[];
extern FrameProperty D_12A000_8017A6DC[];
extern s32 D_12A000_8017A6F4[];
extern AnimationFrame D_12A000_8017A704[];
extern s32 D_12A000_8017A740[];
extern s32 D_12A000_8017A74C[];
extern s32 D_12A000_8017B5F0;

// rodata
extern const char D_12A000_8017A760[]; // "\no\n"
extern s32 D_12A000_8017B5EC;          // g_DebugWaitInfoTimer

void DebugShowWaitInfo(const char* msg) {
    g_CurrentBuffer = g_CurrentBuffer->next;
    FntPrint(msg);
    if (D_12A000_8017B5EC++ & 4) {
        FntPrint(D_12A000_8017A760); // TODO: inline
    }
    DrawSync(0);
    VSync(0);
    PutDrawEnv(&g_CurrentBuffer->draw);
    PutDispEnv(&g_CurrentBuffer->disp);
    FntFlush(-1);
}

void DebugInputWait(const char* msg) {
    while (PadRead(0))
        DebugShowWaitInfo(msg);
    while (!PadRead(0))
        DebugShowWaitInfo(msg);
}

void func_12A000_8017AC08(void) {
    RECT rect;
    RECT rectDummy;
    s16 color;

    color = (g_GameTimer >> 1) % 2 ? 0x039C : 0x199D;
    D_8006EDCC[g_HandId][8] = color;

    rect.x = 0;
    rect.y = 0xF1;
    rect.w = 0x100;
    rect.h = 3;
    LoadImage(&rect, D_8006EDCC);
}

void EntityWeaponAttack(Entity* self) {
    Collider col;
    s32 var_a2;
    s16 xMod;

    switch (self->step) {
    case 0:
        SetSpriteBank1(g_Animset);
        self->animSet = ANIMSET_OVL(16);
        self->palette = 0x110;
        self->unk5A = 0x64;
        if (g_HandId != 0) {
            self->palette += 0x18;
            self->unk5A += 2;
            self->animSet += 2;
        }

        self->zPriority = PLAYER.zPriority + 2;
        self->facingLeft = PLAYER.facingLeft;
        self->flags = FLAG_UNK_08000000 | FLAG_UNK_100000;
        self->unk4C = D_12A000_8017A6B4;
        self->posY.i.hi -= 4;

        D_12A000_8017B5F0 %= 4;
        SetSpeedX(D_12A000_8017A6F4[D_12A000_8017B5F0]);
        self->velocityY = -FIX(2.5);
        g_Player.D_80072F14 = 4;
        D_12A000_8017B5F0++;
        self->step++;
        break;
    case 1:
        self->posX.val += self->velocityX;
        self->posY.val += self->velocityY;
        self->velocityY += FIX(0.15625);
        g_api.CheckCollision(self->posX.i.hi, self->posY.i.hi, &col, 0);
        if (col.effects & EFFECT_SOLID) {
            self->posY.i.hi += col.unk18;
            self->unk4C = D_12A000_8017A684;
            self->animFrameDuration = 0;
            self->animFrameIdx = 0;
            self->blendMode = 0x30;
            g_api.func_80134714(0x619, 0x50, 0);
            g_api.func_8011AAFC(self, ((g_HandId + 1) << 0xC) | 0x38, 0);
            self->step++;
            return;
        }

        if (self->velocityX < 0) {
            xMod = -4;
        } else {
            xMod = 4;
        }
        g_api.CheckCollision(
            (s16)(xMod + self->posX.i.hi), self->posY.i.hi, &col, 0);
        if (col.effects & EFFECT_UNK_0002) {
            if (xMod < 0) {
                self->posX.i.hi += col.unkC;
            } else {
                self->posX.i.hi += col.unk4;
            }
            self->velocityX /= -2;
        }

        g_api.CheckCollision(
            self->posX.i.hi, (s16)(self->posY.i.hi - 8), &col, 0);
        if (col.effects & EFFECT_SOLID) {
            self->posY.i.hi += col.unk20 + 1;
            self->velocityY = FIX(1);
            self->velocityX /= 2;
            return;
        }
        break;
    case 2:
        if (self->animFrameDuration < 0) {
            DestroyEntity(self);
        }
        break;
    }
}

void func_ptr_80170004(Entity* self) {
    s32 unk;

    switch (self->step) {
    case 0:
        self->animSet = self->ext.weapon.parent->animSet;
        self->unk5A = self->ext.weapon.parent->unk5A;
        self->palette = self->ext.weapon.parent->palette;
        self->facingLeft = (self->facingLeft + 1) & 1;
        self->flags = FLAG_UNK_08000000;
        self->zPriority = self->ext.weapon.parent->zPriority - 2;
        self->unk4C = D_12A000_8017A604;
        self->drawFlags |= 3;
        self->rotY = 0;
        self->rotX = 0;
        self->rotPivotY = 0x14;
        self->posY.i.hi -= 0x14;
        self->step++;
        break;
    case 1:
        self->rotX += 4;
        if (self->rotX >= 0x100) {
            self->rotX = 0x100;
            self->unk4C = D_12A000_8017A620;
            self->animFrameIdx = 0;
            self->animFrameDuration = 0;
            self->ext.weapon.equipId =
                self->ext.weapon.parent->ext.weapon.equipId;
            SetWeaponProperties(self, 0);
            self->step++;
        }
        self->rotY = self->rotX;
        break;
    case 2:
        if (self->animFrameDuration == 1) {
            if ((self->animFrameIdx == 6 || self->animFrameIdx == 0xC ||
                 self->animFrameIdx == 0x12)) {
                unk = 0x3E;
                if (g_api.func_8011AAFC(
                        self,
                        ((g_HandId + 1) << 0xC) +
                            (unk + (self->ext.weapon.unk7E << 0x10)),
                        0) != NULL) {
                    self->ext.weapon.unk7E++;
                }
            }
        }
        if (self->animFrameDuration < 0) {
            self->unk4C = D_12A000_8017A6BC;
            self->animFrameIdx = 0;
            self->animFrameDuration = 0;
            self->step++;
        }
        break;
    case 3:
        if (self->rotX == 0x40) {
            g_api.func_8011AAFC(self, ((g_HandId + 1) << 0xC) | 0x46, 0);
        }
        if (self->animFrameIdx != 0) {
            self->rotX -= 4;
        }
        if (self->rotX < 0) {
            DestroyEntity(self);
            return;
        }
        self->rotY = self->rotX;
        break;
    }
    g_api.UpdateAnim(D_12A000_8017A6DC, NULL);
    func_12A000_8017AC08();
}

void func_ptr_80170008(Entity* self) {
    s16 unk;
    s32 modX;

    unk = self->params & 0x7F00;
    unk >>= 8;
    switch (self->step) {
    case 0:
        if (self->ext.weapon.parent->entityId == 0) {
            DestroyEntity(self);
            return;
        }
        self->animSet = self->ext.weapon.parent->animSet;
        self->unk5A = self->ext.weapon.parent->unk5A;
        self->palette = self->ext.weapon.parent->palette;
        self->flags = FLAG_UNK_08000000;
        self->drawFlags = FLAG_DRAW_ROTZ;
        self->zPriority = self->ext.weapon.parent->zPriority - 2;
        self->posY.i.hi -= 0x10;
        if (self->facingLeft == 0) {
            modX = -8;
        } else {
            modX = 8;
        }
        self->posX.i.hi = modX + self->posX.i.hi;

        SetSpeedX(D_12A000_8017A740[unk]);
        self->velocityY = D_12A000_8017A74C[unk];
        self->unk4C = D_12A000_8017A6AC;
        self->ext.weapon.equipId = self->ext.weapon.parent->ext.weapon.equipId;
        SetWeaponProperties(self, 0);
        g_api.PlaySfx(0x628);
        self->step++;
        break;
    case 1:
        self->rotZ -= 0x60;
        self->posX.val += self->velocityX;
        self->posY.val += self->velocityY;
        self->velocityY += FIX(0.15625);
        if (self->hitFlags != 0) {
            self->animSet = 2;
            self->palette = 0x8170;
            self->unk4C = &D_12A000_8017A704;
            self->unk5A = 0;
            self->animFrameDuration = 0;
            self->animFrameIdx = 0;
            self->drawFlags = 0;
            self->velocityY = -FIX(0.5);
            self->step++;
        }
        break;
    case 2:
        self->posY.val += self->velocityY;
        if (self->animFrameDuration < 0) {
            DestroyEntity(self);
            return;
        }
        break;
    }
    g_api.UpdateAnim(D_12A000_8017A6DC, NULL);
}

void func_ptr_8017000C(Entity* self) {
    if (self->step == 0) {
        self->animSet = self->ext.weapon.parent->animSet;
        self->unk5A = self->ext.weapon.parent->unk5A;
        self->palette = self->ext.weapon.parent->palette;
        self->flags = FLAG_UNK_08000000 | FLAG_UNK_100000;
        self->zPriority = self->ext.weapon.parent->zPriority + 2;
        self->unk4C = D_12A000_8017A684;
        self->blendMode = 0x30;
        self->posY.i.hi += 0x10;
        self->step++;
    } else if (self->animFrameDuration < 0) {
        DestroyEntity(self);
    }
}

void func_ptr_80170010(Entity* self) {}

void func_ptr_80170014(Entity* self) {}

int GetWeaponId(void) { return 42; }

void EntityWeaponShieldSpell(Entity* self) {}

void func_ptr_80170024(Entity* self) {}

void func_ptr_80170028(Entity* self) {}

void WeaponUnused2C(void) {}

void WeaponUnused30(void) {}

void WeaponUnused34(void) {}

void WeaponUnused38(void) {}

void WeaponUnused3C(void) {}
