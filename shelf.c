#include "cainiao.h"

// ��������
Shelf* create_shelf(const char* shelf_id, int max_capacity) {
    Shelf* new_shelf = (Shelf*)malloc(sizeof(Shelf));
    if (!new_shelf) {
        perror("Failed to create shelf");
        return NULL;
    }
    strncpy_s(new_shelf->shelf_id, sizeof(new_shelf->shelf_id), shelf_id, _TRUNCATE);
    new_shelf->shelf_id[sizeof(new_shelf->shelf_id) - 1] = '\0';
    new_shelf->max_capacity = max_capacity;
    new_shelf->current_count = 0;
    new_shelf->pickup_list = NULL;
    new_shelf->next = NULL;
    return new_shelf;
}

// ���ӻ��ܣ�ͷ�巨��
void add_shelf(Shelf** head, Shelf* new_shelf) {
    if (!head || !new_shelf) return;
    new_shelf->next = *head;
    *head = new_shelf;
}

//ɾ������
bool delete_shelf(Shelf** head, const char* shelf_id) {
    // ������Ч�Լ�飨�ⲿ��Ӧ�÷��ڵ��ú����У�
    if (!head || !shelf_id || shelf_id[0] == '\0') {
        printf("\n������Ч������\n");
        return false;
    }

    // ��������Ƿ�Ϊ��
    if (!*head) {
        printf("\n���󣺵�ǰû�л������ݣ�\n");
        return false;
    }

    Shelf* prev = NULL, * curr = *head;
    bool found = false;

    // ����Ҫɾ���Ļ���
    while (curr) {
        if (strcmp(curr->shelf_id, shelf_id) == 0) {
            found = true;

            // �������Ƿ�Ϊ��
            if (curr->current_count > 0) {
                printf("\n���󣺻��� %s ������ %d ����ݣ������Ƴ����п�ݣ�\n",
                    shelf_id, curr->current_count);
                return false;
            }

            // ���������Ƴ�
            if (prev) {
                prev->next = curr->next;
            }
            else {
                *head = curr->next;
            }

            // �ͷſ���б��ڴ�
            ExpressNode* express = curr->pickup_list;
            while (express) {
                ExpressNode* temp = express;
                express = express->next;
                free(temp);
            }

            // �ͷŻ����ڴ�
            free(curr);
            printf("\n\033[32m�ɹ������� %s ��ɾ��\033[0m\n", shelf_id);
            return true;
        }
        prev = curr;
        curr = curr->next;
    }

    if (!found) {
        printf("\n����δ�ҵ����� %s\n", shelf_id);
    }
    return false;
}

// ========== �鿴����״̬ ==========
void display_shelves(Shelf* shelves) {
    // �����������Ƿ�Ϊ��
    if (!shelves) {
        printf("\n��ǰû�л������ݣ�\n");
        pause_program();
        clear_input_buffer();
        return;
    }

    printf("\n====== ����״̬���� ======\n");
    // ���Ʊ���ϱ߿�ʹ��Unicode�Ʊ����
    printf("���������������������Щ������������������������������Щ������������������������Щ������������������������Щ�������������������������\n");
    printf("   ����ID      ʹ�����      �������     ��������      ״̬     \n");
    printf("���������������������੤�����������������������������੤�����������������������੤�����������������������੤������������������������\n");

    // ������������
    Shelf* curr = shelves;
    while (curr) {
        // ���㵱ǰʹ���ʣ���ǰ����/���������
        float ratio = (float)curr->current_count / curr->max_capacity;

        // ����ʹ����������ɫ��
        // >80%: ��ɫ \033[31m
        // >50%: ��ɫ \033[33m
        // ����: ��ɫ \033[32m
        const char* color = ratio > 0.8 ? "\033[31m" : ratio > 0.5 ? "\033[33m" : "\033[32m";

        // ����״̬�ı���
        // �ջ���: "��"
        // ʹ����100%: "������"
        // ����: "����"
        const char* status = (curr->current_count == 0) ? "��" : (ratio >= 1.0) ? "������" : "����";

        char shelf_type = toupper(curr->shelf_id[0]);//��ȡ����id����ĸ
        const char* type_name =
            (shelf_type == 'F') ? "����Ʒ" :
            (shelf_type == 'C') ? "����" :
            (shelf_type == 'B') ? "���" : "��ͨ";

        printf("�� %-7s �� ", curr->shelf_id);
        // ����ʹ��������ͼ
        for (int i = 0; i < 10; i++) printf(i < ratio * 10 ? "%s��\033[0m" : " ", color);//%s��������ɫ����
        // ��ӡʣ�����ݣ�ʹ���ʰٷֱȡ���ǰ���������͡�״̬��
        printf(" %-.0f%%   %-10d   %-10s   %-10s  \n",
            ratio * 100, curr->current_count, type_name, status);

        curr = curr->next;
    }

    printf("���������������������ة������������������������������ة������������������������ة������������������������ة�������������������������\n");
    pause_program();
    clear_input_buffer();
}

