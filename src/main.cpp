#include <iostream>
#include <dgen/App.h>
#include <cmath>
#include <dgen/dgen.h>

#define MARGIN 500
#define NPARTICLES 250

static float randf() {
    return (std::rand() % 1000000) / 1000000.0f;
}
static float randf(float min, float max) {
    return min + randf() * (max - min);
}

static float clampf(float v, float min, float max) {
    return v < min ? min : (v > max ? max : v);
}

class Particle {
public:
    sf::Vector2f pos;
    float radius;
    sf::Vector2f v;
    Particle(sf::Vector2f pos) : pos(pos) {
        v = {0, -30};
        radius = randf(2.0, 4.0);
    }
    Particle() : Particle({0, 0}) {}
    void draw(sf::RenderWindow &w) {
        sf::CircleShape circle;
        circle.setPosition(pos);
        circle.setRadius(radius);
        circle.setFillColor(sf::Color::Black);
        circle.setOrigin(radius, radius);
        w.draw(circle);
    }
    void update(App *app) {
        float delta = app->getDelta().asSeconds();
        pos += v * delta;
        v.x += randf(-50, 50) * delta;
        v.x = clampf(v.x, -50, 50);
        v.y += randf(-30, -10) * delta;
        v.y = clampf(v.y, -30, -10);
    }
    static Particle rand(sf::RenderWindow &w) {
        float x = randf(-MARGIN, w.getSize().x + MARGIN);
        float y = randf(-MARGIN, w.getSize().y + MARGIN);
        return {{x, y}};
    }
};

