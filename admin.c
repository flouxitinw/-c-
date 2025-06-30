#include "cainiao.h"

// 创建管理员
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

// 添加管理员（头插法）//用户数量加一
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
    admin_count++; // 统一在此处维护计数器
}

// 根据用户名查找管理员
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

// 根据手机号查找用户
Admin* find_admin_by_phone(User* head, const char* phone) {
    // 检查 phone 是否为 NULL
    if (!phone) return NULL;
    Admin* curr = head;
    while (curr) {
        if (strcmp(curr->phone, phone) == 0) // 字符串完全匹配
            return curr;
        curr = curr->next;
    }
    return NULL;
}

// 保存管理员数据到文件
bool save_admins_to_file(Admin* head, const char* filename) {
    // 检查输入参数有效性
    if (!filename || strlen(filename) == 0) {
        fprintf(stderr, "错误：无效的文件名\n");
        return false;
    }
    // 尝试打开文件
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("保存管理员数据失败 - 无法打开文件");
        return false;
    }

    // 写入列标题
    if (fprintf(fp, "username,phone,password,privilege_level\n") < 0) {
        fclose(fp);
        fprintf(stderr, "错误：写入列标题失败\n");
        return false;
    }

    // 遍历链表并写入每个管理员数据
    Admin* curr = head;
    while (curr) {
        // 写入管理员数据，检查是否成功
        if (fprintf(fp, "\"%s\",\"%s\",\"%s\",%d\n",
            curr->username,
            curr->phone,
            curr->password,
            curr->privilege_level) < 0) {
            fclose(fp);
            fprintf(stderr, "错误：写入管理员数据失败\n");
            return false;
        }
        curr = curr->next;
    }

    // 确保文件正确关闭
    if (fclose(fp) != 0) {
        perror("保存管理员数据失败 - 关闭文件时出错");
        return false;
    }

    return true;
}

// 从文件加载管理员数据（尾插法）
Admin* load_admins_from_file(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) return NULL;

    Admin* head = NULL, * tail = NULL;
    char line[256];
    fgets(line, sizeof(line), fp); // 跳过表头

    while (fgets(line, sizeof(line), fp)) {
        // 跳过空行和注释行（以 # 开头）
        if (line[0] == '\n' || line[0] == '#' || strlen(line) < 5) continue;

        char username[MAX_USERNAME_LEN] = { 0 };
        char phone[MAX_PHONE_LEN] = { 0 };
        char password[MAX_PASSWORD_LEN] = { 0 };
        int privilege_level = 0;

        // 安全解析字段（允许字段包含逗号，需用引号包裹）
        if (sscanf_s(line, "\"%49[^\"]\",\"%11[^\"]\",\"%49[^\"]\",%d",
            username, (unsigned)_countof(username),
            phone, (unsigned)_countof(phone),
            password, (unsigned)_countof(password),
            &privilege_level) != 4)
        {
            fprintf(stderr, "跳过无效行: %s", line);
            continue;
        }

        // 验证字段有效性
        if (strlen(username) == 0 || strlen(phone) != 11 || strlen(password) == 0) {
            fprintf(stderr, "字段校验失败: %s", line);
            continue;
        }

        Admin* admin = create_admin(username, phone, password, privilege_level);
        if (!admin) {
            fprintf(stderr, "创建管理员失败: %s", line);
            continue;
        }

        // 添加到链表
        if (!head) {
            head = tail = admin;
        }
        else {
            tail->next = admin;
            tail = admin;
        }
        admin_count++; // 仅在成功添加时增加计数
    }

    fclose(fp);
    return head;
}

// 修改管理员权限级别(只有超级管理员可以)
bool modify_admin_privilege(Admin* executor, Admin* target) {
    // 权限检查
    if (!is_super_admin(executor)) {
        printf("\n权限不足！只有超级管理员可以修改权限级别。\n");
        return false;
    }

    // 检查是否尝试修改自己
    if (strcmp(target->username, executor->username) == 0) {
        printf("\n不能修改自己的权限级别！\n");
        return false;
    }

    // 显示当前权限
    printf("\n目标管理员: %s\n", target->username);
    printf("当前权限: %s\n",
        target->privilege_level == SUPER_ADMIN ? "超级管理员" : "普通管理员");

    // 获取新权限级别
    int new_level;
    printf("\n请选择新的权限级别:\n");
    printf("1. 普通管理员\n");
    printf("2. 超级管理员\n");
    printf("0. 取消操作\n");
    printf("请输入选择(0-2): ");

    // 输入验证
    while (1) {
        if (scanf_s("%d", &new_level) != 1) {
            clear_input_buffer();
            printf("输入无效，请输入0-2的数字: ");
            continue;
        }

        if (new_level == 0) {
            printf("\n已取消权限修改操作\n");
            return false;
        }

        if (new_level == 1 || new_level == 2) {
            break;
        }

        printf("无效选择，请输入0-2的数字: ");
    }
    clear_input_buffer();

    // 执行修改
    target->privilege_level = (AdminPrivilegeLevel)(new_level - 1);
    printf("\n已成功将 %s 的权限修改为%s管理员\n",
        target->username,
        new_level == 2 ? "超级" : "普通");

    return true;
}

// 检查是否是超级管理员
bool is_super_admin(const Admin* admin) {
    return admin && admin->privilege_level == SUPER_ADMIN;
}

// 删除管理员
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

