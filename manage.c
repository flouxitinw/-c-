#include "cainiao.h"
// ���������нڵ������
int count_nodes(ExpressNode* head) {
    int count = 0;
    ExpressNode* current = head;

    while (current != NULL) {
        count++;
        current = current->next;
    }

    return count;
}
// ����Ŀ����Ϣ�������ļ���Ͷ������ֶΣ�
void save_send_express_info(ExpressNode* head, const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("�޷��򿪼ļ������ļ�");
        return;
    }

    // ��ͷ�����������ֶΣ�
    fprintf(fp, "send_num,tracking_num,status,sender_name,sender_phone,receiver_name,receiver_phone,"
        "weight,volume,package_type,method,region,is_insured,insured_value,"
        "express_type,fee,create_time,complaint_id,complaint_type,complaint_time,"
        "resolve_time,complaint_resolution,compensation\n");

    ExpressNode* current = head;
    while (current != NULL) {
        Express* e = &current->data;


        if (e->status == STATUS_LOST || e->status == STATUS_DAMAGED || e->status == STATUS_ABNORMAL_RESOLVED) {
            fprintf(fp, "%d,%s,%d,%s,%s,%s,%s,%.2f,%.2f,%d,%d,%d,%d,%.2f,%d,%.2f,%lld,"
                "%d,%d,%lld,%lld,%s,%.2f\n",
                e->send_num,
                e->tracking_num,
                e->status,
                e->sender_name,
                e->sender_phone,
                e->receiver_name,
                e->receiver_phone,
                e->weight,
                e->volume,
                e->package_type,
                e->method,
                e->region,
                e->is_insured,
                e->insured_value,
                e->express_type,
                e->fee,
                (long long)e->create_time,
                e->complaint_id,
                e->complaint_type,
                (long long)e->complaint_time,
                (long long)e->resolve_time,
                e->complaint_resolution,
                e->compensation
            );
        }

        else {
            fprintf(fp, "%d,%s,%d,%s,%s,%s,%s,%.2f,%.2f,%d,%d,%d,%d,%.2f,%d,%.2f,%lld,"
                ",,,,0.0\n",  // ���ֶ��ö���ռλ����ֵ�ֶ�д 0
                e->send_num,
                e->tracking_num,
                e->status,
                e->sender_name,
                e->sender_phone,
                e->receiver_name,
                e->receiver_phone,
                e->weight,
                e->volume,
                e->package_type,
                e->method,
                e->region,
                e->is_insured,
                e->insured_value,
                e->express_type,
                e->fee,
                (long long)e->create_time);
        }
        current = current->next;
    }
    fclose(fp);
}
// ���ؼļ�������ݣ���save_send_express_info�ϸ�ƥ�䣩
void load_send_express(ExpressNode** express_head) {
    FILE* fp = fopen("send_express.txt", "r");
    if (!fp) {
        printf("�޼ļ���¼���ļ�δ����\n");
        return;
    }

    char line[512];
    int count = 0;

    // ������ͷ
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return;
    }
    // ʹ���ֶ����ȷ���뱣��˳���ϸ�һ��
    while (fgets(line, sizeof(line), fp)) {
        Express e = { 0 };
        line[strcspn(line, "\n")] = '\0';

        char* token = strtok(line, ",");
        if (token) e.send_num = atoi(token);

        token = strtok(NULL, ",");
        if (token) {
            strncpy(e.tracking_num, token, sizeof(e.tracking_num) - 1);
        }

        token = strtok(NULL, ",");
        if (token) e.status = (PackageStatus)atoi(token);

        token = strtok(NULL, ",");
        if (token) strncpy(e.sender_name, token, sizeof(e.sender_name) - 1);

        token = strtok(NULL, ",");
        if (token) strncpy(e.sender_phone, token, sizeof(e.sender_phone) - 1);

        token = strtok(NULL, ",");
        if (token) strncpy(e.receiver_name, token, sizeof(e.receiver_name) - 1);

        token = strtok(NULL, ",");
        if (token) strncpy(e.receiver_phone, token, sizeof(e.receiver_phone) - 1);

        token = strtok(NULL, ",");
        if (token) e.weight = atof(token);

        token = strtok(NULL, ",");
        if (token) e.volume = atof(token);

        token = strtok(NULL, ",");
        if (token) e.package_type = (PackageType)atoi(token);

        token = strtok(NULL, ",");
        if (token) e.method = (ShippingMethod)atoi(token);

        token = strtok(NULL, ",");
        if (token) e.region = (RegionType)atoi(token);

        token = strtok(NULL, ",");
        if (token) e.is_insured = atoi(token);

        token = strtok(NULL, ",");
        if (token) e.insured_value = atof(token);

        token = strtok(NULL, ",");
        if (token) e.express_type = (DeliveryType)atoi(token);

        token = strtok(NULL, ",");
        if (token) e.fee = atof(token);

        token = strtok(NULL, ",");
        if (token) e.create_time = (time_t)atol(token);

        // Ͷ������ֶ�
        if (e.status == STATUS_LOST || e.status == STATUS_DAMAGED || e.status == STATUS_ABNORMAL_RESOLVED) {
            // ֻ��Ͷ��״̬�Ž��� complaint_* �ֶ�
            token = strtok(NULL, ",");
            if (token) e.complaint_id = atoi(token);

            token = strtok(NULL, ",");
            if (token) e.complaint_type = (ComplaintType)atoi(token);

            token = strtok(NULL, ",");
            if (token) e.complaint_time = (time_t)atoll(token);

            token = strtok(NULL, ",");
            if (token) e.resolve_time = (time_t)atoll(token);

            token = strtok(NULL, ",");
            if (token) strncpy(e.complaint_resolution, token, sizeof(e.complaint_resolution) - 1);

            token = strtok(NULL, ",");
            if (token) e.compensation = atof(token);
        }
        else {
            // ��Ͷ��״̬��������Щ�ֶΣ������ȡ����
            for (int i = 0; i < 6; i++) {
                strtok(NULL, ",");  // ���� 6 �� complaint_* �ֶ�
            }
        }

        // �����ڵ㲢��������ͷ��
        ExpressNode* new_node = create_express_node(&e);
        if (new_node) {
            new_node->next = *express_head;
            *express_head = new_node;
            count++;
        }
    }

    fclose(fp);
    printf("�ɹ����� %d ���ļ���¼\n", count);
}

