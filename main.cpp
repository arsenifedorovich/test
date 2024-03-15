#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <list>
#include <vector>

// sizes and coordinates(разделить потом)
const int16_t KScreenHeight = 700;
const int16_t KScreenWidth = 1000;

const int16_t KPaddleLength = 200;
const int16_t KPaddleHeight = 15;

const int16_t KPaddleStartX = KScreenWidth / 2 - KPaddleLength / 2;
const int16_t KPaddleStartY = KScreenHeight - KPaddleHeight - 40;

const int16_t KBallRadius = 5;
const int16_t KBallDiameter = KBallRadius * 2;

const int16_t KBallStartX = KPaddleStartX + KPaddleLength / 2 - KBallRadius;
const int16_t KBallStartY = KPaddleStartY - KBallDiameter;

const int16_t KBrickSize = 25;
// speeds

const float KBallSpeed = 0.5;
const float KPaddleSpeed = 1.0;

//класс игры в нем handle input update draw (handle input - нажати на клавиатуру)

//update - обновление движения
// состояние - меню начал конец
//factory - базовый класс обьект - от него шарик кирпич и фигня и фабрика будет каждый из них производить
// абстрактый класс обьект
//

// репозиторий лектора - там паттерны





struct Object {
  Object() = default;
  Object(float x, float y, int16_t w, int16_t h) : x(x), y(y), w(w), h(h) {}

  float x = 0;
  float y = 0;
  int16_t w = 0;
  int16_t h = 0;
};

void DrawBricks(const std::list<sf::RectangleShape>& bricks,
                sf::RenderWindow& win) {
  for (const auto& brick : bricks) {
    win.draw(brick);
  }
}

bool IsCollision(const Object& obj1, const Object& obj2) {
  return (obj1.x + obj1.w > obj2.x && obj1.x < obj2.x + obj2.w &&
          obj1.y < obj2.y + obj2.h && obj1.y + obj1.h > obj2.y);
}

int main() {
  sf::RenderWindow win(sf::VideoMode(KScreenWidth, KScreenHeight),
                       "Many Bricks");
  //надстройки(обьекты)

  Object ball_obj;
  Object paddle_obj;
  Object brick_obj;

  //блок paddle

  sf::Vector2f paddle_move;

  //задаем paddle, начальную позицию и цвет
  sf::RectangleShape paddle({KPaddleLength, KPaddleHeight});
  paddle.setPosition(KPaddleStartX, KPaddleStartY);
  paddle.setFillColor(sf::Color::Magenta);

  //блок ball

  sf::Vector2f ball_move(-KBallSpeed, KBallSpeed);

  //задаем ball, начальную позицию и цвет
  sf::CircleShape ball(KBallRadius);
  ball.setPosition(KBallStartX, KBallStartY);

  //задаем кирпичики (oddly)

  int16_t brickGap = 2;
  int16_t rows = 10;
  int16_t top_margin = 100;
  int16_t side_margin = 100;
  int16_t columns = (KScreenWidth - 2 * side_margin) / (KBrickSize + brickGap);

  // это должен быть вектор обьектов brick_obj. Вот этот кусок кода должна выполнять фабрика
  // желательно еще бы передавать в фабрику то, "по каким лекалам строить"
  std::vector<std::pair<float, float>> brick_coordinates;

  for (int16_t row = 0; row < rows; ++row) {
    for (int16_t col = 0; col < columns; ++col) {
      if (col % 2 == 1) {
        continue;
      }
      float x = side_margin + col * (KBrickSize + brickGap) + brickGap;
      float y = top_margin + row * (KBrickSize + brickGap) + brickGap;

      brick_coordinates.push_back({x, y});
    }
  }

  int16_t bricks_num = brick_coordinates.size();
  std::list<sf::RectangleShape> bricks;

  for (int16_t i = 0; i < bricks_num; ++i) {
    sf::RectangleShape brick({KBrickSize, KBrickSize});

    brick.setFillColor(sf::Color::Green);
    brick.setPosition(brick_coordinates[i].first, brick_coordinates[i].second);

    bricks.push_back(brick);
  }
  

  //игровой процесс
  while (win.isOpen()) {
    sf::Event event;

    while (win.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        win.close();
      }

      if (event.type == sf::Event::KeyPressed &&
          event.key.code == sf::Keyboard::Escape) {
        win.close();
      }

      switch (event.type) {
        case sf::Event::KeyPressed:
          if (event.key.code == sf::Keyboard::A ||
              event.key.code == sf::Keyboard::Left)
            paddle_move.x = -KPaddleSpeed;
          if (event.key.code == sf::Keyboard::D ||
              event.key.code == sf::Keyboard::Right)
            paddle_move.x = KPaddleSpeed;
          break;
        case sf::Event::KeyReleased:
          if (event.key.code == sf::Keyboard::A ||
              event.key.code == sf::Keyboard::Left)
            paddle_move.x = 0;
          if (event.key.code == sf::Keyboard::D ||
              event.key.code == sf::Keyboard::Right)
            paddle_move.x = 0;
        default:
          break;
      }
      // шарик летает в любом случае, вне зависимости от того что мы нажимаем
    }
    //идея - сначала сформировать обьект с которым сейчас работаем а потом
    //передавать его в функции все что связано  с шариком

    ball.move(ball_move);

    ball_obj = {ball.getPosition().x, ball.getPosition().y, KBallDiameter,
                KBallDiameter};

    if (ball_obj.x < 0) {
      ball_move.x *= -1;
    }
    if (ball_obj.y < 0) {
      ball_move.y *= -1;
    }
    if (ball_obj.x > KScreenWidth - KBallDiameter) {
      ball_move.x *= -1;
    }

    // если шар ниже платформы то можно завершать
    if (ball.getPosition().y > paddle.getPosition().y) {
      win.close();
    }

    if (IsCollision(ball_obj, paddle_obj)) {
      ball_move.y *= -1;
      ball.setPosition(
          {ball.getPosition().x, paddle.getPosition().y - KBallDiameter});
    }
    //все что связано с шариком и блоками

    for (auto it = bricks.begin(); it != bricks.end(); ++it) {
      auto brick = *it;
      brick_obj = {brick.getPosition().x, brick.getPosition().y, KBrickSize,
                   KBrickSize};

      if (IsCollision(ball_obj, brick_obj)) {
        //заглушка: временно сделаем brick красным
        //          brick.setFillColor(sf::Color::Red);

        bricks.erase(it);
        if (ball_obj.y + ball_obj.h - KBallSpeed <= brick_obj.y ||
            brick_obj.y >= brick_obj.y + brick_obj.h - KBallSpeed) {
          ball_move.y *= -1;
        } else {
          ball_move.x *= -1;
        }
        break;
      }
    }

    //все что связано  с paddle
    // move the paddle based on velocity
    paddle.move(paddle_move);

    paddle_obj = {paddle.getPosition().x, paddle.getPosition().y, KPaddleLength,
                  KPaddleHeight};

    // keep the paddle within the window bounds
    if (paddle_obj.x > KScreenWidth - KPaddleLength) {
      paddle.setPosition(KScreenWidth - KPaddleLength, paddle_obj.y
    }
    if (paddle_obj.x < 0) {
      paddle.setPosition(0, paddle_obj.y);
    }

    // Clear screen
    win.clear();

    // draw the paddle
    win.draw(paddle);
    // draw the ball
    win.draw(ball);

    DrawBricks(bricks, win);

    win.display();
  }

  return EXIT_SUCCESS;
}