// ========== 管理员注册 ==========
void admin_register(Admin* executor) {
    clear_screen();
    printf("\n=================== 管理员注册 ===================\n");
    printf("* 用户名: 14位以内，只包含字母(a-z,A-Z)和数字(0-9)\n");
    printf("* 密码: 40位以内，只包含字母(a-z,A-Z)和数字(0-9)\n");
    printf("* 手机号: 必须为11位纯数字\n");
    char username[15], phone[16], password[41], confirm_pwd[41];
    int privilege_level = SUPER_ADMIN;
    int valid_input;
    int first_attempt = 1;  // 首次尝试标志//首次进入循环时，如果用户直接按回车，不会立即显示"用户名不能为空"的错误 首次注册时从一个页面过来会按enter

    // 输入用户名并验证
    do {
        valid_input = 1;
        if (!first_attempt) {
            printf("用户名: ");
        }

        // 安全获取输入
        if (fgets(username, sizeof(username), stdin) == NULL) {
            clear_input_buffer();
            valid_input = 0;
            first_attempt = 0;
            continue;
        }

        // 检测输入是否过长
        size_t input_len = strlen(username);
        if (input_len > 0 && username[input_len - 1] != '\n') {
            printf("错误：用户名不能超过14个字符！\n");
            clear_input_buffer();
            valid_input = 0;
            first_attempt = 0;
            continue;
        }

        // 去除换行符（如果有）
        if (input_len > 0 && username[input_len - 1] == '\n') {
            username[input_len - 1] = '\0';
            input_len--;
        }

        // 空输入检查逻辑
        if (input_len == 0) {
            valid_input = 0;
            // 只有非首次尝试才显示错误
            if (!first_attempt) {
                printf("错误：用户名不能为空！\n");
            }
            first_attempt = 0;
            continue;
        }

        // 检查用户名是否已存在
        if (find_admin_by_username(admin_list, username)) {
            printf("\n用户名已存在！\n");
            valid_input = 0;
            first_attempt = 0;
            continue;
        }

        // 严格验证用户名格式（仅字母数字）
        for (int i = 0; username[i] != '\0'; i++) {
            if (!isalnum((unsigned char)username[i])) {
                printf("错误：用户名只能包含字母(a-z,A-Z)和数字(0-9)！\n");
                valid_input = 0;
                first_attempt = 0;
                break;
            }
        }
        first_attempt = 0;  // 标记已进行过至少一次验证
    } while (!valid_input);

    // 输入手机号并验证
    do {
        valid_input = 1;
        printf("手机号(必须11位数字): ");

        // 安全获取输入
        if (fgets(phone, sizeof(phone), stdin) == NULL) {
            clear_input_buffer();
            valid_input = 0;
            continue;
        }

        // 检测输入是否过长（超过11位）
        size_t input_len = strlen(phone);
        if (input_len > 0 && phone[input_len - 1] != '\n') {
            printf("错误：手机号必须为11位数字！\n");
            clear_input_buffer();
            valid_input = 0;
            continue;
        }

        // 去除换行符（如果有）
        if (input_len > 0 && phone[input_len - 1] == '\n') {
            phone[input_len - 1] = '\0';
            input_len--;
        }

        // 检查长度是否为11位
        if (input_len != 11) {
            printf("错误：手机号必须为11位数字！当前输入：%zu位\n", input_len);
            valid_input = 0;
            continue;
        }

        // 验证是否为纯数字
        for (int i = 0; i < 11; i++) {
            if (!isdigit((unsigned char)phone[i])) {
                printf("错误：手机号必须为纯数字！第%d位非法\n", i + 1);
                valid_input = 0;
                break;
            }
        }
        // 检查手机号是否已注册
        if (valid_input && find_admin_by_phone(admin_list, phone)) {
            printf("该手机号已注册！请使用其他手机号。\n");
            valid_input = 0;
        }

    } while (!valid_input);

    // 密码输入及确认
    do {
        do {
            valid_input = 1;
            printf("密码: ");

            // 安全获取输入
            if (fgets(password, sizeof(password), stdin) == NULL) {
                clear_input_buffer();
                valid_input = 0;
                continue;
            }

            // 检测输入是否被截断（超过缓冲区大小）
            size_t input_len = strlen(password);
            if (input_len > 0 && password[input_len - 1] != '\n') {
                printf("错误：密码过长（超过 %zu 字节）！请重新输入。\n", sizeof(password) - 1);
                clear_input_buffer();
                valid_input = 0;
                continue;
            }

            // 去除换行符
            if (input_len > 0 && password[input_len - 1] == '\n') {
                password[input_len - 1] = '\0';
                input_len--;
            }

            // 密码不能为空
            if (input_len == 0) {
                printf("错误：密码不能为空！\n");
                valid_input = 0;
                continue;
            }

            // 验证密码长度（不超过40字符）
            if (input_len > 40) {
                printf("错误：密码不能超过40个字符！当前输入：%zu 位\n", input_len);
                valid_input = 0;
                continue;
            }

            // 严格验证密码格式（仅字母数字）
            for (int i = 0; password[i] != '\0'; i++) {
                if (!isalnum((unsigned char)password[i])) {
                    printf("错误：密码只能包含字母(a-z,A-Z)和数字(0-9)！\n");
                    valid_input = 0;
                    break;
                }
            }
        } while (!valid_input);

        printf("确认密码: ");
        if (fgets(confirm_pwd, sizeof(confirm_pwd), stdin) == NULL) {
            clear_input_buffer();
            continue;
        }
        confirm_pwd[strcspn(confirm_pwd, "\n")] = '\0';

        if (strcmp(password, confirm_pwd) != 0) {
            printf("\n两次输入的密码不一致！请重新输入。\n\n");
        }
    } while (strcmp(password, confirm_pwd) != 0);

    // 如果是超级管理员执行注册，可以选择权限级别
    if (executor && executor->privilege_level == SUPER_ADMIN) {//若没有管理员首次注册传的是NULL，则不显示
        printf("权限等级 (1-普通管理员, 2-超级管理员)[默认1]: ");
        char level_input[10];
        fgets(level_input, sizeof(level_input), stdin);
        if (strlen(level_input) > 0) {
            privilege_level = atoi(level_input);
            if (privilege_level < NORMAL_ADMIN || privilege_level > SUPER_ADMIN) {
                privilege_level = NORMAL_ADMIN;//输入其他则注册普通管理员
            }
        }
    }

    Admin* new_admin = create_admin(username, phone, password, privilege_level);
    if (!new_admin) {
        printf("\n注册失败！\n");
        pause_program();
        clear_input_buffer();
        return;
    }

    add_admin(&admin_list, new_admin);//其中已经将管理员数量增加
    save_admins_to_file(admin_list, "admins.txt");
    printf("\n成功注册%s管理员: %s\n",
        privilege_level == SUPER_ADMIN ? "超级" : "普通",
        username);
    clear_input_buffer();
}