//--------------------������������--------------------
// ������ļ���ʽ��
void load_pickup_express(ExpressNode** express_head) {
    FILE* fp = fopen("pickup_express.txt", "r");
    if (!fp) {
        printf("�޴�ȡ��ݼ�¼���ļ�δ����\n");
        return;
    }

    char line[512];
    fgets(line, sizeof(line), fp); // ������ͷ

    while (fgets(line, sizeof(line), fp)) {
        Express new_express = { 0 };
        line[strcspn(line, "\n")] = '\0';

        char* token = strtok(line, ",");
        if (token) strncpy(new_express.tracking_num, token, sizeof(new_express.tracking_num) - 1);

        token = strtok(NULL, ",");
        if (token) strncpy(new_express.sender_name, token, sizeof(new_express.sender_name) - 1);

        token = strtok(NULL, ",");
        if (token) strncpy(new_express.sender_phone, token, sizeof(new_express.sender_phone) - 1);

        token = strtok(NULL, ",");
        if (token) strncpy(new_express.receiver_name, token, sizeof(new_express.receiver_name) - 1);

        token = strtok(NULL, ",");
        if (token) strncpy(new_express.receiver_phone, token, sizeof(new_express.receiver_phone) - 1);

        token = strtok(NULL, ",");
        if (token) new_express.package_type = (PackageType)atoi(token);

        token = strtok(NULL, ",");
        if (token) new_express.method = (ShippingMethod)atoi(token);

        token = strtok(NULL, ",");
        if (token) new_express.status = (PackageStatus)atoi(token);

        token = strtok(NULL, ",");
        if (token) new_express.create_time = (time_t)atol(token);

        // ��������Ĭ��ֵ
        token = strtok(NULL, ",");
        if (token && strcmp(token, "-1") != 0)
            strncpy(new_express.pickup_code, token, sizeof(new_express.pickup_code) - 1);
        else
            memset(new_express.pickup_code, 0, sizeof(new_express.pickup_code));

        token = strtok(NULL, ",");
        if (token && strcmp(token, "-1") != 0)
            strncpy(new_express.shelf_id, token, sizeof(new_express.shelf_id) - 1);
        else
            memset(new_express.shelf_id, 0, sizeof(new_express.shelf_id));

        token = strtok(NULL, ",");
        if (token) new_express.storage_time = (time_t)atol(token);


        token = strtok(NULL, ",");
        if (token && strcmp(token, "-1") != 0)
            strncpy(new_express.pickup_phone, token, sizeof(new_express.pickup_phone) - 1);
        else
            strcpy(new_express.pickup_phone, "0");  // Ĭ��ֵΪ"0"

        // �����������ֶ�
        if (new_express.status == STATUS_REJECTED || new_express.status == STATUS_RETURNED || new_express.status == STATUS_UNPICKED_AGAIN) {
            token = strtok(NULL, ",");
            if (token && strcmp(token, "") != 0)
                strncpy(new_express.reject_reason, token, sizeof(new_express.reject_reason) - 1);
            else
                memset(new_express.reject_reason, 0, sizeof(new_express.reject_reason));

            token = strtok(NULL, ",");
            if (token && strcmp(token, "") != 0)
                new_express.reject_time = (time_t)atoll(token);
            else
                new_express.reject_time = 0;
        }
        else {
            // �Ǿ���/�˻�״̬�������������ֶ�
            strtok(NULL, ","); // ����reject_reason
            strtok(NULL, ","); // ����reject_time
        }


        // ��ӵ�����
        ExpressNode* new_node = create_express_node(&new_express);
        if (new_node) {
            new_node->next = *express_head;
            *express_head = new_node;
        }
    }
    fclose(fp);
}
void save_pickup_express_info(ExpressNode* head, const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("�޷�����������");
        return;
    }

    // д���ͷ
    fprintf(fp, "tracking_num,sender_name,sender_phone,receiver_name,receiver_phone,package_type,method,status,create_time,pickup_code,shelf_id,storage_time,pickup_phone,reject_reason,reject_time\n");

    ExpressNode* current = head;
    while (current) {
        if (current->data.status == STATUS_REJECTED || current->data.status == STATUS_RETURNED || current->data.status == STATUS_UNPICKED_AGAIN) {

            fprintf(fp, "%s,%s,%s,%s,%s,%d,%d,%d,%lld,%s,%s,%lld,%s,%s,%lld\n",
                current->data.tracking_num,
                current->data.sender_name,
                current->data.sender_phone,
                current->data.receiver_name,
                current->data.receiver_phone,
                current->data.package_type,
                current->data.method,
                current->data.status,
                current->data.create_time,
                current->data.pickup_code,
                current->data.shelf_id,
                current->data.storage_time,
                current->data.pickup_phone,
                current->data.reject_reason,
                current->data.reject_time);
        }
        else {
            // ����״̬����������ֶ�����
            fprintf(fp, "%s,%s,%s,%s,%s,%d,%d,%d,%lld,%s,%s,%lld,%s,,\n",
                current->data.tracking_num,
                current->data.sender_name,
                current->data.sender_phone,
                current->data.receiver_name,
                current->data.receiver_phone,
                current->data.package_type,
                current->data.method,
                current->data.status,
                (long long)current->data.create_time,
                current->data.pickup_code,
                current->data.shelf_id,
                (long long)current->data.storage_time,
                current->data.pickup_phone);
        }

        current = current->next;
    }

    fclose(fp);
}


