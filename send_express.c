// cainiao.c
#include "cainiao.h"
//--------------------��������--------------------
//��ȡ��Ʒ��������
const char* get_package_type_name(PackageType type) {
    const char* names[] = {
        "����Ʒ", "ʳƷ", "�ļ�", "����",
        "�����Ʒ", "��", "����", "����",
        "��ȼ/�ױ�/�ж���Ʒ", "������Ʒ", "����",
        "����Ʒ", "����"
    };
    return names[type];
}
// ����Ƿ�����Ч�� 11 λ�ֻ���
int is_valid_phone(const char* phone) {
    if (strlen(phone) != 11) {
        return 0;
    }
    for (int i = 0; i < 11; i++) {
        if (!isdigit(phone[i])) {
            return 0;
        }
    }
    return 1;
}
// ��ȡ�û����뷶Χ�ڵ�����
int input_int_in_range(const char* prompt, int min, int max) {
    int choice;
    while (1) {
        printf("%s", prompt);
        if (scanf_s("%d", &choice) != 1) {  // ����Ƿ�ɹ���ȡ����
            printf("������Ч�����������룡\n");
            while (getchar() != '\n');  // ������뻺����
            continue;
        }
        if (choice < min || choice > max) {  // ����Ƿ��ڷ�Χ��
            printf("���������� %d �� %d ֮�䣬���������룡\n", min, max);
            continue;
        }
        break;  // ������ȷ���˳�ѭ��
    }
    return choice;
}
// ����һ���������������� > 0��
double input_positive_double(const char* prompt) {
    char input[256]; // �㹻��Ļ������洢����

    while (1) {
        printf("%s", prompt);
        fflush(stdout);//������ʾ��Ϣ�ӳ���ʾ

        // 1. ��ȡ����������
        if (fgets(input, sizeof(input), stdin) == NULL) {
            clear_input_buffer();
            printf("����������������룡\n");
            continue;
        }

        // 2. ȥ�����з�
        input[strcspn(input, "\n")] = '\0';

        // 3. ����Ƿ�Ϊ������
        if (input[0] == '\0') {
            printf("���벻��Ϊ�գ�\n");
            continue;
        }

        // 4. ��֤�Ƿ�Ϊ��Ч���ָ�ʽ
        int valid = 1;
        int dot_count = 0;

        for (int i = 0; input[i] != '\0'; i++) {
            if (i == 0 && input[i] == '-') {
                valid = 0; // ��������
                break;
            }

            if (input[i] == '.') {
                dot_count++;
                if (dot_count > 1) { // ���С����
                    valid = 0;
                    break;
                }
            }
            else if (!isdigit((unsigned char)input[i])) {
                valid = 0;
                break;
            }
        }

        if (!valid) {
            printf("��������Ч����������12.34����\n");
            continue;
        }

        // 5. ת��Ϊdouble����֤�Ƿ�Ϊ��
        char* endptr;
        double value = strtod(input, &endptr);

        if (endptr == input || *endptr != '\0') {
            printf("ת��ʧ�ܣ����������ʽ��\n");
            continue;
        }

        if (value <= 0) {
            printf("����������0��\n");
            continue;
        }

        return value; // ��֤ͨ��
    }
}
void input_phone(char* phone, size_t size, const char* prompt) {
    char input[256]; // ��ʱ�洢���ⳤ������

    while (1) {
        // ��һ������ȡԭʼ����
        printf("%s", prompt);
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            clear_input_buffer();
            printf("������������ԣ�\n");
            continue;
        }

        // �ڶ�����ȥ�����з�
        input[strcspn(input, "\n")] = '\0';

        // ����������֤�����Ƿ�Ϊ11
        if (strlen(input) != 11) {
            printf("�ֻ��ű���Ϊ11λ����ǰ������%zuλ\n", strlen(input));
            continue;
        }

        // ���Ĳ�����֤�Ƿ�ȫΪ����
        int is_valid = 1;
        for (int i = 0; i < 11; i++) {
            if (!isdigit((unsigned char)input[i])) {
                is_valid = 0;
                break;
            }
        }

        if (!is_valid) {
            printf("�ֻ���ֻ�ܰ������֣�\n");
            continue;
        }

        // ���岽����ȫ���Ƶ����������
        strncpy(phone, input, size - 1);
        phone[size - 1] = '\0';
        return;
    }
}
void safe_input(char* buf, size_t size, const char* prompt) {
    int input_valid = 0;
    while (!input_valid) {
        printf("%s", prompt);
        fflush(stdout);
        clear_input_buffer();
        if (fgets(buf, size, stdin)) {
            // ����Ƿ��ȡ�������У���⻻�з��Ƿ���ڣ�
            char* newline = strchr(buf, '\n');
            if (newline) {
                *newline = '\0';  // ���������ȥ�����з�
                // ��������Ƿ�Ϊ�գ�����ֻ����ո�
                if (strlen(buf) == 0) {
                    printf("���벻��Ϊ�գ����������롣\n");
                }
                else {
                    input_valid = 1;  // ������Ч
                }
            }
            else {
                // ���1���û����볬����������С
                clear_input_buffer();
                printf("�������볤�Ȳ��ܳ���%zu�ַ���\n", size - 1);

                // ���2��EOF��������Ctrl+Z/D��
                if (feof(stdin)) {
                    clearerr(stdin);
                    printf("��⵽������ֹ������������\n");
                }
            }
        }
        else {
            // ����fgetsʧ���������������
            clear_input_buffer();
            if (feof(stdin)) {
                clearerr(stdin);
                printf("��⵽������ֹ������������\n");
            }
            else {
                printf("�������������\n");
            }
        }
    }
}
//--------------------�Ŀ�ݺ��ĺ���--------------------
void send_express(User* user) {
    Express* new_express = (Express*)malloc(sizeof(Express));
    if (!new_express) {
        perror("�ڴ����ʧ��");
        return;
    }
    memset(new_express, 0, sizeof(Express)); // ��ʼ������

    new_express->create_time = time(NULL);
    printf("== = �Ŀ����Ϣ¼�� == =\n");
    // 1. �ļ�����Ϣ
    printf("\n[�ļ�����Ϣ]\n");
    printf("����: %s\n", user->username);
    printf("�绰: %s\n\n", user->phone);

    strncpy(new_express->sender_name, user->username, sizeof(new_express->sender_name) - 1);
    strncpy(new_express->sender_phone, user->phone, sizeof(new_express->sender_phone) - 1);

    // 2. �ռ�����Ϣ

    printf("[�ռ�����Ϣ]\n");
    safe_input(new_express->receiver_name, sizeof(new_express->receiver_name), "������");
    input_phone(new_express->receiver_phone, sizeof(new_express->receiver_phone), "�绰��");

    if (strcmp(new_express->sender_phone, new_express->receiver_phone) == 0 &&
        strcmp(new_express->sender_name, new_express->receiver_name) != 0) {
        printf("\n���棺һ���ֻ��Ž��ɵ�¼һ���˻�\n");
        clear_input_buffer();  // ��������뻺����
        char ch = getchar();             // �ȴ��û��������
        free(new_express);
        return;                // �������˵�
    }

    // ������Ϣ
    //�������������
    new_express->weight = input_positive_double("����������ǧ��)��");
    new_express->volume = input_positive_double("�������(��������)��");
    if (new_express->weight > 50 || new_express->volume >= 3000000) {
        printf("�������󲻿��ʼ�");
        printf("��������������˵�...");
        clear_input_buffer();  // ����ջ�����
        char ch = getchar();             // �ȴ��û�����
        free(new_express);     // �ͷ��ѷ�����ڴ�
        return;                // ���ص����ú�����ͨ�������˵���
    }
    // ��Ʒ����ѡ��
    printf("��ѡ����Ʒ���ͣ�\n");
    for (int i = 0; i < 13; i++) {
        printf("%d. %s", i, get_package_type_name((PackageType)i));
    }
    int type_choice = input_int_in_range("������ѡ�0-12����", 0, 12);
    if (type_choice >= 5 && type_choice <= 8) {
        printf("���ݹ������������ֹ�ʼ�");
        printf("��������������˵�...");
        clear_input_buffer();  // ��������뻺����
        char ch = getchar();             // �ȴ��û��������
        free(new_express);
        return;                // �������˵�

    }
    new_express->package_type = (PackageType)(type_choice);
    if (new_express->package_type == FRESH) {
        printf("����ѡ����������\n ");
    }
    if (new_express->package_type == VALUABLES || new_express->package_type == DIGITAL) {
        printf("���鿪�����۷���\n");
    }
    if (new_express->weight > 20.0) {
        printf("����ѡ�����ʼķ�ʽ\n");
    }


    // ���䷽ʽѡ��
    printf("��ѡ�����䷽ʽ\n");
    printf("0. ��ͨ���\n1. ����ʼ�\n2. �����ʼ�\n");
    int method_choice = input_int_in_range("������ѡ�0-2����", 0, 2);
    new_express->method = (ShippingMethod)method_choice;
    if (new_express->method == SHIPPING_BULK && new_express->weight < 20) {
        printf("���棺����ʼ�Ҫ����������20ǧ�ˣ�\n");
        clear_input_buffer();  // ��������뻺����
        char ch = getchar();             // �ȴ��û��������
        free(new_express);
        return;                // �������˵�          
    }
    // ����ѡ��
    printf("��ѡ��������ͣ�\n");
    printf("0. ʡ��\n1. ʡ��\n2. ����\n3. �۰�̨\n");
    int region_choice = input_int_in_range("������ѡ�0-3����", 0, 3);
    new_express->region = (RegionType)region_choice;


    // ������Ϣ
    clear_input_buffer();
    new_express->insured_value = input_positive_double("��Ʒ��ֵ��");
    new_express->is_insured = input_int_in_range("�Ƿ񱣼ۣ�0-�� 1-�ǣ���", 0, 1);
    //����ʱЧ
    printf("\n��ѡ������ʱЧ��\n");
    printf("0. ��죨��ͨʱЧ��\n");
    printf("1. �ؿ죨�Ӽ���\n");
    int choice = input_int_in_range("������ѡ�0-1����", 0, 1);
    new_express->express_type = (DeliveryType)choice;
    // ����Ĭ��ֵ
    new_express->status = STATUS_UNSENT;
    strcpy(new_express->tracking_num, "0");
    new_express->fee = calculate_shipping_fee(new_express, user);
    printf("�û�[%s] �ȼ�[%d] Ӧ����: %.2f\n",
        user->username, user->level, new_express->fee);

    switch (user->payment_mode) {
    case PAY_CASH:
        printf("���ֳ�֧��: %.2f\n", new_express->fee);
        printf("�Ƿ�ȷ��֧������0-�� 1-�ǣ�");
        int confirm = input_int_in_range("", 0, 1);
        if (confirm == 0) {
            printf("֧��δ��ɣ���ݼĳ�ʧ��\n");
            free(new_express);
            return;
        }
        else printf("֧���ɹ�����ݼĳ��ɹ�\n");
        break;

    case PAY_MONTHLY:
        printf("�Ѽ����½��˻� \n");

        break;
    case PAY_CONTRACT:
        printf("����Э�����ڽ���\n");
        break;
    }
    // ��������ڵ�
    ExpressNode* new_node = create_express_node(new_express); // �Զ���������
    if (!new_node) {
        perror("�ڴ����ʧ��");
        free(new_express);  // �����ڵ�ʧ��ʱ�ͷ�
        return;
    }

    // ���뵽����ͷ��
    new_node->next = send_list;  // �½ڵ�ָ��ԭͷ�ڵ�
    send_list = new_node;        // ����ͷָ��ָ���½ڵ�

    // ���浽�ļ�
    save_send_express_info(send_list, "send_express.txt");
    free(new_express);  // �����ڵ�ʧ��ʱ�ͷ�
    printf("��������������˵�...");
    clear_input_buffer();  // ��������뻺����
    char ch = getchar();             // �ȴ��û��������
    return;                // �������˵�

}