// ========== 管理员登录 ==========
/*若没有管理员，则先注册管理员，传入NULL，则注册高级管理员*/
void admin_login() {
    clear_screen();
    if (admin_list == NULL) {
        printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        printf("!!!!!!  首次系统运行，必须创建超级管理员账户！ !!!!!!\n");
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
        pause_program();
        clear_input_buffer();

        admin_register(NULL);//传入NULL

        return;  // 注册后直接返回，避免重复登录
    }

    int attempts = 3;
    while (attempts > 0) {
        clear_screen();
        printf("\n========== 管理员登录 ==========\n");
        printf("剩余尝试次数: %d\n", attempts);

        char input[50], password[50];

        // 输入用户名/手机号
        printf("用户名或手机号: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            clear_input_buffer();
            continue;
        }
        input[strcspn(input, "\n")] = '\0'; // 去除换行符

        // 输入密码
        printf("密码: ");
        if (fgets(password, sizeof(password), stdin) == NULL) {
            clear_input_buffer();
            printf("输入错误！\n");
            pause_program();
            clear_input_buffer();
            continue;
        }
        password[strcspn(password, "\n")] = '\0';  // 去除换行符

        // 查找管理员账户
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

        // 验证登录
        if (!admin || strcmp(admin->password, password) != 0) {
            attempts--;
            printf("\n用户名/手机号或密码错误！");
            if (attempts > 0) {
                printf(" 请重试。\n");
                pause_program();
                clear_input_buffer();   // 再次清理暂停时的按键
            }
            else {
                printf("\n尝试次数已用完，返回主菜单。\n");
                pause_program();
                clear_input_buffer();
                return;  // 返回主菜单
            }
            continue;

        }

        // 登录成功
        printf("\n登录成功！欢迎回来，%s %s\n",
            admin->privilege_level == SUPER_ADMIN ? "[超级管理员]" : "[普通管理员]",
            admin->username);
        pause_program();
        clear_input_buffer();
        admin_menu(admin);
        return;
    }
}


// ========== 查看所有用户 ==========
/*遍历用户链表，输出用户信息*/
void view_all_users() {
    clear_screen();
    printf("\n============================== 所有用户列表 ==============================\n");

    if (user_list == NULL) {
        printf("\n当前没有注册用户！\n");
        pause_program();
        clear_input_buffer();
        return;
    }

    // 调整列宽使对齐更合理
    printf("%-20s %-15s %-15s %-10s %-15s\n",
        "用户名", "手机号", "用户类型", "折扣率", "支付方式");
    printf("--------------------------------------------------------------------------\n");

    User* current = user_list;
    while (current != NULL) {
        // 获取用户类型字符串
        char user_type[20];
        switch (current->level) {
        case USER_NORMAL: strcpy_s(user_type, sizeof(user_type), "普通用户"); break;
        case USER_STUDENT: strcpy_s(user_type, sizeof(user_type), "学生用户"); break;
        case USER_VIP: strcpy_s(user_type, sizeof(user_type), "VIP用户"); break;
        case USER_ENTERPRISE: strcpy_s(user_type, sizeof(user_type), "企业用户"); break;
        case USER_PARTNER: strcpy_s(user_type, sizeof(user_type), "合作商户"); break;
        }

        // 获取支付方式字符串
        char payment_mode[20];
        switch (current->payment_mode) {
        case PAY_CASH: strcpy_s(payment_mode, sizeof(payment_mode), "现付"); break;
        case PAY_MONTHLY: strcpy_s(payment_mode, sizeof(payment_mode), "月结"); break;
        case PAY_CONTRACT: strcpy_s(payment_mode, sizeof(payment_mode), "协议结算"); break;
        }

        printf("%-20s %-15s %-15s %-10.2f %-15s\n",
            current->username,
            current->phone,
            user_type,
            current->discount_rate,
            payment_mode);

        current = current->next;
    }

    printf("\n总计: %d 位用户\n", user_count);
    pause_program();
    clear_input_buffer();
}

