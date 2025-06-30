#include "cainiao.h"

// ����Ա�˵�
void admin_menu(Admin* admin) {
    int choice;
    bool is_super = is_super_admin(admin);

    do {
        clear_screen();
        printf("\n======================= ����Ա������� =======================\n");

        // ͳ�Ƹ���״̬������
        int overdue_count = 0;
        int unprocessed_complaints = 0;
        int mispicked_count = 0;
        int unsent_count = 0;
        int rejected_count = 0;

        // ͳ������������pickup_list��
        ExpressNode* curr = pickup_list;
        while (curr) {
            // ������������
            double days = difftime(time(NULL), curr->data.storage_time) / 86400;
            if (curr->data.status == STATUS_OVERDUE && days > 7) {
                overdue_count++;
                printf("����:%-6s ����:%-12s �ռ���:%-10s ������(%.1f��)\n",
                    curr->data.shelf_id,
                    curr->data.tracking_num,
                    curr->data.receiver_name,
                    days);
            }
            curr = curr->next;
        }

        // ͳ��Ͷ�ߺ;��գ�pickup_list��
        curr = pickup_list; // ����ָ��


        while (curr) {
            // ð���
            if (curr->data.status == STATUS_MISPICKED) {
                mispicked_count++;
            }
            // ��������ռ�
            else if (curr->data.status == STATUS_REJECTED) {
                rejected_count++;
            }
            curr = curr->next;
        }

        // ͳ�ƴ��ĳ���ݣ�send_list��
        curr = send_list;
        while (curr) {
            // δ����Ͷ��
            if (curr->data.status == STATUS_LOST || curr->data.status == STATUS_DAMAGED) {
                unprocessed_complaints++;
            }
            if (curr->data.status == STATUS_UNSENT) {
                unsent_count++;
            }
            curr = curr->next;
        }

        // ��ʾ�������
        if (overdue_count > 0 || unprocessed_complaints > 0 ||
            mispicked_count > 0 || unsent_count > 0 || rejected_count > 0) {
            printf("\n==================== ϵͳ���� ====================\n");

            if (overdue_count > 0) {
                printf("[����] �� %d ��������������7�죡��绰��ϵ�ͻ�����\n", overdue_count);
            }

            if (unprocessed_complaints > 0) {
                printf("[��Ҫ] �� %d ��Ͷ��δ�����뾡�촦��ͻ�Ͷ�ߣ�\n", unprocessed_complaints);
            }

            if (mispicked_count > 0) {
                printf("[ע��] �� %d ���������Ʊ�ð�죡�뾡�촦��\n", mispicked_count);
            }

            if (unsent_count > 0) {
                printf("[����] �� %d �����ĳ��������뾡�찲�żĳ���\n", unsent_count);
            }

            if (rejected_count > 0) {
                printf("[����] �� %d �����հ����������뾡�촦��\n", rejected_count);
            }

            printf("==================================================\n");
        }
        else {
            printf("\n��ǰû����Ҫ������������\n");
        }
        printf("==================================================\n");



        printf("\n======= ����Ա�˵� =======\n");
        printf("��ǰ���: %s %s\n",
            is_super ? "[��������Ա]" : "[��ͨ����Ա]",
            admin->username);

        printf("1. �û�����\n");
        printf("2. ��ݹ���\n");
        printf("3. ���ܹ���\n");
        if (is_super) {
            printf("4. ����Ա����\n");
        }
        else {
            printf("4. ע���ҵ��˻�\n");
        }
        printf("5. �޸��ҵ���Ϣ\n");
        printf("6. Ͷ�ߴ���\n");
        printf("7. ���մ���\n");
        printf("0. �˳���¼\n");
        printf("��ѡ�����: ");

        if (scanf_s("%d", &choice) != 1) {
            clear_input_buffer();
            choice = -1; // ��Ч����
        }

        switch (choice) {
        case 1: // �û�����
            user_management_menu(admin);
            break;

        case 2:
            manage_express(admin);
            pause_program();
            clear_input_buffer();
            break;

        case 3: // ���ܹ���
            shelf_management_menu(admin);
            break;

        case 4: // ����Ա����/ע���˻�
            if (is_super) {
                admin_management_menu(admin);
            }
            else {
                admin_delete_account(admin);
            }
            break;
        case 5: // �޸��ҵ���Ϣ
            modify_admin_info(admin);
            break;
        case 6://Ͷ�ߴ���
            admin_handle_complaints();
            break;
        case 7://���մ���
            admin_reject_management_menu();
            break;
        case 0: // �˳���¼
            return;

        default:
            printf("��Ч��ѡ��\n");
            pause_program();
            clear_input_buffer();
        }
    } while (true);
}

