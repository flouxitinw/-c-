#include "cainiao.h"

// ��ʾ���������Ϣ
void display_express_info(Express* express) {
    char time_buffer[26];
    printf("��ݵ��ţ�%s\n", express->tracking_num);
    printf("�ļ��ˣ�%s (%s)\n", express->sender_name, express->sender_phone);
    printf("��Ʒ���ͣ�%s\n", get_package_type_name(express->package_type));
    ctime_s(time_buffer, sizeof(time_buffer), &express->storage_time);
    printf("��վʱ�䣺%s", time_buffer);
    printf("���ܺţ�%s  ȡ���룺%s\n", express->shelf_id, express->pickup_code);
    printf("--------------------------------\n");
}

// ��ʾ�û���ȡ����б�
int display_pending_pickups(User* user, ExpressNode* list) {
    ExpressNode* curr = list;
    int count = 0;
    char time_buffer[26];

    printf("\n=== %s�Ĵ�ȡ��� ===\n", user->username);
    while (curr != NULL) {
        if (strcmp(curr->data.receiver_phone, user->phone) == 0 &&
            (curr->data.status == STATUS_UNPICKED || curr->data.status == STATUS_OVERDUE || curr->data.status == STATUS_UNPICKED_AGAIN)) {

            printf("%d.\n", ++count);
            display_express_info(&curr->data);
        }
        curr = curr->next;
    }

    if (count == 0) {
        printf("��ǰû�����Ĵ�ȡ���\n");
    }

    return count;
}

// ��֤�ֻ���
bool verify_phone(User* user, Express* express) {
    int attempts = 3;
    char comfirm_phone[12];

    while (attempts > 0) {
        printf("\n�������ռ����ֻ��Ž�����֤��ʣ�ೢ�Դ�����%d����", attempts);
        input_phone(comfirm_phone, sizeof(comfirm_phone), "�绰��");
        if (strcmp(comfirm_phone, express->receiver_phone) == 0) {
            return true;
        }

        attempts--;
        if (attempts > 0) {
            printf("�ֻ��Ų�ƥ�䣬�����ԣ�\n");
        }
    }

    return false;
}

// ���ȡ������
void complete_pickup(User* user, Express* express) {
    express->status = STATUS_PICKED;
    express->create_time = time(NULL);
    strncpy(express->pickup_phone, user->phone, sizeof(express->pickup_phone) - 1);
    if (remove_express_from_shelf(&shelf_list, express->shelf_id, express->tracking_num)) {
        char time_buffer[26];
        printf("\n=== ȡ���ɹ� ===\n");
        printf("��ݵ��ţ�%s\n", express->tracking_num);
        ctime_s(time_buffer, sizeof(time_buffer), &express->create_time);
        printf("ǩ��ʱ�䣺%s", time_buffer);

        if (express->package_type == VALUABLES && express->is_insured) {
            printf("����������Ʒ�Ƿ���ã�����ǩ�յ���ǩ��ȷ�ϣ�\n");
        }
        save_pickup_express_info(pickup_list, PICKUP_FILE);
        save_shelves_to_file(shelf_list, "shelves.txt");
    }
    else {
        printf("ȡ��ʧ��");
        return;
    }
}

// ����ȡ������
void process_pickup(User* user) {
    char tracking_num[20] = { 0 };
    printf("\n������Ҫȡ�Ŀ�ݵ��ţ�����0ȡ������");
    scanf_s("%19s", tracking_num, (unsigned)_countof(tracking_num));
    clear_input_buffer();

    if (strcmp(tracking_num, "0") == 0) {
        printf("��ȡ��ȡ������\n");
        return;
    }

    ExpressNode* express = find_express_by_tracking(pickup_list, tracking_num);
    if (!express) {
        printf("δ�ҵ�ƥ��Ŀ�ݼ�¼\n");
        return;
    }
    printf("\n��ݵ��ţ�%s ��ȡ�����ǣ�%s\n", express->data.tracking_num, express->data.pickup_code);

    printf("ȷ��Ҫȡ����������(y/n): ");
    char confirm;
    scanf_s(" %c", &confirm, 1);
    clear_input_buffer();

    if (tolower(confirm) != 'y') {
        printf("��ȡ��ȡ������\n");
        return;
    }

    if (!verify_phone(user, express)) {
        printf("\n��֤ʧ�ܣ��Ѵﵽ����Դ�����\n");
        return;
    }

    complete_pickup(user, express);
}

