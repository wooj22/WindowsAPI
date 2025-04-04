#pragma once

#include <Windows.h>

namespace Input
{
    void Update();                  // Ű input ���� update
    bool IsKeyDown(int vKey);       // �������ִ� ����
    bool IsKeyPressed(int vKey);    // ���� ����
    bool IsKeyReleased(int vKey);   // ���� ����
}