// �û�����˵�
void user_management_menu(Admin* admin) {
    int choice;

    while (1) {
        clear_screen();
        printf("\n========== �û�����˵� ==========\n");
        printf("��ǰ����Ա: %s\n", admin->username);
        printf("1. �鿴�����û�\n");
        printf("2. ����û�\n");
        printf("3. �޸��û���Ϣ\n");
        printf("4. ע���û�\n");
        printf("0. �������˵�\n");
        printf("��ѡ�����: ");

        if (scanf_s("%d", &choice) != 1) {
            clear_input_buffer();
            continue;
        }

        switch (choice) {
        case 1://�鿴�����û�
            view_all_users();
            break;

        case 2://����û�
            admin_add_user();
            break;
        case 3: // �޸��û���Ϣ
            /*����ʾ�����û���Ϣ�����û����ҵ��û��������û��޸��Լ���Ϣ�ĺ���*/

            // ����ʾ�����û�
            view_all_users();
            // �û�ѡ�񲿷�
            char username[50];
            printf("\n����Ҫ�޸ĵ��û���������0���أ�: ");
            if (fgets(username, sizeof(username), stdin) == NULL) {
                clear_input_buffer();
                break;
            }
            username[strcspn(username, "\n")] = '\0'; // ȥ�����з�

            if (strcmp(username, "0") == 0) break;

            User* user = find_user_by_username(user_list, username);//�ҵ��û�
            if (user) {//�����޸�ָ���û���Ϣ����
                if (modify_user_info(user)) {
                    save_users_to_file(user_list, "users.txt");
                    printf("\n�û���Ϣ�Ѹ��£�\n");
                }
            }
            else {
                printf("\n�����û� %s �����ڣ�\n", username);
            }

            pause_program();
            clear_input_buffer();
            break;
        case 4: // ע���û�
        {
            // ����ʾ�����û�
            view_all_users();
            printf("����Ҫע�����û���: ");

            char username[50];
            if (scanf_s("%49s", username, (unsigned)_countof(username)) != 1) {
                clear_input_buffer();
                printf("������Ч��\n");
                pause_program();
                clear_input_buffer();
                break;
            }

            // ������뻺�����е�ʣ���ַ����������з���
            clear_input_buffer();

            // �����û��Ƿ����
            User* user_to_delete = find_user_by_username(user_list, username);
            if (!user_to_delete) {
                printf("�û� %s ������\n", username);
                pause_program();
                clear_input_buffer();
                break;
            }

            // ��ʾҪɾ�����û���ϸ��Ϣ
            printf("\n=== ����ɾ�����û���Ϣ ===\n");
            printf("�û���: %s\n", user_to_delete->username);
            printf("�ֻ���: %s\n", user_to_delete->phone);
            printf("�û�����: ");
            switch (user_to_delete->level) {
            case USER_NORMAL:    printf("��ͨ�û�"); break;
            case USER_STUDENT:   printf("ѧ���û�"); break;
            case USER_VIP:      printf("VIP�û�"); break;
            case USER_ENTERPRISE: printf("��ҵ�û�"); break;
            case USER_PARTNER:  printf("�����̻�"); break;
            }
            printf("\n");

            // ȷ�ϲ���
            printf("\nȷ��Ҫ����ɾ�����û���(y): ");
            char confirm;
            scanf(" %c", &confirm);  // ע��ǰ��Ŀո���������հ��ַ�
            clear_input_buffer();

            if (tolower(confirm) != 'y') {
                printf("��ȡ��ɾ������\n");
                pause_program();
                clear_input_buffer();
                break;
            }

            // ִ��ɾ��
            if (delete_user(&user_list, username)) {
                user_count--;
                printf("\n�û� %s �ѳɹ�ע��\n", username);
                save_users_to_file(user_list, "users.txt");
            }
            pause_program();
            clear_input_buffer();
            break;
        }
        case 0:
            return;

        default:
            printf("��Чѡ��!\n");
            pause_program();
            clear_input_buffer();
        }
    }
}

