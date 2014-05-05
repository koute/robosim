#include "scenewidget.h"
#include "scene.h"
#include "robot.h"

#include <QPainter>
#include <QPaintEvent>

SceneWidget::SceneWidget( const std::shared_ptr< Scene > scene, QWidget * parent ) : QWidget( parent ),
    m_block_size( 32 ),
    m_scale_factor( 1.0f, 1.0f ),
    m_selected_robot( nullptr ),
    m_scene( scene ),
    m_interaction_mode( InteractionMode::None )
{
}

SceneWidget::~SceneWidget()
{
}

void SceneWidget::set_interaction_mode( InteractionMode mode )
{
    m_interaction_mode = mode;

    if( m_selected_robot != nullptr )
        repaint();
}

void SceneWidget::paintEvent( QPaintEvent * )
{
    /* TODO: Redraw only dirty regions. */

    const QRect rect( QPoint(), this->size() );
    QPainter ctx( this );
    ctx.setRenderHint( QPainter::Antialiasing );

    ctx.setBrush( QBrush(Qt::white) );
    ctx.drawRect( rect );

    const QColor border_color = QColor( 0xaa, 0xaa, 0xaa );

    ctx.setPen( QPen( border_color, 1 ) );
    QTransform view_matrix( m_scale_factor.x(), 0, 0,
                            0, m_scale_factor.y(), 0,
                            m_scale_factor.x() * m_translation.x(), m_scale_factor.y() * m_translation.y(), 1 );

    ctx.setTransform( view_matrix );

    /* Draw horizontal lines. */
    for( unsigned y = 0; y <= m_scene->height(); ++y )
    {
        const QLineF line( 0, y * m_block_size, m_scene->width() * m_block_size, y * m_block_size );
        ctx.drawLine( line );
    }

    /* Draw vertical lines. */
    for( unsigned x = 0; x <= m_scene->width(); ++x )
    {
        const QLineF line( x * m_block_size, 0, x * m_block_size, m_scene->height() * m_block_size );
        ctx.drawLine( line );
    }

    const Array2d< ObstacleType >& obstacle_map = [this]() -> const Array2d< ObstacleType >& {
        if( m_selected_robot )
            return m_selected_robot->obstacle_map();
        else
            return m_scene->obstacle_map();
    }();

    auto is_visible = [this]( unsigned x, unsigned y ) {
        return !m_selected_robot || m_selected_robot->can_see( x, y ) || m_interaction_mode == InteractionMode::SetGoal;
    };

    auto loop_through = [this, is_visible, &obstacle_map]( std::function< void (const ObstacleType, const bool, const unsigned x, const unsigned y) > callback ) {
        for( unsigned y = 0; y < m_scene->height(); ++y )
        {
            for( unsigned x = 0; x < m_scene->width(); ++x )
            {
                ObstacleType obstacle_type = obstacle_map.at( x, y );
                const bool can_see = is_visible( x, y );

                if( !can_see )
                    obstacle_type = ObstacleType::None;

                callback( obstacle_type, can_see, x, y );
            }
        }
    };

    loop_through( [this, &ctx, &border_color](const ObstacleType obstacle_type, const bool, const unsigned x, const unsigned y) {

        switch( obstacle_type )
        {
            case ObstacleType::Wall:
            {
                ctx.setPen( QPen( border_color, 1 ) );
                ctx.setBrush( QBrush( QColor( 0x88, 0xff, 0x88 ) ) );
                ctx.drawRect( QRect( x * m_block_size, y * m_block_size, m_block_size, m_block_size ) );
                break;
            }

            case ObstacleType::None:
            {
                if( m_selected_robot && !m_selected_robot->can_see( x, y ) )
                {
                    ctx.setPen( QPen( border_color ) );
                    ctx.setBrush( QBrush( QColor( 0x55, 0x55, 0x55 ) ) );
                    ctx.drawRect( QRect( x * m_block_size, y * m_block_size, m_block_size, m_block_size ) );
                }
                break;
            }

            case ObstacleType::Robot:
                break;
        }

    });

    loop_through( [this, &ctx, &border_color](const ObstacleType obstacle_type, const bool can_see, const unsigned x, const unsigned y) {
        if( obstacle_type != ObstacleType::Robot )
            return;

        const Robot * robot = m_scene->get_robot( x, y );

        float frac_x = 0.5;
        float frac_y = 0.5;

        if( can_see && robot != nullptr )
        {
            frac_x = robot->frac_x();
            frac_y = robot->frac_y();
        }

        ctx.setPen( QPen( border_color, 1 ) );
        ctx.setBrush( QBrush( QColor(0x62, 0xa2, 0xf3) ) );
        ctx.drawEllipse( QPoint( (x + frac_x) * m_block_size, (y + frac_y) * m_block_size ), m_block_size / 3, m_block_size / 3 );
        if( m_selected_robot == robot )
            ctx.setPen( QPen( Qt::yellow ) );
        else
            ctx.setPen( QPen( Qt::white ) );

        if( can_see && robot != nullptr )
        {
            QString id = QString::number( robot->id() );
            ctx.drawText( (x + frac_x - 0.5) * m_block_size, (y + frac_y - 0.5) * m_block_size, m_block_size, m_block_size, Qt::AlignCenter | Qt::AlignVCenter, id );
        }

    });

    /* TODO: If two or more goals are set to the same title draw multiple ids. */
    for( const Robot& robot: m_scene->robot_list() )
    {
        if( !robot.has_goal() )
            continue;

        unsigned goal_x = robot.goal_x();
        unsigned goal_y = robot.goal_y();

        ctx.setPen( QPen( Qt::red ) );
        ctx.setBrush( Qt::NoBrush );
        ctx.drawRect( QRect( goal_x * m_block_size + 1, goal_y * m_block_size + 1, m_block_size - 2, m_block_size - 2 ) );

        if( m_selected_robot && !m_selected_robot->can_see( goal_x, goal_y ) )
            ctx.setPen( QPen( Qt::white ) );
        else
            ctx.setPen( QPen( Qt::black ) );
        ctx.drawText( goal_x * m_block_size + 2, goal_y * m_block_size, m_block_size / 2, m_block_size / 2, 0, QString::number( robot.id() ) );
    }

}

