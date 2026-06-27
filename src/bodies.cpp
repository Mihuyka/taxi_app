#include "header.hpp"
#include <stdexcept>
#include <time.h>
#include <cstdlib>

void locale_manager::load_language(const Glib::ustring& filename) {
    m_dict.clear();
    std::ifstream file(filename.c_str());
    if (!file.is_open()) { return; }
    
    char buffer[2048];
    
    while (file.getline(buffer, sizeof(buffer))) {
        Glib::ustring line(buffer);
        
        std::size_t pos = line.find('=');
        
        if (pos != Glib::ustring::npos) {
            Glib::ustring key = line.substr(0, pos);
            Glib::ustring value = line.substr(pos + 1);
            
            std::size_t pos_n = 0;
            while ((pos_n = value.find("\\n", pos_n)) != Glib::ustring::npos) {
                value.replace(pos_n, 2, "\n");
                pos_n += 1;
            }
            
            m_dict[key] = value;
        }
    }
    file.close();
}

Glib::ustring locale_manager::get_text(const Glib::ustring& key) {
    if (m_dict.find(key) != m_dict.end()) {
        return m_dict[key];
    }
    return key;
}

void auth_box::update_language(locale_manager& loc) {
    m_p_loc = &loc;
    
    m_button_login.set_label(loc.get_text("btn_login"));
    m_button_register.set_label(loc.get_text("btn_register"));
    m_entry_name.set_placeholder_text(loc.get_text("entry_login"));
    m_entry_password.set_placeholder_text(loc.get_text("entry_password"));
}

base_dialog::base_dialog(const Glib::ustring& title) :
    m_main_box(Gtk::Orientation::VERTICAL, 10)
{
    set_title(title);
    set_default_size(400, 400);
    m_main_box.set_margin(10);
    set_child(m_main_box);
    
    set_hide_on_close(true);
    
    set_modal(true);
}

help_dialog::help_dialog() :
    base_dialog("") 
{
    m_text_buffer = Gtk::TextBuffer::create();
    m_textv_info.set_buffer(m_text_buffer);
    m_textv_info.set_editable(false);
    m_textv_info.set_wrap_mode(Gtk::WrapMode::WORD);
    
    m_scroll_window.set_child(m_textv_info);
    m_scroll_window.set_expand(true);
    
    m_main_box.append(m_scroll_window);
}

void help_dialog::update_language(locale_manager& loc) {
    set_title(loc.get_text("title_help"));
    m_text_buffer->set_text(loc.get_text("help_text"));
}

admin_dialog::admin_dialog() :
    base_dialog(""),
    m_box_split(Gtk::Orientation::HORIZONTAL, 20),
    m_box_users(Gtk::Orientation::VERTICAL, 5),
    m_box_tariffs(Gtk::Orientation::VERTICAL, 5)
{
    m_p_loc = nullptr;
    set_default_size(700, 500);
    m_main_box.append(m_box_split);
    
    m_scroll_window_users.set_expand(true);
    m_scroll_window_users.set_child(m_listbox_users);
    m_box_users.append(m_scroll_window_users);
    m_box_users.append(m_button_delete_user);
    m_box_split.append(m_box_users);
    
    m_scroll_window_tariffs.set_expand(true);
    m_scroll_window_tariffs.set_child(m_listbox_tariffs);
    m_box_tariffs.append(m_scroll_window_tariffs);
    
    m_entry_tariff_name.set_placeholder_text("Название тарифа");
    m_entry_car_brand.set_placeholder_text("Марка авто");
    
    std::shared_ptr<Gtk::Adjustment> adj_price = Gtk::Adjustment::create(
        1.0, 0.1, 100.0, 0.1
    );
    m_spin_price.set_adjustment(adj_price);
    m_spin_price.set_digits(2);
    
    std::shared_ptr<Gtk::Adjustment> adj_child = Gtk::Adjustment::create(
        1.5, 0.1, 100.0, 0.1
    );
    m_spin_child_price.set_adjustment(adj_child);
    m_spin_child_price.set_digits(2);
    
    m_box_tariffs.append(m_entry_tariff_name);
    m_box_tariffs.append(m_entry_car_brand);
    
    m_label_price.set_halign(Gtk::Align::START);
    m_box_tariffs.append(m_label_price);
    m_box_tariffs.append(m_spin_price);
    
    m_box_tariffs.append(m_check_has_child);
    
    m_label_child_price.set_halign(Gtk::Align::START);
    m_box_tariffs.append(m_label_child_price);
    m_box_tariffs.append(m_spin_child_price);
    
    m_box_tariffs.append(m_button_add_tariff);
    m_box_tariffs.append(m_button_delete_tariff);
    m_box_split.append(m_box_tariffs);
    
    m_button_delete_user.signal_clicked().connect(
        sigc::mem_fun(*this, &admin_dialog::on_button_delete_user_clicked)
    );
    m_button_add_tariff.signal_clicked().connect(
        sigc::mem_fun(*this, &admin_dialog::on_button_add_tariff_clicked)
    );
    m_button_delete_tariff.signal_clicked().connect(
        sigc::mem_fun(*this, &admin_dialog::on_button_delete_tariff_clicked)
    );
}

