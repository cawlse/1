// stdafx.h: ���������� ���� ��� ����������� ��������� ���������� ������
// ��� ���������� ������ ��� ����������� �������, ������� ����� ������������, ��
// �� ����� ����������
//

#pragma once

#include "targetver.h"

#ifdef _XRLAUNCHER
#define LAUNCHAPI __declspec(dllexport)
#else
#define LAUNCHAPI __declspec(dllimport)
#endif

#define WIN32_LEAN_AND_MEAN             // ��������� ����� ������������ ���������� �� ���������� Windows
// ����� ���������� Windows:
#include <windows.h>



// TODO: ���������� ����� ������ �� �������������� ���������, ����������� ��� ���������
