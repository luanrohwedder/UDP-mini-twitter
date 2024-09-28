#include "login_window.h"
#include "main_window.h"
#include "../core/client.h"


LoginWindow::LoginWindow() 
    : _Vbox(Gtk::ORIENTATION_VERTICAL), _ButtonLogin("Login")
{
    Gtk::Settings::get_default()->property_gtk_application_prefer_dark_theme() = true;

    set_hierarchy();
    draw_widgets();
    show_all_children();
}

void LoginWindow::set_hierarchy()
{
    //Window
    add(_fixed);

    //Fixed
    _fixed.add(_imageLogo);
    _fixed.add(_imageText);
    _fixed.add(_Vbox);

    //Box
    _Vbox.pack_start(_EntryUsername, Gtk::PACK_SHRINK);
    _Vbox.pack_start(_EntryIP, Gtk::PACK_SHRINK);
    _Vbox.pack_start(_EntryPort, Gtk::PACK_SHRINK);
    _Vbox.pack_start(_ButtonLogin, Gtk::PACK_SHRINK);
}

void LoginWindow::draw_widgets()
{
    //Window
    set_title("Login");
    set_border_width(10);
    set_default_size(500, 500);
    set_visible(true);
    set_can_focus(false);
    set_resizable(false);

    //Background
    

    //Fixed
    int x_position = (500 - 350) / 2;
    int x_pos_logo = (500 - 128) / 2;
    _fixed.set_size_request(300, 400);
    _fixed.set_visible(true);
    _fixed.set_can_focus(false);
    _fixed.move(_Vbox, x_position, 285);
    _fixed.move(_imageLogo, x_pos_logo, 15);
    _fixed.move(_imageText, x_position, 158);

    //Image
    _imageLogo.set("assets/twitter_big.png");
    _imageLogo.set_visible(true);
    _imageText.set("assets/Twitter_logo.png");
    _imageText.set_visible(true);

    //Box
    _Vbox.set_spacing(1);
    _Vbox.set_halign(Gtk::ALIGN_CENTER);
    _Vbox.set_valign(Gtk::ALIGN_CENTER);

    //Entry Username
    _EntryUsername.set_can_focus(true);
    _EntryUsername.set_placeholder_text("Username");
    _EntryUsername.set_input_purpose(Gtk::INPUT_PURPOSE_ALPHA);
    _EntryUsername.set_size_request(350, -1);

    //Entry IP
    _EntryIP.set_can_focus(true);
    _EntryIP.set_placeholder_text("IP Address");
    _EntryIP.set_input_purpose(Gtk::INPUT_PURPOSE_NUMBER);
    _EntryUsername.set_size_request(350, -1);

    //Entry Port
    _EntryPort.set_can_focus(true);
    _EntryPort.set_placeholder_text("Port");
    _EntryPort.set_input_purpose(Gtk::INPUT_PURPOSE_NUMBER);
    _EntryUsername.set_size_request(350, -1);

    //Buton
    _ButtonLogin.set_can_focus(true);
    _ButtonLogin.set_focus_on_click(true);
    _ButtonLogin.set_border_width(5);
    _ButtonLogin.set_label("Login");
    _ButtonLogin.signal_clicked().connect(sigc::mem_fun(*this, &LoginWindow::on_button_login_clicked));
}

void LoginWindow::on_button_login_clicked()
{
    std::string username = _EntryUsername.get_text();
    std::string ip = _EntryIP.get_text();
    int port = std::stoi(_EntryPort.get_text());

    std::unique_ptr<Client> client = std::make_unique<Client>(username, ip, port);
    
    if (!client->connectToServer())
    {
        std::string msg = std::string("Não foi possível conectar ao servidor. ") +
                  std::string("Verifique se o endereço e a porta estão corretos e ") +
                  std::string("se o servidor está ativo.");

        Gtk::MessageDialog dialog(*this, msg, false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK, true);
        dialog.set_title("Aviso");
        dialog.run();
        dialog.close();
        return;
    }

    auto refBuilder = Gtk::Builder::create_from_file("layout/MainWindow.glade");

    MainWindow* mainWindow = nullptr;
    refBuilder->get_widget_derived("mainWindow", mainWindow);

    if (mainWindow)
    {
        mainWindow->setClient(std::move(client));
        _app->add_window(*mainWindow);
        mainWindow->show();
    }

    hide();
    _app->remove_window(*this);
}