void admin_dialog::update_language(locale_manager& loc) {
    set_title(loc.get_text("title_admin"));
    m_p_loc = &loc;
    
    m_button_delete_user.set_label(loc.get_text("btn_delete"));
    m_button_add_tariff.set_label(loc.get_text("btn_add_tariff"));
    m_button_delete_tariff.set_label(loc.get_text("btn_del_tariff"));
    m_check_has_child.set_label(loc.get_text("check_admin_child"));
    
    m_entry_tariff_name.set_placeholder_text(loc.get_text("admin_tariff_name"));
    m_entry_car_brand.set_placeholder_text(loc.get_text("admin_car_brand"));
    
    m_label_price.set_text(loc.get_text("admin_lbl_price"));
    m_label_child_price.set_text(loc.get_text("admin_lbl_child_price"));
}

void admin_dialog::load_tariffs() {
    while (Gtk::Widget* child = m_listbox_tariffs.get_first_child()) {
        m_listbox_tariffs.remove(*child);
    }
    std::vector<tariff_record> tariffs = m_tariff_manager.get_all_tariffs();
    for (std::size_t i = 0; i < tariffs.size(); ++i) {
        Gtk::Label* label = Gtk::manage(new Gtk::Label(tariffs[i].name));
        label->set_halign(Gtk::Align::START);
        m_listbox_tariffs.append(*label);
    }
}

sigc::signal<void()> admin_dialog::on_tariffs_changed() {
    return m_signal_tariffs_changed;
}

void admin_dialog::on_button_add_tariff_clicked() {
    tariff_record new_rec;
    std::memset(&new_rec, 0, sizeof(tariff_record));
    
    Glib::ustring t_name = m_entry_tariff_name.get_text();
    Glib::ustring c_brand = m_entry_car_brand.get_text();
    
    if (t_name.empty() || c_brand.empty()) return;
    
    std::strncpy(new_rec.name, t_name.c_str(), sizeof(new_rec.name) - 1);
    std::strncpy(new_rec.car_brand, c_brand.c_str(), sizeof(new_rec.car_brand) - 1);
    
    new_rec.price_per_km = m_spin_price.get_value();
    new_rec.has_child_seat = m_check_has_child.get_active();
    new_rec.child_price_per_km = m_spin_child_price.get_value();
    
    m_tariff_manager.save_tariff(new_rec);
    load_tariffs();
    
    m_signal_tariffs_changed.emit();
    
    m_entry_tariff_name.set_text("");
    m_entry_car_brand.set_text("");
    m_spin_price.set_value(1.0);
    m_spin_child_price.set_value(1.5);
    m_check_has_child.set_active(false);
}

void admin_dialog::on_button_delete_tariff_clicked() {
    Gtk::ListBoxRow* row = m_listbox_tariffs.get_selected_row();
    if (row != nullptr) {
        Gtk::Widget* child = row->get_child();
        Gtk::Label* label = dynamic_cast<Gtk::Label*>(child);
        if (label != nullptr) {
            m_tariff_manager.delete_tariff(label->get_text());
            load_tariffs();
            
            m_signal_tariffs_changed.emit();
        }
    }
}

