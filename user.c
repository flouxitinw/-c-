#include "cainiao.h"

// 初始化用户特权
void init_user_privilege(User* user) {
    switch (user->level) {
    case USER_NORMAL://0
        user->discount_rate = 1.0f;    // 100% 折扣率
        user->payment_mode = PAY_CASH; // 现付
        break;

    case USER_STUDENT://1
        user->discount_rate = 0.85f;   // 85% 折扣率
        user->payment_mode = PAY_CASH; // 现付
        break;

    case USER_VIP://2
        user->discount_rate = 0.9f;    // 90% 折扣率
        user->payment_mode = PAY_MONTHLY;//月结
        break;

    case USER_ENTERPRISE://3
        user->discount_rate = 0.75f;   // 75% 折扣率
        user->payment_mode = PAY_MONTHLY; // 月结
        break;

    case USER_PARTNER://4
        user->discount_rate = 0.6f;   // 协议价基准60%
        user->payment_mode = PAY_CONTRACT; // 周期结算
        break;
    default:
        fprintf(stderr, "警告：未知用户等级 %d，使用默认权限\n", user->level);
        user->discount_rate = 1.0f;
        user->payment_mode = PAY_CASH;
        break;
    }

}

// 创建新用户
User* create_user(const char* phone, const char* pwd, const char* name, UserLevel level) {
    // 分配内存
    User* new_user = (User*)malloc(sizeof(User));
    if (!new_user) {
        perror("Failed to create user");
        return NULL;
    }

    // 基础信息
    strncpy_s(new_user->phone, sizeof(new_user->phone), phone, 11);//将手机号 phone 复制到 new_user->phone 
    new_user->phone[11] = '\0';//手动添加 '\0' 终止符
    strncpy_s(new_user->password, sizeof(new_user->password), pwd, 49);
    new_user->password[49] = '\0';
    strncpy_s(new_user->username, sizeof(new_user->username), name, 49);
    new_user->username[49] = '\0';
    new_user->level = level;

    // 初始化特权
    init_user_privilege(new_user);
    new_user->next = NULL;

    return new_user;
}

//添加用户（头插法）(将用户添加到用户链表中)
void add_user(User** head, User* new_user) {
    if (!new_user) return;
    new_user->next = *head;
    *head = new_user;
}

// 根据手机号查找用户
User* find_user_by_phone(User* head, const char* phone) {
    // 检查 phone 是否为 NULL
    if (!phone) return NULL;

    User* curr = head;
    while (curr) {
        if (strcmp(curr->phone, phone) == 0) // 字符串完全匹配
            return curr;
        curr = curr->next;
    }
    return NULL;
}

// 查找用户（通过用户名）
User* find_user_by_username(User* head, const char* username) {
    //检查 username 是否为 NULL
    if (!username) return NULL;
    User* curr = head;
    while (curr) {
        if (strcmp(curr->username, username) == 0)//字符串完全匹配
            return curr;
        curr = curr->next;
    }
    return NULL;
}

// 删除用户
bool delete_user(User** head, const char* username) {//使用 User** head 是为了能修改调用方的链表头指针
    if (!head || !username || !*head) return false;

    User* prev = NULL;// 当前节点的前驱节点
    User* curr = *head;

    // 查找要删除的用户
    while (curr) {
        if (strcmp(curr->username, username) == 0) {
            // 找到用户，开始删除
            if (prev) {
                prev->next = curr->next; // 普通节点删除
            }
            else {//前驱节点=NULL，前驱节点是头结点
                *head = curr->next; // 删除的是头节点
            }

            // 安全清除用户信息
            memset(curr->phone, 0, sizeof(curr->phone));// 清零手机号
            memset(curr->password, 0, sizeof(curr->password));// 清零密码
            free(curr);// 释放内存

            return true;
        }
        prev = curr;
        curr = curr->next;
    }

    return false; // 未找到用户
}

// 保存用户信息到文件
bool save_users_to_file(User* head, const char* filename) {//传入用户链表头指针
    // 检查文件名是否有效
    if (!filename || strlen(filename) == 0) {
        return false;
    }

    // 写入模式打开文件
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("Failed to save users");
        return false;
    }

    // 写入列标题
    if (fprintf(fp, "username,phone,password,level,payment_mode,discount_rate\n") < 0) {//用户名，手机号，密码，等级，支付方式，折扣率
        fclose(fp);
        return false;
    }

    // 遍历链表并写入每个用户信息
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

    // 检查是否成功关闭文件
    if (fclose(fp) != 0) {
        perror("Failed to close users file");
        return false;
    }

    return true;
}