// ========== 管理员添加用户  ==========
/*先显示所有用户，同注册用户一个思路，加入一个标记（调用显示所有用户后，按enter时转入到添加用户时相当于输入内容）*/
void admin_add_user() {
    clear_screen();
    // 先显示所有用户
    view_all_users();

    printf("========== 添加用户 ==========\n");
    printf("* 用户名: 14位以内，只包含字母(a-z,A-Z)和数字(0-9)\n");
    printf("* 密码: 40位以内，只包含字母(a-z,A-Z)和数字(0-9)\n");
    printf("* 手机号: 必须为11位纯数字\n\n");

    char username[15], phone[16], password[41], confirm_pwd[41];
    int user_type;
    int valid_input;
    int first_attempt = 1;  // 新增首次尝试标志

    // 输入用户名并验证
    do {
        valid_input = 1;
        if (!first_attempt) {
            printf("用户名: ");
        }

        // 安全获取输入
        if (fgets(username, sizeof(username), stdin) == NULL) {
            clear_input_buffer();
            valid_input = 0;
            first_attempt = 0;
            continue;
        }

        // 检测输入是否过长
        size_t input_len = strlen(username);
        if (input_len > 0 && username[input_len - 1] != '\n') {
            printf("错误：用户名不能超过14个字符！\n");
            clear_input_buffer();
            valid_input = 0;
            first_attempt = 0;
            continue;
        }

        // 去除换行符（如果有）
        if (input_len > 0 && username[input_len - 1] == '\n') {
            username[input_len - 1] = '\0';
            input_len--;
        }

        // 修改后的空输入检查逻辑
        if (input_len == 0) {
            valid_input = 0;
            // 只有非首次尝试才显示错误
            if (!first_attempt) {
                printf("错误：用户名不能为空！\n");
            }
            first_attempt = 0;
            continue;
        }

        // 检查用户名是否已存在
        if (find_user_by_username(user_list, username)) {
            printf("\n用户名已存在！\n");
            valid_input = 0;
            first_attempt = 0;
            continue;
        }

        // 严格验证用户名格式
        for (int i = 0; username[i] != '\0'; i++) {
            if (!isalnum((unsigned char)username[i])) {
                printf("错误：用户名只能包含字母(a-z,A-Z)和数字(0-9)！\n");
                valid_input = 0;
                first_attempt = 0;
                break;
            }
        }
        first_attempt = 0;  // 标记已进行过至少一次验证
    } while (!valid_input);

    do {
        valid_input = 1;
        printf("手机号(必须11位数字): ");

        // 安全获取输入
        if (fgets(phone, sizeof(phone), stdin) == NULL) {
            clear_input_buffer();
            valid_input = 0;
            continue;
        }

        // 检测输入是否过长（超过11位）
        size_t input_len = strlen(phone);
        if (input_len > 0 && phone[input_len - 1] != '\n') {
            printf("错误：手机号必须为11位数字！\n");
            clear_input_buffer();
            valid_input = 0;
            continue;
        }

        // 去除换行符（如果有）
        if (input_len > 0 && phone[input_len - 1] == '\n') {
            phone[input_len - 1] = '\0';
            input_len--;
        }

        // 检查长度是否为11位
        if (input_len != 11) {
            printf("错误：手机号必须为11位数字！当前输入：%zu位\n", input_len);
            valid_input = 0;
            continue;
        }

        // 验证是否为纯数字
        for (int i = 0; i < 11; i++) {
            if (!isdigit((unsigned char)phone[i])) {
                printf("错误：手机号必须为纯数字！第%d位非法\n", i + 1);
                valid_input = 0;
                break;
            }
        }

        // 检查手机号是否已注册
        if (valid_input && find_user_by_phone(user_list, phone)) {
            printf("该手机号已注册！请使用其他手机号。\n");
            valid_input = 0;
        }
    } while (!valid_input);

    // 密码输入及确认
    do {
        do {
            valid_input = 1;
            printf("密码: ");

            // 安全获取输入
            if (fgets(password, sizeof(password), stdin) == NULL) {
                clear_input_buffer();
                valid_input = 0;
                continue;
            }

            // 检测输入是否被截断（超过缓冲区大小）
            size_t input_len = strlen(password);
            if (input_len > 0 && password[input_len - 1] != '\n') {
                printf("错误：密码过长（超过 %zu 字节）！请重新输入。\n", sizeof(password) - 1);
                clear_input_buffer();
                valid_input = 0;
                continue;
            }

            // 去除换行符
            if (input_len > 0 && password[input_len - 1] == '\n') {
                password[input_len - 1] = '\0';
                input_len--;
            }

            // 密码不能为空
            if (input_len == 0) {
                printf("错误：密码不能为空！\n");
                valid_input = 0;
                continue;
            }

            // 验证密码长度（不超过40字符）
            if (input_len > 40) {
                printf("错误：密码不能超过40个字符！当前输入：%zu 位\n", input_len);
                valid_input = 0;
                continue;
            }

            // 严格验证密码格式（仅字母数字）
            for (int i = 0; password[i] != '\0'; i++) {
                if (!isalnum((unsigned char)password[i])) {
                    printf("错误：密码只能包含字母(a-z,A-Z)和数字(0-9)！\n");
                    valid_input = 0;
                    break;
                }
            }
        } while (!valid_input);

        printf("确认密码: ");
        if (fgets(confirm_pwd, sizeof(confirm_pwd), stdin) == NULL) {
            clear_input_buffer();
            continue;
        }
        confirm_pwd[strcspn(confirm_pwd, "\n")] = '\0';

        if (strcmp(password, confirm_pwd) != 0) {
            printf("\n两次输入的密码不一致！请重新输入。\n\n");
        }
    } while (strcmp(password, confirm_pwd) != 0);

    // 选择用户类型
    printf("\n请选择用户类型:\n");
    printf("1. 普通用户\n");
    printf("2. 学生用户\n");
    printf("3. VIP用户\n");
    printf("4. 企业用户\n");
    printf("5. 合作商户\n");
    printf("请输入选择(1-5): ");

    while (1) {
        char input[12];
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n输入错误，注册失败！\n");
            return; // 直接退出注册流程
        }

        // 检查输入是否过长（没有换行符表示输入被截断）
        if (strchr(input, '\n') == NULL) {
            printf("\n输入过长，添加失败！\n");
            clear_input_buffer(); // 清空缓冲区剩余内容
            pause_program();
            return; // 直接退出注册流程
        }

        // 去除换行符
        input[strcspn(input, "\n")] = '\0';

        // 验证是否为单个数字1-5
        if (strlen(input) != 1 || !isdigit(input[0])) {
            printf("无效选择，请重新输入(1-5): ");
            continue;
        }

        user_type = input[0] - '0';
        if (user_type >= 1 && user_type <= 5) {
            break;
        }

        printf("无效选择，请重新输入(1-5): ");
    }


    // 创建用户
    User* new_user = create_user(phone, password, username, (UserLevel)(user_type - 1));
    if (!new_user) {
        printf("\n注册失败！\n");
        pause_program();
        return;
    }

    // 保存用户数据
    add_user(&user_list, new_user);
    user_count++;
    save_users_to_file(user_list, "users.txt");

    printf("\n注册成功！");
    clear_input_buffer();
    printf("添加的用户的账户类型: ");
    switch (new_user->level) {
    case USER_NORMAL:    printf("普通用户"); break;
    case USER_STUDENT:  printf("学生用户(85折)"); break;
    case USER_VIP:      printf("VIP用户(9折)"); break;
    case USER_ENTERPRISE: printf("企业用户(75折)"); break;
    case USER_PARTNER:  printf("合作商户(协议价)"); break;
    }
    printf("\n");
    clear_input_buffer(); // 清空缓冲区剩余内容
}


