#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

using namespace std;

sf::RenderWindow* window;

float delta;

bool gameRunning = true;
bool gameOvered = false;



// Klasa glowna menu

class Menu {
private:
	sf::Texture* menuTexture;
public:
	Menu(float width, float height);

	void draw(sf::RenderWindow& window);
	void MoveUp();
	void MoveDown();

	Menu() {
		menuTexture = new sf::Texture();

		menuTexture->loadFromFile("res/bg/tlo1.png");

	}
};
 

//Klasa postaci
sf::Texture* backgroundTexture;
sf::Texture* texture;
class Bird{
private:
	sf::Texture* texture;
	float y;
	float vel;
public:
	Bird() {
		texture = new sf::Texture();

		texture->loadFromFile("res/char/bird/b1.png");
		y = 400;
		vel = 0;
	}

	sf::FloatRect getRect() {
		auto size = texture->getSize();
		return {
			50, y,
			(float)size.x, (float)size.y
		};
	}

	void flap() {
		vel = -400;
	}
	void draw() {
		sf::Sprite birdSprite(*texture);
		birdSprite.setPosition(50, y);
		window->draw(birdSprite);
	}
	void update() {
		vel += delta * 900;
		y += vel * delta;
	}

	void endgame() {
		if (y + texture->getSize().y > backgroundTexture->getSize().y) {
			y = (float)backgroundTexture->getSize().y + texture->getSize().y;
			vel = 0;
			gameOvered = true;

		}
	}

	~Bird() {
		delete texture;
	}
};

Bird* bird;

// Funkcje kluczowe
sf::Texture* groundTexture;

sf::Texture* upperPipe;
sf::Texture* lowerPipe;

class Pipe {
private:
	float x;
	float y;
	bool scored;

public:
	Pipe() {
		x = (float)(window->getSize().x + upperPipe->getSize().x);
		y = 100.0f + (float)(rand() % 5 - 3) * 50;
		scored = false;
	}

	sf::FloatRect getUpperRect() const {
		auto size = upperPipe->getSize();
		return {
				x, y + 340,
				(float)size.x, (float)size.y
		};
	}

	sf::FloatRect getLowerRect() const {
		auto size = upperPipe->getSize();
		return {
				x, y - 340,
				(float)size.x, (float)size.y
		};
	}

	void draw() const {
		sf::Sprite upperSprite(*upperPipe);
		upperSprite.setPosition(x, y + 340);
		sf::Sprite lowerSprite(*lowerPipe);
		lowerSprite.setPosition(x, y - 340);

		window->draw(upperSprite);
		window->draw(lowerSprite);
	}

	void update() {

		x -= 100 * delta;

		x -= 100 * delta;
	}

	void endgame() {
		auto birdRect = bird->getRect();
		if (birdRect.intersects(getUpperRect()) or birdRect.intersects(getLowerRect())) {
			gameOvered = true;
			cout << "Niestety przegrałeś, graj dalej";
		}
	}
};
Pipe* rura;

vector<Pipe*> pipes;
sf::Clock* pipeGeneratingClock;


void setup() {

	srand((unsigned int)time(nullptr));
	pipeGeneratingClock = new sf::Clock;

	window = new sf::RenderWindow(sf::VideoMode(400, 700), "Flappy Bird Politechnika Swietokrzyska");
	window->setPosition({ 1400, 20 }); 

	bird = new Bird();
	
	backgroundTexture = new sf::Texture();

	backgroundTexture->loadFromFile("res/bg/tlo1.png");
	
	groundTexture = new sf::Texture();
	groundTexture->loadFromFile("res/podloga.png");

	upperPipe = new sf::Texture();
	lowerPipe = new sf::Texture();

	sf::Image pipeImage;
	pipeImage.loadFromFile("res/rura.png");

	upperPipe->loadFromImage(pipeImage);
	pipeImage.flipVertically();
	lowerPipe->loadFromImage(pipeImage);

	pipes.push_back(new Pipe());
}
void destroy() {
	delete window;
	bird->draw();
	delete backgroundTexture;
	delete pipeGeneratingClock;

}



void update() {
	bird->update();
	bird->endgame();
	for (const auto& pipe : pipes) {
		pipe->update();
	}
	if (pipeGeneratingClock->getElapsedTime().asSeconds() > 3.5) {
		pipeGeneratingClock->restart();
		pipes.push_back(new Pipe());
	}
	
}
void handleEvent(sf::Event& event) {

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
		bird->flap();
	}

	if (event.type == sf::Event::Closed) {
		window->close();
	}

}

float groundOffset;

void draw() {
	window->clear();
	window->draw(sf::Sprite(*backgroundTexture));
	
	for (const auto& pipe : pipes) {
		pipe->draw();
	}


	sf::Sprite groundSprite(*groundTexture);
	groundOffset -= delta * 100;
	if (groundOffset <= -24) {
		groundOffset += 24;
	}
	groundSprite.setPosition(groundOffset, backgroundTexture->getSize().y);
	window->draw(groundSprite);

	sf::RectangleShape lowerRectangle({
		  (float)window->getSize().x,
		  (float)window->getSize().y - backgroundTexture->getSize().y - groundTexture->getSize().y
		});
	lowerRectangle.setPosition(0, (float)backgroundTexture->getSize().y + groundTexture->getSize().y);
	lowerRectangle.setFillColor({ 245, 228, 138 });
	window->draw(lowerRectangle);
	bird->draw();
}

int main(){
	setup();

	sf::Clock deltaClock;
	while (window->isOpen() && gameOvered == false) {
		sf::Event event{};
		while (window->pollEvent (event)) {
			handleEvent(event);
		}
		delta = deltaClock.getElapsedTime().asMicroseconds() / 1000.0f / 1000.0f;
		deltaClock.restart();
		update();
		draw();

		window->display();
	}
	destroy();

}