//--------------------������������--------------------

// ��ȡ���䷽ʽ����
const char* get_shipping_method_name(ShippingMethod method) {
    static const char* names[] = {
        "��ͨ���",
        "����ʼ�",
        "�����ʼ�"
    };
    return (method >= SHIPPING_NORMAL && method <= SHIPPING_COLD_CHAIN)
        ? names[method]
        : "δ֪���䷽ʽ";
}
void manage_pending_express() {
    while (1) {
        clear_screen();
        printf("\n========== ������������ ==========\n");

        // ��ʾ������б�
        int count = 0;
        ExpressNode* current = pickup_list;
        while (current) {
            if (current->data.status == STATUS_PENDING_IN) {
                printf("%2d. %s | %s -> %s | %10s | ���䷽ʽ: %s\n",
                    ++count,
                    current->data.tracking_num,
                    current->data.sender_name,
                    current->data.receiver_name,
                    get_package_type_name(current->data.package_type),
                    get_shipping_method_name(current->data.method));
            }
            current = current->next;
        }

        if (count == 0) {
            printf("\n��ǰû�д����İ���\n");
            pause_program();
            return;
        }

        printf("\n����ѡ��:\n");
        printf("1. ȫ�����\n");
        printf("2. �����ϼ��˵�\n");
        printf("��ѡ�����: ");

        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1: {
            // ȫ�����
            int success_count = 0;
            current = pickup_list;
            time_t now = time(NULL);
            time_t start_time = now - (5 * 24 * 3600); // 4��ǰ��ʱ���

            while (current) {
                if (current->data.status == STATUS_PENDING_IN) {
                    // ���������ʱ���
                    time_t day_span = 24 * 3600; // һ�������
                    time_t four_days_span = 5 * day_span;

                    // ���ѡ����һ�죨0-4��
                    int random_day = rand() % 5;

                    // ��ѡ�����������ѡ��ʱ��
                    time_t random_time_in_day = rand() % day_span;

                    // �������յ����ʱ��
                    time_t random_time = start_time + (random_day * day_span) + random_time_in_day;

                    // ȷ��������now
                    if (random_time > now) {
                        random_time = now;
                    }
                    // �������
                    char shelf_prefix = (current->data.package_type == FRAGILE) ? 'F'
                        : get_shelf_prefix_by_shipping(current->data.method);
                    if (assign_to_shelf(&current->data, shelf_list, shelf_prefix, random_time)) {
                        success_count++;
                    }
                }
                current = current->next;
            }
            printf("\n�ɹ���� %d ������\n", success_count);
            break;
        }
        case 2:
            // ����
            return;
        default:
            printf("\n��Ч��ѡ��\n");
        }

        // �������
        save_pickup_express_info(pickup_list, "pickup_express.txt");
        save_shelves_to_file(shelf_list, "shelves.txt");

        pause_program();
    }
}
//--------------------δȡ��������--------------------
//// ��ʾ����δȡ��ݣ�������������
void display_all_unpicked_express() {
    clear_screen();
    printf("\n========== ����δȡ����б� ==========\n");

    int count = 0;
    ExpressNode* current = pickup_list;
    while (current != NULL) {
        if (current->data.status == STATUS_UNPICKED ||
            current->data.status == STATUS_OVERDUE || current->data.status == STATUS_UNPICKED_AGAIN) {

            char time_str[20];
            strftime(time_str, sizeof(time_str), "%m-%d %H:%M",
                localtime(&current->data.storage_time));

            printf("%2d. %s | %s (%s) | ����: %s | %10s | ���: %s",
                ++count,
                current->data.tracking_num,
                current->data.receiver_name,
                current->data.receiver_phone,
                current->data.shelf_id,
                get_package_type_name(current->data.package_type),
                time_str);

            // ��ʾ��������
            if (current->data.status == STATUS_OVERDUE) {
                double days = difftime(time(NULL), current->data.storage_time) / 86400;
                printf(" | ������ %.1f ��", days);
            }
            printf("\n");
        }
        current = current->next;
    }

    if (count == 0) {
        printf("\n��ǰû��δȡ�İ���\n");
    }

    pause_program();
}
void display_all_STATUS_OVERDUE_express() {
    clear_screen();
    printf("\n========== ������������б� ==========\n");

    int count = 0;
    ExpressNode* current = pickup_list;
    while (current != NULL) {
        if (current->data.status == STATUS_OVERDUE) {

            char time_str[20];
            strftime(time_str, sizeof(time_str), "%m-%d %H:%M",
                localtime(&current->data.storage_time));

            printf("%3d. %s | %s (%s) | ����: %s | %12s | ���: %s",
                ++count,
                current->data.tracking_num,
                current->data.receiver_name,
                current->data.receiver_phone,
                current->data.shelf_id,
                get_package_type_name(current->data.package_type),
                time_str);
            double days = difftime(time(NULL), current->data.storage_time) / 86400;
            printf(" | ������ %.1f ��", days);

            printf("\n");
        }
        current = current->next;
    }

    if (count == 0) {
        printf("\n��ǰû�������İ���\n");
    }
    pause_program();
}
// ���ռ�����Ϣ���ҿ��
void search_express_by_receiver_info() {
    clear_screen();
    printf("\n========== ���ռ�����Ϣ���ҿ�� ==========\n");

    // ѡ���ѯ��ʽ
    printf("\n��ѡ���ѯ��ʽ��\n");
    printf("1. ���ռ����ֻ��Ų�ѯ\n");
    printf("2. ���ռ���������ѯ\n");
    printf("3. ����ݵ��Ų�ѯ\n");
    printf("0. ����\n");

    int choice = input_int_in_range("������ѡ��(0-3): ", 0, 3);
    if (choice == 0) return;

    char search_phone[12] = { 0 };
    char search_name[50] = { 0 };
    char tracking_num[20] = { 0 };

    // ��ȡ��ѯ����
    switch (choice) {
    case 1:
        clear_input_buffer();
        input_phone(search_phone, sizeof(search_phone), "�������ռ����ֻ���: ");
        break;
    case 2:
        safe_input(search_name, sizeof(search_name), "�������ռ�������(֧��ģ����ѯ): ");
        break;
    case 3:
        printf("�������ݵ���: ");
        scanf_s("%19s", tracking_num, (unsigned)_countof(tracking_num));
        clear_input_buffer();
        break;
    }

    // ����ƥ��Ŀ��
    ExpressNode* current = pickup_list;
    int found_count = 0;

    printf("\n=== ��ѯ��� ===\n");
    while (current != NULL) {
        int match = 0;

        // ����ѡ����������ƥ��
        switch (choice) {
        case 1: // �ֻ���
            match = strcmp(current->data.receiver_phone, search_phone) == 0;
            break;
        case 2: // ����
            match = strstr(current->data.receiver_name, search_name) != NULL;
            break;
        case 3: // ��ݵ���
            match = strcmp(current->data.tracking_num, tracking_num) == 0;
            break;
        }

        if (match) {
            found_count++;
            printf("\n%d. ������Ϣ:\n", found_count);
            printf("��ݵ���: %s\n", current->data.tracking_num);
            printf("�ռ���: %s (%s)\n", current->data.receiver_name, current->data.receiver_phone);
            printf("�ļ���: %s (%s)\n", current->data.sender_name, current->data.sender_phone);
            printf("����λ��: %s | ȡ����: %s\n", current->data.shelf_id, current->data.pickup_code);
            printf("��������: %s\n", get_package_type_name(current->data.package_type));

            char time_str[20];
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S",
                localtime(&current->data.storage_time));
            printf("���ʱ��: %s\n", time_str);

            // ��ʾ״̬��Ϣ
            if (current->data.status == STATUS_UNPICKED || current->data.status == STATUS_UNPICKED_AGAIN) {
                double days = difftime(time(NULL), current->data.storage_time) / 86400;
                if (days > 3) {
                    printf("״̬: δȡ (�Ѵ�� %.1f ��)\n", days);
                }
                else {
                    printf("״̬: δȡ\n");
                }
            }
            else if (current->data.status == STATUS_OVERDUE) {
                double days = difftime(time(NULL), current->data.storage_time) / 86400;
                printf("״̬: ������ (%.1f ��)\n", days);
            }
            else {
                printf("״̬: %s\n", get_status_text(current->data.status));
            }
            printf("--------------------------------\n");
        }
        current = current->next;
    }

    if (found_count == 0) {
        printf("\nδ�ҵ�ƥ��Ŀ�ݼ�¼\n");
    }
    else {
        printf("\n���ҵ� %d ��ƥ���¼\n", found_count);
    }

    pause_program();
}
// ������������鲢�Զ������������
void check_and_mark_overdue_express() {
    time_t now = time(NULL);
    ExpressNode* current = pickup_list;
    int marked_count = 0;

    while (current != NULL) {
        // ����3��δȡ�İ����Զ����Ϊ����
        if ((current->data.status == STATUS_UNPICKED || current->data.status == STATUS_UNPICKED_AGAIN) &&
            difftime(now, current->data.storage_time) > 3 * 86400) {
            current->data.status = STATUS_OVERDUE;
            marked_count++;
        }
        current = current->next;
    }

    if (marked_count > 0) {
        save_pickup_express_info(pickup_list, "pickup_express.txt");
    }
}
// ���������������ʾ��������������
void check_overdue_express_with_display() {
    clear_screen();
    printf("\n========== ����������� ==========\n");

    // ��ȡ��ǰδȡ��������������
    int before_unpicked = 0, before_overdue = 0;
    ExpressNode* current = pickup_list;
    while (current != NULL) {
        if (current->data.status == STATUS_UNPICKED || current->data.status == STATUS_UNPICKED_AGAIN) before_unpicked++;
        if (current->data.status == STATUS_OVERDUE) before_overdue++;
        current = current->next;
    }

    // ִ�м��
    check_and_mark_overdue_express();

    // ��ȡ���������
    int after_unpicked = 0, after_overdue = 0;
    current = pickup_list;
    while (current != NULL) {
        if (current->data.status == STATUS_UNPICKED || current->data.status == STATUS_UNPICKED_AGAIN) after_unpicked++;
        if (current->data.status == STATUS_OVERDUE) after_overdue++;
        current = current->next;
    }

    // ��ʾ���
    printf("\n�����:\n");
    printf("ԭδȡ��������: %d\n", before_unpicked);
    printf("ԭ������������: %d\n", before_overdue);
    printf("�±��Ϊ�����İ�������: %d\n", after_overdue - before_overdue);
    printf("��ǰδȡ��������: %d\n", after_unpicked);
    printf("��ǰ������������: %d\n", after_overdue);
    printf("\n����ѡ��:\n");
    printf("1. �鿴������������\n");
    printf("2. �����ϼ��˵�\n");
    printf("��ѡ��: ");
    int choice;
    scanf_s("%d", &choice);
    clear_input_buffer();
    if (choice == 1) {
        display_all_STATUS_OVERDUE_express();  // ��ʾ����
    }
    pause_program();
}
// δȡ�����������˵�
void manage_unpicked_express() {
    while (1) {
        clear_screen();
        printf("\n========== δȡ�������� ==========\n");

        // �����˵�
        printf("1. ��ʾ����δȡ���\n");
        printf("2. ����δȡ���\n");
        printf("3. �����������\n");
        printf("4. �����ϼ��˵�\n");
        printf("��ѡ�����: ");
        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        // ִ�в���
        switch (choice) {
        case 1:
            display_all_unpicked_express();
            break;
        case 2:
            search_express_by_receiver_info();
            break;
        case 3:
            check_overdue_express_with_display();
            break;
        case 4:
            return;
        default:
            printf("\n��Ч�Ĳ���ѡ��\n");
            pause_program();
        }
    }
}


