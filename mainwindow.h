#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QElapsedTimer>
#include <memory>

namespace Ui {
class MainWindow;
}

class QButtonGroup;
class QAbstractButton;

class Simulation;
class SceneWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow( QWidget * parent = nullptr );
        ~MainWindow();

    private slots:
        void on_action_Quit_triggered();
        void on_startSimulationButton_toggled( bool checked );

        void slot_scene_button_clicked( QAbstractButton * button );
        void slot_update_simulation();

    private:

        bool load( const QString& filename );
        bool save( const QString& filename ) const;

        Ui::MainWindow * m_ui;
        SceneWidget * m_scene_widget;
        QButtonGroup * m_scene_button_group;
        QTimer m_simulation_timer;
        QElapsedTimer m_simulation_timekeeper;

        std::unique_ptr< Simulation > m_simulation;
};

#endif // MAINWINDOW_H
