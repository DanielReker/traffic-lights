#include "MyWindow.hpp"

#include <iostream>

using namespace traffic_lights::qt;


MyWindow::MyWindow(QWidget* parent) : QWidget(parent) {

    // Set size of the window
    setFixedSize(800, 600);

    // Create and position the button
    m_button = new QPushButton("Hello World", this);
    m_button->setGeometry(10, 10, 80, 30);
    m_button->move(0, 0);


    std::cout << "Executing my window! \n";
}