void modify_express(User* user) {
    clear_screen();
    printf("\n========== �޸Ŀ����Ϣ ==========\n");

    // 1. ��ʾ�û�δ�ĳ��Ŀ���б�
    int count = 0;
    ExpressNode* current = send_list;
    ExpressNode** express_ptrs = NULL; // �洢ƥ��Ŀ�ݽڵ�ָ��
    int express_count = 0;

    // ��һ�α�����ͳ��ƥ��Ŀ������
    while (current != NULL) {
        if (strcmp(current->data.sender_phone, user->phone) == 0 &&
            current->data.status == STATUS_UNSENT) {
            express_count++;
        }
        current = current->next;
    }

    if (express_count == 0) {
        printf("��û��δ�ĳ��Ŀ�ݿ��޸ġ�\n");
        pause_program();
        return;
    }

    // �����ڴ�洢���ָ��
    express_ptrs = (ExpressNode**)malloc(express_count * sizeof(ExpressNode*));
    if (!express_ptrs) {
        perror("�ڴ����ʧ��");
        return;
    }

    // �ڶ��α����������ָ������
    current = send_list;
    int index = 0;
    while (current != NULL) {
        if (strcmp(current->data.sender_phone, user->phone) == 0 &&
            current->data.status == STATUS_UNSENT) {
            express_ptrs[index++] = current;
            printf("%3d. | �ռ���: %12s(%s)  | ��������: %12s |  ����ʱ��: %s",
                index,
                current->data.receiver_name, current->data.receiver_phone, get_package_type_name(current->data.package_type),
                ctime(&current->data.create_time));
        }
        current = current->next;
    }

    // 2. ѡ��Ҫ�޸ĵĿ��
    int choice = input_int_in_range("\n��ѡ��Ҫ�޸ĵĿ�ݱ�� (0ȡ��): ", 0, express_count);
    if (choice == 0) {
        free(express_ptrs);
        return;
    }

    Express* express = &(express_ptrs[choice - 1]->data);
    free(express_ptrs);

    // 3. ��ʾ��ǰ��Ϣ��ѡ���޸��ֶ�
    while (1) {
        clear_screen();
        printf("\n��ǰ�����Ϣ:\n");
        printf("1. �ռ�������: %s\n", express->receiver_name);
        printf("2. �ռ��˵绰: %s\n", express->receiver_phone);
        printf("3. ��������: %.2f ǧ��\n", express->weight);
        printf("4. �������: %.2f ��������\n", express->volume);
        printf("5. ��Ʒ����: %s\n", get_package_type_name(express->package_type));
        printf("6. ���䷽ʽ: %s\n",
            (express->method == SHIPPING_NORMAL) ? "��ͨ���" :
            (express->method == SHIPPING_BULK) ? "����ʼ�" : "�����ʼ�");
        printf("7. ��������: %s\n",
            (express->region == REGION_PROVINCE) ? "ʡ��" :
            (express->region == REGION_OUTOFPROVINCE) ? "ʡ��" :
            (express->region == REGION_INTERNATIONAL) ? "����" : "�۰�̨");
        printf("8. ����״̬: %s\n", express->is_insured ? "��" : "��");
        printf("9. ��Ʒ��ֵ: %.2f\n", express->insured_value);
        printf("10. ����ʱЧ: %s\n",
            (express->express_type == STANDARD_DELIVERY) ? "���" : "�ؿ�");
        printf("0. ���沢����\n");

        int field = input_int_in_range("\nѡ��Ҫ�޸ĵ��ֶ� (0-10): ", 0, 10);
        if (field == 0) break;
        char original_name[50];
        strncpy(original_name, express->receiver_name, sizeof(original_name));
        switch (field) {
        case 1: { // �ռ�������
            safe_input(express->receiver_name, sizeof(express->receiver_name), "���ռ�������: ");

            if (strcmp(express->sender_phone, express->receiver_phone) == 0 &&
                strcmp(express->sender_name, express->receiver_name) != 0) {
                printf("\n���棺�ռ�����ļ����ֻ�����ͬ��������ͬ��\n");
                printf("ԭ�ռ�������: %s\n", original_name);

                // �ָ�ԭʼֵ
                strncpy(express->receiver_name, original_name, sizeof(express->receiver_name));

                printf("������������޸�...");
                clear_input_buffer();
                char ch = getchar();
                continue;
            }
            break;
        }

        case 2: { // �ռ��˵绰
            char original_phone[12];
            strncpy(original_phone, express->receiver_phone, sizeof(original_phone));

            clear_input_buffer();
            input_phone(express->receiver_phone, sizeof(express->receiver_phone), "���ռ��˵绰: ");

            if (strcmp(express->sender_phone, express->receiver_phone) == 0 &&
                strcmp(express->sender_name, express->receiver_name) != 0) {
                printf("\n���棺һ���ֻ��Ž��ɵ�¼һ���˻�\n");
                printf("ԭ�ռ��˵绰: %s\n", original_phone);

                // �ָ�ԭʼֵ
                strncpy(express->receiver_phone, original_phone, sizeof(express->receiver_phone));

                printf("������������޸�...");
                clear_input_buffer();
                char ch = getchar();
                continue;
            }
            break;
        }

        case 3: { // ����
            double original_weight = express->weight;

            clear_input_buffer();
            express->weight = input_positive_double("�°������� (ǧ��): ");

            if (express->weight > 50) {
                printf("���棺������������50ǧ�ˣ��޷��ʼģ�\n");
                printf("ԭ����: %.2fǧ��\n", original_weight);

                // �ָ�ԭʼֵ
                express->weight = original_weight;

                printf("������������޸�...");
                clear_input_buffer();
                char ch = getchar();
                continue;
            }
            break;
        }

        case 4: { // ���
            double original_volume = express->volume;

            clear_input_buffer();
            express->volume = input_positive_double("�°������ (��������): ");

            if (express->volume >= 3000000) {
                printf("���棺������������޷��ʼģ�\n");
                printf("ԭ���: %.2f��������\n", original_volume);

                // �ָ�ԭʼֵ
                express->volume = original_volume;

                printf("������������޸�...");
                clear_input_buffer();
                char ch = getchar();
                continue;
            }
            break;
        }

        case 5: { // ��Ʒ����
            PackageType original_type = express->package_type;

            printf("��ѡ����Ʒ���ͣ�\n");
            for (int i = 0; i < 13; i++) {
                printf("%d. %s\n", i, get_package_type_name((PackageType)i));
            }
            int new_type = input_int_in_range("ѡ�� (0-12): ", 0, 12);

            if (new_type >= 5 && new_type <= 8) {
                printf("���ݹ������������ֹ�ʼĴ�����Ʒ��\n");
                printf("ԭ��Ʒ����: %s\n", get_package_type_name(original_type));

                // �ָ�ԭʼֵ
                express->package_type = original_type;

                printf("������������޸�...");
                clear_input_buffer();
                char ch = getchar();
                continue;
            }
            express->package_type = (PackageType)new_type;
            break;
        }
        case 6: // ���䷽ʽ
            printf("��ѡ�����䷽ʽ��\n");
            printf("0. ��ͨ���\n1. ����ʼ�\n2. �����ʼ�\n");
            express->method = (ShippingMethod)input_int_in_range("ѡ�� (0-2): ", 0, 2);
            break;
            if (express->method == SHIPPING_BULK && express->weight < 20) {
                printf("���棺����ʼ�Ҫ����������20ǧ�ˣ�\n");
                printf("������������޸�...");
                clear_input_buffer();
                char ch = getchar();
                continue;
            }
        case 7: // ����
            printf("��ѡ��������ͣ�\n");
            printf("0. ʡ��\n1. ʡ��\n2. ����\n3. �۰�̨\n");
            express->region = (RegionType)input_int_in_range("ѡ�� (0-3): ", 0, 3);
            break;
        case 8: // ����
            express->is_insured = input_int_in_range("�Ƿ񱣼� (0-�� 1-��): ", 0, 1);
            break;
        case 9: // ��Ʒ��ֵ
            clear_input_buffer();
            express->insured_value = input_positive_double("����Ʒ��ֵ: ");
            break;
        case 10: // ����ʱЧ
            printf("��ѡ������ʱЧ��\n");
            printf("0. ��죨��ͨʱЧ��\n");
            printf("1. �ؿ죨�Ӽ���\n");
            express->express_type = (DeliveryType)input_int_in_range("ѡ�� (0-1): ", 0, 1);
        }
    }
    express->fee = calculate_shipping_fee(express, user);
    // 4. �����޸�
    save_send_express_info(send_list, SEND_FILE);
    printf("\n�޸��ѱ��棡\n");
    pause_program();
}


