#pragma once

#include <QWidget>
#include <QPushButton>


namespace traffic_lights {
namespace qt {

class MyWindow : public QWidget {
    Q_OBJECT
public:
    explicit MyWindow(QWidget* parent = 0);
private:
    QPushButton* m_button;
};

} // namespace model
} // namespace traffic_lights