// �̵㵥������
void inventory_shelf(Shelf* shelf) {
    if (!shelf) {
        printf("\n������Ч�Ļ���ָ�룡\n");
        return;
    }

    printf("\n========================= ���� %s �̵� =========================\n", shelf->shelf_id);
    // ���㲢��ʾ����ʹ���ʣ�����ɫԤ����
    float usage = (float)shelf->current_count / shelf->max_capacity;// ����ʹ����(0.0~1.0)
    // ����ʹ����ѡ����ɫ // >80% ��ɫԤ��  // 50%~80% ��ɫ����  // <50% ��ɫ����
    const char* capacity_color = usage > 0.8 ? "\033[31m" : usage > 0.5 ? "\033[33m" : "\033[32m";
    printf(" ����: %s%d/%d (%.0f%%) \033[0m\n",
        capacity_color,
        shelf->current_count,
        shelf->max_capacity,
        usage * 100);
    // ����б�����
    printf("\n ����嵥���� %d ����:\n", shelf->current_count);
    printf("�������������������������������Щ��������������������������������������������Щ������������������������Щ�������������������������\n");
    printf("    ��ݵ���          ���ʱ��            ״̬        ȡ����     \n");
    printf("�������������������������������੤�������������������������������������������੤�����������������������੤������������������������\n");
    // ��������б�
    ExpressNode* curr = shelf->pickup_list;
    if (!curr) {
        printf("                             ���޿��                             \n");
    }
    else {
        while (curr) {
            // ��ʽ��ʱ����ʾ
            char time_str[20]; // time_t���͵�ʱ�������1970 - 01 - 01��ʼ��������
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M",
                localtime(&curr->data.storage_time));// ʱ���ת��

            printf("�� %-12s �� %-20s �� %-10s �� %-10s ��\n",
                curr->data.tracking_num, // ��ݵ���
                time_str,// ���ʱ��
                get_status_text(curr->data.status),// ״̬
                curr->data.pickup_code);// ȡ����
            curr = curr->next;
        }
    }
    printf("�������������������������������ة��������������������������������������������ة������������������������ة�������������������������\n");

    // ��ӷָ���
    printf("\n ������������������������������������������������������������������������������������������������������������������������������\n");
}

// ȫվ�̵�
void full_inventory(Shelf* head) {


    printf("\n========================== ȫվ����̵� =========================\n");
    // �������л���
    while (head) {
        inventory_shelf(head);// ��ʾ������������
        head = head->next;
    }

    clear_input_buffer();
}

