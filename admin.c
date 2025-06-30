#include "cainiao.h"

// ��������Ա
Admin* create_admin(const char* username, const char* phone, const char* password, int privilege_level) {
    Admin* new_admin = (Admin*)malloc(sizeof(Admin));
    if (!new_admin) {
        perror("Failed to create admin");
        return NULL;
    }

    strncpy_s(new_admin->username, MAX_USERNAME_LEN, username, _TRUNCATE);
    new_admin->username[MAX_USERNAME_LEN - 1] = '\0';

    strncpy_s(new_admin->phone, MAX_PHONE_LEN, phone, _TRUNCATE);
    new_admin->phone[MAX_PHONE_LEN - 1] = '\0';

    strncpy_s(new_admin->password, MAX_PASSWORD_LEN, password, _TRUNCATE);
    new_admin->password[MAX_PASSWORD_LEN - 1] = '\0';

    new_admin->privilege_level = (AdminPrivilegeLevel)privilege_level;
    new_admin->next = NULL;

    return new_admin;
}

// ��ӹ���Ա��ͷ�巨��//�û�������һ
void add_admin(Admin** head, Admin* new_admin) {
    if (!new_admin) return;

    new_admin->next = NULL;
    if (*head == NULL) {
        *head = new_admin;
    }
    else {
        Admin* curr = *head;
        while (curr->next) curr = curr->next;
        curr->next = new_admin;
    }
    admin_count++; // ͳһ�ڴ˴�ά��������
}

