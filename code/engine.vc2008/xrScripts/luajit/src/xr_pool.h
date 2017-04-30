// Author	: Abramcumner
// data		: 04.02.2017

#ifndef _XR_POOL_H
#define _XR_POOL_H

// � �������� x64 ���� � luajit`�� - ��-�� ����, ��� ������� ���������� ��������� ������ �� ������� �������,
// �� ������� �������� ������� �� ����� �������� ������, ��� ��� ��� ��� ������ ��� ������ ������� ����.
//��� ������� ����� ��� ������ ���� ���� �� 256�� � ������� ������ luajit`� �� ���.


void XR_INIT();
void* XR_MMAP(size_t size);
void XR_DESTROY(void* p, size_t size);

#endif