// ȫվ����������
void show_all_overdue_items_with_stats() {
    time_t now = time(NULL);// ��ȡ��ǰʱ���
    int total_overdue = 0; //������������ > 3�죩
    int overdue_count = 0;    // 3-7��
    int very_overdue_count = 0; // >7��

    printf("\n========================== ���������� =========================\n");

    // ��һ�飺ͳ������//�������п��
    ExpressNode* curr = pickup_list;
    while (curr) {
        if (curr->data.status == STATUS_OVERDUE) {
            double days = difftime(now, curr->data.storage_time) / 86400;// ������������
            if (days > 3) {
                total_overdue++;
                if (days > 7) {
                    very_overdue_count++;
                }
                else {
                    overdue_count++;
                }
            }
        }
        curr = curr->next;
    }

    // ��ʾͳ��ժҪ
    printf("\n====== ͳ��ժҪ ======\n");
    printf("���� (>7��):  %d ��\n", very_overdue_count);
    printf("���� (3-7��): %d ��\n", overdue_count);
    printf("----------------------\n");
    printf("�ܼ�:       %d ��\n", total_overdue);

    if (total_overdue == 0) {
        printf("\n��ǰû����������3��İ���\n");
        return;
    }

    // �ڶ��飺�����ܷ�����ʾ����
    printf("\n===================== �����б� ======================\n");

    // ����ʾ3-7���
    if (overdue_count > 0) {
        printf("\n================= ���ڰ��� (3-7��) ==================\n");


        char displayed_shelves[100][10] = { 0 }; // ��¼����ʾ�Ļ���
        int shelf_count = 0;

        // �������п��
        curr = pickup_list;
        while (curr) {
            if (curr->data.status == STATUS_OVERDUE) {
                double days = difftime(now, curr->data.storage_time) / 86400;
                if (days > 3 && days <= 7) {
                    // �����������Ƿ��Ѿ���ʾ��
                    int already_displayed = 0;
                    for (int i = 0; i < shelf_count; i++) {
                        if (strcmp(displayed_shelves[i], curr->data.shelf_id) == 0) {
                            already_displayed = 1;
                            break;
                        }
                    }

                    if (!already_displayed) {
                        // ��¼��������Ѿ���ʾ
                        strcpy(displayed_shelves[shelf_count++], curr->data.shelf_id);

                        // ��ʾ��������ϵ��������ڼ�
                        printf("\n--------------------- ����: %s ---------------------\n\n", curr->data.shelf_id);
                        ExpressNode* shelf_curr = pickup_list;
                        while (shelf_curr) {
                            if (shelf_curr->data.status == STATUS_OVERDUE &&
                                strcmp(shelf_curr->data.shelf_id, curr->data.shelf_id) == 0) {
                                double shelf_days = difftime(now, shelf_curr->data.storage_time) / 86400;
                                if (shelf_days > 3 && shelf_days <= 7) {
                                    printf("  ����:%-20s �ռ���:%-10s (%.1f��)\n",
                                        shelf_curr->data.tracking_num,
                                        shelf_curr->data.receiver_name,
                                        shelf_days);
                                }
                            }
                            shelf_curr = shelf_curr->next;
                        }
                        printf("-----------------------------------------------------\n");
                    }
                }
            }
            curr = curr->next;
        }
    }

    // ����ʾ����7���
    if (very_overdue_count > 0) {
        printf("\n==================== �������� (>7��) =====================\n");

        // �������л���
        char displayed_shelves[100][10] = { 0 }; // ��¼����ʾ�Ļ���
        int shelf_count = 0;

        curr = pickup_list;
        while (curr) {
            if (curr->data.status == STATUS_OVERDUE) {
                double days = difftime(now, curr->data.storage_time) / 86400;
                if (days > 7) {
                    // �����������Ƿ��Ѿ���ʾ��
                    int already_displayed = 0;
                    for (int i = 0; i < shelf_count; i++) {
                        if (strcmp(displayed_shelves[i], curr->data.shelf_id) == 0) {
                            already_displayed = 1;
                            break;
                        }
                    }

                    if (!already_displayed) {
                        // ��¼��������Ѿ���ʾ
                        strcpy(displayed_shelves[shelf_count++], curr->data.shelf_id);

                        // ��ʾ��������ϵ��������ڼ�
                        printf("\n--------------------- ����: %s ---------------------\n\n", curr->data.shelf_id);
                        ExpressNode* shelf_curr = pickup_list;
                        while (shelf_curr) {
                            if (shelf_curr->data.status == STATUS_OVERDUE &&
                                strcmp(shelf_curr->data.shelf_id, curr->data.shelf_id) == 0) {
                                double shelf_days = difftime(now, shelf_curr->data.storage_time) / 86400;
                                if (shelf_days > 7) {
                                    printf("  ����:%-20s �ռ���:%-10s (%.1f��)\n",
                                        shelf_curr->data.tracking_num,
                                        shelf_curr->data.receiver_name,
                                        shelf_days);
                                }
                            }
                            shelf_curr = shelf_curr->next;
                        }
                        printf("-----------------------------------------------------\n");
                    }
                }
            }
            curr = curr->next;
        }
        printf("\n�������绰֪ͨ���� %d λ�ռ��˴��������������\n", very_overdue_count);
    }
}