float SceneWidget::to_world_space( float x, float s, float d )
{
    return (x - s * d) / s;
}

QPointF SceneWidget::to_world_space( const QPoint& point ) const
{
    const float x = to_world_space( point.x(), m_scale_factor.x(), m_translation.x() );
    const float y = to_world_space( point.y(), m_scale_factor.y(), m_translation.y() );
    return QPointF( x, y );
}

void SceneWidget::mousePressEvent( QMouseEvent * event )
{
    if( event->buttons() == Qt::MiddleButton )
        m_last_mouse_position = event->pos();

    handle_mouse_interaction( event->pos(), event->button(), false );
}

void SceneWidget::handle_mouse_interaction( QPoint mouse_position, Qt::MouseButtons buttons, bool is_mouse_move )
{
    QPointF position = to_world_space( mouse_position );
    if( position.x() < 0 || position.y() < 0 )
        return;

    unsigned x = position.x() / m_block_size;
    unsigned y = position.y() / m_block_size;

    if( x >= m_scene->width() || y >= m_scene->height() )
        return;

    Robot * robot = m_scene->get_robot( x, y );

    bool action;
    if( buttons == Qt::LeftButton )
        action = 1; /* Place with the left mouse button. */
    else if( buttons == Qt::RightButton )
        action = 0; /* Remove with the right mouse button. */
    else
        return;

    if( m_interaction_mode == InteractionMode::None ||
        (m_interaction_mode == InteractionMode::SetGoal && robot != nullptr ) )
    {
        if( is_mouse_move || buttons != Qt::LeftButton || robot == nullptr )
            return;

        if( m_selected_robot == robot )
            m_selected_robot = nullptr;
        else
            m_selected_robot = robot;
    }
    else if( m_interaction_mode == InteractionMode::ModifyWalls )
    {
        if( m_scene->is_blocked( x, y ) == action )
            return;

        m_scene->set_wall( x, y, action );
    }
    else if( m_interaction_mode == InteractionMode::SetRobot && !is_mouse_move )
    {
        if( action )
        {
            m_scene->add_robot( x, y );
            /*
                m_selected_robot = ...
            */
        }
        else
        {
            if( robot != nullptr )
            {
                m_scene->remove_robot( *robot );

                if( robot == m_selected_robot )
                    m_selected_robot = nullptr;
            }
        }
    }
    else if( m_interaction_mode == InteractionMode::SetGoal && !is_mouse_move && !robot )
    {
        if( m_selected_robot == nullptr )
            return;

        m_selected_robot->set_goal( x, y );

    }
    else
        return;

    if( m_selected_robot != nullptr )
        m_selected_robot->calculate_visibility();

    /* TODO: Only modified cell should be repainted. */
    repaint();
}

void SceneWidget::mouseMoveEvent( QMouseEvent * event )
{
    if( event->buttons() == Qt::MiddleButton )
    {
        const QPoint difference = (event->pos() - m_last_mouse_position);

        m_translation += QPointF(difference.x() / m_scale_factor.x(), difference.y() / m_scale_factor.y());
        m_last_mouse_position = event->pos();
        repaint();
    }

    handle_mouse_interaction( event->pos(), event->buttons(), true );
}

void SceneWidget::wheelEvent( QWheelEvent * event )
{
    /*
     * (s - scaling coefficient, d - offset)
     *
     * World space to screen space:
     *     t(x) = sx + sd
     *
     * Screen space to world space:
     *            t - sd
     *     x(t) = ------
     *               s
     *
     * To scale around a point P we need to ensure that
     * it doesn't move after scaling, that is:
     *     t_{n}(P) = t_{n+1}(P)
     *
     * To make that possible we can introduce an offset 'b' so that:
     *     sP + sd = asP + as(d + b)
     *
     * Which in the end gives us:
     *     b = (P + d) / a - (P + d)
    */

    const float factor = 1.0 + event->delta() / 1500.0f;

    const QPointF mouse_position = to_world_space( event->pos() );

    const auto calc = []( float p, float d, float a ) { return (p + d) / a - (p + d); };
    const float bx = calc( mouse_position.x(), m_translation.x(), factor );
    const float by = calc( mouse_position.y(), m_translation.y(), factor );

    m_scale_factor *= factor;
    m_translation += QPointF( bx, by );

    repaint();
}
