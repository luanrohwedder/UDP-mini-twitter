#ifndef LOGIN_WINDOW_H
#define LOGIN_WINDOW_H

#include <gtkmm.h>

class LoginWindow : public Gtk::Window
{
public:
    LoginWindow();
    virtual ~LoginWindow() {};

    inline void set_application(Glib::RefPtr<Gtk::Application> app)
    {
        _app = app;
    }

private:
    Glib::RefPtr<Gtk::Application> _app;

    Gtk::Fixed _fixed;
    Gtk::Image _imageLogo, _imageText;
    Gtk::VBox _Vbox;
    Gtk::Entry _EntryUsername, _EntryIP, _EntryPort;
    Gtk::Button _ButtonLogin;

    void set_hierarchy();
    void draw_widgets();

    void on_button_login_clicked();
};

#endif