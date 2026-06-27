#ifndef TAXI_APP_HPP
#define TAXI_APP_HPP

#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <gtkmm/textview.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/listbox.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/frame.h>
#include <gtkmm/grid.h>
#include <gtkmm/stack.h>
#include <gtkmm/popovermenubar.h>
#include <gtkmm/alertdialog.h>
#include <gtkmm/cssprovider.h>
#include <giomm/menu.h>
#include <giomm/simpleactiongroup.h>
#include <glibmm/ustring.h>
#include <sigc++/sigc++.h>

#include <fstream>
#include <cstring>
#include <vector>
#include <map>

class locale_manager {
    public:
        void load_language(const Glib::ustring& filename);
        Glib::ustring get_text(const Glib::ustring& key);
        
    private:
        std::map<Glib::ustring, Glib::ustring> m_dict;
};

enum class user_role : short int {
    admin = 0,
    user = 1
};

struct tariff_record {
    char name[32];
    char car_brand[32];
    double price_per_km;
    bool has_child_seat;
    double child_price_per_km;
};

class tariff_manager {
    public:
        void save_tariff(const tariff_record& record);
        void delete_tariff(const Glib::ustring& name);
        std::vector<tariff_record> get_all_tariffs();
};

class auth_manager {
    public:
        bool register_user(
            const Glib::ustring& name, const Glib::ustring& password, user_role role
        );
        bool login(
            const Glib::ustring& name, const Glib::ustring& password, user_role& role
        );
        
        bool delete_user(const Glib::ustring& name);
        
    private:
        bool user_exists(const Glib::ustring& name);
        
        std::size_t hash_fun(const Glib::ustring& str);
};

class base_dialog : public Gtk::Window {
    public:
        base_dialog(const Glib::ustring& title);
        ~base_dialog() override = default;
        
    protected:
        Gtk::Box m_main_box; 
};

class help_dialog : public base_dialog {
    public:
        help_dialog();
        ~help_dialog() override = default;
        
        void update_language(locale_manager& loc);
        
    private:
        Gtk::ScrolledWindow m_scroll_window;
        Gtk::TextView m_textv_info;
        std::shared_ptr<Gtk::TextBuffer> m_text_buffer;
};

class admin_dialog : public base_dialog {
    public:
        admin_dialog();
        ~admin_dialog() override = default;
        
        void load_users();
        void load_tariffs();
        void update_language(locale_manager& loc);
        
        sigc::signal<void()> on_tariffs_changed();
        
    private:
        locale_manager* m_p_loc;
        
        sigc::signal<void()> m_signal_tariffs_changed;
        
        Gtk::Box m_box_split; 
        
        Gtk::Box m_box_users;
        Gtk::ScrolledWindow m_scroll_window_users;
        Gtk::ListBox m_listbox_users;
        Gtk::Button m_button_delete_user;
        
        Gtk::Box m_box_tariffs;
        Gtk::ScrolledWindow m_scroll_window_tariffs;
        Gtk::ListBox m_listbox_tariffs;
        Gtk::Entry m_entry_tariff_name;
        Gtk::Entry m_entry_car_brand;
        
        Gtk::Label m_label_price;
        Gtk::SpinButton m_spin_price;
        
        Gtk::CheckButton m_check_has_child;
        
        Gtk::Label m_label_child_price;
        Gtk::SpinButton m_spin_child_price;
        
        Gtk::Button m_button_add_tariff;
        Gtk::Button m_button_delete_tariff;
        
        auth_manager m_auth_manager;
        tariff_manager m_tariff_manager;
        
        void on_button_delete_user_clicked();
        void on_button_add_tariff_clicked();
        void on_button_delete_tariff_clicked();
};

struct user_record {
    char name[32];
    std::size_t password_hash;
    user_role role;
};

class auth_box : public Gtk::Box {
    public:
        auth_box();
        ~auth_box() override = default;
        
        sigc::signal<void(user_role)> on_signal_login();
        
        sigc::signal<void(const Glib::ustring&, const Glib::ustring&)> on_signal_error();
        
        void update_language(locale_manager& loc);
        
    private:
        locale_manager* m_p_loc;
        
        Gtk::Entry m_entry_name;
        Gtk::Entry m_entry_password;
        Gtk::Button m_button_login;
        Gtk::Button m_button_register;
        Gtk::Label m_label_status;
        
        sigc::signal<void(user_role)> m_signal_login;
        
        sigc::signal<void(const Glib::ustring&, const Glib::ustring&)> m_signal_error;
        
        auth_manager m_auth_manager;
        
        void on_button_login_clicked();
        
        void on_button_register_clicked();
};

class main_box : public Gtk::Box {
    public:
        main_box();
        ~main_box() override = default;
        
        void set_user_role(user_role index);
        
        sigc::signal<void()> on_signal_admin();
        
        void update_language(locale_manager& loc);
        
        void load_tariffs_to_ui();
        
        sigc::signal<void(const Glib::ustring&, const Glib::ustring&)> on_signal_error();
        
    private:
        locale_manager* m_p_loc;
        
        Gtk::Box m_box_input;
        Gtk::ComboBoxText m_combo_tariff;
        Gtk::ComboBoxText m_combo_payment;
        Gtk::Label m_label_km;
        Gtk::SpinButton m_spin_distance;
        Gtk::CheckButton m_check_child;
        Gtk::CheckButton m_check_trunk;
        Gtk::Button m_button_calculate;
        
        Gtk::Button m_button_admin_panel;
        
        Gtk::Frame m_frame_output;
        Gtk::Box m_box_output;
        Gtk::Label m_label_title;
        Gtk::Grid m_grid;
        std::vector<Gtk::Label> m_v_label_title;
        std::vector<Gtk::Label> m_v_label_result;
        Gtk::Label m_label_result;
        
        Gtk::Label m_label_trip_time;
        Gtk::SpinButton m_spin_trip_time;
        Gtk::Label m_label_wait_time;
        Gtk::SpinButton m_spin_wait_time;
        
        sigc::signal<void()> m_signal_admin;
        
        Glib::ustring m_str_min = "мин";
        
        void on_button_calculate_clicked();
        void on_button_admin_panel_clicked();
        
        sigc::signal<void(const Glib::ustring&, const Glib::ustring&)> m_signal_error;
        
        tariff_manager m_tariff_manager;
        std::vector<tariff_record> m_loaded_tariffs;
};

class app_window : public Gtk::Window {
     public:
         app_window();
         ~app_window() override = default;
         
         void show_error(
             const Glib::ustring& title, const Glib::ustring& message
         );
         
     private:
         Gtk::Box m_root_box;
         Gtk::Stack m_stack;
         auth_box m_auth_box;
         main_box m_main_box;
         
         help_dialog m_help_dialog;
         admin_dialog m_admin_dialog;
         
         locale_manager m_loc;
         std::shared_ptr<Gtk::CssProvider> m_css_provider;
         short int m_color;
         
         Gtk::PopoverMenuBar m_p_menu_bar;
         std::shared_ptr<Gio::Menu> m_menu_main_model;
         std::shared_ptr<Gio::Menu> m_menu_settings;
         std::shared_ptr<Gio::Menu> m_menu_lang;
         std::shared_ptr<Gio::SimpleActionGroup> m_action_group;
         
         void on_signal_login_received(user_role role);
         void on_signal_admin_received();
         
         void on_action_help();
         void on_action_logout();
         void on_action_color();
         void on_action_lang(const Glib::ustring& filename);
         
         void update_menu();
};

#endif