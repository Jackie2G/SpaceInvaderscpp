#include <iostream>
#include <SFML\Window.hpp>
#include <SFML\System.hpp>
#include <SFML\Graphics.hpp>
#include <math.h>
#include <cstdlib>
#include <vector>
#include <time.h>

using namespace sf;

//tworzymy klase naboje, ustalamy teksture oraz pozycje
class Bullet
{
public:
	Sprite shape;

	Bullet(Texture *texture, Vector2f position)
	{
		this->shape.setTexture(*texture);
		this->shape.setScale(0.1f, 0.1f);
		this->shape.setPosition(position);
	}

	//Bullet(){}
};

//tworzymy klase gracza, tworzymy zmienne odpowiadajace za HP, teksture i ksztalt
class Player 
{
public:
	Sprite shape;
	Texture *texture;
	int HP;
	int score;
	std::vector<Bullet> bullets;

	Player(Texture *texture)
	{
		this->HP = 3;
		this->score = 0;
		this->texture = texture;
		this->shape.setTexture(*texture);

		this->shape.setScale(0.5f, 0.5f);
		this->shape.setPosition(380, 567);
	}
};

//tworzymy klase przeciwnika
class Invader 
{
public:
	Sprite shape;

	Invader(Texture *texture, Vector2u position)
	{
		this->shape.setTexture(*texture);

		this->shape.setScale(0.17f, 0.17f);
		this->shape.setPosition(rand()% (int)(this->shape.getGlobalBounds().width - position.x), this->shape.getGlobalBounds().height - 100.f);
	}
};

class GameOver
{
public:
	Sprite shape;

	GameOver(Texture *texture)
	{
		this->shape.setTexture(*texture);

		this->shape.setScale(0.20f, 0.20f);
		this->shape.setPosition(200.f, 150.f);
	}
};


