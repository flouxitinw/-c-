#include "cainiao.h"

// ��ʼ���û���Ȩ
void init_user_privilege(User* user) {
    switch (user->level) {
    case USER_NORMAL://0
        user->discount_rate = 1.0f;    // 100% �ۿ���
        user->payment_mode = PAY_CASH; // �ָ�
        break;

    case USER_STUDENT://1
        user->discount_rate = 0.85f;   // 85% �ۿ���
        user->payment_mode = PAY_CASH; // �ָ�
        break;

    case USER_VIP://2
        user->discount_rate = 0.9f;    // 90% �ۿ���
        user->payment_mode = PAY_MONTHLY;//�½�
        break;

    case USER_ENTERPRISE://3
        user->discount_rate = 0.75f;   // 75% �ۿ���
        user->payment_mode = PAY_MONTHLY; // �½�
        break;

    case USER_PARTNER://4
        user->discount_rate = 0.6f;   // Э��ۻ�׼60%
        user->payment_mode = PAY_CONTRACT; // ���ڽ���
        break;
    default:
        fprintf(stderr, "���棺δ֪�û��ȼ� %d��ʹ��Ĭ��Ȩ��\n", user->level);
        user->discount_rate = 1.0f;
        user->payment_mode = PAY_CASH;
        break;
    }

}

// �������û�
User* create_user(const char* phone, const char* pwd, const char* name, UserLevel level) {
    // �����ڴ�
    User* new_user = (User*)malloc(sizeof(User));
    if (!new_user) {
        perror("Failed to create user");
        return NULL;
    }

    // ������Ϣ
    strncpy_s(new_user->phone, sizeof(new_user->phone), phone, 11);//���ֻ��� phone ���Ƶ� new_user->phone 
    new_user->phone[11] = '\0';//�ֶ���� '\0' ��ֹ��
    strncpy_s(new_user->password, sizeof(new_user->password), pwd, 49);
    new_user->password[49] = '\0';
    strncpy_s(new_user->username, sizeof(new_user->username), name, 49);
    new_user->username[49] = '\0';
    new_user->level = level;

    // ��ʼ����Ȩ
    init_user_privilege(new_user);
    new_user->next = NULL;

    return new_user;
}

//����û���ͷ�巨��(���û���ӵ��û�������)
void add_user(User** head, User* new_user) {
    if (!new_user) return;
    new_user->next = *head;
    *head = new_user;
}

// �����ֻ��Ų����û�
User* find_user_by_phone(User* head, const char* phone) {
    // ��� phone �Ƿ�Ϊ NULL
    if (!phone) return NULL;

    User* curr = head;
    while (curr) {
        if (strcmp(curr->phone, phone) == 0) // �ַ�����ȫƥ��
            return curr;
        curr = curr->next;
    }
    return NULL;
}

// �����û���ͨ���û�����
User* find_user_by_username(User* head, const char* username) {
    //��� username �Ƿ�Ϊ NULL
    if (!username) return NULL;
    User* curr = head;
    while (curr) {
        if (strcmp(curr->username, username) == 0)//�ַ�����ȫƥ��
            return curr;
        curr = curr->next;
    }
    return NULL;
}

// ɾ���û�
bool delete_user(User** head, const char* username) {//ʹ�� User** head ��Ϊ�����޸ĵ��÷�������ͷָ��
    if (!head || !username || !*head) return false;

    User* prev = NULL;// ��ǰ�ڵ��ǰ���ڵ�
    User* curr = *head;

    // ����Ҫɾ�����û�
    while (curr) {
        if (strcmp(curr->username, username) == 0) {
            // �ҵ��û�����ʼɾ��
            if (prev) {
                prev->next = curr->next; // ��ͨ�ڵ�ɾ��
            }
            else {//ǰ���ڵ�=NULL��ǰ���ڵ���ͷ���
                *head = curr->next; // ɾ������ͷ�ڵ�
            }

            // ��ȫ����û���Ϣ
            memset(curr->phone, 0, sizeof(curr->phone));// �����ֻ���
            memset(curr->password, 0, sizeof(curr->password));// ��������
            free(curr);// �ͷ��ڴ�

            return true;
        }
        prev = curr;
        curr = curr->next;
    }

    return false; // δ�ҵ��û�
}

