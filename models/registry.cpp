#include "models.h"

std::map<std::string, std::map<std::string, llm_hparams_item>> & get_hparams_map() {
    static std::map<std::string, std::map<std::string, llm_hparams_item>> hparams_maps;
    return hparams_maps;
}

std::map<std::string, std::map<std::string, llm_weight_item>> & get_weights_map() {
    static std::map<std::string, std::map<std::string, llm_weight_item>> weights_maps;
    return weights_maps;
}

std::map<std::string, llm_rope_type>& get_rope_type() {
    static std::map<std::string, llm_rope_type> rope_types;
    return rope_types;
}

std::map<std::string, float>& get_freq_scale() {
    static std::map<std::string, float> freq_scales;
    return freq_scales;
}

std::map<std::string, float>& get_freq_base() {
    static std::map<std::string, float> freq_bases;
    return freq_bases;
}

std::map<std::string, float>& get_ext_factor() {
    static std::map<std::string, float> ext_factors;
    return ext_factors;
}

std::map<std::string, float>& get_attn_factor() {
    static std::map<std::string, float> attn_factors;
    return attn_factors;
}

std::map<std::string, float>& get_beta_fast() {
    static std::map<std::string, float> beta_fasts;
    return beta_fasts;
}

std::map<std::string, float>& get_beta_slow() {
    static std::map<std::string, float> beta_slows;
    return beta_slows;
}

std::map<std::string, template_caller> & get_template_callers() {
    static std::map<std::string, template_caller> template_callers;
    return template_callers;
}