class MyApp : public App {
    sf::Font font;
    sf::Text fpsText;
    sf::Clock fpsClock;
    int fps = 0;
    std::vector<sf::Vector2f> points;
    std::vector<sf::Vector3i> tris;
    std::vector<Particle> particles;
    sf::Vector3f colorBottom;
    sf::Vector3f colorTop;
    bool fullscreen = false;
    bool drawPoints = true;
    bool drawFps = true;
    bool drawCursor = true;
    void load() override {
        font.loadFromFile("res/RobotoMono-Regular.ttf");
        fpsText.setFont(font);
        fpsText.setFillColor(sf::Color::Yellow);
        fpsText.setCharacterSize(24);
        fpsText.setPosition(16, 16);
        fpsText.setOutlineColor(sf::Color::Black);
        fpsText.setOutlineThickness(1);
        clear();
    }
    void update() override {
        App::update();
        fps++;
        if (fpsClock.getElapsedTime().asSeconds() >= 1.0) {
            fpsText.setString("FPS: " + std::to_string(fps));
            fpsClock.restart();
            fps = 0;
        }
        for (auto &p : particles)
            p.update(this);
        auto sz = getWindow().getSize();
        auto it = std::remove_if(particles.begin(), particles.end(),
        [sz](Particle &p) {
            return p.pos.x < -MARGIN || p.pos.y < -MARGIN
                    || p.pos.x > sz.x + MARGIN || p.pos.y > sz.y + MARGIN;
        });
        int gen = std::distance(it, particles.end());
        particles.erase(it, particles.end());
        while (gen--) {
            Particle p = Particle::rand(getWindow());
            particles.push_back(p);
        }
    }
    void clear() {
        particles.clear();
        for (int i = 0; i < NPARTICLES; i++) {
            Particle p = Particle::rand(getWindow());
            particles.push_back(p);
        }
        colorTop = {randf(0.7, 1.0), randf(0.7, 1.0), randf(0.7, 1.0)};
        colorBottom = {randf(0.0, 0.3), randf(0.0, 0.3), randf(0.0, 0.3)};
    }
    Tri getGTri(int ti) {
        auto &tri = tris[ti];
        return {points[tri.x], points[tri.y], points[tri.z]};
    }
    void addEdge(std::vector<sf::Vector2i> &edges, int a, int b) {
        sf::Vector2i v = {std::min(a, b), std::max(a, b)};
        auto it = std::find(edges.begin(), edges.end(), v);
        if (it != edges.end()) {
            edges.erase(it);
        }
        else {
            edges.push_back(v);
        }
    }
    void insert(int pi) {
        std::vector<sf::Vector2i> edges;
        std::vector<int> bad;
        auto p = points[pi];
        for (int ti = 0; ti < (int)tris.size(); ti++) {
            auto tri = tris[ti];
            Tri gtri = getGTri(ti);
            if (gtri.contains(p)) {
                addEdge(edges, tri.x, tri.y);
                addEdge(edges, tri.y, tri.z);
                addEdge(edges, tri.z, tri.x);
                bad.push_back(ti);
            }
        }
        std::sort(bad.begin(), bad.end(), std::greater<int>());
        for (int i : bad) {
            tris.erase(tris.begin() + i);
        }
        for (auto e : edges) {
            tris.push_back({e.x, e.y, pi});
        }
    }
    void triangulate() {
        tris.clear();
        tris.push_back({0, 1, 2});
        for (int i = 3; i < (int)points.size(); i++) {
            insert(i);
        }
    }
    void processInput(const sf::Event &e) override  {
        App::processInput(e);
        if (e.type == sf::Event::Resized) {
            clear();
        }
        else if (e.type == sf::Event::KeyReleased) {
            if (e.key.code == sf::Keyboard::Space) {
                clear();
            }
            else if (e.key.code == sf::Keyboard::F) {
                sf::ContextSettings settings;
                settings.antialiasingLevel = 8;
                if (fullscreen) {
                    getWindow().create(sf::VideoMode(1280, 720), "Delauney",
                            sf::Style::Default, settings);
                    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
                    getWindow().setPosition(sf::Vector2i(
                        (desktop.width - getWindow().getSize().x) / 2,
                        (desktop.height - getWindow().getSize().y) / 2));
                }
                else {
                    getWindow().create(sf::VideoMode::getFullscreenModes()[0],
                            "Delauney",
                            sf::Style::Fullscreen, settings);
                }
                fullscreen = !fullscreen;
            }
            else if (e.key.code == sf::Keyboard::P) {
                drawPoints = !drawPoints;
            }
            else if (e.key.code == sf::Keyboard::O) {
                drawFps = !drawFps;
            }
            else if (e.key.code == sf::Keyboard::M) {
                drawCursor = !drawCursor;
                getWindow().setMouseCursorVisible(drawCursor);
            }
        }
        else if (e.type == sf::Event::MouseButtonPressed) {
            if (e.mouseButton.button == sf::Mouse::Left) {
                particles.push_back({{
                        (float)e.mouseButton.x,
                        (float)e.mouseButton.y}});
                // triangulate();
            }
        }
    }
    sf::Color getColor(float height) {
        sf::Vector3f c = colorBottom;
        c += (colorTop - colorBottom) * height;
        return sf::Color(255 * c.x, 255 * c.y, 255 * c.z);
    }
    void draw() override {
        App::draw();
        sf::RenderWindow &w = getWindow();
        w.clear(sf::Color(100, 100, 100));
        points.clear();
        points.reserve(particles.size() + 3);
        points.push_back({-10000, 10000});
        points.push_back({0, -10000});
        points.push_back({10000, 10000});
        for (auto p : particles)
            points.push_back(p.pos);
        triangulate();
        for (auto t : tris) {
            Tri tri = {points[t.x], points[t.y], points[t.z]};
            float ypos = clampf(tri.ycenter(), 0.0, w.getSize().y);
            float height = 1.0 - (ypos / w.getSize().y);
            sf::Color color = getColor(height);
            sf::VertexArray triangle(sf::Triangles, 3);
            triangle[0] = sf::Vertex(points[t.x], color);
            triangle[1] = sf::Vertex(points[t.y], color);
            triangle[2] = sf::Vertex(points[t.z], color);
            w.draw(triangle);
        }
        if (drawPoints) {
            for (auto &p : particles) {
                p.draw(w);
            }
        }
        if (drawFps)
            w.draw(fpsText);
    }
};

int main(int argc, char **argv) {
    srand(time(0));
    MyApp app;
    app.run();
    return 0;
}
