#include "gui/login_window.h"
#include "gui/main_window.h"
#include "core/client.h"

//Gerenciador das aplicações (janelas)
class MainApp : public Gtk::Application
{
public:
    MainApp() : Gtk::Application("com.mini_twitter_client") {}

    static Glib::RefPtr<MainApp> create()
    {
        return Glib::RefPtr<MainApp>(new MainApp());
    }

protected:
    void on_startup() override
    {
        Gtk::Application::on_startup();
    }

    void on_activate() override
    {
        auto loginWindow = new LoginWindow();
        loginWindow->set_application(Glib::RefPtr<MainApp>::cast_static(Glib::RefPtr<Gtk::Application>(this)));
        add_window(*loginWindow);
        loginWindow->show();

/*         std::unique_ptr<Client> client = std::make_unique<Client>("lulan", "127.0.0.1", 12000);
        client->connectToServer();

        auto refBuilder = Gtk::Builder::create_from_file("layout/MainWindow.glade");

        MainWindow* tempWindow = nullptr;
        refBuilder->get_widget_derived("mainWindow", tempWindow);

        if (tempWindow)
        {
            tempWindow->setClient(std::move(client));
            add_window(*tempWindow);
            tempWindow->show();
        } */
    }
};

int main(int argc, char *argv[])
{
    auto app = MainApp::create();
    app->set_flags(Gio::APPLICATION_NON_UNIQUE);
    return app->run(argc, argv);
}