void admin_dialog::on_button_delete_user_clicked() {
    Gtk::ListBoxRow* row = m_listbox_users.get_selected_row();
    
    if (row != nullptr) {
        Gtk::Widget* child = row->get_child();
        Gtk::Label* label = dynamic_cast<Gtk::Label*>(child);
        
        if (label != nullptr) {
            Glib::ustring username = label->get_text();
            
            if (username != "admin") {
                m_auth_manager.delete_user(username);
                load_users();
            }
        }
    }
}

void admin_dialog::load_users() {
    while (Gtk::Widget* child = m_listbox_users.get_first_child()) {
        m_listbox_users.remove(*child);
    }
    
    std::ifstream file("users.bin", std::ios::binary);
    if (!file.is_open()) { return; }
    
    user_record current_user;
    
    while (file.read(reinterpret_cast<char*>(&current_user), sizeof(user_record))) {
        Glib::ustring username_str = current_user.name;
        
        Gtk::Label* label = Gtk::manage(new Gtk::Label(username_str));
        label->set_halign(Gtk::Align::START);
        label->set_margin(5);
        m_listbox_users.append(*label);
    }
    file.close();
}

std::size_t auth_manager::hash_fun(const Glib::ustring& str) {
    std::size_t hash_code = 14695981039346656037ULL;
    const std::size_t fnv_prime = 1099511628211ULL;
    
    for (std::size_t i = 0; i < str.length(); ++i) {
        hash_code = (hash_code ^ static_cast<std::size_t>(str[i])) * fnv_prime;
    }
    
    return hash_code;
}

bool auth_manager::user_exists(const Glib::ustring& name) {
    std::ifstream file("users.bin", std::ios::binary);
    if (!file.is_open()) { return false; }
    
    user_record current_user;
    
    while (file.read(reinterpret_cast<char*>(&current_user), sizeof(user_record))) {
        if (name == current_user.name) {
            file.close();
            return true;
        }
    }
    file.close();
    return false;
}

bool auth_manager::register_user(
    const Glib::ustring& name, const Glib::ustring& password, user_role role
) {
    if (name.empty() || password.empty()) throw std::invalid_argument("err_empty_fields");
    if (name.length() > 31) throw std::length_error("err_name_len");
    if (password.length() > 50) throw std::length_error("err_pass_len");
    if (user_exists(name)) throw std::runtime_error("err_user_exists");

    user_record new_user;
    std::memset(&new_user, 0, sizeof(user_record));
    std::strncpy(new_user.name, name.c_str(), sizeof(new_user.name) - 1);
    new_user.password_hash = hash_fun(password);
    new_user.role = role;

    std::ofstream file("users.bin", std::ios::binary | std::ios::app);
    if (!file.is_open()) throw std::ios_base::failure("err_file_write");
    
    file.write(reinterpret_cast<const char*>(&new_user), sizeof(user_record));
    file.close();
    return true;
}

bool auth_manager::login(
    const Glib::ustring& name, const Glib::ustring& password, user_role& role
) {
    std::ifstream file("users.bin", std::ios::binary);
    if (!file.is_open()) { return false; }
    
    std::size_t target_hash = hash_fun(password);
    
    user_record current_user;
    
    while (file.read(reinterpret_cast<char*>(&current_user), sizeof(user_record))) {
        if (name == current_user.name && target_hash == current_user.password_hash) {
            role = current_user.role;
            file.close();
            return true;
        }
    }
    
    file.close();
    return false;
}

bool auth_manager::delete_user(const Glib::ustring& name) {
    std::ifstream file_in("users.bin", std::ios::binary);
    if (!file_in.is_open()) { return false; }
    
    std::vector<user_record> users;
    user_record current_user;
    
    while (file_in.read(reinterpret_cast<char*>(&current_user), sizeof(user_record))) {
        if (name != current_user.name) {
            users.push_back(current_user);
        }
    }
    file_in.close();
    
    std::ofstream file_out("users.bin", std::ios::binary | std::ios::trunc);
    if (!file_out.is_open()) { return false; }
    
    for (std::size_t i = 0; i < users.size(); ++i) {
        file_out.write(reinterpret_cast<const char*>(&users[i]), sizeof(user_record));
    }
    file_out.close();
    return true;
}