// �����û���Ϣ���ļ�
bool save_users_to_file(User* head, const char* filename) {//�����û�����ͷָ��
    // ����ļ����Ƿ���Ч
    if (!filename || strlen(filename) == 0) {
        return false;
    }

    // д��ģʽ���ļ�
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("Failed to save users");
        return false;
    }

    // д���б���
    if (fprintf(fp, "username,phone,password,level,payment_mode,discount_rate\n") < 0) {//�û������ֻ��ţ����룬�ȼ���֧����ʽ���ۿ���
        fclose(fp);
        return false;
    }

    // ��������д��ÿ���û���Ϣ
    User* curr = head;
    while (curr) {
        if (fprintf(fp, "%s,%s,%s,%d,%d,%.2f\n",
            curr->username,
            curr->phone,
            curr->password,
            curr->level,
            curr->payment_mode,
            curr->discount_rate) < 0) {
            fclose(fp);
            return false;
        }
        curr = curr->next;
    }

    // ����Ƿ�ɹ��ر��ļ�
    if (fclose(fp) != 0) {
        perror("Failed to close users file");
        return false;
    }

    return true;
}

// ���ļ������û���Ϣ��β�巨��
User* load_users_from_file(const char* filename) {
    //ֻ��ģʽ���ļ�
    FILE* fp = fopen(filename, "r");
    if (!fp) return NULL;

    User* head = NULL, * tail = NULL;
    char line[256];

    // ��ȡ��������ͷ
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        char username[50] = { 0 }, phone[12] = { 0 }, password[50] = { 0 };
        int level, payment_mode;
        float discount_rate;

        // ����ÿ������
        if (sscanf_s(line, "%49[^,],%11[^,],%49[^,],%d,%d,%f",//%49[^,]����ȡ��� 49 ���Ƕ����ַ���Ϊ \0 Ԥ���ռ䣩
            username, (unsigned)_countof(username),
            phone, (unsigned)_countof(phone),
            password, (unsigned)_countof(password),
            &level, &payment_mode, &discount_rate) != 6) {
            continue; // ������ʽ����ȷ����
        }

        // �����û�
        User* user = (User*)malloc(sizeof(User));
        if (!user) continue;

        // ������Ϣ(�����ַ���)
        strncpy_s(user->username, sizeof(user->username), username, 49);
        user->username[49] = '\0';// ȷ����ֹ
        strncpy_s(user->phone, sizeof(user->phone), phone, 11);
        user->phone[11] = '\0';
        strncpy_s(user->password, sizeof(user->password), password, 49);
        user->password[49] = '\0';

        // �û��ȼ�����Ȩ
        user->level = (UserLevel)level;
        user->payment_mode = (PaymentMode)payment_mode;
        user->discount_rate = discount_rate;
        user->next = NULL;

        // ��ӵ�����β�巨��
        if (!head) {//headΪNULL
            head = tail = user;// ��һ���ڵ�
        }
        else {
            tail->next = user;
            tail = user;      // ׷�ӵ�����β��
        }
    }

    fclose(fp);
    return head;
}


