#include "World.h"

const float SCALE = 30.f; // Box2D works in a scale of 30 pixels = 1 meter

World::World(sf::RenderWindow& window):
    m_window(window),
    m_worldView(window.getDefaultView()),
    m_minimapView(window.getDefaultView()),
    m_physicWorld(b2Vec2(0, 15.0f)),
    m_worldBounds(
                  -m_worldView.getSize().x,
                  -m_worldView.getSize().y,
                  2*m_worldView.getSize().x,
                  2*m_worldView.getSize().y
                  ),
    m_player(nullptr),
    m_spawnPosition(200.f,200.f)
{
    loadTextures();

    m_minimapView.zoom(4.0f);
    m_minimapView.setViewport(sf::FloatRect(0.85f, 0.f, 0.15f, 0.15f));
    m_minimapView.setCenter(m_spawnPosition);
    m_worldView.setCenter(m_spawnPosition);

    m_physicWorld.SetContactListener(&m_contactListener);

    buildScene();
}

void World::loadTextures()
{
    m_textures.load(Textures::SpaceBackground, "background.png");
    m_textures.load(Textures::Hero, "astronaut.png");
    m_textures.load(Textures::Asteroid, "asteroid.png");

    // to be improved
    m_fonts.load(Fonts::Main, "arial.ttf");


}

void World::buildScene()
{
    // Init layers
    for(std::size_t i = 0; i < LayerCount; ++i)
    {
        SceneNode::Ptr layer(new SceneNode());
        m_sceneLayers[i] = layer.get();
        m_sceneGraph.attachChild(std::move(layer));
    }

    // Background
    sf::Texture& texture = m_textures.get(Textures::SpaceBackground);
    sf::IntRect textureRect(m_worldBounds);
    texture.setRepeated(true);
    std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(texture, textureRect));
    backgroundSprite->setPosition(m_worldBounds.left, m_worldBounds.top);
    m_sceneLayers[Background]->attachChild(std::move(backgroundSprite));

    // Plateform
    std::unique_ptr<Platform> platform(new Platform(m_physicWorld));
    platform->setPosition(100.f, 500.f);
    m_sceneLayers[Space]->attachChild(std::move(platform));

    // Asteroid_1
    std::unique_ptr<Asteroid> asteroid_1(new Asteroid(m_textures, m_physicWorld));
    asteroid_1->setPosition(-200.f, -100.f);
    m_sceneLayers[Space]->attachChild(std::move(asteroid_1));

    // Asteroid_2
    std::unique_ptr<Asteroid> asteroid_2(new Asteroid(m_textures, m_physicWorld));
    asteroid_2->setPosition(800.f, 100.f);
    m_sceneLayers[Space]->attachChild(std::move(asteroid_2));

    // Planet
    std::unique_ptr<Planet> planet(new Planet(m_textures, m_physicWorld));
    planet->setPosition(-200.f, -450.f);
    m_sceneLayers[Space]->attachChild(std::move(planet));

    // Hero
    std::unique_ptr<Being> hero(new Being(Being::Hero, m_textures, m_fonts, m_physicWorld));
    m_player = hero.get();
    hero->setPosition(m_spawnPosition);
    m_sceneLayers[Space]->attachChild(std::move(hero));
}

void World::update(sf::Time dt)
{
    while(!m_commandQueue.isEmpty())
        m_sceneGraph.onCommand(m_commandQueue.pop(), dt);

    m_physicWorld.Step(dt.asSeconds(), 8, 4);
    m_sceneGraph.update(dt);

    adaptPlayerPosition();
    adaptScrolling();
}

void World::adaptPlayerPosition()
{
    if(!(m_player->getPosition().x > m_worldBounds.left && m_player->getPosition().x < m_worldBounds.width + m_worldBounds.left && m_player->getPosition().y > m_worldBounds.top && m_player->getPosition().y < m_worldBounds.height + m_worldBounds.top))
    {
        m_player->setPosition(m_spawnPosition);
        m_player->resetForces();
    }
}

void World::adaptScrolling()
{
    sf::Vector2f myscroll(m_player->getPosition());
    if(myscroll.x < m_worldBounds.left/2)  myscroll.x = m_worldBounds.left/2;
    if(myscroll.x > (m_worldBounds.width + m_worldBounds.left)/2)  myscroll.x = (m_worldBounds.width + m_worldBounds.left)/2;
    if(myscroll.y < m_worldBounds.top/2)  myscroll.y = m_worldBounds.top/2;
    if(myscroll.y > (m_worldBounds.height + m_worldBounds.top)/2)  myscroll.y = (m_worldBounds.height + m_worldBounds.top)/2;
    m_worldView.setCenter(myscroll);
    m_minimapView.setCenter(myscroll);
}

void World::draw()
{
    m_window.setView(m_worldView);
    m_window.draw(m_sceneGraph);

    m_window.setView(m_minimapView);
    m_window.draw(m_sceneGraph);
}

CommandQueue& World::getCommandQueue()
{
    return m_commandQueue;
}