// �����û������ҹ���Ա
Admin* find_admin_by_username(Admin* head, const char* username) {
    if (!username) return NULL;
    Admin* curr = head;
    while (curr) {
        if (strcmp(curr->username, username) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

// �����ֻ��Ų����û�
Admin* find_admin_by_phone(User* head, const char* phone) {
    // ��� phone �Ƿ�Ϊ NULL
    if (!phone) return NULL;
    Admin* curr = head;
    while (curr) {
        if (strcmp(curr->phone, phone) == 0) // �ַ�����ȫƥ��
            return curr;
        curr = curr->next;
    }
    return NULL;
}

// �������Ա���ݵ��ļ�
bool save_admins_to_file(Admin* head, const char* filename) {
    // ������������Ч��
    if (!filename || strlen(filename) == 0) {
        fprintf(stderr, "������Ч���ļ���\n");
        return false;
    }
    // ���Դ��ļ�
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("�������Ա����ʧ�� - �޷����ļ�");
        return false;
    }

    // д���б���
    if (fprintf(fp, "username,phone,password,privilege_level\n") < 0) {
        fclose(fp);
        fprintf(stderr, "����д���б���ʧ��\n");
        return false;
    }

    // ��������д��ÿ������Ա����
    Admin* curr = head;
    while (curr) {
        // д�����Ա���ݣ�����Ƿ�ɹ�
        if (fprintf(fp, "\"%s\",\"%s\",\"%s\",%d\n",
            curr->username,
            curr->phone,
            curr->password,
            curr->privilege_level) < 0) {
            fclose(fp);
            fprintf(stderr, "����д�����Ա����ʧ��\n");
            return false;
        }
        curr = curr->next;
    }

    // ȷ���ļ���ȷ�ر�
    if (fclose(fp) != 0) {
        perror("�������Ա����ʧ�� - �ر��ļ�ʱ����");
        return false;
    }

    return true;
}

// ���ļ����ع���Ա���ݣ�β�巨��
Admin* load_admins_from_file(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) return NULL;

    Admin* head = NULL, * tail = NULL;
    char line[256];
    fgets(line, sizeof(line), fp); // ������ͷ

    while (fgets(line, sizeof(line), fp)) {
        // �������к�ע���У��� # ��ͷ��
        if (line[0] == '\n' || line[0] == '#' || strlen(line) < 5) continue;

        char username[MAX_USERNAME_LEN] = { 0 };
        char phone[MAX_PHONE_LEN] = { 0 };
        char password[MAX_PASSWORD_LEN] = { 0 };
        int privilege_level = 0;

        // ��ȫ�����ֶΣ������ֶΰ������ţ��������Ű�����
        if (sscanf_s(line, "\"%49[^\"]\",\"%11[^\"]\",\"%49[^\"]\",%d",
            username, (unsigned)_countof(username),
            phone, (unsigned)_countof(phone),
            password, (unsigned)_countof(password),
            &privilege_level) != 4)
        {
            fprintf(stderr, "������Ч��: %s", line);
            continue;
        }

        // ��֤�ֶ���Ч��
        if (strlen(username) == 0 || strlen(phone) != 11 || strlen(password) == 0) {
            fprintf(stderr, "�ֶ�У��ʧ��: %s", line);
            continue;
        }

        Admin* admin = create_admin(username, phone, password, privilege_level);
        if (!admin) {
            fprintf(stderr, "��������Աʧ��: %s", line);
            continue;
        }

        // ��ӵ�����
        if (!head) {
            head = tail = admin;
        }
        else {
            tail->next = admin;
            tail = admin;
        }
        admin_count++; // ���ڳɹ����ʱ���Ӽ���
    }

    fclose(fp);
    return head;
}

// �޸Ĺ���ԱȨ�޼���(ֻ�г�������Ա����)
bool modify_admin_privilege(Admin* executor, Admin* target) {
    // Ȩ�޼��
    if (!is_super_admin(executor)) {
        printf("\nȨ�޲��㣡ֻ�г�������Ա�����޸�Ȩ�޼���\n");
        return false;
    }

    // ����Ƿ����޸��Լ�
    if (strcmp(target->username, executor->username) == 0) {
        printf("\n�����޸��Լ���Ȩ�޼���\n");
        return false;
    }

    // ��ʾ��ǰȨ��
    printf("\nĿ�����Ա: %s\n", target->username);
    printf("��ǰȨ��: %s\n",
        target->privilege_level == SUPER_ADMIN ? "��������Ա" : "��ͨ����Ա");

    // ��ȡ��Ȩ�޼���
    int new_level;
    printf("\n��ѡ���µ�Ȩ�޼���:\n");
    printf("1. ��ͨ����Ա\n");
    printf("2. ��������Ա\n");
    printf("0. ȡ������\n");
    printf("������ѡ��(0-2): ");

    // ������֤
    while (1) {
        if (scanf_s("%d", &new_level) != 1) {
            clear_input_buffer();
            printf("������Ч��������0-2������: ");
            continue;
        }

        if (new_level == 0) {
            printf("\n��ȡ��Ȩ���޸Ĳ���\n");
            return false;
        }

        if (new_level == 1 || new_level == 2) {
            break;
        }

        printf("��Чѡ��������0-2������: ");
    }
    clear_input_buffer();

    // ִ���޸�
    target->privilege_level = (AdminPrivilegeLevel)(new_level - 1);
    printf("\n�ѳɹ��� %s ��Ȩ���޸�Ϊ%s����Ա\n",
        target->username,
        new_level == 2 ? "����" : "��ͨ");

    return true;
}

// ����Ƿ��ǳ�������Ա
bool is_super_admin(const Admin* admin) {
    return admin && admin->privilege_level == SUPER_ADMIN;
}

// ɾ������Ա
bool delete_admin(Admin** head, const char* username) {
    if (!head || !username) return false;

    Admin* prev = NULL, * curr = *head;

    while (curr) {
        if (strcmp(curr->username, username) == 0) {
            if (prev) {
                prev->next = curr->next;
            }
            else {
                *head = curr->next;
            }
            free(curr);
            admin_count--;
            return true;
        }
        prev = curr;
        curr = curr->next;
    }

    return false;
}

// ========== ����Աע�� ==========
void admin_register(Admin* executor) {
    clear_screen();
    printf("\n=================== ����Աע�� ===================\n");
    printf("* �û���: 14λ���ڣ�ֻ������ĸ(a-z,A-Z)������(0-9)\n");
    printf("* ����: 40λ���ڣ�ֻ������ĸ(a-z,A-Z)������(0-9)\n");
    printf("* �ֻ���: ����Ϊ11λ������\n");
    char username[15], phone[16], password[41], confirm_pwd[41];
    int privilege_level = SUPER_ADMIN;
    int valid_input;
    int first_attempt = 1;  // �״γ��Ա�־//�״ν���ѭ��ʱ������û�ֱ�Ӱ��س�������������ʾ"�û�������Ϊ��"�Ĵ��� �״�ע��ʱ��һ��ҳ������ᰴenter

    // �����û�������֤
    do {
        valid_input = 1;
        if (!first_attempt) {
            printf("�û���: ");
        }

        // ��ȫ��ȡ����
        if (fgets(username, sizeof(username), stdin) == NULL) {
            clear_input_buffer();
            valid_input = 0;
            first_attempt = 0;
            continue;
        }

        // ��������Ƿ����
        size_t input_len = strlen(username);
        if (input_len > 0 && username[input_len - 1] != '\n') {
            printf("�����û������ܳ���14���ַ���\n");
            clear_input_buffer();
            valid_input = 0;
            first_attempt = 0;
            continue;
        }

        // ȥ�����з�������У�
        if (input_len > 0 && username[input_len - 1] == '\n') {
            username[input_len - 1] = '\0';
            input_len--;
        }

        // ���������߼�
        if (input_len == 0) {
            valid_input = 0;
            // ֻ�з��״γ��Բ���ʾ����
            if (!first_attempt) {
                printf("�����û�������Ϊ�գ�\n");
            }
            first_attempt = 0;
            continue;
        }

        // ����û����Ƿ��Ѵ���
        if (find_admin_by_username(admin_list, username)) {
            printf("\n�û����Ѵ��ڣ�\n");
            valid_input = 0;
            first_attempt = 0;
            continue;
        }

        // �ϸ���֤�û�����ʽ������ĸ���֣�
        for (int i = 0; username[i] != '\0'; i++) {
            if (!isalnum((unsigned char)username[i])) {
                printf("�����û���ֻ�ܰ�����ĸ(a-z,A-Z)������(0-9)��\n");
                valid_input = 0;
                first_attempt = 0;
                break;
            }
        }
        first_attempt = 0;  // ����ѽ��й�����һ����֤
    } while (!valid_input);

    // �����ֻ��Ų���֤
    do {
        valid_input = 1;
        printf("�ֻ���(����11λ����): ");

        // ��ȫ��ȡ����
        if (fgets(phone, sizeof(phone), stdin) == NULL) {
            clear_input_buffer();
            valid_input = 0;
            continue;
        }

        // ��������Ƿ����������11λ��
        size_t input_len = strlen(phone);
        if (input_len > 0 && phone[input_len - 1] != '\n') {
            printf("�����ֻ��ű���Ϊ11λ���֣�\n");
            clear_input_buffer();
            valid_input = 0;
            continue;
        }

        // ȥ�����з�������У�
        if (input_len > 0 && phone[input_len - 1] == '\n') {
            phone[input_len - 1] = '\0';
            input_len--;
        }

        // ��鳤���Ƿ�Ϊ11λ
        if (input_len != 11) {
            printf("�����ֻ��ű���Ϊ11λ���֣���ǰ���룺%zuλ\n", input_len);
            valid_input = 0;
            continue;
        }

        // ��֤�Ƿ�Ϊ������
        for (int i = 0; i < 11; i++) {
            if (!isdigit((unsigned char)phone[i])) {
                printf("�����ֻ��ű���Ϊ�����֣���%dλ�Ƿ�\n", i + 1);
                valid_input = 0;
                break;
            }
        }
        // ����ֻ����Ƿ���ע��
        if (valid_input && find_admin_by_phone(admin_list, phone)) {
            printf("���ֻ�����ע�ᣡ��ʹ�������ֻ��š�\n");
            valid_input = 0;
        }

    } while (!valid_input);

    // �������뼰ȷ��
    do {
        do {
            valid_input = 1;
            printf("����: ");

            // ��ȫ��ȡ����
            if (fgets(password, sizeof(password), stdin) == NULL) {
                clear_input_buffer();
                valid_input = 0;
                continue;
            }

            // ��������Ƿ񱻽ضϣ�������������С��
            size_t input_len = strlen(password);
            if (input_len > 0 && password[input_len - 1] != '\n') {
                printf("����������������� %zu �ֽڣ������������롣\n", sizeof(password) - 1);
                clear_input_buffer();
                valid_input = 0;
                continue;
            }

            // ȥ�����з�
            if (input_len > 0 && password[input_len - 1] == '\n') {
                password[input_len - 1] = '\0';
                input_len--;
            }

            // ���벻��Ϊ��
            if (input_len == 0) {
                printf("�������벻��Ϊ�գ�\n");
                valid_input = 0;
                continue;
            }

            // ��֤���볤�ȣ�������40�ַ���
            if (input_len > 40) {
                printf("�������벻�ܳ���40���ַ�����ǰ���룺%zu λ\n", input_len);
                valid_input = 0;
                continue;
            }

            // �ϸ���֤�����ʽ������ĸ���֣�
            for (int i = 0; password[i] != '\0'; i++) {
                if (!isalnum((unsigned char)password[i])) {
                    printf("��������ֻ�ܰ�����ĸ(a-z,A-Z)������(0-9)��\n");
                    valid_input = 0;
                    break;
                }
            }
        } while (!valid_input);

        printf("ȷ������: ");
        if (fgets(confirm_pwd, sizeof(confirm_pwd), stdin) == NULL) {
            clear_input_buffer();
            continue;
        }
        confirm_pwd[strcspn(confirm_pwd, "\n")] = '\0';

        if (strcmp(password, confirm_pwd) != 0) {
            printf("\n������������벻һ�£����������롣\n\n");
        }
    } while (strcmp(password, confirm_pwd) != 0);

    // ����ǳ�������Աִ��ע�ᣬ����ѡ��Ȩ�޼���
    if (executor && executor->privilege_level == SUPER_ADMIN) {//��û�й���Ա�״�ע�ᴫ����NULL������ʾ
        printf("Ȩ�޵ȼ� (1-��ͨ����Ա, 2-��������Ա)[Ĭ��1]: ");
        char level_input[10];
        fgets(level_input, sizeof(level_input), stdin);
        if (strlen(level_input) > 0) {
            privilege_level = atoi(level_input);
            if (privilege_level < NORMAL_ADMIN || privilege_level > SUPER_ADMIN) {
                privilege_level = NORMAL_ADMIN;//����������ע����ͨ����Ա
            }
        }
    }

    Admin* new_admin = create_admin(username, phone, password, privilege_level);
    if (!new_admin) {
        printf("\nע��ʧ�ܣ�\n");
        pause_program();
        clear_input_buffer();
        return;
    }

    add_admin(&admin_list, new_admin);//�����Ѿ�������Ա��������
    save_admins_to_file(admin_list, "admins.txt");
    printf("\n�ɹ�ע��%s����Ա: %s\n",
        privilege_level == SUPER_ADMIN ? "����" : "��ͨ",
        username);
    clear_input_buffer();
}

// ========== ����Ա��¼ ==========
/*��û�й���Ա������ע�����Ա������NULL����ע��߼�����Ա*/
void admin_login() {
    clear_screen();
    if (admin_list == NULL) {
        printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        printf("!!!!!!  �״�ϵͳ���У����봴����������Ա�˻��� !!!!!!\n");
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
        pause_program();
        clear_input_buffer();

        admin_register(NULL);//����NULL

        return;  // ע���ֱ�ӷ��أ������ظ���¼
    }

    int attempts = 3;
    while (attempts > 0) {
        clear_screen();
        printf("\n========== ����Ա��¼ ==========\n");
        printf("ʣ�ೢ�Դ���: %d\n", attempts);

        char input[50], password[50];

        // �����û���/�ֻ���
        printf("�û������ֻ���: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            clear_input_buffer();
            continue;
        }
        input[strcspn(input, "\n")] = '\0'; // ȥ�����з�

        // ��������
        printf("����: ");
        if (fgets(password, sizeof(password), stdin) == NULL) {
            clear_input_buffer();
            printf("�������\n");
            pause_program();
            clear_input_buffer();
            continue;
        }
        password[strcspn(password, "\n")] = '\0';  // ȥ�����з�

        // ���ҹ���Ա�˻�
        Admin* admin = NULL;
        Admin* curr = admin_list;
        while (curr) {
            if (strcmp(curr->username, input) == 0 ||
                strcmp(curr->phone, input) == 0) {
                admin = curr;
                break;
            }
            curr = curr->next;
        }

        // ��֤��¼
        if (!admin || strcmp(admin->password, password) != 0) {
            attempts--;
            printf("\n�û���/�ֻ��Ż��������");
            if (attempts > 0) {
                printf(" �����ԡ�\n");
                pause_program();
                clear_input_buffer();   // �ٴ�������ͣʱ�İ���
            }
            else {
                printf("\n���Դ��������꣬�������˵���\n");
                pause_program();
                clear_input_buffer();
                return;  // �������˵�
            }
            continue;

        }

        // ��¼�ɹ�
        printf("\n��¼�ɹ�����ӭ������%s %s\n",
            admin->privilege_level == SUPER_ADMIN ? "[��������Ա]" : "[��ͨ����Ա]",
            admin->username);
        pause_program();
        clear_input_buffer();
        admin_menu(admin);
        return;
    }
}


// ========== �鿴�����û� ==========
/*�����û���������û���Ϣ*/
void view_all_users() {
    clear_screen();
    printf("\n============================== �����û��б� ==============================\n");

    if (user_list == NULL) {
        printf("\n��ǰû��ע���û���\n");
        pause_program();
        clear_input_buffer();
        return;
    }

    // �����п�ʹ���������
    printf("%-20s %-15s %-15s %-10s %-15s\n",
        "�û���", "�ֻ���", "�û�����", "�ۿ���", "֧����ʽ");
    printf("--------------------------------------------------------------------------\n");

    User* current = user_list;
    while (current != NULL) {
        // ��ȡ�û������ַ���
        char user_type[20];
        switch (current->level) {
        case USER_NORMAL: strcpy_s(user_type, sizeof(user_type), "��ͨ�û�"); break;
        case USER_STUDENT: strcpy_s(user_type, sizeof(user_type), "ѧ���û�"); break;
        case USER_VIP: strcpy_s(user_type, sizeof(user_type), "VIP�û�"); break;
        case USER_ENTERPRISE: strcpy_s(user_type, sizeof(user_type), "��ҵ�û�"); break;
        case USER_PARTNER: strcpy_s(user_type, sizeof(user_type), "�����̻�"); break;
        }

        // ��ȡ֧����ʽ�ַ���
        char payment_mode[20];
        switch (current->payment_mode) {
        case PAY_CASH: strcpy_s(payment_mode, sizeof(payment_mode), "�ָ�"); break;
        case PAY_MONTHLY: strcpy_s(payment_mode, sizeof(payment_mode), "�½�"); break;
        case PAY_CONTRACT: strcpy_s(payment_mode, sizeof(payment_mode), "Э�����"); break;
        }

        printf("%-20s %-15s %-15s %-10.2f %-15s\n",
            current->username,
            current->phone,
            user_type,
            current->discount_rate,
            payment_mode);

        current = current->next;
    }

    printf("\n�ܼ�: %d λ�û�\n", user_count);
    pause_program();
    clear_input_buffer();
}

// ========== ����Ա����û�  ==========
/*����ʾ�����û���ͬע���û�һ��˼·������һ����ǣ�������ʾ�����û��󣬰�enterʱת�뵽����û�ʱ�൱���������ݣ�*/
void admin_add_user() {
    clear_screen();
    // ����ʾ�����û�
    view_all_users();

    printf("========== ����û� ==========\n");
    printf("* �û���: 14λ���ڣ�ֻ������ĸ(a-z,A-Z)������(0-9)\n");
    printf("* ����: 40λ���ڣ�ֻ������ĸ(a-z,A-Z)������(0-9)\n");
    printf("* �ֻ���: ����Ϊ11λ������\n\n");

    char username[15], phone[16], password[41], confirm_pwd[41];
    int user_type;
    int valid_input;
    int first_attempt = 1;  // �����״γ��Ա�־

    // �����û�������֤
    do {
        valid_input = 1;
        if (!first_attempt) {
            printf("�û���: ");
        }

        // ��ȫ��ȡ����
        if (fgets(username, sizeof(username), stdin) == NULL) {
            clear_input_buffer();
            valid_input = 0;
            first_attempt = 0;
            continue;
        }

        // ��������Ƿ����
        size_t input_len = strlen(username);
        if (input_len > 0 && username[input_len - 1] != '\n') {
            printf("�����û������ܳ���14���ַ���\n");
            clear_input_buffer();
            valid_input = 0;
            first_attempt = 0;
            continue;
        }

        // ȥ�����з�������У�
        if (input_len > 0 && username[input_len - 1] == '\n') {
            username[input_len - 1] = '\0';
            input_len--;
        }

        // �޸ĺ�Ŀ��������߼�
        if (input_len == 0) {
            valid_input = 0;
            // ֻ�з��״γ��Բ���ʾ����
            if (!first_attempt) {
                printf("�����û�������Ϊ�գ�\n");
            }
            first_attempt = 0;
            continue;
        }

        // ����û����Ƿ��Ѵ���
        if (find_user_by_username(user_list, username)) {
            printf("\n�û����Ѵ��ڣ�\n");
            valid_input = 0;
            first_attempt = 0;
            continue;
        }

        // �ϸ���֤�û�����ʽ
        for (int i = 0; username[i] != '\0'; i++) {
            if (!isalnum((unsigned char)username[i])) {
                printf("�����û���ֻ�ܰ�����ĸ(a-z,A-Z)������(0-9)��\n");
                valid_input = 0;
                first_attempt = 0;
                break;
            }
        }
        first_attempt = 0;  // ����ѽ��й�����һ����֤
    } while (!valid_input);

    do {
        valid_input = 1;
        printf("�ֻ���(����11λ����): ");

        // ��ȫ��ȡ����
        if (fgets(phone, sizeof(phone), stdin) == NULL) {
            clear_input_buffer();
            valid_input = 0;
            continue;
        }

        // ��������Ƿ����������11λ��
        size_t input_len = strlen(phone);
        if (input_len > 0 && phone[input_len - 1] != '\n') {
            printf("�����ֻ��ű���Ϊ11λ���֣�\n");
            clear_input_buffer();
            valid_input = 0;
            continue;
        }

        // ȥ�����з�������У�
        if (input_len > 0 && phone[input_len - 1] == '\n') {
            phone[input_len - 1] = '\0';
            input_len--;
        }

        // ��鳤���Ƿ�Ϊ11λ
        if (input_len != 11) {
            printf("�����ֻ��ű���Ϊ11λ���֣���ǰ���룺%zuλ\n", input_len);
            valid_input = 0;
            continue;
        }

        // ��֤�Ƿ�Ϊ������
        for (int i = 0; i < 11; i++) {
            if (!isdigit((unsigned char)phone[i])) {
                printf("�����ֻ��ű���Ϊ�����֣���%dλ�Ƿ�\n", i + 1);
                valid_input = 0;
                break;
            }
        }

        // ����ֻ����Ƿ���ע��
        if (valid_input && find_user_by_phone(user_list, phone)) {
            printf("���ֻ�����ע�ᣡ��ʹ�������ֻ��š�\n");
            valid_input = 0;
        }
    } while (!valid_input);

    // �������뼰ȷ��
    do {
        do {
            valid_input = 1;
            printf("����: ");

            // ��ȫ��ȡ����
            if (fgets(password, sizeof(password), stdin) == NULL) {
                clear_input_buffer();
                valid_input = 0;
                continue;
            }

            // ��������Ƿ񱻽ضϣ�������������С��
            size_t input_len = strlen(password);
            if (input_len > 0 && password[input_len - 1] != '\n') {
                printf("����������������� %zu �ֽڣ������������롣\n", sizeof(password) - 1);
                clear_input_buffer();
                valid_input = 0;
                continue;
            }

            // ȥ�����з�
            if (input_len > 0 && password[input_len - 1] == '\n') {
                password[input_len - 1] = '\0';
                input_len--;
            }

            // ���벻��Ϊ��
            if (input_len == 0) {
                printf("�������벻��Ϊ�գ�\n");
                valid_input = 0;
                continue;
            }

            // ��֤���볤�ȣ�������40�ַ���
            if (input_len > 40) {
                printf("�������벻�ܳ���40���ַ�����ǰ���룺%zu λ\n", input_len);
                valid_input = 0;
                continue;
            }

            // �ϸ���֤�����ʽ������ĸ���֣�
            for (int i = 0; password[i] != '\0'; i++) {
                if (!isalnum((unsigned char)password[i])) {
                    printf("��������ֻ�ܰ�����ĸ(a-z,A-Z)������(0-9)��\n");
                    valid_input = 0;
                    break;
                }
            }
        } while (!valid_input);

        printf("ȷ������: ");
        if (fgets(confirm_pwd, sizeof(confirm_pwd), stdin) == NULL) {
            clear_input_buffer();
            continue;
        }
        confirm_pwd[strcspn(confirm_pwd, "\n")] = '\0';

        if (strcmp(password, confirm_pwd) != 0) {
            printf("\n������������벻һ�£����������롣\n\n");
        }
    } while (strcmp(password, confirm_pwd) != 0);

    // ѡ���û�����
    printf("\n��ѡ���û�����:\n");
    printf("1. ��ͨ�û�\n");
    printf("2. ѧ���û�\n");
    printf("3. VIP�û�\n");
    printf("4. ��ҵ�û�\n");
    printf("5. �����̻�\n");
    printf("������ѡ��(1-5): ");

    while (1) {
        char input[12];
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n�������ע��ʧ�ܣ�\n");
            return; // ֱ���˳�ע������
        }

        // ��������Ƿ������û�л��з���ʾ���뱻�ضϣ�
        if (strchr(input, '\n') == NULL) {
            printf("\n������������ʧ�ܣ�\n");
            clear_input_buffer(); // ��ջ�����ʣ������
            pause_program();
            return; // ֱ���˳�ע������
        }

        // ȥ�����з�
        input[strcspn(input, "\n")] = '\0';

        // ��֤�Ƿ�Ϊ��������1-5
        if (strlen(input) != 1 || !isdigit(input[0])) {
            printf("��Чѡ������������(1-5): ");
            continue;
        }

        user_type = input[0] - '0';
        if (user_type >= 1 && user_type <= 5) {
            break;
        }

        printf("��Чѡ������������(1-5): ");
    }


    // �����û�
    User* new_user = create_user(phone, password, username, (UserLevel)(user_type - 1));
    if (!new_user) {
        printf("\nע��ʧ�ܣ�\n");
        pause_program();
        return;
    }

    // �����û�����
    add_user(&user_list, new_user);
    user_count++;
    save_users_to_file(user_list, "users.txt");

    printf("\nע��ɹ���");
    clear_input_buffer();
    printf("��ӵ��û����˻�����: ");
    switch (new_user->level) {
    case USER_NORMAL:    printf("��ͨ�û�"); break;
    case USER_STUDENT:  printf("ѧ���û�(85��)"); break;
    case USER_VIP:      printf("VIP�û�(9��)"); break;
    case USER_ENTERPRISE: printf("��ҵ�û�(75��)"); break;
    case USER_PARTNER:  printf("�����̻�(Э���)"); break;
    }
    printf("\n");
    clear_input_buffer(); // ��ջ�����ʣ������
}


