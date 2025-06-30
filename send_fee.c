// cainiao.c
#include "cainiao.h"


//--------------------辅助函数--------------------
// 获取抛比系数
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

// 保价费用计算
double calculate_insurance(double insured_value, PackageType type) {
    double insurance = 0.0;
    if (type == FRAGILE) {  // 易碎品
        if (insured_value <= 500.0) {
            insurance = 2.0;  // 500元以下固定2元
        }
        else if (insured_value <= 1000.0) {
            insurance = 5.0;  // 501-1000元固定5元
        }
        else {
            insurance = insured_value * 0.008;  // 1000元以上8‰
        }
    }
    else {  // 普通物品
        if (insured_value <= 500.0) {
            insurance = 1.0;  // 500元以下固定1元
        }
        else if (insured_value <= 1000.0) {
            insurance = 2.0;  // 501-1000元固定2元
        }
        else {
            insurance = insured_value * 0.005;  // 1000元以上5‰
        }
    }

    // 四舍五入到分（0.01元）
    return round(insurance * 100) / 100;
}

//获取首重续重函数
RateConfig get_base_rates(ShippingMethod method, RegionType region, PackageType type, DeliveryType express_type) {
    RateConfig config = { 0 };

    // 默认费率（普通快递/国内/日用品）
    config.base_weight_fee = 1.0;       // 首重1千克
    config.additional_weight_fee = 2.0; // 续重2元/千克
    // 根据运输方式调整
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

// 根据用户等级计算费用
float calculate_fee(User* user, float base_price) {
    // 合作商户有额外协议折扣
    if (user->level == USER_PARTNER) {
        return base_price * user->discount_rate * 0.9f; // 协议价再打9折
    }
    return base_price * user->discount_rate;
}
//--------------------运费核心函数--------------------
double calculate_shipping_fee(Express* express, User* user) {
    // 体积重量计算
    double volume_weight = express->volume / get_throw_ratio(express->method, express->region);
    double charge_weight = fmax(express->weight, volume_weight);

    // 基础费率配置
    RateConfig config = get_base_rates(express->method, express->region, express->package_type, express->express_type);

    // 计算基础运费
    double base_fee = 0;
    if (charge_weight <= 1.0) {
        base_fee = config.base_weight_fee;
    }
    else {
        base_fee = config.base_weight_fee + (charge_weight - 1.0) * config.additional_weight_fee;
    }

    // 保价费
    double total_fee = base_fee;  // 这里声明并初始化total_fee
    if (express->is_insured) {
        total_fee += calculate_insurance(express->insured_value, express->package_type);
    }
    double final_fee = calculate_fee(user, total_fee);


    // 四舍五入处理
    final_fee = round(final_fee * 100) / 100;
    express->fee = final_fee;
    return final_fee;

}