// ��������Ԥ��
void check_shelf_capacity(Shelf* head) {
    clear_screen();
    printf("\n=============== ��������Ԥ�� ===============\n");
    printf("%-8s %-8s %-8s %-12s %s\n",
        "���ܺ�", "��ǰ", "���", "ʹ����", "״̬");
    printf("--------------------------------------------\n");

    bool has_overload = false;// �Ƿ��г��ػ��ܱ�־
    int total_shelves = 0;// ������������
    int warning_count = 0;// ���ػ��ܼ���

    // �ȱ���һ����ʾ���л���״̬
    Shelf* current = head;
    while (current) {
        total_shelves++;
        float ratio = (float)current->current_count / current->max_capacity;// ��ǰ����ʹ���ʣ�ת��Ϊ��������������������
        char status[20];

        if (ratio > 0.8) {
            strcpy(status, "����");
            has_overload = true;
            warning_count++;
        }
        else if (ratio > 0.6) {
            strcpy(status, "ע��");
        }
        else {
            strcpy(status, "����");
        }

        printf("%-8s %-8d %-8d %-3.0f%% %12s\n",
            current->shelf_id,
            current->current_count,
            current->max_capacity,
            ratio * 100,//ʹ����
            status);

        current = current->next;
    }

    // ��ʾͳ����Ϣ�;���
    printf("\n============ ͳ����Ϣ ============\n");
    if (total_shelves == 0) {
        printf("\n��ǰû�л������ݣ�\n");
    }
    else {
        printf("��������: %d\n", total_shelves);
        printf("���ػ���: %d (ʹ����>80%%)\n", warning_count);
        printf("��������: %d\n", total_shelves - warning_count);

        if (!has_overload) {
            printf("\n���л����������ڰ�ȫ��Χ��(��80%%)");
        }
        else {
            printf("\n[����] ��%d�����ܳ�������80%%���뼰ʱ����", warning_count);
        }
    }

    // ��ʾ�������ѡ��
    char shelf_id[20];
    do {
        printf("\n\n������ܺŲ鿴���飨����0���أ�: ");
        scanf("%19s", shelf_id);
        clear_input_buffer();

        if (strcmp(shelf_id, "0") == 0) {
            break; // ����0���˳�ѭ������
        }

        current = head;
        bool found = false;

        // ���һ���
        while (current) {
            if (strcmp(current->shelf_id, shelf_id) == 0) {
                found = true;
                clear_screen();
                printf("\n========= ���� %s ������� =========\n", shelf_id);
                printf("�������: %d\n", current->max_capacity);
                printf("��ǰ���: %d (%.0f%%)\n",
                    current->current_count,
                    (float)current->current_count / current->max_capacity * 100);
                printf("-------------------------------------\n");

                // ��ʾ�û����ϵĿ��
                ExpressNode* express_current = pickup_list;
                int package_count = 0;
                while (express_current) {
                    if (strcmp(express_current->data.shelf_id, shelf_id) == 0) {
                        package_count++;
                        printf("%d. %s | %s -> %s | %s\n",
                            package_count,
                            express_current->data.tracking_num,
                            express_current->data.sender_name,
                            express_current->data.receiver_name,
                            get_status_text(express_current->data.status));
                    }
                    express_current = express_current->next;
                }

                printf("\n����� %d �����", package_count);
                clear_input_buffer();

            }
            current = current->next;
        }
        if (!found) {
            printf("\nδ�ҵ����� %s������������\n", shelf_id);
        }

    } while (true); // ѭ��ֱ������0

    clear_input_buffer();
}

//-----------------------------------------------------------------------------------------
// ���������Ϣ���ļ�
bool save_shelves_to_file(Shelf* head, const char* filename) {
    if (!filename || filename[0] == '\0') {
        fprintf(stderr, "������Ч�ļ���\n");
        return false;
    }

    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("Failed to save shelves");
        return false;
    }

    // д���ͷ
    fprintf(fp, "shelf_id,max_capacity,current_count,pickup_list\n");

    Shelf* curr_shelf = head;
    while (curr_shelf) {
        // д����ܻ�����Ϣ
        fprintf(fp, "%s,%d,%d,",
            curr_shelf->shelf_id,
            curr_shelf->max_capacity,
            curr_shelf->current_count);

        // д������Ŀ�ݵ��ţ��ֺŷָ���
        ExpressNode* curr_express = curr_shelf->pickup_list;
        while (curr_express) {
            fprintf(fp, "%s", curr_express->data.tracking_num);
            curr_express = curr_express->next;
            if (curr_express) fprintf(fp, ";");
        }

        fprintf(fp, "\n"); // ����
        curr_shelf = curr_shelf->next;
    }

    fclose(fp);
    return true;
}