int main()
{
	srand(time(NULL));
	//tworzymy okno o zadanych wymiarach
	RenderWindow window(VideoMode(800, 600), "Space Invaders");

	//ustawiamy limit klatek na 60
	window.setFramerateLimit(60);

	//dodajemy czcionke
	Font font;
	font.loadFromFile("Fonts/Roboto-Medium.ttf");

	//dodajemy teksture gracza
	Texture player_texture;
	player_texture.loadFromFile("Sprites/shipp.png");

	//dodajemy teksture przeciwnika
	Texture invader_texture;
	invader_texture.loadFromFile("Sprites/invaderr.png");

	//dodajemy teskture pocisku
	Texture bullet_texture;
	bullet_texture.loadFromFile("Sprites/bullet.png");

	//dodajemy teksture konca gry
	Texture end_texture;
	end_texture.loadFromFile("Sprites/gameover.png");

	//inicjalizacja gracza
	Player player(&player_texture);
	int timer_shooting = 20;

	//inizjalizacja przeciwnikow
	std::vector<Invader> invaders;
	int invaders_spawner = 0;

	//inicjalizacja tekstu wyswietlajacego hp
	Text hp_text;
	hp_text.setFont(font);
	hp_text.setCharacterSize(16);
	hp_text.setFillColor(Color::White);

	//inizjalizacja tekstu wyswietlajacego wynik
	Text score_text;
	score_text.setFont(font);
	score_text.setCharacterSize(16);
	score_text.setFillColor(Color::White);

	//inizjalizacja tekstu informujacego o uzyskanych punktach na koniec gry
	Text end_text;
	end_text.setFont(font);
	end_text.setCharacterSize(32);
	end_text.setFillColor(Color::Red);


	//inicjalizacja informacji o koncu gry
	GameOver game_over(&end_texture);

	//standardowa petla czyli serce naszej gry
	while (window.isOpen())
	{

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}

		/*aktualizowanie tego co dzieje sie na ekranie, jesli zdrowie gracza bedzie rowne 0 gra sie zakonczy*/

		if (player.HP >= 0)
		{
			//poruszanie statkiem za pomoca strzalek w prawo i lewo
			if (Keyboard::isKeyPressed(Keyboard::Right))
				player.shape.move(10.f, 0.f);
			if (Keyboard::isKeyPressed(Keyboard::Left))
				player.shape.move(-10.f, 0.f);

			//wyswietlanie ilosci zyc gracza
			hp_text.setPosition(720.f, 20.f);
			hp_text.setString("Lives: " + std::to_string(player.HP));

			//wyswietlanie liczby punktow uzyskanych przez gracza
			score_text.setPosition(30.f, 20.f);
			score_text.setString("Score: " + std::to_string(player.score));

			//liczba punktow ktora zostanie wyswietona na koncu gry czyli wynik ktory uzyskalismy
			end_text.setPosition(335.f, 380.f);
			end_text.setString("Score: " + std::to_string(player.score));

			//kolizja z oknem aby statkiem nie mogl wyjsc poza ekran
			if (player.shape.getPosition().x <= 0)
				player.shape.setPosition(0.f, player.shape.getPosition().y);
			else if (player.shape.getPosition().x >= window.getSize().x - 45.f)
				player.shape.setPosition(window.getSize().x - player.shape.getGlobalBounds().width, player.shape.getPosition().y);

			if (timer_shooting < 20)
				timer_shooting++;

			//strzelanie
			if (Keyboard::isKeyPressed(Keyboard::Space) && timer_shooting >= 20)
			{
				player.bullets.push_back(Bullet(&bullet_texture, player.shape.getPosition() + Vector2f(20.f, 0.f)));
				timer_shooting = 0;
			}


			//niszczenie naboi jezeli znajduja sie poza ekranem oraz ich poruszanie, wykrywanie kolizji z przeciwnikiem
			for (size_t i = 0; i < player.bullets.size(); i++)
			{
				player.bullets[i].shape.move(0.f, -20.f);

				if (player.bullets[i].shape.getPosition().y > window.getSize().y)
					player.bullets.erase(player.bullets.begin() + i);

				for (size_t j = 0; j < invaders.size(); j++)
				{
					if (player.bullets[i].shape.getGlobalBounds().intersects(invaders[j].shape.getGlobalBounds()))
					{
						invaders.erase(invaders.begin() + j);
						player.bullets.erase(player.bullets.begin() + i);
						player.score++;
						break;
					}
				}
			}

			//pojawianie sie przeciwnikow, ich poruszanie oraz usuwanie jesli sa poza ekranem
			if (invaders_spawner < 20)
				invaders_spawner++;

			if (invaders_spawner >= 20)
			{
				invaders.push_back(Invader(&invader_texture, window.getSize()));
				invaders_spawner = 0;
			}

			for (size_t i = 0; i < invaders.size(); i++)
			{
				invaders[i].shape.move(0.f, 2.f);

				if (invaders[i].shape.getPosition().y >= window.getSize().y)
					invaders.erase(invaders.begin() + i);

				if (invaders[i].shape.getGlobalBounds().intersects(player.shape.getGlobalBounds()))
				{
					invaders.erase(invaders.begin() + i);
					player.HP--;
				}
			}
		}
		window.clear();

		window.draw(player.shape);

		//tworzenie nowych naboi
		for (size_t i = 0; i < player.bullets.size(); i++)
		{
			window.draw(player.bullets[i].shape);
		}

		//tworzenie wrogow
		for (size_t i = 0; i < invaders.size(); i++)
		{
			window.draw(invaders[i].shape);
		}

		//wyswietlanie tekstu na ekranie
		window.draw(score_text);
		
		window.draw(hp_text);

		//if sprawdzajacy czy gra sie zakonczyla
		if (player.HP <= 0)
		{
			window.draw(game_over.shape);
			end_text.setPosition(335.f, 380.f);
			//score_text.setCharacterSize(32);
			//score_text.setFillColor(Color::Red);
			//score_text.setString("Score: " + std::to_string(player.score));
			end_text.setString("Score: " + std::to_string(player.score));
			window.draw(end_text);
		}

		window.display();
	}

	return 0;
}