//-----------------------------------------------------------------------------------
// ========== �û�ע�� ==========
/*�����û����û������ֻ��ţ����룬�û����ͣ�������ռ䣬�û������м��û������浽�ļ�*/
void user_register() {
    clear_screen();
    printf("\n==================== �û�ע�� ====================\n");
    printf("* �û���: 14λ���ڣ�ֻ������ĸ(a-z,A-Z)������(0-9)\n");
    printf("* �ֻ���: ����Ϊ11λ������\n");
    printf("* ����: 40λ���ڣ�ֻ������ĸ(a-z,A-Z)������(0-9)\n\n");

    char username[15], phone[16], password[41], confirm_pwd[41];
    int user_type;
    int valid_input;

    // �����û�������֤
    do {
        valid_input = 1;
        printf("�û���: ");

        // ��ȫ��ȡ����
        // �����ȡʧ�ܵ����
        if (fgets(username, sizeof(username), stdin) == NULL) {//��ֹ���������
            clear_input_buffer();//���������
            valid_input = 0;
            continue;
        }

        // ��������Ƿ����
        size_t input_len = strlen(username);
        if (input_len > 0 && username[input_len - 1] != '\n') {
            // �����Ϊ���з����򳬹��ַ�������
            printf("�����û������ܳ���14���ַ���\n");
            clear_input_buffer();  //���������
            valid_input = 0;
            continue;
        }

        // ȥ�����з�������У�fgets���ȡ\n
        if (input_len > 0 && username[input_len - 1] == '\n') {
            username[input_len - 1] = '\0';//\0�����ֹ
            input_len--;
        }

        // �û�������Ϊ��  �����Ч���ȣ�1-14�ַ���
        if (input_len == 0) {
            printf("�����û�������Ϊ�գ�\n");
            valid_input = 0;
            continue;
        }

        // ����û����Ƿ��Ѵ���
        if (find_user_by_username(user_list, username)) {
            printf("�û����Ѵ��ڣ�\n");
            valid_input = 0; // ��ѭ������
            continue;
        }

        // �ϸ���֤�û�����ʽ������ĸ���֣�
        for (int i = 0; username[i] != '\0'; i++) {
            if (!isalnum((unsigned char)username[i])) {
                printf("�����û���ֻ�ܰ�����ĸ(a-z,A-Z)������(0-9)��\n");
                valid_input = 0;
                break;
            }
        }
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
        confirm_pwd[strcspn(confirm_pwd, "\n")] = '\0';//�ҵ� \n ��λ�ú��� '\0'���ַ�����ֹ�����滻��

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
        if (strchr(input, '\n') == NULL) {//�����Ƿ��л��з�
            printf("\n���������ע��ʧ�ܣ�\n");
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
        clear_input_buffer();
        return;
    }


    add_user(&user_list, new_user);//(���û���ӵ��û�������)
    user_count++;//�û�����һ
    // �����û�����
    save_users_to_file(user_list, "users.txt");

    printf("\nע��ɹ�����ӭ���������վ��%s\n", username);
    printf("�����˻�����: ");
    switch (new_user->level) {
    case USER_NORMAL:    printf("��ͨ�û�"); break;
    case USER_STUDENT:  printf("ѧ���û�(85��)"); break;
    case USER_VIP:      printf("VIP�û�(9��)"); break;
    case USER_ENTERPRISE: printf("��ҵ�û�(75��)"); break;
    case USER_PARTNER:  printf("�����̻�(Э���)"); break;
    }
    clear_input_buffer();
}

// ========== �û���¼ ==========
/*�����û������ֻ����Լ����룬���û������в���ƥ�䣬��3�λ��ᣨͨ��whileѭ����*/
void user_login() {
    int attempts = 3;

    while (attempts > 0) {
        clear_screen();
        printf("\n========== �û���¼ ==========\n");
        printf("ʣ�ೢ�Դ���: %d\n", attempts);
        char input[50] = { 0 };
        char password[50] = { 0 };

        // ��ȡ�û���/�ֻ���
        printf("�û������ֻ���: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {//��fgets��ȫ
            clear_input_buffer();
            printf("�������\n");
            pause_program();
            clear_input_buffer();
            continue;
        }
        input[strcspn(input, "\n")] = '\0';  // ȥ�����з�

        // ��ȡ����
        printf("����: ");
        if (fgets(password, sizeof(password), stdin) == NULL) {
            clear_input_buffer();
            printf("�������\n");
            pause_program();
            clear_input_buffer();
            continue;
        }
        password[strcspn(password, "\n")] = '\0';  // ȥ�����з�


        // �����û������������(ͨ���û��������ֻ���)
        User* user = NULL;
        User* curr = user_list;  // user_list ������ͷָ��
        while (curr) {
            if (strcmp(curr->username, input) == 0 ||
                strcmp(curr->phone, input) == 0) {
                user = curr;
                break;
            }
            curr = curr->next;
        }

        if (!user) {
            attempts--;
            printf("\n�����û������ֻ��Ų����ڣ�");
            clear_input_buffer();
        }
        else if (strcmp(user->password, password) != 0) {
            attempts--;
            printf("\n�������벻��ȷ��");
            clear_input_buffer();

        }
        else {
            printf("\n��¼�ɹ�����ӭ������%s\n", user->username);
            pause_program();
            clear_input_buffer();
            user_menu(user);//����˵�
            return;
        }

        if (attempts > 0) {
            printf(" ʣ�ೢ�Դ���: %d\n", attempts);
            clear_input_buffer();
        }
        else {
            printf("\n���Դ��������꣬�������˵���\n");
            pause_program();
            clear_input_buffer();
            return;  // �������˵�
        }
    }


}

// ========== �û��鿴�Լ���Ϣ ==========
void view_my_info(User* user) {
    if (!user) return;

    clear_screen();
    printf("\n========== �ҵ��˻���Ϣ ==========\n");

    // ������Ϣ
    printf("\n��������Ϣ��\n");
    printf("�û���: %s\n", user->username);
    printf("�ֻ���: %s\n", user->phone);

    // �˻�����
    printf("\n���˻����͡�\n");
    printf("�û��ȼ�: ");
    switch (user->level) {
    case USER_NORMAL:    printf("��ͨ�û�"); break;
    case USER_STUDENT:   printf("ѧ���û� (����85���Ż�)"); break;
    case USER_VIP:       printf("VIP�û� (����9���Ż�)"); break;
    case USER_ENTERPRISE:printf("��ҵ�û� (����75���Ż�)"); break;
    case USER_PARTNER:   printf("�����̻� (����Э���)"); break;
    }
    printf("\n�ۿ���: %.2f\n", user->discount_rate);

    // ֧����ʽ
    printf("\n��֧����ʽ��\n");
    printf("��ǰ֧����ʽ: ");
    switch (user->payment_mode) {
    case PAY_CASH:    printf("�ָ�"); break;
    case PAY_MONTHLY: printf("�½�"); break;
    case PAY_CONTRACT:printf("Э�����ڽ���"); break;
    }

    printf("\n----------------------------------\n");
    pause_program();
    clear_input_buffer();
}

// ========== �û��޸��Լ���Ϣ ==========
/*����ʾ�û�������Ϣ����ѡ���޸ĵ����ע��ʱһ���ĸ�ʽҪ�󣬱�����Ϣ���ļ�*/
bool modify_user_info(User* user) {
    if (!user) return false;

    clear_screen();
    printf("\n========== �޸��û���Ϣ ==========\n");
    printf("��ǰ��Ϣ��\n");
    printf("1. �û���: %s\n", user->username);
    printf("2. �ֻ���: %s\n", user->phone);
    //printf("3. ����: %s\n", user->password);
    printf("3. ����: ******\n"); //��*��ʾ�����Ӱ�ȫ   

    printf("0. ����\n");
    printf("\n��ѡ��Ҫ�޸ĵ���Ŀ (1-3, 0ȡ��): ");

    int choice;
    char input[10];
    fgets(input, sizeof(input), stdin);
    if (sscanf(input, "%d", &choice) != 1) {
        printf("��Ч���룡\n");
        pause_program();
        clear_input_buffer();
        return false;
    }
    switch (choice) {
    case 1: { // �޸��û���
        char new_username[15];
        int valid_input;//��֤��ʽ�Ƿ���ȷ�ı�־

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
                clear_input_buffer();
                valid_input = 0;
                continue;
            }

            // ����û����Ƿ��Ѵ���
            if (find_user_by_username(user_list, new_username)) {
                printf("�����û�����ע�����ԭ�û�����ͬ��\n");
                clear_input_buffer();
                valid_input = 0;
                continue;
            }

            // ��֤�û�����ʽ
            for (int i = 0; new_username[i] != '\0'; i++) {
                if (!isalnum((unsigned char)new_username[i])) {
                    printf("�����û���ֻ�ܰ�����ĸ(a-z,A-Z)������(0-9)��\n");
                    clear_input_buffer();
                    valid_input = 0;
                    break;
                }
            }
        } while (!valid_input);

        strncpy_s(user->username, sizeof(user->username), new_username, 14);//����
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
            if (valid_input && find_user_by_phone(user_list, new_phone) &&
                strcmp(new_phone, user->phone) != 0) {
                printf("���󣺸��ֻ�����ע�����ԭ�ֻ�����ͬ\n");
                valid_input = 0;
            }
        } while (!valid_input);

        strncpy_s(user->phone, sizeof(user->phone), new_phone, 11);
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

        strncpy_s(user->password, sizeof(user->password), new_password, 40);
        printf("�����޸ĳɹ���\n");
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
    save_users_to_file(user_list, "users.txt");
    clear_input_buffer();
    return true;
}

