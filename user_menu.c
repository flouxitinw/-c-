#include "cainiao.h"

// �û��˵�
void user_menu(User* user) {
    while (1) {
        clear_screen();

        // ������� - �̶����40�ַ�
        printf("\n�X�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�[\n");
        printf("�U              �û�����                  �U\n");
        printf("�^�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�a\n\n");

        // �û���ӭ��Ϣ - �̶���ȶ���
        printf(" ��ӭ������%-15s", user->username);
        switch (user->level) {
        case USER_NORMAL:    printf("(��ͨ�û�)\n"); break;
        case USER_STUDENT:   printf("(ѧ���û�)\n"); break;
        case USER_VIP:      printf("(VIP�û�)\n"); break;
        case USER_ENTERPRISE: printf("(��ҵ�û�)\n"); break;
        case USER_PARTNER:  printf("(�����̻�)\n"); break;
        }
        printf(" �ֻ���: %-15s\n\n", user->phone);
        int overdue_count = 0;
        int fresh_count = 0;
        int mispicked_count = 0;
        ExpressNode* curr = pickup_list;

        // ͳ������
        while (curr) {
            if (strcmp(curr->data.receiver_phone, user->phone) == 0) {
                if (curr->data.status == STATUS_OVERDUE) overdue_count++;
                if (curr->data.package_type == FRESH && curr->data.status == STATUS_UNPICKED) fresh_count++;
            }
            if (strcmp(curr->data.receiver_phone, user->phone) != 0 &&
                curr->data.status == STATUS_PICKED && strcmp(curr->data.pickup_phone, user->phone) == 0) mispicked_count++;
            curr = curr->next;
        }

        // ��ʾ���� - ���п���ͳһΪ40�ַ�
        if (overdue_count > 0 || fresh_count > 0 || mispicked_count > 0) {
            printf("�X�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�[\n");
            printf("�U              ��Ҫ����                  �U\n");
            printf("�^�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�a\n");

            // ��ʾ������������
            if (overdue_count > 0) {
                printf("\n ���� %-2d ������������Ҫ����ȡ�ߣ�\n", overdue_count);
                curr = pickup_list;
                while (curr) {
                    if (strcmp(curr->data.receiver_phone, user->phone) == 0 &&
                        curr->data.status == STATUS_OVERDUE) {

                        printf("������������������������������������������������������������������������������������\n");
                        printf("�� ��ݵ���: %-29s��\n", curr->data.tracking_num);
                        printf("�� �ļ���: %-14s %-15s ��\n", curr->data.sender_name, curr->data.sender_phone);
                        printf("�� ��������: %-28s ��\n", get_package_type_name(curr->data.package_type));

                        // ������������
                        double days = difftime(time(NULL), curr->data.storage_time) / 86400;
                        printf("�� ������: %-6.1f��                       ��\n", days);
                        printf("������������������������������������������������������������������������������������\n");
                    }
                    curr = curr->next;
                }
            }

            // ��ʾ���ʰ�������
            if (fresh_count > 0) {
                printf("\n ���� %-2d �����ʰ�����Ҫ��ʱȡ�ߣ�\n", fresh_count);
                curr = pickup_list;
                while (curr) {
                    if (strcmp(curr->data.receiver_phone, user->phone) == 0 &&
                        curr->data.package_type == FRESH &&
                        curr->data.status == STATUS_UNPICKED) {

                        printf("������������������������������������������������������������������������������������\n");
                        printf("�� ��ݵ���: %-29s��\n", curr->data.tracking_num);
                        printf("�� �ļ���: %-14s %-15s ��\n", curr->data.sender_name, curr->data.sender_phone);
                        printf("�� ��������: %-28s ��\n", get_package_type_name(curr->data.package_type));

                        // �������ʱ��
                        char time_str[20];
                        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M",
                            localtime(&curr->data.storage_time));
                        printf("�� ���ʱ��: %-28s ��\n", time_str);
                        printf("������������������������������������������������������������������������������������\n");
                    }
                    curr = curr->next;
                }
            }

            if (overdue_count > 0 || fresh_count > 0) {
                printf("\n�� �뾡��ǰ����ݵ�ȡ����\n");
            }
        }

        int unsent_count = 0;
        int sent_count = 0;
        ExpressNode* current = send_list;

        // ͳ������
        while (current) {
            if (strcmp(current->data.sender_phone, user->phone) == 0) {
                if (current->data.status == STATUS_UNSENT) {
                    unsent_count++;
                }
                else if (current->data.status == STATUS_SENT) {
                    sent_count++;
                }
            }
            current = current->next;
        }

        // ��ʾ�ļ����� - ͳһ���40�ַ�
        if (unsent_count > 0 || sent_count > 0) {
            printf("\n�X�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�[\n");
            printf("�U              �ļ�����                  �U\n");
            printf("�^�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�a\n");

            if (unsent_count > 0) {
                printf("\n ���� %-2d �����ĳ�����\n", unsent_count);
            }

            if (sent_count > 0) {
                current = send_list;
                printf("\n ���°����Ѽĳ���\n");
                while (current) {
                    if (strcmp(current->data.sender_phone, user->phone) == 0 &&
                        current->data.status == STATUS_SENT) {

                        char time_str[20];
                        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M",
                            localtime(&current->data.create_time));

                        printf("������������������������������������������������������������������������������������\n");
                        printf("�� ��ݵ���: %-28s ��\n", current->data.tracking_num);
                        printf("�� ����: %-14s %-17s ��\n", current->data.receiver_name, current->data.receiver_phone);
                        printf("�� �ĳ�ʱ��: %-28s ��\n", time_str);
                        printf("������������������������������������������������������������������������������������\n");
                    }
                    current = current->next;
                }
            }
        }

        // ��ʾð������ - ͳһ���40�ַ�
        if (mispicked_count > 0) {
            printf("\n�X�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�[\n");
            printf("�U              ð������                  �U\n");
            printf("�^�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�a\n");

            printf("\n ���� %-2d ���������ð�죡\n", mispicked_count);
            curr = pickup_list;
            while (curr) {
                if (strcmp(curr->data.receiver_phone, user->phone) != 0 &&
                    curr->data.status == STATUS_PICKED && strcmp(curr->data.pickup_phone, user->phone) == 0) {

                    printf("������������������������������������������������������������������������������������\n");
                    printf("�� ��ݵ���: %-28s ��\n", curr->data.tracking_num);
                    printf("�� �ļ���: %-14s %-15s ��\n", curr->data.sender_name, curr->data.sender_phone);
                    printf("�� ��������: %-28s ��\n", get_package_type_name(curr->data.package_type));
                    printf("������������������������������������������������������������������������������������\n");
                }
                curr = curr->next;
            }
            printf("\n ��ȷ��ð�죬��������ѡ�\n");
        }

        // �˵�ѡ�� - ͳһ���40�ַ�
        printf("\n�X�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�[\n");
        printf("�U              ���ܲ˵�                  �U\n");
        printf("�d�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�g\n");
        printf("�U 1. �鿴�ҵ���Ϣ                        �U\n");
        printf("�U 2. �޸ĸ�����Ϣ                        �U\n");
        printf("�U 3. ��ݷ���                            �U\n");
        printf("�U 4. ע���Լ�                            �U\n");
        printf("�U 0. �˳���¼                            �U\n");
        printf("�^�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�a\n");
        printf("��ѡ�����: ");

        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1: // �鿴�ҵ���Ϣ
            view_my_info(user);
            break;
        case 2://�޸ĸ�����Ϣ
            modify_user_info(user);
            break;
        case 3://��ݷ���
            express_service_menu(user);
            break;
        case 4: { // �û�ע��
            if (user_unregister(&user_list, &user_count, &user)) {
                return; // ���ע���ɹ����������˵�
            }
            break;
        }
        case 0:
            return;
        default:
            printf("\n ��Ч��ѡ�����������룡\n");
            pause_program();
            clear_input_buffer();
        }
    }
}
// Ͷ�߲˵� - ͳһ���40�ַ�
void express_complaint_menu(User* user) {
    while (1) {
        clear_screen();
        printf("\n�X�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�[\n");
        printf("�U              Ͷ�߷���                  �U\n");
        printf("�d�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�g\n");
        printf("�U 1. �ύͶ��                            �U\n");
        printf("�U 2. ��ѯͶ��                            �U\n");
        printf("�U 0. �����ϼ��˵�                        �U\n");
        printf("�^�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�a\n");
        printf("��ѡ�����: ");

        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1:
            submit_complaint(user);
            break;
        case 2:
            view_user_complaints(user);
            break;
        case 0:
            return;
        default:
            printf("\n��Ч��ѡ�����������룡\n");
            pause_program();
            clear_input_buffer();
        }
    }
}


// ��ݷ���˵� - ͳһ���40�ַ�
void express_service_menu(User* user) {
    while (1) {
        clear_screen();
        printf("\n�X�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�[\n");
        printf("�U              ��ݷ���                  �U\n");
        printf("�d�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�g\n");
        printf("�U 1. �Ŀ��                              �U\n");
        printf("�U 2. ȡ���                              �U\n");
        printf("�U 3. Ͷ��                                �U\n");
        printf("�U 4. ����                                �U\n");
        printf("�U 0. �����ϼ��˵�                        �U\n");
        printf("�^�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�a\n");
        printf("��ѡ�����: ");

        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1:
            send_express_menu(user);
            break;
        case 2:
            pickup_express(user);
            break;
        case 3:
            express_complaint_menu(user);
            break;
        case 4:
            reject_package_menu(user);
            break;
        case 0:
            return;
        default:
            printf("\n��Ч��ѡ�����������룡\n");
            pause_program();
            clear_input_buffer();
        }
    }
}