// ���ܹ���˵�
void shelf_management_menu(Admin* admin) {
    if (!admin) return;

    int choice;

    do {
        clear_screen();
        printf("\n========== ���ܹ���ϵͳ ==========\n");
        printf("��ǰ�û�: %s\n", admin->username);
        printf("-----------------------------------\n");
        printf("1. �����»���\n");
        printf("2. �鿴����״̬\n");
        printf("3. ɾ������\n");
        printf("4. �����̵�\n");
        printf("5. ���Ԥ��\n");
        printf("0. �����ϼ��˵�\n");
        printf("-----------------------------------\n");
        printf("��ѡ�����: ");

        if (scanf_s("%d", &choice) != 1) {
            clear_input_buffer();
            choice = -1;
        }

        switch (choice) {
        case 1:   // �����»���
        {
            char shelf_prefix;//��������ǰ׺
            int max_capacity = 0;//�������
            bool is_valid = false;

            // ������������ѭ��
            while (!is_valid) {
                clear_screen();
                printf("\n========== �����»��� ==========\n");
                printf("��ѡ��������ͣ�\n");
                printf("N - ��ͨ����\n");
                printf("F - ����Ʒ����\n");
                printf("C - ��������\n");
                printf("B - �������\n");
                printf("Q - ȡ������\n");
                printf("--------------------------------\n");
                printf("������ѡ��(N,F,C,B,Q): ");

                // ��ȡ����
                if (scanf_s(" %c", &shelf_prefix, 1) != 1) {
                    clear_input_buffer();
                    printf("\n�������\n");
                    pause_program();
                    clear_input_buffer();
                    continue;
                }
                clear_input_buffer();

                shelf_prefix = toupper(shelf_prefix);//תΪ��д

                // ����Ƿ�Ҫ���˳�
                if (shelf_prefix == 'Q') {
                    break;  // ����ѭ��������case
                }

                // ��֤����
                if (strchr("NFCB", shelf_prefix) == NULL) {
                    printf("\n����ֻ������N��F��C��B��\n");
                    pause_program();
                    clear_input_buffer();
                    continue;
                }

                is_valid = true;
            }

            if (!is_valid) break; // �û�ȡ���˲���

            // �Զ����ɻ���ID����N01��N02�ȣ�
            int max_num = 0;
            Shelf* curr = shelf_list;
            while (curr) {
                // ���ͬ���ͻ��ܣ��ҵ���
                if (toupper(curr->shelf_id[0]) == shelf_prefix) {
                    // ��ȡ��Ų���(��N01�е�01) Ȼ��+1��Ϊ�Լ���
                    int num = atoi(curr->shelf_id + 1);
                    if (num > max_num) {
                        max_num = num;
                    }
                }
                curr = curr->next;
            }

            char shelf_id[10];// �洢���ɵ�ID
            // ��ʽ����ID��ǰ׺+��λ����(��N01)
            snprintf(shelf_id, sizeof(shelf_id), "%c%02d", shelf_prefix, max_num + 1);

            // ������������ѭ���������Ի��ƣ�
            is_valid = false;
            while (!is_valid) {
                clear_screen();
                printf("\n��������: %c\n", shelf_prefix);
                printf("�Զ����ɻ���ID: %s\n", shelf_id);
                printf("--------------------------------\n");
                printf("���������������1-999������0ȡ����: ");

                if (scanf_s("%d", &max_capacity) != 1) {
                    clear_input_buffer();
                    printf("\n�����������֣�\n");
                    pause_program();
                    clear_input_buffer();
                    continue;
                }
                clear_input_buffer();

                // ���ȡ������
                if (max_capacity == 0) {
                    printf("\n��ȡ����������\n");
                    pause_program();
                    clear_input_buffer();
                    break;
                }

                // ��֤��Χ
                if (max_capacity < 1 || max_capacity > 999) {
                    printf("\n����������1-999֮�䣡\n");
                    pause_program();
                    clear_input_buffer();
                }
                else {
                    is_valid = true;
                }
            }

            if (!is_valid) break; // �û�ȡ������������

            // ��������
            Shelf* new_shelf = create_shelf(shelf_id, max_capacity);
            if (!new_shelf) {
                printf("\n��������ʧ�ܣ��ڴ治�㣩��\n");
                pause_program();
                clear_input_buffer();
                break;
            }

            // ���û�������
            switch (shelf_prefix) {
            case 'F': new_shelf->type = SHELF_FRAGILE; break;// ����Ʒ
            case 'C': new_shelf->type = SHELF_COLD; break;// ����
            case 'B': new_shelf->type = SHELF_BULK; break;// ���
            default: new_shelf->type = SHELF_NORMAL; // Ĭ����ͨ
            }

            // ��ӵ�����
            add_shelf(&shelf_list, new_shelf);

            // ��������
            if (save_shelves_to_file(shelf_list, "shelves.txt")) {
                // ��ɫ��ʾ�ɹ���Ϣ
                printf("\n\033[32m[�ɹ�] ���� %s �Ѵ�����������%d��\033[0m\n",
                    new_shelf->shelf_id, new_shelf->max_capacity);
                printf("��ǰ����������%d\n", count_shelves(shelf_list));
            }
            else {
                // ��ɫ��ʾ������Ϣ
                printf("\n\033[33m���棺���ܴ����ɹ�������ʧ�ܣ�\033[0m\n");
                printf("���ֶ��������ݲ���ϵ����Ա\n");
            }

            pause_program();
            clear_input_buffer();
            break;
        }
        case 2: // �鿴����״̬
        {
            clear_screen();
            // ����Ƿ��л�������
            if (!shelf_list) {// �����������Ƿ�Ϊ��
                printf("\nϵͳ����û�п��õĻ������ݣ�\n");
                printf("����ԭ��\n");
                printf("1. ��δ�����κλ���\n");
                printf("2. ����δ��ȷ����\n");
                pause_program();
                clear_input_buffer();
                break;
            }
            // ��ʾ���л���״̬
            display_shelves(shelf_list);
            break;
        }
        case 3: // ɾ������
        {
            char shelf_id[10] = { 0 };
            bool success = false;

            while (!success) {
                clear_screen();
                printf("\n====== ɾ������ ======\n");
                printf("��ǰ�����б�\n");
                display_shelves(shelf_list);  // ��ʾ���л���


                printf("\n������Ҫɾ���Ļ���ID������Q�˳���: ");
                if (scanf_s("%9s", shelf_id, (unsigned)_countof(shelf_id)) != 1) {
                    clear_input_buffer();
                    printf("\n�������\n");
                    pause_program();
                    clear_input_buffer();
                    continue;
                }
                clear_input_buffer();

                // ����Ƿ�Ҫ���˳�
                if (strcmp(shelf_id, "Q") == 0 || strcmp(shelf_id, "q") == 0) {
                    break;
                }

                // ��֤����ID��ʽ
                if (!is_valid_shelf_id(shelf_id)) {
                    printf("\n������Ч�Ļ���ID��ʽ��\n");
                    printf("����ID��ʽҪ��\n");
                    printf("1. ����ĸ��ͷ����A-Z��\n");
                    printf("2. ֻ������ĸ������\n");
                    printf("3. ����2-9���ַ�\n");
                    printf("��ȷ��ʽʾ����A01, B02\n");
                    pause_program();
                    clear_input_buffer();
                    continue;
                }

                // ����ɾ��
                success = delete_shelf(&shelf_list, shelf_id);
                if (!success) {
                    pause_program();
                    clear_input_buffer();// ���û�����������Ϣ
                }
            }
            save_shelves_to_file(shelf_list, "shelves.txt");
            pause_program();
            clear_input_buffer();
            break;
        }
        case 4: // �����̵�
        {// �����̵�˵�
            inventory_menu(shelf_list);
            break;
        }

        case 5:  // ���Ԥ��
            check_shelf_capacity(shelf_list);
            break;

        case 0:  // �����ϼ��˵�
            return;

        default:
            printf("��Ч��ѡ��\n");
            pause_program();
            clear_input_buffer();
        }
    } while (true);
}