// ���ļ��л�ȡ������Ϣ
Shelf* load_shelves_from_file(const char* filename, ExpressNode* pickup_list) {
    if (!filename) return NULL;

    FILE* fp = fopen(filename, "r");
    if (!fp) return NULL;

    Shelf* head = NULL, * tail = NULL;
    char line[512]; // ����һ�����512�ֽ�

    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        char shelf_id[20], express_items[256];
        int max_cap, curr_cnt;

        // ����CSV��
        if (sscanf(line, "%19[^,],%d,%d,%255[^\n]",
            shelf_id, &max_cap, &curr_cnt, express_items) < 3) {
            continue; // ����ʧ��������
        }

        Shelf* shelf = create_shelf(shelf_id, max_cap);
        if (!shelf) continue;

        shelf->current_count = curr_cnt;

        // ������ݵ��Ų�����������
        char* token = strtok(express_items, ";");
        while (token) {
            ExpressNode* exp_node = find_express_by_tracking(pickup_list, token);
            if (exp_node) {
                ExpressNode* node = (ExpressNode*)malloc(sizeof(ExpressNode));
                if (node) {
                    node->data = exp_node->data;  // ���ƿ������
                    node->next = shelf->pickup_list;
                    shelf->pickup_list = node;
                    strncpy(exp_node->data.shelf_id, shelf->shelf_id, sizeof(exp_node->data.shelf_id) - 1);
                }
            }
            token = strtok(NULL, ";");
        }

        // �����ܼ�������
        if (!head) head = tail = shelf;
        else {
            tail->next = shelf;
            tail = shelf;
        }
    }

    fclose(fp);
    return head;
}

