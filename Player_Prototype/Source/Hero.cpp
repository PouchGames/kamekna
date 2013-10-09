#include "Hero.h"

const float SCALE = 30.f; // Box2D works in a scale of 30 pixels = 1 meter
extern int numFootContacts;

Hero::Hero(b2World& world): Entity(world, Entity::Type::Hero)
{
    // Player
    const int PLAYER_WIDTH = 32;
    const int PLAYER_HEIGHT = 64;

    setOrigin(sf::Vector2f(PLAYER_WIDTH/2,PLAYER_HEIGHT/2));
    m_shape.setSize(sf::Vector2f(PLAYER_WIDTH,PLAYER_HEIGHT));
    m_shape.setFillColor(sf::Color::Red);
}

void Hero::handleEvent(const sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        // Jumping
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
        {
            if (numFootContacts >= 1)
                m_body->ApplyLinearImpulse(b2Vec2(0,-m_body->GetMass()*8), m_body->GetWorldCenter());
        }
    }
}

void Hero::handleRealTimeInput()
{
    // Moving left/right
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
    {
        if (numFootContacts >= 1 && m_body->GetLinearVelocity().x * SCALE > -150)
            m_body->ApplyForce(b2Vec2(-m_body->GetMass()*8,0), m_body->GetWorldCenter());
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    {
        if (numFootContacts >= 1 && m_body->GetLinearVelocity().x * SCALE < 150)
            m_body->ApplyForce(b2Vec2(m_body->GetMass()*8,0), m_body->GetWorldCenter());
    }

    // Thrusters
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
    {
        m_body->ApplyForce(b2Vec2(0, m_body->GetMass()*25), m_body->GetWorldCenter());
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
    {
        m_body->ApplyForce(b2Vec2(0, -m_body->GetMass()*25), m_body->GetWorldCenter());
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    {
        m_body->ApplyForce(b2Vec2(m_body->GetMass()*10, 0), m_body->GetWorldCenter());
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    {
        m_body->ApplyForce(b2Vec2(-m_body->GetMass()*10, 0), m_body->GetWorldCenter());
    }
}

void Hero::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_shape, states);
}