// ========== �鿴���й���Ա��Ϣ ==========
bool view_all_admins() {
    clear_screen();
    printf("\n===================== ���й���Ա�б� ======================\n");

    // ������Ա�б��Ƿ�Ϊ��
    if (admin_list == NULL) {
        printf("\n��ǰû��ע�����Ա��\n");
        pause_program();
        clear_input_buffer();
        return false;  // ����false��ʾû��������ʾ
    }

    // ������
    printf("%-20s %-15s %-15s %-20s\n",
        "�û���", "�ֻ���", "Ȩ�޼���", "����");
    printf("-----------------------------------------------------------\n");

    // ��������Ա����
    Admin* current = admin_list;
    int count = 0;
    bool display_success = true;

    while (current != NULL) {
        // ��ȡȨ�޼����ַ���
        const char* privilege_level;
        switch (current->privilege_level) {
        case SUPER_ADMIN:
            privilege_level = "��������Ա";
            break;
        case NORMAL_ADMIN:
            privilege_level = "��ͨ����Ա";
            break;
        default:
            privilege_level = "δ֪";
            display_success = false; // ����δ֪Ȩ�޼�����Ϊ��ʾ����
        }

        // ��ӡ��������Ա��Ϣ������ԭʼ���룩
        printf("%-20s %-15s %-15s ",
            current->username,
            current->phone,
            privilege_level);
        printf("******\n");

        // �ƶ�����һ���ڵ�
        current = current->next;
        count++;

        // ��������Ƿ���
        if (count > 1000) { // ��ֹ����ѭ��
            printf("\n���棺��⵽���ܵ�����ѭ����\n");
            display_success = false;
            break;
        }
    }

    printf("\n�ܼ�: %d λ����Ա\n", count);
    pause_program();
    clear_input_buffer();

    return display_success;  // ������ʾ�Ƿ���ȫ�ɹ�
}