// 从文件加载用户信息（尾插法）
User* load_users_from_file(const char* filename) {
    //只读模式打开文件
    FILE* fp = fopen(filename, "r");
    if (!fp) return NULL;

    User* head = NULL, * tail = NULL;
    char line[256];

    // 读取并跳过表头
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        char username[50] = { 0 }, phone[12] = { 0 }, password[50] = { 0 };
        int level, payment_mode;
        float discount_rate;

        // 解析每行数据
        if (sscanf_s(line, "%49[^,],%11[^,],%49[^,],%d,%d,%f",//%49[^,]：读取最多 49 个非逗号字符（为 \0 预留空间）
            username, (unsigned)_countof(username),
            phone, (unsigned)_countof(phone),
            password, (unsigned)_countof(password),
            &level, &payment_mode, &discount_rate) != 6) {
            continue; // 跳过格式不正确的行
        }

        // 创建用户
        User* user = (User*)malloc(sizeof(User));
        if (!user) continue;

        // 基本信息(复制字符串)
        strncpy_s(user->username, sizeof(user->username), username, 49);
        user->username[49] = '\0';// 确保终止
        strncpy_s(user->phone, sizeof(user->phone), phone, 11);
        user->phone[11] = '\0';
        strncpy_s(user->password, sizeof(user->password), password, 49);
        user->password[49] = '\0';

        // 用户等级和特权
        user->level = (UserLevel)level;
        user->payment_mode = (PaymentMode)payment_mode;
        user->discount_rate = discount_rate;
        user->next = NULL;

        // 添加到链表（尾插法）
        if (!head) {//head为NULL
            head = tail = user;// 第一个节点
        }
        else {
            tail->next = user;
            tail = user;      // 追加到链表尾部
        }
    }

    fclose(fp);
    return head;
}


//-----------------------------------------------------------------------------------
// ========== 用户注册 ==========
/*创建用户（用户名，手机号，密码，用户类型），分配空间，用户链表中加用户，保存到文件*/
void user_register() {
    clear_screen();
    printf("\n==================== 用户注册 ====================\n");
    printf("* 用户名: 14位以内，只包含字母(a-z,A-Z)和数字(0-9)\n");
    printf("* 手机号: 必须为11位纯数字\n");
    printf("* 密码: 40位以内，只包含字母(a-z,A-Z)和数字(0-9)\n\n");

    char username[15], phone[16], password[41], confirm_pwd[41];
    int user_type;
    int valid_input;

    // 输入用户名并验证
    do {
        valid_input = 1;
        printf("用户名: ");

        // 安全获取输入
        // 处理读取失败的情况
        if (fgets(username, sizeof(username), stdin) == NULL) {//防止缓冲区溢出
            clear_input_buffer();//清除缓冲区
            valid_input = 0;
            continue;
        }

        // 检测输入是否过长
        size_t input_len = strlen(username);
        if (input_len > 0 && username[input_len - 1] != '\n') {
            // 若最后不为换行符，则超过字符数限制
            printf("错误：用户名不能超过14个字符！\n");
            clear_input_buffer();  //清除缓冲区
            valid_input = 0;
            continue;
        }

        // 去除换行符（如果有）fgets会读取\n
        if (input_len > 0 && username[input_len - 1] == '\n') {
            username[input_len - 1] = '\0';//\0代表截止
            input_len--;
        }

        // 用户名不能为空  检查有效长度（1-14字符）
        if (input_len == 0) {
            printf("错误：用户名不能为空！\n");
            valid_input = 0;
            continue;
        }

        // 检查用户名是否已存在
        if (find_user_by_username(user_list, username)) {
            printf("用户名已存在！\n");
            valid_input = 0; // 让循环继续
            continue;
        }

        // 严格验证用户名格式（仅字母数字）
        for (int i = 0; username[i] != '\0'; i++) {
            if (!isalnum((unsigned char)username[i])) {
                printf("错误：用户名只能包含字母(a-z,A-Z)和数字(0-9)！\n");
                valid_input = 0;
                break;
            }
        }
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
        confirm_pwd[strcspn(confirm_pwd, "\n")] = '\0';//找到 \n 的位置后，用 '\0'（字符串终止符）替换它

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
        if (strchr(input, '\n') == NULL) {//查找是否有换行符
            printf("\n输入过长，注册失败！\n");
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
        clear_input_buffer();
        return;
    }


    add_user(&user_list, new_user);//(将用户添加到用户链表中)
    user_count++;//用户数加一
    // 保存用户数据
    save_users_to_file(user_list, "users.txt");

    printf("\n注册成功！欢迎加入菜鸟驿站，%s\n", username);
    printf("您的账户类型: ");
    switch (new_user->level) {
    case USER_NORMAL:    printf("普通用户"); break;
    case USER_STUDENT:  printf("学生用户(85折)"); break;
    case USER_VIP:      printf("VIP用户(9折)"); break;
    case USER_ENTERPRISE: printf("企业用户(75折)"); break;
    case USER_PARTNER:  printf("合作商户(协议价)"); break;
    }
    clear_input_buffer();
}

// ========== 用户登录 ==========
/*输入用户名或手机号以及密码，在用户链表中查找匹配，有3次机会（通过while循环）*/
void user_login() {
    int attempts = 3;

    while (attempts > 0) {
        clear_screen();
        printf("\n========== 用户登录 ==========\n");
        printf("剩余尝试次数: %d\n", attempts);
        char input[50] = { 0 };
        char password[50] = { 0 };

        // 获取用户名/手机号
        printf("用户名或手机号: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {//用fgets安全
            clear_input_buffer();
            printf("输入错误！\n");
            pause_program();
            clear_input_buffer();
            continue;
        }
        input[strcspn(input, "\n")] = '\0';  // 去除换行符

        // 获取密码
        printf("密码: ");
        if (fgets(password, sizeof(password), stdin) == NULL) {
            clear_input_buffer();
            printf("输入错误！\n");
            pause_program();
            clear_input_buffer();
            continue;
        }
        password[strcspn(password, "\n")] = '\0';  // 去除换行符


        // 查找用户（链表遍历）(通过用户名或者手机号)
        User* user = NULL;
        User* curr = user_list;  // user_list 是链表头指针
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
            printf("\n错误：用户名或手机号不存在！");
            clear_input_buffer();
        }
        else if (strcmp(user->password, password) != 0) {
            attempts--;
            printf("\n错误：密码不正确！");
            clear_input_buffer();

        }
        else {
            printf("\n登录成功！欢迎回来，%s\n", user->username);
            pause_program();
            clear_input_buffer();
            user_menu(user);//进入菜单
            return;
        }

        if (attempts > 0) {
            printf(" 剩余尝试次数: %d\n", attempts);
            clear_input_buffer();
        }
        else {
            printf("\n尝试次数已用完，返回主菜单。\n");
            pause_program();
            clear_input_buffer();
            return;  // 返回主菜单
        }
    }


}