// ========== �û�ע���Լ� ==========
/*����������֤�����λ��ᣬע��ʱ���û��Ƴ������û�ָ����NULL*/
bool user_unregister(User** user_list, int* user_count, User** current_user) {
    if (!user_list || !*current_user) return false;
    clear_screen();
    printf("\n========== �˻�ע�� ==========\n");

    // ��ʾ��ǰ�û���Ϣ
    User* user = *current_user;
    printf("������ע�������˻���\n");
    printf("�û���: %s\n", user->username);
    printf("�ֻ���: %s\n", user->phone);
    printf("�û�����: ");
    switch (user->level) {
    case USER_NORMAL:    printf("��ͨ�û�"); break;
    case USER_STUDENT:   printf("ѧ���û�"); break;
    case USER_VIP:       printf("VIP�û�"); break;
    case USER_ENTERPRISE: printf("��ҵ�û�"); break;
    case USER_PARTNER:   printf("�����̻�"); break;
    }
    printf("\n\n");

    // ������֤
    int attempts = 0;
    const int max_attempts = 3;
    bool authenticated = false;//���������Ƿ���ȷ�ı��

    while (attempts < max_attempts && !authenticated) {
        printf("����ʣ�����: %d/%d\n", max_attempts - attempts, max_attempts);
        printf("��������������ȷ��ע��������0ȡ����: ");

        char password[50];
        if (fgets(password, sizeof(password), stdin) == NULL) {
            clear_input_buffer();
            continue;
        }
        password[strcspn(password, "\n")] = '\0';

        // ȡ������(��0ȡ��)
        if (strcmp(password, "0") == 0) {
            printf("\n��ȡ��ע��������\n");
            clear_input_buffer();
            return false;
        }

        // ��֤����
        if (strcmp(password, user->password) == 0) {
            authenticated = true;
        }
        else {
            attempts++;
            if (attempts < max_attempts) {
                printf("\n�������벻��ȷ��\n");
                clear_input_buffer();
                clear_screen();
                printf("\n========== �˻�ע�� ==========\n");
                printf("�û���: %s\n", user->username);
            }
        }
    }

    if (!authenticated) {
        printf("\n����������֤ʧ�ܴ������࣡\n");
        printf("���ڰ�ȫ���ǣ�����ֹע�����̡�\n");
        clear_input_buffer();
        return false;
    }

    // ����ȷ��
    printf("\n���棺�˲������ɳ�����\n");
    printf("ȷ��Ҫ����ע���˻���(y): ");
    char confirm = getchar();
    clear_input_buffer();

    if (tolower(confirm) != 'y') {
        printf("\n��ȡ��ע��������\n");
        pause_program();
        clear_input_buffer();
        return false;
    }

    // ִ��ע��
    if (delete_user(user_list, user->username)) {
        (*user_count)--;
        save_users_to_file(*user_list, "users.txt");

        printf("\n�˻��ѳɹ�ע����\n");
        printf("��л��ʹ�����ǵķ���\n");
        *current_user = NULL; // �����ǰ��¼״̬

        clear_input_buffer();
        return true;
    }
    else {
        printf("\n����ע��ʧ�ܣ�����ϵ����Ա��\n");
        clear_input_buffer();
        return false;
    }

}

