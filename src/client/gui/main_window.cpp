#include "main_window.h"
#include "message.h"
#include <iostream>

MainWindow::MainWindow(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &refGlade)
    : Gtk::Window(cobject), _refGlade(refGlade)
{
    initialize_widgets();
    show_all_children();
}

MainWindow::~MainWindow()
{
    if (_listenThread.joinable())
    {
        _listenThread.join();
    }
}

void MainWindow::setClient(std::unique_ptr<Client> client)
{
    _client = std::move(client);
    _labelUsername->set_label(_client->getUsername() + "#" + std::to_string(_client->getId()));
    
    _listenThread = std::thread([this]() { 
        on_message_received(); 
    });
}

void MainWindow::initialize_widgets()
{
    _refGlade->get_widget("boxApp", _boxApp);
    _refGlade->get_widget("stackSidebar", _stackSidebar);
    _refGlade->get_widget("stack", _stack);
    _refGlade->get_widget("boxMain", _boxMain);
    _refGlade->get_widget("mainGrid", _mainGrid);
    _refGlade->get_widget("boxClients", _boxClients);
    _refGlade->get_widget("headerBar", _headerBar);
    _refGlade->get_widget("btnTweet", _btnTweet);
    _refGlade->get_widget("textTweet", _textTweet);
    _refGlade->get_widget("labelUsername", _labelUsername);
    _refGlade->get_widget("imgLogo", _imgLogo);
    _refGlade->get_widget("boxListClients", _boxListClients);
    _refGlade->get_widget("stackClients", _stackClients);
    _refGlade->get_widget("stackClientsSidebar", _stackClientsSidebar);
    _refGlade->get_widget("scrolledWindowText", _scrolledWindowText);
    _refGlade->get_widget("viewTweets", _viewTweets);
    _refGlade->get_widget("boxTweets", _boxTweets);

    _btnTweet->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_btnTweet_clicked));
    _stackSidebar->set_stack(*_stack);
    _stack->connect_property_changed("visible-child", sigc::mem_fun(*this, &MainWindow::on_stack_page_changed));
    _imgLogo->set("assets/twitter_small.png");

    signal_hide().connect(sigc::mem_fun(*this, &MainWindow::on_window_hide));
    

    if (!_boxApp || !_stackSidebar || !_stack || !_boxMain || !_mainGrid || 
        !_boxClients || !_clientGrid ||!_btnTweet || !_textTweet || 
        !_labelUsername || !_imgLogo || !_scrolledWindowText)
    {
        g_warning("Failed to load one or more widgets from the Glade file.");
    }
}

void MainWindow::on_btnTweet_clicked()
{
    Glib::RefPtr<Gtk::TextBuffer> buffer = _textTweet->get_buffer();
    std::string tweetText = buffer->get_text();

    if (tweetText.length() > 140)
    {
        handleError("O tweet não pode ter mais de 140 caracteres.");
        return;
    }

    if (!tweetText.empty())
    {
        _client->sendMessage(tweetText);
        buffer->set_text("");
    } 
}

void MainWindow::on_message_received()
{
    while (_client->getRunning())
    {
        Message* msg = _client->receiveMessages();

        if (msg)
        {
            if (msg->getType() == Message::MSG)
                handleMessage(msg);

            if (msg->getType() == Message::ERRO)
                handleError(msg->getText());

            if (msg->getType() == Message::LIST)
                ;//handleList

            delete msg;
        }
        else
        {
            
        }
    }
}

void MainWindow::on_stack_page_changed()
{
    if (_stack->get_visible_child_name() == "boxClients")
    {
        _client->receiveClientsOnline();

        for (const auto& client : _client->getClientOnline())
        {
            auto box = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL, 2);
            auto label = Gtk::make_managed<Gtk::Label>(client.second.username);

            box->pack_start(*label, true, true, 0);

            _stackClients->add(*box, client.second.username, client.second.username);
        }
    }
}

void MainWindow::on_window_hide()
{
    _client->sendDisconnect();
    std::cout << "Disconnect" << std::endl;
}

void MainWindow::handleMessage(Message* message)
{
    if (message->getOriginID() != _client->getId() && 
        message->getText().rfind("STATUS: ", 0) != 0)
    {
        addTweet(message->getUsername() + "#" + std::to_string(message->getOriginID()), message->getText());
    }
    else if (message->getOriginID() == _client->getId() && 
        message->getText().rfind("STATUS: ", 0) != 0)
    {
        addTweet("Eu#" + std::to_string(message->getOriginID()), message->getText());
    }
    else if (message->getDestinationID() == _client->getId() &&
        message->getText().rfind("STATUS: ", 0) == 0)
    {
        std::cout << message->getText() << std::endl;
    }

}

void MainWindow::handleError(std::string message)
{
    Gtk::MessageDialog dialog(*this, message, false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK, true);
    dialog.set_title("Aviso");
    dialog.run();
    dialog.close();
}

void MainWindow::addTweet(std::string username, std::string tweetText)
{
    Glib::signal_idle().connect_once([this, username, tweetText]() {
        std::lock_guard<std::mutex> lock(_textMutex);
        Gtk::Box* pBox = createTweetWidget(username, tweetText);
        _boxTweets->prepend(*pBox);
        show_all_children();
    });
}

Gtk::Box* MainWindow::createTweetWidget(std::string label, std::string text)
{
    Gtk::Box* pBox = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_VERTICAL);
    pBox->set_margin_start(5);
    pBox->set_margin_end(5);

    Gtk::Label* pLabel = Gtk::make_managed<Gtk::Label>(label);
    Pango::FontDescription font_desc;
    font_desc.set_family("Monospace");
    font_desc.set_weight(Pango::WEIGHT_BOLD);
    font_desc.set_size(14 * PANGO_SCALE);
    pLabel->override_font(font_desc);
    pLabel->set_margin_start(5);
    pLabel->set_halign(Gtk::ALIGN_START);

    Gtk::TextView* pView = Gtk::make_managed<Gtk::TextView>();
    pView->set_name("textview");
    pView->get_buffer()->set_text(text);
    pView->set_editable(false);
    pView->set_accepts_tab(false);
    pView->set_wrap_mode(Gtk::WRAP_WORD_CHAR);
    pView->set_margin_top(10);
    pView->set_margin_bottom(0);
    pView->set_margin_start(10);
    pView->set_margin_end(10);
    pView->set_size_request(-1, 50);
    pView->set_cursor_visible(false);

    auto cssProvider = Gtk::CssProvider::create();
    cssProvider->load_from_data(
        "textview text { background-color: transparent; }"
    );
    auto styleContext = pView->get_style_context();
    styleContext->add_provider(cssProvider, GTK_STYLE_PROVIDER_PRIORITY_USER);

    pBox->pack_start(*pLabel, Gtk::PACK_SHRINK);
    pBox->pack_start(*pView, Gtk::PACK_EXPAND_WIDGET);

    return pBox;
}
