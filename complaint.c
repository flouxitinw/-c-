#include "cainiao.h"

//--------------------�û�Ͷ��ģ��--------------------
//�ļ��˷���Ͷ�ߣ���Ϊ�⳥�Ǹ��ļ��˵�
// ֻ���мĳ���ݲ����ύͶ��
// ��ʧ������

// ����Ψһ��Ͷ��ID
int generate_complaint_id(ExpressNode* head) {
    int max_id = 0;// ��ʼ��IDΪ0
    ExpressNode* current = head;

    while (current != NULL) {
        if (current->data.complaint_id > max_id) {// �����е�Ͷ��id�ҵ������ID
            max_id = current->data.complaint_id;// �������ֵ
        }
        current = current->next;
    }

    return max_id + 1; // �������ID+1��Ϊ��ID
}

// ========== �ύͶ�� ==========
/*��ʾ��Ͷ�߿�ݡ�ѡ���ݡ�ѡ��Ͷ�����͡���¼Ͷ����Ϣ�ͱ�������*/
void submit_complaint(User* user) {
    clear_screen();
    printf("\n========== �ύͶ�� ==========\n");

    // ��ʾ�û��Ѽĳ��Ŀ��//��ʾ�û���Ͷ�ߵĿ���б�
    printf("�����Ѽĳ�����б�:\n");
    int count = 0;
    ExpressNode* current = send_list;
    while (current != NULL) {
        // ����Ƿ����ڵ�ǰ�û���״̬Ϊ"�Ѽĳ�"
        if (strcmp(current->data.sender_phone, user->phone) == 0 &&
            current->data.status == STATUS_SENT) {

            printf("%d. ��ݵ���: %s | �ռ���: %s (%s) | �ĳ�ʱ��: %s",
                ++count, current->data.tracking_num,
                current->data.receiver_name, current->data.receiver_phone,
                ctime(&current->data.create_time));
        }
        current = current->next;
    }

    if (count == 0) {
        printf("��û���Ѽĳ��Ŀ�ݿ���Ͷ�ߡ�\n");
        pause_program();
        clear_input_buffer();
        return;
    }

    // ѡ��ҪͶ�ߵĿ��
    char tracking_num[20];
    printf("\n������ҪͶ�ߵĿ�ݵ���: ");
    scanf_s("%19s", tracking_num, (unsigned)_countof(tracking_num));
    clear_input_buffer();

    // ���ҿ��
    Express* express = NULL;
    current = send_list;
    while (current != NULL) {
        // ��鵥�ź��ֻ����Ƿ�ƥ��
        if (strcmp(current->data.tracking_num, tracking_num) == 0 &&
            strcmp(current->data.sender_phone, user->phone) == 0) {
            express = &current->data;
            break;
        }
        current = current->next;
    }

    if (express == NULL) {
        printf("δ�ҵ�ƥ��Ŀ�ݼ�¼��\n");
        pause_program();
        clear_input_buffer();
        return;
    }

    // ѡ��Ͷ������
    printf("\n��ѡ��Ͷ������:\n");
    printf("1. ������ʧ\n");
    printf("2. ������\n");
    int type_choice = input_int_in_range("������ѡ��(1-2): ", 1, 2);

    // ����Ͷ����Ϣ
    express->complaint_id = generate_complaint_id(send_list);// ����Ͷ��ID
    express->complaint_type = (type_choice == 1) ? COMPLAINT_LOST : COMPLAINT_DAMAGED;
    express->complaint_time = time(NULL);// ����Ͷ��ʱ��Ϊ��ǰʱ��
    express->status = (type_choice == 1) ? STATUS_LOST : STATUS_DAMAGED;

    // �������
    save_send_express_info(send_list, SEND_FILE);

    printf("\nͶ���ύ�ɹ���Ͷ��ID: %d\n", express->complaint_id);
    pause_program();
    clear_input_buffer();
}

// ��ʾͶ������
void display_complaint(Express* express) {
    printf("\n============== Ͷ������ ==============\n");
    printf("Ͷ��ID: %d\n", express->complaint_id);
    printf("��ݵ���: %s\n", express->tracking_num);
    printf("Ͷ������: %s\n",
        express->complaint_type == COMPLAINT_LOST ? "������ʧ" : "������");
    printf("Ͷ��ʱ��: %s", ctime(&express->complaint_time));

    if (express->status == STATUS_ABNORMAL_RESOLVED) {//�쳣�Ѵ���
        printf("\n[������]\n");
        printf("����ʱ��: %s", ctime(&express->resolve_time));
        printf("������: %s\n", express->complaint_resolution);
        printf("�⳥���: %.2fԪ\n", express->compensation);
    }
    else {
        printf("\n״̬: ������\n");
    }
    printf("=======================================\n");
}

