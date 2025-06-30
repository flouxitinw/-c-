
// cainiao.h
#ifndef CAINIAO_H
#define CAINIAO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include<math.h>
#include <ctype.h> 
#include <conio.h> // ���� getch()
#include <stdbool.h>
#include <windows.h>
//--------------------�û�--------------------
// �û��ȼ�ö��
typedef enum {
    USER_NORMAL,    // ��ͨ�û�
    USER_STUDENT,   // ѧ���û�
    USER_VIP,       // VIP�û�
    USER_ENTERPRISE,// ��ҵ�û�
    USER_PARTNER    // �����̻�
} UserLevel;

// ֧����ʽö��
typedef enum {
    PAY_CASH,       // �ָ�
    PAY_MONTHLY,    // �½�
    PAY_CONTRACT    // ���ڽ���
} PaymentMode;

// �û��ṹ��
typedef struct User {
    char username[50];
    char phone[12];
    char password[50];
    UserLevel level;
    PaymentMode payment_mode;
    float discount_rate;
    struct User* next;
} User;

//--------------------����Ա--------------------
// ����ԱȨ�޼���ö��
typedef enum {
    NORMAL_ADMIN = 1,
    SUPER_ADMIN = 2
} AdminPrivilegeLevel;

// ����Ա�ṹ��
typedef struct Admin {
    char username[50];
    char phone[12];
    char password[50];
    AdminPrivilegeLevel privilege_level;
    struct Admin* next;
} Admin;


//--------------------���--------------------
// ���״̬ö��
// ���״̬ö��
typedef enum {
    STATUS_PENDING_IN,    // �����
    STATUS_UNSENT,      // δ�ĳ�
    STATUS_SENT,        // �Ѽĳ�
    STATUS_UNPICKED,    // δȡ��
    STATUS_PICKED,      // ��ȡ��
    STATUS_OVERDUE,     // ����
    STATUS_LOST,        // ��ʧ 
    STATUS_DAMAGED,     // ��
    STATUS_MISPICKED,   // ����
    STATUS_WRONGPICKED,   // ð��
    STATUS_ABNORMAL,    // �쳣����������
    STATUS_ABNORMAL_RESOLVED, // �쳣�Ѵ���
    STATUS_REJECTED,    //����
    STATUS_RETURNED,     //�˻�
    STATUS_UNPICKED_AGAIN  //���´�ȡ��
} PackageStatus;

// ��������ö��
typedef enum {
    DAILY_GOODS,    // ����Ʒ
    FOOD,           // ʳƷ
    DOCUMENT,       // �ļ�
    CLOTHING,       // ����
    DIGITAL,        // �����Ʒ
    ALCOHOL,        // ���ࣨ�����ʼģ�
    ANIMAL,         // ��������ʼģ�
    WEAPON,         // �����������ʼģ�
    HAZARDOUS,      // ��ȼ/�ױ�/�ж���Ʒ�������ʼģ�
    VALUABLES,      // ������Ʒ
    FRESH,          // ����
    FRAGILE,        // ����Ʒ
    OTHER,          // ����
    BANNED_ITEMS    // �����ʼ���Ʒ��ǣ��ڲ�ʹ�ã�
} PackageType;

// ���䷽ʽö��
typedef enum {
    SHIPPING_NORMAL,   // ��ͨ���
    SHIPPING_BULK,         // ����ʼ�
    SHIPPING_COLD_CHAIN    // �����ʼ�
} ShippingMethod;

// ��������ö��
typedef enum {
    REGION_PROVINCE,  // ʡ��
    REGION_OUTOFPROVINCE,    // ʡ��
    REGION_INTERNATIONAL, // ����
    REGION_HONGKONG_MACAO // �۰�̨
} RegionType;
// ����ʱЧö��
typedef enum {
    STANDARD_DELIVERY,  // ��죨Ĭ�ϣ�
    EXPRESS_DELIVERY        // �ؿ�
} DeliveryType;
typedef struct {
    double base_weight_fee;         // ��������(kg)
    double additional_weight_fee;   // ���ص���(Ԫ/kg)
} RateConfig;
// Ͷ������ö��
typedef enum {
    COMPLAINT_LOST,     // ������ʧ
    COMPLAINT_DAMAGED   // ������
} ComplaintType;

