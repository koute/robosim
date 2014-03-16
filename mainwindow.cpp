#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "scenewidget.h"
#include "routingalgorithmregistry.h"
#include "simulation.h"
#include "scene.h"
#include "robot.h"
#include "routingalgorithm.h"

#include <QButtonGroup>
#include <QDir>

static QString get_user_path()
{
    QString qpath = QDir::homePath();
    QDir dir;

    #ifdef Q_WS_WIN
        qpath.append( "\\Application Data\\robosim\\" );
    #else
        qpath.append( "/.local/" );
        if( dir.exists( qpath ) == false )
            dir.mkdir( qpath );
        qpath.append( "robosim/" );
    #endif

    if( dir.exists( qpath ) == false )
        dir.mkdir( qpath );

    return qpath;
}

MainWindow::MainWindow( QWidget *parent ) :
    QMainWindow( parent ),
    m_ui( new Ui::MainWindow ),
    m_simulation( new Simulation( std::make_shared< Scene >( 32, 32 ) ) )
{
    m_ui->setupUi( this );

    m_scene_button_group = new QButtonGroup( this );
    m_scene_button_group->setExclusive( false );
    m_scene_button_group->addButton( m_ui->addWallsModeButton );
    m_scene_button_group->addButton( m_ui->addRobotsModeButton );
    m_scene_button_group->addButton( m_ui->setGoalModeButton );

    QObject::connect( m_scene_button_group,
                      SIGNAL(buttonClicked(QAbstractButton*)),
                      SLOT(slot_scene_button_clicked(QAbstractButton*)) );

    QObject::connect( &m_simulation_timer, SIGNAL(timeout()), SLOT(slot_update_simulation()) );
    m_simulation_timer.setInterval( 10 );

    m_scene_widget = new SceneWidget( m_simulation->scene() );
    m_ui->sceneScrollArea->setWidget( m_scene_widget );

    for( auto& pair: RoutingAlgorithmRegistry::instance().algorithm_map() )
    {
        auto& name = pair.first;
        m_ui->algorithmComboBox->addItem( QString::fromStdString(name) );
    }

    load( get_user_path() + "autosave.dat" );
}

MainWindow::~MainWindow()
{
    save( get_user_path() + "autosave.dat" );
    delete m_ui;
}

bool MainWindow::load( const QString& filename )
{
    QFile fp( filename );
    if( fp.open( QIODevice::ReadOnly ) )
    {
        QDataStream stream( &fp );
        m_simulation->scene()->deserialize( stream );

        fp.close();
        return true;
    }

    return false;
}

bool MainWindow::save( const QString& filename ) const
{
    QFile fp( filename );
    if( fp.open( QIODevice::WriteOnly ) )
    {
        QDataStream stream( &fp );
        m_simulation->scene()->serialize( stream );

        fp.close();
        return true;
    }

    return false;
}

void MainWindow::slot_update_simulation()
{
    float elapsed = double( m_simulation_timekeeper.nsecsElapsed() ) / double( 1000000000.0 );
    m_simulation_timekeeper.restart();
    m_simulation->run( elapsed );

    /* TODO: Only dirty regions should be repainted. */
    m_scene_widget->repaint();
}

void MainWindow::on_action_Quit_triggered()
{
    this->close();
}

void MainWindow::on_startSimulationButton_toggled( bool checked )
{
    if( checked )
    {
        auto& algorithm_map = RoutingAlgorithmRegistry::instance().algorithm_map();
        auto i = algorithm_map.find( m_ui->algorithmComboBox->currentText().toStdString() );

        assert( i != algorithm_map.end() );
        if( i != algorithm_map.end() )
        {
            auto& factory_method = i->second;
            for( Robot& robot: m_simulation->scene()->robot_list() )
            {
                /*
                    if( robot.routing_algorithm() != nullptr )
                        continue;
                */

                auto algorithm = factory_method();
                assert( algorithm.get() != nullptr );

                robot.set_routing_algorithm( std::move( algorithm ) );
            }
        }
    }

    m_ui->startSimulationButton->setText( checked ? "Running..." : "Start" );
    m_simulation_timekeeper.start();

    if( checked )
        m_simulation_timer.start();
    else
        m_simulation_timer.stop();
}

void MainWindow::slot_scene_button_clicked( QAbstractButton * pressed_button )
{
    /*
     * Unfortunately QButtonGroup::sexExclusive forces at least one button to be
     * checked at any time; since we want to let the user have no buttons checked
     * we need to do it manually.
     */

    for( QAbstractButton * button: m_scene_button_group->buttons() )
        button->setChecked( button == pressed_button && button->isChecked() );

    if( m_ui->addWallsModeButton->isChecked() )
        m_scene_widget->set_interaction_mode( SceneWidget::InteractionMode::ModifyWalls );
    else if( m_ui->addRobotsModeButton->isChecked() )
        m_scene_widget->set_interaction_mode( SceneWidget::InteractionMode::SetRobot );
    else if( m_ui->setGoalModeButton->isChecked() )
        m_scene_widget->set_interaction_mode( SceneWidget::InteractionMode::SetGoal );
    else
        m_scene_widget->set_interaction_mode( SceneWidget::InteractionMode::None );
}
