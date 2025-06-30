
// cainiao.h
#ifndef CAINIAO_H
#define CAINIAO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include<math.h>
#include <ctype.h> 
#include <conio.h> // 用于 getch()
#include <stdbool.h>
#include <windows.h>
//--------------------用户--------------------
// 用户等级枚举
typedef enum {
    USER_NORMAL,    // 普通用户
    USER_STUDENT,   // 学生用户
    USER_VIP,       // VIP用户
    USER_ENTERPRISE,// 企业用户
    USER_PARTNER    // 合作商户
} UserLevel;

// 支付方式枚举
typedef enum {
    PAY_CASH,       // 现付
    PAY_MONTHLY,    // 月结
    PAY_CONTRACT    // 周期结算
} PaymentMode;

// 用户结构体
typedef struct User {
    char username[50];
    char phone[12];
    char password[50];
    UserLevel level;
    PaymentMode payment_mode;
    float discount_rate;
    struct User* next;
} User;

//--------------------管理员--------------------
// 管理员权限级别枚举
typedef enum {
    NORMAL_ADMIN = 1,
    SUPER_ADMIN = 2
} AdminPrivilegeLevel;

// 管理员结构体
typedef struct Admin {
    char username[50];
    char phone[12];
    char password[50];
    AdminPrivilegeLevel privilege_level;
    struct Admin* next;
} Admin;


//--------------------快递--------------------
// 快递状态枚举
// 快递状态枚举
typedef enum {
    STATUS_PENDING_IN,    // 待入库
    STATUS_UNSENT,      // 未寄出
    STATUS_SENT,        // 已寄出
    STATUS_UNPICKED,    // 未取件
    STATUS_PICKED,      // 已取件
    STATUS_OVERDUE,     // 滞留
    STATUS_LOST,        // 丢失 
    STATUS_DAMAGED,     // 损坏
    STATUS_MISPICKED,   // 误领
    STATUS_WRONGPICKED,   // 冒领
    STATUS_ABNORMAL,    // 异常件（新增）
    STATUS_ABNORMAL_RESOLVED, // 异常已处理
    STATUS_REJECTED,    //拒收
    STATUS_RETURNED,     //退回
    STATUS_UNPICKED_AGAIN  //重新待取件
} PackageStatus;

// 包裹类型枚举
typedef enum {
    DAILY_GOODS,    // 日用品
    FOOD,           // 食品
    DOCUMENT,       // 文件
    CLOTHING,       // 衣物
    DIGITAL,        // 数码产品
    ALCOHOL,        // 酒类（不可邮寄）
    ANIMAL,         // 动物（不可邮寄）
    WEAPON,         // 武器（不可邮寄）
    HAZARDOUS,      // 易燃/易爆/有毒物品（不可邮寄）
    VALUABLES,      // 贵重物品
    FRESH,          // 生鲜
    FRAGILE,        // 易碎品
    OTHER,          // 其他
    BANNED_ITEMS    // 不可邮寄物品标记（内部使用）
} PackageType;

// 运输方式枚举
typedef enum {
    SHIPPING_NORMAL,   // 普通快递
    SHIPPING_BULK,         // 大件邮寄
    SHIPPING_COLD_CHAIN    // 冷链邮寄
} ShippingMethod;

// 地区类型枚举
typedef enum {
    REGION_PROVINCE,  // 省内
    REGION_OUTOFPROVINCE,    // 省外
    REGION_INTERNATIONAL, // 国际
    REGION_HONGKONG_MACAO // 港澳台
} RegionType;
// 运输时效枚举
typedef enum {
    STANDARD_DELIVERY,  // 标快（默认）
    EXPRESS_DELIVERY        // 特快
} DeliveryType;
typedef struct {
    double base_weight_fee;         // 首重重量(kg)
    double additional_weight_fee;   // 续重单价(元/kg)
} RateConfig;
// 投诉类型枚举
typedef enum {
    COMPLAINT_LOST,     // 包裹丢失
    COMPLAINT_DAMAGED   // 包裹损坏
} ComplaintType;

