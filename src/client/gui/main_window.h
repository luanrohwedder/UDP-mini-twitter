#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "../core/client.h"
#include <gtkmm.h>
#include <thread>
#include <mutex>

class ClientColumns : public Gtk::TreeModel::ColumnRecord {
public:
    ClientColumns() {
        add(id);
    }
    Gtk::TreeModelColumn<int> id;
};

class MainWindow : public Gtk::Window {
public:
    MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade);
    virtual ~MainWindow();

    void setClient(std::unique_ptr<Client>);
private:
    std::unique_ptr<Client> _client;
    Glib::RefPtr<Gtk::Builder> _refGlade;
    std::thread _listenThread;
    std::thread _clientsThread;
    std::mutex _textMutex;

    ClientColumns columns;

    Gtk::Box* _boxApp;
    Gtk::Box* _boxMain;
    Gtk::Grid* _mainGrid;
    Gtk::HeaderBar* _headerBar;
    Gtk::Button* _btnTweet;
    Gtk::TextView* _textTweet;
    Gtk::Label* _labelUsername;
    Gtk::Image* _imgLogo;
    Gtk::ScrolledWindow* _scrolledWindowText;
    Gtk::Viewport* _viewTweets;
    Gtk::ListBox* _boxTweets;
    Gtk::ScrolledWindow* _scrolledWindowMain;
    Gtk::Viewport* _viewportMain;
    Gtk::ListBox* _listboxMain;
    Gtk::ComboBox* _comboboxID;

    void initialize_widgets();
    
    void on_btnTweet_clicked();
    void on_message_received();
    void on_window_hide();

    void handleMessage(Message*);
    void handleError(std::string);
    void handleClientList(Message*);

    void addTweet(std::string, std::string);
    void addClient();
    void updateComboBox(std::unordered_map<int, std::string>);
    
    Gtk::Box* createTweetWidget(std::string, std::string);
    Gtk::Box* createChatWidget(std::string, std::string);
};

#endif // MAIN_WINDOW_H
