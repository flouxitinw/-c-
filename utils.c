#include "cainiao.h"


// ��������ʵ��
void pause_program() {
    printf("\n�����������...");
    fflush(stdout);

    // ��ȫ������뻺����
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    // �ȴ�������
    //getchar();
}

void clear_screen() {
    system("cls || clear");
}
// ����������
void clear_input_buffer() {
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