// 快递信息结构体
typedef struct Express {
    int send_num;
    char sender_name[50];
    char sender_phone[12];
    char receiver_name[50];
    char receiver_phone[12];
    double weight;         // 实际重量(kg)
    double volume;         // 体积(cm?)
    PackageType package_type;
    ShippingMethod method;
    RegionType region;
    int is_insured;       // 是否保价(0/1)
    double insured_value;  // 物品价值
    DeliveryType express_type;
    double fee;            // 运费
    PackageStatus status;      // 快递状态
    time_t create_time;   // 创建时间
    char pickup_code[15];       // 6位取件码+'\0'
    char tracking_num[20];	 // 快递单号
    char shelf_id[10];	 // 货架号
    char pickup_phone[12];	 // 取件人电话号码
    time_t storage_time;   // 入库时间
    int complaint_id;          // 投诉ID
    ComplaintType complaint_type; // 投诉类型(丢失/损坏)
    char complaint_desc[256];  // 投诉描述
    char complaint_resolution[256]; // 处理结果
    double compensation;       // 赔偿金额
    time_t complaint_time;     // 投诉时间
    time_t resolve_time;       // 处理时间
    struct Express* next;
    char reject_reason[100];  // 拒收原因
    time_t reject_time;       // 拒收时间
} Express;
typedef struct ExpressNode {
    Express data;
    struct ExpressNode* next;
} ExpressNode;
static ExpressNode* create_express_node(Express* data) {
    ExpressNode* node = (ExpressNode*)malloc(sizeof(ExpressNode));
    if (node) {
        node->data = *data;
        node->next = NULL;
    }
    return node;
}

// 货架类型枚举
typedef enum {
    SHELF_NORMAL,    // 普通货架
    SHELF_FRAGILE,   // 易碎品货架
    SHELF_COLD,      // 冷链货架
    SHELF_BULK       // 大件货架
} ShelfType;

// 货架结构体
typedef struct Shelf {
    char shelf_id[10];         // 货架ID
    ShelfType type;            // 货架类型
    int max_capacity;          // 最大容量
    int current_count;         // 当前数量
    float temperature;         // 温度（冷链专用）
    int security_level;        // 安全等级（易碎品专用）
    ExpressNode* pickup_list; // 快递链表
    struct Shelf* next;
} Shelf;


// 全局变量
extern User* user_list;      // 用户链表头指针
extern int user_count;       // 用户数量
extern int send_num;         //寄出数量
extern Admin* admin_list;    // 管理员链表头指针
extern int admin_count;      // 管理员数量
extern Shelf* shelf_list;
extern ExpressNode* send_list; // 快递链表头指针
extern ExpressNode* pickup_list; // 快递链表头指针
extern int current_count;

// 定义常量
#define MAX_USERNAME_LEN 50
#define MAX_PHONE_LEN 12
#define MAX_PASSWORD_LEN 50
// 文件路径
#define SEND_FILE "send_express.txt"
#define PICKUP_FILE "pickup_express.txt"
// 主菜单和用户相关
void main_menu(void);
void user_menu(User* user);
void admin_menu(Admin* admin);
void admin_management_menu(Admin* current_admin);
void user_management_menu(Admin* admin);
void view_all_users(void);
void shelf_management_menu(Admin* admin);
void inventory_menu(Shelf* head);
void express_service_menu(User* user);

//费率计算相关
float calculate_fee(User* user, float base_price);

// 快递节点创建（移到函数声明区）
ExpressNode* create_express_node(Express* data);
// --------------------寄快递流程函数--------------------
const char* get_package_type_name(PackageType type);//获取物品类型名称
int is_valid_phone(const char* phone);// 检查是否是有效的 11 位手机号
int input_int_in_range(const char* prompt, int min, int max);// 获取用户输入范围内的整数
void input_phone(char* phone, size_t size, const char* prompt);
void safe_input(char* buf, size_t size, const char* prompt);
double input_positive_double(const char* prompt);// 输入一个正浮点数（必须 > 0）
void send_express(User* user_head);//寄快递核心函数
void modify_express(User* user);// 修改快递信息
void delete_express(User* user);// 删除快递信息
void find_pending_express(User* user);// 查询未寄出快递
void show_pending_bills(User* user);// 显示待支付账单
void send_express_menu(User* user);// 寄快递菜单
// --------------------运费计算函数--------------------
int get_throw_ratio(ShippingMethod method, RegionType region);// 获取抛比系数
double calculate_insurance(double insured_value, PackageType type);// 保价费用计算
RateConfig get_base_rates(ShippingMethod method, RegionType region, PackageType type, DeliveryType express_type);//获取首重续重函数
double calculate_shipping_fee(Express* express, User* user);//运费计算核心函数
// --------------------取快递流程函数--------------------
void pickup_express(User* current_user);
void process_pickup(User* current_user);
// --------------------文件保存函数--------------------
void save_send_express_info(ExpressNode* head, const char* filename);
void save_pickup_express_info(ExpressNode* head, const char* filename);
int count_nodes(ExpressNode* head);