void tariff_manager::save_tariff(const tariff_record& record) {
    std::ofstream file("tariffs.bin", std::ios::binary | std::ios::app);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&record), sizeof(tariff_record));
        file.close();
    }
}

std::vector<tariff_record> tariff_manager::get_all_tariffs() {
    std::vector<tariff_record> tariffs;
    std::ifstream file("tariffs.bin", std::ios::binary);
    if (!file.is_open()) return tariffs;
    
    tariff_record current;
    while (file.read(reinterpret_cast<char*>(&current), sizeof(tariff_record))) {
        tariffs.push_back(current);
    }
    file.close();
    return tariffs;
}

void tariff_manager::delete_tariff(const Glib::ustring& name) {
    std::vector<tariff_record> tariffs = get_all_tariffs();
    std::ofstream file("tariffs.bin", std::ios::binary | std::ios::trunc);
    if (!file.is_open()) return;
    
    for (std::size_t i = 0; i < tariffs.size(); ++i) {
        Glib::ustring current_name = tariffs[i].name;
        if (current_name != name) {
            file.write(reinterpret_cast<const char*>(&tariffs[i]), sizeof(tariff_record));
        }
    }
    file.close();
}

auth_box::auth_box() :
    Gtk::Box(Gtk::Orientation::VERTICAL, 10)
    {
        m_p_loc = nullptr;   
        
        set_margin(60);   
        
        m_entry_name.set_placeholder_text("Логин");
        m_entry_password.set_placeholder_text("Пароль");
        m_entry_password.set_visibility(false);
        
        append(m_entry_name);
        append(m_entry_password);
        append(m_button_login);
        append(m_button_register);
        append(m_label_status);
        
        m_button_login.signal_clicked().connect(
            sigc::mem_fun(*this, &auth_box::on_button_login_clicked)
        );
        m_button_register.signal_clicked().connect(
            sigc::mem_fun(*this, &auth_box::on_button_register_clicked)
        );
    }
    
sigc::signal<void(user_role)> auth_box::on_signal_login() {
    return m_signal_login;
}

sigc::signal<void(const Glib::ustring&, const Glib::ustring&)> auth_box:: on_signal_error() {
    return m_signal_error;
}
    
void auth_box::on_button_login_clicked() {
    Glib::ustring name = m_entry_name.get_text();
    Glib::ustring password= m_entry_password.get_text();
    user_role current_role;
    
    if (m_auth_manager.login(name, password, current_role)) {
        if (m_p_loc) m_label_status.set_text(m_p_loc->get_text("msg_success_login"));
        m_signal_login.emit(current_role);
    } else {
        if (m_p_loc) m_signal_error.emit(m_p_loc->get_text("err_auth_title"), m_p_loc->get_text("err_auth_msg"));
    }
}

void auth_box::on_button_register_clicked() {
    Glib::ustring name = m_entry_name.get_text();
    Glib::ustring password = m_entry_password.get_text();
    user_role role;
    
    if (name == "admin" && password == "7777") {
        role = user_role::admin;
    } else if (name == "admin") {
        if (m_p_loc) m_signal_error.emit(m_p_loc->get_text("err_reg_title"), m_p_loc->get_text("err_admin_taken"));
        return;
    } else {
        role = user_role::user;
    }
    
    try {
        m_auth_manager.register_user(name, password, role);
        if (m_p_loc) m_label_status.set_text(m_p_loc->get_text("msg_success_reg"));
    } catch (const std::length_error& error) {
        if (m_p_loc) m_signal_error.emit(m_p_loc->get_text("err_len_title"), m_p_loc->get_text(error.what()));
    } catch (const std::invalid_argument& error) {
        if (m_p_loc) m_signal_error.emit(m_p_loc->get_text("err_input_title"), m_p_loc->get_text(error.what()));
    } catch (const std::exception& error) {
        if (m_p_loc) m_signal_error.emit(m_p_loc->get_text("err_sys_title"), m_p_loc->get_text(error.what()));
    }
}