// ========== 查看所有管理员信息 ==========
bool view_all_admins() {
    clear_screen();
    printf("\n===================== 所有管理员列表 ======================\n");

    // 检查管理员列表是否为空
    if (admin_list == NULL) {
        printf("\n当前没有注册管理员！\n");
        pause_program();
        clear_input_buffer();
        return false;  // 返回false表示没有数据显示
    }

    // 表格标题
    printf("%-20s %-15s %-15s %-20s\n",
        "用户名", "手机号", "权限级别", "密码");
    printf("-----------------------------------------------------------\n");

    // 遍历管理员链表
    Admin* current = admin_list;
    int count = 0;
    bool display_success = true;

    while (current != NULL) {
        // 获取权限级别字符串
        const char* privilege_level;
        switch (current->privilege_level) {
        case SUPER_ADMIN:
            privilege_level = "超级管理员";
            break;
        case NORMAL_ADMIN:
            privilege_level = "普通管理员";
            break;
        default:
            privilege_level = "未知";
            display_success = false; // 遇到未知权限级别视为显示问题
        }

        // 打印完整管理员信息（包含原始密码）
        printf("%-20s %-15s %-15s ",
            current->username,
            current->phone,
            privilege_level);
        printf("******\n");

        // 移动到下一个节点
        current = current->next;
        count++;

        // 检查链表是否损坏
        if (count > 1000) { // 防止无限循环
            printf("\n警告：检测到可能的链表循环！\n");
            display_success = false;
            break;
        }
    }

    printf("\n总计: %d 位管理员\n", count);
    pause_program();
    clear_input_buffer();

    return display_success;  // 返回显示是否完全成功
}

