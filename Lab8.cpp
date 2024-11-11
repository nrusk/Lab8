#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;
using namespace sfp;

const float KB_SPEED = 0.2;

void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}

void MoveCrossbow(PhysicsSprite& crossbow, int elapsedMS) {
    if (Keyboard::isKeyPressed(Keyboard::Right)) {
        Vector2f newPos(crossbow.getCenter());
        newPos.x = newPos.x + (KB_SPEED * elapsedMS);
        crossbow.setCenter(newPos);
    }
    if (Keyboard::isKeyPressed(Keyboard::Left)) {
        Vector2f newPos(crossbow.getCenter());
        newPos.x = newPos.x - (KB_SPEED * elapsedMS);
        crossbow.setCenter(newPos);
    }
}

Vector2f GetTextSize(Text text) {
    FloatRect r = text.getGlobalBounds();
    return Vector2f(r.width, r.height);
}

int main()
{
    RenderWindow window(VideoMode(800, 600), "Pterodactyl Hunter");
    World world(Vector2f(0, 0));
    int score(0);
    int arrows(5);

    SoundBuffer popBuffer;
    if (!popBuffer.loadFromFile("assets/gotOne.ogg")) {
        cout << "could not load gotOne.ogg" << endl;
        exit(5);
    }
    Sound popSound;
    popSound.setBuffer(popBuffer);

    Music music;
    if (!music.openFromFile("assets/jurassicflute.ogg")) {
        cout << "Failed to load jurassicflute.ogg ";
        exit(6);
    }
    music.play();

    PhysicsSprite& crossBow = *new PhysicsSprite();
    Texture cbowTex;
    LoadTex(cbowTex, "assets/crossbow.png");
    crossBow.setTexture(cbowTex);
    Vector2f sz = crossBow.getSize();
    crossBow.setCenter(Vector2f(400, 600 - (sz.y / 2)));

    PhysicsSprite arrow;
    Texture arrowTex;
    LoadTex(arrowTex, "assets/arrow.png");
    arrow.setTexture(arrowTex);
    bool drawingArrow(false);

    PhysicsRectangle top;
    top.setSize(Vector2f(800, 10));
    top.setCenter(Vector2f(400, 5));
    top.setStatic(true);
    world.AddPhysicsBody(top);

    /*PhysicsRectangle left;
    left.setSize(Vector2f(10, 600));
    left.setCenter(Vector2f(5, 300));
    left.setStatic(true);
    world.AddPhysicsBody(left);*/

    PhysicsRectangle right;
    right.setSize(Vector2f(10, 600));
    right.setCenter(Vector2f(795, 300));
    right.setStatic(true);
    world.AddPhysicsBody(right);

    Texture redTex;
    LoadTex(redTex, "assets/pterodactyl.png");
    PhysicsShapeList<PhysicsSprite> pterodactyls;


        


    top.onCollision = [&drawingArrow, &world, &arrow]
    (PhysicsBodyCollisionResult result) {
        drawingArrow = false;
        world.RemovePhysicsBody(arrow);
        };

    Font fnt;
    if (!fnt.loadFromFile("assets/arial.ttf")) {
        cout << "Could not load font." << endl;
        exit(3);
    }
    Clock clock;
    Time lastTime(clock.getElapsedTime());
    Time currentTime(lastTime);

    long pterodactylMS(0);
    while ((arrows > 0) || drawingArrow) {
        currentTime = clock.getElapsedTime();
        Time deltaTime = currentTime - lastTime;
        long deltaMS = deltaTime.asMilliseconds();
        if (deltaMS > 9) {
            pterodactylMS = pterodactylMS + deltaMS;
            lastTime = currentTime;
            world.UpdatePhysics(deltaMS);
            MoveCrossbow(crossBow, deltaMS);

            

            if (Keyboard::isKeyPressed(Keyboard::Space) && !drawingArrow) {
                drawingArrow = true;
                arrows = arrows - 1;
                arrow.setCenter(crossBow.getCenter());
                arrow.setVelocity(Vector2f(0, -1));
                world.AddPhysicsBody(arrow);

            }

            

            window.clear();
            if (drawingArrow) {
                window.draw(arrow);
            }
            pterodactyls.DoRemovals();
            for (PhysicsShape& pterodactyl : pterodactyls) {
                window.draw((PhysicsSprite&)pterodactyl);
            }
            window.draw(crossBow);
            Text scoreText;
            scoreText.setString(to_string(score));
            scoreText.setFont(fnt);
            scoreText.setPosition(Vector2f(790 - GetTextSize(scoreText).x, 585 - (GetTextSize(scoreText).y)));
            window.draw(scoreText);
            Text arrowCountText;
            arrowCountText.setString(to_string(arrows));
            arrowCountText.setFont(fnt);
            arrowCountText.setPosition(Vector2f(20 - GetTextSize(arrowCountText).x, 585 - (GetTextSize(arrowCountText).y)));
            window.draw(arrowCountText);
            //world.VisualizeAllBounds(window);

            window.display();

        }

        if (pterodactylMS > 1000) {
            pterodactylMS = 0;
            PhysicsSprite& pterodactyl = pterodactyls.Create();
            pterodactyl.setTexture(redTex);
            int x = -50;
            pterodactyl.setCenter(Vector2f(x, 100));
            pterodactyl.setVelocity(Vector2f(0.25, 0));
            world.AddPhysicsBody(pterodactyl);

            

            pterodactyl.onCollision = [&drawingArrow, &world, &arrow, &pterodactyl, &pterodactyls, &score, &popSound, &right](PhysicsBodyCollisionResult result) {
                if (result.object2 == arrow) {
                    popSound.play();
                    drawingArrow = false;
                    world.RemovePhysicsBody(arrow);
                    world.RemovePhysicsBody(pterodactyl);
                    pterodactyls.QueueRemove(pterodactyl);
                    score += 10;
                }
                if (result.object2 == right) {
                    world.RemovePhysicsBody(pterodactyl);
                    pterodactyls.QueueRemove(pterodactyl);
                }
                };
            
        }
        
    }
    window.display(); // this is needed to see the last frame
    Text gameOverText;
    gameOverText.setString("GAME OVER");
    gameOverText.setFont(fnt);
    sz = GetTextSize(gameOverText);
    gameOverText.setPosition(400 - (sz.x / 2), 300 - (sz.y / 2));
    window.draw(gameOverText);
    window.display();
    while (true);

}