// ========== �鿴�ҵ�Ͷ�ߣ��û��� ==========
void view_user_complaints(User* user) {
    clear_screen();
    printf("\n========== �ҵ�Ͷ�߼�¼ ==========\n");

    int count = 0;
    ExpressNode* current = send_list;

    while (current != NULL) {
        if (strcmp(current->data.sender_phone, user->phone) == 0 &&
            (current->data.status == STATUS_LOST ||
                current->data.status == STATUS_DAMAGED ||
                current->data.status == STATUS_ABNORMAL_RESOLVED)) {
            printf("%d. Ͷ��ID: %d | ��ݵ���: %s | ����: %s | ״̬: %s\n",
                ++count, current->data.complaint_id,
                current->data.tracking_num,
                current->data.complaint_type == COMPLAINT_LOST ? "��ʧ" : "��",
                get_status_text(current->data.status));
        }
        current = current->next;
    }

    if (count == 0) {
        printf("��û���ύ���κ�Ͷ�ߡ�\n");
        pause_program();
        clear_input_buffer();
        return;
    }

    printf("\n����ѡ��:\n");
    printf("1. �鿴Ͷ������\n");
    printf("2. ����\n");
    printf("��ѡ��: ");

    int choice = input_int_in_range("", 1, 2);
    if (choice == 1) {
        int complaint_id;
        printf("������Ҫ�鿴��Ͷ��ID: ");
        scanf_s("%d", &complaint_id);
        clear_input_buffer();

        // ����Ͷ��
        current = send_list;
        while (current != NULL) {
            if (current->data.complaint_id == complaint_id &&
                strcmp(current->data.sender_phone, user->phone) == 0) {
                display_complaint(&current->data);// ��ʾͶ������
                break;
            }
            current = current->next;
        }

        if (current == NULL) {
            printf("δ�ҵ�ָ����Ͷ�߼�¼��\n");
        }
    }

    pause_program();
    clear_input_buffer();
}




//--------------------����ԱͶ�ߴ���ģ��--------------------
// ��ʾͶ�߿�ݼ�Ҫ��Ϣ
void display_abnormal_express_brief(Express* express) {
    printf("����: %s | �ռ���: %s(%s) | ����: %s\n",
        express->tracking_num,
        express->receiver_name,
        express->receiver_phone,
        get_status_text(express->status));
    printf("Ͷ��ID: %d | Ͷ������: %s | ���۽��: ��%.2f\n",
        express->complaint_id,
        express->complaint_type == COMPLAINT_LOST ? "��ʧ" : "��",
        express->insured_value);
    printf("Ͷ��ʱ��: %s", ctime(&express->complaint_time));
    printf("--------------------------------------------------------------\n");
}


// ��ʾͶ�߿����ϸ��Ϣ
void display_abnormal_express_detail(Express* express) {
    printf("\n============= �쳣������� ============\n");
    printf("��ݵ���: %s\n", express->tracking_num);
    printf("�ռ���: %s | �绰: %s\n", express->receiver_name, express->receiver_phone);
    printf("��������: %s | ���۽��: ��%.2f\n",
        get_package_type_name(express->package_type),
        express->insured_value);
    printf("Ͷ��ID: %d | ״̬: %s\n", express->complaint_id, get_status_text(express->status));
    printf("Ͷ������: %s\n", express->complaint_type == COMPLAINT_LOST ? "������ʧ" : "������");
    printf("Ͷ��ʱ��: %s", ctime(&express->complaint_time));

    if (express->status == STATUS_ABNORMAL_RESOLVED) {
        printf("\n[�Ѵ�����Ϣ]\n");
        printf("����ʱ��: %s", ctime(&express->resolve_time));
        printf("������: %s\n", express->complaint_resolution);
        printf("�⳥���: ��%.2f\n", express->compensation);
    }
    printf("=======================================\n");
}

// �����⳥���
/*��ʧ�����۽��ȫ���⳥   �𻵰����۽���50%�⳥*/
double calculate_compensation(const Express* express) {
    if (express->complaint_type == COMPLAINT_LOST) {
        // ��ʧ�����۽��ȫ���⳥
        return express->insured_value;
    }
    else {
        // �𻵰����۽���50%�⳥
        return express->insured_value * 0.5;
    }
}

// ������Ͷ�߿��
void process_single_abnormal_express(Express* express) {
    if (express->status != STATUS_LOST && express->status != STATUS_DAMAGED) {
        printf("�ÿ�ݲ���Ҫ������Ѵ���!\n");
        return;
    }

    double compensation = calculate_compensation(express);
    printf("\n�����⳥���: ��%.2f\n", compensation);

    printf("������ʵ���⳥���(��ǰ����:��%.2f): ", compensation);
    scanf_s("%lf", &compensation);
    clear_input_buffer();

    char resolution[256];
    printf("�����봦��˵��: ");
    fgets(resolution, sizeof(resolution), stdin);
    resolution[strcspn(resolution, "\n")] = '\0';

    express->status = STATUS_ABNORMAL_RESOLVED;
    express->compensation = compensation;
    strncpy_s(express->complaint_resolution, sizeof(express->complaint_resolution),
        resolution, _TRUNCATE);
    express->resolve_time = time(NULL);

    printf("\n����ɹ�! ���⳥ ��%.2f\n", compensation);
}