void delete_express(User* user) {
    clear_screen();
    printf("\n========== ȡ���ʼ� ==========\n");

    // 1. ��������ͳ�Ʋ���ʾ����δ�ĳ��Ŀ��
    ExpressNode* current = send_list;
    int item_count = 0;

    // ��һ�α�����ͳ����������ʾ
    printf("����δ�ĳ�����б�\n");
    while (current != NULL) {
        if (strcmp(current->data.sender_phone, user->phone) == 0 &&
            current->data.status == STATUS_UNSENT) {
            item_count++;
            printf("%2d. �ռ���: %s (%s) \n    ��������: %12s | ����ʱ��: %s",
                item_count,
                current->data.receiver_name,
                current->data.receiver_phone,
                get_package_type_name(current->data.package_type),
                ctime(&current->data.create_time));
        }
        current = current->next;
    }

    if (item_count == 0) {
        printf("��û��δ�ĳ��Ŀ�ݿ�ȡ����\n");
        pause_program();
        return;
    }

    // 2. ѡ��Ҫɾ���Ŀ��
    int choice = input_int_in_range("\n��ѡ��Ҫȡ���Ŀ�ݱ�� (0����): ", 0, item_count);
    if (choice == 0) {
        return;
    }

    // 3. ��λ��ѡ��Ŀ��
    current = send_list;
    ExpressNode* prev = NULL;
    int count = 0;

    while (current != NULL) {
        if (strcmp(current->data.sender_phone, user->phone) == 0 &&
            current->data.status == STATUS_UNSENT) {
            count++;

            if (count == choice) {
                // ��ʾ���鲢ȷ��
                printf("\n����ȡ���Ŀ����Ϣ:\n");
                printf("�ռ���: %s (%s)\n", current->data.receiver_name, current->data.receiver_phone);
                printf("����: %.2fǧ�� | ���: %.2f��������\n", current->data.weight, current->data.volume);
                printf("��Ʒ����: %s\n", get_package_type_name(current->data.package_type));
                printf("����ʱ��: %s", ctime(&current->data.create_time));

                int confirm = input_int_in_range("\nȷ��ȡ����(0-���� 1-ȷ��ȡ��): ", 0, 1);
                if (confirm == 1) {
                    // ��������ɾ���ڵ�
                    if (prev == NULL) {  // ɾ��ͷ�ڵ�
                        send_list = current->next;
                    }
                    else {  // ɾ���м��β�ڵ�
                        prev->next = current->next;
                    }

                    free(current);  // �ͷ��ڴ�
                    save_send_express_info(send_list, SEND_FILE);  // ���浽�ļ�
                    printf("\nȡ���ʼĳɹ���\n");
                }
                else {
                    printf("\n�ѱ����ÿ�ݡ�\n");
                }
                pause_program();
                return;
            }
        }
        prev = current;
        current = current->next;
    }

    printf("ѡ����Ч�������²�����\n");
    pause_program();
}


