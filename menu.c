#include "cainiao.h"

// ���˵�
void main_menu() {
    while (1) {
        clear_screen();
        printf("\n========== ������վ����ϵͳ ==========\n");
        printf("1. �û�ע��\n");
        printf("2. �û���¼\n");
        printf("3. ����Ա��¼\n");
        printf("0. �˳�ϵͳ\n");
        printf("��ѡ�����: ");

        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1:
            user_register();
            break;
        case 2:
            user_login();
            break;
        case 3:
            admin_login();
            break;
        case 0:
            printf("\n��лʹ�ò�����վ����ϵͳ���ټ���\n");
            exit(0);
        default:
            printf("\n��Ч��ѡ�����������룡\n");
            pause_program();
            clear_input_buffer();
        }
    }
}