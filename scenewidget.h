#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include <QWidget>
#include <memory>

class Scene;
class Robot;

class SceneWidget : public QWidget
{
    Q_OBJECT

    unsigned m_block_size;

    QPoint m_last_mouse_position;
    QPointF m_scale_factor;
    QPointF m_translation;

    Robot * m_selected_robot;

    std::shared_ptr< Scene > m_scene;

    public:

        enum class InteractionMode
        {
            None,
            ModifyWalls,
            SetRobot,
            SetGoal
        };

        explicit SceneWidget( const std::shared_ptr< Scene > scene, QWidget * parent = nullptr );
        ~SceneWidget();

        void set_interaction_mode( InteractionMode mode );

    protected:

        virtual void paintEvent( QPaintEvent * event ) override;
        virtual void mousePressEvent( QMouseEvent * event ) override;
        virtual void mouseMoveEvent( QMouseEvent * event ) override;
        virtual void wheelEvent( QWheelEvent * event ) override;

        static float to_world_space( float x, float s, float d );
        QPointF to_world_space( const QPoint& point ) const;
        void handle_mouse_interaction( QPoint mouse_position, Qt::MouseButtons buttons, bool is_mouse_move );

    private:

        InteractionMode m_interaction_mode;
};

#endif // SCENEWIDGET_H