// �����̵�˵�
void inventory_menu(Shelf* head) {
    int choice;
    char shelf_id[16];
    Shelf* target_shelf = NULL;// ���ڲ���Ŀ�����

    while (1) {
        clear_screen();
        printf("\n====== ��վ����̵�ϵͳ ======\n");
        printf("1. �̵㵥������\n");
        printf("2. ȫվ�̵�\n");
        printf("0. �������˵�\n");
        printf("��ѡ�����: ");

        if (scanf_s("%d", &choice) != 1) {
            printf("������Ч�����������֣�\n");
            clear_input_buffer();
            continue;
        }

        // �����û�ѡ��
        switch (choice) {
        case 1: { // ���������̵�
            printf("���������ID: ");
            if (scanf_s("%15s", shelf_id, (unsigned)_countof(shelf_id)) != 1) {
                clear_input_buffer();
                // ����������
            }
            clear_input_buffer();// ������з�

            target_shelf = shelf_list;
            while (target_shelf && strcmp(target_shelf->shelf_id, shelf_id) != 0) {
                target_shelf = target_shelf->next;
            }

            if (target_shelf) {
                inventory_shelf(target_shelf);  // ��ʾ���ܿ�� �����̵㺯��

            }
            else {
                printf("\n����δ�ҵ�ָ�����ܣ�\n");
                printf("����ԭ��\n");
                printf("��δ�����û���\n");
            }
            pause_program();
            clear_input_buffer();
            break;
        }

        case 2: // ȫվ�̵�
            if (!head) {
                printf("\n���󣺵�ǰû�л������ݣ�\n");
            }
            else {
                full_inventory(head);  // ��ʾ���л���״̬
                printf("\n");
                show_all_overdue_items_with_stats(); // ��ʾ��������������
            }
            pause_program();
            clear_input_buffer();
            break;

        case 0: // �˳�
            return;

        default:
            printf("��Чѡ�����������룡\n");
            pause_program();
            clear_input_buffer();
        }
    }
}