void find_pending_express(User* user) {
    clear_screen();
    printf("\n========== ���ĳ���ݲ�ѯ ==========\n");

    // 1. ѡ���ѯ��ʽ
    printf("\n��ѡ���ѯ��ʽ��\n");
    printf("1. ���ռ���������ѯ\n");
    printf("2. ���ռ����ֻ��Ų�ѯ\n");
    printf("3. ����Ʒ���Ͳ�ѯ\n");
    printf("0. ����\n");

    int choice = input_int_in_range("������ѡ��(0-3): ", 0, 3);
    if (choice == 0) return;

    // 2. ����ѡ���ȡ��ѯ����
    char search_name[50] = { 0 };
    char search_phone[12] = { 0 };
    int search_type = -1;

    switch (choice) {
    case 1: // ��������ѯ
        safe_input(search_name, sizeof(search_name), "�������ռ��������ؼ���: ");
        break;

    case 2: // ���ֻ��Ų�ѯ
        clear_input_buffer();
        input_phone(search_phone, sizeof(search_phone), "�������ռ��������ֻ���(11λ): ");
        break;

    case 3: // ����Ʒ���Ͳ�ѯ
        printf("\n��ѡ����Ʒ���ͣ�\n");
        for (int i = 0; i < 13; i++) {
            printf("%d. %s\n", i, get_package_type_name((PackageType)i));
        }
        search_type = input_int_in_range("��������Ʒ���ͱ��(0-12): ", 0, 12);
        break;
    }

    // 3. ��������ʾƥ����
    ExpressNode* current = send_list;
    int total_matches = 0;

    printf("\n=== ��ѯ��� ===\n");
    while (current != NULL) {
        if (strcmp(current->data.sender_phone, user->phone) == 0 &&
            current->data.status == STATUS_UNSENT) {

            // ����ѯ����
            int match = 0;
            switch (choice) {
            case 1: // ����ƥ��
                match = strstr(current->data.receiver_name, search_name) != NULL;
                break;
            case 2: // �ֻ���ƥ��
                match = strcmp(current->data.receiver_phone, search_phone) == 0;
                break;
            case 3: // ��Ʒ����ƥ��
                match = current->data.package_type == search_type;
                break;
            }

            if (match) {
                total_matches++;
                printf("\n%d.\n", total_matches);
                printf("�ռ���: %-20s\n�绰: %s\n",
                    current->data.receiver_name,
                    current->data.receiver_phone);
                printf("����: %.2fǧ��  ���: %.2f��������  ����: %s\n",
                    current->data.weight,
                    current->data.volume,
                    get_package_type_name(current->data.package_type));
                printf("����ʱ��: %s", ctime(&current->data.create_time));
            }
        }
        current = current->next;
    }

    if (total_matches == 0) {
        printf("\nδ�ҵ����������Ŀ�ݡ�\n");
        clear_input_buffer();  // ��������뻺����
        char ch = getchar();             // �ȴ��û��������
        return;                // �������˵�
    }
    else {
        printf("\n���ҵ� %d ����¼��\n", total_matches);
    }

    pause_program();
}