// --------------------快递管理函数--------------------
void manage_express(Admin* admin);
void load_pickup_express(ExpressNode** express_head);
void manage_pending_express();
void load_send_express(ExpressNode** express_head);
// 误领处理函数
void mistaken_pickup(User* current_user);
// 提交投诉
void submit_complaint(User* user);
// 查看用户投诉
void view_user_complaints(User* user);
// --------------------用户管理函数--------------------
void init_user_privilege(User* user);// 初始化用户特权
User* create_user(const char* phone, const char* pwd, const char* name, UserLevel level);// 创建新用户
User* find_user_by_username(User* head, const char* username);
User* find_user_by_phone(User* head, const char* phone);
bool save_users_to_file(User* head, const char* filename);// 保存用户信息到文件
User* load_users_from_file(const char* filename);// 从文件加载用户信息（尾插法)
bool delete_user(User** head, const char* username);// 注销用户
void add_user(User** head, User* new_user);
void view_my_info(User* user);
void user_login();// 用户登录
void user_register();// 用户注册函数
bool modify_user_info(User* user);//修改用户信息
// 用户注销函数
bool user_unregister(User** user_list, int* user_count, User** current_user);

// --------------------管理员管理函数--------------------
void admin_menu(Admin* admin);
Admin* create_admin(const char* username, const char* phone, const char* password, int privilege_level);
void add_admin(Admin** head, Admin* new_admin);
Admin* find_admin_by_username(Admin* head, const char* username);
Admin* find_admin_by_phone(User* head, const char* phone);
bool save_admins_to_file(Admin* head, const char* filename);
Admin* load_admins_from_file(const char* filename);
bool is_super_admin(const Admin* admin);
void admin_login();
void admin_register(Admin* executor);
bool delete_admin(Admin** head, const char* username);
void admin_delete_account(Admin* current_admin);
bool modify_admin_info(Admin* current_admin);
bool super_modify_admin_info(Admin* super_admin);
bool view_all_admins();
void admin_add_user();

// --------------------货架函数--------------------
Shelf* create_shelf(const char* shelf_id, int max_capacity);
void add_shelf(Shelf** head, Shelf* new_shelf);
bool delete_shelf(Shelf** head, const char* shelf_id);
char get_shelf_prefix_by_shipping(ShippingMethod method);
Shelf* find_available_shelf(Shelf* shelves, char prefix);
int assign_to_shelf(Express* express, Shelf* shelf_list, char shelf_prefix, time_t storage_time);
bool save_shelves_to_file(Shelf* head, const char* filename);
Shelf* load_shelves_from_file(const char* filename, ExpressNode* pickup_list);
void inventory_shelf(Shelf* shelf);
void full_inventory(Shelf* head);
void check_shelf_capacity(Shelf* head);
int count_shelves(Shelf* shelves);
void display_shelves(Shelf* shelves);
bool is_valid_shelf_id(const char* shelf_id);
bool remove_express_from_shelf(Shelf** shelves, const char* shelf_id, const char* tracking_num);
void show_all_overdue_items_with_stats();
Express* find_express_by_tracking(ExpressNode* head, const char* tracking_num);

//管理员处理拒收
void admin_process_rejected_packages();
void view_rejected_packages(User* current_user);
void reject_package_menu(User* current_user);
void reject_package(User* current_user);
void admin_reject_management_menu();

//--------------------管理员投诉处理模块--------------------
void display_abnormal_express_brief(Express* express);
void display_abnormal_express_detail(Express* express);
void process_single_abnormal_express(Express* express);
double calculate_compensation(const Express* express);
void batch_process_all_abnormal_express();
void query_and_process_by_tracking();
void admin_handle_complaints();

void clear_input_buffer();
void pause_program();
void clear_screen();
const char* get_status_text(PackageStatus status);

//--------------------用户投诉模块--------------------
// 投诉相关函数
int generate_complaint_id(ExpressNode* head);
void submit_complaint(User* user);
void display_complaint(Express* express);
void view_user_complaints(User* user);

#endif // CAINIAO_H