main_box::main_box() :
    Gtk::Box(Gtk::Orientation::HORIZONTAL, 10),
    m_box_input(Gtk::Orientation::VERTICAL, 10),
    m_box_output(Gtk::Orientation::VERTICAL, 10),
    m_v_label_title(5),
    m_v_label_result(5)
{
        m_p_loc = nullptr;
    
        srand(time(nullptr));   
        
        set_margin(10);
        append(m_box_input);
        
        m_box_output.set_margin(60);
        m_frame_output.set_child(m_box_output);
        m_frame_output.set_expand(true);
        append(m_frame_output);
        
        std::shared_ptr<Gtk::Adjustment> adj_dist = Gtk::Adjustment::create(5.0, 1.0, 1000.0, 1.0);
        m_spin_distance.set_adjustment(adj_dist);
        m_spin_distance.set_digits(1);
    
        std::shared_ptr<Gtk::Adjustment> adj_time = Gtk::Adjustment::create(10.0, 1.0, 1000.0, 1.0);
        m_spin_trip_time.set_adjustment(adj_time);
    
        std::shared_ptr<Gtk::Adjustment> adj_wait = Gtk::Adjustment::create(0.0, 0.0, 1000.0, 1.0);
        m_spin_wait_time.set_adjustment(adj_wait);
    
        m_box_input.append(m_combo_tariff);
        m_box_input.append(m_combo_payment);
    
        m_label_km.set_halign(Gtk::Align::START);
        m_box_input.append(m_label_km);
        m_box_input.append(m_spin_distance);
    
        m_label_trip_time.set_halign(Gtk::Align::START);
        m_box_input.append(m_label_trip_time);
        m_box_input.append(m_spin_trip_time);
    
        m_label_wait_time.set_halign(Gtk::Align::START);
        m_box_input.append(m_label_wait_time);
        m_box_input.append(m_spin_wait_time);
    
        m_box_input.append(m_check_child);
        m_box_input.append(m_check_trunk);
        m_box_input.append(m_button_calculate);
        m_box_input.append(m_button_admin_panel); 
        
        for (short int i = 0; i < 5; ++i) {
            m_grid.attach(m_v_label_title[i], 0, i);
            m_v_label_title[i].set_halign(Gtk::Align::START);
            
            m_grid.attach(m_v_label_result[i], 1, i);
            m_v_label_result[i].set_halign(Gtk::Align::END);
        }
        m_box_output.append(m_grid);
        
        m_button_calculate.signal_clicked().connect(
            sigc::mem_fun(*this, &main_box::on_button_calculate_clicked)
        );
        m_button_admin_panel.signal_clicked().connect(
            sigc::mem_fun(*this, &main_box::on_button_admin_panel_clicked)
        );
}

sigc::signal<void(const Glib::ustring&, const Glib::ustring&)> main_box::on_signal_error() {
    return m_signal_error;
}

void main_box::load_tariffs_to_ui() {
    m_combo_tariff.remove_all();
    m_loaded_tariffs = m_tariff_manager.get_all_tariffs();
    for (std::size_t i = 0; i < m_loaded_tariffs.size(); ++i) {
        m_combo_tariff.append(m_loaded_tariffs[i].name);
    }
    if (!m_loaded_tariffs.empty()) m_combo_tariff.set_active(0);
}

void main_box::set_user_role(user_role index) {
    if (index == user_role::admin) {
        m_button_admin_panel.set_visible(true);
    } else if (index == user_role::user) {
        m_button_admin_panel.set_visible(false);
    }
}

sigc::signal<void()> main_box::on_signal_admin() {
    return m_signal_admin;
}

void main_box::on_button_admin_panel_clicked() {
    m_signal_admin.emit();
}