// ========== 超级管理员修改其他管理员信息 ==========
void super_modify_admin_info(Admin* super_admin) {
    if (!is_super_admin(super_admin)) {
        printf("\n权限不足！只有超级管理员可以修改其他管理员信息。\n");
        pause_program();
        clear_input_buffer();
        return;
    }

    clear_screen();
    printf("\n========== 修改管理员信息 ==========\n");

    // 显示所有管理员列表
    view_all_admins();

    char target_username[MAX_USERNAME_LEN] = { 0 };
    printf("\n请输入要修改的管理员用户名: ");
    if (fgets(target_username, sizeof(target_username), stdin) == NULL) {
        clear_input_buffer();
        printf("输入无效！\n");
        pause_program();
        clear_input_buffer();
        return;
    }
    target_username[strcspn(target_username, "\n")] = '\0';

    Admin* target = find_admin_by_username(admin_list, target_username);
    if (!target) {
        printf("找不到指定管理员: %s\n", target_username);
        pause_program();
        clear_input_buffer();
        return;
    }

    if (strcmp(target->username, super_admin->username) == 0) {
        printf("不能修改自己的信息，请使用个人资料修改功能！\n");
        pause_program();
        clear_input_buffer();
        return;
    }

    // 修改操作循环
    while (1) {
        clear_screen();
        printf("\n========== 正在修改管理员信息 ==========\n");
        printf("管理员: %s\n", target->username);
        printf("当前信息:\n");
        printf("1. 用户名: %s\n", target->username);
        printf("2. 手机号: %s\n", target->phone);
        printf("3. 密码: ******\n");
        printf("4. 权限级别: %s\n",
            target->privilege_level == SUPER_ADMIN ? "超级管理员" : "普通管理员");
        printf("0. 返回上级菜单\n");
        printf("\n请选择要修改的项目: ");

        int choice;
        char input[10];
        fgets(input, sizeof(input), stdin);
        if (sscanf(input, "%d", &choice) != 1) {
            clear_input_buffer();
            printf("无效的输入！\n");
            pause_program();
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) {
        case 0:
            return;
        case 1: { // 修改用户名
            char new_username[15];
            int valid_input;

            do {
                valid_input = 1;
                printf("新用户名(1-14位字母数字): ");

                if (fgets(new_username, sizeof(new_username), stdin) == NULL) {
                    clear_input_buffer();
                    valid_input = 0;
                    continue;
                }

                // 检测输入是否过长
                size_t input_len = strlen(new_username);
                if (input_len > 0 && new_username[input_len - 1] != '\n') {
                    printf("错误：用户名不能超过14个字符！\n");
                    clear_input_buffer();
                    valid_input = 0;
                    continue;
                }

                // 去除换行符
                if (input_len > 0 && new_username[input_len - 1] == '\n') {
                    new_username[input_len - 1] = '\0';
                    input_len--;
                }

                // 检查有效长度
                if (input_len == 0) {
                    printf("错误：用户名不能为空！\n");
                    valid_input = 0;
                    continue;
                }

                // 检查用户名是否已存在
                if (find_admin_by_username(admin_list, new_username)) {
                    printf("错误：用户名已注册或与原用户名相同！\n");
                    valid_input = 0;
                    continue;
                }

                // 验证用户名格式
                for (int i = 0; new_username[i] != '\0'; i++) {
                    if (!isalnum((unsigned char)new_username[i])) {
                        printf("错误：用户名只能包含字母(a-z,A-Z)和数字(0-9)！\n");
                        valid_input = 0;
                        break;
                    }
                }
            } while (!valid_input);

            strncpy_s(target->username, sizeof(target->username), new_username, 14);//复制
            printf("用户名修改成功！\n");
            break;
        }
        case 2: { // 修改手机号
            char new_phone[16];
            int valid_input;

            do {
                valid_input = 1;
                printf("新手机号(11位数字): ");

                if (fgets(new_phone, sizeof(new_phone), stdin) == NULL) {
                    clear_input_buffer();
                    valid_input = 0;
                    continue;
                }

                // 检测输入是否过长
                size_t input_len = strlen(new_phone);
                if (input_len > 0 && new_phone[input_len - 1] != '\n') {
                    printf("错误：手机号必须为11位数字！\n");
                    clear_input_buffer();
                    valid_input = 0;
                    continue;
                }

                // 去除换行符
                if (input_len > 0 && new_phone[input_len - 1] == '\n') {
                    new_phone[input_len - 1] = '\0';
                    input_len--;
                }

                // 检查长度
                if (input_len != 11) {
                    printf("错误：手机号必须为11位数字！当前输入：%zu位\n", input_len);
                    valid_input = 0;
                    continue;
                }

                // 验证是否为纯数字
                for (int i = 0; i < 11; i++) {
                    if (!isdigit((unsigned char)new_phone[i])) {
                        printf("错误：手机号必须为纯数字！第%d位非法\n", i + 1);
                        valid_input = 0;
                        break;
                    }
                }

                // 检查手机号是否已注册或与原手机号相同
                if (valid_input && find_admin_by_phone(admin_list, new_phone) &&
                    strcmp(new_phone, target->phone) != 0) {
                    printf("错误：该手机号已注册或与原手机号相同\n");
                    valid_input = 0;
                }
            } while (!valid_input);

            strncpy_s(target->phone, sizeof(target->phone), new_phone, 11);
            printf("手机号修改成功！\n");
            break;
        }

        case 3: { // 修改密码
            char new_password[41] = { 0 };
            char confirm_pwd[41] = { 0 };
            int valid_input;

            do {
                valid_input = 1;
                printf("新密码(1-40位字母数字): ");

                if (fgets(new_password, sizeof(new_password), stdin) == NULL) {
                    clear_input_buffer();
                    valid_input = 0;
                    continue;
                }


                // 检测输入是否过长
                size_t input_len = strlen(new_password);
                if (input_len > 0 && new_password[input_len - 1] != '\n') {
                    printf("错误：密码不能超过40个字符！\n");
                    clear_input_buffer();
                    valid_input = 0;
                    continue;
                }

                // 去除换行符
                if (input_len > 0 && new_password[input_len - 1] == '\n') {
                    new_password[input_len - 1] = '\0';
                    input_len--;
                }

                // 检查有效长度
                if (input_len == 0) {
                    printf("错误：密码不能为空！\n");
                    valid_input = 0;
                    continue;
                }

                // 验证密码格式
                for (int i = 0; new_password[i] != '\0'; i++) {
                    if (!isalnum((unsigned char)new_password[i])) {
                        printf("错误：密码只能包含字母(a-z,A-Z)和数字(0-9)！\n");
                        valid_input = 0;
                        break;
                    }
                }

                if (!valid_input) continue;

                printf("确认新密码: ");
                if (fgets(confirm_pwd, sizeof(confirm_pwd), stdin) == NULL) {
                    clear_input_buffer();
                    valid_input = 0;
                    continue;
                }
                confirm_pwd[strcspn(confirm_pwd, "\n")] = '\0';

                if (strcmp(new_password, confirm_pwd) != 0) {
                    printf("错误：两次输入的密码不一致！\n");
                    valid_input = 0;
                }
            } while (!valid_input);

            strncpy_s(target->password, sizeof(target->password), new_password, 40);
            printf("密码修改成功！\n");
            break;
        }

        case 4: { // 修改权限级别
            clear_screen();
            printf("\n========== 修改管理员权限 ==========\n");
            printf("正在修改管理员 %s 的权限\n", target->username);

            int new_level;
            printf("\n请选择新的权限级别:\n");
            printf("1. 普通管理员\n");
            printf("2. 超级管理员\n");
            printf("0. 取消操作\n");
            printf("请输入选择(0-2): ");

            char level_input[10];
            fgets(level_input, sizeof(level_input), stdin);
            if (sscanf(level_input, "%d", &new_level) != 1) {
                printf("输入无效！\n");
                pause_program();
                clear_input_buffer();
                break;
            }

            if (new_level == 0) {
                printf("\n已取消权限修改操作\n");
                pause_program();
                clear_input_buffer();
                break;
            }

            if (new_level < 1 || new_level > 2) {
                printf("无效选择，请输入0-2的数字！\n");
                pause_program();
                clear_input_buffer();
                break;
            }

            // 修改权限级别
            target->privilege_level = (new_level == 2) ? SUPER_ADMIN : NORMAL_ADMIN;

            // 保存修改到文件
            if (!save_admins_to_file(admin_list, "admins.txt")) {
                printf("警告：权限修改成功但保存到文件失败！\n");
            }
            else {
                printf("\n已成功将 %s 的权限修改为%s管理员\n",
                    target->username,
                    (target->privilege_level == SUPER_ADMIN) ? "超级" : "普通");
            }

            pause_program();
            clear_input_buffer();
            break;
        }

        default:
            printf("无效的选择！\n");
        }

        // 每次修改后保存
        if (choice >= 1 && choice <= 3) {
            if (!save_admins_to_file(admin_list, "admins.txt")) {
                printf("警告：管理员数据保存失败！\n");
            }
            pause_program();
            clear_input_buffer();
        }
    }
}

