#include "Input.h"

namespace Input
{
    SHORT prevState[256] = { 0 };
    SHORT currState[256] = { 0 };

    // Ű input ���� update
    void Update() {
        memcpy_s(prevState, sizeof(prevState), currState, sizeof(currState));
        for (int i = 0; i < 256; i++) {
            currState[i] = GetAsyncKeyState(i);
        }
    }

    // �������ִ� ����
    bool IsKeyDown(int vKey) {
        return (currState[vKey] & 0x8000) != 0;
    }

    // ���� ����
    bool IsKeyPressed(int vKey) {
        return (!(prevState[vKey] & 0x8000) && (currState[vKey] & 0x8000));
    }

    // ���� ����
    bool IsKeyReleased(int vKey) {
        return ((prevState[vKey] & 0x8000) && !(currState[vKey] & 0x8000));
    }
}