//--------------------��ȡ��������--------------------
// ��ȡ��������
void manage_picked_express() {
    while (1) {
        clear_screen();
        printf("\n========== ��ȡ�������� ==========\n");

        // ��ʾ��ȡ�����б���ȡ��ʱ�䵹��
        int count = 0;
        ExpressNode* current = pickup_list;
        while (current != NULL) {
            if (current->data.status == STATUS_PICKED) {
                char time_str[20];
                strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M",
                    localtime(&current->data.create_time));

                printf("%d. %s | %s (%s) | ����: %s | ȡ�����ֻ���: %s  | ȡ��ʱ��: %s \n",
                    ++count,
                    current->data.tracking_num,
                    current->data.receiver_name,
                    current->data.receiver_phone,
                    current->data.shelf_id,
                    current->data.pickup_phone,
                    time_str);
            }
            current = current->next;
        }

        if (count == 0) {
            printf("\n��ǰû����ȡ������¼\n");
            pause_program();
            return;
        }

        // �����˵�
        printf("\n����ѡ��:\n");
        printf("1. ɾ��������ȡ������¼\n");
        printf("2. ѡ��ɾ��\n");
        printf("3. �����ϼ��˵�\n");
        printf("��ѡ�����: ");

        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1: {
            // ɾ��������ȡ������¼
            char confirm;
            printf("\n���棺�⽫����ɾ��������ȡ������¼��\n");
            printf("ȷ��ɾ����(y/n): ");
            scanf_s(" %c", &confirm, 1);
            clear_input_buffer();

            if (tolower(confirm) == 'y') {
                int deleted_count = 0;
                ExpressNode** ptr = &pickup_list;

                while (*ptr != NULL) {
                    if ((*ptr)->data.status == STATUS_PICKED) {
                        ExpressNode* to_delete = *ptr;
                        *ptr = (*ptr)->next;
                        free(to_delete);
                        deleted_count++;
                    }
                    else {
                        ptr = &(*ptr)->next;
                    }
                }

                printf("\n��ɾ�� %d ����ȡ������¼\n", deleted_count);
                save_pickup_express_info(pickup_list, "pickup_express.txt");
            }
            else {
                printf("\nȡ��ɾ������\n");
            }
            break;
        }

        case 2: {
            // ѡ��ɾ��
            char tracking_num[20] = { 0 };
            printf("\n������Ҫɾ���Ŀ�ݵ���: ");
            scanf_s("%19s", tracking_num, (unsigned)_countof(tracking_num));
            clear_input_buffer();

            ExpressNode** ptr = &pickup_list;
            int found = 0;

            while (*ptr != NULL) {
                if (strcmp((*ptr)->data.tracking_num, tracking_num) == 0 &&
                    (*ptr)->data.status == STATUS_PICKED) {

                    ExpressNode* to_delete = *ptr;
                    *ptr = (*ptr)->next;
                    free(to_delete);
                    found = 1;

                    printf("\n��� %s ��¼��ɾ��\n", tracking_num);
                    save_pickup_express_info(pickup_list, "pickup_express.txt");
                    break;
                }
                ptr = &(*ptr)->next;
            }

            if (!found) {
                printf("\nδ�ҵ��ÿ�ݻ���״̬������\n");
            }
            break;
        }

        case 3:
            return; // �����ϼ��˵�

        default:
            printf("\n��Ч��ѡ��\n");
        }

        pause_program();
    }
}
//--------------------���ĳ���������--------------------
// ���ĳ���������
// ���ɲ��ظ��Ŀ�ݵ��ţ���ʽ��������+4λ���������202308250001��
void generate_tracking_num(Express* express) {
    // ֻ��δ���ɵ��ŵĿ�ݲ���Ҫ����
    if (strcmp(express->tracking_num, "0") == 0) {
        time_t now = time(NULL);
        struct tm* tm_info = localtime(&now);
        char buffer[20];

        strftime(buffer, 9, "%Y%m%d", tm_info);
        int random_num = rand() % 10000;
        sprintf(buffer + 8, "%04d", random_num);
        buffer[12] = '\0';

        // ����Ƿ��ظ�
        while (find_express_by_tracking(send_list, buffer)) {
            random_num = (random_num + 1) % 10000;
            sprintf(buffer + 8, "%04d", random_num);
        }

        strcpy(express->tracking_num, buffer);
    }
}
void manage_unsent_express() {
    clear_screen();
    printf("\n=== ���ĳ��������� ===\n");

    // ͳ������
    int total_count = 0;
    int invalid_count = 0;
    ExpressNode* current = send_list;

    // ��һ�α�����ͳ����������Ч����
    while (current != NULL) {
        if (current->data.status == STATUS_UNSENT) {
            total_count++;
            if (current->data.package_type == ALCOHOL || current->data.package_type == ANIMAL || current->data.package_type == WEAPON ||
                current->data.package_type == HAZARDOUS || current->data.weight > 50.0 || current->data.volume > 3000000.0) {
                invalid_count++;
            }
        }
        current = current->next;
    }

    if (total_count == 0) {
        printf("\n��ǰû�д��ĳ��İ���\n");
        pause_program();
        return;
    }

    // ��ʾ���棨����в����ʼĵģ�
    if (invalid_count > 0) {
        printf("\n���� %d �������ʼİ���������/����/����\n", invalid_count);
    }

    // ��ʾ���
    printf("\n���ĳ������б���%d������\n", total_count);
    printf("�����������Щ��������������������������������Щ����������������������������Щ��������������������������������Щ����������������������������Щ�����������������������������\n");
    printf("��%-4s�� %-14s �� %-12s �� %-14s �� %-12s �� %-12s ��\n",
        "���", "�ļ���", "�ļ��绰", "�ռ���", "�ռ��绰", "״̬");
    printf("�����������੤�������������������������������੤���������������������������੤�������������������������������੤���������������������������੤����������������������������\n");

    // �ڶ��α�������ʾ����
    current = send_list;
    int index = 0;
    while (current != NULL) {
        if (current->data.status == STATUS_UNSENT) {
            Express* e = &current->data;
            const char* status = "���ʼ�";

            if (e->package_type == ALCOHOL || e->package_type == ANIMAL || e->package_type == WEAPON ||
                e->package_type == HAZARDOUS) {
                status = "�����ʼģ����ˣ�";
            }
            else if (e->weight > 50.0) {
                status = "�����ʼģ����أ�";
            }
            else if (e->volume > 3000000.0) {
                status = "�����ʼģ�����";
            }

            printf("��%-4d�� %-14s �� %-12s �� %-14s �� %-12s �� %-12s ��\n",
                ++index,
                e->sender_name,
                e->sender_phone,
                e->receiver_name,
                e->receiver_phone,
                status);
        }
        current = current->next;
    }
    printf("�����������ة��������������������������������ة����������������������������ة��������������������������������ة����������������������������ة�����������������������������\n");

    // �����˵�
    printf("\n����ѡ��:\n");
    printf("1. �����ĳ����п��ʼİ���\n");
    printf("2. �����ϼ��˵�\n");

    int choice = input_int_in_range("��ѡ�����(1-2): ", 1, 2);
    clear_input_buffer();

    if (choice == 1) {
        // �����ĳ�����������ʼĵģ�
        int success_count = 0;
        current = send_list;
        while (current != NULL) {
            if (current->data.status == STATUS_UNSENT) {
                Express* e = &current->data;

                // ���������ʼĵ�
                if (e->package_type != ALCOHOL && e->package_type != ANIMAL && e->package_type != WEAPON &&
                    e->package_type != HAZARDOUS &&
                    e->weight <= 50.0 &&
                    e->volume <= 3000000.0) {
                    e->status = STATUS_SENT;
                    if (strcmp(e->tracking_num, "0") == 0) {
                        generate_tracking_num(e);
                    }
                    e->create_time = time(NULL);
                    success_count++;
                }
            }
            current = current->next;
        }

        save_send_express_info(send_list, SEND_FILE);
        printf("\n���������\n");
        printf("  �ɹ��ĳ�: %d ��\n", success_count);
        printf("  ���������ʼ�: %d ��\n", invalid_count);

        if (invalid_count > 0) {
            printf("\nע�⣺���˹���ϵ�ļ��˴������ʼİ�����\n");
        }
    }

    pause_program();
}
// ����Ա����������
void manage_mispicked_express() {
    while (1) {
        clear_screen();
        printf("\n========== ��ȡ�������� ==========\n");

        // ��ʾ��ȡ�����б�
        int count = 0;
        ExpressNode* current = pickup_list;
        while (current) {
            if (current->data.status == STATUS_MISPICKED) {
                printf("%2d. %s | %s -> %s | %8s | ���䷽ʽ: %s\n",
                    ++count,
                    current->data.tracking_num,
                    current->data.sender_name,
                    current->data.receiver_name,
                    get_package_type_name(current->data.package_type),
                    get_shipping_method_name(current->data.method));
            }
            current = current->next;
        }

        if (count == 0) {
            printf("\n��ǰû����ȡ�İ���\n");
            pause_program();
            return;
        }

        printf("\n����ѡ��:\n");
        printf("1. һ������\n");
        printf("2. �����ϼ��˵�\n");
        printf("��ѡ�����: ");

        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1: {
            // ȫ���������
            int success_count = 0;
            current = pickup_list;
            time_t now = time(NULL);
            int day_offset = 4; // ��4��ǰ��ʼ

            while (current) {
                if (current->data.status == STATUS_MISPICKED) {
                    // ���ݰ������ͺ����䷽ʽȷ������ǰ׺
                    char shelf_prefix;
                    if (current->data.package_type == FRAGILE) {
                        shelf_prefix = 'F'; // ����Ʒר�û���
                    }
                    else {
                        shelf_prefix = get_shelf_prefix_by_shipping(current->data.method);
                    }

                    // ���ö����ķ��亯��
                    if (assign_to_shelf(&current->data, shelf_list, shelf_prefix,
                        current->data.storage_time)) {
                        success_count++;
                        current->data.status = STATUS_UNPICKED; // ����״̬Ϊ��ȡ��
                    }
                }
                current = current->next;
            }
            printf("\n�ɹ����� %d ����ȡ����\n", success_count);
            break;
        }
        case 2:
            // ����
            return;
        default:
            printf("\n��Ч��ѡ��\n");
        }

        // �������
        save_pickup_express_info(pickup_list, "pickup_express.txt");
        save_shelves_to_file(shelf_list, "shelves.txt");

        pause_program();
    }
}
// ��ݹ���������
void manage_express(Admin* admin) {
    while (1) {
        clear_screen();
        printf("\n========== ��ݹ��� ==========\n");
        printf("1. ��������\n");
        printf("2. ���ĳ�����\n");
        printf("3. δȡ����\n");
        printf("4. ��ȡ����\n");
        printf("5. ��ȡ����\n");
        printf("6. �����ϼ��˵�\n");
        printf("��ѡ�����: ");

        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1: manage_pending_express(); break;
        case 2: manage_unsent_express(); break;
        case 3: manage_unpicked_express(); break;
        case 4: manage_picked_express(); break;
        case 5: manage_mispicked_express(); break;
        case 6: return;
        default:
            printf("��Ч��ѡ��\n");
            pause_program();
        }
    }
}