// ========== ��������Ա�޸���������Ա��Ϣ ==========
void super_modify_admin_info(Admin* super_admin) {
    if (!is_super_admin(super_admin)) {
        printf("\nȨ�޲��㣡ֻ�г�������Ա�����޸���������Ա��Ϣ��\n");
        pause_program();
        clear_input_buffer();
        return;
    }

    clear_screen();
    printf("\n========== �޸Ĺ���Ա��Ϣ ==========\n");

    // ��ʾ���й���Ա�б�
    view_all_admins();

    char target_username[MAX_USERNAME_LEN] = { 0 };
    printf("\n������Ҫ�޸ĵĹ���Ա�û���: ");
    if (fgets(target_username, sizeof(target_username), stdin) == NULL) {
        clear_input_buffer();
        printf("������Ч��\n");
        pause_program();
        clear_input_buffer();
        return;
    }
    target_username[strcspn(target_username, "\n")] = '\0';

    Admin* target = find_admin_by_username(admin_list, target_username);
    if (!target) {
        printf("�Ҳ���ָ������Ա: %s\n", target_username);
        pause_program();
        clear_input_buffer();
        return;
    }

    if (strcmp(target->username, super_admin->username) == 0) {
        printf("�����޸��Լ�����Ϣ����ʹ�ø��������޸Ĺ��ܣ�\n");
        pause_program();
        clear_input_buffer();
        return;
    }

    // �޸Ĳ���ѭ��
    while (1) {
        clear_screen();
        printf("\n========== �����޸Ĺ���Ա��Ϣ ==========\n");
        printf("����Ա: %s\n", target->username);
        printf("��ǰ��Ϣ:\n");
        printf("1. �û���: %s\n", target->username);
        printf("2. �ֻ���: %s\n", target->phone);
        printf("3. ����: ******\n");
        printf("4. Ȩ�޼���: %s\n",
            target->privilege_level == SUPER_ADMIN ? "��������Ա" : "��ͨ����Ա");
        printf("0. �����ϼ��˵�\n");
        printf("\n��ѡ��Ҫ�޸ĵ���Ŀ: ");

        int choice;
        char input[10];
        fgets(input, sizeof(input), stdin);
        if (sscanf(input, "%d", &choice) != 1) {
            clear_input_buffer();
            printf("��Ч�����룡\n");
            pause_program();
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) {
        case 0:
            return;
        case 1: { // �޸��û���
            char new_username[15];
            int valid_input;

            do {
                valid_input = 1;
                printf("���û���(1-14λ��ĸ����): ");

                if (fgets(new_username, sizeof(new_username), stdin) == NULL) {
                    clear_input_buffer();
                    valid_input = 0;
                    continue;
                }

                // ��������Ƿ����
                size_t input_len = strlen(new_username);
                if (input_len > 0 && new_username[input_len - 1] != '\n') {
                    printf("�����û������ܳ���14���ַ���\n");
                    clear_input_buffer();
                    valid_input = 0;
                    continue;
                }

                // ȥ�����з�
                if (input_len > 0 && new_username[input_len - 1] == '\n') {
                    new_username[input_len - 1] = '\0';
                    input_len--;
                }

                // �����Ч����
                if (input_len == 0) {
                    printf("�����û�������Ϊ�գ�\n");
                    valid_input = 0;
                    continue;
                }

                // ����û����Ƿ��Ѵ���
                if (find_admin_by_username(admin_list, new_username)) {
                    printf("�����û�����ע�����ԭ�û�����ͬ��\n");
                    valid_input = 0;
                    continue;
                }

                // ��֤�û�����ʽ
                for (int i = 0; new_username[i] != '\0'; i++) {
                    if (!isalnum((unsigned char)new_username[i])) {
                        printf("�����û���ֻ�ܰ�����ĸ(a-z,A-Z)������(0-9)��\n");
                        valid_input = 0;
                        break;
                    }
                }
            } while (!valid_input);

            strncpy_s(target->username, sizeof(target->username), new_username, 14);//����
            printf("�û����޸ĳɹ���\n");
            break;
        }
        case 2: { // �޸��ֻ���
            char new_phone[16];
            int valid_input;

            do {
                valid_input = 1;
                printf("���ֻ���(11λ����): ");

                if (fgets(new_phone, sizeof(new_phone), stdin) == NULL) {
                    clear_input_buffer();
                    valid_input = 0;
                    continue;
                }

                // ��������Ƿ����
                size_t input_len = strlen(new_phone);
                if (input_len > 0 && new_phone[input_len - 1] != '\n') {
                    printf("�����ֻ��ű���Ϊ11λ���֣�\n");
                    clear_input_buffer();
                    valid_input = 0;
                    continue;
                }

                // ȥ�����з�
                if (input_len > 0 && new_phone[input_len - 1] == '\n') {
                    new_phone[input_len - 1] = '\0';
                    input_len--;
                }

                // ��鳤��
                if (input_len != 11) {
                    printf("�����ֻ��ű���Ϊ11λ���֣���ǰ���룺%zuλ\n", input_len);
                    valid_input = 0;
                    continue;
                }

                // ��֤�Ƿ�Ϊ������
                for (int i = 0; i < 11; i++) {
                    if (!isdigit((unsigned char)new_phone[i])) {
                        printf("�����ֻ��ű���Ϊ�����֣���%dλ�Ƿ�\n", i + 1);
                        valid_input = 0;
                        break;
                    }
                }

                // ����ֻ����Ƿ���ע�����ԭ�ֻ�����ͬ
                if (valid_input && find_admin_by_phone(admin_list, new_phone) &&
                    strcmp(new_phone, target->phone) != 0) {
                    printf("���󣺸��ֻ�����ע�����ԭ�ֻ�����ͬ\n");
                    valid_input = 0;
                }
            } while (!valid_input);

            strncpy_s(target->phone, sizeof(target->phone), new_phone, 11);
            printf("�ֻ����޸ĳɹ���\n");
            break;
        }

        case 3: { // �޸�����
            char new_password[41] = { 0 };
            char confirm_pwd[41] = { 0 };
            int valid_input;

            do {
                valid_input = 1;
                printf("������(1-40λ��ĸ����): ");

                if (fgets(new_password, sizeof(new_password), stdin) == NULL) {
                    clear_input_buffer();
                    valid_input = 0;
                    continue;
                }


                // ��������Ƿ����
                size_t input_len = strlen(new_password);
                if (input_len > 0 && new_password[input_len - 1] != '\n') {
                    printf("�������벻�ܳ���40���ַ���\n");
                    clear_input_buffer();
                    valid_input = 0;
                    continue;
                }

                // ȥ�����з�
                if (input_len > 0 && new_password[input_len - 1] == '\n') {
                    new_password[input_len - 1] = '\0';
                    input_len--;
                }

                // �����Ч����
                if (input_len == 0) {
                    printf("�������벻��Ϊ�գ�\n");
                    valid_input = 0;
                    continue;
                }

                // ��֤�����ʽ
                for (int i = 0; new_password[i] != '\0'; i++) {
                    if (!isalnum((unsigned char)new_password[i])) {
                        printf("��������ֻ�ܰ�����ĸ(a-z,A-Z)������(0-9)��\n");
                        valid_input = 0;
                        break;
                    }
                }

                if (!valid_input) continue;

                printf("ȷ��������: ");
                if (fgets(confirm_pwd, sizeof(confirm_pwd), stdin) == NULL) {
                    clear_input_buffer();
                    valid_input = 0;
                    continue;
                }
                confirm_pwd[strcspn(confirm_pwd, "\n")] = '\0';

                if (strcmp(new_password, confirm_pwd) != 0) {
                    printf("����������������벻һ�£�\n");
                    valid_input = 0;
                }
            } while (!valid_input);

            strncpy_s(target->password, sizeof(target->password), new_password, 40);
            printf("�����޸ĳɹ���\n");
            break;
        }

        case 4: { // �޸�Ȩ�޼���
            clear_screen();
            printf("\n========== �޸Ĺ���ԱȨ�� ==========\n");
            printf("�����޸Ĺ���Ա %s ��Ȩ��\n", target->username);

            int new_level;
            printf("\n��ѡ���µ�Ȩ�޼���:\n");
            printf("1. ��ͨ����Ա\n");
            printf("2. ��������Ա\n");
            printf("0. ȡ������\n");
            printf("������ѡ��(0-2): ");

            char level_input[10];
            fgets(level_input, sizeof(level_input), stdin);
            if (sscanf(level_input, "%d", &new_level) != 1) {
                printf("������Ч��\n");
                pause_program();
                clear_input_buffer();
                break;
            }

            if (new_level == 0) {
                printf("\n��ȡ��Ȩ���޸Ĳ���\n");
                pause_program();
                clear_input_buffer();
                break;
            }

            if (new_level < 1 || new_level > 2) {
                printf("��Чѡ��������0-2�����֣�\n");
                pause_program();
                clear_input_buffer();
                break;
            }

            // �޸�Ȩ�޼���
            target->privilege_level = (new_level == 2) ? SUPER_ADMIN : NORMAL_ADMIN;

            // �����޸ĵ��ļ�
            if (!save_admins_to_file(admin_list, "admins.txt")) {
                printf("���棺Ȩ���޸ĳɹ������浽�ļ�ʧ�ܣ�\n");
            }
            else {
                printf("\n�ѳɹ��� %s ��Ȩ���޸�Ϊ%s����Ա\n",
                    target->username,
                    (target->privilege_level == SUPER_ADMIN) ? "����" : "��ͨ");
            }

            pause_program();
            clear_input_buffer();
            break;
        }

        default:
            printf("��Ч��ѡ��\n");
        }

        // ÿ���޸ĺ󱣴�
        if (choice >= 1 && choice <= 3) {
            if (!save_admins_to_file(admin_list, "admins.txt")) {
                printf("���棺����Ա���ݱ���ʧ�ܣ�\n");
            }
            pause_program();
            clear_input_buffer();
        }
    }
}