// ==========  注销其他普通管理员or注销自己  ==========
/*(高级管理员则可以注销普通管理员用户，普通管理员则可以注销自己)//包括其他高级管理员*/
void admin_delete_account(Admin* current_admin) {
    clear_screen();
    printf("\n========== 管理员账户注销 ==========\n");

    // 显示当前用户信息
    printf("当前账户: %s (%s)\n\n",
        current_admin->username,
        current_admin->privilege_level == SUPER_ADMIN ? "超级管理员" : "普通管理员");

    char target_username[MAX_USERNAME_LEN] = { 0 };
    Admin* target = NULL;

    // 超级管理员可以选择注销其他账户
    if (current_admin->privilege_level == SUPER_ADMIN) {
        // 显示所有管理员列表
        view_all_admins();

        // 获取目标用户名（必须明确输入）
        while (1) {
            printf("\n请输入要注销的管理员用户名: ");
            if (fgets(target_username, MAX_USERNAME_LEN, stdin) == NULL) {
                clear_input_buffer();
                printf("\n输入无效！\n");
                pause_program();
                clear_input_buffer();
                return;
            }
            target_username[strcspn(target_username, "\n")] = '\0'; // 去除换行符

            // 检查是否输入为空
            if (strlen(target_username) == 0) {
                printf("\n错误：必须输入有效的用户名！\n");
                continue;
            }

            target = find_admin_by_username(admin_list, target_username);
            if (!target) {
                printf("\n错误：管理员 %s 不存在！请重新输入。\n", target_username);
                continue;
            }

            // 检查是否尝试自我注销
            if (strcmp(target->username, current_admin->username) == 0) {
                printf("\n安全限制：超级管理员不能自我注销！\n");
                printf("请让其他超级管理员执行此操作。\n");
                pause_program();
                clear_input_buffer();
                return;
            }
            break;
        }
    }
    else {
        // 普通管理员只能注销自己
        target = current_admin;
        printf("您正在尝试注销自己的账户。\n");
    }

    // 确认操作 - 修改为只接受"y"确认
    char confirm;
    printf("\n警告：此操作将永久删除管理员账户 %s！\n", target->username);
    printf("确认要注销吗？(输入 y 确认，其他取消): ");
    if (scanf(" %c", &confirm) != 1) {

        printf("\n输入无效，操作已取消。\n");
        pause_program();
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    // 仅当输入小写或大写的y时才确认
    if (tolower(confirm) != 'y') {
        printf("\n操作已取消。\n");

        pause_program();
        clear_input_buffer();
        return;
    }

    // 执行注销
    if (delete_admin(&admin_list, target->username)) {
        if (save_admins_to_file(admin_list, "admins.txt")) {
            printf("\n账户已成功注销！\n");
            clear_input_buffer();
            // 如果是注销自己，则退出系统
            if (strcmp(target->username, current_admin->username) == 0) {
                printf("系统将退出...\n");
                pause_program();
                clear_input_buffer();
                exit(0);
            }
        }
        else {
            printf("\n账户已注销但保存失败！\n");
        }
    }
    else {
        printf("\n注销失败！\n");
    }
    pause_program();
    clear_input_buffer();
}

// ==========  修改管理员信息（自己）  ==========
bool modify_admin_info(Admin* current_admin) {
    if (!current_admin) return false;

    clear_screen();
    printf("\n========== 修改管理员信息 ==========\n");

    // 显示当前信息
    printf("当前信息:\n");
    printf("1. 用户名: %s\n", current_admin->username);
    printf("2. 手机号: %s\n", current_admin->phone);
    //printf("3. 密码: %s\n", current_admin->password);
    printf("3. 密码: ******\n");

    printf("4. 权限级别: %s\n",
        current_admin->privilege_level == SUPER_ADMIN ? "超级管理员" : "普通管理员");
    printf("0. 返回\n");
    printf("\n请选择要修改的项目 (1-4, 0取消): ");
    int choice;
    scanf_s("%d", &choice);
    clear_input_buffer();

    switch (choice) {
    case 1: { // 修改用户名
        char new_username[15];
        int valid_input;

        do {
            valid_input = 1;
            printf("新用户名(1-14位字母数字): ");

            if (fgets(new_username, sizeof(new_username), stdin) == NULL) {
                clear_input_buffer();
                valid_input = 0;
                continue;
            }

            // 检测输入是否过长
            size_t input_len = strlen(new_username);
            if (input_len > 0 && new_username[input_len - 1] != '\n') {
                printf("错误：用户名不能超过14个字符！\n");
                clear_input_buffer();
                valid_input = 0;
                continue;
            }

            // 去除换行符
            if (input_len > 0 && new_username[input_len - 1] == '\n') {
                new_username[input_len - 1] = '\0';
                input_len--;
            }

            // 检查有效长度
            if (input_len == 0) {
                printf("错误：用户名不能为空！\n");
                valid_input = 0;
                continue;
            }

            // 检查用户名是否已存在
            if (find_admin_by_username(admin_list, new_username)) {
                printf("错误：用户名已注册或与原用户名相同！\n");
                valid_input = 0;
                continue;
            }

            // 验证用户名格式
            for (int i = 0; new_username[i] != '\0'; i++) {
                if (!isalnum((unsigned char)new_username[i])) {
                    printf("错误：用户名只能包含字母(a-z,A-Z)和数字(0-9)！\n");
                    valid_input = 0;
                    break;
                }
            }
        } while (!valid_input);

        strncpy_s(current_admin->username, sizeof(current_admin->username), new_username, 14);
        printf("用户名修改成功！\n");
        break;
    }

    case 2: { // 修改手机号
        char new_phone[16];
        int valid_input;

        do {
            valid_input = 1;
            printf("新手机号(11位数字): ");

            if (fgets(new_phone, sizeof(new_phone), stdin) == NULL) {
                clear_input_buffer();
                valid_input = 0;
                continue;
            }

            // 检测输入是否过长
            size_t input_len = strlen(new_phone);
            if (input_len > 0 && new_phone[input_len - 1] != '\n') {
                printf("错误：手机号必须为11位数字！\n");
                clear_input_buffer();
                valid_input = 0;
                continue;
            }

            // 去除换行符
            if (input_len > 0 && new_phone[input_len - 1] == '\n') {
                new_phone[input_len - 1] = '\0';
                input_len--;
            }

            // 检查长度
            if (input_len != 11) {
                printf("错误：手机号必须为11位数字！当前输入：%zu位\n", input_len);
                valid_input = 0;
                continue;
            }

            // 验证是否为纯数字
            for (int i = 0; i < 11; i++) {
                if (!isdigit((unsigned char)new_phone[i])) {
                    printf("错误：手机号必须为纯数字！第%d位非法\n", i + 1);
                    valid_input = 0;
                    break;
                }
            }

            // 检查手机号是否已注册或与原手机号相同
            if (valid_input && find_admin_by_phone(admin_list, new_phone) &&
                strcmp(new_phone, current_admin->phone) != 0) {
                printf("错误：该手机号已注册或与原手机号相同\n");
                valid_input = 0;
            }
        } while (!valid_input);

        strncpy_s(current_admin->phone, sizeof(current_admin->phone), new_phone, 11);
        printf("手机号修改成功！\n");
        break;
    }

    case 3: { // 修改密码
        char new_password[41], confirm_pwd[41];
        int valid_input;

        do {
            valid_input = 1;
            printf("新密码(1-40位字母数字): ");

            if (fgets(new_password, sizeof(new_password), stdin) == NULL) {
                clear_input_buffer();
                valid_input = 0;
                continue;
            }

            // 检测输入是否过长
            size_t input_len = strlen(new_password);
            if (input_len > 0 && new_password[input_len - 1] != '\n') {
                printf("错误：密码不能超过40个字符！\n");
                clear_input_buffer();
                valid_input = 0;
                continue;
            }

            // 去除换行符
            if (input_len > 0 && new_password[input_len - 1] == '\n') {
                new_password[input_len - 1] = '\0';
                input_len--;
            }

            // 检查有效长度
            if (input_len == 0) {
                printf("错误：密码不能为空！\n");
                valid_input = 0;
                continue;
            }

            // 验证密码格式
            for (int i = 0; new_password[i] != '\0'; i++) {
                if (!isalnum((unsigned char)new_password[i])) {
                    printf("错误：密码只能包含字母(a-z,A-Z)和数字(0-9)！\n");
                    valid_input = 0;
                    break;
                }
            }

            if (!valid_input) continue;

            printf("确认新密码: ");
            if (fgets(confirm_pwd, sizeof(confirm_pwd), stdin) == NULL) {
                clear_input_buffer();
                valid_input = 0;
                continue;
            }
            confirm_pwd[strcspn(confirm_pwd, "\n")] = '\0';

            if (strcmp(new_password, confirm_pwd) != 0) {
                printf("错误：两次输入的密码不一致！\n");
                valid_input = 0;
            }
        } while (!valid_input);

        strncpy_s(current_admin->password, sizeof(current_admin->password), new_password, 40);
        printf("密码修改成功！\n");
        break;
    }
    case 4: { // 修改权限级别
        if (!is_super_admin(current_admin)) {
            printf("\n权限不足！只有超级管理员可以修改权限级别。\n");
            pause_program();
            clear_input_buffer();
            return false;
        }

        printf("\n当前权限: %s\n",
            current_admin->privilege_level == SUPER_ADMIN ? "超级管理员" : "普通管理员");

        int new_level;
        printf("\n请选择新的权限级别:\n");
        printf("1. 普通管理员\n");
        printf("2. 超级管理员\n");
        printf("0. 取消操作\n");
        printf("请输入选择(0-2): ");

        while (1) {
            if (scanf_s("%d", &new_level) != 1) {
                clear_input_buffer();
                printf("输入无效，请输入0-2的数字: ");
                continue;
            }

            if (new_level == 0) {
                printf("\n已取消权限修改操作\n");
                return false;
            }

            if (new_level == 1 || new_level == 2) {
                break;
            }

            printf("无效选择，请输入0-2的数字: ");
        }
        clear_input_buffer();

        current_admin->privilege_level = (AdminPrivilegeLevel)(new_level - 1);
        printf("\n已成功将权限修改为%s管理员\n",
            new_level == 2 ? "超级" : "普通");
        break;
    }
    case 0:
        return false;

    default:
        printf("无效的选择！\n");
        pause_program();
        clear_input_buffer();
        return false;
    }

    // 保存修改到文件
    save_admins_to_file(admin_list, "admins.txt");
    pause_program();
    clear_input_buffer();
    return true;
}