// ���Ҵ�ȡ���
void search_pickup_express(User* user) {
    clear_screen();
    printf("\n========== ��ȡ��ݲ�ѯ ==========\n");

    printf("\n��ѡ���ѯ��ʽ��\n");
    printf("1. ���ļ���������ѯ\n");
    printf("2. ���ļ����ֻ��Ų�ѯ\n");
    printf("3. ����Ʒ���Ͳ�ѯ\n");
    printf("0. ����\n");

    int choice = input_int_in_range("������ѡ��(0-3): ", 0, 3);
    if (choice == 0) return;

    char search_name[50] = { 0 };
    char search_phone[12] = { 0 };
    int search_type = -1;

    switch (choice) {
    case 1:
        safe_input(search_name, sizeof(search_name), "������ļ��������ؼ���: ");
        break;
    case 2:
        clear_input_buffer();
        input_phone(search_phone, sizeof(search_phone), "������ļ��������ֻ���: ");
        break;
    case 3:
        printf("\n��ѡ����Ʒ���ͣ�\n");
        for (int i = 0; i < 13; i++) {
            printf("%d. %s\n", i, get_package_type_name((PackageType)i));
        }
        search_type = input_int_in_range("��������Ʒ���ͱ��(0-12): ", 0, 12);
        break;
    }

    ExpressNode* current = pickup_list;
    int total_matches = 0;

    printf("\n=== ��ѯ��� ===\n");
    while (current != NULL) {
        if (strcmp(current->data.receiver_phone, user->phone) == 0 &&
            (current->data.status == STATUS_UNPICKED || current->data.status == STATUS_OVERDUE || current->data.status == STATUS_UNPICKED_AGAIN)) {

            int match = 0;
            switch (choice) {
            case 1:
                match = strstr(current->data.sender_name, search_name) != NULL;
                break;
            case 2:
                match = strcmp(current->data.sender_phone, search_phone) == 0;
                break;
            case 3:
                match = current->data.package_type == search_type;
                break;
            }

            if (match) {
                total_matches++;
                printf("\n%d.\n", total_matches);
                display_express_info(&current->data);
            }
        }
        current = current->next;
    }

    if (total_matches == 0) {
        printf("\nδ�ҵ����������Ŀ�ݡ�\n");
    }
    else {
        printf("\n���ҵ� %d ����ȡ��ݼ�¼��\n", total_matches);
        process_pickup(user);
    }
}

// ��ȡ������
void pickup_express(User* user) {
    if (!user) {
        printf("ϵͳ���󣺲�����Ч\n");
        return;
    }

    while (1) {
        clear_screen();
        printf("\n=== ȡ������ ===\n");
        printf("1. �鿴���д�ȡ��ݲ�ȡ��\n");
        printf("2. ���Ҵ�ȡ��ݲ�ȡ��\n");
        printf("3. ���챨��\n");
        printf("4. �����ϼ��˵�\n");
        printf("��ѡ�����: ");

        int choice = input_int_in_range("", 1, 4);
        clear_input_buffer();

        switch (choice) {
        case 1:
            if (display_pending_pickups(user, pickup_list) > 0) {
                process_pickup(user);
            }
            pause_program();
            break;
        case 2:
            search_pickup_express(user);
            pause_program();
            break;
        case 3:
            mistaken_pickup(user);
            break;
        case 4:
            return;
        default:
            printf("��Ч��ѡ��\n");
            pause_program();
        }
    }
}

// ��ʾ�û���ȡ����б�
int display_picked(User* user, ExpressNode* list) {
    ExpressNode* curr = list;
    int count = 0;

    printf("\n=== %s����ȡ��� ===\n", user->username);
    while (curr != NULL) {
        if (strcmp(curr->data.pickup_phone, user->phone) == 0 &&
            (curr->data.status == STATUS_PICKED)) {
            printf("%d.\n", ++count);
            display_express_info(&curr->data);
        }
        curr = curr->next;
    }

    if (count == 0) {
        printf("��δȡ�����\n");
    }

    return count;
}
// ���촦����
void mistaken_pickup(User* user) {
    clear_screen();
    printf("\n========== ������촦�� ==========\n");

    // ��ʾ�û���ȡ���Ŀ��
    int count = display_picked(user, pickup_list);
    if (count == 0) {
        pause_program();
        return;
    }
    printf("\n����������Ŀ�ݵ��ţ�����0ȡ������");
    char tracking_num[20] = { 0 };
    scanf_s("%19s", tracking_num, (unsigned)_countof(tracking_num));
    clear_input_buffer();
    if (strcmp(tracking_num, "0") == 0) {
        printf("��ȡ������\n");
        return;
    }
    ExpressNode* express = find_express_by_tracking(pickup_list, tracking_num);
    if (!express || strcmp(express->data.pickup_phone, user->phone) != 0) {
        printf("\nδ�ҵ��ÿ��\n");
        pause_program();
        return;
    }
    if (strcmp(express->data.pickup_phone, express->data.receiver_phone) == 0)
    {
        printf("\n��Ϊ�˿�ݵ��ռ��ˣ�����������ȡ\n");
        pause_program();
        return;
    }
    if (express->data.status != STATUS_PICKED &&
        express->data.status != STATUS_WRONGPICKED &&
        express->data.status != STATUS_LOST) {
        printf("\n�ÿ��״̬������������������\n");
        pause_program();
        return;
    }
    printf("\nȷ���ύ����������(y/n): ");
    char confirm;
    scanf_s(" %c", &confirm, 1);
    clear_input_buffer();

    if (tolower(confirm) == 'y') {
        express->data.status = STATUS_MISPICKED;
        save_pickup_express_info(pickup_list, PICKUP_FILE);
        printf("\n�ѳɹ��ύ��������\n");
    }
    else {
        printf("\n��ȡ������\n");
    }
    pause_program();
}