// �����Ϣ�ṹ��
typedef struct Express {
    int send_num;
    char sender_name[50];
    char sender_phone[12];
    char receiver_name[50];
    char receiver_phone[12];
    double weight;         // ʵ������(kg)
    double volume;         // ���(cm?)
    PackageType package_type;
    ShippingMethod method;
    RegionType region;
    int is_insured;       // �Ƿ񱣼�(0/1)
    double insured_value;  // ��Ʒ��ֵ
    DeliveryType express_type;
    double fee;            // �˷�
    PackageStatus status;      // ���״̬
    time_t create_time;   // ����ʱ��
    char pickup_code[15];       // 6λȡ����+'\0'
    char tracking_num[20];	 // ��ݵ���
    char shelf_id[10];	 // ���ܺ�
    char pickup_phone[12];	 // ȡ���˵绰����
    time_t storage_time;   // ���ʱ��
    int complaint_id;          // Ͷ��ID
    ComplaintType complaint_type; // Ͷ������(��ʧ/��)
    char complaint_desc[256];  // Ͷ������
    char complaint_resolution[256]; // ������
    double compensation;       // �⳥���
    time_t complaint_time;     // Ͷ��ʱ��
    time_t resolve_time;       // ����ʱ��
    struct Express* next;
    char reject_reason[100];  // ����ԭ��
    time_t reject_time;       // ����ʱ��
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

// ��������ö��
typedef enum {
    SHELF_NORMAL,    // ��ͨ����
    SHELF_FRAGILE,   // ����Ʒ����
    SHELF_COLD,      // ��������
    SHELF_BULK       // �������
} ShelfType;

// ���ܽṹ��
typedef struct Shelf {
    char shelf_id[10];         // ����ID
    ShelfType type;            // ��������
    int max_capacity;          // �������
    int current_count;         // ��ǰ����
    float temperature;         // �¶ȣ�����ר�ã�
    int security_level;        // ��ȫ�ȼ�������Ʒר�ã�
    ExpressNode* pickup_list; // �������
    struct Shelf* next;
} Shelf;


// ȫ�ֱ���
extern User* user_list;      // �û�����ͷָ��
extern int user_count;       // �û�����
extern int send_num;         //�ĳ�����
extern Admin* admin_list;    // ����Ա����ͷָ��
extern int admin_count;      // ����Ա����
extern Shelf* shelf_list;
extern ExpressNode* send_list; // �������ͷָ��
extern ExpressNode* pickup_list; // �������ͷָ��
extern int current_count;

// ���峣��
#define MAX_USERNAME_LEN 50
#define MAX_PHONE_LEN 12
#define MAX_PASSWORD_LEN 50
// �ļ�·��
#define SEND_FILE "send_express.txt"
#define PICKUP_FILE "pickup_express.txt"
// ���˵����û����
void main_menu(void);
void user_menu(User* user);
void admin_menu(Admin* admin);
void admin_management_menu(Admin* current_admin);
void user_management_menu(Admin* admin);
void view_all_users(void);
void shelf_management_menu(Admin* admin);
void inventory_menu(Shelf* head);
void express_service_menu(User* user);

//���ʼ������
float calculate_fee(User* user, float base_price);

// ��ݽڵ㴴�����Ƶ�������������
ExpressNode* create_express_node(Express* data);
// --------------------�Ŀ�����̺���--------------------
const char* get_package_type_name(PackageType type);//��ȡ��Ʒ��������
int is_valid_phone(const char* phone);// ����Ƿ�����Ч�� 11 λ�ֻ���
int input_int_in_range(const char* prompt, int min, int max);// ��ȡ�û����뷶Χ�ڵ�����
void input_phone(char* phone, size_t size, const char* prompt);
void safe_input(char* buf, size_t size, const char* prompt);
double input_positive_double(const char* prompt);// ����һ���������������� > 0��
void send_express(User* user_head);//�Ŀ�ݺ��ĺ���
void modify_express(User* user);// �޸Ŀ����Ϣ
void delete_express(User* user);// ɾ�������Ϣ
void find_pending_express(User* user);// ��ѯδ�ĳ����
void show_pending_bills(User* user);// ��ʾ��֧���˵�
void send_express_menu(User* user);// �Ŀ�ݲ˵�
// --------------------�˷Ѽ��㺯��--------------------
int get_throw_ratio(ShippingMethod method, RegionType region);// ��ȡ�ױ�ϵ��
double calculate_insurance(double insured_value, PackageType type);// ���۷��ü���
RateConfig get_base_rates(ShippingMethod method, RegionType region, PackageType type, DeliveryType express_type);//��ȡ�������غ���
double calculate_shipping_fee(Express* express, User* user);//�˷Ѽ�����ĺ���
// --------------------ȡ������̺���--------------------
void pickup_express(User* current_user);
void process_pickup(User* current_user);
// --------------------�ļ����溯��--------------------
void save_send_express_info(ExpressNode* head, const char* filename);
void save_pickup_express_info(ExpressNode* head, const char* filename);
int count_nodes(ExpressNode* head);

// --------------------��ݹ�����--------------------
void manage_express(Admin* admin);
void load_pickup_express(ExpressNode** express_head);
void manage_pending_express();
void load_send_express(ExpressNode** express_head);
// ���촦����
void mistaken_pickup(User* current_user);
// �ύͶ��
void submit_complaint(User* user);
// �鿴�û�Ͷ��
void view_user_complaints(User* user);
// --------------------�û�������--------------------
void init_user_privilege(User* user);// ��ʼ���û���Ȩ
User* create_user(const char* phone, const char* pwd, const char* name, UserLevel level);// �������û�
User* find_user_by_username(User* head, const char* username);
User* find_user_by_phone(User* head, const char* phone);
bool save_users_to_file(User* head, const char* filename);// �����û���Ϣ���ļ�
User* load_users_from_file(const char* filename);// ���ļ������û���Ϣ��β�巨)
bool delete_user(User** head, const char* username);// ע���û�
void add_user(User** head, User* new_user);
void view_my_info(User* user);
void user_login();// �û���¼
void user_register();// �û�ע�ắ��
bool modify_user_info(User* user);//�޸��û���Ϣ
// �û�ע������
bool user_unregister(User** user_list, int* user_count, User** current_user);

// --------------------����Ա������--------------------
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

// --------------------���ܺ���--------------------
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

//����Ա�������
void admin_process_rejected_packages();
void view_rejected_packages(User* current_user);
void reject_package_menu(User* current_user);
void reject_package(User* current_user);
void admin_reject_management_menu();

//--------------------����ԱͶ�ߴ���ģ��--------------------
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

//--------------------�û�Ͷ��ģ��--------------------
// Ͷ����غ���
int generate_complaint_id(ExpressNode* head);
void submit_complaint(User* user);
void display_complaint(Express* express);
void view_user_complaints(User* user);

#endif // CAINIAO_H



