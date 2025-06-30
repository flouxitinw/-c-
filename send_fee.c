// cainiao.c
#include "cainiao.h"


//--------------------��������--------------------
// ��ȡ�ױ�ϵ��
int get_throw_ratio(ShippingMethod method, RegionType region) {
    if (method == SHIPPING_COLD_CHAIN) {
        return 3000;
    }
    else {
        switch (region) {
        case REGION_PROVINCE: return 12000;
        case REGION_OUTOFPROVINCE: return 6000;
        case REGION_INTERNATIONAL: return 5000;
        default:return 6000;
        }
    }
}

// ���۷��ü���
double calculate_insurance(double insured_value, PackageType type) {
    double insurance = 0.0;
    if (type == FRAGILE) {  // ����Ʒ
        if (insured_value <= 500.0) {
            insurance = 2.0;  // 500Ԫ���¹̶�2Ԫ
        }
        else if (insured_value <= 1000.0) {
            insurance = 5.0;  // 501-1000Ԫ�̶�5Ԫ
        }
        else {
            insurance = insured_value * 0.008;  // 1000Ԫ����8��
        }
    }
    else {  // ��ͨ��Ʒ
        if (insured_value <= 500.0) {
            insurance = 1.0;  // 500Ԫ���¹̶�1Ԫ
        }
        else if (insured_value <= 1000.0) {
            insurance = 2.0;  // 501-1000Ԫ�̶�2Ԫ
        }
        else {
            insurance = insured_value * 0.005;  // 1000Ԫ����5��
        }
    }

    // �������뵽�֣�0.01Ԫ��
    return round(insurance * 100) / 100;
}

//��ȡ�������غ���
RateConfig get_base_rates(ShippingMethod method, RegionType region, PackageType type, DeliveryType express_type) {
    RateConfig config = { 0 };

    // Ĭ�Ϸ��ʣ���ͨ���/����/����Ʒ��
    config.base_weight_fee = 1.0;       // ����1ǧ��
    config.additional_weight_fee = 2.0; // ����2Ԫ/ǧ��
    // �������䷽ʽ����
    if (region == REGION_INTERNATIONAL) {
        config.base_weight_fee = 30;
        config.additional_weight_fee = 20;
    }

    else if (region == REGION_HONGKONG_MACAO) {
        config.base_weight_fee = 40;
        config.additional_weight_fee = 30;
    }
    else {
        if (method == SHIPPING_COLD_CHAIN) {
            if (region == REGION_PROVINCE) {
                config.base_weight_fee = 15.0;
                config.additional_weight_fee = 3.0;
            }
            else {
                config.base_weight_fee = 20.0;
                config.additional_weight_fee = 13.0;
            }
        }
        if (method == SHIPPING_BULK) {
            if (region == REGION_PROVINCE) {
                config.base_weight_fee = 14.0;
                config.additional_weight_fee = 2.0;
            }
            else {
                config.base_weight_fee = 18.0;
                config.additional_weight_fee = 7.0;
            }
        }
        else {
            if (region == REGION_PROVINCE) {
                config.base_weight_fee = 14.0;
                config.additional_weight_fee = 2.7;
            }
            else {
                config.base_weight_fee = 18.0;
                config.additional_weight_fee = 7.0;
            }
        }
    }
    if (express_type == EXPRESS_DELIVERY) {
        config.base_weight_fee += 2.0;
        config.additional_weight_fee += 2.0;
    }
    return config;
}

// �����û��ȼ��������
float calculate_fee(User* user, float base_price) {
    // �����̻��ж���Э���ۿ�
    if (user->level == USER_PARTNER) {
        return base_price * user->discount_rate * 0.9f; // Э����ٴ�9��
    }
    return base_price * user->discount_rate;
}
//--------------------�˷Ѻ��ĺ���--------------------
double calculate_shipping_fee(Express* express, User* user) {
    // �����������
    double volume_weight = express->volume / get_throw_ratio(express->method, express->region);
    double charge_weight = fmax(express->weight, volume_weight);

    // ������������
    RateConfig config = get_base_rates(express->method, express->region, express->package_type, express->express_type);

    // ��������˷�
    double base_fee = 0;
    if (charge_weight <= 1.0) {
        base_fee = config.base_weight_fee;
    }
    else {
        base_fee = config.base_weight_fee + (charge_weight - 1.0) * config.additional_weight_fee;
    }

    // ���۷�
    double total_fee = base_fee;  // ������������ʼ��total_fee
    if (express->is_insured) {
        total_fee += calculate_insurance(express->insured_value, express->package_type);
    }
    double final_fee = calculate_fee(user, total_fee);


    // �������봦��
    final_fee = round(final_fee * 100) / 100;
    express->fee = final_fee;
    return final_fee;

}