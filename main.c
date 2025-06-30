#include "cainiao.h"

// ������
int main() {
    // ��ʼ��ϵͳ
    //initialize_system();

    // ��������

    load_pickup_express(&pickup_list);
    load_send_express(&send_list);
    user_list = load_users_from_file("users.txt");
    admin_list = load_admins_from_file("admins.txt");
    shelf_list = load_shelves_from_file("shelves.txt", pickup_list);



    // ͳ������

    User* curr = user_list;
    while (curr) {
        user_count++;
        curr = curr->next;
    }

    Admin* current = admin_list;
    while (current) {
        admin_count++;
        current = current->next;

    }

    // ��ʾ���˵�
    main_menu();

    // �����˳�ǰ��������
    save_users_to_file(user_list, "users.txt");
    save_admins_to_file(admin_list, "admins.txt");
    save_shelves_to_file(shelf_list, "shelves.txt");
    save_pickup_express_info(pickup_list, "pickup_express.txt");
    save_send_express_info(send_list, SEND_FILE);


    return 0;
}