// ========== �����Ų�ѯ��������Ͷ�߿�� ==========
/*��ʾδ����Ͷ����ϸ��Ϣ*/
void query_and_process_by_tracking() {
    char tracking_num[20];
    printf("\n������Ҫ��ѯ�Ŀ�ݵ���: ");
    scanf_s("%19s", tracking_num, (unsigned)_countof(tracking_num));
    clear_input_buffer();

    Express* express = find_express_by_tracking(send_list, tracking_num);
    if (express == NULL) {
        printf("δ�ҵ��ÿ��!\n");
        return;
    }

    if (express->status != STATUS_LOST && express->status != STATUS_DAMAGED) {
        printf("�ÿ�ݲ����쳣״̬!\n");
        return;
    }

    display_abnormal_express_detail(express);

    printf("\n�Ƿ�Ҫ�����Ͷ��? (1-���� 0-ȡ��): ");
    int choice;
    scanf_s("%d", &choice);
    clear_input_buffer();

    if (choice == 1) {
        process_single_abnormal_express(express);
        save_send_express_info(send_list, SEND_FILE);
    }
}

// ========== ������������Ͷ�߿�� ==========
void batch_process_all_abnormal_express() {
    ExpressNode* current = send_list;
    int processed_count = 0; //�Ѵ�������
    double total_compensation = 0.0; //���⳥���

    while (current != NULL) {
        if (current->data.status == STATUS_LOST || current->data.status == STATUS_DAMAGED) {
            printf("\n���ڴ�����: %s\n", current->data.tracking_num);
            double compensation = calculate_compensation(&current->data);
            printf("�Զ������⳥���: ��%.2f\n", compensation);

            current->data.status = STATUS_ABNORMAL_RESOLVED;
            current->data.compensation = compensation;
            snprintf(current->data.complaint_resolution,
                sizeof(current->data.complaint_resolution),
                "ϵͳ���������⳥���:��%.2f", compensation);
            current->data.resolve_time = time(NULL);

            processed_count++;
            total_compensation += compensation;

            printf("�Ѵ���: %s �⳥ ��%.2f\n", current->data.tracking_num, compensation);
        }
        current = current->next;
    }

    if (processed_count > 0) {
        printf("\n�����������! ������ %d ���쳣��ݣ��ܼ��⳥ ��%.2f\n",
            processed_count, total_compensation);
        save_send_express_info(send_list, SEND_FILE);
    }
    else {
        printf("\n��ǰû����Ҫ������쳣���!\n");
    }
}

// ����ԱͶ�ߴ���������
void admin_handle_complaints() {
    while (1) {
        clear_screen();
        printf("\n========== ����ԱͶ�ߴ��� ==========\n");

        int total_abnormal = 0, total_lost = 0, total_damaged = 0;
        ExpressNode* current = send_list;
        while (current != NULL) {
            if (current->data.status == STATUS_LOST) {
                total_abnormal++;
                total_lost++;
            }
            else if (current->data.status == STATUS_DAMAGED) {

                total_abnormal++;
                total_damaged++;
            }
            current = current->next;
        }

        printf("��ǰͶ����Ϣ����: %d (��ʧ:%d ��:%d)\n\n",
            total_abnormal, total_lost, total_damaged);

        printf("1. �鿴����Ͷ����Ϣ\n");
        printf("2. �����Ų�ѯ���������\n");
        printf("3. ������������Ͷ��\n");
        printf("0. �����ϼ��˵�\n");
        printf("��ѡ�����: ");

        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1: {
            clear_screen();
            printf("\n======================== ����Ͷ���б� ========================\n");
            int count = 0;
            current = send_list;
            while (current != NULL) {
                if (current->data.status == STATUS_LOST ||
                    current->data.status == STATUS_DAMAGED || current->data.status == STATUS_ABNORMAL_RESOLVED) {
                    display_abnormal_express_brief(&current->data);
                    count++;
                }
                current = current->next;
            }
            if (count == 0) {
                printf("��ǰû��Ͷ����Ϣ!\n");
            }
            pause_program();
            clear_input_buffer();
            break;
        }
        case 2:
            query_and_process_by_tracking();
            pause_program();
            clear_input_buffer();
            break;
        case 3:
            batch_process_all_abnormal_express();
            pause_program();
            clear_input_buffer();
            break;
        case 0:
            return;
        default:
            printf("\n��Ч��ѡ��!\n");
            pause_program();
            clear_input_buffer();
        }
    }
}