void main_box::on_button_calculate_clicked() {
    int tariff_idx = m_combo_tariff.get_active_row_number();
    if (tariff_idx < 0 || tariff_idx >= static_cast<int>(m_loaded_tariffs.size())) {
        if (m_p_loc) m_signal_error.emit(m_p_loc->get_text("err_title"), m_p_loc->get_text("err_no_tariff"));
        return;
    }
    
    tariff_record selected_tariff = m_loaded_tariffs[tariff_idx];
    
    int payment_idx = m_combo_payment.get_active_row_number();
    double distance = m_spin_distance.get_value();
    int trip_time = m_spin_trip_time.get_value_as_int();
    int wait_time = m_spin_wait_time.get_value_as_int();
    
    bool child = m_check_child.get_active();
    bool trunk = m_check_trunk.get_active();
    
    if (child && !selected_tariff.has_child_seat) {
        if (m_p_loc) m_signal_error.emit(m_p_loc->get_text("err_book_title"), m_p_loc->get_text("err_no_child_seat"));
        return;
    }
    
    double min_distance = 3.0;
    double calc_distance = (distance < min_distance) ? min_distance : distance;
    
    double base_cost = 0.0;
    if (child) {
        base_cost = calc_distance * selected_tariff.child_price_per_km;
    } else {
        base_cost = calc_distance * selected_tariff.price_per_km;
    }
    
    double total_cost = base_cost;
    
    if (calc_distance > 15.0) { total_cost *= 0.90; }
    
    if (payment_idx == 1) { total_cost *= 1.05; } 
    
    if (trunk) { total_cost += 3.0; }
    
    if (wait_time > 10) {
        total_cost *= 0.90;
    }
    
    int rand_num = 1000 + rand() % 9000;
    Glib::ustring car_plate = Glib::ustring::format(rand_num, " AB-7");
    
    m_v_label_result[0].set_text(selected_tariff.car_brand);
    m_v_label_result[1].set_text(car_plate);
    m_v_label_result[2].set_text(Glib::ustring::format(wait_time, " ", m_str_min));
    m_v_label_result[3].set_text(Glib::ustring::format(trip_time, " ", m_str_min));
    
    total_cost = static_cast<int>(total_cost * 100 + 0.5) / 100.0;
    m_v_label_result[4].set_text(Glib::ustring::format(total_cost, " BYN"));
}

void main_box::update_language(locale_manager& loc) {
    m_p_loc = &loc;
    m_button_calculate.set_label(loc.get_text("btn_calc"));
    m_button_admin_panel.set_label(loc.get_text("btn_admin"));
    m_frame_output.set_label(loc.get_text("frame_order"));
    
    m_check_child.set_label(loc.get_text("check_child"));
    m_check_trunk.set_label(loc.get_text("check_trunk"));
    m_label_km.set_text(loc.get_text("lbl_km"));
    
    m_v_label_title[0].set_text(loc.get_text("lbl_brand"));
    m_v_label_title[1].set_text(loc.get_text("lbl_plate"));
    m_v_label_title[2].set_text(loc.get_text("lbl_time"));
    m_v_label_title[3].set_text(loc.get_text("lbl_duration"));
    m_v_label_title[4].set_text(loc.get_text("lbl_total"));

    int active_payment = m_combo_payment.get_active_row_number();
    m_combo_payment.remove_all();
    m_combo_payment.append(loc.get_text("pay_card"));
    m_combo_payment.append(loc.get_text("pay_cash"));
    if (active_payment != -1) { 
        m_combo_payment.set_active(active_payment); 
    } else {
        m_combo_payment.set_active(0);
    }

    m_str_min = loc.get_text("lbl_min");
    m_label_trip_time.set_text(loc.get_text("lbl_trip_time"));
    m_label_wait_time.set_text(loc.get_text("lbl_wait_time"));
}