// ========== 用户查看自己信息 ==========
void view_my_info(User* user) {
    if (!user) return;

    clear_screen();
    printf("\n========== 我的账户信息 ==========\n");

    // 基本信息
    printf("\n【基本信息】\n");
    printf("用户名: %s\n", user->username);
    printf("手机号: %s\n", user->phone);

    // 账户类型
    printf("\n【账户类型】\n");
    printf("用户等级: ");
    switch (user->level) {
    case USER_NORMAL:    printf("普通用户"); break;
    case USER_STUDENT:   printf("学生用户 (享受85折优惠)"); break;
    case USER_VIP:       printf("VIP用户 (享受9折优惠)"); break;
    case USER_ENTERPRISE:printf("企业用户 (享受75折优惠)"); break;
    case USER_PARTNER:   printf("合作商户 (享受协议价)"); break;
    }
    printf("\n折扣率: %.2f\n", user->discount_rate);

    // 支付方式
    printf("\n【支付方式】\n");
    printf("当前支付方式: ");
    switch (user->payment_mode) {
    case PAY_CASH:    printf("现付"); break;
    case PAY_MONTHLY: printf("月结"); break;
    case PAY_CONTRACT:printf("协议周期结算"); break;
    }

    printf("\n----------------------------------\n");
    pause_program();
    clear_input_buffer();
}