//���ݿ�ݵ��Ų����ض��Ŀ��
Express* find_express_by_tracking(ExpressNode* head, const char* tracking_num) {
    while (head) {
        if (strcmp(head->data.tracking_num, tracking_num) == 0) {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

// ��ȡ״̬�ı�
const char* get_status_text(PackageStatus status) {
    static const char* texts[] = {
        "�����",       // STATUS_PENDING_IN
        "δ�ĳ�",       // STATUS_UNSENT
        "�Ѽĳ�",       // STATUS_SENT
        "��ȡ��",       // STATUS_UNPICKED
        "��ȡ��",       // STATUS_PICKED
        "������",       // STATUS_OVERDUE
        "��ʧ��",         // STATUS_LOST
        "�𻵼�",       // STATUS_DAMAGED
        "�����",       // STATUS_MISPICKED
        "ð���",	  // STATUS_WRONGPICKED
        "�쳣��",       // STATUS_ABNORMAL
        "�쳣�Ѵ���",   // STATUS_ABNORMAL_RESOLVED
        "����",        //STATUS_REJECTED
        "�˻�" ,      //STATUS_RETURNED
        "���´�ȡ�� "   //STATUS_UNPICKED_AGAIN

    };
    return (status >= STATUS_UNSENT && status <= STATUS_UNPICKED_AGAIN)
        ? texts[status]
        : "δ֪״̬";
}

//ͳ�ƻ�������
int count_shelves(Shelf* shelves) {
    int count = 0;
    Shelf* current = shelves;

    while (current != NULL) {
        count++;
        current = current->next;
    }

    return count;
}

//��֤����ID��ʽ�Ƿ�Ϸ�
bool is_valid_shelf_id(const char* shelf_id) {
    // ����Ƿ�ΪNULL����ַ���
    if (!shelf_id || shelf_id[0] == '\0') {
        return false;
    }

    // ��鳤�ȣ�ʾ��Ҫ��2-10���ַ���
    size_t len = strlen(shelf_id);
    if (len < 2 || len > 10) {
        return false;
    }

    // ����ʽ��ʾ�������ַ�����Ϊ��ĸ������Ϊ��ĸ�����֣�
    if (!isalpha(shelf_id[0])) {
        return false;
    }

    for (size_t i = 1; i < len; i++) {
        if (!isalnum(shelf_id[i])) {
            return false;
        }
    }

    return true;
}

// �������䷽ʽ��ȡ����ǰ׺
char get_shelf_prefix_by_shipping(ShippingMethod method) {
    switch (method) {
    case SHIPPING_COLD_CHAIN: return 'C';
    case SHIPPING_BULK:       return 'B';
    case SHIPPING_NORMAL:     return 'N';
    default:                  return 'N';
    }
}

// ���ҿ��û���
Shelf* find_available_shelf(Shelf* shelves, char prefix) {
    Shelf* curr = shelves;
    while (curr) {
        // ����������ǰ׺ƥ���ҵ�ǰ����δ���������
        if (toupper(curr->shelf_id[0]) == toupper(prefix) &&
            curr->current_count < curr->max_capacity) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

// �����ķ�����ܺ���
/*�ҵ����õĻ��ܺ󽫿�ݼ���û��ܵĿ��������*/
int assign_to_shelf(Express* express, Shelf* shelf_list, char shelf_prefix, time_t storage_time) {
    // ���ҿ��û���
    Shelf* target = find_available_shelf(shelf_list, shelf_prefix);
    if (!target) {// ����Ƿ��ҵ����û���
        printf("��� %s ����ʧ�ܣ�%c���ͻ��ܲ����ڻ�������\n",
            express->tracking_num, shelf_prefix);
        return 0;
    }

    // ����ȡ���루��ʽ������ID-��ţ��� N01-001��
    int seq_num = target->current_count + 1;// ��ȡ��ǰ���
    snprintf(express->pickup_code, sizeof(express->pickup_code),
        "%s-%03d", target->shelf_id, seq_num);

    // ������ݽڵ㲢�����������
    ExpressNode* new_node = (ExpressNode*)malloc(sizeof(ExpressNode));
    if (!new_node) {
        perror("�ڴ����ʧ��");
        return 0;
    }
    //  �������ӵ���������ͷ��
    new_node->data = *express;// ���ƿ������
    new_node->next = target->pickup_list;// ��������ͷ��
    target->pickup_list = new_node;
    target->current_count++;

    // ���¿����Ϣ
    strncpy(express->shelf_id, target->shelf_id, sizeof(express->shelf_id) - 1);
    express->shelf_id[sizeof(express->shelf_id) - 1] = '\0';// ȷ���ַ�����ֹ
    express->storage_time = storage_time;
    express->status = STATUS_UNPICKED;

    printf("|-- ��� %s �Ѵ��� %s ���ܣ�ȡ���룺%s\n",
        express->tracking_num, target->shelf_id, express->pickup_code);

    // �����޸ĵ��ļ�
    if (!save_shelves_to_file(shelf_list, "shelves.txt")) {
        printf("\n\033[33m���棺����ɹ�������ʧ�ܣ�\033[0m\n");
    }
    return 1;
}

//�ӻ�����ɾ�����
/*����ݴӻ��ܵĿ��������ɾ��*/
bool remove_express_from_shelf(Shelf** shelves, const char* shelf_id, const char* tracking_num) {
    if (!shelves || !shelf_id || !tracking_num) {
        printf("\n������Ч������\n");
        return false;
    }

    // ����Ŀ�����
    Shelf* curr_shelf = *shelves;
    while (curr_shelf && strcmp(curr_shelf->shelf_id, shelf_id) != 0) {
        curr_shelf = curr_shelf->next;
    }

    if (!curr_shelf) {
        printf("\n����δ�ҵ����� %s\n", shelf_id);
        return false;
    }

    // ���Ҳ��Ƴ���ݽڵ�
    ExpressNode* prev = NULL;
    ExpressNode* curr = curr_shelf->pickup_list;
    bool found = false;

    while (curr) {
        if (strcmp(curr->data.tracking_num, tracking_num) == 0) {
            found = true;
            // ���������Ƴ��ڵ�
            if (prev) prev->next = curr->next;
            else curr_shelf->pickup_list = curr->next;

            curr_shelf->current_count--;
            free(curr);
            printf("\n\033[32m�ɹ������ %s �Ѵӻ��� %s �Ƴ�\033[0m\n",
                tracking_num, shelf_id);
            break;
        }
        prev = curr;
        curr = curr->next;
    }

    if (!found) {
        printf("\n���󣺻��� %s ��δ�ҵ���� %s\n", shelf_id, tracking_num);
        return false;
    }

    // �����޸ĵ��ļ�
    if (!save_shelves_to_file(*shelves, "shelves.txt")) {
        printf("\n\033[33m���棺�Ƴ��ɹ�������ʧ�ܣ�\033[0m\n");
    }

    return true;
}