// ����Ա����˵�
void admin_management_menu(Admin* current_admin) {
    if (!current_admin) return;

    // Ȩ�޼�� - ֻ�г�������Ա���Է��ʴ˲˵�
    if (!is_super_admin(current_admin)) {
        printf("\nȨ�޲��㣡ֻ�г�������Ա���Է��ʹ���Ա����ϵͳ��\n");
        pause_program();
        clear_input_buffer();
        return;
    }

    int choice;
    Admin* target_admin = NULL;

    while (1) {
        clear_screen();
        printf("\n========== ����Ա����ϵͳ ==========\n");
        printf("��ǰ�û�: %s [��������Ա]\n", current_admin->username);
        printf("-----------------------------------\n");
        printf("1. �鿴���й���Ա\n");
        printf("2. ע���¹���Ա\n");
        printf("3. �޸���������Ա��Ϣ\n");
        printf("4. ע����ͨ����Ա�˻�\n");
        printf("0. �������˵�\n");
        printf("-----------------------------------\n");
        printf("��ѡ�����: ");

        if (scanf_s("%d", &choice) != 1) {
            clear_input_buffer();
            printf("��Ч���룬������ѡ��\n");
            pause_program();
            clear_input_buffer();
            continue;
        }

        switch (choice) {
        case 0:  // �������˵�
            return;
        case 1:  // �鿴���й���Ա
            view_all_admins();
            break;
        case 2:  // ע���¹���Ա
            admin_register(current_admin);
            break;

        case 3:  // �޸���������Ա��Ϣ
            super_modify_admin_info(current_admin);
            break;

        case 4:  // ע����ͨ����Ա�˻�
            admin_delete_account(current_admin);
            break;

        default:
            printf("��Чѡ�����������룡\n");
            pause_program();
            clear_input_buffer();
        }
    }
}