// ==========  ע��������ͨ����Աorע���Լ�  ==========
/*(�߼�����Ա�����ע����ͨ����Ա�û�����ͨ����Ա�����ע���Լ�)//���������߼�����Ա*/
void admin_delete_account(Admin* current_admin) {
    clear_screen();
    printf("\n========== ����Ա�˻�ע�� ==========\n");

    // ��ʾ��ǰ�û���Ϣ
    printf("��ǰ�˻�: %s (%s)\n\n",
        current_admin->username,
        current_admin->privilege_level == SUPER_ADMIN ? "��������Ա" : "��ͨ����Ա");

    char target_username[MAX_USERNAME_LEN] = { 0 };
    Admin* target = NULL;

    // ��������Ա����ѡ��ע�������˻�
    if (current_admin->privilege_level == SUPER_ADMIN) {
        // ��ʾ���й���Ա�б�
        view_all_admins();

        // ��ȡĿ���û�����������ȷ���룩
        while (1) {
            printf("\n������Ҫע���Ĺ���Ա�û���: ");
            if (fgets(target_username, MAX_USERNAME_LEN, stdin) == NULL) {
                clear_input_buffer();
                printf("\n������Ч��\n");
                pause_program();
                clear_input_buffer();
                return;
            }
            target_username[strcspn(target_username, "\n")] = '\0'; // ȥ�����з�

            // ����Ƿ�����Ϊ��
            if (strlen(target_username) == 0) {
                printf("\n���󣺱���������Ч���û�����\n");
                continue;
            }

            target = find_admin_by_username(admin_list, target_username);
            if (!target) {
                printf("\n���󣺹���Ա %s �����ڣ����������롣\n", target_username);
                continue;
            }

            // ����Ƿ�������ע��
            if (strcmp(target->username, current_admin->username) == 0) {
                printf("\n��ȫ���ƣ���������Ա��������ע����\n");
                printf("����������������Աִ�д˲�����\n");
                pause_program();
                clear_input_buffer();
                return;
            }
            break;
        }
    }
    else {
        // ��ͨ����Աֻ��ע���Լ�
        target = current_admin;
        printf("�����ڳ���ע���Լ����˻���\n");
    }

    // ȷ�ϲ��� - �޸�Ϊֻ����"y"ȷ��
    char confirm;
    printf("\n���棺�˲���������ɾ������Ա�˻� %s��\n", target->username);
    printf("ȷ��Ҫע����(���� y ȷ�ϣ�����ȡ��): ");
    if (scanf(" %c", &confirm) != 1) {

        printf("\n������Ч��������ȡ����\n");
        pause_program();
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    // ��������Сд���д��yʱ��ȷ��
    if (tolower(confirm) != 'y') {
        printf("\n������ȡ����\n");

        pause_program();
        clear_input_buffer();
        return;
    }

    // ִ��ע��
    if (delete_admin(&admin_list, target->username)) {
        if (save_admins_to_file(admin_list, "admins.txt")) {
            printf("\n�˻��ѳɹ�ע����\n");
            clear_input_buffer();
            // �����ע���Լ������˳�ϵͳ
            if (strcmp(target->username, current_admin->username) == 0) {
                printf("ϵͳ���˳�...\n");
                pause_program();
                clear_input_buffer();
                exit(0);
            }
        }
        else {
            printf("\n�˻���ע��������ʧ�ܣ�\n");
        }
    }
    else {
        printf("\nע��ʧ�ܣ�\n");
    }
    pause_program();
    clear_input_buffer();
}

// ==========  �޸Ĺ���Ա��Ϣ���Լ���  ==========
bool modify_admin_info(Admin* current_admin) {
    if (!current_admin) return false;

    clear_screen();
    printf("\n========== �޸Ĺ���Ա��Ϣ ==========\n");

    // ��ʾ��ǰ��Ϣ
    printf("��ǰ��Ϣ:\n");
    printf("1. �û���: %s\n", current_admin->username);
    printf("2. �ֻ���: %s\n", current_admin->phone);
    //printf("3. ����: %s\n", current_admin->password);
    printf("3. ����: ******\n");

    printf("4. Ȩ�޼���: %s\n",
        current_admin->privilege_level == SUPER_ADMIN ? "��������Ա" : "��ͨ����Ա");
    printf("0. ����\n");
    printf("\n��ѡ��Ҫ�޸ĵ���Ŀ (1-4, 0ȡ��): ");
    int choice;
    scanf_s("%d", &choice);
    clear_input_buffer();

    switch (choice) {
    case 1: { // �޸��û���
        char new_username[15];
        int valid_input;

        do {
            valid_input = 1;
            printf("���û���(1-14λ��ĸ����): ");

            if (fgets(new_username, sizeof(new_username), stdin) == NULL) {
                clear_input_buffer();
                valid_input = 0;
                continue;
            }

            // ��������Ƿ����
            size_t input_len = strlen(new_username);
            if (input_len > 0 && new_username[input_len - 1] != '\n') {
                printf("�����û������ܳ���14���ַ���\n");
                clear_input_buffer();
                valid_input = 0;
                continue;
            }

            // ȥ�����з�
            if (input_len > 0 && new_username[input_len - 1] == '\n') {
                new_username[input_len - 1] = '\0';
                input_len--;
            }

            // �����Ч����
            if (input_len == 0) {
                printf("�����û�������Ϊ�գ�\n");
                valid_input = 0;
                continue;
            }

            // ����û����Ƿ��Ѵ���
            if (find_admin_by_username(admin_list, new_username)) {
                printf("�����û�����ע�����ԭ�û�����ͬ��\n");
                valid_input = 0;
                continue;
            }

            // ��֤�û�����ʽ
            for (int i = 0; new_username[i] != '\0'; i++) {
                if (!isalnum((unsigned char)new_username[i])) {
                    printf("�����û���ֻ�ܰ�����ĸ(a-z,A-Z)������(0-9)��\n");
                    valid_input = 0;
                    break;
                }
            }
        } while (!valid_input);

        strncpy_s(current_admin->username, sizeof(current_admin->username), new_username, 14);
        printf("�û����޸ĳɹ���\n");
        break;
    }

    case 2: { // �޸��ֻ���
        char new_phone[16];
        int valid_input;

        do {
            valid_input = 1;
            printf("���ֻ���(11λ����): ");

            if (fgets(new_phone, sizeof(new_phone), stdin) == NULL) {
                clear_input_buffer();
                valid_input = 0;
                continue;
            }

            // ��������Ƿ����
            size_t input_len = strlen(new_phone);
            if (input_len > 0 && new_phone[input_len - 1] != '\n') {
                printf("�����ֻ��ű���Ϊ11λ���֣�\n");
                clear_input_buffer();
                valid_input = 0;
                continue;
            }

            // ȥ�����з�
            if (input_len > 0 && new_phone[input_len - 1] == '\n') {
                new_phone[input_len - 1] = '\0';
                input_len--;
            }

            // ��鳤��
            if (input_len != 11) {
                printf("�����ֻ��ű���Ϊ11λ���֣���ǰ���룺%zuλ\n", input_len);
                valid_input = 0;
                continue;
            }

            // ��֤�Ƿ�Ϊ������
            for (int i = 0; i < 11; i++) {
                if (!isdigit((unsigned char)new_phone[i])) {
                    printf("�����ֻ��ű���Ϊ�����֣���%dλ�Ƿ�\n", i + 1);
                    valid_input = 0;
                    break;
                }
            }

            // ����ֻ����Ƿ���ע�����ԭ�ֻ�����ͬ
            if (valid_input && find_admin_by_phone(admin_list, new_phone) &&
                strcmp(new_phone, current_admin->phone) != 0) {
                printf("���󣺸��ֻ�����ע�����ԭ�ֻ�����ͬ\n");
                valid_input = 0;
            }
        } while (!valid_input);

        strncpy_s(current_admin->phone, sizeof(current_admin->phone), new_phone, 11);
        printf("�ֻ����޸ĳɹ���\n");
        break;
    }

    case 3: { // �޸�����
        char new_password[41], confirm_pwd[41];
        int valid_input;

        do {
            valid_input = 1;
            printf("������(1-40λ��ĸ����): ");

            if (fgets(new_password, sizeof(new_password), stdin) == NULL) {
                clear_input_buffer();
                valid_input = 0;
                continue;
            }

            // ��������Ƿ����
            size_t input_len = strlen(new_password);
            if (input_len > 0 && new_password[input_len - 1] != '\n') {
                printf("�������벻�ܳ���40���ַ���\n");
                clear_input_buffer();
                valid_input = 0;
                continue;
            }

            // ȥ�����з�
            if (input_len > 0 && new_password[input_len - 1] == '\n') {
                new_password[input_len - 1] = '\0';
                input_len--;
            }

            // �����Ч����
            if (input_len == 0) {
                printf("�������벻��Ϊ�գ�\n");
                valid_input = 0;
                continue;
            }

            // ��֤�����ʽ
            for (int i = 0; new_password[i] != '\0'; i++) {
                if (!isalnum((unsigned char)new_password[i])) {
                    printf("��������ֻ�ܰ�����ĸ(a-z,A-Z)������(0-9)��\n");
                    valid_input = 0;
                    break;
                }
            }

            if (!valid_input) continue;

            printf("ȷ��������: ");
            if (fgets(confirm_pwd, sizeof(confirm_pwd), stdin) == NULL) {
                clear_input_buffer();
                valid_input = 0;
                continue;
            }
            confirm_pwd[strcspn(confirm_pwd, "\n")] = '\0';

            if (strcmp(new_password, confirm_pwd) != 0) {
                printf("����������������벻һ�£�\n");
                valid_input = 0;
            }
        } while (!valid_input);

        strncpy_s(current_admin->password, sizeof(current_admin->password), new_password, 40);
        printf("�����޸ĳɹ���\n");
        break;
    }
    case 4: { // �޸�Ȩ�޼���
        if (!is_super_admin(current_admin)) {
            printf("\nȨ�޲��㣡ֻ�г�������Ա�����޸�Ȩ�޼���\n");
            pause_program();
            clear_input_buffer();
            return false;
        }

        printf("\n��ǰȨ��: %s\n",
            current_admin->privilege_level == SUPER_ADMIN ? "��������Ա" : "��ͨ����Ա");

        int new_level;
        printf("\n��ѡ���µ�Ȩ�޼���:\n");
        printf("1. ��ͨ����Ա\n");
        printf("2. ��������Ա\n");
        printf("0. ȡ������\n");
        printf("������ѡ��(0-2): ");

        while (1) {
            if (scanf_s("%d", &new_level) != 1) {
                clear_input_buffer();
                printf("������Ч��������0-2������: ");
                continue;
            }

            if (new_level == 0) {
                printf("\n��ȡ��Ȩ���޸Ĳ���\n");
                return false;
            }

            if (new_level == 1 || new_level == 2) {
                break;
            }

            printf("��Чѡ��������0-2������: ");
        }
        clear_input_buffer();

        current_admin->privilege_level = (AdminPrivilegeLevel)(new_level - 1);
        printf("\n�ѳɹ���Ȩ���޸�Ϊ%s����Ա\n",
            new_level == 2 ? "����" : "��ͨ");
        break;
    }
    case 0:
        return false;

    default:
        printf("��Ч��ѡ��\n");
        pause_program();
        clear_input_buffer();
        return false;
    }

    // �����޸ĵ��ļ�
    save_admins_to_file(admin_list, "admins.txt");
    pause_program();
    clear_input_buffer();
    return true;
}