app_window::app_window() :
    m_root_box(Gtk::Orientation::VERTICAL),
    m_color(0)
{
    set_default_size(600, 200);
    
    m_css_provider = Gtk::CssProvider::create();
    Gtk::StyleContext::add_provider_for_display(
        Gdk::Display::get_default(), m_css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    
    m_action_group = Gio::SimpleActionGroup::create();
    insert_action_group("app", m_action_group);
    
    m_action_group->add_action(
        "help", sigc::mem_fun(*this, &app_window::on_action_help)
    );
    m_action_group->add_action(
        "logout", sigc::mem_fun(*this, &app_window::on_action_logout)
    );
    m_action_group->add_action(
        "color", sigc::mem_fun(*this, &app_window::on_action_color)
    );
    
    m_action_group->add_action(
        "lang_ru", sigc::bind(
            sigc::mem_fun(*this, &app_window::on_action_lang), Glib::ustring("ru.txt")
        )
    );
    m_action_group->add_action(
        "lang_en", sigc::bind(
            sigc::mem_fun(*this, &app_window::on_action_lang), Glib::ustring("en.txt")
        )
    );
    m_action_group->add_action(
        "lang_be", sigc::bind(
            sigc::mem_fun(*this, &app_window::on_action_lang), Glib::ustring("be.txt")
        )
    );
    
    m_stack.add(m_auth_box, "auth_page");
    m_stack.add(m_main_box, "main_page");
    
    m_root_box.append(m_p_menu_bar);
    m_root_box.append(m_stack);
    
    set_child(m_root_box);
    m_stack.set_visible_child("auth_page");
    
    m_auth_box.on_signal_login().connect(
        sigc::mem_fun(*this, &app_window::on_signal_login_received)
    );
    m_auth_box.on_signal_error().connect(
        sigc::mem_fun(*this, &app_window::show_error)
    );
    m_main_box.on_signal_admin().connect(
        sigc::mem_fun(*this, &app_window::on_signal_admin_received)
    );
    
    m_main_box.on_signal_error().connect(
        sigc::mem_fun(*this, &app_window::show_error)
    );
    m_main_box.load_tariffs_to_ui();
    
    m_help_dialog.set_transient_for(*this);
    m_admin_dialog.set_transient_for(*this);
    
    m_admin_dialog.on_tariffs_changed().connect(
        sigc::mem_fun(m_main_box, &main_box::load_tariffs_to_ui)
    );

    on_action_lang("ru.txt"); 
}

void app_window::update_menu() {
    m_menu_main_model = Gio::Menu::create();
    m_menu_settings = Gio::Menu::create();
    m_menu_lang = Gio::Menu::create();
    
    m_menu_lang->append("Русский", "app.lang_ru");
    m_menu_lang->append("English", "app.lang_en");
    m_menu_lang->append("Беларуская", "app.lang_be");
    
    m_menu_settings->append_submenu(m_loc.get_text("menu_lang"), m_menu_lang);
    m_menu_settings->append(m_loc.get_text("menu_color"), "app.color");
    m_menu_settings->append(m_loc.get_text("menu_help"), "app.help");
    m_menu_settings->append(m_loc.get_text("menu_exit"), "app.logout");
    
    m_menu_main_model->append_submenu(m_loc.get_text("menu_main"), m_menu_settings);
    
    m_p_menu_bar.set_menu_model(m_menu_main_model);
}

void app_window::on_action_help() {
    m_help_dialog.show();
}

void app_window::on_signal_admin_received() {
    m_admin_dialog.load_users();
    m_admin_dialog.load_tariffs();
    m_admin_dialog.show();
}

void app_window::on_action_logout() {
    if (m_stack.get_visible_child_name() == "auth_page") {
        hide();
    } else {
        m_stack.set_visible_child("auth_page");
    }
}

void app_window::on_action_color() {
    m_color = (m_color + 1) % 3;
    
    Glib::ustring css_data;
    if (m_color == 0) {
        css_data = "";
    } else if (m_color == 1) {
        css_data = "window { background-color: #1a2b4c; }";
    } else {
        css_data = "window { background-color: #670f00; }";
    }
    
    m_css_provider->load_from_data(css_data);
}

void app_window::on_action_lang(const Glib::ustring& filename) {
    m_loc.load_language(filename);
    
    set_title(m_loc.get_text("title_app"));
    
    m_auth_box.update_language(m_loc);
    m_main_box.update_language(m_loc);
    m_help_dialog.update_language(m_loc);
    m_admin_dialog.update_language(m_loc);
    
    update_menu();
}

void app_window::on_signal_login_received(user_role role) {
    m_main_box.set_user_role(role);
    m_stack.set_visible_child("main_page");
}

void app_window::show_error(
    const Glib::ustring& title, const Glib::ustring& message
) {
    std::shared_ptr<Gtk::AlertDialog> dialog = Gtk::AlertDialog::create(title);
    dialog->set_detail(message);
    dialog->show(*this);
}