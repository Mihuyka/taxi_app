#include "header.hpp"

int main(int argc, char* argv[]) {
    std::shared_ptr<Gtk::Application> app = Gtk::Application::create("by.mikhaileutserau.taxiapp");
    
    return app->make_window_and_run<app_window>(argc, argv);
}