void show_pending_bills(User* user) {
    clear_screen();
    printf("\n========== ��֧���˵� ==========\n");

    // 1. ����û����ͣ�ֻ��VIP/��ҵ/�����̻����д�֧���˵�
    if (user->level == USER_NORMAL || user->level == USER_STUDENT) {
        printf("��ͨ�û���ѧ���û��޴�֧���˵����ָ�ģʽ��\n");
        pause_program();
        return;
    }

    // 2. ����ʱ�䷶Χ
    time_t now = time(NULL);
    struct tm timeinfo;
    localtime_s(&timeinfo, &now);

    // ������ʼʱ��Ϊ����1�ջ���1��1��
    struct tm start_tm = { 0 };
    start_tm.tm_year = timeinfo.tm_year;

    if (user->level == USER_VIP || user->level == USER_ENTERPRISE) {
        start_tm.tm_mon = timeinfo.tm_mon;  // ����1��
        printf("ͳ�Ʒ�Χ: ����(1������)�Ĵ�֧���˵�\n");
    }
    else { // USER_PARTNER
        start_tm.tm_mon = 0;  // 1��1��
        printf("ͳ�Ʒ�Χ: ����(1��1������)�Ĵ�֧���˵�\n");
    }
    start_tm.tm_mday = 1;
    time_t start_time = mktime(&start_tm);

    // 3. �������������Ҵ�֧���˵�
    ExpressNode* current = send_list;
    double total_amount = 0.0;
    int bill_count = 0;

    printf("\n%-15s %-15s %-20s %-12s %-10s\n",
        "�ռ���", "�ֻ���", "��Ʒ����", "�ĳ�ʱ��", "���");
    printf("----------------------------------------------------------------\n");

    while (current != NULL) {
        // ����Ƿ��Ǹ��û��ġ��Ѽĳ���δ֧���Ŀ��
        if (strcmp(current->data.sender_phone, user->phone) == 0 &&
            (current->data.status == STATUS_SENT || current->data.status == STATUS_UNSENT) &&
            current->data.fee > 0.01 &&  // δ֧��
            current->data.create_time >= start_time) {

            // ��ʽ��ʱ��
            char time_str[20];
            struct tm create_tm;
            localtime_s(&create_tm, &current->data.create_time);
            strftime(time_str, sizeof(time_str), "%m-%d %H:%M", &create_tm);

            // ��ȡ��Ʒ��������
            const char* type_name = get_package_type_name(current->data.package_type);

            printf("%-15s %-15s %-20s %-12s ��%-8.2f\n",
                current->data.receiver_name,
                current->data.receiver_phone,
                type_name,
                time_str,
                current->data.fee
            );

            total_amount += current->data.fee;
            bill_count++;
        }
        current = current->next;
    }

    // 4. ��ʾͳ�ƽ��
    if (bill_count == 0) {
        printf("\n��ǰû�д�֧���˵�\n");
        clear_input_buffer();  // ��������뻺����
        char ch = getchar();             // �ȴ��û��������
        return;                // �������˵�
    }
    else {
        printf("\n�ܼ�: %d ���˵�  Ӧ�����ܶ�: ��%.2f\n", bill_count, total_amount);

        // 5. ֧��ȷ��
        int choice = input_int_in_range("\n�Ƿ�����֧����(1-ȷ��֧�� 0-ȡ��): ", 0, 1);
        if (choice == 1) {
            // ���Ϊ��֧��
            current = send_list;
            while (current != NULL) {
                if (strcmp(current->data.sender_phone, user->phone) == 0 &&
                    (current->data.status == STATUS_SENT || current->data.status == STATUS_UNSENT) &&
                    current->data.fee > 0.01 &&
                    current->data.create_time >= start_time) {
                    current->data.fee = 0.0;  // ����Ϊ��֧��
                }
                current = current->next;
            }
            // ��������
            save_send_express_info(send_list, SEND_FILE);
            printf("\n֧���ɹ����ѽ��� ��%.2f\n", total_amount);
        }
    }
    pause_program();
}