// ========== 用户修改自己信息 ==========
/*先显示用户基本信息，后选择修改的项，跟注册时一样的格式要求，保存信息到文件*/
bool modify_user_info(User* user) {
    if (!user) return false;

    clear_screen();
    printf("\n========== 修改用户信息 ==========\n");
    printf("当前信息：\n");
    printf("1. 用户名: %s\n", user->username);
    printf("2. 手机号: %s\n", user->phone);
    //printf("3. 密码: %s\n", user->password);
    printf("3. 密码: ******\n"); //用*表示，更加安全   

    printf("0. 返回\n");
    printf("\n请选择要修改的项目 (1-3, 0取消): ");

    int choice;
    char input[10];
    fgets(input, sizeof(input), stdin);
    if (sscanf(input, "%d", &choice) != 1) {
        printf("无效输入！\n");
        pause_program();
        clear_input_buffer();
        return false;
    }
    switch (choice) {
    case 1: { // 修改用户名
        char new_username[15];
        int valid_input;//验证格式是否正确的标志

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
                clear_input_buffer();
                valid_input = 0;
                continue;
            }

            // 检查用户名是否已存在
            if (find_user_by_username(user_list, new_username)) {
                printf("错误：用户名已注册或与原用户名相同！\n");
                clear_input_buffer();
                valid_input = 0;
                continue;
            }

            // 验证用户名格式
            for (int i = 0; new_username[i] != '\0'; i++) {
                if (!isalnum((unsigned char)new_username[i])) {
                    printf("错误：用户名只能包含字母(a-z,A-Z)和数字(0-9)！\n");
                    clear_input_buffer();
                    valid_input = 0;
                    break;
                }
            }
        } while (!valid_input);

        strncpy_s(user->username, sizeof(user->username), new_username, 14);//复制
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
            if (valid_input && find_user_by_phone(user_list, new_phone) &&
                strcmp(new_phone, user->phone) != 0) {
                printf("错误：该手机号已注册或与原手机号相同\n");
                valid_input = 0;
            }
        } while (!valid_input);

        strncpy_s(user->phone, sizeof(user->phone), new_phone, 11);
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

        strncpy_s(user->password, sizeof(user->password), new_password, 40);
        printf("密码修改成功！\n");
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
    save_users_to_file(user_list, "users.txt");
    clear_input_buffer();
    return true;
}

// ========== 用户注销自己 ==========
/*输入密码验证，三次机会，注销时将用户移除链表，用户指针置NULL*/
bool user_unregister(User** user_list, int* user_count, User** current_user) {
    if (!user_list || !*current_user) return false;
    clear_screen();
    printf("\n========== 账户注销 ==========\n");

    // 显示当前用户信息
    User* user = *current_user;
    printf("您即将注销以下账户：\n");
    printf("用户名: %s\n", user->username);
    printf("手机号: %s\n", user->phone);
    printf("用户类型: ");
    switch (user->level) {
    case USER_NORMAL:    printf("普通用户"); break;
    case USER_STUDENT:   printf("学生用户"); break;
    case USER_VIP:       printf("VIP用户"); break;
    case USER_ENTERPRISE: printf("企业用户"); break;
    case USER_PARTNER:   printf("合作商户"); break;
    }
    printf("\n\n");

    // 密码验证
    int attempts = 0;
    const int max_attempts = 3;
    bool authenticated = false;//输入密码是否正确的标记

    while (attempts < max_attempts && !authenticated) {
        printf("尝试剩余次数: %d/%d\n", max_attempts - attempts, max_attempts);
        printf("请输入您的密码确认注销（输入0取消）: ");

        char password[50];
        if (fgets(password, sizeof(password), stdin) == NULL) {
            clear_input_buffer();
            continue;
        }
        password[strcspn(password, "\n")] = '\0';

        // 取消操作(按0取消)
        if (strcmp(password, "0") == 0) {
            printf("\n已取消注销操作。\n");
            clear_input_buffer();
            return false;
        }

        // 验证密码
        if (strcmp(password, user->password) == 0) {
            authenticated = true;
        }
        else {
            attempts++;
            if (attempts < max_attempts) {
                printf("\n错误：密码不正确！\n");
                clear_input_buffer();
                clear_screen();
                printf("\n========== 账户注销 ==========\n");
                printf("用户名: %s\n", user->username);
            }
        }
    }

    if (!authenticated) {
        printf("\n错误：密码验证失败次数过多！\n");
        printf("出于安全考虑，已终止注销流程。\n");
        clear_input_buffer();
        return false;
    }

    // 最终确认
    printf("\n警告：此操作不可撤销！\n");
    printf("确定要永久注销账户吗？(y): ");
    char confirm = getchar();
    clear_input_buffer();

    if (tolower(confirm) != 'y') {
        printf("\n已取消注销操作。\n");
        pause_program();
        clear_input_buffer();
        return false;
    }

    // 执行注销
    if (delete_user(user_list, user->username)) {
        (*user_count)--;
        save_users_to_file(*user_list, "users.txt");

        printf("\n账户已成功注销！\n");
        printf("感谢您使用我们的服务。\n");
        *current_user = NULL; // 清除当前登录状态

        clear_input_buffer();
        return true;
    }
    else {
        printf("\n错误：注销失败！请联系管理员。\n");
        clear_input_buffer();
        return false;
    }

}