void send_express_menu(User* user) {
    while (1) {

        clear_screen();
        printf("\n========== �Ŀ�ݲ˵� ==========\n");
        printf("�û�: %s (%s)\n", user->username,
            user->level == USER_NORMAL ? "��ͨ�û�" :
            user->level == USER_STUDENT ? "ѧ���û�" :
            user->level == USER_VIP ? "VIP�û�" :
            user->level == USER_ENTERPRISE ? "��ҵ�û�" : "�����̻�");

        // ��̬�˵�ѡ��
        printf("\n1. �ĳ��¿��\n");
        printf("2. �޸Ĵ��ĳ����\n");
        printf("3. ɾ�����ĳ����\n");
        printf("4. ��ѯ���ĳ����\n");
        // �����û�������ʾ֧��ѡ��
        if (user->level != USER_NORMAL && user->level != USER_STUDENT) {
            printf("5. ֧���˵�\n");
            printf("6. �������˵�\n");
            printf("\n��ѡ����� (1-6): ");
        }
        else {
            printf("5. �������˵�\n");
            printf("\n��ѡ����� (1-5): ");
        }

        // ��ȡ�û�ѡ��
        int max_choice = (user->level != USER_NORMAL && user->level != USER_STUDENT) ? 6 : 5;
        int choice = input_int_in_range("", 1, max_choice);

        switch (choice) {
        case 1:  // �ĳ��¿��
            send_express(user);
            break;

        case 2:  // �޸Ŀ��
            modify_express(user);
            break;

        case 3:  // ɾ�����
            delete_express(user);
            break;

        case 4:  // ��ѯ���ĳ����
            find_pending_express(user);
            break;

        case 5:  // ֧���򷵻�
            if (user->level != USER_NORMAL && user->level != USER_STUDENT) {
                show_pending_bills(user);
            }
            else {
                return;  // ��ͨ�û�ֱ�ӷ���
            }
            break;

        case 6:  // ��VIP/��ҵ/�����̻��ɼ�
            return;

        default:
            printf("��Чѡ�\n");
            pause_program();
        }
    }
}